#ifndef _DEF_MMI_HOMEZONE_H_
#define _DEF_MMI_HOMEZONE_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                            
 $Module:		SMS
 $File:		    MmiHomezone.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description: Homezone functionality in MMI
 
   
                        
********************************************************************************

 $History: MmiHomezone.h

	15/02/02			Original Condat(UK) BMI version.	

	Jun 14, 2005 REF: MMI-FIX-30439 x0018858
   	Description: The homezone/cityzone tags were not being displayed properly.
   	Solution: Modified to save the tags properly in caches and also made the 
	appropriate modifications to update the same properly during mobility.
	   
 $End

*******************************************************************************/

/****************************************************************************
 *																			*
 *   CONSTANTS 																*
 *																			*
 ****************************************************************************/

#define	HZ_ZONES_MAX					4				// Max number of homezones the user can have
#define	HZ_RECENT_MAX	 				5				// Number of recent cells to be stored
#define	HZ_CACHE_MAX					21				// Size of cache for each homezone
#define	HZ_DISTANCE_MAX					100000			// Maximum distance in metres that user can be from centre of homezone

#define	HZ_IDENTIFIER					221				// Message ID that identifies a homezone CB
#define	HZ_COORD_DIGITS					6				// Number of digits in coordinates received in CB message
#define	HZ_ACTIVE_FLAG					0x01			// If homezone is active, first bit in byte is set
#define	HZ_CITYZONE_FLAG				0x02			// If homezone is a cityzone, second bit in byte is set

#define	HZ_SIM_PARAMETERS				0x6F60			// Field in SIM storing homezone parameters
#define	HZ_SIM_PARAMETERS_SIZE			123				// Size of this field in bytes
#define	HZ_SIM_FIRST_ZONE				17				// Offset from start of parameters file that first zone is found
#define	HZ_SIM_ZONE_SIZE				27				// Size of each zone in bytes
#define	HZ_SIM_CELL_CACHE				0x6F61			// First of series of caches storing homezone cells
#define	HZ_SIM_CELL_CACHE_SIZE			4				// Size of each cache entry (there will be HZ_CACHE_MAX of these per zone)
#define	HZ_NAME_LENGTH					12				// Length of homezone name tag

#define	HZ_NOT_IN_ZONE					0xFF			// Value for current_zone when we aren't in a homezone

#define	HZ_CB_NAME						"Homezone"		// Homezone cell broadcast channel name

/****************************************************************************
 *																			*
 *   Permitted Network Names												*
 *																			*
 ****************************************************************************/

#define HZ_PERMITTED_NETWORKS			1


/****************************************************************************
 *																			*
 *   Information storage for individual homezone							*
 *   T_homezone																*
 *																			*
 ****************************************************************************/

typedef struct
{
	UBYTE	zoneID;			// The ID of the homezone
 	BOOL	active;			// Whether the homezone is activated at the moment
 	BOOL	cityzone;		// TRUE if the zone is a cityzone
	U32		X;				// The X position of its centre
	U32		Y;				// The Y position of its centre
	U32		Rsquared;		// The radius of the homezone, squared
	char 	Tag[13];		// The text tag to be displayed when in the homezone
	USHORT	lac[HZ_CACHE_MAX];  // The cache of location area codes of cells known to be in this homezone
	USHORT	cid[HZ_CACHE_MAX];  // The cache of cell IDs known to be in this homezone
} T_homezone;


/****************************************************************************
 *																			*
 *   General homezone information storage 									*
 *   T_homezone_data														*
 *																			*
 ****************************************************************************/

typedef struct
{	
	UBYTE			current_zone;							// Current zone number (1 to HZ_ZONES_MAX). 0 if none.
	
	U32				current_X;								// Coordinates (received from CB) of centre of current cell
	U32				current_Y;

	UBYTE			received;								// Makes sure we have received all necessary data
	UBYTE			new_cell_res;							// TRUE if a new cell reselection has just occurred
	USHORT			lac;
	USHORT			cid;

	UBYTE			recent_cell_index;						// Location of recent cells that are within one of the user's homezones.
	USHORT 			recent_cell[HZ_RECENT_MAX];				// These are used for quick lookup
	UBYTE			recent_cell_zone[HZ_RECENT_MAX];		// The homezone/cityzone of the recent cell

	T_homezone		homezone[HZ_ZONES_MAX];					// The user's homezones, up to HZ_ZONES_MAX

	UBYTE 			buffer[HZ_SIM_PARAMETERS_SIZE];			// Buffers for reading from or writing to the SIM
	UBYTE			buffer2[HZ_SIM_CELL_CACHE_SIZE];
} T_homezone_data;

typedef enum
{
	HZ_RECEIVED_CB = 1,
	HZ_RECEIVED_CELLRES = 2,
	HZ_RECEIVED_SIMDATA = 4,
	HZ_RECEIVED_ALL = 7,
	HZ_RECEIVED_HZ_OFF = 128				/* Homezone is switched off */
} HZ_RECEIVED_TYPE; /*a0393213 warnings removal-declaration requires a typedef name*/


typedef enum
{
	HZ_READ_ONCE_ONLY = 0,					/* Only read SIM first time this function is called */
	HZ_READ_SIM_AGAIN = 1					/* Read SIM any number of times */
} HZ_READ_TYPE;/*a0393213 warnings removal-declaration requires a typedef name*/

/****************************************************************************
 *																			*
 *   FUNCTION PROTOTYPES													*
 *																			*
 ****************************************************************************/

void homezoneInit();

void homezoneCBData(char *message);

void homezoneReadSim(UBYTE read_again);

void homezoneReadSimCb(USHORT field, USHORT record, USHORT result);

void homezoneWriteSim(USHORT field, USHORT record);

void homezoneWriteSimCb(USHORT field, USHORT record, USHORT result);

//Jun 14, 2005 REF: MMI-FIX-30439 x0018858
// Added third argument to the function
//begin 30439
void homezoneUpdate(USHORT lac, USHORT cid, UBYTE keep);
//end 30439

void homezoneUpdateDisplay(void);

void homezoneActivateCB(void);
#endif
