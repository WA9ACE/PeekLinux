/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: ATBPbGI.c $|
| 
+--------------------------------------------------------------------+

   MODULE  : ATBPbGI

   PURPOSE : This modul contains phonebook management functions using ATB.

   HISTORY:

     Oct 27, 2005	REF: OMAPS 48881 b-nekkare
      Description: PhoneBook: The modify option under phonebook doesn't 
                        work as expected.
      Solution :  Change is needed when we initially store the PB entry with 
                      a '+' and then we delete the '+' and resave the number again. 
	               If this is not done we would always end up preserving the TON 
	               as INTERNATIONAL type
*/
#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include <stdio.h>
#include <string.h>

#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_ss.h"

#include "cus_aci.h"
#include "prim.h"
#include "pcm.h"

#include "ATBPb.h"
#include "ATBPbGI.h"


/*******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *
 ******************************************************************************/

void GI_alpha_AlphaToAscii(T_MFW_PHB_TEXT *ascii, T_PB_ALPHA *alpha, SHORT max_len);
void GI_alpha_AsciiToAlpha(T_PB_ALPHA *alpha, T_MFW_PHB_TEXT *ascii, SHORT max_len);
T_PB_INDEX GI_index_Convert(UBYTE mode);


/*******************************************************************************

 $Function:    	GI_pb_GetPhonebook
 
 $Description:	Checks to see which file system is selected

 $Returns:		MFW_PHB_OK			Action completed OK.
				MFW_PHB_FAIL		File write encountered an error

 $Arguments:	intphonebook		Place to store type of phonebook selected
 									(TRUE for FFS)
 
*******************************************************************************/

T_MFW	GI_pb_GetPhonebook(UBYTE *intphonebook)
{
	PB_RET result;
	T_MFW ret;
	T_PB_TYPE current_type;

	result = ATB_pb_GetPhonebook(PB_BOOK_DEFAULT, &current_type);

	if (current_type==PB_TYPE_FFS)
		*intphonebook = TRUE;
	else
		*intphonebook = FALSE;
	
	if (result==PB_OK)
		ret = MFW_PHB_OK;
	else
		ret = MFW_PHB_FAIL;

	return ret;
}


/*******************************************************************************

 $Function:    	GI_pb_SetPhonebook
 
 $Description:	Selects file system
 
 $Returns:		MFW_PHB_OK			Action completed OK.
				MFW_PHB_FAIL		File write encountered an error

 $Arguments:	intphonebook		TRUE if FFS is to be used
 
*******************************************************************************/

T_MFW	GI_pb_SetPhonebook(UBYTE intphonebook)
{
	PB_RET result;
	T_MFW ret;
	T_PB_TYPE current_type;

	if (intphonebook)
		current_type = PB_TYPE_FFS;
	else
		current_type = PB_TYPE_SIM;
	
	result = ATB_pb_SetPhonebook((SHORT)PB_BOOK_DEFAULT, current_type);
	
	if (result==PB_OK)
		ret = MFW_PHB_OK;
	else
		ret = MFW_PHB_FAIL;

	return ret;
}


/*******************************************************************************

 $Function:    	GI_pb_Initialise
 
 $Description:	Initialise an instance of phonebook.  Dynamically allocates a record
 				cache in RAM.  Creates the necessary file(s) if they do not already exist.
 				Sorts the phonebook by name and by number, creating the appropriate
 				index tables.
 				
 $Returns:		MFW_PHB_OK
 				MFW_PHB_FAIL

 $Arguments:	None
								
*******************************************************************************/

T_MFW	GI_pb_Initialise(void)
{
	PB_RET result;
	T_MFW ret=MFW_PHB_FAIL;

	tracefunction("GI_pb_Initialise");

	result = ATB_pb_Initialise(PB_BOOK_DEFAULT, PB_TYPE_FFS, PB_RECORDS_MAX,
		PB_CACHE_MAX, PB_ALPHATAG_MAX, PB_NUMBER_MAX, PB_EXT_MAX);

	switch(result)
	{
		case PB_OK:
			ret = MFW_PHB_OK;
			break;
		case PB_EXCT:
			ret = MFW_PHB_OK;
			break;
		case PB_BOOKALREADYEXISTS:
			ret = MFW_PHB_FAIL;
			break;
	}

	return ret;
}


/*******************************************************************************

 $Function:    	GI_pb_Exit
 
 $Description:	Frees memory associated with phonebook.  To be called when the
 				phonebook is no longer required, or at shutdown.

 $Returns:		MFW_PB_OK		Action completed OK.

 $Arguments:	None
 
*******************************************************************************/

T_MFW	GI_pb_Exit(void)
{
	tracefunction("GI_pb_Exit");

	ATB_pb_Exit(PB_BOOK_DEFAULT);
	return MFW_PHB_OK;
}


/*******************************************************************************

 $Function:    	GI_pb_Info
 
 $Description:	Returns information about the phonebook, which will be returned
 				in the data structure pointed to by status.  The caller must
 				allocate the T_MFW_PHB_STATUS structure.

 $Returns:		MFW_PB_OK		Action completed OK.

 $Arguments:	status			Data structure to contain phonebook info
 
*******************************************************************************/

void GI_pb_Info(T_MFW_PHB_STATUS *status)
{
	T_PB_INFO info;

	tracefunction("GI_pb_Info");

	ATB_pb_Info(PB_BOOK_DEFAULT, &info);
	
	status->book = PHB_IPB;
	status->max_entries = (UBYTE)info.records_max;
	status->used_entries = (UBYTE)info.records_used;
	status->avail_entries = (UBYTE)info.records_free;
	status->tag_len = PHB_MAX_LEN;

	return;
}


/*******************************************************************************

 $Function:    	GI_pb_Status
 
 $Description:	Returns whether or not the phonebook is busy.  If so, stat will contain
 				PHB_BUSY, otherwise PHB_READY.

 $Returns:		None

 $Arguments:	stat			UBYTE to contain status information
 
*******************************************************************************/

void GI_pb_Status(UBYTE *stat)
{
	
	
	tracefunction("GI_pb_Status");

	ATB_pb_Status(PB_BOOK_DEFAULT);
	
	return;
}



/*******************************************************************************

 $Function:    	GI_pb_WriteRec
 
 $Description:	Writes a record to the physical position index.  If the index is
 				0, a new record is added, otherwise the previous
 				record at that position is overwritten.

 $Returns:		MFW_PHB_OK
 				MFW_PHB_FAIL
 				MFW_PHB_FULL

 $Arguments:	entry	The phonebook entry to write
 
*******************************************************************************/

T_MFW	GI_pb_WriteRec(T_MFW_PHB_ENTRY *entry)
{
	T_MFW ret;
	PB_RET result;
	SHORT	index;
	T_PB_RECORD *record;
	UBYTE *number;

	tracefunction("GI_pb_WriteRec");

	/* Allocate memory for record */

	record = ATB_pb_AllocRec(PB_BOOK_DEFAULT);
	
	/* If the provided index is 0, we must add new record */
	
	if (entry->index==0)
		index = PB_NEW_RECORD;
	else
		index = (SHORT)(entry->index-1);	/* MFW/ACI indices start at 1, ATB indices start at 0 */

	/* Convert name to unicode*/
	
	record->alpha.dcs = entry->name.dcs;
	record->alpha.length = (UBYTE)entry->name.len;
	GI_alpha_AsciiToAlpha(&record->alpha, &entry->name, PHB_MAX_LEN);

	/* Convert number to BCD */

	record->ton_npi = entry->npi & 0x0F;
	record->ton_npi |= (entry->ton<<4);
	
	number = entry->number;

	if (number[0]=='+')
	{
		record->ton_npi &= 0xF0;
		record->ton_npi |= (MFW_TON_INTERNATIONAL<<4);
		number++;
	}
	// Oct 27, 2005	REF: OMAPS OMAPS00048881 b-nekkare
	else if(entry->ton == MFW_TON_INTERNATIONAL)
	{
              record->ton_npi = entry->npi & 0x0F;
              record->ton_npi |= (MFW_TON_UNKNOWN<<4);
	}
       /* Anything else keep as is */

	TRACE_EVENT_P1("ton_npi: %X", record->ton_npi);
  
	ATB_pb_CharToBCD(record->number, (char *)number, PB_NUMBER_MAX);

	/* Write the record */
	
	result = ATB_pb_WriteRec(PB_BOOK_DEFAULT, INDEX_PHYSICAL, index, record);

	/* Free allocated record */

	ATB_pb_FreeRec(PB_BOOK_DEFAULT, record);
	
	switch(result)
	{
		case PB_OK:
			ret = MFW_PHB_OK;
			break;
		case PB_BOOKFULL:
			ret = MFW_PHB_FULL;
			break;
		default:
			ret = MFW_PHB_FAIL;
			break;
	}

	return ret;
}


/*******************************************************************************

 $Function:    	GI_pb_DeleteRec
 
 $Description:	Deletes a record at a physical position index.

 $Returns:		MFW_PHB_OK
 				MFW_PHB_FAIL
 
 $Arguments:	index		Physical index of the record to delete

*******************************************************************************/

T_MFW	GI_pb_DeleteRec(UBYTE index)
{
	T_MFW ret;
	PB_RET result;

	tracefunction("GI_pb_DeleteRec");

	/* MFW/ACI indices start at 1, ATB indices start at 0 */

	index--;

	/* Delete record */
	
	result = ATB_pb_DeleteRec((SHORT)PB_BOOK_DEFAULT, INDEX_PHYSICAL, (SHORT)index);

	switch(result)
	{
		case PB_OK:
			ret = MFW_PHB_OK;
			break;
		default:
			ret = MFW_PHB_FAIL;
			break;
	}

	return ret;	
}


/*******************************************************************************

 $Function:    	GI_pb_Find
 
 $Description:	Find the nearest match to the supplied record. 

 $Returns:		MFW_PHB_OK
 				MFW_PHB_FAIL

 $Arguments:	mode 			Specifies the sorting for the index list
				index			Place to store the index of the matching record
				num_recs		Number of records to read in
				search_pattern	The string or number to match
				entries			Place to store the resulting list of entries
 
*******************************************************************************/

T_MFW GI_pb_Find(UBYTE mode, UBYTE *index, UBYTE num_recs,  T_MFW_PHB_TEXT *search_pattern, T_MFW_PHB_LIST *entries)
{
	PB_RET result;
	T_MFW ret=MFW_PHB_FAIL;
	T_PB_INDEX index_type = GI_index_Convert(mode);
	T_PB_RECORD *record;
	SHORT rec_index;
	UBYTE *number;

	tracefunction("GI_pb_Find");

	/* Allocate memory for record */

	record = ATB_pb_AllocRec(PB_BOOK_DEFAULT);

	/* Convert name to unicode, or number to BCD */
		
	if(index_type==INDEX_NAME)
	{
		GI_alpha_AsciiToAlpha(&record->alpha, search_pattern, PB_ALPHATAG_MAX);
	}
    else
    {
    	/* Convert number to BCD */

		number = search_pattern->data;
		
		if (number[0]=='+')
		{
			number++;
		}
      	ATB_pb_CharToBCD(record->number, (char *)number, PB_NUMBER_MAX);
	}

	result = ATB_pb_Find(PB_BOOK_DEFAULT, index_type, record, &rec_index, NULL);
	
	if (result==PB_OK)
	{
		*index = rec_index+1;	/* MFW/ACI indices start at 1, ATB indices start at 0 */
		ret = GI_pb_ReadRecList(mode, (UBYTE)(*index), num_recs, entries);/*a0393213 warnings removal-result changed to ret*/
	}

	/* Free allocated record */

	ATB_pb_FreeRec(PB_BOOK_DEFAULT, record);

	
	switch(result)
	{
		case PB_OK:
			/*ret = MFW_PHB_OK;*//*a0393213 warnings removal-commented because of last change for warning*/
			break;
		default:
			ret = MFW_PHB_FAIL;
			break;
	}
	
	return ret;
}

/*******************************************************************************

 $Function:    	GI_pb_ReadRecList
 
 $Description:	Read in a list of entries.

 $Returns:		MFW_PHB_OK
 				MFW_PHB_FAIL

 $Arguments:	mode 			Specifies the sorting for the index list
				index			Place to store the index of the matching record
				num_recs		Number of records to read in
				entries			Place to store the resulting list of entries
 
*******************************************************************************/

T_MFW GI_pb_ReadRecList(UBYTE mode, UBYTE index, UBYTE num_recs, T_MFW_PHB_LIST *entries)
{
	PB_RET result;
	T_MFW ret;
	T_PB_INDEX index_type = GI_index_Convert(mode);
	T_PB_RECORD *record;
	SHORT recs_count;

	tracefunction("GI_pb_ReadRecList");

	/* MFW/ACI indices start at 1, ATB indices start at 0 */
	
	index--;

	/* Allocate memory for record */

	record = ATB_pb_AllocRec(PB_BOOK_DEFAULT);

	for (recs_count = 0; recs_count<(SHORT)num_recs; recs_count++)
	{
		result = ATB_pb_ReadRec(PB_BOOK_DEFAULT, index_type, (SHORT)(index+recs_count), record);
		if (result!=PB_OK)
			break;
		
		entries->entry[recs_count].book = PHB_IPB;
		entries->entry[recs_count].index = GI_pb_ConvIndex(index_type, (UBYTE)(index+recs_count), INDEX_PHYSICAL);
		entries->entry[recs_count].ton = (record->ton_npi & 0xF0)>>4;
		entries->entry[recs_count].npi = record->ton_npi & 0x0F;
		GI_alpha_AlphaToAscii(&entries->entry[recs_count].name, &record->alpha, PB_ALPHATAG_MAX);
		ATB_pb_BCDToChar((char *)entries->entry[recs_count].number, record->number, PB_NUMBER_MAX);
	}

	entries->book = PHB_IPB;

	if (recs_count)
	{
	    entries->result = MFW_ENTRY_EXIST;
	    entries->num_entries = (UBYTE) recs_count;
	    /* SPR#1994 - SH - If at least one record is found, result is a success */
		result = PB_OK;
	}
	else
	{
		entries->result = MFW_NO_ENTRY;
	}

	/* Free allocated record */

	ATB_pb_FreeRec(PB_BOOK_DEFAULT, record);
		
	switch(result)
	{
		case PB_OK:
			ret = MFW_PHB_OK;
			break;
		case PB_RECDOESNOTEXIST:
			ret = MFW_PHB_FAIL;
			break;
		default:
			ret = MFW_PHB_FAIL;
			break;
	}

	return ret;
}

/*******************************************************************************

 $Function:    	GI_pb_ConvIndex
 
 $Description:	Returns the index in table new_index_type corresponding to the index
 				in table index_type.
 
 $Returns:		The new index

 $Arguments:	index_type		The index table of the original index.
				index			The original logical index.
				new_index_type	The index table required.
				
*******************************************************************************/

UBYTE GI_pb_ConvIndex(T_PB_INDEX index_type, UBYTE index, T_PB_INDEX new_index_type)
{
	SHORT new_index;

	tracefunction("GI_pb_ConvIndex");
	
	ATB_pb_ConvIndex(PB_BOOK_DEFAULT, index_type, (SHORT)index, new_index_type, &new_index);

	/* MFW/ACI indices start at 1, ATB indices start at 0 */

	if (new_index_type==INDEX_PHYSICAL)
	{
		new_index++;
	}
	
	return (UBYTE)new_index;
}


/********************************
 * Functions called by the ATB PB  *
 ********************************/

/*******************************************************************************

 $Function:    	GI_pb_OK
 
 $Description:	This function is called if the requested command executed successfully.
 				The param parameter is a pointer to command specific information,
 				when such information is required to be passed from the ATB to the GI.
 				See the individual functions for more details.
 
 $Returns:		None.

 $Arguments:	phonebook_id	The phonebook identifier
				command_id		Identifies the command.
				param			Pointer to command-specific information
 
*******************************************************************************/

void GI_pb_OK(SHORT phonebook_id, SHORT command_id, void *param)
{
	trace("GI_pb_OK");

	if (param!=NULL)
	{
		trace_P1("Param is: %d", *((SHORT *)param));
	}
}

/*******************************************************************************

 $Function:    	GI_pb_Error
 
 $Description:	This function is called if an error was returned while executing the
 				requested command.
 
 $Returns:		None.

 $Arguments:	phonebook_id	The phonebook identifier
				command_id		Identifies the command.
				error_id		Identifies the error that occurred.
 
*******************************************************************************/

void GI_pb_Error(SHORT phonebook_id, SHORT command_id, SHORT error_id)
{
	trace("GI_pb_Error");
	trace_P2("***ERROR*** - command %d, error %d", command_id, error_id);

	return;
}


/*******************************************************************************

 $Function:    	GI_pb_MemAlloc
 
 $Description:	Dynamically allocates memory for the phonebook.
 
 $Returns:		The address of the block of allocated memory, or NULL if failed.

 $Arguments:	size	- Amount of memory to be allocated in bytes
 
*******************************************************************************/
// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32

UBYTE *GI_pb_MemAlloc(U32 size)
{
	return mfwAlloc((U32)size);
}


/*******************************************************************************

 $Function:    	GI_pb_MemFree
 
 $Description:	Frees dynamically allocated memory.
 
 $Returns:		None
 
 $Arguments:	mem - Pointer to a block of allocated memory
 				size - Size of the block
 
*******************************************************************************/
// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32

void GI_pb_MemFree(UBYTE *mem, U32 size)
{
	mfwFree(mem, (U32)size);
	return;
}


/*******************************************************************************

 $Function:    	GI_pb_Match
 
 $Description:	This function allows the user to specify the way in which entries are
 				matched during searching.  This provides the capability of searching
 				languages with non-Latin alphabets, or searching through special user
 				fields.
				This function attempts a match between two records based on the
				field appropriate for the index list, specified in index_type.  It returns
				a value indicating whether there is no match (MATCH_NONE),
				a partial match (MAX_START), or an exact match (MATCH_EXACT).
				Note that the contents of the records themselves should not be altered
				by this function.  The sole result should be a return value that specifies
				how the first record matches the second.  If it is preferred that the
				default matching routine is used (i.e match from the start for alpha
				tags, match from the end for phone numbers) then MATCH_DEFAULT
				should be returned.

 $Returns:		MATCH_DEFAULT, MATCH_EXACT, MATCH_START or MATCH_NONE

 $Arguments:	record1		The first record to be compared
				record2		The second record to be compared
				index_type	Indicator of the field which is to be compared
 
*******************************************************************************/

T_PB_MATCH GI_pb_Match(T_PB_RECORD *record1,T_PB_RECORD *record2, T_PB_INDEX index_type)
{
	return MATCH_DEFAULT;
}


/*******************************************************************************

 $Function:    	GI_pb_Compare
 
 $Description:	This function allows the user to specify the way in which entries are
 				compared during sorting.  This provides the capability of sorting languages
 				with non-Latin alphabets, or sorting to alternative orders (for example,
 				reverse alphabetical).
				This function compares two records based on the field appropriate for the
				index list, specified in index_type.
				It returns a value indicating whether the first record should come before
				or after the second.
				Note that the contents of the records themselves should not be altered by
				this function.  The sole result should be a return value that specifies
				whether or not the positions of the records should be swapped.  If it
				is preferred that the default comparison routine is used (e.g.Latin
				alphabetical ascending) then COMPARE_DEFAULT should be returned.


 $Returns:		COMPARE_DEFAULT		No comparison made - use the default comparison method
				COMPARE_FIRSTBEFORE		The first record should come before the second record
				COMPARE_FIRSTAFTER		The first record should come after the second record

 $Arguments:	record1		The first record to be compared
				record2		The second record to be compared
				index_type	Index list to use
 
*******************************************************************************/

T_PB_COMPARE GI_pb_Compare (T_PB_RECORD *record1,T_PB_RECORD *record2, T_PB_INDEX index_type)
{
	return COMPARE_DEFAULT;
}


/*******************************************************************************

 $Function:    	GI_pb_GetTable
 
 $Description:	If fields other than name and number are required, then index tables
 				must be provided for each field.  This function is called when access
 				to an index table is required for an unknown index type.

 				The array provided must be a user-allocated array of type SHORT,
 				records_max in size, e.g.

 				static SHORT user_table[RECORDS_MAX];
 				return user_table;

 $Returns:		A pointer to an array of SHORTs records_max in size.

 $Arguments:	index_type	Index list to use
 
*******************************************************************************/

SHORT *GI_pb_GetTable (T_PB_INDEX index_type)
{
	SHORT *user_table = NULL;	/*DUMMY IMPLEMENTATION*/
	return user_table;
}


/*******************************************************************************

 $Function:    	GI_alpha_AlphaToAscii
 
 $Description:	Convert an alpha tag to an ascii string

 $Returns:		None

 $Arguments:	ascii		The ascii string
 				alpha		The alpha tag
 				max_len		The maximum length of the resulting string
 
*******************************************************************************/

void GI_alpha_AlphaToAscii(T_MFW_PHB_TEXT *ascii, T_PB_ALPHA *alpha, SHORT max_len)
{
	SHORT asciiIndex;
	SHORT alphaIndex;

	/* Unicode entry - swap the low and high bytes, add in 0x80 at start*/

	ascii->dcs = alpha->dcs;
	
	if (alpha->dcs == MFW_DCS_UCS2)
	{
		ascii->data[0] = 0x80;
		asciiIndex = 1;
		alphaIndex = 0;
		
		while (alphaIndex<alpha->length && asciiIndex<max_len)
		{
			ascii->data[asciiIndex++] = (char) (alpha->data[alphaIndex]>>8);
			ascii->data[asciiIndex++] = (char) (alpha->data[alphaIndex] & 0xFF);
			alphaIndex++;
		}

		/* Put two NULLs at the end, if there is space */
		
		if (asciiIndex<max_len)
			ascii->data[asciiIndex++] = NULL;
		if (asciiIndex<max_len)
			ascii->data[asciiIndex] = NULL;

		ascii->len = asciiIndex;
	}

	/* Ascii entry - convert from unicode */
	
	else
	{
		asciiIndex = 0;
		alphaIndex = 0;
		
		while (alphaIndex<alpha->length && asciiIndex<max_len)
		{
			ascii->data[asciiIndex++] = (char) (alpha->data[alphaIndex++]);
		}

		/* Put a NULL at the end, if there is space */

		if (asciiIndex<max_len)
			ascii->data[asciiIndex] = NULL;

		ascii->len = asciiIndex;
	}
	
	return;
}


/*******************************************************************************

 $Function:    	GI_alpha_AsciiToAlpha
 
 $Description:	Convert an ascii string to an alpha tag

 $Returns:		None

 $Arguments:	alpha		The alpha tag
 				ascii		The ascii string
 				max_len		The maximum length of the resulting string
 
*******************************************************************************/

void GI_alpha_AsciiToAlpha(T_PB_ALPHA *alpha, T_MFW_PHB_TEXT *ascii, SHORT max_len)
{
	SHORT asciiIndex;
	SHORT alphaIndex;

	alpha->dcs = ascii->dcs;
	
	/* Unicode entry - swap the low and high bytes, ignore first '0x80' character */
	
	if (ascii->dcs == MFW_DCS_UCS2)
	{
		asciiIndex = 1;
		alphaIndex = 0;
	
		while (asciiIndex<ascii->len && alphaIndex<max_len)
		{
			alpha->data[alphaIndex] = (USHORT) ((ascii->data[asciiIndex]<<8) | ascii->data[asciiIndex+1]);
			alphaIndex=alphaIndex+1;
			asciiIndex=asciiIndex+2;
		}

		/* Put a NULL at the end, if there is space */
		
		if (alphaIndex<max_len)
			alpha->data[alphaIndex] = NULL;
	}

	/* Ascii entry - convert to unicode */
	
	else
	{
		asciiIndex = 0;
		alphaIndex = 0;
	
		while (asciiIndex<ascii->len && alphaIndex<max_len)
		{
			alpha->data[alphaIndex++] = (USHORT)ascii->data[asciiIndex++];
		}
		
		/* Put a NULL at the end, if there is space */
		
		if (alphaIndex<max_len)
			alpha->data[alphaIndex] = NULL;
	}
	
	alpha->length = alphaIndex;

	return;
}


/*******************************************************************************

 $Function:    	GI_index_Convert
 
 $Description:	Convert indexing types from MFW to ATB.

 $Returns:		The ATB indexing type

 $Arguments:	mode		The MFW sort mode
 
*******************************************************************************/

T_PB_INDEX GI_index_Convert(UBYTE mode)
{
	T_PB_INDEX index_type=INDEX_DEFAULT_MAX;
	
	switch(mode)
	{
		case MFW_PHB_ALPHA:
			index_type = INDEX_NAME;
			break;
		case MFW_PHB_NUMBER:
			index_type = INDEX_NUMBER;
			break;
		case MFW_PHB_INDEX:
			index_type = INDEX_PHYSICAL;	/* NOTE - may not work quite as intended */
			break;
		case MFW_PHB_PHYSICAL:
			index_type = INDEX_PHYSICAL;
			break;
	}

	return index_type;
}
