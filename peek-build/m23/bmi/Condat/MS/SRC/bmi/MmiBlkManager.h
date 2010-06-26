#ifndef _MMI_BLKMANAGER_H_
#define _MMI_BLKMANAGER_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBlkManager.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    The block manager is responsible for creating and initialising
    the structures and tables to allow access to the strings, icons
    sounds and vibrations required by the MMI.
   
    Each of these items is effectively a contiguous block of memory,
    which is accessed via a block resource. Each of the block resources
    are provided by this package

                        
********************************************************************************
 $History: MmiBlkManager.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/


#include "MmiBlkResources.h"


/* Define the block resources we are dealing with, for now just
   identify the sounds icons and vibrations, and fill the rest
   with languages. We don't actually care what these are for now,
   they're just placeholders
*/

#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY  mfwFree

#define MMI_LANG_0  0
#define MMI_LANG_1  1
#define MMI_LANG_2  2
#define MMI_LANG_3  3
#define MMI_LANG_4  4
#define MMI_LANG_5  5
#define MMI_LANG_6  6
#define MMI_LANG_7  7
#define MMI_LANG_8  8
#define MMI_LANG_9  9
#define MMI_LANG_10 10
#define MMI_LANG_11 11
#define MMI_LANG_12 12
#define MMI_SOUNDS  13
#define MMI_ICONS   14
#define MMI_MOVES   15


/* Define initialise and shutdown methods
*/
tBlkHandle mmibm_Initialise( tBlkHandle BlkBase, int NumEntries );
void mmibm_ShutDown( tBlkHandle *BlkHandle );


/* Since this module will be managing each of the block handlers
   for each of the resources, we need to be able to supply the
   appropriate handle for any given type to the calling routine.
*/
tBlkHandle mmibm_SupplyResourceHandler( tBlkHandle ManagerHandle, tBlkId Id );

/* Added to remove warning Aug - 11 */
EXTERN U8 *mfwAlloc (U32 size);
/* End - Remove warning */

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
