/*********************************************
 * PB_RECS_PER_FILE
 * 
 * Specifies how many records should be stored in
 * each file on the flash.
 *********************************************/
 
#define PB_RECS_PER_FILE			10

/*********************************************
 * PB_xxx_UPPER_LIMIT
 * 
 * The absolute maximum size of various aspects
 * of the records stored on the file system.  Used
 * to keep values read in from file within a 
 * sensible range.
 *********************************************/
 
#define PB_RECORDS_UPPER_LIMIT		500
#define PB_ALPHATAG_UPPER_LIMIT		500
#define PB_NUMBER_UPPER_LIMIT		500
#define PB_EXT_UPPER_LIMIT			500


/**********************************************
 * T_PB_FSDATA	
 * 
 * Internal information for FS
 *********************************************/
 
typedef struct
{
	SHORT records_max;
	SHORT alpha_max;
	SHORT number_max;
	SHORT ext_max;
	SHORT record_size;
	T_FFS_FD info_file;
	SHORT fileID;
	T_FFS_FD file;
}
T_PB_FSDATA;

/**********************************************
 * T_PB_FSINFO
 * 
 * Information about phonebook -
 * e.g. is SIM or FFS selected ?
 *********************************************/
 
typedef struct
{
	UBYTE	type_selected;	/* The current selected phonebook */
	SHORT records_max;
	SHORT alpha_max;
	SHORT number_max;
	SHORT ext_max;
}
T_PB_FSINFO;

/**********************************************
 * LOCAL FUNCTION PROTOTYPES
 *
 *********************************************/
 
void FS_file_GetIndex(SHORT phys_index, SHORT *fileIndex, SHORT *recIndex);
void FS_file_GetName(char *filename, SHORT phonebook_id, SHORT fileIndex);
SHORT FS_file_OpenForRec(SHORT phonebook_id, SHORT phys_index);




