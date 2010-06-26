/**********************************************
 * T_PB_COMMAND
 *
 * Command ID of executing function
 *
 **********************************************/

typedef enum
{
	PB_INITIALISE,
	PB_READREC,
	PB_WRITEREC,
	PB_DELETEREC,
	PB_FIND,
	PB_READRECLIST,
	PB_SEARCH
}
T_PB_COMMAND;


/**********************************************
 * T_PB_STATUS
 *
 * Status of executing function
 *
 **********************************************/
 
typedef enum
{
	PB_STATUS_NONE,
	PB_STATUS_INIT,
	PB_STATUS_EXEC,
	PB_STATUS_COMPLETE
} T_PB_STATUS;

typedef struct
{
	T_PB_TYPE	type;			/* The type of phonebook */
	SHORT			records_max;	/* The maximum no. of records that can be stored */
	SHORT			phys_index;
	T_PB_RECORD *record;			/* Currently read record */
	UBYTE		first_record;	/* Flag that is true when the first record is being read */
}
T_PB_INITIALISE;

typedef struct
{
	SHORT log_index;
	T_PB_INDEX index_type;/*a0393213 warnings removal-T_PB_TYPE changed to T_PB_INDEX*/
	SHORT phys_index;
	T_PB_RECORD *record;
}
T_PB_WRITEREC;

typedef struct
{
	SHORT log_index;
	T_PB_INDEX index_type;/*a0393213 warnings removal-T_PB_TYPE changed to T_PB_INDEX*/
	SHORT phys_index;
}
T_PB_DELETEREC;

typedef union
{
	T_PB_INITIALISE	Initialise;
	T_PB_WRITEREC	WriteRec;
	T_PB_DELETEREC	DeleteRec;
}
T_PB_PARAM;

/**************************************************
 * T_PB_DATA
 *
 * Data structure to keep track of phonebook entries
 * and index tables.
 *
**************************************************/
 
typedef struct
{
	T_PB_TYPE		current_type;	/* Currently selected phonebook */
	SHORT 			records_max;	/* Total number of records available */
	SHORT 			records_used;	/* Number of records that are in use */
	SHORT			search_results;	/* Number of results for last search */
	
	SHORT			alpha_max;		/* Max size of alpha tag in bytes */
	SHORT			number_max;		/* Max size of phone number in bytes */
	SHORT			ext_max;		/* Max size of extended data in bytes */
	
	SHORT* 			name_table;		/* Index table, sorted by name.  Dynamically updated. */
	SHORT*			number_table;	/* Index table, sorted by phone number. Dynamically updated. */
	SHORT*			search_table;	/* Search table, contains matching elements.  Sorted by whichever field is searched.
									   Not dynamically updated.  */

	SHORT*			in_memory;		/* -1 if the record does not exist
									 * 0 if the record is not in RAM
									 * >0 - number of times record has been read */
	SHORT			cache_max;		/* Maximum number of records that can be stored in memory concurrently*/
	SHORT			cache_size;		/* Current size of the cache */
	T_PB_RECORD**	cache;			/* List of entries in cache */
	
	T_PB_COMMAND	command_id;		/* Currently processing command */
	T_PB_STATUS		status;			/* Status of current command */
	T_PB_PARAM		param;			/* Parameter storage for current command */
}
T_PB_DATA;


/* FUNCTION PROTOTYPES */
T_PB_DATA*		ATB_hnd_GetPbData(SHORT phonebook_id);
void			ATB_hnd_SetPbData(SHORT phonebook_id, T_PB_DATA *data);
PB_RET			ATB_status_Initialise(SHORT phonebook_id);
PB_RET			ATB_status_WriteRec(SHORT phonebook_id);
PB_RET			ATB_status_DeleteRec(SHORT phonebook_id);

PB_RET			ATB_index_Find (SHORT phonebook_id, T_PB_INDEX index_type, T_PB_RECORD *record, SHORT *new_log_index, T_PB_MATCH *match);
SHORT			*ATB_index_GetTable(SHORT phonebook_id, T_PB_INDEX index_type);
SHORT			ATB_index_GetPhysIndex(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index);
SHORT			ATB_index_GetLogIndex(SHORT phonebook_id, T_PB_INDEX index_type, SHORT phys_index);
void			ATB_index_AddRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT phys_index, T_PB_RECORD *record, SHORT *new_log_index);
void			ATB_index_RemoveRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index);

T_PB_MATCH 		ATB_index_Match(T_PB_RECORD *record1, T_PB_RECORD *record2, T_PB_INDEX index_type);
T_PB_COMPARE	ATB_index_Compare(T_PB_RECORD *record1, T_PB_RECORD *record2, T_PB_INDEX index_type);
T_PB_MATCH	ATB_alpha_Match(T_PB_ALPHA *alpha1, T_PB_ALPHA *alpha2);/*a0393213 warnings removal-return type changed from T_PB_COMPARE*/
T_PB_COMPARE	ATB_alpha_Compare(T_PB_ALPHA *Alpha_1, T_PB_ALPHA *Alpha_2);
UBYTE			ATB_num_Digit(UBYTE *num, SHORT numIndex);
T_PB_MATCH		ATB_num_Match(UBYTE *num1, UBYTE *num2);
T_PB_COMPARE	ATB_num_Compare(UBYTE *num1, UBYTE *num2);
void			ATB_mem_UpdateCache(SHORT phonebook_id, SHORT phys_index, T_PB_RECORD *record);
PB_RET			ATB_mem_ReadRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index, T_PB_RECORD *record);
