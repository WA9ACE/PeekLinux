#ifndef ATBPB_H
#define ATBPB_H

#define BLOCKING

#define trace(x)			TRACE_EVENT(x)
#define tracefunction(x)	TRACE_FUNCTION(x)
#define trace_P1(x,a)		TRACE_EVENT_P1(x,a)
#define trace_P2(x,a,b)		TRACE_EVENT_P2(x,a,b)
#define trace_P3(x,a,b,c)	TRACE_EVENT_P3(x,a,b,c)
#define trace_P4(x,a,b,c,d)	TRACE_EVENT_P4(x,a,b,c,d)
#define trace_P5(x,a,b,c,d,e)   TRACE_EVENT_P5(x,a,b,c,d,e)
#define trace_P6(x,a,b,c,d,e,f)  TRACE_EVENT_P6(x,a,b,c,d,e,f)

/**********************************************
 * STANDARD DEFINITIONS
 *
 **********************************************/
 
#define UBYTE		unsigned char
#define USHORT		unsigned short
#define ULONG		unsigned long

#define TRUE		1
#define FALSE		0

/**********************************************
 * CONSTANTS
 *
 **********************************************/

#define PB_BOOK_DEFAULT			0		/* Default phonebook id */
#define PB_PHONEBOOKS_MAX		1		/* Maximum number of concurrent phonebooks possible */
#define PB_RECORDS_MAX			50		/* Max number of records per phonebook */
#define PB_CACHE_MAX			5		/* Maximum records stored in the RAM cache */
#define PB_ALPHATAG_MAX			20		/* Maximum length of alpha tag in characters */
#define PB_NUMBER_MAX			41		/* Maximum length of BCD number in digits*/
#define PB_EXT_MAX				0 		/* Maximum length of extended data in bytes */
#define PB_NEW_RECORD			-1		/* Indicate that a new record is to be created. */
#define PB_EMPTY_RECORD			-1		/* Indicates that the physical record is empty */


/**********************************************
 * PB_RET
 *
 * Standard return type from functions
 *
 **********************************************/
 
typedef enum
{
	PB_OK,
	PB_EXCT,
	PB_ERROR,							/* Generic error */

	PB_BUSY,							/* Phonebook is busy */
	PB_FILEREADFAIL,					/* File read encountered an error */
	PB_FILEWRITEFAIL,					/* File write encountered an error */
	PB_BOOKDOESNOTEXIST,				/* Tried to access a phonebook that does not exist */
	PB_BOOKALREADYEXISTS,				/* Tried to create a phonebook that exists already */
	PB_BOOKFULL,						/* Tried to add a record to a full phonebook */
	PB_RECDOESNOTEXIST,					/* Tried to access a record that does not exist */
	PB_INDEXINVALID						/* Index type is not valid for this operation */
} PB_RET;


/**********************************************
 * T_PB_TYPE
 *
 * Type of phonebook (storage type)
 *
 **********************************************/
 
typedef enum
{
	PB_TYPE_FFS = 0,
	PB_TYPE_SIM = 1
} T_PB_TYPE;


/**************************************************
 * T_PB_INFO
 *
 * Data structure to contain phonebook information.
 *
 **************************************************/

typedef struct
{
	SHORT			records_max;		/* Total number of records available */
	SHORT			records_used;		/* Number of records that are in use */
	SHORT			records_free;		/* Number of free records */
	SHORT			search_results;		/* Number of search results in search table */
}
T_PB_INFO;


/**************************************************
 * T_PB_ALPHA
 *
 * Phonebook string type
 *
 **************************************************/

typedef struct
{
	UBYTE			dcs;				/* Data coding scheme */
	USHORT			length;				/* Length of the alpha tag */
	USHORT			*data;				/* Alpha tag data */
}
T_PB_ALPHA;


/**************************************************
 * T_PB_RECORD
 *
 * Phonebook record
 *
 **************************************************/

typedef struct
{
	T_PB_ALPHA		alpha;				/* Alpha tag */
	UBYTE			*number;			/* Number - BCD format */
	UBYTE			ton_npi;
	void			*ext_data;			/* Extended data */
} T_PB_RECORD;


/**************************************************
 * T_PB_LIST
 *
 * List of records
 *
 **************************************************/

typedef struct
{
	SHORT			records_max;		/* Maximum number of items in list */
	SHORT			records_used;		/* Occupied items in list */
	T_PB_RECORD		*record;			/* Array of records */
}
T_PB_LIST;



/**********************************************
 * T_PB_INDEX
 *
 * Type of index list
 *
 **********************************************/
 
typedef enum
{
	INDEX_PHYSICAL = 0,					/* Direct mapping logical-physical */
	INDEX_NAME = 1,						/*Index list sorted by the name field*/
	INDEX_NUMBER = 2,					/*Index list sorted by the number field*/
	INDEX_SEARCH = 3,					/* Index list created by search */
	INDEX_DEFAULT_MAX					/*Anything above this is a user-specific index */
} T_PB_INDEX;
	

/**********************************************
 * T_PB_MATCH
 *
 * Result of matching operation
 *
 **********************************************/

typedef enum
{
	MATCH_DEFAULT,						/* No match made - use the default matching method */
	MATCH_EXACT,						/* The string matches a record exactly */
	MATCH_START,						/* The string is found at the start of a record */
	MATCH_FRAGMENT,						/* The string is found within a record */
	MATCH_NONE							/* The string does not match any record */
}
T_PB_MATCH;


/**********************************************
 * T_PB_COMPARE
 *
 * Result of comparison operation
 *
 **********************************************/

typedef enum
{
	COMPARE_DEFAULT,					/* No comparison made - use the default comparison method*/
	COMPARE_IDENTICAL,					/* The two records are identical */
	COMPARE_FIRSTBEFORE,				/* The first record should come before the second record*/
	COMPARE_FIRSTAFTER					/* The first record should come after the second record*/
}
T_PB_COMPARE;


/**********************************************
 * ATB PB FUNCTION PROTOTYPES
 *
 **********************************************/

PB_RET		ATB_pb_GetPhonebook(SHORT phonebook_id, T_PB_TYPE *current_type);
PB_RET		ATB_pb_Initialise(SHORT phonebook_id, T_PB_TYPE type, SHORT records_max, SHORT cache_max,
	SHORT alpha_max, SHORT number_max, SHORT ext_max);
PB_RET		ATB_pb_Exit(SHORT phonebook_id);
PB_RET		ATB_pb_Status(SHORT phonebook_id);
PB_RET		ATB_pb_Info(SHORT phonebook_id, T_PB_INFO *phonebook_info);
PB_RET		ATB_pb_ReadRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index, T_PB_RECORD *record);
/*a0393213 warnings removal-data type of 'index_type' changed from T_PB_TYPE to T_PB_INDEX for ATB_pb_WriteRec and ATB_pb_DeleteRec*/
PB_RET		ATB_pb_WriteRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index, T_PB_RECORD *record);
PB_RET		ATB_pb_DeleteRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index);
PB_RET		ATB_pb_Find (SHORT phonebook_id, T_PB_INDEX index_type, T_PB_RECORD *record, SHORT *new_log_index, T_PB_MATCH *match);
PB_RET		ATB_pb_ReadRecList(SHORT phonebook_id, T_PB_INDEX index_type, SHORT start_log_index, SHORT num_recs, T_PB_LIST *list);
PB_RET 		ATB_pb_Search(SHORT phonebook_id, T_PB_INDEX index_type, T_PB_RECORD *record, SHORT *recs_count);
PB_RET		ATB_pb_ConvIndex(SHORT phonebook_id, T_PB_INDEX src_index_type, SHORT log_index,
			T_PB_INDEX dest_index_type, SHORT *new_log_index);
PB_RET		ATB_pb_OK(SHORT phonebook_id);
void		ATB_pb_Error(SHORT phonebook_id, SHORT error_id);
T_PB_RECORD *ATB_pb_AllocRec(SHORT phonebook_id);
void		ATB_pb_FreeRec(SHORT phonebook_id, T_PB_RECORD *record);
T_PB_LIST	*ATB_pb_AllocRecList(SHORT phonebook_id, SHORT num_recs);
void		ATB_pb_FreeRecList(SHORT phonebook_id, T_PB_LIST *list);
void		ATB_pb_CopyRec(SHORT phonebook_id, T_PB_RECORD *dest_record, T_PB_RECORD *src_record);
void 		ATB_pb_CharToBCD(UBYTE *dest, char *src, int max_len);
void		ATB_pb_BCDToChar(char *dest, UBYTE *src, int max_len);
PB_RET	ATB_pb_SetPhonebook(SHORT phonebook_id, T_PB_TYPE current_type);

/**********************************************
 * GI FUNCTION PROTOTYPES
 *
 * Functions in GI called by ATB PB
 *
 **********************************************/

void		GI_pb_OK(SHORT phonebook_id, SHORT command_id, void *param);
void		GI_pb_Error(SHORT phonebook_id, SHORT command_id, SHORT error_id);
// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32
UBYTE*		GI_pb_MemAlloc(U32 size);
void		GI_pb_MemFree(UBYTE *mem, U32 size);
T_PB_COMPARE GI_pb_Compare (T_PB_RECORD *record1,T_PB_RECORD *record2, T_PB_INDEX index_type);
T_PB_MATCH	GI_pb_Match(T_PB_RECORD *record1,T_PB_RECORD *record2, T_PB_INDEX index_type);
SHORT*		GI_pb_GetTable (T_PB_INDEX index_type);


/**********************************************
 * FS FUNCTION PROTOTYPES
 *
 * Functions in FS called by ATB PB
 *
 **********************************************/

PB_RET	FS_pb_GetPhonebook(SHORT phonebook_id, T_PB_TYPE *current_type);
PB_RET	FS_pb_SetPhonebook(SHORT phonebook_id, T_PB_TYPE current_type);
PB_RET	FS_pb_Initialise(SHORT phonebook_id, T_PB_TYPE type, SHORT records_max, SHORT alpha_max, SHORT number_max, SHORT ext_max);
PB_RET	FS_pb_ReadRec(SHORT phonebook_id, SHORT phys_index, T_PB_RECORD *record);
PB_RET	FS_pb_WriteRec(SHORT phonebook_id, SHORT phys_index, T_PB_RECORD *record);
PB_RET	FS_pb_DeleteRec(SHORT phonebook_id, SHORT phys_index);
PB_RET	FS_pb_Finished(SHORT phonebook_id);
PB_RET	FS_pb_ReadTables(SHORT phonebook_id, SHORT *records_used, SHORT *name_table, SHORT *number_table);
PB_RET	FS_pb_WriteTables(SHORT phonebook_id, SHORT records_used, SHORT *name_table, SHORT *number_table);

#endif
