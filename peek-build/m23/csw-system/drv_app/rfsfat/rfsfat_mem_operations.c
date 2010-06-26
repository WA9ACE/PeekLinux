/**
 * @file	rfsfat_mem_operations.c
 *
 * Memory and string functions
 *
 * @author	Anton van Breemen (abreemen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/29/2004	Anton van Breemen (abreemen@ict.nl)		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

#include "rfsfat/rfsfat_i.h"
#include "rfsfat_ffs_disk.h"
#include "rfsfat/rfsfat_mem_operations.h"
#include "rfs/rfs_api.h"

#define RFSFAT_MEM_OPT 			1

#if (RFSFAT_MEM_OPT ==1)
#include <string.h>
#endif

/******Copyright by ICT Embedded BV All rights reserved **********************/
void *rfsfat_vpMemmove
  (void *vpDestMem, RW_PTR_TO_RO (void) vpSourceMem, UINT16 uiCount)
/* PURPOSE      : Copies a source memory buffer to a destination memory buffer.
 * ARGUMENTS    : vpDestMem,
 *                pointer (not a null pointer) to destination memory buffer.
 *                vpSourceMem,
 *                pointer (not a null pointer) to source memory buffer.
 *                uiCount,
 *                number of bytes to copy.
 * RETURNS      : A pointer to the destination buffer.
 * PRECONDITION : 
 * POSTCONDITION: 
 * ERROR AND EXCEPTION HANDLING: 
 * NOTE: This routine ensures that, if buffers overlap, the original source 
 *       bytes in the overlapping region are copied before being overwritten.
 *       This in contrast with the MEMCPY function.
 *****************************************************************************/
{
  /* Make a copy of the given destination pointer so that it can be returned. */
  void *vpCopyDestMem = vpDestMem;


  /* Check input parameters.                                                 */
  RFSFAT_ASSERT_LEAVE (((vpDestMem != NULL) && (vpSourceMem != NULL)),
					   return (NULL);
	);

  if ((vpDestMem <= vpSourceMem) ||
	  ((UINT8 *) vpDestMem >= ((UINT8 *) vpSourceMem + uiCount)))
	{							/* 
								   Non-overlapping memory: copy from lower addresses to higher addresses. 
								   This part is the same as the MEMCPY.
								 */
	  while (uiCount--)
		{
		  /* Copy the content of the source to the destination.                  */
		  *(UINT8 *) vpDestMem = *(UINT8 *) vpSourceMem;
		  /* Get the next memory position of both the destination and the source. */
		  vpDestMem = (UINT8 *) vpDestMem + 1;
		  vpSourceMem = (UINT8 *) vpSourceMem + 1;
		}
	}
  else
	{							/* 
								   Overlapping buffers: copy from higher addresses to lower addresses.    
								   Move the buffer pointers to the last position to be copied.
								 */
	  vpDestMem = (UINT8 *) vpDestMem + uiCount - 1;
	  vpSourceMem = (UINT8 *) vpSourceMem + uiCount - 1;
	  while (uiCount--)
		{
		  /* Copy the content of the source to the destination.                  */
		  *(UINT8 *) vpDestMem = *(UINT8 *) vpSourceMem;
		  /* 
		     Get the previous memory position of both the destination and the source.
		   */
		  vpDestMem = (UINT8 *) vpDestMem - 1;
		  vpSourceMem = (UINT8 *) vpSourceMem - 1;
		}
	}

  return vpCopyDestMem;
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
void *rfsfat_vpMemcpy
  (void *  vpDestMem, RW_PTR_TO_RO (void) vpSourceMem, UINT16 uiCount)
/* PURPOSE      : Copies a source memory buffer to a destination memory buffer.
 * ARGUMENTS    : vpDestMem,
 *                pointer (not a null pointer) to destination memory buffer.
 *                vpSourceMem,
 *                pointer (not a null pointer) to source memory buffer.
 *                uiCount,
 *                number of bytes to copy.
 * RETURNS      : A pointer to the destination buffer.
 * PRECONDITION : 
 * POSTCONDITION: 
 * ERROR AND EXCEPTION HANDLING: 
 * NOTE: This routine does NOT recognize overlapping buffers, and thus can lead
 *       to propagation.
 *****************************************************************************/
{

#if (RFSFAT_MEM_OPT ==1)
  /* Make a copy of the given destination pointer so that it can be returned. */
  void *vpCopyDestMem = vpDestMem;
#endif


  /* Check input parameters.                                                 */
  RFSFAT_ASSERT_LEAVE (((vpDestMem != NULL) && (vpSourceMem != NULL)),
					   return (NULL);
	);


#if (RFSFAT_MEM_OPT ==1)
       return memcpy(vpDestMem,vpSourceMem,uiCount);
#else

  /* Copy uiCount bytes from source to destination memory buffer.            */
  while (uiCount--)
	{
	  /* Copy the content of the source to the destination.                    */
	  *(UINT8 *) vpCopyDestMem = *(UINT8 *) vpSourceMem;
	  /* Get the next memory position of both the destination and the source.  */
	  vpCopyDestMem = (UINT8 *) vpCopyDestMem + 1;
	  vpSourceMem = (UINT8 *) vpSourceMem + 1;
	}
	//to prevent the callers pointer to be messed up
  return vpDestMem;

#endif	
}


/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT16 rfsfat_iMemcmp
  (RW_PTR_TO_RO (void) vpMemBuf1,
   RW_PTR_TO_RO (void) vpMemBuf2, UINT16 uiCount)
/* PURPOSE      : Compare characters in two buffers
 * ARGUMENTS    : vpMemBuf1,
 *                pointer (not a null pointer) to the first memory buffer
 *                vpMemBuf2,
 *                pointer (not a null pointer) to the second memory buffer
 *                uiCount,
 *                number of bytes to compare.
 * RETURNS      : The return value indicates the relationship between the 
 *                buffers.
 *                <0: vpMemBuf1 less than vpMemBuf2 
 *                0 : vpMemBuf1 identical to vpMemBuf2 
 *                >0: vpMemBuf1 greater than vpMemBuf2 
 * PRECONDITION : 
 * POSTCONDITION: 
 * ERROR AND EXCEPTION HANDLING: 
 * NOTE: 
 *****************************************************************************/
{
  UINT8 ucContentMemBuf1 = 0;
  UINT8 ucContentMemBuf2 = 0;

  const UINT8 *temp_buf1_p;
  const UINT8 *temp_buf2_p;

  /* Check input parameters.                                                 */
  RFSFAT_ASSERT_LEAVE (((vpMemBuf1 != NULL) && (vpMemBuf2 != NULL)), return 0;
	);

  /* If nothing has to be compared, return 0                                 */
  if (uiCount == 0)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: Nothing to compare ",
						 RV_TRACE_LEVEL_WARNING);
	  return 0;
	}

  temp_buf1_p = (RW_PTR_TO_RO (UINT8)) vpMemBuf1;
  temp_buf2_p = (RW_PTR_TO_RO (UINT8)) vpMemBuf2;
  /* 
     Compare byte for byte. Until the number of bytes to compare has been 
     reached, or until 2 bytes are different.
   */
  do
	{
	  ucContentMemBuf1 = *(temp_buf1_p)++;
	  ucContentMemBuf2 = *(temp_buf2_p)++;
	  if (ucContentMemBuf1 != ucContentMemBuf2)
		{
		  break;
		}
	}
  while (--uiCount > 0);

  /* Return the difference between the last compared bytes.                  */
  return (ucContentMemBuf1 - ucContentMemBuf2);
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
void *
rfsfat_vpMemset (void *  vpDestMem, UINT16 uiMemsetValue, UINT16 uiCount)
/* PURPOSE      : Set a number of bytes in the destination memory to a specific
 *                value.
 * ARGUMENTS    : vpDestMem,
 *                pointer (not a null pointer) to destination memory buffer.
 *                uiMemsetValue,
 *                value to put in destination bytes.
 *                uiCount,
 *                number of bytes to set.
 * RETURNS      : A pointer to the destination buffer.
 * PRECONDITION : 
 * POSTCONDITION: 
 * ERROR AND EXCEPTION HANDLING: 
 * NOTE: 
 *****************************************************************************/
{

#if (RFSFAT_MEM_OPT ==1)
  /* Make a copy of the given destination pointer so that it can be returned. */
  void *vpCopyDestMem = vpDestMem;
#endif

  /* Check input parameters.                                                 */
  RFSFAT_ASSERT_LEAVE ((vpDestMem != NULL), return (NULL););


#if (RFSFAT_MEM_OPT ==1)
             return memset(vpDestMem,uiMemsetValue,uiCount);
       
#else
  while (uiCount--)
	{							/* Copy the value of uiMemsetValue to the destination memory buffer.     */
	  *(UINT8 *) vpCopyDestMem = (UINT8) uiMemsetValue;
	  /* Get the next memory position of the destination.                      */
	  vpCopyDestMem = (UINT8 *) vpCopyDestMem + 1;
	}
  return vpDestMem;
#endif
  
}


/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT16
rfsfat_uiStrlen (RW_PTR_TO_RO (UINT8) pucString)
/* PURPOSE      : Determine the length of a string excluding the null 
 *                terminator.
 * ARGUMENTS    : pucString,
 *                a pointer (not a null pointer) to a null terminated array.
 * RETURNS      : The number of characters in the string.
 * PRECONDITION : 
 * POSTCONDITION: 
 * ERROR AND EXCEPTION HANDLING: 
 * NOTE: No return value is reserved to indicate an error.
 *****************************************************************************/
{
#if (RFSFAT_MEM_OPT ==1)
    return ((UINT16)strlen((const char *)pucString));
#else

  UINT16 uiStrlenCounter = 0;

  /* Check input parameters.                                                 */
  RFSFAT_ASSERT_LEAVE ((pucString != NULL), return 0;
	);

  /* Get the next character from the string until null is encountered.       */
  while (*pucString++)
	{							/* Increment the strlen counter.                                         */
	  uiStrlenCounter++;
	}

  return uiStrlenCounter;
#endif  
}




/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 *
rfsfat_ucStrcpy (UINT8 *  pucDestStr, const UINT8* const pucSourceStr)
/* PURPOSE      : Copy a string to another string.
 * ARGUMENTS    : pucDestStr,
 *                pointer (not a null pointer) to destination string.
 *                pucSourceStr,
 *                pointer (not a null pointer) to null terminated source string.
 * RETURNS      : A pointer to the destination string. 
 * PRECONDITION : 
 * POSTCONDITION: 
 * ERROR AND EXCEPTION HANDLING: 
 * NOTE: If the source and destination string overlap, the result is undefined.
 *       No return value is reserved to indicate an error.
 *****************************************************************************/
{

#if (RFSFAT_MEM_OPT ==1)

 return ((UINT8*)(strcpy((char *)pucDestStr,(char *)pucSourceStr)));

#else
  /* Make a copy of the given destination pointer so that it can be returned. */
  UINT8 *pucCopyDestStr = pucDestStr;
  const UINT8 *pucCopySrcStr = pucSourceStr;

  /* Check input parameters.                                                 */
  RFSFAT_ASSERT_LEAVE (((pucDestStr != NULL) && (pucSourceStr != NULL)),
					   return 0;
	);

  /* 
     Assign characters from the source to the destination until a null 
     terminator is encountered. 
   */
  while (*pucCopySrcStr)
	{
	 *(pucCopyDestStr++) = *(pucCopySrcStr++);							/* Do nothing                                                          */
	}
  *pucCopyDestStr=*pucCopySrcStr;
  return pucDestStr;
      
#endif
  
}




/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT8 *rfsfat_ucStrncpy
  (UINT8 * pucDestStr, RW_PTR_TO_RO (UINT8) pucSourceStr, UINT16 uiCount)
/* PURPOSE      : Copy a number of characters of a string to another string.
 * ARGUMENTS    : pucDestStr,
 *                pointer (not a null pointer) to destination string.
 *                pucSourceStr,
 *                pointer (not a null pointer) to null terminated source string.
 *                uiCount,
 *                number of characters to copy.
 * RETURNS      : A pointer to the destination string.
 * PRECONDITION : 
 * POSTCONDITION: 
 * ERROR AND EXCEPTION HANDLING: 
 * NOTE: If the source and destination string overlap, the result is undefined.
 *       No return value is reserved to indicate an error.
 *****************************************************************************/
{

#if (RFSFAT_MEM_OPT ==1)
   return ((UINT8*)(strncpy((char *)pucDestStr, (char *)pucSourceStr,uiCount)));

#else
  /* Make a copy of the given destination pointer so that it can be returned. */
  UINT8 *pucCopyDestStr = pucDestStr;

  /* Check input parameters.                                                 */
  RFSFAT_ASSERT_LEAVE (((pucDestStr != NULL) && (pucSourceStr != NULL)),
					   return (0);
	);

  /* 
     Assign characters from the source to the destination until a null 
     terminator is encountered or until the indicated number of characters has 
     been copied. 
   */
  while (uiCount && (*pucDestStr))
	{
	  uiCount--;
	  *(pucDestStr++) = *(pucSourceStr++);
	}

 *pucDestStr = *pucSourceStr;
  /* 
     The indicated number of characters to copy have been copied. The 
     destination string has to be pad out with zeros.
   */
  if (uiCount)
	{
	  while (--uiCount)
		{
		  *pucDestStr++ = EOSTR;
		}
	}
  return pucCopyDestStr;
#endif
  
}

/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT16 rfsfat_iStrcmp
  (RW_PTR_TO_RO (UINT8) pucString1, RW_PTR_TO_RO (UINT8) pucString2)
/* PURPOSE      : Compare 2 strings.
 * ARGUMENTS    : pucString1,
 *                pointer (not a null pointer) to string 1.
 *                pucString2,
 *                pointer (not a null pointer) to string 2.
 * RETURNS      : The return value indicates the relationship between the 
 *                strings.
 *                <0: pucString1 less than pucString2 
 *                0 : pucString1 identical to pucString2 
 *                >0: pucString1 greater than pucString2 
 * PRECONDITION : 
 * POSTCONDITION: 
 * ERROR AND EXCEPTION HANDLING: 
 * NOTE: 
 *****************************************************************************/
{
  UINT8 ucContentString1;
  UINT8 ucContentString2;

  /* Check input parameters.                                                 */
  RFSFAT_ASSERT_LEAVE (((pucString1 != NULL) && (pucString2 != NULL)),
					   return (0);
	);

  /* 
     Get the next characters from both strings and keep doing that until the end
     of pucString1 is encountered or until pucString1 and pucString2 are 
     different.
   */
  do
	{
	  ucContentString1 = *pucString1++;
	  ucContentString2 = *pucString2++;
	}
  while ((ucContentString1 > 0) && (ucContentString1 == ucContentString2));

  /* 
     Return the difference between the last 2 characters that have been 
     compared. 
   */
  return (ucContentString1 - ucContentString2);
}




/******Copyright by ICT Embedded BV All rights reserved **********************/
UINT16 rfsfat_iStrncmp
  (RW_PTR_TO_RO (UINT8) pucString1,
   RW_PTR_TO_RO (UINT8) pucString2, UINT16 uiCount)
/* PURPOSE      : Compare a number of characters of 2 strings.
 * ARGUMENTS    : pucString1,
 *                pointer (not a null pointer) to string 1.
 *                pucString2,
 *                pointer (not a null pointer) to string 2.
 *                uiCount,
 *                number of characters to compare.
 * RETURNS      : The return value indicates the relationship between the 
 *                strings.
 *                <0: pucString1 less than pucString2 
 *                0 : pucString1 identical to pucString2 
 *                >0: pucString1 greater than pucString2 
 * PRECONDITION : 
 * POSTCONDITION: 
 * ERROR AND EXCEPTION HANDLING: 
 * NOTE: 
 *****************************************************************************/
{
  UINT8 ucContentString1;
  UINT8 ucContentString2;

  /* Check input parameters.                                                 */
  RFSFAT_ASSERT_LEAVE (((pucString1 != NULL) && (pucString2 != NULL)),
					   return 0;
	);

  /* If nothing has to be compared, return 0                                 */
  if (uiCount == 0)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: Nothing to compare ",
						 RV_TRACE_LEVEL_WARNING);
	  return 0;
	}

  /* 
     Get the next characters from both strings and keep doing that until the end
     of pucString1 is encountered or until pucString1 and pucString2 are 
     different.
   */
  do
	{
	  ucContentString1 = *pucString1++;
	  ucContentString2 = *pucString2++;
	}
  while ((ucContentString1 > 0) && (ucContentString1 == ucContentString2) &&
		 (--uiCount > 0));

  /* 
     Return the difference between the last 2 characters that have been 
     compared. 
   */
  return (ucContentString1 - ucContentString2);
}

UINT16 rfsfat_strlen_uint16( const UINT16* const str_p)
{
  UINT16 uiStrlenCounter = 0;
  UINT16* workstr_p = (UINT16*)str_p;
  
  /* Check input parameters.                                                 */
  RFSFAT_ASSERT_LEAVE ((str_p!= NULL), return 0;
	);

  /* Get the next character from the string until null is encountered.       */
  while (*workstr_p++)
	{							/* Increment the strlen counter.                                         */
	  uiStrlenCounter++;
	}

  return uiStrlenCounter;
}




#if (FFS_UNICODE == 1)
/**************************************************/
// Unicode      
/**************************************************/

UINT8  rfsfat_conversionUnicodeToU8(
                    const T_WCHAR  *fileName_p,
                    char                  *u8Name_p)
{
  UINT16  cnt = 0;

  //we do it very, very simple (for now)
  while((fileName_p[cnt] != EOSTR) && (cnt < LFN_DIR_SIZE_META_NAME))
  {
    u8Name_p[cnt] = (char)fileName_p[cnt];
    cnt++;
  }
  u8Name_p[cnt] = EOSTR;

  if (u8Name_p[0] == EOSTR)
  {
    return FFS_RV_ERR_INVALID_PATH;
  }
  else
  {
    return FFS_NO_ERRORS;
  }
}


//  rfsfat_env_ctrl_blk_p->uc_conv.convert_u8_to_unicode = NULL
void convertUcToU8(const  T_WCHAR *mp_uc, UINT8 *mp)
{
  if (rfsfat_env_ctrl_blk_p->uc_conv.convert_unicode_to_u8 == NULL)
  {
   
   
   RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: function convert_unicode_to_u8 is  NULL");

   convertUcToSFN(mp_uc,mp);
   
  }
  else
  {
    rfsfat_env_ctrl_blk_p->uc_conv.convert_unicode_to_u8(mp_uc, mp);
  }

}


void convertU8ToUc(const  UINT8 *mp,T_WCHAR *mp_uc )
{
  if (rfsfat_env_ctrl_blk_p->uc_conv.convert_u8_to_unicode== NULL)
  {
   UINT16 cnt=0;
   
   RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: function convert_unicode_to_u8 is  NULL");
 
    cnt=0;
	while(mp[cnt] != EOSTR)
		{
		   mp_uc[cnt]=((T_WCHAR)(mp[cnt]));
		   cnt++;
		}
	mp_uc[cnt]=EOSTR;
  }
  else
  {
    rfsfat_env_ctrl_blk_p->uc_conv.convert_u8_to_unicode(mp,mp_uc);
  }

}



/**************************************************/
// end of Unicode      
/**************************************************/
#endif


