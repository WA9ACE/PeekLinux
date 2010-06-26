#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#include <stdio.h>
#include <string.h>

#include "mfw_mfw.h"
#include "mfw_sys.h"

#include "cus_aci.h"
#include "prim.h"
#include "pcm.h"

/* BEGIN ADD: Sumit : Req ID: : 31-Mar-2005*/
#ifndef NEPTUNE_BOARD
/* END ADD: Sumit : Req ID: : 31-Mar-2005*/
#include "ffs/ffs.h"
/* BEGIN ADD: Sumit : Req ID: : 31-Mar-2005*/
#else
#include "ffs.h"
#endif
/* END ADD: Sumit : Req ID: : 31-Mar-2005*/

#include "ffs_coat.h"
#include "ATBPb.h"
#include "ATBPbFS.h"

T_PB_FSDATA fsdata;

UBYTE empty_rec[2] = {0xFF,0};

EXTERN T_HANDLE         aci_handle;
#ifdef TI_PS_HCOMM_CHANGE
#define _hCommMMI  aci_handle
#else 
#define hCommMMI        aci_handle
#endif

/*******************************************************************************

 $Function:    	FS_pb_GetPhonebook

 $Description:	Checks to see if FFS phonebook is present, and if it is selected.  If it
 				is not present, creates info file with default parameters.

 $Returns:		PB_OK		Action completed OK.
				PB_EXCT		Action currently executing, callback awaited.
				PB_ERROR	Error.

 $Arguments:	phonebook_id	The phonebook identifier
 				current_type	Place to store type of phonebook selected.

*******************************************************************************/

PB_RET	FS_pb_GetPhonebook(SHORT phonebook_id, T_PB_TYPE *current_type)
{
	T_PB_FSINFO info;

	tracefunction("FS_pb_GetPhonebook");

	fsdata.info_file = FFS_open("/mmi/pbinfo", FFS_O_RDWR);

	if (fsdata.info_file<0)
	{
		/* File does not exist.  Attempt to create it. */

		fsdata.info_file = FFS_open("/mmi/pbinfo", FFS_O_RDWR | FFS_O_CREATE);
		if (fsdata.info_file<0)
		{
			trace("** Cannot create file - flash phonebook not available **");
			return PB_FILEWRITEFAIL;
		}

		/* Have opened file.  Set phonebook to SIM and store setting. */

		info.type_selected = PB_TYPE_SIM;
		info.records_max = PB_RECORDS_MAX;
		info.alpha_max = PB_ALPHATAG_MAX;
		info.number_max = PB_NUMBER_MAX;
		info.ext_max = PB_EXT_MAX;

		FFS_write(fsdata.info_file, (void *)&info, sizeof(T_PB_FSINFO));
		FFS_close(fsdata.info_file);
	}
	else
	{
		FFS_read(fsdata.info_file, (void *)&info, sizeof(T_PB_FSINFO));
		FFS_close(fsdata.info_file);
	}

	/* Check to make sure parameters are in acceptable range */

	if (info.records_max<0 || info.records_max>PB_RECORDS_UPPER_LIMIT)
		info.records_max = PB_RECORDS_UPPER_LIMIT;

	if (info.alpha_max<0 || info.alpha_max>PB_ALPHATAG_UPPER_LIMIT)
		info.alpha_max = PB_ALPHATAG_UPPER_LIMIT;

	if (info.number_max<0 || info.number_max>PB_NUMBER_UPPER_LIMIT)
		info.number_max = PB_NUMBER_UPPER_LIMIT;

	if (info.ext_max<0 || info.ext_max>PB_EXT_UPPER_LIMIT)
		info.ext_max = PB_EXT_UPPER_LIMIT;

	fsdata.records_max = info.records_max;
	fsdata.alpha_max = info.alpha_max;
	fsdata.number_max = info.number_max;
	fsdata.ext_max = info.ext_max;
	fsdata.record_size = info.alpha_max*sizeof(USHORT)+info.number_max/2+info.ext_max+sizeof(UBYTE);

	/* x0045876, 14-Aug-2006 (WR - enumerated type mixed with another type) */
	*current_type = (T_PB_TYPE) info.type_selected;

	return PB_OK;
}


/*******************************************************************************

 $Function:    	FS_pb_SetPhonebook

 $Description:	Select a phonebook

 $Returns:		PB_OK		Action completed OK.
				PB_EXCT		Action currently executing, callback awaited.
				PB_ERROR	Error.

 $Arguments:	phonebook_id	The phonebook identifier
 				current_type	New value for selected phonebook

*******************************************************************************/

PB_RET	FS_pb_SetPhonebook(SHORT phonebook_id, T_PB_TYPE current_type)
{
	T_PB_FSINFO info;
	T_FFS_SIZE size;
	T_FFS_RET fsret;

	tracefunction("FS_pb_SetPhonebook");

	/* Have opened file.  Read in current settings. */

	size = FFS_file_read("/mmi/pbinfo", (void *)&info, sizeof(T_PB_FSINFO));

	if (size<=0)
	{
		return PB_FILEREADFAIL;
	}

	info.type_selected = current_type;

	fsret = FFS_file_write("/mmi/pbinfo", (void *)&info, sizeof(T_PB_FSINFO), FFS_O_TRUNC);

	if (fsret!=EFFS_OK)
		return PB_FILEWRITEFAIL;

	return PB_OK;
}


/*******************************************************************************

 $Function:    	FS_pb_Initialise

 $Description:	Creates the necessary phonebook file(s) if they do not already exist.

 $Returns:		PB_OK			Action completed OK.
				PB_EXCT			Action currently executing, callback awaited.
				PB_FILEREADFAIL	File read encountered an error
				PB_FILEWRITEFAIL File write encountered an error


 $Arguments:	phonebook_id	The phonebook identifier
 				type			Type of phonebook.
				records_max		Indicates the maximum number of entries the
								phonebook can hold.
				alpha_max		Maximum size of unicode alpha tag in characters
				number_max		Maximum size of phone number in characters
 				ext_max			Maximum size of extended data in bytes

*******************************************************************************/

PB_RET	FS_pb_Initialise(SHORT phonebook_id, T_PB_TYPE type, SHORT records_max,
	SHORT alpha_max, SHORT number_max, SHORT ext_max)
{
	SHORT phys_index;
	char filename[30];
	SHORT fileIndex;
	SHORT recIndex;
	UBYTE newfile=FALSE;
	T_FFS_RET retvalue;
	T_FFS_SIZE sizevalue;
	UBYTE *blankRec;

	tracefunction("FS_pb_Initialise");

	fsdata.file = NULL;
	fsdata.fileID = -1;

	/* Create blank record */

	blankRec = (UBYTE *)mfwAlloc(fsdata.record_size);
	memset(blankRec, 0xFF, fsdata.record_size);

	/* Create files */

	trace("Creating files...");

	for (phys_index = 0; phys_index<records_max; phys_index++)
	{
		FS_file_GetIndex(phys_index, &fileIndex, &recIndex);

		/* Check if we're starting a new file */

		if (recIndex == 0 )
		{
			if (fileIndex!=0)
			{
				retvalue = FFS_close(fsdata.file);
				fsdata.file = NULL;
				fsdata.fileID = -1;
				trace_P2("Closing file %d, result %d",fsdata.file, retvalue);
			}

			FS_file_GetName(filename, phonebook_id, fileIndex);

			fsdata.file = FFS_open(filename, FFS_O_RDWR);
			trace_P2("Try to open file %s, result %d",filename, fsdata.file);
			if (fsdata.file<0)
			{
				fsdata.file = FFS_open(filename, FFS_O_RDWR | FFS_O_CREATE);
				fsdata.fileID = fileIndex;
				trace_P2("Create file %s, result %d",filename, fsdata.file);
				newfile = TRUE;
			}
			else
				newfile = FALSE;
		}

		if (newfile)
		{
			sizevalue = FFS_write(fsdata.file, (void *)blankRec, fsdata.record_size);
			trace_P2("Writing record %d, result %d",phys_index, sizevalue);
		}
	}

	/* Close the last file */
	retvalue = FFS_close(fsdata.file);
	trace_P2("Closing last file %d, result %d",fsdata.file, retvalue);

	fsdata.file = NULL;
	fsdata.fileID = -1;

	/* Get rid of blank record */

	mfwFree(blankRec, fsdata.record_size);

#ifdef BLOCKING
	return PB_OK;
#else
	return PB_EXCT;
#endif
}


/*******************************************************************************

 $Function:    	FS_pb_ReadRec

 $Description:	Reads a record from the physical position index.

 $Returns:		PB_OK			Action completed OK.
				PB_EXCT			Action currently executing, callback awaited.
				PB_FILEREADFAIL	File read encountered an error

 $Arguments:	phonebook_id	The phonebook identifier
				phys_index		Physical index of the record to read.
				record			Structure in which to store record data (allocated by
								caller).

*******************************************************************************/

PB_RET	FS_pb_ReadRec(SHORT phonebook_id, SHORT phys_index, T_PB_RECORD *record)
{
	SHORT charIndex;
	UBYTE *recordstore;
	SHORT recIndex;
	USHORT *unicode;

	tracefunction("FS_pb_ReadRec");

	/* Make sure record exists */

	if (phys_index>=fsdata.records_max)
		return PB_RECDOESNOTEXIST;

	/* Open the file */

	recIndex = FS_file_OpenForRec(phonebook_id, phys_index);

	/* Find the record in the file */

	FFS_seek(fsdata.file, fsdata.record_size*recIndex, FFS_SEEK_SET);

	/* Read the record */

	recordstore = (UBYTE *)mfwAlloc(fsdata.record_size);

	FFS_read(fsdata.file, (void *)recordstore, fsdata.record_size);

	/* Copy alpha tag */

	unicode = (USHORT *)recordstore;
	charIndex = 0;

	do
	{
		record->alpha.data[charIndex] = unicode[charIndex];
		if (unicode[charIndex]!=0)
			charIndex++;
	}
	while (unicode[charIndex]!=0 && unicode[charIndex]!=0xFFFF && charIndex<fsdata.alpha_max);


	record->alpha.length = charIndex;

	/* Copy number */

	memcpy((UBYTE *)record->number, (UBYTE *)&recordstore[fsdata.alpha_max*sizeof(USHORT)], fsdata.number_max/2);

	/* Copy ton/npi */

	record->ton_npi = recordstore[fsdata.alpha_max*sizeof(USHORT)+fsdata.number_max/2];

	mfwFree(recordstore, fsdata.record_size);

#ifdef BLOCKING
	return PB_OK;
#else
	return PB_EXCT;
#endif

}


/*******************************************************************************

 $Function:    	FS_pb_WriteRec

 $Description:	Writes a record to the physical position index.

 $Returns:		PB_OK			Action completed OK.
				PB_EXCT			Action currently executing, callback awaited.
				PB_FILEWRITEFAIL File write encountered an error

 $Arguments:	phonebook_id	The phonebook identifier
				phys_index		Physical index of the record to write.
				record			Record data to write to phonebook (allocated by caller).

*******************************************************************************/


PB_RET	FS_pb_WriteRec(SHORT phonebook_id, SHORT phys_index, T_PB_RECORD *record)
{
	SHORT charIndex;
	UBYTE *recordstore;
	SHORT recIndex;
	USHORT *unicode;
	
	/* Make sure record exists */

	if (phys_index>=fsdata.records_max)
		return PB_RECDOESNOTEXIST;

	/* Copy alpha tag */

	recordstore = (UBYTE *)mfwAlloc(fsdata.record_size);

	unicode = (USHORT *)recordstore;

	for (charIndex=0; charIndex<record->alpha.length; charIndex++)
	{
		unicode[charIndex] = record->alpha.data[charIndex];
	}

	unicode[charIndex] = 0;

	/* Copy number */

	memcpy(&recordstore[fsdata.alpha_max*sizeof(USHORT)], record->number, fsdata.number_max/2);

	/* Copy ton/npi */

	recordstore[fsdata.alpha_max*sizeof(USHORT)+fsdata.number_max/2] = record->ton_npi;

	/* Open the file */

	recIndex = FS_file_OpenForRec(phonebook_id, phys_index);

	/* Find the record in the file */

	FFS_seek(fsdata.file, fsdata.record_size*recIndex, FFS_SEEK_SET);

	/* Write record */

	FFS_write(fsdata.file, (void *)recordstore, fsdata.record_size);

	mfwFree(recordstore, fsdata.record_size);

#ifdef BLOCKING
	return PB_OK;
#else
	return PB_EXCT;
#endif

}


/*******************************************************************************

 $Function:    	FS_pb_DeleteRec

 $Description:	Deletes a record at a physical position index.

 $Returns:		PB_OK			Action completed OK.
				PB_EXCT			Action currently executing, callback awaited.
				PB_FILEWRITEFAIL File write encountered an error

 $Arguments:	phonebook_id	The phonebook identifier
				phys_index		Physical index of the record to delete.

*******************************************************************************/

PB_RET	FS_pb_DeleteRec(SHORT phonebook_id, SHORT phys_index)
{
	UBYTE *recordstore;
	SHORT recIndex;

	/* Make sure record exists */

	if (phys_index>=fsdata.records_max)
		return PB_RECDOESNOTEXIST;

	/* Allocate null buffer */

	recordstore = (UBYTE *)mfwAlloc(fsdata.record_size);
	memset(recordstore, 0xFF, fsdata.record_size);

	/* Open the file */

	recIndex = FS_file_OpenForRec(phonebook_id, phys_index);

	/* Find the record in the file */

	FFS_seek(fsdata.file, fsdata.record_size*recIndex, FFS_SEEK_SET);

	/* Write record */

	FFS_write(fsdata.file, (void *)recordstore, fsdata.record_size);

	mfwFree(recordstore, fsdata.record_size);

#ifdef BLOCKING
	return PB_OK;
#else
	return PB_EXCT;
#endif
}


/*******************************************************************************

 $Function:    	FS_pb_Finished

 $Description:	Operation is over; close the last file.

 $Returns:		PB_OK		Action completed OK.
				PB_ERROR	Error.

 $Arguments:	phonebook_id	The phonebook identifier


*******************************************************************************/

PB_RET	FS_pb_Finished(SHORT phonebook_id)
{
	
	if (fsdata.fileID!=-1 && fsdata.file)
	{
		FFS_close(fsdata.file);
	}

	fsdata.file = NULL;
	fsdata.fileID = -1;

	return PB_OK;
}


/*******************************************************************************

 $Function:    	FS_pb_ReadTables

 $Description:	Read in index table files

 $Returns:		PB_OK		Action completed OK.
				PB_ERROR	Error.

 $Arguments:	phonebook_id	The phonebook identifier
				records_used	Number of entries in phonebook
				name_table		Table of entries ordered by name
				number_table	Table of entries ordered by number

*******************************************************************************/

PB_RET	FS_pb_ReadTables(SHORT phonebook_id, SHORT *records_used, SHORT *name_table, SHORT *number_table)
{
	T_FFS_RET ret;
	T_FFS_FD file;
	T_FFS_SIZE size;

	UBYTE writing;

	tracefunction("FS_pb_ReadTables");

	/* Check if file wrote OK */

	writing = 2;	/* Dummy value to check for errors */
	size = FFS_file_read("/mmi/pbverify", (void *)&writing, sizeof(UBYTE));
	trace_P1("Try to read file /mmi/pbverify, size = %d", size);

	if (size<0)
	{
		ret = FFS_file_write("/mmi/pbverify", (void *)&writing, sizeof(UBYTE), FFS_O_CREATE);
		trace_P1("Try to create file /mmi/pbverify, ret = %d", ret);
	}

	trace_P1("Value of writing = %d", writing);

	if (writing==TRUE)		/* Operation was aborted while reading tables file */
	{
		trace("***ERROR - tables not written properly");
		return PB_FILEREADFAIL;
	}

	/* Read in tables file */

	file = FFS_open("/mmi/pbtables", FFS_O_RDWR);
	trace_P1("Try to open file /mmi/pbtables, result %d", file);

	if (file<0)
	{
		return PB_FILEREADFAIL;
	}

	FFS_read(file, (void *)records_used, sizeof(SHORT));
	FFS_read(file, (void *)name_table, fsdata.records_max*sizeof(SHORT));
	FFS_read(file, (void *)number_table, fsdata.records_max*sizeof(SHORT));

	FFS_close(file);

	return PB_OK;
}


/*******************************************************************************

 $Function:    	FS_pb_WriteTables

 $Description:	Update index table files

 $Returns:		PB_OK		Action completed OK.
				PB_FILEWRITEFAIL

 $Arguments:	phonebook_id	The phonebook identifier
				records_used	Number of entries in phonebook
				name_table		Table of entries ordered by name
				number_table	Table of entries ordered by number

*******************************************************************************/

PB_RET	FS_pb_WriteTables(SHORT phonebook_id, SHORT records_used, SHORT *name_table, SHORT *number_table)
{
	T_FFS_RET ret;
	T_FFS_FD file;
	UBYTE writing;

	tracefunction("FS_pb_WriteTables");

	/* Indicate that file writing is in progress */

	writing = TRUE;
	ret = FFS_file_write("/mmi/pbverify", (void *)&writing, sizeof(UBYTE), FFS_O_TRUNC);
	trace_P1("1. Try to write file /mmi/pbverify, result = %d", ret);

	/* Update tables file */

	file = FFS_open("/mmi/pbtables", FFS_O_RDWR);
	trace_P1("Try to open file /mmi/pbtables, result %d", file);

	if (file<0)
	{
		file = FFS_open("/mmi/pbtables", FFS_O_RDWR | FFS_O_CREATE);
		trace_P1("Create file /mmi/pbtables, result %d", file);
		if (file<0)
		{
			trace("** Cannot create file - flash phonebook not available **");
			return PB_FILEWRITEFAIL;
		}
	}

	FFS_write(file, (void *)&records_used, sizeof(SHORT));
	FFS_write(file, (void *)name_table, fsdata.records_max*sizeof(SHORT));
	FFS_write(file, (void *)number_table, fsdata.records_max*sizeof(SHORT));

	FFS_close(file);

	/* Indicate that file was written OK */

	writing = FALSE;
	ret = FFS_file_write("/mmi/pbverify", (void *)&writing, sizeof(UBYTE), FFS_O_TRUNC);
	trace_P1("2. Try to write file /mmi/pbverify, result = %d", ret);

	return PB_OK;
}


/*******************************************************************************

 $Function:    	FS_file_GetIndex

 $Description:	For a given physical index, return the file and record number.

 $Returns:		None

 $Arguments:	phys_index		The physical record index
 				fileIndex		Pointer to variable to store file index
 				recIndex		Pointer to variable to store record index


*******************************************************************************/

void FS_file_GetIndex(SHORT phys_index, SHORT *fileIndex, SHORT *recIndex)
{
	*fileIndex = phys_index/PB_RECS_PER_FILE;
	*recIndex = phys_index % PB_RECS_PER_FILE;
	return;
}

/*******************************************************************************

 $Function:    	FS_file_GetName

 $Description:	For a given file index, return the filename.

 $Returns:		None

 $Arguments:	filename		Pointer to string to store the filename
 				phonebook_id	The identifier of the phonebook
 				fileIndex		The index of the file


*******************************************************************************/

void FS_file_GetName(char *filename, SHORT phonebook_id, SHORT fileIndex)
{
	sprintf(filename, "/mmi/pb%d", fileIndex);
	return;
}

/*******************************************************************************

 $Function:    	FS_file_OpenForRec

 $Description:	Open the appropriate file to access a specific record

 $Returns:		None

 $Arguments:	phonebook_id	The phonebook identifier
 				phys_index		The physical record index


*******************************************************************************/

SHORT FS_file_OpenForRec(SHORT phonebook_id, SHORT phys_index)
{
	SHORT fileIndex;
	SHORT recIndex;
	char filename[30];

	/* Open file if it is not already open */

	FS_file_GetIndex(phys_index, &fileIndex, &recIndex);

	if (fsdata.fileID!=fileIndex || fsdata.file==NULL)
	{
		/* Close currently open file */
		if (fsdata.file!=NULL)
		{
			FFS_close(fsdata.file);
			fsdata.file = NULL;
			fsdata.fileID = -1;
		}

		FS_file_GetName(filename, phonebook_id, fileIndex);

		fsdata.file = FFS_open(filename, FFS_O_RDWR);
		trace_P2("Try to open file %s, result %d",filename, fsdata.file);
		fsdata.fileID = fileIndex;
	}

	return recIndex;
}

