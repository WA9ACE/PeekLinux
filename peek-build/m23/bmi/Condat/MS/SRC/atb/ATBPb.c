/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	                                                      
 $Project code:	                                                           
 $Module:		
 $File:		    ATBPb.c
 $Revision:		                                                      
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		                                                          
                                                                               
********************************************************************************
                                                                              
 Description:
    
                        
********************************************************************************

 $History: ATBPb.c

    Jan 18, 2004 REF: CRR MMI-SPR-25332 xnkulkar
    Description: Moving entries from Sim Card to phone memory fails. 
    Solution: When entries are moved, corresponding updations are done in cache as well.
	
	   
 $End

*******************************************************************************/

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

#include "cus_aci.h"
#include "prim.h"
#include "pcm.h"

#include "ATBPb.h"
#include "ATBPb_i.h"

/* Global data for phonebook */

static T_PB_DATA *phonebook[PB_PHONEBOOKS_MAX] = {0};


/*******************************************************************************

 $Function:    	ATB_pb_GetPhonebook
 
 $Description:	Checks to see which file system is selected

 $Returns:		PB_OK					Action completed OK.
				PB_FILEWRITEFAIL		File write encountered an error

 $Arguments:	phonebook_id	The phonebook identifier
 				current_type	Place to store type of phonebook selected.
 
*******************************************************************************/

PB_RET	ATB_pb_GetPhonebook(SHORT phonebook_id, T_PB_TYPE *current_type)
{
	PB_RET 		result;
	T_PB_DATA	*data	= ATB_hnd_GetPbData(phonebook_id);

	tracefunction("ATB_pb_GetPhonebook");

	if (data)
	{
		*current_type = data->current_type;
		return PB_OK;
	}
		
	result = FS_pb_GetPhonebook(phonebook_id, current_type);

	return result;
}


/*******************************************************************************

 $Function:    	ATB_pb_SetPhonebook
 
 $Description:	Select file system

 $Returns:		PB_OK					Action completed OK.
				PB_FILEREADFAIL			File read encountered an error

 $Arguments:	phonebook_id	The phonebook identifier
 				current_type	Type of phonebook selected.
 
*******************************************************************************/

PB_RET	ATB_pb_SetPhonebook(SHORT phonebook_id, T_PB_TYPE current_type)
{
	PB_RET 		result;
	T_PB_DATA	*data	= ATB_hnd_GetPbData(phonebook_id);

	tracefunction("ATB_pb_SetPhonebook");
	
	if (data)
		data->current_type = current_type;
	
	result = FS_pb_SetPhonebook(phonebook_id, current_type);

	return result;
}

		
/*******************************************************************************

 $Function:    	ATB_pb_Initialise
 
 $Description:	Initialise an instance of phonebook.  Dynamically allocates a record
 				cache in RAM.  Creates the necessary file(s) if they do not already exist.
 				Sorts the phonebook by name and by number, creating the appropriate
 				index tables.
				IMPORTANT: if the file reading is non-blocking - i.e. if a request to read
				a file does not return with an answer straight away, but the system
				awaits a callback - then "cache_max" must equal "records_max".  This
				ensures that all records are stored in RAM and can be retrieved without
				waiting for a response.  If the file reading is blocking, then cache_max
				can be smaller than records_max.

				The memory allocated by this operation will not be freed until
				ATB_pb_Exit() is called.

 $Returns:		PB_OK					Action completed OK.
				PB_EXCT					Action currently executing, callback awaited.
										GI_pb_OK will be called if successful,
										GI_pb_Error otherwise.
				PB_BUSY					Failed, phonebook is busy.
				PB_FILEREADFAIL			File read encountered an error
				PB_FILEWRITEFAIL		File write encountered an error
				PB_BOOKALREADYEXISTS 	Tried to initialise a phonebook that already
										exists

 $Arguments:	phonebook_id	The phonebook identifier
				type			Type of phonebook.
				records_max		Indicates the maximum number of entries the
								phonebook can hold.
				cache_max		Indicates the maximum number of records that the
								PB will store concurrently in RAM.
				alpha_max		Maximum size of unicode alpha tag in characters
				number_max		Maximum size of phone number in digits
				ext_max			Maximum size of extended data in bytes
								
*******************************************************************************/

PB_RET	ATB_pb_Initialise(SHORT phonebook_id, T_PB_TYPE type, SHORT records_max, SHORT cache_max,
	SHORT alpha_max, SHORT number_max, SHORT ext_max)
{
	T_PB_DATA *data;
	
	tracefunction("ATB_pb_Initialise()");
	
	/* Check to see if this phonebook has already been allocated */
	
	if (ATB_hnd_GetPbData(phonebook_id)!=NULL)
	{
		trace("* ERROR * - Phonebook already exists");
		GI_pb_Error(phonebook_id, PB_INITIALISE, PB_BOOKALREADYEXISTS);
		return PB_BOOKALREADYEXISTS;
	}

	/* Allocate memory for phonebook data */

	data = (T_PB_DATA *)GI_pb_MemAlloc(sizeof(T_PB_DATA));
	ATB_hnd_SetPbData(phonebook_id, data);
	
	data->records_max = records_max;
	data->records_used = 0;
	data->search_results = 0;

	data->alpha_max = alpha_max;
	data->number_max = number_max;
	data->ext_max = ext_max;
	
	/* Allocate memory for index tables */
	
	data->name_table = (SHORT *)GI_pb_MemAlloc(records_max*sizeof(SHORT));
	memset(data->name_table, 0, records_max*sizeof(SHORT));
	data->number_table = (SHORT *)GI_pb_MemAlloc(records_max*sizeof(SHORT));
	memset(data->number_table, 0, records_max*sizeof(SHORT));
	data->search_table = (SHORT *)GI_pb_MemAlloc(records_max*sizeof(SHORT));
	memset(data->search_table, 0, records_max*sizeof(SHORT));
	data->in_memory = (SHORT *)GI_pb_MemAlloc(records_max*sizeof(SHORT));
	memset(data->in_memory, PB_EMPTY_RECORD, records_max*sizeof(SHORT));

	/* Allocate memory for cache */

	data->cache_max = cache_max;
	data->cache_size = 0;
	data->cache	= (T_PB_RECORD **)GI_pb_MemAlloc(records_max*sizeof(T_PB_RECORD *));
	memset(data->cache, 0, records_max*sizeof(T_PB_RECORD *));
	
	/* Set up command */
	
	data->command_id = PB_INITIALISE;
	data->status	= PB_STATUS_INIT;
	data->param.Initialise.type = type;
	data->param.Initialise.records_max = records_max;
	
	return ATB_status_Initialise(phonebook_id);
}

/* Status function for Initialise */

PB_RET ATB_status_Initialise(SHORT phonebook_id)
{
	T_PB_DATA		*data		= ATB_hnd_GetPbData(phonebook_id);
	T_PB_INITIALISE	*Initialise;
	SHORT				blocking;
	PB_RET			result=PB_ERROR;

	/* Ensure that phonebook exists */
	if (!data)
	{
		trace("**ERROR** Phonebook does not exist");
		return PB_BOOKDOESNOTEXIST;
	}
	
	Initialise = &data->param.Initialise;
	blocking = TRUE;
	while (blocking)
	{
		switch(data->status)
		{
			case PB_STATUS_INIT:
				trace("ATB_status_Initialise: INIT");
				result = FS_pb_Initialise(phonebook_id, Initialise->type, Initialise->records_max,
					data->alpha_max, data->number_max, data->ext_max);
				
				/* Select next state */
				
				data->status = PB_STATUS_EXEC;
				Initialise->phys_index = 0;
				Initialise->first_record = TRUE;
				Initialise->record = ATB_pb_AllocRec(phonebook_id);
				
				/* Allow exit from function */
				
				if (result!=PB_OK)
					blocking = FALSE;
				break;

			case PB_STATUS_EXEC:
				trace("ATB_status_Initialise: EXEC");

				/* Try to read in index tables from file system.  If we can't find them, the
				 * records will be read in and sorted. */

				if (Initialise->first_record)
				{
					result = FS_pb_ReadTables(phonebook_id, &data->records_used, data->name_table, data->number_table);
					if (result==PB_OK)
					{
						/* Escape to PB_STATUS_COMPLETE */
						result = PB_OK;
						data->status = PB_STATUS_COMPLETE;
						break;
					}
				}
				/* If we've already read a record, and it's an existing record, add it to the
				 * index tables and to the cache */
				 
				else
				{	
					if (Initialise->record->alpha.data[0]!=(USHORT)0xFFFF)
					{
						ATB_mem_UpdateCache(phonebook_id, Initialise->phys_index, Initialise->record);
						ATB_index_AddRec(phonebook_id, INDEX_NAME, Initialise->phys_index, Initialise->record, NULL);
						ATB_index_AddRec(phonebook_id, INDEX_NUMBER, Initialise->phys_index, Initialise->record, NULL);
						data->records_used++;
					}

					Initialise->phys_index++;
				}

				/* Start processing after first record is read */
				
				Initialise->first_record = FALSE;
				
				/* If we haven't just read the last record, read the next one */
				
				if (Initialise->phys_index<data->records_max)
				{
					result = FS_pb_ReadRec(phonebook_id, Initialise->phys_index, Initialise->record);
				}
				else
				{
					/* Set next state as finished */
					result = PB_OK;
					data->status = PB_STATUS_COMPLETE;
				}


				/* Allow exit from function */
				
				if (result!=PB_OK)
					blocking = FALSE;
				break;

			case PB_STATUS_COMPLETE:
				trace("ATB_status_Initialise: COMPLETE");
				blocking = FALSE;

				/* Close file */

				FS_pb_Finished(phonebook_id);
				FS_pb_WriteTables(phonebook_id, data->records_used, data->name_table, data->number_table);
				
				/* Free the allocated record */

				ATB_pb_FreeRec(phonebook_id, Initialise->record);
				
				/* Notify the GI of success */
				GI_pb_OK(phonebook_id, data->command_id, NULL);
				result = PB_OK;
				data->status = PB_STATUS_NONE;
				break;
		}
	}

	return result;
}


/*******************************************************************************

 $Function:    	ATB_pb_Exit
 
 $Description:	Frees memory associated with phonebook.  To be called when the
 				phonebook is no longer required, or at shutdown.

 $Returns:		PB_OK		Action completed OK.

 $Arguments:	phonebook_id	The phonebook identifier
 
*******************************************************************************/

PB_RET ATB_pb_Exit(SHORT phonebook_id)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT phys_index;

	trace("ATB_pb_Exit");

	/* Ensure that phonebook exists */
	if (!data)
	{
		trace("**ERROR** Phonebook does not exist");
		return PB_BOOKDOESNOTEXIST;
	}
	
	for (phys_index = 0; phys_index<data->records_max; phys_index++)
	{
		if (data->cache[phys_index]!=NULL)
			ATB_pb_FreeRec(phonebook_id, data->cache[phys_index]);
	}

	GI_pb_MemFree((UBYTE *)data->cache, data->records_max*sizeof(T_PB_RECORD *));
	GI_pb_MemFree((UBYTE *)data->in_memory, data->records_max*sizeof(SHORT));
	GI_pb_MemFree((UBYTE *)data->search_table, data->records_max*sizeof(SHORT));
	GI_pb_MemFree((UBYTE *)data->number_table, data->records_max*sizeof(SHORT));
	GI_pb_MemFree((UBYTE *)data->name_table, data->records_max*sizeof(SHORT));
	
	GI_pb_MemFree((UBYTE *)data, sizeof(T_PB_DATA));

	ATB_hnd_SetPbData(phonebook_id, NULL);	
	return PB_OK;
}


/*******************************************************************************

 $Function:    	ATB_pb_Status
 
 $Description:	Returns PB_OK if the phonebook is ready for a new command,
 				or PB_BUSY if the phonebook is busy.

 $Returns:		PB_OK		All tasks completed, phonebook ready.
 				PB_BUSY		Commands currently executing.

 $Arguments:	phonebook_id	The phonebook identifier
 
*******************************************************************************/

PB_RET	ATB_pb_Status(SHORT phonebook_id)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	PB_RET	result;

	/* Ensure that phonebook exists */
	if (!data)
	{
		trace("**ERROR** Phonebook does not exist");
		return PB_BOOKDOESNOTEXIST;
	}

	// Jan 18, 2004 REF: CRR MMI-SPR-25332 xnkulkar
    	// Description: Moving entries from Sim Card to phone memory fails. 
    	// Solution: result should be set as PB_OK when PB_STATUS_COMPLETE is true also.
	if ((data->status==PB_STATUS_NONE) ||(data->status==PB_STATUS_COMPLETE))
		result = PB_OK;
	else
		result = PB_BUSY;
	
	return result;
}


/*******************************************************************************

 $Function:    	ATB_pb_Info
 
 $Description:	Returns information about the phonebook, which will be returned
 				in the data structure pointed to by phonebook_info.  The caller must
 				allocate the T_PB_INFO structure.

 $Returns:		PB_OK		Action completed OK.

 $Arguments:	phonebook_id	The phonebook identifier
				phonebook_info	Pointer to data structure to contain phonebook
								information (allocated by caller).
 
*******************************************************************************/

PB_RET	ATB_pb_Info(SHORT phonebook_id, T_PB_INFO *phonebook_info)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);

	/* Ensure that phonebook exists */
	if (!data)
	{
		trace("**ERROR** Phonebook does not exist");
		return PB_BOOKDOESNOTEXIST;
	}
	
	phonebook_info->records_used = data->records_used;
	phonebook_info->records_max = data->records_max;
	phonebook_info->records_free = data->records_max - data->records_used;
	phonebook_info->search_results = data->search_results;
	
	return PB_OK;
}


/*******************************************************************************

 $Function:    	ATB_pb_ReadRec
 
 $Description:	Reads a record from the logical position index.  The record information
 				will be returned in the data structure pointed to by record.  The 
				T_PB_RECORD structure must be allocated by the user.

				This function calls the ATB_mem_ReadRec function, with an additional
				check to make sure no other commands are currently executing.

 $Returns:		PB_OK				Action completed OK.
 				PB_BUSY				Failed, phonebook is busy.
				PB_FILEREADFAIL		File read encountered an error
				PB_RECDOESNOTEXIST	Tried to access a record that does not exist

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index list from which the record is to be read
				log_index		Logical index in the index list of the record to read
				record			Structure in which to store record data
 
*******************************************************************************/

PB_RET	ATB_pb_ReadRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index, T_PB_RECORD *record)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	PB_RET result;

	tracefunction("ATB_pb_ReadRec");

	/* Ensure that phonebook exists */
	if (!data)
	{
		trace("**ERROR** Phonebook does not exist");
		return PB_BOOKDOESNOTEXIST;
	}
	
	/* Make sure phonebook is not busy */
	
	if (ATB_pb_Status(phonebook_id)==PB_BUSY)
	{
		trace("* ERROR * - Phonebook is busy");
		result = PB_BUSY;
	}
	else
	{
		/* Read the record from cache or from file */
		
		result = ATB_mem_ReadRec(phonebook_id, index_type, log_index, record);
	}

	/* Send success/failure info to the GI */
	
	if (result==PB_OK)
		GI_pb_OK(phonebook_id, PB_READREC, NULL);
	else if (result!=PB_EXCT)
		GI_pb_Error(phonebook_id, PB_READREC, result);

	/* Close any open file */
	
	FS_pb_Finished(phonebook_id);
	
	return result;
}


/*******************************************************************************

 $Function:    	ATB_pb_WriteRec
 
 $Description:	Writes a record to the logical position index.  If the index is
 				PB_NEW_RECORD, a new record is added, otherwise the previous
 				record at that position is overwritten. The name and number index lists
 				will be updated with the new entry in the appropriate place in each.
				GI_pb_OK will be called to confirm that the write completed successfully,
				otherwise GI_pb_Error will be called with an error code.
				Since the order of the records may change due to sorting, GI_pb_OK
				will receive a pointer to an integer new_log_index, which will store the
				new position of the record in the chosen index list.  This is allocated
				by and will be destroyed by the phonebook, so must be copied by the
				GI before function return.

 $Returns:		PB_OK				Action completed OK.
				PB_EXCT				Action currently executing, callback awaited.
									GI_pb_OK will be called if successful,
									GI_pb_Error otherwise.
									GI_pb_OK will receive a pointer to the following:
									SHORT new_log_index - The new position of the
 									record in the chosen index list
 				PB_BUSY				Failed, phonebook is busy.
				PB_FILEWRITEFAIL	File write encountered an error
				PB_RECDOESNOTEXIST	(Overwriting only) Tried to access a record that
									does not exist

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index list of the record to write
				log_index		Logical index in the index list of the record to write
				record			Pointer to record data to write to phonebook
								(allocated by caller)
 
*******************************************************************************/
/*a0393213 warnings removal-second parameter type changed from T_PB_TYPE to T_PB_INDEX*/
PB_RET ATB_pb_WriteRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index, T_PB_RECORD *record)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT phys_index;

	tracefunction("ATB_pb_WriteRec");

	/* Ensure that phonebook exists */
	if (!data)
	{
		trace("**ERROR** Phonebook does not exist");
		return PB_BOOKDOESNOTEXIST;
	}
	
	/* Make sure phonebook is not busy */
	
	if (ATB_pb_Status(phonebook_id)==PB_BUSY)
	{
		trace("* ERROR * - Phonebook is busy");
		GI_pb_Error(phonebook_id, PB_WRITEREC, PB_BUSY);
		return PB_BUSY;
	}

	/* Are we adding a new record, or overwriting an old one? */
	
	if (log_index==PB_NEW_RECORD)
	{
		/* Is phonebook full? */

		if (data->records_used==data->records_max)
		{
			GI_pb_Error(phonebook_id, PB_WRITEREC, PB_BOOKFULL);
			return PB_BOOKFULL;
		}
		
		/* Find an empty physical record */
		for (phys_index=0; phys_index<data->records_max; phys_index++)
		{
			if (data->in_memory[phys_index]==PB_EMPTY_RECORD)
				break;
		}
	}
	else
	{
		/* Check that record exists */

		if (log_index<0
			|| (log_index>=data->records_used && (index_type==INDEX_NAME || index_type==INDEX_NUMBER))
			|| (index_type==INDEX_SEARCH && log_index>data->search_results)
			|| (index_type==INDEX_PHYSICAL && log_index>data->records_max))
		{
			GI_pb_Error(phonebook_id, PB_WRITEREC, PB_RECDOESNOTEXIST);
			return PB_RECDOESNOTEXIST;
		}
		
		phys_index = ATB_index_GetPhysIndex(phonebook_id, index_type, log_index);
	}

	/* Set up command */
	
	data->command_id 	= PB_WRITEREC;
	data->status		= PB_STATUS_INIT;

	data->param.WriteRec.log_index = log_index;
	data->param.WriteRec.index_type = index_type;
	data->param.WriteRec.phys_index = phys_index;
	data->param.WriteRec.record = record;
	
	return ATB_status_WriteRec(phonebook_id);
}

/* Status function for WriteRec */

PB_RET ATB_status_WriteRec(SHORT phonebook_id)
{
	T_PB_DATA		*data		= ATB_hnd_GetPbData(phonebook_id);
	T_PB_WRITEREC	*WriteRec	= &data->param.WriteRec;
	SHORT				blocking;
	PB_RET			result=PB_ERROR;
	SHORT				name_log_index;
	SHORT				number_log_index;
	SHORT				*log_index_return;

	blocking = TRUE;
	while (blocking)
	{
		switch(data->status)
		{
			case PB_STATUS_INIT:
				result = FS_pb_WriteRec(phonebook_id, WriteRec->phys_index, WriteRec->record);
				
				/* Select next state */
				
				data->status = PB_STATUS_COMPLETE;
				
				/* Allow exit from function */
				
				if (result!=PB_OK)
					blocking = FALSE;
				break;

			case PB_STATUS_COMPLETE:

				if (WriteRec->log_index!=PB_NEW_RECORD)
				{
					name_log_index = ATB_index_GetLogIndex(phonebook_id, INDEX_NAME, WriteRec->phys_index);
					ATB_index_RemoveRec(phonebook_id, INDEX_NAME, name_log_index);
					number_log_index = ATB_index_GetLogIndex(phonebook_id, INDEX_NUMBER, WriteRec->phys_index);
					ATB_index_RemoveRec(phonebook_id, INDEX_NUMBER, number_log_index);
					data->records_used--;
				}

				/* Indicate that record is new, then add to the cache */
				
				data->in_memory[WriteRec->phys_index] = 0;
				ATB_mem_UpdateCache(phonebook_id, WriteRec->phys_index, WriteRec->record); 

				/* Add the record to the index lists */
				
				ATB_index_AddRec(phonebook_id, INDEX_NAME, WriteRec->phys_index, WriteRec->record, &name_log_index);
				ATB_index_AddRec(phonebook_id, INDEX_NUMBER, WriteRec->phys_index, WriteRec->record, &number_log_index);
				data->records_used++;
				
				blocking = FALSE;

				/* Close any open file */
				
				FS_pb_Finished(phonebook_id);
				FS_pb_WriteTables(phonebook_id, data->records_used, data->name_table, data->number_table);
	
				/* Notify the GI of success */
				log_index_return = NULL;
				if (WriteRec->index_type==INDEX_NAME)
					log_index_return = &name_log_index;
				if (WriteRec->index_type==INDEX_NUMBER)
					log_index_return = &number_log_index;
				
				GI_pb_OK(phonebook_id, data->command_id, log_index_return);
				result = PB_OK;
				data->status = PB_STATUS_NONE;
				break;
		}
	}

	/* Report any errors to the GI */
	
	if (result!=PB_OK && result!=PB_EXCT)
	{
		GI_pb_Error(phonebook_id, PB_WRITEREC, result);	
	}
	
	return result;
}


/*******************************************************************************

 $Function:    	ATB_pb_DeleteRec
 
 $Description:	Deletes a record at a logical position index.
				GI_pb_OK will be called to confirm that the delete completed successfully,
				otherwise GI_pb_Error will be called with an error code.

 $Returns:		PB_OK				Action completed OK.
				PB_EXCT				Action currently executing, callback awaited.
									GI_pb_OK will be called if successful,
									GI_pb_Error otherwise.
				PB_BUSY				Failed, phonebook is busy.
				PB_FILEWRITEFAIL	File write encountered an error
				PB_RECDOESNOTEXIST	Tried to access a record that does not exist

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index list of the record to delete
				log_index		Logical index in the index list of the record to delete

*******************************************************************************/

PB_RET ATB_pb_DeleteRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);

	tracefunction("ATB_pb_DeleteRec");
	
	/* Ensure that phonebook exists */
	if (!data)
	{
		trace("**ERROR** Phonebook does not exist");
		return PB_BOOKDOESNOTEXIST;
	}

	/* Make sure phonebook is not busy */
	
	if (ATB_pb_Status(phonebook_id)==PB_BUSY)
	{
		trace("* ERROR * - Phonebook is busy");
		GI_pb_Error(phonebook_id, PB_DELETEREC, PB_BUSY);
		return PB_BUSY;
	}
	
	/* Check that record exists */

	if (log_index<0
		|| (log_index>=data->records_used && (index_type==INDEX_NAME || index_type==INDEX_NUMBER))
		|| (index_type==INDEX_SEARCH && log_index>data->search_results)
		|| (index_type==INDEX_PHYSICAL && log_index>data->records_max))
	{
		GI_pb_Error(phonebook_id, PB_DELETEREC, PB_RECDOESNOTEXIST);
		return PB_RECDOESNOTEXIST;
	}
	
	/* Set up the command */
	
	data->command_id = PB_DELETEREC;
	data->status	= PB_STATUS_INIT;

	data->param.DeleteRec.log_index = log_index;
	data->param.DeleteRec.index_type = index_type;
	data->param.DeleteRec.phys_index = ATB_index_GetPhysIndex(phonebook_id, index_type, log_index);
	
	return ATB_status_DeleteRec(phonebook_id);
}

/* Status function for DeleteRec */

PB_RET ATB_status_DeleteRec(SHORT phonebook_id)
{
	T_PB_DATA		*data		= ATB_hnd_GetPbData(phonebook_id);
	T_PB_DELETEREC	*DeleteRec	= &data->param.DeleteRec;
	SHORT				blocking;
	PB_RET			result=PB_ERROR;
	SHORT				name_log_index;
	SHORT				number_log_index;

	blocking = TRUE;
	while (blocking)
	{
		switch(data->status)
		{
			case PB_STATUS_INIT:
				result = FS_pb_DeleteRec(phonebook_id, DeleteRec->phys_index);
				
				/* Select next state */
				
				data->status = PB_STATUS_COMPLETE;
				
				/* Allow exit from function */
				
				if (result!=PB_OK)
					blocking = FALSE;
				break;

			case PB_STATUS_COMPLETE:
				name_log_index = ATB_index_GetLogIndex(phonebook_id, INDEX_NAME, DeleteRec->phys_index);
				ATB_index_RemoveRec(phonebook_id, INDEX_NAME, name_log_index);
				number_log_index = ATB_index_GetLogIndex(phonebook_id, INDEX_NUMBER, DeleteRec->phys_index);
				ATB_index_RemoveRec(phonebook_id, INDEX_NUMBER, number_log_index);
				data->records_used--;
				data->in_memory[DeleteRec->phys_index] = -1;

				// Jan 18, 2004 REF: CRR MMI-SPR-25332 xnkulkar
    				// Description: Moving entries from Sim Card to phone memory fails. 
    				// Solution: When entries are moved, corresponding updations are done in cache as well.
				if(data->cache[DeleteRec->phys_index]!=NULL)
				{
             				ATB_pb_FreeRec(phonebook_id, data->cache[DeleteRec->phys_index]);
	 				data->cache[DeleteRec->phys_index]=NULL;
	 				data->cache_size--;
				}

				/* Close any open file */
	
				FS_pb_Finished(phonebook_id);
				FS_pb_WriteTables(phonebook_id, data->records_used, data->name_table, data->number_table);
					
				blocking = FALSE;
				
				/* Notify the GI of success */

				GI_pb_OK(phonebook_id, data->command_id, NULL);
				result = PB_OK;
				data->status = PB_STATUS_NONE;
				break;
		}
	}

	/* Report any errors to the GI */
	
	if (result!=PB_OK && result!=PB_EXCT)
	{
		GI_pb_Error(phonebook_id, PB_DELETEREC, result);	
	}
	
	return result;
}


/*******************************************************************************

 $Function:    	ATB_pb_Find
 
 $Description:	Find the nearest match to the supplied record.  Only the index_type
 				specified will be considered: if the index_type is INDEX_NAME, then the
 				name only will be matched; if INDEX_NUMBER, then just the number.
				The new_log_index parameter should be a pointer to a user allocated
				integer.  This will return the logical index of the closest match in the
				index list specified.  This index either corresponds to a perfect match,
				or to the record that would appear just after the provided record in the
				list.  For example, if the list consisted of record 0, Alice and record 1,
				Carol, then a search for Bob would return 1.  A search for a record
				that would occur before the first record always returns 0.  A search for
				a record that would occur after the last record returns the index of the
				last record +1.
				The "match" parameter should be a pointer to a user allocated variable
				of type T_PB_MATCH.  This returns information about how closely the
				found entry matches the record.  In the example above, a search for
				Bob would return 1 and MATCH_NONE; a search for Ca would return 1
				and MATCH_START, and a search for Carol would return 1 and
				MATCH_EXACT. Rarely, match may be MATCH_FRAGMENT: this can be
				considered equivalent to MATCH_NONE in this context.

 $Returns:		PB_OK				Action completed OK.
 				PB_BUSY				Failed, phonebook is busy.
				PB_FILEREADFAIL		File read encountered an error
				PB_RECDOESNOTEXIST	Tried to access a record that does not exist
									(should never happen)

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index_type of the provided record that is to be
								searched for, and the index list that the resulting
								new_log_index pertains to
				record			The record whose index_type is to be matched
				new_log_index	Returns with the logical index of the closest matching
								record
				match			Specifies how good the resulting match was
								(MATCH_NONE, MATCH_START, MATCH_FRAGMENT
								or MATCH_EXACT).
 
*******************************************************************************/


PB_RET ATB_pb_Find (SHORT phonebook_id, T_PB_INDEX index_type, T_PB_RECORD *record, SHORT *new_log_index, T_PB_MATCH *match)
{
	PB_RET result;

	tracefunction("ATB_pb_Find");
	
	result = ATB_index_Find(phonebook_id, index_type, record, new_log_index, match);

	/* Close any open file */
	
	FS_pb_Finished(phonebook_id);
		
	/* Send success/failure info to the GI */
	
	if (result==PB_OK)
		GI_pb_OK(phonebook_id, PB_FIND, NULL);
	else if (result!=PB_EXCT)
		GI_pb_Error(phonebook_id, PB_FIND, result);

	return result;
}


/*******************************************************************************

 $Function:    	ATB_pb_ReadRecList
 
 $Description:	Fetches record information of sequential records in an index list, starting
 				at logical record start_log_index and fetching recs_count records in total.
 				The information will be stored in the caller allocated array of records
 				provided.

 $Returns:		PB_OK				Action completed OK.
				PB_BUSY				Failed, phonebook is busy.
				PB_FILEREADFAIL		File read encountered an error
				PB_RECDOESNOTEXIST	Tried to access a record that does not exist

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index list to use
				start_log_index	Logical index in the index list of the first record to
								read
				recs_count		Total number of sequential records to read. 
				record			Array of entries in which to store the fetched records.
								The array must be recs_count in size. (Caller allocated).
 
*******************************************************************************/

PB_RET ATB_pb_ReadRecList(SHORT phonebook_id, T_PB_INDEX index_type, SHORT start_log_index, SHORT num_recs, T_PB_LIST *list)
{
	PB_RET result;
	SHORT log_index;

	tracefunction("ATB_pb_ReadRecList");
	
	list->records_used = 0;

	/* Make sure phonebook is not busy */
	
	if (ATB_pb_Status(phonebook_id)==PB_BUSY)
	{
		trace("* ERROR * - Phonebook is busy");
			/* Report any errors to the GI */
		GI_pb_Error(phonebook_id, PB_READRECLIST, PB_BUSY);	
		return PB_BUSY;
	}

	/* Read in list of records */
	
	for (log_index = start_log_index; log_index<start_log_index+num_recs && log_index<list->records_max; log_index++)
	{
		result = ATB_mem_ReadRec(phonebook_id, index_type, log_index, &list->record[log_index-start_log_index]);
		if (result!=PB_OK)
			break;
		list->records_used++;
	}

	/* Close any open file */
	
	FS_pb_Finished(phonebook_id);
	
	/* Send success/failure info to the GI */
	
	if (result==PB_OK)
		GI_pb_OK(phonebook_id, PB_READRECLIST, NULL);
	else if (result!=PB_EXCT)
		GI_pb_Error(phonebook_id, PB_READRECLIST, result);
	
	return result;
}


/*******************************************************************************

 $Function:    	ATB_pb_Search
 
 $Description:	Searches every record in the index list for a fragment.  The index list
 				must be either INDEX_NAME or INDEX_NUMBER.  If it is INDEX_NAME,
 				then a name fragment will be searched for; if it is INDEX_NUMBER,
 				a number fragment will be searched for.

				The results are stored in an index list.  They may be accessed by
				using the normal read and write functions, using the index type
				INDEX_SEARCH.  The size of the search list is returned in the variable
				recs_count.

 $Returns:		PB_OK				Action completed OK.
				PB_BUSY				Failed, phonebook is busy.
				PB_FILEREADFAIL		File read encountered an error
				PB_RECDOESNOTEXIST	Tried to access a record that does not exist
				(should never happen)

 $Arguments:	phonebook_id The phonebook identifier
				index_type	The index list to use
				record		Record containing the fragment to search for
				recs_count	Pointer to an int (user allocated) in which will be returned
							the number of search results. 

 
*******************************************************************************/

PB_RET ATB_pb_Search(SHORT phonebook_id, T_PB_INDEX index_type, T_PB_RECORD *record, SHORT *recs_count)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	PB_RET result;
	SHORT count = 0;
	SHORT log_index;
	T_PB_MATCH match;
	T_PB_RECORD *cur_record;

	/* Ensure that phonebook exists */
	if (!data)
	{
		trace("**ERROR** Phonebook does not exist");
		return PB_BOOKDOESNOTEXIST;
	}

	/* Allocate record for temporary use */
	
	cur_record = ATB_pb_AllocRec(phonebook_id);
		
	/* Make sure phonebook is not busy */
	
	if (ATB_pb_Status(phonebook_id)==PB_BUSY)
	{
		trace("* ERROR * - Phonebook is busy");
		GI_pb_Error(phonebook_id, PB_SEARCH, PB_BUSY);	
		return PB_BUSY;
	}
	
	/* Make sure we're searching a valid index type */

	if (index_type==INDEX_PHYSICAL || index_type==INDEX_SEARCH)
	{
		GI_pb_Error(phonebook_id, PB_SEARCH, PB_INDEXINVALID);	
		return PB_INDEXINVALID;
	}
	
	for (log_index=0; log_index<data->records_used; log_index++)
	{
		result = ATB_mem_ReadRec(phonebook_id, index_type, log_index, cur_record);
		if (result!=PB_OK)
			break;
			
		match = ATB_index_Match(record, cur_record, index_type);
		
		if (match==MATCH_EXACT || match==MATCH_START || match==MATCH_FRAGMENT)
		{
			data->search_table[count] = ATB_index_GetPhysIndex(phonebook_id, index_type, log_index);
			count++;
		}
	}

	data->search_results = count;
	*recs_count = count;

	/* Free allocated record */
	
	ATB_pb_FreeRec(phonebook_id, cur_record);

	/* Close any open file */
	
	FS_pb_Finished(phonebook_id);
	
	/* Send success/failure info to the GI */
	
	if (result==PB_OK)
		GI_pb_OK(phonebook_id, PB_SEARCH, NULL);
	else if (result!=PB_EXCT)
		GI_pb_Error(phonebook_id, PB_SEARCH, result);

	return result;
}


/*******************************************************************************

 $Function:    	ATB_pb_ConvIndex
 
 $Description:	Returns the index in table dest_index_type corresponding to the index
 				in table src_index_type.
 
 $Returns:		PB_OK			Action completed OK.

 $Arguments:	phonebook_id	The phonebook identifier.
				index_type	The index table of the original index.
				log_index		The original logical index.
				new_index_type	The index table required.
				new_log_index	Pointer to where the new logical index will be stored
				
*******************************************************************************/

PB_RET ATB_pb_ConvIndex(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index,
	T_PB_INDEX new_index_type, SHORT *new_log_index)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT phys_index;

	/* Ensure that phonebook exists */
	if (!data)
	{
		trace("**ERROR** Phonebook does not exist");
		return PB_BOOKDOESNOTEXIST;
	}
	
	if (index_type==new_index_type)
	{
		*new_log_index = log_index;
	}
	else
	{	
		phys_index = ATB_index_GetPhysIndex(phonebook_id, index_type, log_index);
		*new_log_index = ATB_index_GetLogIndex(phonebook_id, new_index_type, phys_index);
	}
	
	return PB_OK;
}


/*******************************************************************************

 $Function:    	ATB_pb_CharToBCD

 $Description:	Converts an ascii string of digits into BCD form, with 4 bits representing
 				each digit.
 
 $Returns:		None

 $Arguments:	src 	- source string (ascii)
 				dest	- destination string for BCD digits
 
*******************************************************************************/

void ATB_pb_CharToBCD(UBYTE *dest, char *src, int max_len)
{
	UBYTE srcIndex = 0;
	UBYTE destIndex = 0;
	BOOL leftbits = TRUE;														/* Left or right nibble */
	UBYTE digit = 0;
  //add "*" "#" in number by Peng Hou on 19/06/2006 begin	
    while (digit!=0xF && srcIndex<max_len)
	{
		if (src[srcIndex]==NULL)
			digit = 0xF;														/* 0xF terminates BCD */
	   else
		{
            switch ( src[srcIndex] )
			{
               case '0':
               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9':
                digit = src[srcIndex]-'0';	
               break;

               case '*':
                 digit = 0x0a;
               break;

              case '#':
                 digit = 0x0b;
              break;

              case 'p':
              case 'P':
                 digit = 0x0c;
              break;

              case 'w':
              case 'W':
                 digit = 0x0d;
              break;

              case 'E':
                 digit = 0x0e;
               break;

             default:
               continue;
			}
		}
	//add "*" "#" in number by Peng Hou on 19/06/2006 end		
			
			//	digit = src[srcIndex]-'0';											/* The digit, 0 to 9. */


		if (leftbits)
		{
			dest[destIndex] = digit;
			leftbits = FALSE;
		}
		else
		{
			dest[destIndex] |= digit<<4;										/* *16 shifts right 4 bits */
			leftbits = TRUE;
			destIndex++;
		}
		srcIndex++;
	} 
	
	return;
}


/*******************************************************************************

 $Function:    	ATB_pb_BCDToChar

 $Description:	Converts a BCD string to ascii digits
 
 $Returns:		None

 $Arguments:	src 	- source string (BCD)
 				dest	- destination string for ascii digits
 
*******************************************************************************/

void ATB_pb_BCDToChar(char *dest, UBYTE *src, int max_len)
{
	SHORT numIndex = 0;
	UBYTE digit = 0xF; //dummy
 //add "*" "#" in number by Peng Hou on 19/06/2006 begin
	while (digit!=NULL && numIndex<max_len)
	{
		/*HELLO!*/
		digit = ATB_num_Digit(src, numIndex);

		if (digit==0xF)
			digit = NULL;
		else
		{
		      switch ( digit )
              {
                 case 0:
                 case 1:
                 case 2:
                 case 3:
                 case 4:
                 case 5:
                 case 6:
                 case 7:
                 case 8:
                 case 9:
      	          {
      	              digit+='0';
		              dest[numIndex] = digit;
		           }   
		         break;

                case 0x0a:
                      dest[numIndex]='*';
	            break;
                case 0x0b:
		              dest[numIndex]='#';
     	        break;
               case 0x0c:
		              dest[numIndex]='P';
     	       break;
               case 0x0d:
		              dest[numIndex]='W';
     	       break;
               case 0x0e:
		               dest[numIndex] ='E';
     	       break;
               default:
                       dest[numIndex] ='\0';
  }

}

		//	digit+='0';
 //add "*" "#" in number by Peng Hou on 19/06/2006 end
		numIndex++;
	}

	return;
}


/*******************************************************************************

 $Function:    	ATB_pb_AllocRec
 
 $Description:	Allocates memory for a record

 $Returns:		

 $Arguments:	phonebook_id	The phonebook identifier
 
*******************************************************************************/

T_PB_RECORD *ATB_pb_AllocRec(SHORT phonebook_id)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	T_PB_RECORD *record;
	
	record = (T_PB_RECORD *)GI_pb_MemAlloc(sizeof(T_PB_RECORD));
	
	record->alpha.data = (USHORT *)GI_pb_MemAlloc(data->alpha_max*sizeof(USHORT));
	record->number = (UBYTE *)GI_pb_MemAlloc(data->number_max/2);
	if (data->ext_max>0)
	{
		record->ext_data = (UBYTE *)GI_pb_MemAlloc(data->ext_max);
	}
	else
	{
		record->ext_data = NULL;
	}
	record->ton_npi = 0;

	return record;
}


/*******************************************************************************

 $Function:    	ATB_pb_FreeRec
 
 $Description:	Frees memory allocated for a record

 $Returns:		

 $Arguments:	phonebook_id	The phonebook identifier
 				record			The record to destroy
 
*******************************************************************************/

void ATB_pb_FreeRec(SHORT phonebook_id, T_PB_RECORD *record)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);

	if (data->ext_max>0 && record->ext_data!=NULL)
	{
		GI_pb_MemFree((UBYTE *)record->ext_data, data->ext_max);
	}
	GI_pb_MemFree((UBYTE *)record->number, data->number_max/2);
	GI_pb_MemFree((UBYTE *)record->alpha.data, data->alpha_max*sizeof(USHORT));

	GI_pb_MemFree((UBYTE *)record, sizeof(T_PB_RECORD));
	
	return;
}


/*******************************************************************************

 $Function:    	ATB_pb_AllocRecList
 
 $Description:	Allocates memory for a list of records

 $Returns:		

 $Arguments:	phonebook_id	The phonebook identifier
 				num_recs		The number of records to allocate
 
*******************************************************************************/

T_PB_LIST *ATB_pb_AllocRecList(SHORT phonebook_id, SHORT num_recs)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	T_PB_LIST *list;
	SHORT rec_index;
	
	list = (T_PB_LIST *)GI_pb_MemAlloc(sizeof(T_PB_LIST));
	list->records_max = num_recs;
	list->records_used = 0;
	list->record = (T_PB_RECORD *)GI_pb_MemAlloc(sizeof(T_PB_RECORD)*num_recs);
	
	for (rec_index=0; rec_index<num_recs; rec_index++)
	{
		list->record[rec_index].alpha.data = (USHORT *)GI_pb_MemAlloc(data->alpha_max*sizeof(USHORT));
		list->record[rec_index].number = (UBYTE *)GI_pb_MemAlloc(data->number_max/2);
		if (data->ext_max>0)
		{
			list->record[rec_index].ext_data = (UBYTE *)GI_pb_MemAlloc(data->ext_max);
		}
		else
		{
			list->record[rec_index].ext_data = NULL;
		}
	}

	return list;
}


/*******************************************************************************

 $Function:    	ATB_pb_FreeRecList
 
 $Description:	Frees memory allocated for a list of records

 $Returns:		

 $Arguments:	phonebook_id	The phonebook identifier
 				record			The records to destroy
 				num_recs		Number of records in the list
 
*******************************************************************************/

void ATB_pb_FreeRecList(SHORT phonebook_id, T_PB_LIST *list)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT rec_index;
	
	for (rec_index=0; rec_index<list->records_max; rec_index++)
	{
		if (data->ext_max>0 && list->record[rec_index].ext_data!=NULL)
		{
			GI_pb_MemFree((UBYTE *)list->record[rec_index].ext_data, data->ext_max);
		}
		GI_pb_MemFree((UBYTE *)list->record[rec_index].number, data->number_max/2);
		GI_pb_MemFree((UBYTE *)list->record[rec_index].alpha.data, data->alpha_max*sizeof(USHORT));
	}

	GI_pb_MemFree((UBYTE *)list->record, sizeof(T_PB_RECORD)*list->records_max);
	GI_pb_MemFree((UBYTE *)list, sizeof(T_PB_LIST));

	return;
}


/*******************************************************************************

 $Function:    	ATB_mem_CopyRec
 
 $Description:	Copies a record from one location to another.  The destination should
 				have memory allocated for its strings.

 $Returns:		

 $Arguments:	dest_record		The destination record
 				src_record		The source record
 
*******************************************************************************/

void	ATB_pb_CopyRec(SHORT phonebook_id, T_PB_RECORD *dest_record, T_PB_RECORD *src_record)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	
	dest_record->alpha.length = src_record->alpha.length;
	dest_record->alpha.dcs = src_record->alpha.dcs;
	memcpy((UBYTE *)dest_record->alpha.data, (UBYTE *)src_record->alpha.data, data->alpha_max*sizeof(USHORT));
	memcpy(dest_record->number, src_record->number, data->number_max/2);
	if (data->ext_max>0)
	{
		memcpy(dest_record->ext_data, src_record->ext_data, data->ext_max);
	}
	dest_record->ton_npi = src_record->ton_npi;
	
	return;
}


/*******************************************************************************

 $Function:    	ATB_pb_OK
 
 $Description:	This function is called if the requested FS command executed successfully.

 $Returns:		None.

 $Arguments:	phonebook_id	The phonebook identifier
 
*******************************************************************************/


PB_RET ATB_pb_OK(SHORT phonebook_id)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	PB_RET result=PB_ERROR;

	switch(data->command_id)
	{
		case PB_INITIALISE:
			result = ATB_status_Initialise(phonebook_id);
			break;
		case PB_WRITEREC:
			result = ATB_status_WriteRec(phonebook_id);
			break;
		case PB_DELETEREC:
			result = ATB_status_DeleteRec(phonebook_id);
			break;
	}

	return result;
}


/*******************************************************************************

 $Function:    	ATB_pb_Error
 
 $Description:	This function is called if an error was returned while executing the
 				requested command.

 $Returns:		None.

 $Arguments:	phonebook_id	The phonebook identifier
				error_id		Identifies the error that occurred.

*******************************************************************************/

void ATB_pb_Error(SHORT phonebook_id, SHORT error_id)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);

	/* Forward the error report to the GI */

	GI_pb_Error(phonebook_id, data->command_id, error_id);
	data->status = PB_STATUS_NONE;
	
	return;
}

/*******************************************************************************

 $Function:    	ATB_index_GetTable
 
 $Description:	Returns a pointer to the index table specified by the index_type

 $Returns:		None.

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index table required.

*******************************************************************************/

SHORT * ATB_index_GetTable(SHORT phonebook_id, T_PB_INDEX index_type)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT *table = NULL;
	
	switch(index_type)
	{
		case INDEX_NAME:
			table = data->name_table;
			break;
		case INDEX_NUMBER:
			table = data->number_table;
			break;
		case INDEX_SEARCH:
			table = data->search_table;
			break;
		default:
			table = GI_pb_GetTable(index_type);
			break;
	}

	return table;
}


/*******************************************************************************

 $Function:    	ATB_index_Find
 
 $Description:	Find the nearest match to the supplied record.  See ATB_pb_Find for
 				more information.

 $Returns:		PB_OK				Action completed OK.
 				PB_BUSY				Failed, phonebook is busy.
				PB_FILEREADFAIL		File read encountered an error
				PB_RECDOESNOTEXIST	Tried to access a record that does not exist
									(should never happen)

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index_type of the provided record that is to be
								searched for, and the index list that the resulting
								new_log_index pertains to
				record			The record whose index_type is to be matched
				new_log_index	Returns with the logical index of the closest matching
								record
				match			Specifies how good the resulting match was
								(MATCH_NONE, MATCH_START, MATCH_FRAGMENT
								or MATCH_EXACT).
 
*******************************************************************************/

PB_RET ATB_index_Find (SHORT phonebook_id, T_PB_INDEX index_type, T_PB_RECORD *record, SHORT *new_log_index, T_PB_MATCH *match)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	PB_RET result = PB_OK;
	/*SHORT *table;*/ /*a0393213 warning removal-variable set but not used*/
	SHORT lower;
	SHORT upper;
	SHORT middle;
	T_PB_RECORD *comp_record;
	T_PB_COMPARE compare;
	UBYTE searching;
	
	/* Ensure that phonebook exists */
	if (!data)
	{
		trace("**ERROR** Phonebook does not exist");
		return PB_BOOKDOESNOTEXIST;
	}

	/* Allocate record for comparison */

	comp_record = ATB_pb_AllocRec(phonebook_id);
	
	/* Make sure we're searching a valid index type */

	if (index_type==INDEX_PHYSICAL || index_type==INDEX_SEARCH)
	{
		GI_pb_Error(phonebook_id, PB_FIND, PB_INDEXINVALID);	
		return PB_INDEXINVALID;
	}
	
	/*table = ATB_index_GetTable(phonebook_id, index_type);*//*a0393213 warning removal-variable set but not used*/

	/* The initial upper and lower bounds are the bounds of the array */
	lower = 0;
	upper = data->records_used-1;

	searching = TRUE;

	/* If the list is empty, insert at 0 */
	
	if (data->records_used==0)
	{
		middle = 0;
		searching = FALSE;
	}
	
	while (searching)
	{
		/* Compare our record with the middle element of the partition */

		middle = (lower+upper)/2;

		result = ATB_mem_ReadRec(phonebook_id, index_type, middle, comp_record);
		
		if (result!=PB_OK)
			break;
			
		compare = ATB_index_Compare(record, comp_record, index_type);

		switch(compare)
		{
			case COMPARE_IDENTICAL:
				/* We've found a match; stop searching */
				searching = FALSE;
				break;
				
			case COMPARE_FIRSTBEFORE:
				if (middle==lower)
				{
					/* The partition is a single element, so stop here. */
					searching = FALSE;
				}
				else
				{
					/* Our record comes before the middle element;
					 * redefine the partiton */
					upper = middle - 1;
				}
				break;
				
			case COMPARE_FIRSTAFTER:
				if (middle==upper)
				{
				/* The partition is a single element, so stop here.  The record
				 * we need is the record after this one. */
					middle++;
					searching = FALSE;
				}
				else
				{
					/* Our record comes after the middle element;
					 * redefine the partiton */
					lower = middle+1;
				}
				break;
		}
	}	

	/* Store our results */
	
	if (new_log_index)
		*new_log_index = middle;

	if (match)
	{
		ATB_mem_ReadRec(phonebook_id, index_type, middle, comp_record);
		*match = ATB_index_Match(record, comp_record, index_type);
	}

	/* Free allocated record */

	ATB_pb_FreeRec(phonebook_id, comp_record);

	/* Send success/failure info to the GI */
	
	if (result==PB_OK)
		GI_pb_OK(phonebook_id, PB_FIND, NULL);
	else if (result!=PB_EXCT)
		GI_pb_Error(phonebook_id, PB_FIND, result);

	return result;
}


/*******************************************************************************

 $Function:    	ATB_index_GetPhysIndex
 
 $Description:	Returns the physical index corresponding to the given logical index, in
 				the index table specified by the index_type.

 $Returns:		None.

 $Arguments:	phonebook_id	The phonebook identifier.
				index_type		The index table required.
				log_index		The logical index.

*******************************************************************************/

SHORT ATB_index_GetPhysIndex(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT phys_index=-1;
	SHORT *table;

	switch(index_type)
	{
		case INDEX_PHYSICAL:
			phys_index = log_index;
			break;
		case INDEX_NAME:
		case INDEX_NUMBER:
		case INDEX_SEARCH:
			table = ATB_index_GetTable(phonebook_id, index_type);
			phys_index = table[log_index];
			break;
		default:
			TRACE_ERROR("ATB_index_GetPhysIndex():Unknown index");
			break;
	}

	return phys_index;
}


/*******************************************************************************

 $Function:    	ATB_index_GetLogIndex
 
 $Description:	Provided with the physical index, finds the logical index of the record
 				in the index table specified.

 $Returns:		None.

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index table required.
				phys_index		The logical index of the record to find

*******************************************************************************/

SHORT ATB_index_GetLogIndex(SHORT phonebook_id, T_PB_INDEX index_type, SHORT phys_index)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT *table = ATB_index_GetTable(phonebook_id, index_type);
	SHORT log_index;

	if (index_type==INDEX_PHYSICAL)
	{
		log_index = phys_index;
	}
	else
	{
		for (log_index=0; log_index<data->records_used; log_index++)
		{
			if (table[log_index]==phys_index)
				break;
		}
	}
	
	return log_index;
}


/*******************************************************************************

 $Function:    	ATB_index_AddRec
 
 $Description:	Adds a record to an index list, sorting automatically as required.

 $Returns:		None.

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index table required.
				phys_index		The physical index of the record to add.
				record			The record to add.
				new_log_index	New logical index of record.

*******************************************************************************/

void ATB_index_AddRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT phys_index, T_PB_RECORD *record, SHORT *new_log_index)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT *table = ATB_index_GetTable(phonebook_id, index_type);
	SHORT log_index;
	SHORT found_log_index;

	ATB_index_Find(phonebook_id, index_type, record, &found_log_index, NULL);

	/* Shift end of index table down */

	for (log_index=data->records_used; log_index>found_log_index; log_index--)
	{
		table[log_index] = table[log_index-1];
	}
	
	table[found_log_index] = phys_index;

	if (new_log_index)
		*new_log_index = found_log_index;
	
	return;
}

/*******************************************************************************

 $Function:    	ATB_index_RemoveRec
 
 $Description:	Removes a record reference from an index list, sorting the index list
 				appropriately.

 $Returns:		None.

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index table required.
				log_index		The logical index of the record to remove

*******************************************************************************/

void ATB_index_RemoveRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT *table = ATB_index_GetTable(phonebook_id, index_type);
	SHORT table_index;

	for (table_index = log_index+1; table_index<data->records_used; table_index++)
	{
		table[table_index-1] = table[table_index];
	}

	return;
}


/*******************************************************************************

 $Function:    	ATB_index_Match
 
 $Description:	This function attempts a match between two records based on the
 				index_type specified in index_type.  It returns a value indicating
 				whether there is no match (MATCH_NONE), a partial match
 				(MAX_START), or an exact match (MATCH_EXACT).
				The function GI_pb_Match is called first, to check whether there is
				any user-specific matching required.  If not, then ATB_alpha_Match
				is called for an alpha tag, or ATB_num_Match for a phone number.
				For any other index_type, MATCH_NONE is returned as default.

 $Returns:		MATCH_EXACT, MATCH_START or MATCH_NONE

 $Arguments:	record1		The first record to be compared
				record2		The second record to be compared
				index_type	Indicator of the index_type which is to be matched

*******************************************************************************/

T_PB_MATCH ATB_index_Match(T_PB_RECORD *record1, T_PB_RECORD *record2, T_PB_INDEX index_type)
{
	T_PB_MATCH match;

	match = GI_pb_Match(record1, record2, index_type);

	if (match==MATCH_DEFAULT)
	{
		switch(index_type)
		{
			case INDEX_NAME:
				match = ATB_alpha_Match(&record1->alpha, &record2->alpha);
				break;
			case INDEX_NUMBER:
				match = ATB_num_Match(record1->number, record2->number);
				break;
		}
	}

	return match;
}



/*******************************************************************************

 $Function:    	ATB_index_Compare
 
 $Description:	This function compares two records based on the index_type specified in
 				index_type.  It returns a value indicating whether the first record should
 				come before or after the second.
				The function GI_pb_Compare is called first, to check whether there is
				any user-specific sorting required.  If not, then ATB_alpha_Compare is
				called for an alpha tag, or ATB_num_Compare for a phone number.
				For any other index_type, COMPARE_FIRSTBEFORE is returned as default.

 $Returns:		COMPARE_IDENTICAL		The two records are identical
				COMPARE_FIRSTBEFORE		The first record should come before the second record
				COMPARE_FIRSTAFTER		The first record should come after the second record

 $Arguments:	record1		The first record to be compared
				record2		The second record to be compared
				index_type	Indicator of the index_type which is to be compared

*******************************************************************************/

T_PB_COMPARE ATB_index_Compare(T_PB_RECORD *record1, T_PB_RECORD *record2, T_PB_INDEX index_type)
{
	T_PB_COMPARE compare;
	
	compare = GI_pb_Compare(record1, record2, index_type);

	if (compare==COMPARE_DEFAULT)
	{
		switch(index_type)
		{
			case INDEX_NAME:
				compare = ATB_alpha_Compare(&record1->alpha, &record2->alpha);
				break;
			case INDEX_NUMBER:
				compare = ATB_num_Compare(record1->number, record2->number);
				break;
		}
	}

	return compare;
}


/*******************************************************************************

 $Function:    	ATB_alpha_Compare
 
 $Description:	This function compares two alpha strings on the basis of an ascending
 				alphanumeric latin list, and specifies which record would come first.


 $Returns:		COMPARE_FIRSTBEFORE		The first record should come before the second record
				COMPARE_FIRSTAFTER		The first record should come after the second record

 $Arguments:	alpha1				The first alpha tag
				alpha2				The second alpha tag

*******************************************************************************/

T_PB_COMPARE ATB_alpha_Compare(T_PB_ALPHA *alpha1, T_PB_ALPHA *alpha2)
{
	T_PB_COMPARE compare;
	SHORT charIndex;
	USHORT length1 = alpha1->length;
	USHORT length2 = alpha2->length;
	USHORT char1;
	USHORT char2;

	compare = COMPARE_IDENTICAL;	/* Default */

	/* Special case, length of first string is 0 */
	
	if (length1==0)
		compare = COMPARE_FIRSTBEFORE;
	
	for (charIndex = 0; charIndex < length1; charIndex++)
	{
		if (charIndex==length2)	/* E.g. "Johnson" and "John" */
		{
			compare = COMPARE_FIRSTAFTER;
			break;
		}

		char1 = alpha1->data[charIndex];
		if (char1>=(SHORT)'A' && char1<=(SHORT)'Z')
			char1 += (SHORT)('a'-'A');
		char2 = alpha2->data[charIndex];
		if (char2>=(SHORT)'A' && char2<=(SHORT)'Z')
			char2 += (SHORT)('a'-'A');
		
		if (char1 < char2)
		{
			compare = COMPARE_FIRSTBEFORE;
			break;
		}
		if (char1 > char2)
		{
			compare = COMPARE_FIRSTAFTER;
			break;
		}
		if (charIndex==length1-1 && length2>length1)  /*E.g. "John" and "Johnson" */
		{
			compare = COMPARE_FIRSTBEFORE;
			break;
		}
	}

	return compare;
}


/*******************************************************************************

 $Function:    	ATB_alpha_Match
 
 $Description:	This function matches two alpha strings on the basis of a latin string
 				matched from the start, and specifies how they match.

 $Returns:		MATCH_NONE			The strings do not match
				MATCH_START			The first string matches the start of the second string
				MATCH_EXACT			The two strings match exactly

 $Arguments:	alpha1				The first alpha tag
				alpha2				The second alpha tag

*******************************************************************************/

T_PB_MATCH ATB_alpha_Match(T_PB_ALPHA *alpha1, T_PB_ALPHA *alpha2)
{
	T_PB_MATCH match;
	SHORT charIndex;
	USHORT length1 = alpha1->length;
	USHORT length2 = alpha2->length;
	SHORT offset;
	UBYTE searching;
	UBYTE some_match;
	USHORT char1, char2;

	match = MATCH_EXACT;	/* Default */
	searching = TRUE;
	some_match = FALSE;
	offset = 0;
	
	do
	{
		for (charIndex = 0; charIndex < length1; charIndex++)
		{

			if (charIndex==(length2-offset))	/* E.g. "Johnson" and "John" */
			{
				searching = FALSE;
				break;							/* No match, will exit do/while */
			}

			char1 = alpha1->data[charIndex];
			if (char1>=(SHORT)'A' && char1<=(SHORT)'Z')
				char1 += (SHORT)('a'-'A');
			char2 = alpha2->data[charIndex+offset];
			if (char2>=(SHORT)'A' && char2<=(SHORT)'Z')
				char2 += (SHORT)('a'-'A');
		
			if (char1 != char2)
			{
				some_match = FALSE;				/* Any fragment so far identified does not fit */
				break;							/* No match, keep looking */
			}

			some_match = TRUE;

			if (charIndex==length1-1 && length2>length1)  /*E.g. "John" and "Johnson" */
			{
				if (offset==0)
					match = MATCH_START;
				else
					match = MATCH_FRAGMENT;
				break;
			}
		}

		if (some_match==TRUE)
		{
			searching = FALSE;
		}
		else
		{
			offset++;

			/* If the fragment won't fit, don't keep looking */
			if ((offset+length1)>length2)
			{
				match=MATCH_NONE;
				searching = FALSE;
			}
		}
	}while (searching);
	
	return match;
}


/*******************************************************************************

 $Function:    	ATB_num_Digit
 
 $Description:	Extracts the four-bit digit from a BCD number

 $Returns:		The four bit digit

 $Arguments:	num				The BCD number
 				numIndex		The position in the BCD number to look

*******************************************************************************/

UBYTE ATB_num_Digit(UBYTE *num, SHORT numIndex)
{
	UBYTE digit;
	UBYTE shift = (numIndex&0x1)*0x4;
		
	digit = num[numIndex>>1];		/* Each BCD digit takes up half a byte */
	digit &= (0xF<<shift);			/* Isolate the digit */
	digit >>= shift;				/* Shift it so it has a value 0x0 - 0xF */
	
	return digit;
}


/*******************************************************************************

 $Function:    	ATB_num_Length
 
 $Description:	Returns the length of a BCD number in digits

 $Returns:		Length of BCD number

 $Arguments:	num				The BCD number

*******************************************************************************/

SHORT ATB_num_Length(UBYTE *num)
{
	SHORT length;
	
	for (length = 0; ATB_num_Digit(num, length)!=0xF; length++)
	{
	}

	return length;
}


/*******************************************************************************

 $Function:    	ATB_num_Compare
 
 $Description:	This function compares two phone numbers by the standard comparison
 				method (ascending numeric, from the end of the number) and specifies
 				which record would come first if sorted in this method.

 $Returns:		COMPARE_FIRSTBEFORE		The first record should come before the second record
				COMPARE_FIRSTAFTER		The first record should come after the second record

 $Arguments:	num1				The first number
				num2				The second number

*******************************************************************************/

T_PB_COMPARE ATB_num_Compare(UBYTE *num1, UBYTE *num2)
{
	T_PB_COMPARE compare;
	SHORT charIndex;
	SHORT length1 = ATB_num_Length(num1);
	SHORT length2 = ATB_num_Length(num2);
	UBYTE digit1;
	UBYTE digit2;

	compare = COMPARE_IDENTICAL;	/* Default */
	
	for (charIndex = 0; charIndex < length1; charIndex++)
	{
		if (charIndex==length2)	/* E.g. "123456" and "1234" */
		{
			compare = COMPARE_FIRSTAFTER;
			break;
		}

		digit1 = ATB_num_Digit(num1, (SHORT)(length1-charIndex-1));
		digit2 = ATB_num_Digit(num2, (SHORT)(length2-charIndex-1));
		
		if (digit1 < digit2)
		{
			compare = COMPARE_FIRSTBEFORE;
			break;
		}
		if (digit1 > digit2)
		{
			compare = COMPARE_FIRSTAFTER;
			break;
		}
		if (charIndex==length1-1 && length2>length1)  /*E.g. "1234" and "123456" */
		{
			compare = COMPARE_FIRSTBEFORE;
			break;
		}
	}

	return compare;
}

/*******************************************************************************

 $Function:    	ATB_num_Match
 
 $Description:	This function matches phone numbers, from the end backwards, and
 				specifies how they match.

 $Returns:		MATCH_NONE			The numbers do not match
				MATCH_START			The first number matches the end of the second number
				MATCH_EXACT			The two numbers match exactly

 $Arguments:	num1				The first number
				num2				The second number

*******************************************************************************/

T_PB_MATCH ATB_num_Match(UBYTE *num1, UBYTE *num2)
{
	T_PB_MATCH match;
	SHORT charIndex;
	SHORT length1 = ATB_num_Length(num1);
	SHORT length2 = ATB_num_Length(num2);
	SHORT offset;
	UBYTE searching;
	UBYTE some_match;
	UBYTE digit1;
	UBYTE digit2;

	match = MATCH_EXACT;	/* Default */
	searching = TRUE;
	some_match = FALSE;
	offset = 0;
	
	do
	{
		for (charIndex = 0; charIndex < length1; charIndex++)
		{
			if (charIndex==(length2-offset))	/* E.g. "12345" and "123" */
			{
				searching = FALSE;
				break;							/* No match, will exit do/while */
			}

			digit1 = ATB_num_Digit(num1, (SHORT)(length1-charIndex-1));
			digit2 = ATB_num_Digit(num2, (SHORT)(length2-charIndex-1));
		
			if (digit1 != digit2)
			{
				some_match = FALSE;				/* Any fragment so far identified does not fit */
				break;							/* No match, keep looking */
			}

			some_match = TRUE;

			if (charIndex==length1-1 && length2>length1)  /*E.g. "123" and "12345" */
			{
				if (offset==0)
					match = MATCH_START;
				else
					match = MATCH_FRAGMENT;
				break;
			}
		}

		if (some_match==TRUE)
		{
			searching = FALSE;
		}
		else
		{
			offset++;

			/* If the fragment won't fit, don't keep looking */
			if ((offset+length1)>length2)
			{
				match=MATCH_NONE;
				searching = FALSE;
			}
		}
	} while (searching);
	
	return match;
}


/*******************************************************************************

 $Function:    	ATB_mem_UpdateCache
 
 $Description:	Updates the cache with the record provided.  If the cache is full, the
 				least used record is overwritten.

 $Returns:		None.

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index table required.
				phys_index		The physical index of the record to add.
				record			The record to add.

*******************************************************************************/

void ATB_mem_UpdateCache(SHORT phonebook_id, SHORT phys_index, T_PB_RECORD *record)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT cacheIndex;
	SHORT leastUsed;
	SHORT	leastIndex;
	
	/* First check if record is already in RAM */
	
	if (data->cache[phys_index] != NULL)
	{
		/* Copy the record in case it has changed */

		ATB_pb_CopyRec(phonebook_id, data->cache[phys_index], record);
		
		if (data->in_memory[phys_index]<0x7FFF)/*a0393213 lint WR:Constant out of range Soln:0xffff changed to 0x7fff*/
			data->in_memory[phys_index]++;
		return;
	}

	/* If the cache is full, find the least accessed record */

	if (data->cache_size==data->cache_max)
	{
		leastIndex = 0;
		leastUsed = 255;
			
		for (cacheIndex=0; cacheIndex<data->records_max; cacheIndex++)
		{
			if (data->cache[cacheIndex]!=NULL && data->in_memory[cacheIndex] < leastUsed)
			{
				leastUsed = data->in_memory[cacheIndex];
				leastIndex = cacheIndex;
			}
		}

		/* Give the new record the memory allocated for the least used record */
		
		data->cache[phys_index] = data->cache[leastIndex];
		data->cache[leastIndex] = NULL;
		data->in_memory[leastIndex] = 0;
	}
	
	/* If the cache is not full, allocate memory for record */
	
	else
	{
		data->cache[phys_index] = ATB_pb_AllocRec(phonebook_id);
		data->cache_size++;
	}

	/* Copy the record into cache */

	ATB_pb_CopyRec(phonebook_id, data->cache[phys_index], record);
	data->in_memory[phys_index] = 1;

	return;
}

/*******************************************************************************

 $Function:    	ATB_mem_ReadRec
 
 $Description:	Reads a record from the logical position index.  Information is read from
 				cache if it is present in memory, otherwise it will be read from file.

 				The record information will be returned in the data structure pointed to
 				by record.  The T_PB_RECORD structure must be allocated by the caller.

 $Returns:		PB_OK				Action completed OK.
				PB_FILEREADFAIL		File read encountered an error
				PB_RECDOESNOTEXIST	Tried to access a record that does not exist

 $Arguments:	phonebook_id	The phonebook identifier
				index_type		The index list from which the record is to be read
				log_index		Logical index in the index list of the record to read
				record			Structure in which to store record data
 
*******************************************************************************/

PB_RET	ATB_mem_ReadRec(SHORT phonebook_id, T_PB_INDEX index_type, SHORT log_index, T_PB_RECORD *record)
{
	T_PB_DATA *data = ATB_hnd_GetPbData(phonebook_id);
	SHORT phys_index;
	PB_RET result;
	
	/* Check that record exists */

	if (log_index<0
		|| (log_index>=data->records_used && (index_type==INDEX_NAME || index_type==INDEX_NUMBER))
		|| (index_type==INDEX_SEARCH && log_index>data->search_results)
		|| (index_type==INDEX_PHYSICAL && log_index>data->records_max))
	{
		trace("*** Error - record does not exist ***");
		return PB_RECDOESNOTEXIST;
	}
	
	/* Get the corresponding physical record index */
	
	phys_index = ATB_index_GetPhysIndex(phonebook_id, index_type, log_index);

	/* If the record is in the cache, simply copy it.  Otherwise, call FS to read the record. */
	
	if (data->in_memory[phys_index]>0)
	{
		ATB_pb_CopyRec(phonebook_id, record, data->cache[phys_index]);
		result = PB_OK;
	}
	else
	{
		/* NB - assumes blocking read operation.  If the read operation is blocking,
		 * the cache must be large enough to store all the phonebook entries. */
		result = FS_pb_ReadRec(phonebook_id, phys_index, record);
		ATB_mem_UpdateCache(phonebook_id, phys_index, record);
	}

	return result;
}


/*******************************************************************************

 $Function:    	ATB_hnd_GetPbData
 
 $Description:	Returns phonebook data handle

 $Returns:		The phonebook data handle

 $Arguments:	phonebook_id	The phonebook identifier
 
*******************************************************************************/

T_PB_DATA*	ATB_hnd_GetPbData(SHORT phonebook_id)
{
	return phonebook[phonebook_id];
}


/*******************************************************************************

 $Function:    	ATB_hnd_SetPbData
 
 $Description:	Sets phonebook data handle

 $Returns:		None

 $Arguments:	phonebook_id	The phonebook identifier
 				data			The phonebook data
 
*******************************************************************************/

void ATB_hnd_SetPbData(SHORT phonebook_id, T_PB_DATA *data)
{
	phonebook[phonebook_id] = data;

	return;
}

