/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                            
 $Module:		MMI
 $File:		    Dixtionary.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		                                                         
                                                                               
********************************************************************************
                                                                              
 Description:  Predictive text dictionary functions.  
 Interfaces between the Lation text editor and the EziText API.

  

********************************************************************************

 $History: Dictionary.c
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

 03/10/02		Replaced most of the file with version on 3.3.3 line (MC, SPR 1242)
	   
 $End

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
//#include "MmiBookShared.h"
#include "MmiBlkLangDB.h"
#include "MmiDictionary.h"
#include "zi8api.h"
#include "mfw_kbd.h"



/************************************ MACROS****************************************/
#define MAX_NO_OF_ELEMENTS 		12
#define CANDIDATES_PER_GROUP 	1




/************************************* Data Type definitions ****************************/
typedef struct dict_data_
{
ZI8GETPARAM getParam; //global ezitext data structure
int no_of_candidates; 
int candidate_cursor;
int more_count;

} dict_data;

/*MC, SPR 1150 - Added data structure definitions*/
typedef struct
{
	UBYTE condat_lang;	/*Condat language code e.g. ENGLISH LANGAUGE*/
	UBYTE zi_lang;		/*Zi library language code */
	UBYTE zi_sub_lang;	/*Zi library sub-language code e.g.  British English*/

}lang_code_table_entry;

typedef struct
{
	UBYTE no_of_entries;
	lang_code_table_entry* entries;

}lang_code_table;
/*MC end*/
/*************************************Global Variables***********************************/

ZI8WCHAR element_buffer[MAX_NO_OF_ELEMENTS];
ZI8WCHAR candidates_buffer[MAX_NO_OF_ELEMENTS*CANDIDATES_PER_GROUP];


static dict_data Dictionary;

/*MC SPR 1150- hardcoded Language table*/
/*This is used to correlate the current language with the language settings needed for Ezitext*/
/*If we add new lanaguages, we just need to amend this table*/
lang_code_table_entry LangCodeTableEntries[] =
{
		{ENGLISH_LANGUAGE, ZI8_LANG_EN, ZI8_SUBLANG_EN_UK},
		{GERMAN_LANGUAGE, ZI8_LANG_DE, ZI8_SUBLANG_DEFAULT}
};
lang_code_table LangCodeTable = { 2, LangCodeTableEntries};
/*************************************Prototypes***********************************/
void convert_chars_to_ascii(char* string, UBYTE language);/*SPR 1508*/

/*******************************************************************************

 $Function:    	Initialize_Dict

 $Description:	Initialises Predictive Text libs
 
 $Returns:		status int

 $Arguments:	 language, text entry mode(may be used for chinese e.g. pinyin/stroke)
*******************************************************************************/
int Initialize_Dict(UBYTE language, UBYTE entry_mode)
{	int i;

	Zi8Initialize();
	/*MC, SPR 1150- more generic code for setting Zi library parameters*/
	
	/* for each entry in language table*/
	for(i=0; i< LangCodeTable.no_of_entries; i++)
	{/*if current language in table*/
		if (LangCodeTable.entries[i].condat_lang == language)
		{	/*use Zi language definitions*/
			Dictionary.getParam.Language = LangCodeTable.entries[i].zi_lang;
			Dictionary.getParam.SubLanguage = LangCodeTable.entries[i].zi_sub_lang;
			break;
		}			
	}
	/*if no entry found*/
	if (i>=LangCodeTable.no_of_entries)
		return 1; /*exit with error code*/
	Dictionary.getParam.GetMode = ZI8_GETMODE_DEFAULT;
	Dictionary.getParam.SubLanguage = ZI8_SUBLANG_EN_ALL;
	Dictionary.getParam.Context = ZI8_GETCONTEXT_DEFAULT;
	Dictionary.getParam.GetOptions = ZI8_GETOPTION_STRINGS;
	Dictionary.getParam.pElements = element_buffer;
	Dictionary.getParam.pCandidates = candidates_buffer;
	Dictionary.getParam.MaxCandidates = CANDIDATES_PER_GROUP + 1;
	Dictionary.getParam.pScratch = NULL;
	Dictionary.getParam.FirstCandidate = 0;
	
	memset(element_buffer, 0, MAX_NO_OF_ELEMENTS*sizeof(ZI8WCHAR));
	memset(candidates_buffer, 0, MAX_NO_OF_ELEMENTS*CANDIDATES_PER_GROUP);
	Dictionary.getParam.ElementCount = 0;
	Dictionary.more_count = 0;
	Dictionary.no_of_candidates = 0;
	Dictionary.candidate_cursor = 0;
	return 0;
}

/*******************************************************************************

 $Function:    	DictAlphaKeyPress

 $Description:	Passes keypress to pred text lib and puts first match into string param
 
 $Returns:		status int

 $Arguments:	 keycode, string
*******************************************************************************/
int DictAlphaKeyPress(UBYTE keycode, char* first_candidate)
{ 	
	ZI8WCHAR new_keycode=0;
	//convert keycode to Ezitext code
	
		switch (keycode)
		{	
			case KCD_1:  new_keycode = ZI8_CODE_LATIN_PUNCT; break;
			case KCD_2: new_keycode = ZI8_CODE_LATIN_ABC; break;
			case KCD_3: new_keycode = ZI8_CODE_LATIN_DEF; break;
			case KCD_4: new_keycode = ZI8_CODE_LATIN_GHI; break;
			case KCD_5: new_keycode = ZI8_CODE_LATIN_JKL; break;
			case KCD_6: new_keycode = ZI8_CODE_LATIN_MNO; break;
			case KCD_7: new_keycode = ZI8_CODE_LATIN_PQRS; break;
			case KCD_8: new_keycode = ZI8_CODE_LATIN_TUV; break;
			case KCD_9: new_keycode = ZI8_CODE_LATIN_WXYZ; break;
			default: break;
		}
	/*MC, SPR 1150  - removed any code for chinese Pinyin from this function*/
		
	//place in array of elements
	Dictionary.getParam.pElements[Dictionary.getParam.ElementCount++] = new_keycode;
	Dictionary.more_count = 0;
	Dictionary.getParam.FirstCandidate = 0; //start new word search
	Dictionary.candidate_cursor = 0;
	Dictionary.no_of_candidates = Zi8GetCandidates(&Dictionary.getParam);

	if (Dictionary.no_of_candidates == 0)
	{	TRACE_EVENT("NO candidates returned");
		return DictBackSpace(first_candidate);
	}
	else
	{
		/***************************Go-lite Optimization changes Start***********************/
		
		//Aug 16, 2004    REF: CRR 24323   Deepa M.D	
		TRACE_EVENT_P1("No of candidates returned: %d", Dictionary.no_of_candidates);
		/***************************Go-lite Optimization changes end***********************/

	}
	Zi8Copy((PZI8UCHAR)first_candidate, (&Dictionary.getParam), Dictionary.candidate_cursor);
	/*MC, SPR 1150 - removed any code for chinese Pinyin from this function*/

	/*SPR 1508*/
	if (Mmi_getCurrentLanguage() == GERMAN_LANGUAGE)
			convert_chars_to_ascii(first_candidate, Mmi_getCurrentLanguage());
	/*return 0;*/
	/* SH - '1' will now be returned if the key is accepted, 0 otherwise */
	return 1;
		
}

/*******************************************************************************

 $Function:    	ResetDictSearch

 $Description:	Resets dictionary search as match selected
 
 $Returns:		status int

 $Arguments:	none
*******************************************************************************/

int  ResetDictSearch()
{
	Dictionary.getParam.ElementCount = 0;
	Dictionary.more_count = 0;
	Dictionary.no_of_candidates = 0;
	Dictionary.candidate_cursor = 0;

	return 0;
	
}

/*******************************************************************************

 $Function:    	MoreCandidates

 $Description:	gets next (set of) candidate(s)
 
 $Returns:		status int

 $Arguments:	string, pointer to int(not used ATM)
*******************************************************************************/
//get the next set of candidates
//At the moment we only get one candidate at a time, but this allows us to get more than one
int MoreCandidates(char* first_candidate, int* no_of_candidates)
{	


	//any more candidates to retrieve?
	if (Dictionary.no_of_candidates > CANDIDATES_PER_GROUP)
	{
		Dictionary.more_count++;
		Dictionary.getParam.FirstCandidate = Dictionary.more_count  *CANDIDATES_PER_GROUP;
		Dictionary.no_of_candidates = Zi8GetCandidates(&Dictionary.getParam);

		Zi8Copy((PZI8UCHAR)first_candidate, &(Dictionary.getParam), Dictionary.candidate_cursor);
		
		//return first candidate in group
	}

	else 
	{
		Dictionary.more_count = 0;
		Dictionary.getParam.FirstCandidate = 0;
		Dictionary.no_of_candidates = Zi8GetCandidates(&Dictionary.getParam);

		Zi8Copy((PZI8UCHAR)first_candidate, &(Dictionary.getParam), Dictionary.candidate_cursor);
	}
	/*SPR 1508*/
	if (Mmi_getCurrentLanguage() == GERMAN_LANGUAGE)
		convert_chars_to_ascii(first_candidate, Mmi_getCurrentLanguage());	

	return 0;
}	

/*******************************************************************************

 $Function:    	ConvertToKeyPresses

 $Description:	Takes a string and sends it as keypresses to dictionary to start new search
 
 $Returns:		status int

 $Arguments:	input string, output string(first match)
*******************************************************************************/
//takes in a string
int ConvertToKeyPresses(char* string, char* first_candidate)
{	int no_of_chars;
	int i;
	
	ResetDictSearch();
	no_of_chars = strlen(string);
	for (i=0; i < no_of_chars; i++)
	{
		switch (string[i])
		{
			
			case ('A'):
			case ('a'):
			case ('B'):
			case ('b'):
			case ('C'):
			case ('c'):
					DictAlphaKeyPress(KCD_2, first_candidate); break;
			case ('D'):
			case ('d'):
			case ('E'):
			case ('e'):
			case ('F'):
			case ('f'):
					DictAlphaKeyPress(KCD_3, first_candidate); break;
			case ('G'):
			case ('g'):
			case ('H'):
			case ('h'):
			case ('I'):
			case ('i'):
					DictAlphaKeyPress(KCD_4, first_candidate); break;
			case ('J'):
			case ('j'):
			case ('K'):
			case ('k'):
			case ('L'):
			case ('l'):
					DictAlphaKeyPress(KCD_5, first_candidate); break;
			case ('M'):
			case ('m'):
			case ('N'):
			case ('n'):
			case ('O'):
			case ('o'):
					DictAlphaKeyPress(KCD_6, first_candidate); break;
			case ('P'):
			case ('p'):
			case ('Q'):
			case ('q'):
			case ('R'):
			case ('r'):
			case ('S'):
			case ('s'):
					DictAlphaKeyPress(KCD_7, first_candidate); break;
			case ('T'):
			case ('t'):
			case ('U'):
			case ('u'):
			case ('V'):
			case ('v'):
					DictAlphaKeyPress(KCD_8, first_candidate); break;
			case ('W'):
			case ('w'):
			case ('X'):
			case ('x'):
			case ('Y'):
			case ('y'):
			case ('Z'):
			case ('z'):
					DictAlphaKeyPress(KCD_9, first_candidate); break;
			default:  DictAlphaKeyPress(KCD_1, first_candidate); break;
		}
	}
	Dictionary.no_of_candidates = Zi8GetCandidates(&Dictionary.getParam);
	Zi8Copy((PZI8UCHAR)first_candidate, &Dictionary.getParam, Dictionary.candidate_cursor);
	/*SPR1508*/
	if (Mmi_getCurrentLanguage() == GERMAN_LANGUAGE)
			convert_chars_to_ascii(first_candidate, Mmi_getCurrentLanguage());	
	return 0;
}		
/*******************************************************************************

 $Function:    	DictBackSpace
 $Description:	Removes last char from list of elements in dictionary search
 
 $Returns:		status int

 $Arguments:  output string(first match)
*******************************************************************************/
int DictBackSpace(char* first_candidate)
{	

	if (Dictionary.more_count != 0)
	{	Dictionary.more_count --;}
	else 
		if (Dictionary.getParam.ElementCount != 0)
		{
			//replace last element with 0
			Dictionary.getParam.pElements[--Dictionary.getParam.ElementCount] = 0;
		}
	Dictionary.more_count = 0;
	Dictionary.getParam.FirstCandidate = 0; //start new word search
	Dictionary.candidate_cursor = 0;
	Dictionary.no_of_candidates = Zi8GetCandidates(&Dictionary.getParam);
	Zi8Copy((PZI8UCHAR)first_candidate, &Dictionary.getParam, Dictionary.candidate_cursor);
	/*SPR 1508*/
	if (Mmi_getCurrentLanguage() == GERMAN_LANGUAGE)
			convert_chars_to_ascii(first_candidate, Mmi_getCurrentLanguage());
	return 0;
}
/*******************************************************************************

 $Function:    	convert_chars_to_ascii
 $Description:	Changes any language-specific chars from ezitext encoding to our ascii 
 				encoding. Added for SPR 1508. 
 
 $Returns:		none

 $Arguments:  input string, language
*******************************************************************************/
	void convert_chars_to_ascii(char* string, UBYTE language)
{ 
	switch(language)
	{

		  case (GERMAN_LANGUAGE):
		  {	unsigned int i;

		  	for (i=0; i< strlen(string);i++)
		  	{
				
		  		switch (string[i])
		  	{	case (0x1e):string[i] = (char)225;break;	/*SS*/
		  		case (0x7B):string[i] = (char)132;break;	/*a with umlaut*/
		  		case (0x7c):string[i] = (char)148;break;	/*o with umlaut*/
		  		case (0x7e):string[i] = (char)154;break;	/*u with umlaut*/
		  	}

		  }
		  break;
		}
	}

}
