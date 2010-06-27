/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   PhoneBook
 $File:       MmiBookServices.c
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

    This provides the implementation of service functions for use with the
  phone book module

********************************************************************************
 $History: MmiBookServices.c

       Sep 20, 2006 REF: OMAPS00145786  x0080701 (Bharat)  
	Bug:PhoneBook: Wrong confirmation message in Phonebook storage when copying/moving a contact 
	                         after adding one or making a change
 	Fix: ACI shall not send rAT_OK for entry addition / entry change operations in Internal PHB
 	      Hence, the "addname_flag" and "changename_flag" are immdtly reset incase of Internal PHB
 	
    June 22, 2006    REF: DRT OMAPS00081761  x0012849 : Jagannatha
    Description:Handset crash when adding new entry with more then 20 digits to phonebook SIM card 
    Solution: Number editor will be restricted to 20 characters when EXT1 is not supported.

       June 14, 2006 REF: OMAPS00079308  x0039928  
	Bug:Validation and Optimization of Memory consumption
 	Fix: Memory optimization is done in bookFindNameInSpecificPhonebook to save around 450 bytes of memory.

    May 29, 2006 DR: OMAPS00079431 - xreddymn
    Description: Name field is not limited to 14 bytes in case of SIM phonebook.
    Solution: In bookInputStartNameEditor, limit the number of characters after
    checking if SIM or ME phonebook is being used.

	Jan 22, 2006 REF: OMAPS00061930  x0039928  
	Bug:PhoneBook -Change entry and view entry through Change option-failed
 	Fix: Phonebook is populated with phone numbers after some time and not immediately while saving the changed entries.

     	June 07,  2005   REF: CRR MMI-FIX-31545 x0021334
    	Issue description: SIM: wrong behavior when FDN is not allocated in SST
   	Solution: Before displaying the FDN options, a check is made if FDN is allocated. Only if 
                  	FDN is allocated, the options are displayed.

  	Sep 29, 2004        REF: CRR 25041  xvilliva  
	Bug:		add new entry in the FDN , phone gets rebooted.
	Fix:		The bookMenuStart() is called with the window data->phbk->win, 
			similar to the function bookFDNMenu() to avoid crash. The menu 
			window thus created is stored, which will be deleted while 
			phonebook menu is destroyed. 

	Jul 29, 2004        REF: CRR 20888  Deepa M.D
	Bug:Message received not showing with name, name stored in phonebook
	Fix:Memset the structure(p_pszNumberText.data) to 0 and then memcpy.

 	Jul 28, 2004       	REF: CRR:20887 xrashmic - SASKEN
	Description: Current state of FDN not highlighted
	Fix: Instead of showing the submenu directly we call this function 
	where we set the menu and also higlight the previously selected menu item

  	June 25, 2004        REF: CRR 21547  xvilliva  
	Bug:	Taking blank entries in phonebook if select book is either SIM or Phone.
	Fix:	 We check for presence of either phone number or name before add/modify.

 	Jun 23, 2004        	REF: CRR MMI-SPR-21636  Rashmi C N(Sasken) 
	Description:		The confirmation for deleting entry from address book was a timed confirmation
  					screen.
  	Solution:			Now the time is set to 'FOREVER' from 'TEN_SECS'

	Issue Number : SPR#12722 on 29/03/04 by Deepa M.D 


  25/10/00      Original Condat(UK) BMI version.

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
#include "mfw_cnvt.h"	//GW Added for 'mfw_SIM2GsmStr'

#include "dspl.h"

#include "ksd.h"
#include "psa.h"
#include "psa_sim.h" 	// June 07,  2005	REF: CRR MMI-FIX-31545 x0021334

#include "MmiMain.h"
#include "MmiBookController.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"



#include "MmiMenu.h"
#include "MmiCall.h"
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
#include "MmiBookController.h"
#include "MmiDialogs.h"
#include "mmiColours.h"
#include "MmiBookMenuWindow.h"

/* SPR#1428 - SH - New Editor: Include needed for MmiBookInputWindow functions, see below*/
#ifdef NEW_EDITOR
#ifdef EASY_TEXT_ENABLED
#include "MmiChineseInput.h"
#endif
#endif
#include "psa_sim.h"  // June 22, 2006 REF: DRT OMAPS00081761  x0012849

#define TRACE_PB_DEBUG 

extern int menuFDN;
extern T_call call_data;

/*******************************************************************************

                                Private data elements

*******************************************************************************/

static MfwHnd phb;
/* SPR#1112 - SH - Internal phonebook flags */
#ifdef INT_PHONEBOOK
BOOL ipbBookSelected = FALSE;
BOOL ipbBookAvailable = FALSE;
#endif

  // June 14, 2006 REF: OMAPS00079308  x0039928  
  // Fix: All optimized code is put under flag FF_MMI_PB_OPTIM
#ifdef FF_MMI_PB_OPTIM
# define PB_MAX_FIND_ENTRIES   1
#else
# define PB_MAX_FIND_ENTRIES   4
#endif

// Jul 28, 2004       	REF: CRR:20887 xrashmic - SASKEN

#define FIRSTMENUITEM 0
#define SECONDMENUITEM 1
#define OLD_NUM_LENGTH 21    // June 22, 2006 REF: DRT OMAPS00081761  x0012849

/*******************************************************************************

                                Private methods

*******************************************************************************/

/* Sep 20, 2006 REF: OMAPS00145786  x0080701 (Bharat) -- Start */

extern BOOL addname_flag;	
		
extern BOOL changename_flag;

static void mmi_int_phb_store_change_completion();

/* Sep 20, 2006 REF: OMAPS00145786  x0080701 (Bharat) -- End */


/*******************************************************************************

 $Function:     bookDeleteEntryCB

 $Description:  Delete the current entry from the phone book.
                This routine is invoked as a callback from the confirmation
                dialog and will perform the delete if the OK key has been
                selected.

 $Returns:    None

 $Arguments:  win, handle of parent window,
                id, of invocation,
                reason, for invocation

*******************************************************************************/

static void bookDeleteEntryCB( T_MFW_HND win, USHORT id, UBYTE reason )
{
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

	TRACE_EVENT_P1("bookDeleteEntryCB index %d", Phbk->current.entry[Phbk->current.selectedName].index);
	
    /* This call back is invoked from the delete entry dialog confirmation
       screen, and will only respond to the left key (OK) being selected.
    */
    switch ( reason )
    {
    case INFO_KCD_LEFT:
        {
      if  (( Phbk->current.status.book == PHB_UPN) ||
      			(Phbk->current.status.book == PHB_FDN ))
            deleteFromFile( Phbk, Phbk->current.status.book );
      else
        deleteFromFile( Phbk, bookActiveBook(WRITE) );

            /* retrieve he status of the book, and create a string for output
               based on the status
            */
          bookShowInformation( Phbk->win, MmiBookCurrentText(), NULL,NULL );
          SEND_EVENT(win, E_MNU_ESCAPE, NULL, NULL);
        }
    break;

        default:
        {
            /* No action required
            */
        }
        break;
    }
}








/*******************************************************************************

                                Public Methods

*******************************************************************************/








/*******************************************************************************

 $Function:     bookInitialise

 $Description:  Initialise the phone book application

 $Returns:    none, side effect is to set up the static phone
                book element

 $Arguments:  none

*******************************************************************************/

void bookInitialise( MfwHnd parent )
{
  TRACE_FUNCTION( "bookInitialise()" );
  
  /* SPR#1112 - SH - Added E_PHB_READY and E_PHB_BUSY */
  phb  = phb_create( 0, (MfwEvt) (E_PHB_UPN_LIST | E_PHB_ERROR | E_PHB_STATUS
  	| E_PHB_READY | E_PHB_BUSY), (MfwCb) bookEvent );

}

/* SPR#1112 - Initialise internal phonebook */
#ifdef INT_PHONEBOOK

/*******************************************************************************

 $Function:     bookInitInternal

 $Description:  Initialise the internal phone book application

 $Returns:    none, side effect is to set up the static phone
                book element

 $Arguments:  none

*******************************************************************************/

void bookInitInternal( void )
{
	UBYTE intphonebook;
	T_MFW ret;
	
	ret = GI_pb_GetPhonebook(&intphonebook);

	if (ret==MFW_PHB_OK)
	{
		bookSetBookAvailable(TRUE);
		GI_pb_Initialise();
		if (intphonebook)
		{
			bookSetBookSelected(TRUE);
		}
		else
			bookSetBookSelected(FALSE);
	}
	else
		bookSetBookAvailable(FALSE);
	
	return;
}


/*******************************************************************************

 $Function:		bookGetBookSelected

 $Description:	Is the internal phonebook selected?

 $Returns:		TRUE if internal phonebook is selected, FALSE otherwise

 $Arguments:	none

*******************************************************************************/

BOOL bookGetBookSelected()
{
	return ipbBookSelected;
}


/*******************************************************************************

 $Function:		bookSetBookSelected

 $Description:	Select or deselect the internal phonebook

 $Returns:		None.

 $Arguments:	TRUE to select internal phonebook, FALSE to select SIM phonebook

*******************************************************************************/

void bookSetBookSelected(UBYTE bookSelected)
{
	ipbBookSelected = bookSelected;

	return;
}


/*******************************************************************************

 $Function:		bookGetBookAvailable

 $Description:	Is the internal phonebook available for use?

 $Returns:		TRUE if internal phonebook is available, FALSE otherwise

 $Arguments:	none

*******************************************************************************/

BOOL bookGetBookAvailable()
{
	return ipbBookAvailable;
}


/*******************************************************************************

 $Function:		bookSetBookAvailable

 $Description:	Indicate whether or not the internal phonebook is available for use

 $Returns:		None.

 $Arguments:	TRUE to if internal phonebook is available, FALSE otherwise

*******************************************************************************/

void bookSetBookAvailable(UBYTE bookAvailable)
{
	ipbBookAvailable = bookAvailable;

	return;
}

#endif




/*******************************************************************************

 $Function:     bookExit

 $Description:  Exit the phone book application, this will delete the static
                phone book element

 $Returns:    none

 $Arguments:  none

*******************************************************************************/

void bookExit( void )
{
    TRACE_FUNCTION( "bookExit()" );
	phb_delete( phb );
	/* SPR#1112 - SH - Now redundant */
   // bookDeletePhoneBook();

/* SPR#1112 - SH - Exit internal phonebook if activated */
#ifdef INT_PHONEBOOK
	if (bookGetBookAvailable())
		GI_pb_Exit();
#endif

	return;
}








/*******************************************************************************

 $Function:     bookAddName

 $Description:  Adds a new number to the phone book

 $Returns:    tbd

 $Arguments:  tbd

*******************************************************************************/

void bookAddName( T_MFW_HND win, char *number )
{
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = (T_phbk          *)data->phbk;

    /* Populate the name and number records with the provided number
    */


/*MC SPR 1257, name strings should use MAX_ALPHA_LEN*/
  memset( Phbk->edt_buf_name,   0,      MAX_ALPHA_LEN );
    memcpy( Phbk->edt_buf_number, (char*)number,    PHB_MAX_LEN );

    /* Populate the input specifier structure and use it to
       perform the next action, note when we return we start
       adding a new name.
    */
    TRACE_EVENT ("bookAddName");
  data->phbk->input_name_win = bookInputStartNameEditor(
        data->win, (char*)Phbk->edt_buf_name );
}








/*******************************************************************************

 $Function:     bookShowInformation

 $Description:  general information dialog routine

 $Returns:    none

 $Arguments:  parent, window,
                text, to be displayed,
                cb_function, call back on completion of window

*******************************************************************************/

void bookShowInformation( MfwHnd parent, int txtId, char *text, T_VOID_FUNC cb_function )
{
    T_DISPLAY_DATA display_info;

    TRACE_FUNCTION( "bookShowInformation()" );

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, txtId, TxtNull, COLOUR_BOOK_POPUP);
    dlg_initDisplayData_events( &display_info, (T_VOID_FUNC) cb_function, THREE_SECS, KEY_CLEAR );
    display_info.TextString   = text;

    /* Call Info Screen
    */
    info_dialog( parent, &display_info );
}


/*******************************************************************************

 $Function:     bookShowInfoScreen

 $Description:  SPR1112 - SH - Show an info screen
 
 $Returns:    none

 $Arguments: parent, window,
             txtId1 - first line ID
             txtId2 - second line ID
             time - timeout for dialog

*******************************************************************************/

T_MFW_HND bookShowInfoScreen( MfwHnd parent, int txtId1, int txtId2, T_VOID_FUNC cb_function,
	USHORT Identifier, ULONG Time)
{
    T_DISPLAY_DATA display_info;

    TRACE_FUNCTION( "bookShowInformation()" );

    dlg_initDisplayData_TextId(&display_info, TxtNull, TxtNull, txtId1, txtId2, COLOUR_BOOK_POPUP);
    dlg_initDisplayData_events(&display_info, (T_VOID_FUNC)cb_function, Time, KEY_CLEAR);
    
    display_info.Identifier	= Identifier;

    /* Call Info Screen
    */
    return info_dialog( parent, &display_info );
}


/*******************************************************************************

 $Function:     bookFindIndexMatchPB

 $Description:  Locates the best match from a supplied list of phonebook entries

 $Returns:      0 if error, 1 if successful

 $Arguments:  listPB, pointer to phonebook entries
                    numEntries, number of entries in listPB
                    srcNumberr, pointer to pattern to match

 $History:
 SPR#1727 - DS - Merged from 1.6 branch.

*******************************************************************************/
int bookFindIndexMatchPB(T_MFW_PHB_LIST* listPB, UBYTE numEntries, const char* srcNumber)
{
    int indexMatch = -1;
    int i = 0;
    int j = 0;
    int maxMatchChars = 0;
    int count; /* Number of matching characters */
    int lenNum;
    int lenNumPB;
    int firstNotMatch=0;
	

 

    TRACE_FUNCTION("bookFindIndexMatchPB()");

#ifdef TRACE_PB_DEBUG
    TRACE_EVENT_P1("numEntries to examine: %d", numEntries);
#endif /* TRACE_PB_DEBUG */

    if ( (listPB == NULL) || (srcNumber == NULL) ) /* Invalid data */
    {
        TRACE_EVENT("Invalid data passed to findIndexMatchPB");
        return (-1);
    }

    if (numEntries <= 0) /* Invalid number of entries */
    {
        TRACE_EVENT_P1("Invalid number of entries passed into findIndexMatchPB: %d", numEntries);
        return (-1);
    }

    /* Calculate length of number */
    lenNum = strlen(srcNumber);

#ifdef TRACE_PB_DEBUG
    TRACE_EVENT_P1("Len of number: %d", lenNum);
#endif /* TRACE_PB_DEBUG */
    
    /* Loop round number of entries */
    for (i=0; i<numEntries; i++)
    {
        char* entryPB = (char*)listPB->entry[i].number;
        
        /* Calculate length of number in PB entry */
        lenNumPB = strlen(entryPB);

#ifdef TRACE_PB_DEBUG
        TRACE_EVENT_P2("Len of PB number for entry %d: %d", i, lenNumPB);
#endif /* TRACE_PB_DEBUG */

        /* Set count and loop-counter to 0 before comparing numbers */
        count = 0;
        j = 0;

        /* Determine number of matching digits
          * Exit loop if digits don't match or if reached end of either number buffer
          */
        while ( (!firstNotMatch) &&  !(j > lenNumPB) && !(j > lenNum) )
        {
            /* Compare digits */
            if (srcNumber[lenNum-j] == entryPB[lenNumPB-j]) /* Matching character */
            {
#ifdef TRACE_PB_DEBUG
             TRACE_EVENT_P1("Found match at pos %d", j);
#endif /* TRACE_PB_DEBUG */
                count = count + 1;
            }
            else
            {
                /* Digits don't match */
                firstNotMatch=1;
            }
            
            /* Increment index to point at next set of digits */
            j = j + 1;
        }

        /* If better match store count and index */
        /* SPR#1727 - DS - Added numbersMatch() check */
        if ( (count > maxMatchChars) && numbersMatch((char*)srcNumber, entryPB) )
        {
#ifdef TRACE_PB_DEBUG
            TRACE_EVENT_P2("Entry %d is better match. Matches %d chars", i, count);
#endif /* TRACE_PB_DEBUG */
            maxMatchChars = count;
            indexMatch = i;
        }
    }

    /* Return index of best match */
    return indexMatch;
}





/*******************************************************************************

 $Function:     bookFindNameInPhonebook

 $Description:  SPR#1112 - Modified function to search in alternate phonebook.
 			  Locates the name in the current phonebook.  If the current phonebook is
 			  the internal phonebook and no result is found, the SIM phonebook is also
 			  searched.

 $Returns:      0 if error, 1 if successful

 $Arguments:  p_pszNumber, pointer to pattern to match, p_pEntry, pointer
                to buffer into which to store result.

*******************************************************************************/
//GW-SPR#810-Simplified phonebook entry search based on 1.3.3 code.

int bookFindNameInPhonebook( const char *p_pszNumber, T_MFW_PHB_ENTRY *p_pEntry )
{
  int result;
  
  TRACE_FUNCTION( "bookFindNameInPhonebook()" );

  if(p_pszNumber == 0 || p_pEntry == 0)
    return (0);

  if (strlen(p_pszNumber) < 1)
    return (0);
 
  result = bookFindNameInSpecificPhonebook(p_pszNumber, p_pEntry);
  
  #ifdef INT_PHONEBOOK
  /* If not found in internal phonebook, try SIM */

  if (!result && bookGetBookAvailable() && bookGetBookSelected())
  {
  	bookSetBookSelected(FALSE);
  	result = bookFindNameInSpecificPhonebook(p_pszNumber, p_pEntry);
  	bookSetBookSelected(TRUE);
  }
  #endif
  
  return result;
}


/*******************************************************************************

 $Function:     bookFindNameInSpecificPhonebook

 $Description:  Locates the name in a specified phone book
			  SPR#1112 - SH - Separate this part of function, so we can call it twice
 $Returns:      0 if error, 1 if successful

 $Arguments:  p_pszNumber, pointer to pattern to match, p_pEntry, pointer
                to buffer into which to store result.

 SPR#1727 - Modified to use bookFindIndexMatchPB.

*******************************************************************************/

int bookFindNameInSpecificPhonebook( const char *p_pszNumber, T_MFW_PHB_ENTRY *p_pEntry )
{
#ifdef NO_ASCIIZ
	T_MFW_PHB_TEXT p_pszNumberText;
#endif
  
  T_MFW_PHB_LIST  phb_list;    //, new_list;  // RAVI

  SHORT     phb_index = 0;
  UBYTE     l_name[MAX_ALPHA_LEN];
  // June 14, 2006 REF: OMAPS00079308  x0039928  
  // Fix: All optimized code is put under flag FF_MMI_PB_OPTIM
  #ifndef FF_MMI_PB_OPTIM
  T_MFW_PHB_ENTRY temp_pEntry[PB_MAX_FIND_ENTRIES];
  #endif
 
/*MC SPR 1319*/
#ifdef EASY_TEXT_ENABLED
#ifndef FF_MMI_PB_OPTIM
 char debug[MAX_ALPHA_LEN];
 int k;
#endif
#endif
  int indexMatch;
  int ret;

  
  if(p_pszNumber == 0 || p_pEntry == 0)
    return (0);
  
  if (strlen(p_pszNumber) < 1)
	return (0);

  memset( p_pEntry,  0, sizeof(T_MFW_PHB_ENTRY) );
  memset( &phb_list, 0, sizeof(phb_list) );
  #ifdef FF_MMI_PB_OPTIM
  phb_list.entry = p_pEntry;
  #else
  phb_list.entry = temp_pEntry;
  #endif
  phb_list.num_entries = PB_MAX_FIND_ENTRIES;
    
  /* Search phonebook for up to 4 entries with the last 6 digits matching number */

#ifdef NO_ASCIIZ
  /* GW Set up data structure for NO_ASCIIZ */
  
  p_pszNumberText.dcs = MFW_DCS_8bits;
  p_pszNumberText.len = strlen(p_pszNumber);
  
  // strcpy((char*)p_pszNumberText.data, p_pszNumber);
  /* cq15688  copy number into the  text info structure using memcpy() instead of strcpy()3/03/04 MZ */
  //Jul 29, 2004    REF: CRR 20888  Deepa M.D
  //First memset the structure and then memcpy.
  memset(p_pszNumberText.data, 0, sizeof(p_pszNumberText.data));
  memcpy( (char*)p_pszNumberText.data, p_pszNumber , p_pszNumberText.len );
  
  ret = phb_find_entries(
	  bookActiveBook(READ),   /* phonebook                */
	  &phb_index,				/* returns index in phb     */
	  MFW_PHB_NUMBER,			/* searching for number     */
	  PB_MAX_FIND_ENTRIES,						/* return max. four entry    */
	  &p_pszNumberText,  /* search this pattern      */
	  &phb_list				/* return structure         */
	  );
  
#else
  ret = phb_find_entries(
	  bookActiveBook(READ),   /* phonebook                */
	  &phb_index,				/* returns index in phb     */
	  MFW_PHB_NUMBER,			/* searching for number     */
	  PB_MAX_FIND_ENTRIES,						/* return max. four entry    */
	  (char *)p_pszNumber,  /* search this pattern      */
	  &phb_list       /* return structure         */
	  );
#endif //NO_ASCIIZ

    //Check return from phb_find_entries
    if (ret == MFW_PHB_FAIL)
    {
		return (0);
    }
	

    /* SPR#1727 - DS - Examine returned PB entries and find best match */
    indexMatch = bookFindIndexMatchPB(&phb_list, phb_list.num_entries, p_pszNumber);

    if (indexMatch == -1) /* No match found */
    {
        TRACE_EVENT("No match found in findIndexMatchPB !");
        return (0);
    }

#ifdef TRACE_PB_DEBUG
    TRACE_EVENT_P1("Match found in findIndexMatchPB. Pb entry %d", indexMatch);
#endif /* TRACE_PB_DEBUG */


    /* Copy matched phonebook entry details */
#ifndef FF_MMI_PB_OPTIM
    memcpy( p_pEntry, &temp_pEntry[indexMatch] , sizeof(T_MFW_PHB_ENTRY) );
#endif

    if (p_pEntry == NULL)
    {
        return (0);
    }

    /* Convert name into appropriate format */
#ifdef NO_ASCIIZ
	/*MC SPR 1319*/
#ifdef EASY_TEXT_ENABLED
#ifndef FF_MMI_PB_OPTIM
	for (k=0;k<p_pEntry->name.len; k++)
	{	
	    if (p_pEntry->name.data[k]==0 )
	    {
		  debug[k] = '0';
	    }
	    else
	    {
	        debug[k] = p_pEntry->name.data[k];
	    }
	}
	TRACE_EVENT_P1("BFNIP: %s ", debug);	
#endif

	/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/	
	if (p_pEntry->name.data[0] == 0x80) /* Unicode 0x80 style */
	{
	       /*MC, removed var "actual_length*/
		int i;
             for (i=1; i<MAX_ALPHA_LEN; i+=1)
        	{
        		l_name[i+1] = p_pEntry->name.data[i];
        		
        	}
	       l_name[0] =	p_pEntry->name.data[0]; 
	       l_name[1] = MAX_ALPHA_LEN;
			
		memcpy( p_pEntry->name.data, (char*)l_name, MAX_ALPHA_LEN );
		
	      if (p_pEntry->name.len%2 == 1)/*if length of string odd*/
	      {
   		    p_pEntry->name.len++; /*MC we have to increase the length by one as we've shifted the string up*/
	      }

   	      /*MC, ensure all the chars in the string after string length are 0*/
  	     for (i=p_pEntry->name.len; i<MAX_ALPHA_LEN; i++)	 		
  	     {
   	          p_pEntry->name.data[i]= 0;
  	     }

#ifndef FF_MMI_PB_OPTIM 	     
	     for (k=0;k<p_pEntry->name.len; k++)
	     {
                if (p_pEntry->name.data[k]==0 )
                {
                    debug[k] = '0';
                }
                else
                {
                    debug[k] = p_pEntry->name.data[k];
                }
	     }

            TRACE_EVENT_P1("BNFIP conv: %s", debug);

	      sprintf(debug, "Length of phonebook entry:%d", p_pEntry->name.len);
#endif
	
	}
       else
#endif /* EASY_TEXT_ENABLED */
	/* Marcus: Issue 963: 11/09/2002: Start */
	{
	    /*
	     * bookGsm2Alpha requires ASCIIZ string - make sure it is.
	     * This also will help calling functions that may assume ASCIIZ.
	     */
	    if (sizeof(p_pEntry->name.data) > p_pEntry->name.len)
	    {
	        p_pEntry->name.data[p_pEntry->name.len] = '\0';
	    }
/*SPR2175, use new function to convert from GSM to ASCII*/
		{

			TRACE_EVENT_P1("Converting name from 8-bit to ASCII, dcs: %d", p_pEntry->name.dcs);
			ATB_convert_String((char*)p_pEntry->name.data, MFW_DCS_8bits, p_pEntry->name.len, (char*)l_name, MFW_ASCII, MAX_ALPHA_LEN, FALSE);
	    	p_pEntry->name.dcs = MFW_ASCII;
	    	memcpy(p_pEntry->name.data, l_name, MAX_ALPHA_LEN);
	    }
	    
	}
	/* Marcus: Issue 963: 11/09/2002: End */
/*MC end*/
#else
  	bookGsm2Alpha( (UBYTE *) p_pEntry->name );
#endif /* NO_ASCIIZ */
  
  return (1);
}




/*******************************************************************************

 $Function:    	bookFindNumberByPosition

 $Description:	locate a number given the index
 
 $Returns:		0 if failure, 1 otherwise

 $Arguments:	index of the number to find, p_pentry, return structure
 
*******************************************************************************/

UBYTE bookFindNumberByPosition (UBYTE index,T_MFW_PHB_ENTRY* p_pEntry)
{
  T_MFW_PHB_LIST  phb_list;
  /*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
  UBYTE l_name[MAX_ALPHA_LEN];

  TRACE_FUNCTION( "bookFindNumberByPosition()" );

    /* only search if we have valid input information
    */
  if( index == 0 || p_pEntry == 0 )
    return 0;

    /* Clear the output buffers
    */
  memset( p_pEntry,  0, sizeof( T_MFW_PHB_ENTRY ) );
  memset( &phb_list, 0, sizeof( phb_list ) );

    /* searc for the selected entry
    */
  phb_list.entry          = p_pEntry;
  phb_list.num_entries    = 1;
  phb_read_entries( bookActiveBook(READ), index, MFW_PHB_INDEX, 1, &phb_list );

    /* if we haven't found the entry return 0
    */
    if ( phb_list.result == MFW_ENTRY_EXIST )
        return 0;

    /* otherwise copy the located information to the output structure
    */
#ifdef NO_ASCIIZ

	{
	#ifdef EASY_TEXT_ENABLED
		/*MC SPR 1257, name strings should use MAX_ALPHA_LEN*/
		int i;
		if (p_pEntry->name.data[0] == 0x80 ||p_pEntry->name.data[0]== 0x00)
			{	for (i=1; i<MAX_ALPHA_LEN; i+=1)
				{
					l_name[i+1] = p_pEntry->name.data[i];
					
				}
	       	 	l_name[0] =	p_pEntry->name.data[0]; 
	       		l_name[1] = MAX_ALPHA_LEN;
			
				memcpy( p_pEntry->name.data, (char*)l_name, MAX_ALPHA_LEN );

   		 		p_pEntry->name.len++;/* = actual_length;*/
				
			}
		else
	#endif

	/*SPR2175, use new function to convert from GSM to ASCII*/
	if (p_pEntry->name.dcs == MFW_DCS_8bits && p_pEntry->name.data[0] != 0x80)
	{
		ATB_convert_String((char*)p_pEntry->name.data, MFW_DCS_8bits, p_pEntry->name.len, (char*)l_name, MFW_ASCII, MAX_ALPHA_LEN, FALSE);
		p_pEntry->name.dcs = MFW_ASCII;
		memcpy(p_pEntry->name.data, l_name, MAX_ALPHA_LEN);
	}
}
#else

  bookGsm2Alpha( (UBYTE *) p_pEntry->name );

#endif

    /* successful return status
    */
  return 1;

}








/*******************************************************************************

 $Function:     bookCallIndex

 $Description:  Calls the number in the physical index

 $Returns:    status from the phb_read_entries routine

 $Arguments:  index, of the number to call

*******************************************************************************/

int bookCallIndex( UBYTE index )
{
  T_MFW_PHB_LIST  phb_list;
  T_MFW_PHB_ENTRY entry;
  UBYTE           status;

  TRACE_FUNCTION( "bookCallIndex()" );

  memset( &entry,    0, sizeof( T_MFW_PHB_ENTRY ) );
  memset( &phb_list, 0, sizeof( phb_list ) );

  phb_list.entry          = &entry;
  phb_list.num_entries    = 1;
  if ( ( status = phb_read_entries( bookActiveBook(READ), index, MFW_PHB_PHYSICAL, 1, &phb_list ) ) == MFW_PHB_OK )
    callNumber( entry.number );
    return status;
}











/*******************************************************************************

 $Function:     bookGsm2Alpha

 $Description:  Convert a string from it's GSM to alpha characters

 $Returns:    none

 $Arguments:  alpha, pointer to string to be converted (Must be null
                terminated string)

*******************************************************************************/


void bookGsm2Alpha( UBYTE *alpha )
{
  int index;
  int length=0;

  if (alpha != NULL)
  {
    length = strlen( (char *) alpha );
  }
  else
  {
  return;
  }
  
  for ( index = 0; index < length; index++ )
    alpha[index] = alpha[index] & 0x7F;
}




/*******************************************************************************

 $Function:     bookActiveBook

 $Description:  Determine if the current active book is restricted or not

 $Returns:    PHB_FDN if restricted, PHB_ADN otherwise

 $Arguments:  None

*******************************************************************************/

UBYTE bookActiveBook( int process )
{
  TRACE_FUNCTION( "bookActiveBook()" );

/* SPR#1112 - SH - Return internal phonebook ID when appropriate */
// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
//	To get FDN book, when FDN is disabled, but only when accessing from FDM menu and not while we are in active call

	if(menuFDN && (call_data.calls.numCalls==0))
	{		
		return  PHB_FDN;		
	}

#ifdef INT_PHONEBOOK
  if (bookGetBookAvailable() && bookGetBookSelected())
  	return PHB_IPB;
#endif
// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
//	When FDN is disabled we need to display only ADN entries in the phonebook menu.
//	We now return PHB_ADN instead of PHB_ADN_FDN.

  if(process == READ)
    return ( phb_get_mode() == PHB_RESTRICTED ) ? PHB_FDN : PHB_ADN;
  else if(process == WRITE)
  	return ( phb_get_mode() == PHB_RESTRICTED ) ? PHB_FDN : PHB_ADN;
  return PHB_NONE; /*a0393213 warnings removal-added return statement*/
}



/*******************************************************************************

 $Function:     bookMemoryView

 $Description:  Display the memory status

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

void bookMemoryView( void )
{
    T_MFW_HND       win         = mfwParent( mfw_header() );
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
  char            Buffer[16];
  int TxtToShow;

	
    /* Determine which book we are currently using
    */
    data->phbk->current.status.book = bookActiveBook(READ);

    /* retrieve he status of the book, and create a string for output
       based on the status
    */
   #if(BOARD!=61) 
      Buffer[0] = '\0';   // OMAPS00042166 - RAVI /*a0393213 warnings removal-== changed to =*/
    #endif  
    if ( bookGetCurrentStatus( &data->phbk->current.status ) == MFW_PHB_OK )
  {
  /* SPR#1112 - Internal phonebook - show the current selected phonebook*/
#ifdef INT_PHONEBOOK
	if (bookGetBookAvailable() && bookGetBookSelected())
		TxtToShow = TxtPhone;
	else
		TxtToShow = TxtSIM;
#else
    TxtToShow = TxtUsed;
#endif
        sprintf( Buffer, "%d/%d",
                data->phbk->current.status.used_entries,
                data->phbk->current.status.max_entries );
  }
    else
    TxtToShow = TxtReadError;


	{
	    T_DISPLAY_DATA display_info;

		dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtToShow, TxtNull , COLOUR_STATUS);
		dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_CLEAR );
	    display_info.TextString2  = Buffer;

	    /* Call Info Screen
	    */
	    info_dialog( win, &display_info );
	}

}








/*******************************************************************************

 $Function:     bookDeleteEntry

 $Description:  Delete the current entry from the phone book.
                This routine doesn't actually perform the deletion, it
                sets up a confirmation dialog, where the call back function
                is used to perform the deletion (Refer above)

 $Returns:    None

 $Arguments:  None

*******************************************************************************/

void bookDeleteEntry( T_MFW_HND Window)
{
  tBookDialogData Dialog;

    /* Initialise the dialog control block with default information
  */
	dlg_initDisplayData_TextId( &Dialog, TxtSoftOK, TxtCancel, TxtPressOk, TxtDelete, COLOUR_STATUS);

//  	Jun 23, 2004        	REF: CRR MMI-SPR-21636  Rashmi C N(Sasken) 
//	Description:		The confirmation for deleting entry from address book was a timed confirmation
//  					screen.
// 	Solution:			Now the time is set to 'FOREVER' from 'TEN_SECS'
	dlg_initDisplayData_events( &Dialog, (T_VOID_FUNC)bookDeleteEntryCB, FOREVER, KEY_CLEAR | KEY_LEFT | KEY_RIGHT );

  /* Show the dialog
  */
  	info_dialog( Window, &Dialog );
}

/*******************************************************************************

 $Function:    	bookActivateFDN

 $Description:	information screen to tell user to activate FDN
 
 $Returns:		None

 $Arguments:	None
 
*******************************************************************************/

void bookActivateFDN( T_MFW_HND window )
{
	tBookDialogData Dialog;

    /* Initialise the dialog control block with default information
	*/
	dlg_initDisplayData_TextId( &Dialog, TxtNull, TxtNull, TxtPlease, TxtActivateFDN, COLOUR_STATUS);
	dlg_initDisplayData_events( &Dialog, (T_VOID_FUNC)NULL, FOUR_SECS, KEY_CLEAR | KEY_LEFT | KEY_RIGHT );

	/* Show the dialog
	*/
	info_dialog( window, &Dialog );
}

/* SPR#1428 - SH - New Editor: moved the following two functions
 * from MmiBookInputWindow.c, which is now a redundant file */
#ifdef NEW_EDITOR

/*******************************************************************************

 $Function:		bookInputStartNumberEditor

 $Description:	Start the input handler using default properties
 
 $Returns:		Handle of newly created window, or NULL if failure

 $Arguments:	parent, parent window handle, buffer, input buffer
 
*******************************************************************************/

MfwHnd bookInputStartNumberEditor( MfwHnd parent, void *buffer )
{
	T_AUI_EDITOR_DATA editor_data;
    U16 editor_num_length = NUMBER_LENGTH;;
	TRACE_FUNCTION ("bookInputStartNumberEditor()");

    // June 22, 2006 REF: DRT OMAPS00081761  x0012849
    // Number length can be upto 41 when EXT1 is allocated and activated in SIM service table
    // 40 characters + 1 Null character
    switch(bookActiveBook(WRITE))
    {
        case   PHB_ADN:
            TRACE_EVENT("ADN book is selcted");
            if (sim_serv_table_check(SRV_EXT1) == ALLOCATED_AND_ACTIVATED)
            {
                editor_num_length = NUMBER_LENGTH;
                TRACE_EVENT("SIM is EXT1 supported");
            }
            else
            {
                // June 22, 2006 REF: DRT OMAPS00081761  x0012849
                // When EXT1 is not supported Number can be upto 20 characters.
                editor_num_length = OLD_NUM_LENGTH;
                TRACE_EVENT("EXT1 is not supported for this SIM");             
            }             
            break;

        case   PHB_FDN:
            TRACE_EVENT("FDN book is selcted");            
            // June 22, 2006 REF: DRT OMAPS00081761  x0012849
            //If EXT2 is present in service table then the FDN phone number can go upto 40 digits.
            if (sim_serv_table_check(SRV_EXT2) == ALLOCATED_AND_ACTIVATED)
            {
                editor_num_length = NUMBER_LENGTH;
                TRACE_EVENT("SIM is EXT2 supported");
            }
            else
            {
                 // June 22, 2006 REF: DRT OMAPS00081761  x0012849
                 // When EXT1 is not supported Number can be upto 20 characters.
                editor_num_length = OLD_NUM_LENGTH;
                TRACE_EVENT("EXT2 is not supported for this SIM");             
            }                
            break;

        case   PHB_IPB: // Internal phone book
            TRACE_EVENT("IPB - Internal phone book selected");
            editor_num_length = NUMBER_LENGTH; 
            break;            
        default:
            TRACE_EVENT("Selected phone book is other than ADN,FDN,IPB");
            editor_num_length = NUMBER_LENGTH;             
            break;
    }    
    
    TRACE_EVENT_P1 ("Number length = %d ",editor_num_length);
	AUI_edit_SetDefault(&editor_data);
	AUI_edit_SetDisplay(&editor_data, NUMBER_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
	AUI_edit_SetEvents(&editor_data, 0, FALSE, FOREVER, (T_AUI_EDIT_CB)bookAddNumberCB);
	// Change by Sasken ( Deepa M.d) on March 29th 2004
	// Issue Number : MMI-SPR-12722
	// Subject: Right softkey should be "Back" when the editor is empty
	// Bug : Right softkey should be "Back" when the editor is empty
	// Solution : Added alternative string for the right softkey tobe displayed when the editor is empty

	AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtEnterNumber, NULL);
	AUI_edit_SetAltTextStr(&editor_data, 0, NULL, TRUE, TxtSoftBack);
     // June 22, 2006 REF: DRT OMAPS00081761  x0012849
    // Changed the limit frommm NUMBER_LENGTH to editor_num_length.
	AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)buffer, editor_num_length);
	AUI_edit_SetMode(&editor_data, 0, ED_CURSOR_UNDERLINE);

	return AUI_edit_Start(parent, &editor_data);
}



/*******************************************************************************

 $Function:		bookInputStartNameEditor

 $Description:	Start the input handler using default properties
 
 $Returns:		Handle of newly created window, or NULL if failure

 $Arguments:	parent, parent window handle, buffer, input buffer
 
*******************************************************************************/

MfwHnd bookInputStartNameEditor( MfwHnd parent, void *buffer )
{
	T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) parent )->data;
	tBookStandard   *data       = (tBookStandard *) win_data->user;
	T_phbk          *Phbk       = data->phbk;
	T_AUI_EDITOR_DATA editor_data;
	U16             editor_length;

	TRACE_FUNCTION ("bookInputStartNameEditor()");

	/* xreddymn OMAPS00079431 May-29-2006
	 * Restrict the number of characters that the user can input for name
	 * based on which phonebook is selected.
	 */
	if(bookGetBookSelected() == TRUE)
	{
		/* For ME phonebook use MAX_ALPHA_LEN */
		editor_length = MAX_ALPHA_LEN;
	}
	else
	{
		/* For SIM phonebook, restrict length to 14 */
		editor_length = 15;
	}

    /* Set up the default parameters for the input window */

#if defined(CHINESE_MMI) && defined(EASY_TEXT_ENABLED)
	/*MC, the chinese editor MUST have a unicode string as input*/
	/*MC , SPR 1242 merged in from b-sample build*/
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
	{
		T_CHINESE_DATA chinese_data;
		chinese_data.TextString = (char*) Phbk->edt_buf_name;
		if (chinese_data.TextString[0]== 0)//if, empty string
		{
			chinese_data.TextString[0] = (char)0x80;//give string Unicode tag /*a0393213 lint warnings removal-typecast done*/
			chinese_data.TextString[1] = 0x7F;
		}
		else
		{	
			if (chinese_data.TextString[0]!= (char)0x80)/*If ASCII string*//*a0393213 lint warnings removal-typecast done*/
			{
			
			/*SPR 1752, if ASCII string, use a latin editor*/
				AUI_edit_SetDefault(&editor_data);
				AUI_edit_SetDisplay(&editor_data, NUMBER_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
				AUI_edit_SetEvents(&editor_data, 0, FALSE, FOREVER, (T_AUI_EDIT_CB)bookAddNameCB);
				AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtEnterName, NULL);
				/* xreddymn OMAPS00079431 May-29-2006: Length changes based on selected phonebook */
				AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)buffer, editor_length);
				AUI_edit_SetMode(&editor_data, ED_MODE_ALPHA, ED_CURSOR_BAR);

				return AUI_edit_Start(parent, &editor_data);
			}
		}
		/*MC end*/
		chinese_data.Callback = (T_AUI_EDIT_CB)bookAddNameCB;
		chinese_data.Identifier = INPUT_LEFT ;
		chinese_data.LeftSoftKey = TxtSave;
		chinese_data.DestroyEditor = TRUE;
		/* xreddymn OMAPS00079431 May-29-2006: Length changes based on selected phonebook */
		chinese_data.EditorSize = (USHORT)((editor_length-4)/2);//i.e.8
		return chinese_input(parent, &chinese_data);
	}
	else
#endif /* CHINESE_MMI && EASY_TEXT_ENABLED */
	{
		AUI_edit_SetDefault(&editor_data);
		AUI_edit_SetDisplay(&editor_data, NUMBER_EDITOR, COLOUR_EDITOR_XX, EDITOR_FONT);
		AUI_edit_SetEvents(&editor_data, 0, FALSE, FOREVER, (T_AUI_EDIT_CB)bookAddNameCB);
		AUI_edit_SetTextStr(&editor_data, TxtSoftOK, TxtDelete, TxtEnterName, NULL);
/*SPR 1752*/
#ifdef NO_ASCIIZ
		if (Phbk->edt_buf_name[0] ==0x80)
			/* xreddymn OMAPS00079431 May-29-2006: Length changes based on selected phonebook */
			AUI_edit_SetBuffer(&editor_data, ATB_DCS_UNICODE, &Phbk->edt_buf_name[2], editor_length/2 -2);
		else
#endif
			/* xreddymn OMAPS00079431 May-29-2006: Length changes based on selected phonebook */
			AUI_edit_SetBuffer(&editor_data, ATB_DCS_ASCII, (UBYTE *)buffer, editor_length);
		AUI_edit_SetMode(&editor_data, ED_MODE_ALPHA, ED_CURSOR_BAR);

		return AUI_edit_Start(parent, &editor_data);
	}
}



/*******************************************************************************

 $Function:		bookInputDestroy

 $Description:	Destroy Input Window
 
 $Returns:		None

 $Arguments:	Handle of window to be destroyed
 
*******************************************************************************/

void bookInputDestroy( MfwHnd window )
{
	TRACE_FUNCTION ("bookInputDestroy()");
	/*MC SPR 1752, send destroy event to window*/
	SEND_EVENT( window, E_ED_DESTROY, 0, 0 );

}

extern T_MFW_HND PhbSearchTim;
/*******************************************************************************

 $Function:     bookAddNameCB

 $Description:  default call back for name entry

 $Returns:    None

 $Arguments:  win, handle of parent window, reason, for invocation

*******************************************************************************/

void bookAddNameCB( T_MFW_HND win, USHORT Identifier, SHORT reason )
{


    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;

	TRACE_FUNCTION("bookAddNameCB");

	switch (reason)
    {
		case INFO_KCD_LEFT:
        {	
            /* Construct a new entry to be written to the phone book
            */

			/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
			if (Phbk->edt_buf_name[0] == 0x80 )//MC, if unicode
			{	//MC, if empty string, remove 0x80 at beginning
				if (Phbk->edt_buf_name[2]==0x00 &&  Phbk->edt_buf_name[3] == 0x00)
					memset(Phbk->edt_buf_name, 0, MAX_ALPHA_LEN);
			}

	// xvilliva SPR21547 We check for presence of either phone number or name.
	//				    We add or modify the entry and flash an alert correspondingly.
	if(strlen((char *) Phbk->edt_buf_number)>0 ||  strlen((char *)Phbk->edt_buf_name)>0)
	{
            /* Store in the current phone book
            */
            if ( Phbk->UpdateAction == CREATE_ENTRY )
				storeInFile( Phbk );
            else if ( Phbk->UpdateAction == MODIFY_EXISTING )
				changeInFile( Phbk );				
            bookShowInformation( win, MmiBookCurrentText(), NULL,NULL );

			/* Sep 20, 2006 REF: OMAPS00145786	x0080701 (Bharat) -- Start */
			mmi_int_phb_store_change_completion();
			/* Sep 20, 2006 REF: OMAPS00145786	x0080701 (Bharat) -- End */			
			
	}
	else
		bookShowInformation( win, TxtInvalidInput, NULL,NULL );	

            /* And shut down, clearing the number and name windows as we go
            */
            if ( Phbk->input_number_win )
            {    
 				AUI_edit_Destroy(Phbk->input_number_win);
    		    Phbk->input_number_win = 0;
    		}
			/*SPR 1752, send destroy event rather than calling destroy function*/
			SEND_EVENT( Phbk->input_name_win, E_ED_DESTROY, 0, 0 );
    		Phbk->input_name_win = 0;

		// Jan 22, 2006 REF: OMAPS00061930  x0039928
		// Fix : Start the phonebook timer in case of trying to modify the existing entry
		if ( Phbk->UpdateAction == MODIFY_EXISTING )
			timStart(PhbSearchTim);

			/*MC-- get rid of search window(and options menu), if just changed a chinese entry 
			(prevent showing scrambled entry)*/
			if ( Phbk->UpdateAction == MODIFY_EXISTING && Mmi_getCurrentLanguage()==CHINESE_LANGUAGE)
			{
				bookMenuDestroy(Phbk->menu_options_win);
				Phbk->menu_options_win=0;
				bookSearchDestroy( Phbk->search_win);
				Phbk->search_win=0;
			}
        }
		break;

        case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:
            /* Both right and HUP can do the same task here
            */
            /*SPR 1752, send destroy event rather than calling destroy function*/
            SEND_EVENT( Phbk->input_name_win, E_ED_DESTROY, 0, 0 );
            Phbk->input_name_win = 0;
		    break;

        default:
        {
            /* otherwise no action to be performed
            */
        break;
        }
  }
  if (Phbk->root_win==Phbk->win)    /*To destroy the phonebook when entering from idle*/
    bookPhonebookDestroy(Phbk->win);
}


/*******************************************************************************

 $Function:     bookAddNumberCB

 $Description:  Default call back for number entry

 $Returns:    None

 $Arguments:  win, parent window, reason, for call back

*******************************************************************************/

void bookAddNumberCB( T_MFW_HND win, USHORT Identifier, SHORT reason )
{
    T_MFW_WIN		*win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard	*data       = (tBookStandard *) win_data->user;

	switch ( reason )
    {
        case INFO_KCD_LEFT:
			data->phbk->input_name_win = bookInputStartNameEditor( data->win, data->phbk->edt_buf_name );
	    	break;

		case INFO_KCD_RIGHT:
	    case INFO_KCD_HUP:
            /* Both right and HUP can do the same task here
            */
            AUI_edit_Destroy( data->phbk->input_number_win );
            data->phbk->input_number_win = 0;
	    	break;

		default:
            /* otherwise no action to be performed
            */
			break;
	}
}


#else /* NEW EDITOR */


/*******************************************************************************

 $Function:     bookAddNameCB

 $Description:  default call back for name entry

 $Returns:    None

 $Arguments:  win, handle of parent window, reason, for invocation

*******************************************************************************/

void bookAddNameCB( T_MFW_HND win, UBYTE reason )
{
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;
    T_phbk          *Phbk       = data->phbk;
  TRACE_FUNCTION("bookAddNameCB");
  switch (reason)
    {
		case INPUT_LEFT:
        {	

            /* Construct a new entry to be written to the phone book
            */
#ifdef NO_ASCIIZ
/*MC SPR 1257, replacing PHB_MAX_LEN with MAX_ALPHA_LEN for name strings*/
			if (Phbk->edt_buf_name[0] == 0x80 )//MC, if unicode
			{	//MC, if empty string, remove 0x80 at beginning
				if (Phbk->edt_buf_name[2]==0x00 &&  Phbk->edt_buf_name[3] == 0x00)
					memset(Phbk->edt_buf_name, 0, MAX_ALPHA_LEN);
			}
#endif

            /* Store in the current phone book
            */
            if ( Phbk->UpdateAction == CREATE_ENTRY )
          storeInFile( Phbk );
            else if ( Phbk->UpdateAction == MODIFY_EXISTING )
          changeInFile( Phbk );

				
            bookShowInformation( win, MmiBookCurrentText(), NULL,NULL );

			/* Sep 20, 2006 REF: OMAPS00145786	x0080701 (Bharat) -- Start */
			mmi_int_phb_store_change_completion();
			/* Sep 20, 2006 REF: OMAPS00145786	x0080701 (Bharat) -- End */			
				

            /* And shut down, clearing the number and name windows as we go
            */
            if ( Phbk->input_number_win )
            {
		        SEND_EVENT( Phbk->input_number_win, INPUT_DESTROY, 0, 0 );
    		    Phbk->input_number_win = 0;
    		}


			SEND_EVENT( Phbk->input_name_win, INPUT_DESTROY, 0, 0 );
			Phbk->input_name_win = 0;

#ifdef NO_ASCIIZ
/*MC-- get rid of search window(and options menu), if just changed a chinese entry 
(prevent showing scrambled entry)*/
			if ( Phbk->UpdateAction == MODIFY_EXISTING &&Mmi_getCurrentLanguage()==CHINESE_LANGUAGE)
			{	bookMenuDestroy(Phbk->menu_options_win);
				Phbk->menu_options_win=0;
				bookSearchDestroy( Phbk->search_win);
				Phbk->search_win=0;
				
			}
#endif
			

        }
		break;

        case INPUT_RIGHT:
		case INPUT_CLEAR:
        {
            /* Both right and clear can do the same task here
            */
			SEND_EVENT( Phbk->input_name_win, INPUT_DESTROY, 0, 0 );
            Phbk->input_name_win = 0;
        }
    break;

        default:
        {
            /* otherwise no action to be performed
            */
        break;
        }
  }
  if (Phbk->root_win==Phbk->win)    /*To destroy the phonebook when entering from idle*/
    bookPhonebookDestroy(Phbk->win);
}


/*******************************************************************************

 $Function:     bookAddNumberCB

 $Description:  Default call back for number entry

 $Returns:    None

 $Arguments:  win, parent window, reason, for call back

*******************************************************************************/

void bookAddNumberCB( T_MFW_HND win, UBYTE reason )
{
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tBookStandard   *data       = (tBookStandard *) win_data->user;

  switch ( reason )
    {
        case INPUT_LEFT:
        {
      data->phbk->input_name_win =
                bookInputStartNameEditor( data->win, data->phbk->edt_buf_name );
        }
    break;

        case INPUT_RIGHT:
    case INPUT_CLEAR:
        {
            /* Both right and clear can do the same task here
            */
            SEND_EVENT( data->phbk->input_number_win, INPUT_DESTROY, 0, 0 );
            data->phbk->input_number_win = 0;
        }
    break;

        default:
        {
            /* otherwise no action to be performed
            */
        break;
        }
  }
}

#endif /* NEW_EDITOR */
// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
// This is called when we exit the FDN Numbers menu. So unsetting the menuFDN flag here

void fdn_menu_cb(T_MFW_HND parent_win, UBYTE identifier, UBYTE reason)
{
	TRACE_EVENT("fdn_menu_cb");
	menuFDN=0;
}

// May 3, 2004        REF: CRR MMI-SPR-18555  Rashmi C N(Sasken) 
// This is called to display the FDN Numbers menu. The menuFDN flag is set here

int bookFDNMenu(MfwMnu* menu, MfwMnuItem* item)
{
	T_MFW_HND 		win = mfw_parent(mfw_header());
	T_MFW_WIN		*win_data	= ((T_MFW_HDR *) win)->data;
	tBookStandard	*data		= (tBookStandard *) win_data->user;
	

	
	menuFDN=1;
  	// Sep 29, 2004        REF: CRR 25041  xvilliva  
	// The bookMenuStart() is called with the window data->phbk->win, 
	// similar to the function bookFDNMenu() to avoid crash. The menu window 
	// thus created is stored, which will be deleted while phonebook menu is 
	// destroyed. 
	data->phbk->menu_call_options_win_2 = bookMenuStart( data->phbk->win, bookNameMenuFDNAttributes(),0);
	SEND_EVENT(data->phbk->menu_call_options_win_2, ADD_CALLBACK, NULL, (void *)fdn_menu_cb);

      return 0; /* to remove warning Aug - 11 */
}

// Jul 28, 2004       	REF: CRR:20887 xrashmic - SASKEN
// Description: Current state of FDN not highlighted
// Fix: Instead of showing the submenu directly we call this function 
// where we set the menu and also higlight the previously selected menu item

extern MfwMnuAttr *FDNMenu(void);
void settingsFocusFDNCurSel(void)
{
	UBYTE			CurSel;  //, state; // RAVI



	T_MFW_HND 		win = mfw_parent(mfw_header());
	T_MFW_WIN		*win_data	= ((T_MFW_HDR *) win)->data;
	tBookStandard	*data		= (tBookStandard *) win_data->user;

  	// Sep 29, 2004        REF: CRR 25041  xvilliva  
	// The bookMenuStart() is called with the window data->phbk->win, 
	// similar to the function bookFDNMenu() to avoid crash. The menu window 
	// thus created is stored, which will be deleted while phonebook menu is
	// destroyed. 
	data->phbk->menu_options_win_2= bookMenuStart(data->phbk->win, FDNMenu(), 0);
	CurSel = (phb_get_mode() == PHB_RESTRICTED)?FIRSTMENUITEM:SECONDMENUITEM;
	SEND_EVENT(data->phbk->menu_options_win_2, DEFAULT_OPTION, NULL, &CurSel);
}

// June 07,  2005         REF: CRR MMI-FIX-31545 x0021334
// Issue description: SIM: wrong behavior when FDN is not allocated in SST
// Added this function to check if FDN is allocated
void checkFDNSupport(void)
{
	T_MFW_HND 		win = mfw_parent(mfw_header());



       UBYTE serv_stat;
       T_DISPLAY_DATA   display_info;

	// Check in SIM service table if FDN is allocated. If not allocated, display this info dialog
	/* x0045876, 14-Aug-2006 (WR - "serv_stat" was set but never used) */
	serv_stat = sim_serv_table_check(SRV_FDN);
	/*if ((serv_stat = sim_serv_table_check(SRV_FDN)) EQ NO_ALLOCATED)*/
	if (serv_stat  EQ NO_ALLOCATED)
	{
		dlg_initDisplayData_TextId(&display_info,  TxtSoftOK, NULL, TxtNotSupported, TxtNull, COLOUR_STATUS);
		dlg_initDisplayData_events(&display_info, (T_VOID_FUNC)NULL, THREE_SECS, KEY_LEFT | KEY_RIGHT);
		info_dialog (win, &display_info);
	}
	else // Else show the FDN menu and set focus on current selection
		settingsFocusFDNCurSel();
}

/* Sep 20, 2006 REF: OMAPS00145786	x0080701 (Bharat) -- Start */

/*******************************************************************************

 $Function:      mmi_int_phb_store_change_completion

 $Description:  Reset all flags on adding / editing an Internal PHB Entry

 $Returns:    	None

 $Arguments:  None

*******************************************************************************/
static void mmi_int_phb_store_change_completion()
{
	/* Reset the "add" and "change" flags only incase of Internal PHB */
	if ( bookGetBookSelected() == TRUE ) 
	{
		addname_flag = FALSE;	
			
		changename_flag = FALSE;
	}
	
	return;
}

/* Sep 20, 2006 REF: OMAPS00145786	x0080701 (Bharat) -- End */

/*******************************************************************************

                                End of File

*******************************************************************************/

