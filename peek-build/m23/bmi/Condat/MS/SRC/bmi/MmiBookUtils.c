/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		PhoneBook
 $File:		    MmiBookUtils.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    This module provides utility function for use in the
    phone book application
                        
********************************************************************************
 $History: MmiBookUtils.c
        $History: MmiBookUtils.c
        
		Aug 09, 2007 DR:OMAPS00140447 x0056422(Srilekha)
		Description: PhoneBook: Unexpected confirmation message when making changes to entries in SIM storage.
		Solution: Added a new flag changename_flag to handle the Phonebook-SIM Memory entry changed event.

 	17 Aug, 2007  DR: OMAPS00141963 x0066814(Geetha) 
 	Description: PhoneBook: The "Options" menu is not returned after a current entry is copied.
	 Solution: After copying, the menu control was not being returned back to the right window.
 	Added a callback, that will be called after copying an entry, where the control wil be returned to
	 correct window. 

     July 11, 2007 DR:OMAPS00137908 x00666692(Asha)
     Description:PHONEBOOK: changing name or number under SIM storage, Please Wait message 
    appears instead of Saved
    Solution     :Changes made to handle the condition MFW_PHB_EXCT in the function changeInFile() 
       	which modifies the phone book entry.
        
        27 June, 2007  DR: OMAPS00136608 x0073106(sundeep) 
        Description:PHONEBOOK-Unexpected behavior when copying all entries from Phone to SIM
        Solution     :Updated the execution state(MFW_PHB_EXCT) in the phonebook.
        
       June 27, 2007 DR:OMAPS00136627 x0062172(Syed)
    		Description:PHONEBOOK: Unexpected behavior when moving all entries from Phone to SIM
             Solution     : Return Proper Value after phb_store_entry() in bookCopy  to move all the entries 
	       			properly.
        
       June 05, 2007 DR:OMAPS00133241 x0061560(sateeshg)
       Description:ASTEC29607,SIM:The sim card should not return 9240 still.
       Solution     :MMI should not display "Saved" after db is updated but has to wait until SIM is 
       			updated with the entry and RAT_OK is received from ACI.
       

 	June 06, 2007 DR:OMAPS00132045 x0073106 Sundeep
 	Description : Phonebook- behaviour inconsistency in the MMI when move contact
 	Solution     : changed a if condition in function bookCopyMultiple(), in the if(move).
 	

       JUNE 4,07 (sateeshg) OMAPS00131777, x0061560 
       Phonebook - Delete all command for phonebook contacts answer "list empty" even if there are contacts in the list
	Solution:Updated the current status of the phonebook
	
	Apr 30,2007, OMAPS00129173, x0066814(Geetha)
	Description : Phone crashes if 'Delete All' is done on SIM contacts when SIM memory is empty
	Solution: Added a condition in bookDeleteBook to check the number of used entries in the seleted phonebook.
	If the condition is satified then the delete operation will proceed else an 'Empty List' dialog is displayed.
 
       Nov 17, 2006 DR:OMAPS00104376 a0393213(R.Prabakar)
       Description : When tried to delete the last entry either incase of SIM or Phone book (i.e. last deletion), the board resets.
       Solution     : The crash happens because of race condition between actual deletion by ACI and status check in BMI. 
       Now the status before deletion is stored and status check is done on this previous status.

	Aug 18, 2006   ER: OMAPS00089840  x0039928
	Description: Changes to be made in MMI for the functionality "Delete all" ADN entries
	Solution: A new menu item "Delete All" is added in the phone book menu and bookDeleteBook
	is called to delete all the phone book entries.

	July  17, 2006  DR: OMAPS00082792  x0039928
	Description:Changes to be done in MMI for Move all entries from SIM to FFS (Defect 76492)
	Solution: Condition to delete search window in deleteFromFile is modified.

	June 01, 2006  DR: OMAPS00079716
	Description:Phonebook: List of entries is not refreshed after a phonebook entry move
	Solution: Added a new callback function to handle the populating of phonebook entries
	
	Feb 20, 2006 REF: OMAPS00061948  x0039928  
	Bug:PhoneBook-My number menu -failed.
 	Fix: phb_read_entries is called with used entries as the no of entries if used entries are less than 
 	MAX_SEARCH_NAME.

	Jan 27, 2006 REF: OMAPS00061925  x0039928  
	Bug:Phonebook-Changing an entry and Immidiately deleting the entry failed.
 	Fix: Phonebook is populated with phone numbers after some time and not immediately after deleting an entry.

	Jan 22, 2006 REF: OMAPS00061930  x0039928  
	Bug:PhoneBook -Change entry and view entry through Change option-failed
 	Fix: Phonebook is populated with phone numbers after some time and not immediately while saving the changed entries.
	 	
	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

// Issue Number : SPR#15671 on 24/03/04 by Ajith K.P
 
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

#include "mfw_sys.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
#include "mfw_lng.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_tim.h"

#include "mfw_sim.h"
#include "mfw_cm.h"
#include "mfw_nm.h"
#include "mfw_phb.h"
#include "mfw_mme.h"
#include "mfw_sat.h"
#include "mfw_sms.h"

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "MmiMain.h"
#include "MmiBookController.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"



#include "MmiMenu.h"
#include "mmiCall.h"
#include "Mmiicons.h"
#include "MmiIdle.h"

#include "MmiSoftKeys.h"
#include "MmiSounds.h"
#include "MmiIdle.h"
#include "MmiNetwork.h"
#include "mmiSat_i.h"
#include "MmiAoc.h"

#include "gdi.h"
#include "audio.h"

#include "cus_aci.h"
#include "mfw_ffs.h"
#include "MmiTimers.h"

#include "MmiBookShared.h"

#include "mmiColours.h"
#include "mmiSmsRead.h"/*MC , SPR 1242 */

//  May 24, 2004 REF: CRR MMI-SPR-15813 Ajith K P
//  Issue description: Followup from 15623, this is the BMI part of this Issue.
//	At last, in the record "SSC09" the number is displayed with the "+" 
//	character at the beginning of the number instead of "**21*+014543877989#".
 

extern int menuFDN;
extern T_call call_data;
// Jan 27, 2006 REF: OMAPS00061925  x0039928
// Fix : Declare a extern phonebook timer handler
extern T_MFW_HND PhbSearchTim;

// Aug 18, 2006   ER: OMAPS00089840  x0039928
// Fix : Initialize Delete All flag and a window handler of please wait dialog.
BOOL DeleteAll_flag = FALSE;
T_MFW_HND waitWindel = NULL;
	//x0061560 Date May17,07 (sateeshg) OMAPS00133241
BOOL addname_flag = FALSE;
/* Aug 09, 2007 DR:OMAPS00140447 x0056422(Srilekha)		
Added a new flag changename_flag to handle the Phonebook-SIM Memory entry changed event. */

BOOL changename_flag = FALSE;
/*******************************************************************************
                                                                              
                                Private data elements
                                                                              
*******************************************************************************/

/* x0039928 - Lint warnings fix
static T_MFW_UPN_LIST   *upnList = NULL;   */

/* SPR#1112 - SH - This is redundant */
//static T_phbk           *GlobalPhoneBookHandle = NULL;

/* SPR#1112 - SH - Internal phonebook flag */
#ifdef INT_PHONEBOOK
static UBYTE ipbCopyIndex = 0;
static UBYTE ipbMove = FALSE;
static T_MFW_HND ipbWin = NULL;
#endif

// ADDED BY RAVI - 28-11-2005
EXTERN T_MFW     mfw_Ucs2SIMStr(T_MFW_DCS outMode, U16 *inUcsStr, UBYTE maxOutSize, 
                         UBYTE *outMfwStr, UBYTE *outLen);

EXTERN T_MFW     mfw_Gsm2SIMStr(T_MFW_DCS outMode, UBYTE *inGsmStr, UBYTE maxOutSize, 
                         UBYTE *outMfwStr,UBYTE *outLen);
// END RAVI - 28-11-2005
/*******************************************************************************
                                                                              
                                Private Methods
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	changeName

 $Description:	Change a phone book entry if we have a valid book selected
 
 $Returns:		status of change, or MWB_PHB_FAIL if an error occurs

 $Arguments:	book, to be updated
                entry, to update
                currentStatus, pointer to status word
 
*******************************************************************************/

static UBYTE changeName( UBYTE book, T_MFW_PHB_ENTRY *entry, T_MFW_PHB_STATUS *currentStatus )
{
	
	TRACE_FUNCTION( "changeName()" );

	/* Aug 09, 2007 DR:OMAPS00140447 x0056422(Srilekha) */
	changename_flag = TRUE;

	if ( book == PHB_SDN )
		return MFW_PHB_FAIL;

	return phb_store_entry( book, entry, currentStatus );		
}







/*******************************************************************************

 $Function:    	deleteName

 $Description:	Delete a phone book entry
 
 $Returns:		status of delete, or MWB_PHB_FAIL if an error occurs

 $Arguments:	book, to be updated
                index, index of entry to delete
                currentStatus, pointer to status word
 
*******************************************************************************/

static UBYTE deleteName( UBYTE book, UBYTE index, T_MFW_PHB_STATUS *currentStatus )
{
	TRACE_FUNCTION( "deleteName()" );

	if (  book == PHB_SDN )
		return MFW_PHB_FAIL;

	return phb_delete_entry( book, index, currentStatus );
}










/*******************************************************************************

 $Function:    	addName

 $Description:	Add a new name to the phone book
 
 $Returns:		status of change, or MWB_PHB_FAIL if an error occurs

 $Arguments:	book, to be updated
                entry, to update
                currentStatus, pointer to status word
 
*******************************************************************************/

static UBYTE addName( UBYTE book, T_MFW_PHB_ENTRY *entry, T_MFW_PHB_STATUS *currentStatus )
{
	TRACE_FUNCTION( "addName()" );

  //MC-- Commented out UPN-specific code as it causes the name string in a UPN
  //entry to be stored incorrectly and doesn't seem to be neccessary.
  //Generic code here seems to work fine for UPN
    {
        /* normal store operation
        */
	{
		char temp[120];
		memset(temp,'\0',100);
			//x0061560 Date May17,07 (sateeshg) OMAPS00133241
		addname_flag = TRUE;
#ifdef NO_ASCIIZ
{
	T_MFW_PHB_TEXT buf;

		/*SPR 1752 check for unicode*/
	   	if (entry->name.data[0] ==0x80)
		{
			/*MC, don't do any conversions!!*/
			TRACE_EVENT("Converting Unicode 2 SIM");

			sprintf(temp,"Book: %d Name %s Number %s",
				book,
				(char*)buf.data,(char*)entry->number);

		}
		else
		
		{
			// convert from GSM to Alpha characters
			bookGsm2Alpha( (UBYTE *) entry->name.data );

			sprintf(temp,"Book: %d Name %s Number %s",
				book,
				(char*)entry->name.data,(char*)entry->number);
		}
}
#else
		sprintf(temp,"Book: %d Name %s Number %s",
				book,
			(char*)entry->name,(char*)entry->number);
#endif
		TRACE_EVENT(temp);
	}
	    entry->index = 0;
		
        return phb_store_entry( book, entry, currentStatus );
    }
}








/*******************************************************************************
                                                                              
                          File Handling Utilities
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	storeInFile

 $Description:	Converts the name to a storeable format and stores it
                in the phone book
 
 $Returns:		None

 $Arguments:	phbk, pointer to phonebook structure
 
*******************************************************************************/

void storeInFile( T_phbk *phbk )
{	int i;
	UBYTE actual_length = 0;
	char debug[20];
	TRACE_FUNCTION( "storeInFile()" );

    /* Convert the name
    */
#ifdef NO_ASCIIZ

    /* if the pattern is GSM default format, use the function mfw_Gsm2SIMStr() here
    */


    if (phbk->edt_buf_name[0] == 0x80)//if unicode
    {
    		/*a0393213 lint warning:Possible creation of out-of-bounds pointer soln:PHB_MAX_LEN changed to MAX_ALPHA_LEN-1*/
   		 for (i =0; i < (MAX_ALPHA_LEN-1); i+=2) //work out string length 
   	 	{	if (phbk->edt_buf_name[i] == 0 && phbk->edt_buf_name[i+1] == 0)
   			{	if (i== 0)
   					actual_length =0;
   				else
   					actual_length = i -2;
    			break;
    		}
    	}
    	phbk->edt_buf_name[0] = (UBYTE)(actual_length/2);//add string length to beginning of string
    	phbk->edt_buf_name[1] = 0x00;
    /*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN*/
    memset(phbk->newEntry.name.data, 0,MAX_ALPHA_LEN);
    //convert to SIM string
	mfw_Ucs2SIMStr( MFW_DCS_UCS2 , (U16*)phbk->edt_buf_name, MAX_ALPHA_LEN,
	
        phbk->newEntry.name.data, &phbk->newEntry.name.len );
	TRACE_EVENT(	(char*)phbk->newEntry.name.data);
	sprintf(debug, "length:%d", phbk->newEntry.name.len);
	TRACE_EVENT(debug);
    }
    else
    {
    	mfw_Gsm2SIMStr( MFW_DCS_7bits, (U8*)phbk->edt_buf_name, MAX_ALPHA_LEN,
        phbk->newEntry.name.data, &phbk->newEntry.name.len );	
    }
   
#else
/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN*/
    strncpy( (char *)phbk->newEntry.name, (char *)phbk->edt_buf_name, MAX_ALPHA_LEN );

#endif

    /* update the number
    */
    strncpy( (char *) phbk->newEntry.number, (char *) phbk->edt_buf_number, PHB_MAX_LEN );
	/*SPR 1327, copy current phonebook type to new entry*/
	phbk->newEntry.book = phbk->current.status.book; 
	/*If current phonebook UPN, don't overwrite phonebook type*/
	// Feb 20, 2006 REF: OMAPS00061948  x0039928 
	// Fix : A check is made with enum PHB_UPN of MFW instead of UPN of ACI
	if( phbk->newEntry.book != PHB_UPN)
		phbk->newEntry.book = bookActiveBook(WRITE);
    phbk->newEntry.index = 0;

    /* Add the name/number combination to the phone book and setup
       the current text status string
    */
    switch ( addName( phbk->newEntry.book, &phbk->newEntry, &phbk->current.status ) )
    {
        case MFW_PHB_OK :
        {
            /* Entry saved okay
            */
            MmiBookSetCurrentText( TxtSaved  );
        }
        break;
	//x0061560 Date May16,07 (sateeshg) OMAPS00133241 
	case  MFW_PHB_EXCT :
        {
            /* Entry saved okay
            */
            MmiBookSetCurrentText( TxtPleaseWait  );
		//	MmiBookSetCurrentText( TxtSaved  );
      		
        }
	//end
        break;

        case MFW_PHB_FULL :
        {
            /* Phone book full
            */
            MmiBookSetCurrentText( TxtPhbkFull  );
        }
        break;

        case MFW_PHB_FAIL :
        {
            /* Write failure
            */
            MmiBookSetCurrentText( TxtWriteError  );
        }
        break;

		default:
        {
            /* No other returns possible
            */
        }
        break;
    }
}







/*******************************************************************************

 $Function:    	changeInFile

 $Description:	change the phone book entry
 
 $Returns:		none

 $Arguments:	phbk, pointer to phonebook structure
 
*******************************************************************************/

void changeInFile( T_phbk *phbk )
{	int i;
	int actual_length=0;
	TRACE_FUNCTION( "changeInFile()" );

	if(phbk->current.status.book == PHB_ADN_FDN)
		phbk->current.status.book = PHB_ADN;	//JVJE We can only store in ADN/FDN 
	else
		phbk->current.status.book = phbk->current.status.book;
	
	TRACE_EVENT_P1("Current Book = %d", phbk->current.status.book);
	TRACE_EVENT_P1("Current Name = %d",  phbk->current.selectedName);
	TRACE_EVENT_P1("Current Buf Name = %s", phbk->edt_buf_name);


    /* Convert the name
    */
#ifdef NO_ASCIIZ
	/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
    if (phbk->edt_buf_name[0] == 0x80)//if unicode
    {
    /*a0393213 lint warning:Possible access of out-of-bounds pointer soln:MAX_ALPHA_LEN-1*/
   		 for (i =0; i < MAX_ALPHA_LEN-1; i+=2) //work out string length
   	 	{	if (phbk->edt_buf_name[i] == 0 && phbk->edt_buf_name[i+1] == 0)
   			{	if (i== 0)
   					actual_length =0;
   				else
   					actual_length = i -2;
    			break;
    		}
    	}
    	phbk->edt_buf_name[0] = (UBYTE)(actual_length/2);//add string length to beginning of string
    	phbk->edt_buf_name[1] = 0x00;
    
      memset(phbk->current.entry[phbk->current.selectedName].name.data, 0, MAX_ALPHA_LEN);
    //convert to SIM string
	mfw_Ucs2SIMStr( MFW_DCS_UCS2, (U16*)phbk->edt_buf_name, MAX_ALPHA_LEN,
        phbk->current.entry[phbk->current.selectedName].name.data,
        &phbk->current.entry[phbk->current.selectedName].name.len );
    }
else
    {	mfw_Gsm2SIMStr( MFW_DCS_7bits, (U8*)phbk->edt_buf_name, MAX_ALPHA_LEN,
        phbk->current.entry[phbk->current.selectedName].name.data, &phbk->current.entry[phbk->current.selectedName].name.len );
	}
#else

    strncpy( (char *) phbk->current.entry[ phbk->current.selectedName ].name,
        (char *) phbk->edt_buf_name, MAX_ALPHA_LEN );

#endif


/* update the number
    */
    strncpy( (char *) phbk->current.entry[ phbk->current.selectedName ].number,
        (char *) phbk->edt_buf_number, PHB_MAX_LEN );

    /* Add the name/number combination to the phone book and setup
       the current text status string
    */
    switch ( changeName( phbk->current.status.book,
        &phbk->current.entry[ phbk->current.selectedName ], &phbk->current.status ) )
    {
        case MFW_PHB_OK :
        {
            /* Entry saved okay
            */
            MmiBookSetCurrentText( TxtSaved  );
        }
        break;

        case MFW_PHB_FAIL :
        {
            /* Write failure
            */
            MmiBookSetCurrentText(  TxtWriteError );
        }
        break;
		/* x0056422 on Aug 02,2007 for 
		   OMAPS00140447 - PhoneBook: Unexpected confirmation message 
		   when making changes to entries in SIM storage. */

//  July 11, 2007 DR:OMAPS00137908 x00666692(Asha)
//PHONEBOOK: changing name or number under SIM storage, Please Wait 
//message appears instead of Saved
	case  MFW_PHB_EXCT :
        {
            /* Operation in progress
            */
            MmiBookSetCurrentText( TxtPleaseWait  );
		//	MmiBookSetCurrentText( TxtSaved  );
      		
        }
	//end
        break;

		default:
        {
            /* No other returns possible
            */
        }
        break;
    }
}









/*******************************************************************************

 $Function:    	deleteFromFile

 $Description:	delete an entry from a phone book
 
 $Returns:		None

 $Arguments:	phbk, phone book handle,
                book, book to update,
 
*******************************************************************************/

void deleteFromFile( T_phbk *phbk, UBYTE book )
{
	int index;
	T_MFW_PHB_STATUS phb_status_before_delete; /*OMAPS00104376 a0393213(R.Prabakar)*/
	
	TRACE_FUNCTION( "deleteFromFile()" );

	index = phbk->current.selectedName - phbk->current.missedCallsOffset;
	/*OMAPS00104376 a0393213(R.Prabakar)- getting the status of the phonebook before deletion*/
	phb_status_before_delete.book=book;
	bookGetCurrentStatus( &phb_status_before_delete );
    switch ( deleteName( book,
        phbk->current.entry[index].index, &phbk->current.status ) )
    {
        case MFW_PHB_OK :
        {
            /* Entry saved okay
            */
            MmiBookSetCurrentText(  TxtEntryDeleted  );

#ifdef NO_ASCIIZ
				phbk->current.entry[index].name.len = 0;
				memset((char*)phbk->current.entry[index].number, '\0', PHB_MAX_LEN); 

#else
/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
				memset((char*)phbk->current.entry[index].name, '\0', MAX_ALPHA_LEN);
				memset((char*)phbk->current.entry[index].number, '\0', PHB_MAX_LEN); 
#endif
            /* Move the current index to the next valid entry
            */
				if(phbk->current.selectedName > 0)
					phbk->current.selectedName --;
				else
					phbk->current.selectedName = 0;
				
            	if(phbk->current.index > 1)
            		phbk->current.index--;
            	else
                	phbk->current.index = 1;

            /* And if we have a search window, perform the search to
               update the current valid entry
            */
			if ( phbk->search_win )
            {

		// July  17, 2006  DR: OMAPS00082792  x0039928
		// Check used entry is 1 as delete is not complete yet as it is asynchronous.
		/*Nov 17,2006 OMAPS00104376 a0393213(R.Prabakar)
		    Now the check is done with the status got before deletion instead of the status got after deletion*/
				if (phb_status_before_delete.used_entries == 1)
                {
                    /* empty phone book
                    */
					bookSearchDestroy( phbk->search_win );
					phbk->search_win = 0;
				}
				else
                {
                    /* perform the search
                    */
			//Jan 27, 2006 REF: OMAPS00061925  x0039928
			// Fix : Start the phonebook timer for populating the list
			timStart(PhbSearchTim);
                }
			}
        }
        break;

        case MFW_PHB_FAIL :
        {
            /* Write failure
            */
            MmiBookSetCurrentText(  TxtWriteError );
        }
        break;

		default:
        {
            /* No other action required
            */
        }
        break;
    }
}


/* SPR#1112 - SH - Internal phonebook use */
#ifdef INT_PHONEBOOK

/*******************************************************************************

 $Function:    	bookChangePB

 $Description:	Change the phonebook
 
 $Returns:		None

 $Arguments:	None.
 
*******************************************************************************/

void bookChangePB(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
	T_MFW_WIN			*win_data   = ( (T_MFW_HDR *) win )->data;
    T_phbk				*Phbk		=	(T_phbk *) win_data->user;
    
	GI_pb_SetPhonebook(identifier);
	bookShowInfoScreen( Phbk->win, TxtPhonebook, TxtChanged, NULL, NULL, 3000);
	return;
}


/*******************************************************************************

 $Function:    	bookCopy

 $Description:	Copy or move a phonebook entry from one book to another
 
 $Returns:		status of change, or MWB_PHB_FAIL if an error occurs

 $Arguments:	entry 		- the entry to be moved or copied
 				srcStatus	- Status information about the source phonebook
                destStatus 	- Status information about the destination phonebook
                move		- TRUE if the original record is to be deleted
 
*******************************************************************************/

UBYTE bookCopy( T_MFW_PHB_ENTRY *entry, T_MFW_PHB_STATUS	*destStatus, T_MFW_PHB_STATUS	*srcStatus, UBYTE move )
{
    T_MFW				ret;
    UBYTE				srcIndex;
    UBYTE				srcBook;
 
	TRACE_FUNCTION("bookCopy");
		
	if (!destStatus->avail_entries)
	{
		/* There's no room in the destination phonebook to add the entry */
		TRACE_EVENT("bookCopy: dest phonebook full");
		return MFW_PHB_FULL;
	}

	srcIndex = entry->index;
	entry->index = 0;

	if (entry->name.len>destStatus->tag_len)
	{
		TRACE_EVENT("bookCopy: Alpha tag cropped");
		entry->name.len = destStatus->tag_len;
		entry->name.data[entry->name.len] = NULL;
	}
	
    ret = phb_store_entry( destStatus->book, entry, destStatus );
    TRACE_EVENT_P2("bookCopy: phb_store_entry book %d result %d", destStatus->book, ret);
    
// 27 June, 2007 DR:OMAPS00136627 x0062172 Syed
    if ( (ret!=MFW_PHB_OK) && (ret!=MFW_PHB_EXCT) ) 
    	return ret;

	/* Only try to delete the entry if we are moving an entry from one book
	 * to another.  Don't try if it is an FDN entry */

	TRACE_EVENT_P2("move %d, entry->book %d", move, entry->book);
	 
	if (move && entry->book!=PHB_FDN)
	{
    	if  (( srcStatus->book == PHB_UPN) || (srcStatus->book == PHB_FDN ))
            srcBook = srcStatus->book;
      	else
       		srcBook = bookActiveBook(WRITE);

		ret = phb_delete_entry(srcBook, srcIndex, srcStatus);
		TRACE_EVENT_P3("bookCopy: phb_delete_entry book %d index %d result %d", srcStatus->book, srcIndex, ret);
	}

   /* 30 Aug ,2007 DR:OMAPS00143539 ,x0081909 ( Deepak ) */

   /* If we copy an entry and then try to delete it from either SIM or Phone storage,before delete opertaion Index of an entry is restored  */
   
	entry->index = srcIndex;
	
	TRACE_EVENT("bookCopy: leaving function");
	
	return ret;
}


/*******************************************************************************

 $Function:    	bookDelMoveCB

 $Description:	callback after moving a phonebook entry
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
//x0pleela 01 june, 2006  DR: OMAPS00079716

static void bookDelMoveCB( T_MFW_HND win, USHORT Identifier,UBYTE reason)
{
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *) win)->data;
    T_phbk				*Phbk		=	(T_phbk *) win_data->user;

	bookGetCurrentStatus(&Phbk->current.status);
	bookMenuDestroy(Phbk->menu_options_win);
	if( Phbk->current.status.used_entries > 0 )
	{
		  SEND_EVENT( Phbk->search_win, SEARCH_STRING, 1, 0);
		    winShow(Phbk->search_win );
	} 
	else
	{
		if( Phbk->search_win)
		{
			 //destroy current window
		 	bookSearchDestroy(Phbk->search_win );	
		}
	}

}
/*******************************************************************************

 $Function:    	bookCopyCB

 $Description:	callback after Copying a phonebook entry
 
 $Returns:		

 $Arguments:	
 
*******************************************************************************/
//x0066814(Geetha) 17 Aug, 2007  DR: OMAPS00141963

static void bookCopyCB( T_MFW_HND win, USHORT Identifier,UBYTE reason)
{
    T_MFW_WIN		*win_data	= ((T_MFW_HDR *) win)->data;
    T_phbk				*Phbk		=	(T_phbk *) win_data->user;
	
    bookMenuDestroy(Phbk->menu_options_win);
	
}
/*******************************************************************************
 $Function:    	bookCopySingle

 $Description:	Copy or move a single phonebook entry
 
 $Returns:		None

 $Arguments:	None.
 
*******************************************************************************/

void bookCopySingle(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
	T_MFW_WIN			*win_data   = ( (T_MFW_HDR *) win )->data;
    T_phbk				*Phbk		=	(T_phbk *) win_data->user;
    BOOL				oldBook;
	int					txtId;
    T_MFW_PHB_STATUS	destStatus;
    tMmiPhbData			*current = &Phbk->phbk->current;

	TRACE_FUNCTION("bookCopySingle");
	
	oldBook = bookGetBookSelected();
// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
//	While doing a copy from the FDN menu the destination should always be internal phonebook
	if(menuFDN && (call_data.calls.numCalls==0))
	{
		destStatus.book = PHB_IPB;
	}
	else
	{
		bookSetBookSelected(!oldBook);
	destStatus.book = bookActiveBook(WRITE);
	}

	bookGetCurrentStatus( &destStatus );

	bookSetBookSelected(oldBook);
  	txtId = TxtStored;

  	if (destStatus.avail_entries)
  	{
		switch (bookCopy(&current->entry[current->selectedName], &destStatus, &current->status, identifier))
		{
			case MFW_PHB_OK:
			//27 June, 2007  DR: OMAPS00136608 x0073106(sundeep) 	
			case MFW_PHB_EXCT:
				txtId = TxtStored;
				break;
			case MFW_PHB_FULL:
				txtId = TxtPhbkFull;
				break;
			default:
				txtId = TxtFailed;
				break;
	    }
  	}
	else
	{
		txtId = TxtPhbkFull;
	}

	//x0pleela 01 June, 2006  DR: OMAPS00079716
	//Calling a callback for move operation
	if(identifier)
		bookShowInformation( win, txtId, NULL, (T_VOID_FUNC)bookDelMoveCB );
	else
	//x0066814(Geetha) 17 Aug, 2007  DR: OMAPS00141963
		bookShowInformation( win, txtId, NULL, (T_VOID_FUNC)bookCopyCB );
	/*if (identifier)
	{
		// Destroy options menu and regenerate address list 
		
		if(Phbk->current.status.used_entries > 0)
			SEND_EVENT(Phbk->search_win, SEARCH_SCROLL_UP, 0, 0 );
		SEND_EVENT(Phbk->search_win, SEARCH_UPDATE, 0, (void*)Phbk->current.status.book );
	}*/
			
    return;
}


/*******************************************************************************

 $Function:    	bookCopyMultiple

 $Description:	Copy or move multiple phonebook entries
 
 $Returns:		None

 $Arguments:	None.
 
*******************************************************************************/

void bookCopyMultiple(T_MFW_HND win, UBYTE move, UBYTE reason)
{
	T_MFW_WIN			*win_data   = ( (T_MFW_HDR *) win )->data;
    T_phbk				*Phbk		=	(T_phbk *) win_data->user;
    UBYTE				oldBook;
    T_MFW_PHB_STATUS	destStatus;
    T_MFW_PHB_STATUS	srcStatus;
    T_MFW_PHB_LIST		entries;
    T_MFW_PHB_ENTRY		entry;
	UBYTE				readIndex;
	int 				txtId;
//	T_MFW_HND			info_win;    // RAVI
	static T_MFW_HND	waitWin = NULL;
    /* Set up some data */

    TRACE_FUNCTION("bookCopyMultiple");
    
	entries.entry = &entry;

	srcStatus.book = bookActiveBook(WRITE);
	bookGetCurrentStatus( &srcStatus );

		oldBook = bookGetBookSelected();
	bookSetBookSelected(!oldBook);

	destStatus.book = bookActiveBook(WRITE);
	bookGetCurrentStatus( &destStatus );

	bookSetBookSelected(oldBook);

	/* First time entering function, set up values.
	 * Store information so this function can be called again
	 * when the phonebook is ready */

	if (ipbCopyIndex==0)
	{
		ipbCopyIndex = srcStatus.used_entries;
		ipbWin = win;
		ipbMove = move;

		if (!bookGetBookSelected() && !move)
		{
			/* We need to drop out of this function briefly in order to get the dialog to display
			 * for this particular case!  Show info screen very briefly, which calls this function as
			 * a callback.  The info screen remains until we drop out of this function a second
			 * time */
			waitWin = NULL;
			bookShowInfoScreen( Phbk->win, TxtPleaseWait, NULL, (T_VOID_FUNC)bookCopyMultiple, NULL, 100);
			return;
		}
		waitWin = bookShowInfoScreen( Phbk->win, TxtPleaseWait, NULL, NULL, NULL, FOREVER);
	}
	
	TRACE_EVENT_P1("Entries in list: %d", srcStatus.used_entries);

	/* For special case where SIM will give no E_PHB_READY response, the
	 * do...while operates as a loop below.  Otherwise, it will only go through
	 * once, and this function will be called repeatedly every E_PHB_READY */

	do
	{
		txtId = TxtStored;

		if (!destStatus.avail_entries)
		{
			txtId = TxtPhbkFull;
		}
		else if (ipbCopyIndex==0)
		{
			txtId = TxtEmpty;
		}
		else
		{
			/* For moving, always delete first entry alphabetically, list moves down.
			 * For copying, move through the list */
			 
			if (move)
				readIndex = 1;
			else
				readIndex = ipbCopyIndex;
			
			TRACE_EVENT_P1("Moving entry %d", readIndex);

			/* Read in entry and copy/move it */

			phb_read_entries(srcStatus.book, readIndex, MFW_PHB_ALPHA, 1, &entries);

			switch (bookCopy(entries.entry, &destStatus, &srcStatus, move))
			{
				case MFW_PHB_OK:
				//27 June, 2007  DR: OMAPS00136608 x0073106(sundeep) 
				case MFW_PHB_EXCT:
				  	txtId = TxtStored;
					break;
				case MFW_PHB_FULL:
					txtId = TxtPhbkFull;
					break;
				default:
					txtId = TxtFailed;
					break;
		    }
			
			ipbCopyIndex--;
		}
	}/* Is a loop in special case - see above */
	while (!bookGetBookSelected() && !move && txtId==TxtStored && ipbCopyIndex>0); 
	
	/* If we've finished, or an error has occurred, show info dialog */
	
	if (ipbCopyIndex==0 || txtId!=TxtStored)
	{
		ipbCopyIndex = 0;

		/* Destroy the Please Wait window and show the result message */
			
    	bookShowInformation( win, txtId, NULL, NULL );
		if (waitWin)
		{
			SEND_EVENT(waitWin, DIALOG_DESTROY,0,0);
		}

		if (move)
		{	
			/* Destroy options menu and regenerate address list */
		
			bookMenuDestroy(Phbk->menu_options_win);
			bookGetCurrentStatus( &Phbk->current.status );
			//06 June, 2007  DR: OMAPS00132045
			//changed the condition from used_entries to ipbCopyIndex
			if((ipbCopyIndex > 0))
			{
				SEND_EVENT(Phbk->search_win, SEARCH_SCROLL_UP, 0, 0 );
				SEND_EVENT(Phbk->search_win, SEARCH_UPDATE, 0, (void*)Phbk->current.status.book );
			}
			else
				bookSearchDestroy(Phbk->search_win);
		}
	}
	
	return;
}
#endif



/*******************************************************************************
                                                                              
                                Public Methods
                                                                              
*******************************************************************************/




/*******************************************************************************

 $Function:    	bookCurrentWindow

 $Description:

   This returns the window handle associated with the current
   MFW element
 
 $Returns:		window handle of current MFW item

 $Arguments:	none.
 
*******************************************************************************/

tBookMfwHnd bookCurrentWindow( void )
{
	return mfwParent( mfwHeader() );
}







/*******************************************************************************

 $Function:    	bookDefaultCallBack

 $Description:	

   This is a simple call back function which is invoked when
   a not implemented dialog is displayed. We don't want anyone
   outside of this module using it so define it as static,
   this will still be callable from the info dialogs as it
   will use the reference we set up.
 
 $Returns:		zero, always

 $Arguments:	win, parent window
                identifier, of the window
				reason, for the window
 
*******************************************************************************/

static tBookStatus bookDefaultCallBack( tBookMfwHnd win, UBYTE identifier, UBYTE reason )
{
	if ( win )
	    winShow( win );

	return 0;
}








/*******************************************************************************

 $Function:    	bookWindowData

 $Description:	

   This routine is designed to be invoked from within a menu
   handler or call back function and will return a pointer to
   the window data associated with the current MFW element.
 
 $Returns:		pointer to the data area

 $Arguments:	none.
 
*******************************************************************************/

void *bookWindowData( void )
{
	tBookMfwHnd WinPtr;

	if ( ( WinPtr = bookCurrentWindow() ) == NULL )
		return (void *) NULL;

	return (void *)((tBookMfwHdr *) WinPtr)->data;
}









/*******************************************************************************

 $Function:    	bookMenuSelectedNumber

 $Description:	

   This will return the currently selected entry in the phone
   book, when the routine is being called from a menu handler
 
 $Returns:		pointer to the selected number buffer

 $Arguments:	none.
 
*******************************************************************************/

char *bookMenuSelectedNumber( void )
{


	/* Have we got a valid window ?
	*/
	if ( ( (tBookMfwHnd) bookWindowData() ) == NULL )
		return NULL;

	return NULL;
}









/*******************************************************************************

 $Function:    	bookPhoneBookLoading

 $Description:	access routine for the local static menu area

    Utility functions to determine if the phone book is
    still in the loading state
  
 $Returns:		BOOK_FAILURE if loading, otherwise BOOK_SUCCESS

 $Arguments:	none.
 
*******************************************************************************/

tBookStatus bookPhoneBookLoading( void )
{
	int temp;
	char tempst[20];
	TRACE_FUNCTION("bookPhoneBookLoading");
	temp = phb_get_mode();
	sprintf(tempst,"phb_get_mode %d",temp);
	TRACE_EVENT(tempst);
	return ( (phb_get_mode() == PHB_LOADING)||(phb_get_mode() == MFW_PHB_FAIL) ) ? BOOK_FAILURE : BOOK_SUCCESS;
}









/*******************************************************************************

 $Function:    	bookInfoDialog

 $Description:
 
    Utility functions just to throw a string onto the screen
    primarily for debug purposes, the modal version will wait
    for a user input before removing the message, the non-modal
    will clear after three seconds

 $Returns:		One.

 $Arguments:	String1, String2, character strings to be displayed.
 
*******************************************************************************/

tBookStatus bookInfoDialog( char *String1, char *String2 )
{
	tBookMfwHnd Win = bookCurrentWindow();
	tBookDialogData Dialog;

	/* Initialise the dialog control block with default information
	*/
	dlg_initDisplayData_TextStr( &Dialog, TxtNull, TxtNull, String1, String2, COLOUR_STATUS);
	dlg_initDisplayData_events( &Dialog, (T_VOID_FUNC)bookDefaultCallBack, THREE_SECS, KEY_CLEAR);

	/* Show the dialog
	*/
	info_dialog( Win, &Dialog );

	return 1;
}









/*******************************************************************************

 $Function:    	bookNotImplemented

 $Description:	

    This will present an information dialog indicating the current
    feature is still under development. This routine will use the
    current MFW element to detect the handle of the parent window
    if the incoming window handle is NULL.
 
 $Returns:		One

 $Arguments:	Parent, window, can be NULL
 
*******************************************************************************/

tBookStatus bookNotImplemented( tBookMfwHnd Parent )
{
	tBookMfwHnd Win = ( Parent ) ? Parent : bookCurrentWindow();
	tBookDialogData Dialog;

	/* Initialise the dialog control block with default information
	*/
	dlg_initDisplayData_TextStr( &Dialog, TxtNull, TxtNull, MmiRsrcGetText( TxtNotImplemented ), NULL, COLOUR_STATUS);
	dlg_initDisplayData_events( &Dialog, (T_VOID_FUNC)bookDefaultCallBack, THREE_SECS, KEY_CLEAR);

	/* Show the dialog
	*/
	info_dialog( Win, &Dialog );

	return 1;

}

/* Menu handler to invoke the routine above
*/
tBookStatus bookMenuNotImplemented( tBookMfwMenu *Menu, tBookMfwMenuItem *Item )
{
    return bookNotImplemented( NULL );
}






/* SPR#1428 - SH - New Editor changes: No longer required */
#ifndef NEW_EDITOR

/*******************************************************************************

 $Function:    	set_edt_attr

 $Description:	
 
    Sets up the edit attribute structure with the provided parameters

 $Returns:		None

 $Arguments:	winPx, winPy, winSx, winSy, positioning information
                fgColor, colour for foreground
				font, mode, controls, pretty much what they say
				*text, initial text to be edited
				size, of the text (in bytes)
				attr, the block to be set up with the other parameters
 
 GW - Changed colour from 8 bits (foreground) to 2x32bits fgd and bgd
For the colour display we pass a 32 bit word that defines the size and position of the editor.
This can be either a specific size or a more generic ('middle of the screen'/full width of display)
*******************************************************************************/
void bookSetEditAttributes(
    int  zone_id,
	U32  colIndex,
	U8  font,  U8  mode,  U8  *controls,
	char *text,   U16 size,
	MfwEdtAttr*   attr)
{
	/* Populate the structure with the parameters
	*/
	calculateWindow(attr, zone_id );
	attr->edtCol = colIndex;
	attr->font = font;
	attr->mode = mode;
	attr->controls = controls;
	attr->text = text;
	attr->size = size;
}
#endif /* NEW_EDITOR */










/*******************************************************************************
                                                                              
                             Name Handling Functions
                                                                              
*******************************************************************************/

/*******************************************************************************

 $Function:    	bookGetCurrentStatus

 $Description:	Determines the current status of the phone book
 
 $Returns:		status byte

 $Arguments:	currentStatus, pointer to structure to be populated with
                status information
 
*******************************************************************************/

UBYTE bookGetCurrentStatus( T_MFW_PHB_STATUS *currentStatus )
{
	UBYTE temp;
	TRACE_FUNCTION( "bookGetCurrentStatus()" );


    temp = phb_get_status( currentStatus );
    {
		char temp[255];
		sprintf(temp,"book %d max_entries %d used_entries %d aval_entries %d",
				currentStatus->book,currentStatus->max_entries,
				currentStatus->used_entries,
				currentStatus->avail_entries);
		TRACE_EVENT(temp);
    }
    return temp;
}








/*******************************************************************************

 $Function:    	bookFindNames

 $Description:	Fine name in phone book
 
 $Returns:		status of search

 $Arguments:	number, number of entries to search, current,
                pointer to structure containing search context
 
*******************************************************************************/

UBYTE bookFindName( UBYTE number, tMmiPhbData *current )
{
    UBYTE   ret;
	int     index;   // , Count = 0;  // RAVI
	
    UBYTE   book_type, index_type;

    

//  May 24, 2004 REF: CRR MMI-SPR-15813 Ajith K P
//  Added these variables to Fix the Issue MMI-SPR-15813
//  nondigitindex => index after which '+' sign to be added
//  numlength = lenght of the number array.
//  nondigitFound = Checks whether nondigit is present in the number array or not.

   int nondigitindex = -1,j,numlength = 0, nondigitFound = -1; 

	
	TRACE_FUNCTION( "bookFindName()" );

    /* Firstly, check for an empty book
    */
	bookGetCurrentStatus( &current->status );
    if ( current->status.used_entries < 1 )
    {
        current->index = 0;
        return MFW_PHB_OK;
    }

	//x0066692 Date:23 April 2007 DR:OMAPS00121834
	// this condition ensures that stop index doesn't exceed max no. of entries can be added to a phonebook 
	number = current->status.used_entries - current->index +1;

	if ( number > MAX_SEARCH_NAME )
		number = MAX_SEARCH_NAME;  
	// Feb 20, 2006 REF: OMAPS00061948  x0039928 
	// Fix : If used entries are less than MAX_SARCH_NAME then no. of entries 
	// passed to the phb_read_entries is used entries.

    /* Okay, we have some entries to search, so initialise the current
       entry buffer to empty
    */
	memset( current->entry, 0, MAX_SEARCH_CALL_LIST * sizeof( T_MFW_PHB_ENTRY ) );

   
   
    current->list.entry         = current->entry;
    current->list.num_entries   = number;

    /* Sort out how we index the information
    */
    book_type  = current->status.book;
    index_type = MFW_PHB_INDEX;
    if ( ( book_type == bookActiveBook(WRITE) ) || ( book_type == PHB_SDN ) )
        index_type = MFW_PHB_ALPHA;

    /* Sort out the type of search we are performing
    */
	if ((book_type == PHB_LDN) || (book_type == PHB_LRN)|| (book_type == PHB_LMN))
        index_type = MFW_PHB_INDEX;
    else
    {
	    if ( current->KindOfSearch == SEARCH_BY_NAME )
    	    index_type = MFW_PHB_ALPHA;
	    else if ( current->KindOfSearch == SEARCH_BY_NUMBER )
    	    index_type = MFW_PHB_NUMBER;
	    else if ( current->KindOfSearch == SEARCH_BY_LOCATION )
    	    index_type = MFW_PHB_INDEX;
    }

   
    /* read the appropriate entries from the phone book
    */
	{
		char temp[255];
		sprintf(temp,"book %d index %d indextype %d number %d",
					book_type,current->index,index_type,number);
		TRACE_EVENT(temp);		
	}	
// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
//When book is ADN, we need to read only ADN entries. no need to display ADN and FDN together
 
	if (book_type == PHB_ADN)	/*Quick test */
		{
		TRACE_EVENT("book_type == PHB_ADN");
		ret = phb_read_entries( PHB_ADN, current->index, index_type, number, &current->list );
		}
	else
		{
		TRACE_EVENT("book_type == OTHER");
		ret = phb_read_entries( book_type, current->index, index_type, number, &current->list );
		}
	
    /* Populate the output structures
    */
    if (current->list.result == MFW_NO_ENTRY)
        current->index = 0;

	
#ifdef NO_ASCIIZ
	
#ifdef EASY_TEXT_ENABLED
for ( index = 0; index < number; index++ )
{/*MC , SPR 1242 merged in this section from b-sample build*/
	
        /* GSM character sets
        */
        UBYTE   l_name[MAX_ALPHA_LEN];/*MC SPR 1257, name strings should use MAX_ALPHA_LEN*/
	  char debug[22];	
	  int i;
        TRACE_EVENT( (char*)current->entry[index].name.data);
        TRACE_EVENT("Convert from SIM to UCS2");

    	TRACE_EVENT_P1("length of name string: %d", current->entry[index].name.len);
    	/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
		if (current->entry[index].name.data[0] == 0x80 )
		{	for (i=1; i<MAX_ALPHA_LEN; i+=1)
			{
				l_name[i+1] = current->entry[index].name.data[i];
			}
       	 	l_name[0] =	current->entry[index].name.data[0]; 
       		l_name[1] = MAX_ALPHA_LEN;
		
			memcpy( current->entry[index].name.data, (char*)l_name, MAX_ALPHA_LEN );
			/*MC, we'll just use the length provided by the MFW*/
			if (current->entry[index].name.len%2 == 1)/*if length of string odd*/
   		 		current->entry[index].name.len++;/* INCREASE LENGTH BY 1*/

			/*make sure all chars after length are 0*/
			for (i=current->entry[index].name.len; i<MAX_ALPHA_LEN; i++)
			{	current->entry[index].name.data[i] = 0; }
			
			for (i=0; i < MAX_ALPHA_LEN;i++)
			{
				if ( current->entry[index].name.data[i]== 0 && current->entry[index].name.data[i+1] != 0)
					debug[i] = '0';
				else
					debug[i] = (char)current->entry[index].name.data[i];
	        }
		
			
   		 TRACE_EVENT("Decoded entry");
    	TRACE_EVENT(debug);
			
		}

		else

		{	/*SPR2175, use new function to convert from GSM to ASCII*/
				{	ATB_convert_String((char*)current->entry[index].name.data, MFW_DCS_8bits, current->entry[index].name.len,
								(char*)l_name, MFW_ASCII, PHB_MAX_LEN, FALSE);
					current->entry[index].name.dcs = MFW_ASCII;
					memcpy( current->entry[index].name.data, (char*)l_name, MAX_ALPHA_LEN );
				}

			
		}
	}
			
#endif
/*MC end*/
#else
for ( index = 0; index < number; index++ )
	{	
			UBYTE   l_name[MAX_ALPHA_LEN];/*MC SPR 1257, name strings should use MAX_ALPHA_LEN*/
			TRACE_EVENT_P1("length of name string: %d", current->entry[index].name.len);
			/*SPR2175, use new function to convert from GSM to ASCII*/
			{	ATB_convert_String((char*)current->entry[index].name.data, MFW_DCS_8bits, current->entry[index].name.data.len,
								l_name, MFW_ASCII, PHB_MAX_LEN);
				current->entry[index].name.dcs = MFW_ASCII;
				memcpy( current->entry[index].name.data, (char*)l_name, MAX_ALPHA_LEN );
			}
	}
#endif

    /* Needed to make international calls from the phonebook
    */
	for ( index = 0; index < number; index++ )
	{		
		if ( current->entry[index].ton )
		{

//  May 24, 2004 REF: CRR MMI-SPR-15813 Ajith K P

//  Issue description: Followup from 15623, this is the BMI part of this Issue.
//	At last, in the record "SSC09" the number is displayed with the "+" 
//	character at the beginning of the number instead of "**21*+014543877989#"

//  Solution: Find the location of nondigit(e.g:*, #) in the number array 
//	and put '+' to next position if the index is less than numlength-8. This is done 
//	as there can be ss strings(like *11*20#) attached to the number in the end.
//	For eg: **61#+9844093515*11*20#
//  If the total length is less than 8 assign this index to nondigitindex.
//  This solution is based on special sequences we have encountered till now.
		
		// Finds the length of the number array.
		numlength = strlen((char*)current->entry[ index ].number);
		//checks whether the length is within the limit.
		if(numlength <=PHB_MAX_LEN)
//TISH, patch for OMAPS00144985, modified by Jinshu Wang, 2007-09-07
//start
if (current->entry[ index ].number[0]=='*' || current->entry[ index ].number[0]=='#')
{
			for( j=0; j< numlength ; j++)
 			{
				if( !isdigit((char*)current->entry[ index ].number[j]) )
                {
					// Found a nondigit can be */#
					nondigitFound = j;
					// Checking , whether the nondigit index is not at the end 
					// part of the number array.
					if(j < (numlength-8))
						{
							// Current index is less than numlength-8
						   	nondigitindex = j;
						}
					else
						{
							if(numlength <8) 
							{
								// If lenght of the number array is less than 8
								// assign the index to nondigitindex
								// This condition might occur when the user
								// presses invalid short sequence
								nondigitindex = j;
							}
						}
				}
			 }
}
//end			
			if((nondigitindex == -1) && (nondigitFound> -1) && (nondigitFound < 4))
			{
				// Nondigit is existing and it's position is less than 4 and not met 
				// any of the above conditions.
				nondigitindex = nondigitFound;
			}

			// Move the contents accordingly.
			memmove((char *) &(current->entry[ index ].number[ nondigitindex+2]),
			(char *) &(current->entry[ index ].number[nondigitindex+1]), numlength - nondigitindex+1);

			// Add the '+' sign after the appropriate nondigit character, if any.
			current->entry[index].number[nondigitindex+1] = '+';
		  }
		
 	   }
	//}
    
    return current->result = ret;
}








/*******************************************************************************

 $Function:    	bookEvent

 $Description:	Event handler for a phone book event
 
 $Returns:		MFW_EVENT_CONSUMED if recognised message, otherwise
                MFW_EVENT_PASSED

 $Arguments:	e, event, para, general parameter giving context of call
 
*******************************************************************************/

int bookEvent( MfwEvt e, void *para )
{
	T_MFW_PHB_STATUS *status;

    TRACE_FUNCTION( "bookEvent" );

    /* deal with the incoming event
    */
	switch( e )
    {
        case E_PHB_UPN_LIST :
        {
		/* x0039928 - Lint warnings fix
		upnList = (T_MFW_UPN_LIST *)para; */
            /* tbd */
        }
        break;

		case E_PHB_STATUS:
        {
			status = (T_MFW_PHB_STATUS *)para;
			if ( ( status->book == PHB_FDN ) || ( status->book == PHB_ADN ) )
			{
				if ( pinsSetIsFocussed() == FOCUSSED_PINS )
					backpinFDNactdeact();
			}
        }
        break;

		case E_PHB_ERROR:
        {
		//x0061560 Date May16,07 (sateeshg) OMAPS00133241 		
     			 T_MFW_HND    win = mfwParent( mfw_header() );
	
      			 TRACE_ERROR("E_PHB_ERROR store entry");
 		   	bookShowInformation(win, TxtError, NULL, NULL );
  		      }	
	
     
        break;
        
        /* SPR#1112 - SH - New events to indicate status of phonebook */

        case E_PHB_READY:
        {
// Aug 18, 2006   ER: OMAPS00089840  x0039928
// Fix : If the E_PHB_READY is for delete all then destroy the please wait window and display the string "Deleted"
		if( DeleteAll_flag == TRUE)
		{
     			 T_MFW_HND    win = mfwParent( mfw_header() );
			int txtid;
	
      			 TRACE_EVENT("E_PHB_READY Delete all");
			if (waitWindel)
  			{
				SEND_EVENT(waitWindel, DIALOG_DESTROY,0,0);
				waitWindel = NULL;
				txtid = TxtDeleted;
	 		   	bookShowInformation(win, txtid, NULL, NULL );
  			}
			DeleteAll_flag = FALSE;
			break;
		}
		//x0061560 Date May17,07 (sateeshg) OMAPS00133241
		if(addname_flag == TRUE)
              {
		            	  	
			T_MFW_HND    win = mfwParent( mfw_header() );
			int txtid;
	
      			 TRACE_EVENT("E_PHB_READY Saved");
			
				txtid = TxtSaved;
	 		   	bookShowInformation(win, txtid, NULL, NULL );
				addname_flag = FALSE;
				break;
			}
		/* Aug 09, 2007 DR:OMAPS00140447 x0056422(Srilekha) */		
		if(TRUE == changename_flag)
		{
				T_MFW_HND    win = mfwParent( mfw_header() );
				int txtid;
	
				TRACE_EVENT("Saved prompt for change entry");
      			TRACE_EVENT("E_PHB_READY Saved");
			
				txtid = TxtSaved;
	 		   	bookShowInformation(win, txtid, NULL, NULL );
				changename_flag = FALSE;
				break;
		}


            TRACE_EVENT("E_PHB_READY");
#ifdef INT_PHONEBOOK
			/* SPR#1112 - SH - If we're copying or moving a list,
			 * the current operation has been successful - keep copying. */	 
            if (ipbCopyIndex)
            {
            	TRACE_EVENT("Copying/moving in progress.");
            	bookCopyMultiple(ipbWin, ipbMove, 0);
            }
#endif
        }
        break;

        case E_PHB_BUSY:
        {
            TRACE_EVENT("E_PHB_BUSY");
        }
        break;
        
        /* end SPR#1112 */
        
        default:
        {
            /* indicate we haven't dealt with event
            */
			return MFW_EVENT_PASSED;
        }
       // break;   // RAVI
    }

    /* got to here, we have dealt with the event
    */
    return MFW_EVENT_CONSUMED;
}



// Aug 18, 2006   ER: OMAPS00089840  x0039928
// Fix : Added a new function to delete all the phone book entries.
/*******************************************************************************

 $Function:     bookDeleteBook

 $Description:  Delete the complete phone book entries

 $Returns:    None

 $Arguments:	win - Window Handler

*******************************************************************************/

void bookDeleteBook(T_MFW_HND win)
{
    T_MFW_WIN			*win_data   = ( (T_MFW_HDR *) win )->data;
    T_phbk				*Phbk		=	(T_phbk *) win_data->user;

    Phbk ->current.status.book = bookActiveBook(READ);
	//x0061560 Date JUNE 4,07 (sateeshg) OMAPS00131777 
	//Updating the current status of the phonebook
 bookGetCurrentStatus(&Phbk ->current.status);
if(Phbk ->current.status.used_entries !=0)//Apr 30,2007, OMAPS00129173, x0066814(Geetha)
{
    if (bookGetBookSelected() )
		bookShowInfoScreen( Phbk->win, TxtPleaseWait, NULL, NULL, NULL, 100);
    else
	DeleteAll_flag = TRUE;
	
    if (phb_delete_book(Phbk->current.status.book) == MFW_PHB_OK)
    {
        if (!bookGetBookSelected() )
    	 	waitWindel = bookShowInfoScreen( Phbk->win, TxtPleaseWait, NULL, NULL, NULL, FOREVER);
	else
	{
		MmiBookSetCurrentText(TxtDeleted);
		bookShowInformation( Phbk->win, MmiBookCurrentText(),NULL, NULL );
	}
    }
    else
    {
         DeleteAll_flag = FALSE;
	  MmiBookSetCurrentText(TxtFailed);
         bookShowInformation( Phbk->win, MmiBookCurrentText(),NULL, NULL );
    }
    Phbk ->current.status.used_entries=0;//Apr 30,2007, OMAPS00129173, x0066814(Geetha)
}
else//Apr 30,2007, OMAPS00129173, x0066814(Geetha)
{
bookShowInformation( idle_get_window(), TxtEmptyList, NULL, NULL );
    }

    return;
}


/*******************************************************************************

 $Function:     bookCopyPhbName

 $Description:  Copiers a phonebook name structure (for NO_ASCII set or cleared)

 $Returns:    None

 $Arguments:	destEntry - destination phonebook entry structure
 				srcEntry - source phonebook entry structure
				maxLen - max number oc chars to be copied 

*******************************************************************************/
//GW Created function to copy name (and length when required).
void bookCopyPhbName( T_MFW_PHB_ENTRY* destEntry, T_MFW_PHB_ENTRY* srcEntry, int maxLen )
{
	int len; //length of string to be copied
	if ((destEntry==NULL) || (srcEntry==NULL))
		return;
#ifdef NO_ASCIIZ
	//Ensure we do not copy more than the PHB array can hold.
	if ((maxLen <  PHB_MAX_LEN) && (maxLen >0))
		len = maxLen;
	else
		len = PHB_MAX_LEN;

#else
/*MC SPR1319, name string is only up to MAX_ALPHA_LEN in length*/
	//Ensure we do not copy more than the PHB array can hold.
	if ((maxLen < MAX_ALPHA_LEN) && (maxLen >0))
		len = maxLen;
	else
		len = MAX_ALPHA_LEN;
#endif
#ifdef NO_ASCIIZ	
	memset (destEntry->name.data, 0, PHB_MAX_LEN);
	memcpy( destEntry->name.data, srcEntry->name.data, len);
	if (srcEntry->name.len < len)
		destEntry->name.len = srcEntry->name.len;
	else
	{	//we have truncated the name - add termination (if possible)
		destEntry->name.len = len;
		if (len < PHB_MAX_LEN)
		{	// there is space in array for termination - add it.
			destEntry->name.data[len] = 0x00;
			
		}
	}
#else

	memcpy( destEntry->name, srcEntry->name, maxLen);
#endif		
}



/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

