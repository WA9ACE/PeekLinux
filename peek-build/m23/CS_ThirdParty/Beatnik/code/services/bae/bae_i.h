/**
 * @file	bae_i.h
 *
 * Internal definitions for BAE.
 *
 * @author	Richard Powell (richard@beatnik.com)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	9/22/2003	Richard Powell (richard@beatnik.com)		Create.
 *	7/21/2004	Richard Powell (richard@beatnik.com)		changed DRC commands to work on specific player.
 *
 * (C) Copyright 2003 by Beatnik, Inc., All Rights Reserved
 */

#ifndef __BAE_INST_I_H_
#define __BAE_INST_I_H_
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"

#include "mobileBAE.h"
#include "PAL_HostHardware_Riviera.h"
#include "bae/bae_api.h"

#ifdef BAE_WRITE_OUTPUT_FILE
#include "ffs/ffs_api.h"
#include "rfs/rfs_fm.h"
#endif // BAE_WRITE_OUTPUT_FILE


typedef struct BankFileInfo {
	// memory pointer for file put in memory.
	void *bank_in_memory;
	int bank_in_memory_size;
	T_BAE_BANK_LOCATION bank_location;
    /* x0056422 - OMAPS00156759 - Unicode */
	// the bank file name
	mbChar bankFilePath[BAE_MAX_FILEPATH];
	struct BankFileInfo *nextBank;
} BankFileInfo;



/**
 * The Control Block buffer of BAE, which gathers all 'Global variables'
 * used by BAE instance.
 *
 * A structure should gathers all the 'global variables' of BAE instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_BAE_ENV_CTRL_BLK buffer is allocated when creating BAE instance and is 
 * then always refered by BAE instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
	/** Pointer to the error function */
	T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name,	
							 T_RVM_RETURN error_cause,
							 T_RVM_ERROR_TYPE error_type,
							 T_RVM_STRING error_msg);
	/** Mem bank id. */
	T_RVF_MB_ID prim_mb_id;

	T_RVF_MB_ID int_mb_id;

	T_RVF_ADDR_ID	addr_id;

	// system id for the mobile system
	mbObjectID system;

	// handle to the audio hardware
	PAL_EngineOutputToken *engine_token;

	BankFileInfo *bankFiles;
	
	// number of channels currently playing
	int channels;

	// keep track of existing players for bank unloads
	int existing_players;

	// keep track of how much audio time has passed (for PAL_Microseconds)
	int msAudioGenerated;

#ifdef BAE_RUNTIME_METRICS
	// counter to keep track of most MHz used
	UINT32 most_ticks;
	UINT32 total_ticks;
	int total_loops;
#endif // BAE_RUNTIME_METRICS
#ifdef BAE_WRITE_OUTPUT_FILE
#if BAE_USE_RFS
    T_RFS_FD output_file;        
#else        
    T_FFS_FD output_file;
#endif    
#endif // BAE_WRITE_OUTPUT_FILE

} T_BAE_ENV_CTRL_BLK;


/** External ref "global variables" structure. */
extern T_BAE_ENV_CTRL_BLK	*bae_env_ctrl_blk_p;


#endif /* __BAE_INST_I_H_ */
