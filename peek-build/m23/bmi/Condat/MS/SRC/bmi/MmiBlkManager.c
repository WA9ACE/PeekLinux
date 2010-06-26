/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBlkManager.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This module, in conjunction with the MmiBlkResources module,
	provides the access to block resources for the MMI.
                        
    The block manager is responsible for creating and initialising
    the structures and tables to allow access to the strings, icons
    sounds and vibrations required by the MMI.
   
    Each of these items is effectively a contiguous block of memory,
    which is accessed via a block resource. Each of the block resources
    are provided by this package

********************************************************************************

 $History: MmiBlkManager.c

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/


/*******************************************************************************
                                                                              
                                Include Files
                                                                              
*******************************************************************************/

#define ENTITY_MFW

/* includes */
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
// #include <malloc.h>

#include "MmiBlkManager.h"


/*******************************************************************************
                                                                              
                       Local structures and definitions
                                                                              
*******************************************************************************/

/* Define a type for the manager control block, this contains
   an array identifying each of the resource manager handles
   which are populated as part of the initialisation sequence
*/
#define BLOCK_MANAGER_KEY          0x00BABE01L
#define BLOCK_MANAGER_ENTRY_COUNT  0x00000010L

typedef struct _tManagerEntry_
{
	tBlkId     Id;
	int		   NumEntries;
	tBlkHandle EntryBase;
} tManagerEntry, *pManagerEntry;

typedef struct _tManagerControl_
{
    long int        BlkKey;
	tBlkHandle      BaseAddress;
	int             BlkLength;
	tBlkHandle      RsrcList[BLOCK_MANAGER_ENTRY_COUNT]; 
	pManagerEntry   EntryPtr;
} tManagerControl, *pManagerControl;


/*******************************************************************************
                                                                              
                                Local routines
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	ValidManagerResource

 $Description:	Determines if the resource indicated by the handle is valid

 $Returns:		Zero if failure, non-zero if valid resource

 $Arguments:	ManagerRsrc, handle of resource manager
 
*******************************************************************************/

static int ValidManagerResource( tBlkHandle ManagerRsrc )
{
	pManagerControl ManagerControl = (pManagerControl) ManagerRsrc;

	if ( ManagerControl == NULL )
		return 0;

	return ( ManagerControl->BlkKey == BLOCK_MANAGER_KEY );
}


/*******************************************************************************
                                                                              
                                Public routines
                                                                              
*******************************************************************************/


/*******************************************************************************

 $Function:    	mmibm_Initialise

 $Description:	Initialise a block manager object 

 $Returns:		Handle of block manager object, NULL if failure

 $Arguments:	BlkBase, base address of the block manager data
                NumEntries, number of entries to be dealt with by the
				block manager
 
*******************************************************************************/

tBlkHandle mmibm_Initialise( tBlkHandle BlkBase, int NumEntries )
{
	pManagerControl MyControl;
	tBlkId          Entry;
	int i;

	if ( ( MyControl = (pManagerControl) ALLOC_MEMORY( sizeof(tManagerControl) ) ) != NULL )
	{
		/* Fill in the bits we know are pretty constant
		*/
		MyControl->BlkKey      = BLOCK_MANAGER_KEY;
		MyControl->BaseAddress = BlkBase;
		MyControl->BlkLength   = NumEntries;
		MyControl->EntryPtr    = (pManagerEntry) BlkBase;

		/* Initialise the handle array to empty initially
		*/
		for ( i = 0; i < BLOCK_MANAGER_ENTRY_COUNT; i++ )
			MyControl->RsrcList[i] = NULL;

		/* Now for each entry in the incoming block list we can create
		   a handler instance
		*/
		for ( i = 0; i < NumEntries; i++ )
			if ( ( Entry = MyControl->EntryPtr[i].Id ) < BLOCK_MANAGER_ENTRY_COUNT )
				MyControl->RsrcList[ Entry ] = mmibr_Initialise( 
				    MyControl->EntryPtr[i].EntryBase, MyControl->EntryPtr[i].NumEntries );
	}

	return MyControl;
}


/*******************************************************************************

 $Function:    	mmibm_ShutDown

 $Description:	Shutdown routine to deallocate resources ina controlled manner

 $Returns:		none.

 $Arguments:	*BlkHandle, pointer to resource manager handle
 
*******************************************************************************/

void mmibm_ShutDown( tBlkHandle *BlkHandle )
{
	/* Convert and verify the incoming handle
	*/
	pManagerControl MyControl = (pManagerControl) *BlkHandle;
	int i;

	if ( ValidManagerResource( *BlkHandle ) )
	{
	    /* Clear down the allocated resource managers
	    */
	    for ( i = 0; i < BLOCK_MANAGER_ENTRY_COUNT; i++ )
		    if ( MyControl->RsrcList[i] != NULL )
			    mmibr_ShutDown( &MyControl->RsrcList[i] );

		/* and free the resource manager handle
		*/
        free( *BlkHandle );
	    *BlkHandle = NULL;
	}
}


/*******************************************************************************

 $Function:    	mmibm_SupplyResourceHandler

 $Description:	

    Since this module will be managing each of the block handlers
    for each of the resources, we need to be able to supply the
    appropriate handle for any given type to the calling routine.

 $Returns:		Handle to requesteb block resource handler, NULL if failure

 $Arguments:	ManagerHandle, handle of the block manager
                Id, identifier of the resource table for which the resource
				handler is required
 
*******************************************************************************/

tBlkHandle mmibm_SupplyResourceHandler( tBlkHandle ManagerHandle, tBlkId Id )
{
	/* Convert and verify the incoming handle
	*/
	pManagerControl MyControl = (pManagerControl) ManagerHandle;
	if ( ! ValidManagerResource( ManagerHandle ) )
		return NULL;

	/* Verify the id is within the range we expect
	*/
	if ( ( Id >= 0 ) && ( Id < BLOCK_MANAGER_ENTRY_COUNT ) )
		return MyControl->RsrcList[Id];

	/* Okay, we have something invalid, so return NULL
	*/
	return NULL;
}



/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

