/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBlkResources.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This modules provides, in conjunction with the MmiBlkManager module,
	the resource management facilities for the MMI.
                        
********************************************************************************

 $History: MmiBlkResources.c

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

#include "MmiBlkResources.h"

/*******************************************************************************
                                                                              
                                Local Structures                              
                                                                              
*******************************************************************************/

/* Define the control structures used to implement the block resource
   manager
*/
#define RESOURCE_MANAGER_KEY	0x00BABE02L
#define RESOURCE_SEARCH_LIMIT	4

typedef struct _tBlkEntry_
{
	tBlkId     BlkId;
	tBlkHandle BlkHandle;
} tBlkEntry, *pBlkEntry;

typedef struct _tBlkControl_
{
	long int   BlkKey;
	tBlkHandle BlkBase;
	int        BlkLength;
	pBlkEntry  BlkStartPtr;
	tBlkHandle BlkMinAddress;
	tBlkHandle BlkMaxAddress;
} tBlkControl, *pBlkControl;


/*******************************************************************************
                                                                              
                                Private Routines                               
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	ValidBlockResource

 $Description:	Verifies that the resource indicated by the handle is a valid
                resource.

 $Returns:		0 If invalid, non-zero if valid.

 $Arguments:	BlkRsrc, handle of resource
 
*******************************************************************************/


static int ValidBlockResource( tBlkHandle BlkRsrc )
{
	pBlkControl BlkControl = (pBlkControl) BlkRsrc;

	/* Check for non-NULL handle
	*/
	if ( BlkControl == NULL )
		return 0;

	/* Check key has been set up correctly
	*/
	return ( BlkControl->BlkKey == RESOURCE_MANAGER_KEY );
}


/*******************************************************************************
                                                                              
                                Public Routines
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	mmibr_Initialise

 $Description:

   The initialisation routine must be called as part of the startup
   phase of the system, it will allocate working space for the block
   handler if required, and will initialise any structures required
   to maintain the corect operation of the functions. This routine
   must be called prior to invocation of any other block resource
   function

 $Returns:		Handle to a resource block, NULL if unsuccessful

 $Arguments:	BlkBase, Base address of the block of data relating
                to the resource
				NumEntries, number of entries associated with the resource
 
*******************************************************************************/

tBlkHandle mmibr_Initialise( tBlkHandle BlkBase, int NumEntries )
{
	pBlkControl Blk;
	int i;

	/* As part of the initialisation process, we need to allocate a
	   block of memory in which to store the control information
	   associated with this block resource manager
	*/
	if ( ( Blk = (pBlkControl) ALLOC_MEMORY( sizeof(tBlkControl) ) ) != NULL )
	{
		/* Store the things we know straight off
		*/
		Blk->BlkKey      = RESOURCE_MANAGER_KEY;
		Blk->BlkBase     = BlkBase;
		Blk->BlkLength   = NumEntries;
		Blk->BlkStartPtr = (pBlkEntry) BlkBase;

		/* In order to detect memory allocations, we scan the list
		   of known entries, storing the maximum and minimum addresses
		   in the list. This scan allows us to detect when allocated
		   memory is being returned, since the address will be outside
		   the contiguous memory block we are managing.
		*/
		Blk->BlkMinAddress = Blk->BlkMaxAddress = BlkBase;
		for ( i = 0; i < NumEntries; i++ )
		{
			if ( Blk->BlkMaxAddress < Blk->BlkStartPtr[i].BlkHandle )
				Blk->BlkMaxAddress = Blk->BlkStartPtr[i].BlkHandle;
		}
	}

	return Blk;
}

/*******************************************************************************

 $Function:    	mmibr_ShutDown

 $Description:

   The shutdown function can be called to free any allocations set up
   by the Initialise routine. In a running system this is unlikely to
   be called unless a catastrophic error has occurred and the system
   needs to be restarted.

 $Returns:		none.

 $Arguments:	Pointer to a block resource handle (ie ptr to ptr)
 
*******************************************************************************/

void mmibr_ShutDown( tBlkHandle *BlkRsrc )
{
	/* Only allow the resource manager to be shutdown if the provided
	   handle is consistent
	*/
	if ( ValidBlockResource( *BlkRsrc ) )
	{
		free( *BlkRsrc );
		*BlkRsrc = NULL;
	}
}

/*******************************************************************************

 $Function:    	mmibr_Fetch

 $Description:

   We will adopt a mechanism where each block of data being provided
   will need to be returned to the block resource manager when it
   is no longer being used, this will provide an orthogonal approach
   when dealing with data coming from either ROM or dynamically
   allocated memory.

 $Returns:		pointer to resource, NULL if unsuccessful

 $Arguments:	BlkRsrc, resource handle created by mmibr_Initialise
                Id, identifier of the resource to be returned
 
*******************************************************************************/

tBlkHandle mmibr_Fetch( tBlkHandle BlkRsrc, tBlkId Id )
{
	int        Start, End, Search;
	tBlkId     CurrentId;
	tBlkHandle SearchPtr;

	/* convert the handle and verify it's valid
	*/
	pBlkControl BlkControl = (pBlkControl) BlkRsrc;
	if ( ! ValidBlockResource( BlkRsrc ) )
		return NULL;

	/* When locating a specific entry, we need to search the list of
	   ids for one matching the input value. Since the Ids will be
	   organised as an ordered list, very important that bit, we can
	   perform a simple binary search to locate the items.
	*/
	Start     = 0;
	End       = BlkControl->BlkLength - 1;
	SearchPtr = NULL;
	do
	{
		/* grab the entry midway between the current start and end
		*/
		Search    = Start + ((End - Start) >> 1);
		CurrentId = BlkControl->BlkStartPtr[Search].BlkId;

		/* Binary chop the search space
		*/
		if ( CurrentId == Id )
		{
			/* Found a match, grab handle and terminate search
			*/
			SearchPtr = BlkControl->BlkStartPtr[Search].BlkHandle;
			Start = End;
		}
		else if ( CurrentId > Id )
		{
			/* Not got a match, but it's not in the top half so move
			   the End pointer down
	        */
			End = Search;
		}
		else
		{
			/* Not got a match, but it's not in the bottom half so
			   move the Start pointer up
			*/
			Start = Search;
		}

		/* when we get down to the last three or four entries, just
		   search linearly to solve it, this is generally quicker for
		   a small number of entries than continuing the binary chop
		*/
		if ( ( End - Start ) < RESOURCE_SEARCH_LIMIT )
		{
			/* search quickly through the possibles
			*/
			for ( Search = Start; Search <= End; Search++ )
				if ( Id == BlkControl->BlkStartPtr[Search].BlkId )
  			        SearchPtr = BlkControl->BlkStartPtr[Search].BlkHandle;

			/* And terminate the binary chop
			*/
			Start = End;
		}

	} while ( Start != End );

	return SearchPtr;
}

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

