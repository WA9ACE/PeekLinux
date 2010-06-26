/*******************************************************************************\
*
*   FILE NAME:      btl_utils.c
*
*   DESCRIPTION:    This file contains utility functions.
*
*   AUTHOR:         Ronen Levy
*
\*******************************************************************************/
#include "utils.h"
#include "bttypes.h"
#include "btl_defs.h"
#include "btl_utils.h"

#include "osapi.h"
#include "bttypes.h"
#include "bthal_fs.h"
#include "btl_config.h"
#include "btl_utils.h"
#include "btl_debug.h"

#include "btl_obex_utils.h"
#include "btl_bip_common.h"
#include "btl_bip_xml.h"
#include "btl_bipint.h"
#include "bip.h"



BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BTL_COMMON);

/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/
#define BTL_UTILS_SCRATCH_LENGTH      10


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
 * Globals
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Local function prototypes
 *
 *******************************************************************************/

/* XML Build Attribute support routines */
static char btlUtilsNtoc(U8 Nr);
static char *btlUtilsItoa(U32 Nr, S8 *AddrString, BOOL bZeroFill, U8 nrOfChar);




BtStatus BTL_UTILS_FindMatchingListEntry(
				ListEntry			 		*listHead, 
				const ListEntry				*entryToMatch, 
				BtlUtilsListComparisonFunc	comparisonFunc,
				ListEntry					**matchingEntry)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	ListEntry	*checkedEntry = 0;
	
	BTL_FUNC_START("BTL_UTILS_FindMatchingListEntry");
	
	BTL_VERIFY_FATAL((0 != listHead), BT_STATUS_INTERNAL_ERROR, ("Null list argument"));
	BTL_VERIFY_FATAL((0 != entryToMatch), BT_STATUS_INTERNAL_ERROR, ("Null entryToMatch argument"));
	BTL_VERIFY_FATAL((0 != comparisonFunc), BT_STATUS_INTERNAL_ERROR, ("Null comparisonFunc argument"));
	BTL_VERIFY_FATAL((0 != matchingEntry), BT_STATUS_INTERNAL_ERROR, ("Null matchingEntry argument"));

	/* Assume a matching entry will not be found */
	*matchingEntry = 0;

	/* Obtain the first entry */
	checkedEntry = GetHeadList(listHead);

	/* Check all list entries, until a match is found, or list exhausted */
	while (checkedEntry != listHead)
	{
		/* Check the current entry, using the specified comparison function */
		if (TRUE == comparisonFunc(entryToMatch, checkedEntry))
		{
			*matchingEntry = checkedEntry;

			/* entry matches, stop comparing */
			break;
		}

		/* Move on to check the next entry */
		checkedEntry = checkedEntry->Flink;
	}

	BTL_FUNC_END();
	
	return status;
}

char    *BTL_UTILS_LockedBdaddrNtoa(const BD_ADDR *addr)
{
	static char bdAddressStr[17 + 1];
	
	return bdaddr_ntoa(addr, bdAddressStr);
}


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
                                     BtlDateAndTimeStruct *dateAndTimeStruct)
{
  	U8 i;
  	S8 tempTimeBuff[5];
  	U8 TimeBuffIndex = 0;

  	/* Extracting year */
  	for(i = 0; i < 4; i++)
  	{
    		tempTimeBuff[i] = dateAndTimeString[TimeBuffIndex];
    		TimeBuffIndex++;
  	}
  	tempTimeBuff[4] = '\0';
  	dateAndTimeStruct->year = (U16) OS_AtoU32((char*)tempTimeBuff);

  	/* Extracting month */
  	for(i = 0; i < 2; i++)
  	{
    		tempTimeBuff[i] = dateAndTimeString[TimeBuffIndex];
    		TimeBuffIndex++;
  	}
  	tempTimeBuff[2] = '\0';
  	dateAndTimeStruct->month = (U8)OS_AtoU32((char*)tempTimeBuff);

  	/* Extracting day */
  	for(i = 0; i < 2; i++)
  	{
    		tempTimeBuff[i] = dateAndTimeString[TimeBuffIndex];
    		TimeBuffIndex++;
  	}
  	tempTimeBuff[2] = '\0';
  	dateAndTimeStruct->day = (U8)OS_AtoU32((char*)tempTimeBuff);

  	/* skip the T char in middle */
  	TimeBuffIndex++;

  	/* Extracting hour*/
  	for(i = 0; i < 2; i++)
  	{
    		tempTimeBuff[i] = dateAndTimeString[TimeBuffIndex];
    		TimeBuffIndex++;
  	}
  	tempTimeBuff[2] = '\0';
  	dateAndTimeStruct->hour = (U8)OS_AtoU32((char*)tempTimeBuff);

  	/* Extracting minutes*/
  	for(i = 0; i < 2; i++)
  	{
    		tempTimeBuff[i] = dateAndTimeString[TimeBuffIndex];
    		TimeBuffIndex++;
  	}
  	tempTimeBuff[2] = '\0';
  	dateAndTimeStruct->minute = (U8)OS_AtoU32((char*)tempTimeBuff);

  	/* Extracting seconds*/
  	for(i = 0; i < 2; i++)
  	{
    		tempTimeBuff[i] = dateAndTimeString[TimeBuffIndex];
    		TimeBuffIndex++;
  	}
  	tempTimeBuff[2] = '\0';
  	dateAndTimeStruct->second = (U8)OS_AtoU32((char*)tempTimeBuff);

  	if (dateAndTimeString[TimeBuffIndex] == 'Z')
    {
    		dateAndTimeStruct->utcTime = TRUE;
    }
  	else
    {
    		dateAndTimeStruct->utcTime = FALSE;
    }
}


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
                                     BtlDateAndTimeString dateAndTimeString)
{
    S8 str[BTL_UTILS_SCRATCH_LENGTH]; /* temp scratch area for retrieving individual data/time elements. */

    /* Date and time info present as input? --> build YYYYMMDDTHHMMSSZ string */
    if (NULL != dateAndTimeStruct)
    {
        /* YYYY */
        btlUtilsItoa(dateAndTimeStruct->year, str, TRUE, 4);
        OS_StrCpy((char*)&dateAndTimeString[0], (char*)str );
        /* MM */
        btlUtilsItoa(dateAndTimeStruct->month, str, TRUE,2);
        OS_StrCat((char*)&dateAndTimeString[3], (char*)str);
        /* DD */
        btlUtilsItoa(dateAndTimeStruct->day, str, TRUE, 2);
        OS_StrCat((char*)&dateAndTimeString[5], (char*)str);

        /* 'T' as boundary between date and time. */
        OS_StrCat((char*)&dateAndTimeString[7], "T" );

        /* HH */
        btlUtilsItoa(dateAndTimeStruct->hour, str, TRUE, 2);
        OS_StrCat((char*)&dateAndTimeString[8], (char*)str );
        /* MM */
        btlUtilsItoa(dateAndTimeStruct->minute, str, TRUE, 2);
        OS_StrCat((char*)&dateAndTimeString[10], (char*)str );
        /* SS */
        btlUtilsItoa(dateAndTimeStruct->second, str, TRUE, 2);
        OS_StrCat((char*)&dateAndTimeString[12], (char*)str);

        /* UTC time? --> add a 'Z'. */
        if (TRUE == dateAndTimeStruct->utcTime)
        {
            OS_StrCat((char*)&dateAndTimeString[14], "Z");
        }
    }

    /* No date and time as input! --> make a don't care string (any date and time). */
    else
    {
        OS_StrCpy((char*)&dateAndTimeString[0], "*" );
    }
}


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
 *
 *		permString [out] - Permissions in 0-terminated string format
 *          The format is "RDW".
 *
 * Returns:
 *      void
 *
 */
void BTL_UTILS_XmlBuildPermissions(BTHAL_U16 perm, BtlPermissionString permString)
{
    OS_StrCpy((char*)permString, "");

    if (perm & BTHAL_FS_PERM_READ)
    {
        OS_StrCat((char*)permString, "R");
    }    

    if (perm & BTHAL_FS_PERM_WRITE)
    {
        OS_StrCat((char*)permString, "W");
    }    

    if (perm & BTHAL_FS_PERM_DELETE)
    {
        OS_StrCat((char*)permString, "D");
    }    
}


/*-------------------------------------------------------------------------------
 * btlUtilsNtoc()
 */
static char btlUtilsNtoc( U8 Nr )
{
    S8  c;

    if( Nr > 10 )
        return 0;

    c = (S8)(0x30 + Nr);
    return c;
}

/*-------------------------------------------------------------------------------
 * btlUtilsItoa()
 */
static char *btlUtilsItoa(U32 Nr, S8 *AddrString, BOOL bZeroFill, U8 nrOfChar)
{
    U8           u, l, t;

    AddrString[BTL_UTILS_SCRATCH_LENGTH] = 0;

    for( u=0; u < BTL_UTILS_SCRATCH_LENGTH; u++ )
    {
        AddrString[u] = btlUtilsNtoc((S8)(Nr%10));
        Nr = Nr / 10;
    }
    u--;
    if( FALSE == bZeroFill )
    {
        while( (AddrString[u] == '0') && u!= 0 )
        {
            AddrString[u] = 0;
            u--;
        }
    }
    else
    {
        if( nrOfChar < BTL_UTILS_SCRATCH_LENGTH )
        {
            AddrString[nrOfChar] = 0;
        }
    }
    l = (U8)(OS_StrLen((char*)AddrString)-1);
    u = 0;
    while( u < l )
    {
        t = AddrString[u];
        AddrString[u] = AddrString[l];
        AddrString[l] = t;
        u++;
        l--;
    }
    return (char*)AddrString;
}



