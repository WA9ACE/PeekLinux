/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                            
 $Module:		
 $File:		    MmiHomezone.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    15/02/02                                                      
                                                                               
********************************************************************************
                                                                              
 Description: Homezone functionality in MMI
 
   
                        
********************************************************************************

 $History: MmiHomezone.c

	15/02/02			Original Condat(UK) BMI version.
	28/05/02			Latest Condat(UK) BMI version

	Sep 14, 2007	REF: OMAPS00145865  Adrian Salido
	Description:	FT - MMI: Wrong trace class 
	Solution:		changed some event traces to function traces because the information content of 
				this is low for non-MMI people and against TI coding convention.

	Mar 14, 2006 DR: OMAPS00061468 - x0035544.
   	Description: SAT 27.22.4.22.2 SET UP IDLE MODE TEXT (Icon support) fails 
   	Solution : In the function homezoneUpdateDisplay(), additional parameter has been added to the
   	function call addSatMessage() under the flag FF_MMI_SAT_ICON to support SAT Icon for IDLE TEXT display.

       Dec 15, 2005   REF: MMI-FIX-OMAPS00058684 x0035544
       Description: OMAPS00045726 / MMI-FIX-30439 Follow-Up : ifdef for FF Home Zone forgotten.
       Solution: Added Feature flag FF_HOMEZONE for the function sms_homezoneCBToggle(U8 Activate_CB)
       in this file as it has got the prototype for this function.
       
	Jun 14, 2005 REF: MMI-FIX-30439 x0018858
   	Description: The homezone/cityzone tags were not being displayed properly.
   	Solution: Modified to save the tags properly in caches and also made the 
	appropriate modifications to update the same properly during mobility.
	   
 $End

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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


#include "mfw_sys.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_icn.h"
#include "mfw_phb.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#include "mfw_mnu.h"
#include "mfw_sat.h"
#include "mfw_tim.h"
#include "mfw_cphs.h"

#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"

#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "MmiCall.h"

#include "MmiBookController.h"

#include "MmiIdle.h"
#include "Mmiicons.h"
#include "MmiNetwork.h"
#include "mmiSmsBroadcast.h"
#include "MmiHomezone.h"

#include "cus_aci.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "include/pcm.h"
#endif


//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
// enable HZ trace 
//#undef	TRACE_MMIHOMEZONE
#define TRACE_MMIHOMEZONE 1

T_homezone_data		homezoneData;
extern T_SMSCB_DATASTORE g_smscb_data; /* storage for Cell Broadcast lists */
static int  hzFlag = 0; //Jun 14, 2005 REF: MMI-FIX-30439 x0018858

//Dec 14, 2005 REF: MMI-FIX-OMAPS00058684 x0035544
#ifdef FF_HOMEZONE
//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
// Externed the function for activation\deactivation of Homezone.
extern void sms_homezoneCBToggle(U8 Activate_CB);
#endif

/* x0039928 - Lint warning fix */
static char *homezoneMNC_MCC[HZ_PERMITTED_NETWORKS] =
{
	"26207", /* VIAG Interkom */
};

//void sms_homezoneCBToggle(U8 Activate_CB);

/*******************************************************************************

LOCALLY REFERENCED FUNCTIONS

*******************************************************************************/

void		homezoneReadParams();
void		homezoneLocate();
static U32	calculateDecimal( char *string, int length );
static U32	readHexFromSimfile( UBYTE *simFile, int length );
static void	writeHexToSimfile( UBYTE *simFile, U32 value, int length );

/*******************************************************************************

 $Function:    	homezoneInit

 $Description:	Initialise homezone data structure.  Reset the recent cache.
 
 $Returns:		

 $Arguments:
*******************************************************************************/

void homezoneInit()
{
	T_homezone_data	*data			= &homezoneData;
	USHORT cacheIndex;
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	// Added the following local variables.
	//begin 30439
	UBYTE          version; 
	USHORT         max_rcd;
	EF_HZCACHE     cache;
	//end 30439
	
	TRACE_FUNCTION("homezoneInit()");

	/* Set all data to 0 */

	memset((UBYTE *)data, 0, sizeof(T_homezone_data));

	data->current_zone = HZ_NOT_IN_ZONE;									// 0xFF = not in a homezone
	data->recent_cell_index = HZ_RECENT_MAX-1;								// Next entry will loop round & be at start of cache

	/* Set up recent cell cache */
	
	for (cacheIndex = 0; cacheIndex<HZ_RECENT_MAX; cacheIndex++)
	{
		data->recent_cell[cacheIndex] = 0;
		data->recent_cell_zone[cacheIndex] = HZ_NOT_IN_ZONE;
		//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
		//begin 30439
			/* Read cache from PCM */
		if (pcm_ReadRecord((UBYTE *)EF_HZCACHE_ID,
                         (USHORT)(cacheIndex+1),
                         SIZE_EF_HZCACHE,
                         (UBYTE *)&cache,
                         &version,
                         &max_rcd) NEQ DRV_OK)
		{
			TRACE_EVENT ("Init cache read from PCM wrong");
			break;                                      /* read error */
		}
		else
		{
			if (cache.cid[0] != 0xFF AND cache.cid[1] != 0xFF)
			{
				data->recent_cell[cacheIndex] = readHexFromSimfile(&cache.cid[0],2);
				data->recent_cell_zone[cacheIndex] = cache.zone;
			}
        }
		TRACE_EVENT_P3("Init cache %d: CID %X, zone %X",cacheIndex, data->recent_cell[cacheIndex], data->recent_cell_zone[cacheIndex]);
		//end 30439
	}

	data->new_cell_res = FALSE;
	
	return;
}

			
/*******************************************************************************

 $Function:    	homezoneReadSim

 $Description:	Read homezone data in from SIM.  Checks to see if conditions (SIM,
 				network) are correct for homezone; switches it off if not.
 
 $Returns:		None.

 $Arguments:	read_again	- if this is HZ_READ_ONCE_ONLY, then SIM fields will not
 					be read in again if they have already been read in once.
 					If this is HZ_READ_SIM_AGAIN, they will be read in anyway.
 					
*******************************************************************************/

void homezoneReadSim(UBYTE read_again)
{
	T_homezone_data		*data = &homezoneData;
	char IMSI[17];
	char MNC_MCC[6];
	
	USHORT				networkIndex;
	UBYTE				networkOK;
	USHORT				tries;

	/* Check if homezone is switched off */
	
	if (data->received == HZ_RECEIVED_HZ_OFF)
		return;

	/* SPR877 - If read_again specifies that SIM data should not be re-read, exit from function
	 * provided we have read it once. */
	 
	if (read_again == HZ_READ_ONCE_ONLY && data->received & HZ_RECEIVED_SIMDATA)
		return;
	
	TRACE_FUNCTION("homezoneReadSim()");

	/* Check if network operator is in list */

	networkOK = FALSE;

	tries = 0;
	
	while ((qAT_PlusCIMI(CMD_SRC_LCL, IMSI)) != AT_CMPL  && tries < 5)
	{
		tries++;
	}
		
	strncpy(MNC_MCC, IMSI, 5);
	MNC_MCC[5] =0;	//terminate string

#ifdef TRACE_MMIHOMEZONE
	TRACE_EVENT_P1("MNC_MCC: %s", (char *)MNC_MCC);
#endif

	for (networkIndex=0; networkIndex<HZ_PERMITTED_NETWORKS; networkIndex++)
	{
		if (!strncmp((char *)MNC_MCC, homezoneMNC_MCC[networkIndex],5))
		{
#ifdef TRACE_MMIHOMEZONE
			TRACE_EVENT_P1("Matches entry %d", networkIndex);
#endif
			networkOK = TRUE;
		}
	}
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//begin 30439
	/* Attempt to read first sim field */
	if (read_again == HZ_READ_SIM_AGAIN)
		hzFlag = 1;
	//end 30439

	if (networkOK)
	{
		if (!Read_Sim(HZ_SIM_PARAMETERS, 0, HZ_SIM_PARAMETERS_SIZE, data->buffer))
		{
			TRACE_EVENT("SIM read error 1: Homezone OFF");	/* SIM field not present, switch off homezone */
			data->received = HZ_RECEIVED_HZ_OFF;
			hzFlag = 0; //Jun 14, 2005 REF: MMI-FIX-30439 x0018858 - Seting the variable
		}
	}
	else
	{
		TRACE_EVENT("Wrong network: Homezone OFF");		/* Network not in list, switch off homezone */
		data->received = HZ_RECEIVED_HZ_OFF;
		hzFlag = 0; //Jun 14, 2005 REF: MMI-FIX-30439 x0018858 - Seting the variable
	}
	
	return;
}


/*******************************************************************************

 $Function:    	homezoneReadSimCb

 $Description:	Callback, called when read sim has occurred.  Fills up the homezone
 				data structure with the appropriate values retrieved from SIM.
 
 $Returns:		None.

 $Arguments: 	field - the SIM field that was just read
 				record - the record in the SIM field that was read
 				result - the result code returned - 0 for successful.
*******************************************************************************/

void homezoneReadSimCb(USHORT field, USHORT record, USHORT result)
{
	T_homezone_data	*data				= &homezoneData;
	T_homezone		*homezone			= data->homezone;
	UBYTE			*simFile;
	USHORT			fileIndex;
	USHORT			hzIndex;
	UBYTE			keepLooking;
	U8	Activate_cb =0; //Jun 14, 2005 REF: MMI-FIX-30439 x0018858 
						//To activate/deactivate homezoneCB
	TRACE_FUNCTION("homezoneReadSimCb()");
	
#ifdef TRACE_MMIHOMEZONE
	TRACE_EVENT_P2("Have just read SIM file: %x record %d", field, record);
#endif

	/* Read in general homezone data.  If reading first SIM field fails, switch homezone off */

	if (field==HZ_SIM_PARAMETERS)
	{	
		/* check SIM read result for homezone parameter */
		//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
		//begin 30439
		//Modified the conditional statement
		//if (result)
		if (result != 0x80FF)
//end 30439
		{
			TRACE_EVENT("SIM read error 2: Homezone OFF");
			data->received = HZ_RECEIVED_HZ_OFF;
			hzFlag = 0; //Jun 14, 2005 REF: MMI-FIX-30439 x0018858 -Setting the variable
			return;
		}
		homezoneReadParams();
		/*homezoneActivateCB();*/				/* Switch on cell broadcast */
		/* SH - this is already done at startup */
		record = HZ_CACHE_MAX;
	}
	
	/* Read in records for each homezone cache */

	/* check SIM read result for hoemzone cache */
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//begin 30439
	//Modified the conditional statement
	//if (field>HZ_SIM_PARAMETERS && !result)
	if (field>HZ_SIM_PARAMETERS && (result == 0x80FF))
	//end 30439
	{
		hzIndex = field - HZ_SIM_CELL_CACHE;
		simFile = data->buffer2;
		fileIndex = 0;
		
		homezone[hzIndex].lac[record-1] = readHexFromSimfile(&simFile[fileIndex],2);
		fileIndex+=2;
		homezone[hzIndex].cid[record-1] = readHexFromSimfile(&simFile[fileIndex],2);

#ifdef TRACE_MMIHOMEZONE
		TRACE_EVENT_P2("read lac = %x, cid = %x", homezone[hzIndex].lac[record-1], homezone[hzIndex].cid[record-1]);
#endif
	}

	/* Read in the next record */

	keepLooking = TRUE;
	
	while (keepLooking)
	{
		/* check SIM read result for hoemzone cache */
		//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
		//begin 30439
		//Modified the conditional statement
		//if (record==HZ_CACHE_MAX || result)
		if (record==HZ_CACHE_MAX || (result != 0x80FF))
//end 30439
		{
			field++;												// Look at next field
			record = 1;
			//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
			// Print the current feild.
			TRACE_EVENT_P1("current field = %x", field);
		}
		else
			record++;

		hzIndex = field-HZ_SIM_CELL_CACHE;

		if (hzIndex==HZ_ZONES_MAX)
		{
			keepLooking = FALSE;
			/* Have read all sim data, set flag so icon etc will be updated */
			data->received |= HZ_RECEIVED_SIMDATA;
			//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
			//begin 30439
			/* homezone display update after SIM REFRESH (when hzFlag set to 1) */
			TRACE_EVENT_P1("hzFlag = %d", hzFlag);
			if (hzFlag)
			{
//				hzFlag = 0;
				homezoneUpdate(0, 0, 1);
			}
			else
			{

				TRACE_EVENT("activate homezone CB");
				//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
				//Call to activate the Homezone CB.
				Activate_cb = 1;
				
#ifdef FF_HOMEZONE
				sms_homezoneCBToggle(Activate_cb);
#endif
				homezoneUpdate(0, 0, 1); /* new VO : speed up update */
			}
			//end 30439
			 
		}
		else if (homezone[hzIndex].active)
		{
			keepLooking = FALSE;
			
#ifdef TRACE_MMIHOMEZONE
	//TRACE_EVENT_P3("HZ READ HZ %d field %X record %d", hzIndex, field, record);
#endif
			if (!Read_Sim_Record(field, record, HZ_SIM_CELL_CACHE_SIZE, data->buffer2))
			{
				TRACE_EVENT("SIM reading error!");
			}
		}
	}
	
	return;
}


/*******************************************************************************

 $Function:    	homezoneReadParams

 $Description:	Decode the homezone parameters from the first SIM field
 
 $Returns:		None.

 $Arguments:	None.
*******************************************************************************/

void homezoneReadParams()
{
	T_homezone_data	*data				= &homezoneData;
	T_homezone		*homezone			= data->homezone;
	UBYTE			*simFile;
	USHORT			fileIndex;
	USHORT			tagIndex;
	USHORT			hzIndex;
	char			character;

	TRACE_FUNCTION("homezoneReadParams()");
	
	simFile = data->buffer;
	fileIndex = HZ_SIM_FIRST_ZONE;										// Point to the first homezone data

	for (hzIndex=0; hzIndex<HZ_ZONES_MAX; hzIndex++)					// For each homezone...
	{
		homezone[hzIndex].zoneID = simFile[fileIndex];				// Read in the various parameters
		fileIndex++;
		
		homezone[hzIndex].X = readHexFromSimfile(&simFile[fileIndex],4);
		fileIndex += 4;
		homezone[hzIndex].Y = readHexFromSimfile(&simFile[fileIndex],4);
		fileIndex += 4;
		homezone[hzIndex].Rsquared = readHexFromSimfile(&simFile[fileIndex],4);
		fileIndex += 4;
		
		homezone[hzIndex].active = (BOOL) (simFile[fileIndex] & HZ_ACTIVE_FLAG)/HZ_ACTIVE_FLAG;
		homezone[hzIndex].cityzone = (BOOL) (simFile[fileIndex] & HZ_CITYZONE_FLAG)/HZ_CITYZONE_FLAG;
		fileIndex++;
					
		/* Read in the 12-character tag name */
		
		for (tagIndex = 0; tagIndex<HZ_NAME_LENGTH; tagIndex++)					
		{
			character = (char) simFile[fileIndex];
			fileIndex++;
			if (character<32 /*|| character>127*/)   /*a0393213 - lint warnings removal - 'Relational operator '>' always evaluates to 'false'*/
				character = 0;
			homezone[hzIndex].Tag[tagIndex] = character;
		}
		homezone[hzIndex].Tag[HZ_NAME_LENGTH] = 0;								// Make sure tag is null-terminated

#ifdef TRACE_MMIHOMEZONE
			TRACE_EVENT_P1("Homezone %d:", hzIndex);
			TRACE_EVENT_P1("ID: %d", homezone[hzIndex].zoneID);
			TRACE_EVENT_P1("Active: %d", (UBYTE)homezone[hzIndex].active);
			TRACE_EVENT_P1("Cityzone: %d", (UBYTE)homezone[hzIndex].cityzone);
			TRACE_EVENT_P1("X: %X", homezone[hzIndex].X);
			TRACE_EVENT_P1("Y: %X", homezone[hzIndex].Y);
			TRACE_EVENT_P1("Rsquared: %X", homezone[hzIndex].Rsquared);
		TRACE_EVENT_P1("Homezone name: %s", homezone[hzIndex].Tag);
#endif

	}

	return;
}


/*******************************************************************************

 $Function:    	homezoneWriteSim

 $Description:	Write homezone data to the SIM
				SPR 877 - SH - now just writes a single record
 
 $Returns:		None.

 $Arguments:	cache, record
********************************************************************************/

void homezoneWriteSim(USHORT field, USHORT record)
{
	T_homezone_data	*data				= &homezoneData;
	USHORT hzIndex;
	UBYTE *simFile;
	USHORT fileIndex;

	/* Check if homezone is already switched off */

	if (data->received==HZ_RECEIVED_HZ_OFF)
		return;
	
	TRACE_FUNCTION("homezoneWriteSim()");

	hzIndex = field-HZ_SIM_CELL_CACHE;
	
#ifdef TRACE_MMIHOMEZONE
	TRACE_EVENT_P4("HZ WRITE field: %X record %d lac %X cid %X", field, record, data->homezone[hzIndex].lac[record-1], data->homezone[hzIndex].cid[record-1]);
#endif

	if (data->homezone[hzIndex].active)
	{	
		simFile = data->buffer2;
		fileIndex = 0;
		writeHexToSimfile(&simFile[fileIndex], data->homezone[hzIndex].lac[record-1], 2 );
		fileIndex += 2;
		writeHexToSimfile(&simFile[fileIndex], data->homezone[hzIndex].cid[record-1], 2 );
	}

	if (!Write_Sim_Record(field, record, HZ_SIM_CELL_CACHE_SIZE, data->buffer2))
	{
		TRACE_EVENT("SIM writing error!");
	}
	return;
}


/*******************************************************************************

 $Function:    	homezoneWriteSimCb

 $Description:	Write homezone data to the SIM.  This function is called after a previous
 				SIM write (or, in the initial case, by homezoneWriteSim above).
				SPR877 - SH - Previous functionality no longer required here.
 
 $Returns:		None.

 $Arguments:	field - the SIM field that is to be written
 				record - the SIM record that is to be written
 				result - the result of the last write attempt
*******************************************************************************/

void homezoneWriteSimCb(USHORT field, USHORT record, USHORT result)
{
	TRACE_FUNCTION("homezoneWriteSimCb()");

#ifdef TRACE_MMIHOMEZONE
	TRACE_EVENT_P1("RESULT: %d", result);
#endif

	return;
}


/*******************************************************************************

 $Function:    	homezoneCBData

 $Description:	Make sure correct homezone CB channel is set up and activate CB
 
 $Returns:		None.

 $Arguments:	message		- The text of the cell broadcast message
*******************************************************************************/

void homezoneActivateCB(void)
{
	T_homezone_data	*data				= &homezoneData;
	USHORT index;
	UBYTE stopSearch;
	USHORT emptyID;

	/* Check if homezone is already switched off */

	if (data->received==HZ_RECEIVED_HZ_OFF)
		return;
	
	TRACE_FUNCTION("homezoneActivateCB()");
	
	/* Check table to see if correct channel is included */
	
	stopSearch = FALSE;
	emptyID = 0xFFFF;
	
	for (index=0; index<MAX_MIDS && !stopSearch; index++)
	{
		TRACE_EVENT_P2("Mid %d is %X", index, g_smscb_data.CBTopics.entry[index].msg_id);
		if (g_smscb_data.CBTopics.entry[index].msg_id==HZ_IDENTIFIER)
		{
			stopSearch = TRUE;
		}
		else if (g_smscb_data.CBTopics.entry[index].msg_id==NO_MID)
		{
			emptyID = index;
			stopSearch = TRUE;
		}		
	}

	TRACE_EVENT_P1("Emtpy ID %d", emptyID);
	
	if (emptyID!=0xFFFF && emptyID<MAX_MIDS-1)
	{
		strncpy (g_smscb_data.CBTopics.entry[emptyID].name, HZ_CB_NAME, CB_TAG_LENGTH);
	    g_smscb_data.CBTopics.entry[emptyID].msg_id = HZ_IDENTIFIER;
	     g_smscb_data.CBTopics.entry[emptyID].dcs = 0;

	    // add a new entry
	    emptyID += 1;
	    g_smscb_data.CBTopics.length = emptyID;
	}
	
    smscb_setCBsettings(SMSCB_SWITCH_ON);
	return;
}


/*******************************************************************************

 $Function:    	homezoneCBData

 $Description:	A cell broadcast message has been received, containing information about
 				the location of the current cell
 
 $Returns:		None.

 $Arguments:	message		- The text of the cell broadcast message
*******************************************************************************/

void homezoneCBData(char *message)
{
	T_homezone_data	*data				= &homezoneData;

	/* Check if homezone is already switched off */

	if (data->received==HZ_RECEIVED_HZ_OFF)
		return;

	TRACE_FUNCTION("homezoneCellData()");

	/* Calculate the current position from the text of the message */
	
	data->current_X = calculateDecimal( &message[0], HZ_COORD_DIGITS );
	data->current_Y = calculateDecimal( &message[HZ_COORD_DIGITS], HZ_COORD_DIGITS );

#ifdef TRACE_MMIHOMEZONE
	TRACE_EVENT_P2("current_X: %X, current_Y: %X", data->current_X, data->current_Y);
#endif
			
	data->received |= HZ_RECEIVED_CB;				// We now have received cell broadcast message.

	/* SPR877 - SH - now only call HZ algorithm if a cell res has recently taken place */
	
	if (data->received==HZ_RECEIVED_ALL && data->new_cell_res)  // If we've received everything, call algorithm.
	{
		homezoneLocate();
		data->new_cell_res = FALSE;
	}
			
 	return;
}


/*******************************************************************************

 $Function:    	homezoneLocate

 $Description:	Looks at coordinates from cell broadcast message and calculates if
 				they are in a homezone
 
 $Returns:		None

 $Arguments:	None
 
*******************************************************************************/

void homezoneLocate()
{
	T_homezone_data	*data				= &homezoneData;
	T_homezone		*homezone			= data->homezone;
	long			deltaX;
	long			deltaY;
	USHORT			hzIndex;
	USHORT			cacheIndex=0;
	U32				Dsquared;
	BOOL			foundInCache;
	USHORT			cacheFreeSpace=0;	
	USHORT			field;
	USHORT			record=0;
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//begin 30439
	EF_HZCACHE      cache;
	UBYTE			exist = 0;
	//end 30439

	/* Check if homezone is already switched off */
	
	if (data->received==HZ_RECEIVED_HZ_OFF)
		return;
	
	TRACE_FUNCTION("homezoneLocate()");

	data->current_zone = HZ_NOT_IN_ZONE;							// We don't know if we're in a homezone yet (homezones = 0 to 3)
	field = HZ_NOT_IN_ZONE;											// The cache to update

	/* Work out if we're in a homezone using current_X and current_Y, received from CB message */

	if (data->received == HZ_RECEIVED_ALL)
	{
#ifdef TRACE_MMIHOMEZONE
		TRACE_EVENT_P2("Current cell X: %X, Y: %X", data->current_X,data->current_Y);
#endif
	
		/* Check to see if (current_X,current_Y) is in any of the defined homezones */
		
		for (hzIndex=0; hzIndex<HZ_ZONES_MAX && data->current_zone==HZ_NOT_IN_ZONE; hzIndex++)						// For each homezone...
		{
#ifdef TRACE_MMIHOMEZONE		
			TRACE_EVENT_P1("Homezone %d...", hzIndex);
			TRACE_EVENT_P3("Has coords X: %X, Y: %X, R2: %X", homezone[hzIndex].X, homezone[hzIndex].Y, homezone[hzIndex].Rsquared);
#endif

			if (homezone[hzIndex].active)										// Provided the homezone is active
			{
#ifdef TRACE_MMIHOMEZONE														// Calculate X and Y distance 
				TRACE_EVENT("Is active.");
#endif
				deltaX = labs(data->current_X-homezone[hzIndex].X);				// between centre of current cell
				deltaY = labs(data->current_Y-homezone[hzIndex].Y);				// and centre of the homezone
#ifdef TRACE_MMIHOMEZONE
				TRACE_EVENT_P1("deltaX = %d",deltaX);
				TRACE_EVENT_P1("deltaY = %d",deltaY);
#endif		
				if (deltaX<=HZ_DISTANCE_MAX && deltaY<=HZ_DISTANCE_MAX) 		// If not too far...
				{
					Dsquared = deltaX*deltaX+deltaY*deltaY;						// Calculate the resultant distance squared
#ifdef TRACE_MMIHOMEZONE
						TRACE_EVENT_P1("Dsquared = %d",Dsquared);
#endif							
					if (Dsquared <= homezone[hzIndex].Rsquared)					// If within radius of homezone squared
					{
#ifdef TRACE_MMIHOMEZONE
						TRACE_EVENT("Contains the current cell.");
#endif
						data->current_zone = hzIndex;						// Current zone is this homezone
						
						if (!homezone[hzIndex].cityzone)							// Then: if it's not a cityzone
						{
#ifdef TRACE_MMIHOMEZONE
							TRACE_EVENT("Is not a cityzone.");
#endif
							foundInCache = FALSE;									// Look through the cache for this
							cacheFreeSpace = HZ_CACHE_MAX;							// homezone to see if cell is listed

							for (cacheIndex=0; cacheIndex<HZ_CACHE_MAX && !foundInCache; cacheIndex++)
							{
								if (data->cid==homezone[hzIndex].cid[cacheIndex])
								{
									foundInCache = TRUE;							// We've found it!
								}
								if (homezone[hzIndex].cid[cacheIndex]==0 && cacheFreeSpace==HZ_CACHE_MAX)
									cacheFreeSpace = cacheIndex;					// Or we've found an empty space
							}

							if (!foundInCache)										// If not there, need to add it
							{
#ifdef TRACE_MMIHOMEZONE
								TRACE_EVENT("Is not in the cache.");
#endif
								if (cacheFreeSpace==HZ_CACHE_MAX)					// If we didn't find a free space...
								{
									cacheFreeSpace = rand() % HZ_CACHE_MAX;			// Choose a random entry 
								}
								homezone[hzIndex].cid[cacheFreeSpace] = data->cid;	// And store current cell there
								homezone[hzIndex].lac[cacheFreeSpace] = data->lac;	// with its location area code

								field = hzIndex+HZ_SIM_CELL_CACHE;					// We've changed the cache
								record = cacheFreeSpace+1;

#ifdef TRACE_MMIHOMEZONE
								TRACE_EVENT_P1("Have added to cache, entry %d.",cacheFreeSpace);
#endif
							}
						}
					}
					else															// If we're not in the homezone...
					{
						foundInCache = FALSE;										// Look through the cache for this
																					// homezone to see if cell is listed
						for (cacheIndex=0; cacheIndex<HZ_CACHE_MAX && !foundInCache; cacheIndex++)
						{
							if (data->cid==homezone[hzIndex].cid[cacheIndex])
							{
								foundInCache = TRUE;								// We've found it!
								cacheFreeSpace = cacheIndex;						// Record where it is
								break;
							}
						}

						if (foundInCache)											// If it's there...
						{
#ifdef TRACE_MMIHOMEZONE
							TRACE_EVENT("Is in the cache and shouldn't be.");
#endif
							homezone[hzIndex].cid[cacheFreeSpace] = 0;				// ...remove it
							field = hzIndex+HZ_SIM_CELL_CACHE;					// We've changed the cache
							record = cacheFreeSpace+1;
							data->current_zone = HZ_NOT_IN_ZONE;						// We're not in this zone
						}
#ifdef TRACE_MMIHOMEZONE
						else
						{
							TRACE_EVENT("Isn't in the cache and shouldn't be.");
						}
#endif
					}

					//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
					//begin 30439
					/* check if the cid is already in cache */
					for (cacheIndex=0; cacheIndex<HZ_RECENT_MAX; cacheIndex++) /*a0393213 - lint warnings removal - HZ_CACHE_MAX changed to HZ_RECENT_MAX*/
					{
						if (data->recent_cell[cacheIndex] == data->cid)
							exist = 1;
					}
					/* Replace oldest cell in dynamic cache with new cell */
					//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
					//begin 30439
					//Added a conditional statement
					if (!exist)
					{
					data->recent_cell_index++;
					if (data->recent_cell_index>=HZ_RECENT_MAX)
						data->recent_cell_index = 0;
#ifdef TRACE_MMIHOMEZONE
					TRACE_EVENT_P1("Adding to recent list, index = %d", data->recent_cell_index);
#endif
					data->recent_cell[data->recent_cell_index] = data->cid;
					data->recent_cell_zone[data->recent_cell_index] = data->current_zone;
					
					//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
					//begin 30439
					/* write cache to PCM */
					writeHexToSimfile(&cache.cid[0], data->recent_cell[data->recent_cell_index], 2 );
					cache.zone = data->recent_cell_zone[data->recent_cell_index];
					/* write cache to PCM */
					pcm_WriteRecord((UBYTE *)EF_HZCACHE_ID,
									(USHORT)(data->recent_cell_index+1),
									SIZE_EF_HZCACHE,
									&cache.cid[0]);

					}
					//end 30439
#ifdef TRACE_MMIHOMEZONE
for (cacheIndex = 0; cacheIndex<HZ_RECENT_MAX; cacheIndex++)
{
		TRACE_EVENT_P3("Recent cache %d: CID %X, zone %X",cacheIndex, data->recent_cell[cacheIndex], data->recent_cell_zone[cacheIndex]);
}
#endif
				}

			}

		}

	}

#ifdef TRACE_MMIHOMEZONE
	TRACE_EVENT("CACHES AFTER UPDATE");
	for (hzIndex=0; hzIndex<HZ_ZONES_MAX; hzIndex++)			// For each homezone...
	{	
		/* x0083025 on Sep 14, 2007 for OMAPS00145865 (adrian) */
		MMI_TRACE_EVENT_P2("Zone %d (%s)", hzIndex, homezone[hzIndex].Tag);

		if (homezone[hzIndex].active && !homezone[hzIndex].cityzone)				// If it's active, and not a cityzone
		{
			for (cacheIndex=0; cacheIndex<HZ_CACHE_MAX; cacheIndex++)	// Check the cache.
			{ 
				TRACE_EVENT_P3("Cache %d: lac %X, cid %X", cacheIndex, homezone[hzIndex].lac[cacheIndex], homezone[hzIndex].cid[cacheIndex]);
			}
		}
	}
#endif
	/* Update icon/text tag. */
	
	homezoneUpdateDisplay();
	
	/* Write data back to SIM if cache has changed & SIM files present */

	if (field!=HZ_NOT_IN_ZONE && (data->received & HZ_RECEIVED_SIMDATA))
	{
		homezoneWriteSim(field, record);
	}
	
	return;
}


/*******************************************************************************

 $Function:    	homezoneUpdate

 $Description:	Main homezone function; check whether MS is in a homezone or not
 
 $Returns:		

 $Arguments:	lac		- the location area code of the current cell (on cell reselection)
 				cid	- the ID of the current cell (on cell reselection)
				keep - 1: no change in lac and cid
*******************************************************************************/
//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
//begin 30439
//Added an extra argument in the function -- "keep"
void homezoneUpdate(USHORT lac, USHORT cid, UBYTE keep)
{
	T_homezone_data	*data				= &homezoneData;
	T_homezone		*homezone			= data->homezone;
	USHORT			hzIndex;
	USHORT			cacheIndex;
	UBYTE			inCache = 0;

	/* Check if homezone is already switched off */

	TRACE_EVENT_P1("Data received: %d", data->received);
	
	if (data->received==HZ_RECEIVED_HZ_OFF)
		return;
	
	TRACE_FUNCTION("homezoneUpdate()");

	data->current_zone = HZ_NOT_IN_ZONE;							// We don't know if we're in a homezone yet (homezones = 0 to 3)
	data->received |= HZ_RECEIVED_CELLRES;							// Have received this event
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//Modified to add a conditional statement
	//begin 30439
	if (!keep)
	{
		data->lac = lac;
		data->cid = cid;
	}
	/* keep the current lac and cid */
	else
	{
		lac = data->lac;
		cid = data->cid;
	}
	//end 30439

#ifdef TRACE_MMIHOMEZONE
	TRACE_EVENT_P3("lac: %X, cid: %X, received: %d", lac, cid, data->received);
#endif

	/* If either are zero or -1, the reselection data is invalid */
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//Modified to add a conditional statement
	if (((lac==0 || cid==0) || (lac==0xFFFF || cid==0xFFFF)) AND !keep)
	{
		TRACE_EVENT_P1("homezoneUpdateDisplay ~HZ_RECEIVED_CELLRES keep=%d", keep);
	//end 30439
		data->received &= (~HZ_RECEIVED_CELLRES);
		homezoneUpdateDisplay();
		return;
	}

	data->new_cell_res = TRUE;										// Cell res just occurred

	/* Must have received SIM data to continue*/

	if (!(data->received & HZ_RECEIVED_SIMDATA))
		return;
	
	/* Check homezone cache for each homezone */
	/* Can do this even if we haven't received the CB message */

#ifdef TRACE_MMIHOMEZONE
	TRACE_EVENT("Checking zone caches...");
#endif
	
	for (hzIndex=0; hzIndex<HZ_ZONES_MAX && data->current_zone==HZ_NOT_IN_ZONE; hzIndex++)			// For each homezone...
	{
#ifdef TRACE_MMIHOMEZONE
		/* x0083025 on Sep 14, 2007 for OMAPS00145865 (adrian) */
		MMI_TRACE_EVENT_P2("Zone %d (%s)", hzIndex, homezone[hzIndex].Tag);
#endif
		
		if (homezone[hzIndex].active && !homezone[hzIndex].cityzone)				// If it's active, and not a cityzone
		{
#ifdef TRACE_MMIHOMEZONE
		TRACE_EVENT("Is Active.");
#endif		
			for (cacheIndex=0; cacheIndex<HZ_CACHE_MAX && data->current_zone==HZ_NOT_IN_ZONE; cacheIndex++)	// Check the cache.
			{ 
#ifdef TRACE_MMIHOMEZONE
					TRACE_EVENT_P3("Cache %d: lac %X, cid %X", cacheIndex, homezone[hzIndex].lac[cacheIndex], homezone[hzIndex].cid[cacheIndex]);
#endif
				
				if (cid==homezone[hzIndex].cid[cacheIndex])					// If the current cell is in the cache....
				{
#ifdef TRACE_MMIHOMEZONE
					TRACE_EVENT_P1("Cell found in cache %d.",cacheIndex);
#endif
					data->current_zone = hzIndex;									// Then this is our current homezone!
					inCache = 1;
				}
			}
			if (!inCache AND hzFlag)
			{
				hzFlag = 0;
				TRACE_EVENT ("not in cache, caculate needed");
				homezoneLocate();
				return;
			}
			else
				inCache = 0;
		}
	}
	//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
	//added a conditional statement
	//begin 30439
	/* If we didn't find a homezone, check recent cells cache to see if we're in a cityzone */
	for (hzIndex=0; hzIndex<HZ_ZONES_MAX && data->current_zone==HZ_NOT_IN_ZONE; hzIndex++)			// For each homezone...
	{
#ifdef TRACE_MMIHOMEZONE
		/* x0083025 on Sep 14, 2007 for OMAPS00145865 (adrian) */
		MMI_TRACE_EVENT_P2("Zone %d (%s)", hzIndex, homezone[hzIndex].Tag);
#endif
		
		if (homezone[hzIndex].active && homezone[hzIndex].cityzone)				// If it's active, and not a cityzone
		{
#ifdef TRACE_MMIHOMEZONE
		TRACE_EVENT("Is Active.");
#endif		
		
		for (cacheIndex=0; cacheIndex<HZ_RECENT_MAX && data->current_zone==HZ_NOT_IN_ZONE; cacheIndex++)	// Check the cache.
		{ 
#ifdef TRACE_MMIHOMEZONE
				TRACE_EVENT_P5("Cache %d: lac %X, cid %X, data->recent_cell %X, data->recent_cell_zone %d", cacheIndex, homezone[hzIndex].lac[cacheIndex], homezone[hzIndex].cid[cacheIndex], data->recent_cell[cacheIndex], data->recent_cell_zone[cacheIndex]);
#endif
				
			if (cid==data->recent_cell[cacheIndex] && data->recent_cell_zone[cacheIndex]!=HZ_NOT_IN_ZONE) // If we're in that cell 
				{
#ifdef TRACE_MMIHOMEZONE
				TRACE_EVENT_P1("FOUND cityzone in cache %d.",cacheIndex);
#endif
				//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
				//Modified to directly assign value
				data->current_zone = hzIndex;									// Then this is our current homezone!
				inCache = 1;
			}
			if (!inCache AND hzFlag)
			{
				hzFlag = 0;
				TRACE_EVENT ("not in cache, caculate needed");
				homezoneLocate();
				return;
			}
			else
				inCache = 0;
		}
		}
	}

	homezoneUpdateDisplay();								// Update icon/text tag.

	return;
}


/*******************************************************************************

 $Function:    	homezoneUpdateDisplay

 $Description:	
 
 $Returns:		Updates the homezone icon and text string appropriately

 $Arguments:	none
*******************************************************************************/

void homezoneUpdateDisplay(void)
{
	T_homezone_data	*data	= &homezoneData;

	TRACE_FUNCTION("homezoneUpdateDisplay()");

	iconsDeleteState(iconIdHomezone);
	
#ifdef FF_MMI_SAT_ICON
	addSatMessage(NULL, 0, 0, NULL, FALSE, SAT_ICON_NONE);
#else
	addSatMessage(NULL);
#endif
	
	if (data->current_zone!=HZ_NOT_IN_ZONE)										// If not in a homezone, leave blank
	{
//x0035544 Mar 14, 2006 DR:OMAPS00061468
#ifdef FF_MMI_SAT_ICON
	addSatMessage(data->homezone[data->current_zone].Tag, 0, 0, NULL, FALSE, SAT_ICON_NONE);
#else
	addSatMessage(data->homezone[data->current_zone].Tag);
#endif
		if (!data->homezone[data->current_zone].cityzone)						// Display icon if homezone rather than cityzone
		{
			iconsSetState(iconIdHomezone);
		}
	}
	
	return;
}


/*******************************************************************************

 $Function:    	calculateDecimal

 $Description:	
 
 $Returns:		Returns a decimal number from a string of ascii digits

 $Arguments:	none
*******************************************************************************/

static U32 calculateDecimal( char *string, int length )
{
	U32 total = 0;
	int index;
	
	/* decode each decimal character in turn */
	for (index = 0; index < length; index++ )
	{
		if (string[index]>='0' && string[index]<='9')
		{
			total = ( total * 10 ) + ( string[index] - '0' );
		}
	}
	
	return total;
}

/*******************************************************************************

 $Function:    	readHexFromSimfile

 $Description:	Reads a hex string (MSB first) from a sim file
 
 $Returns:		The hex number

 $Arguments:	simFile - the point in the simFile at which the string occurs
 				length  - the length of the string
*******************************************************************************/

static U32 readHexFromSimfile( UBYTE *simFile, int length )
{
	U32 total = 0;
	int index;
	
	for ( index = 0; index < length; index++ )
	{
		total = ( total << 8 ) + simFile[index];
	}
  
	return total;
}

/*******************************************************************************

 $Function:    	writeHexToSimfile

 $Description:	Writes a hex string (MSB first) to a sim file
 
 $Returns:		None

 $Arguments:	simFile - the point in the simFile at which the string occurs
 				length  - the length of the string
*******************************************************************************/

static void writeHexToSimfile( UBYTE *simFile, U32 value, int length )
{
	int index;

#ifdef TRACE_MMIHOMEZONE
	TRACE_EVENT_P1("Hex of %X is:", value);
#endif

	/* Note: '-1' below clarification:
	 * E.g. index=0, length=4: most significant byte is number shifted 3 bytes right. */
	 
	for ( index = 0; index < length; index++ )
	{
		simFile[index] = (value >> ((length-index-1)*8)) & 0xFF;	/* SPR877 - SH - wrong before, added *8 */
#ifdef TRACE_MMIHOMEZONE
		TRACE_EVENT_P2("Simfile %d is %X", index,  simFile[index]);
#endif
	}
  
	return;
}
