#ifndef _MMI_BlkResources_H_
#define _MMI_BlkResources_H_

#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY  mfwFree

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBlkResources.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    The block resource manager handles a list of blocks of information,
    passing back pointers to the block of data on request. It provides
    a mechanism for mapping identifiers to strings, bitmaps, melodies
    or any other block of information.
                        
********************************************************************************

 $History: MmiBlkResources.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/


/* Define the prototypes for the basic routines
*/
typedef void    *tBlkHandle;
typedef long int tBlkId;

/* The initialisation routine must be called as part of the startup
   phase of the system, it will allocate working space for the block
   handler if required, and will initialise any structures required
   to maintain the correct operation of the functions. This routine
   must be called prior to invocation of any other block resource
   function
*/
tBlkHandle mmibr_Initialise( tBlkHandle BlkBase, int NumEntries );

/* The shutdown function can be called to free any allocations set up
   by the Initialise routine. In a running system this is unlikely to
   be called unless a catastrophic error has occurred and the system
   needs to be restarted.
*/
void mmibr_ShutDown( tBlkHandle *BlkRsrc );

/* We will adopt a mechanism where each block of data being provided
   will need to be returned to the block resource manager when it
   is no longer being used, this will provide an orthogonal approach
   when dealing with data coming from either ROM or dynamically
   allocated memory
*/
tBlkHandle mmibr_Fetch( tBlkHandle BlkRsrc, tBlkId Id );

/* Added to remove warning Aug - 11 */
EXTERN U8 *mfwAlloc (U32 size);
/* End - Remove warning */

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
