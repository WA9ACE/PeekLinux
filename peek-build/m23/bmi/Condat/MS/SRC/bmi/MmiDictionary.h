#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_
/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                        
 $Module:		MMI
 $File:		    MmiDictionary
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		                                                      
                                                                               
********************************************************************************
                                                                              
 Description:  Headers of predictive text dictionary functions.  
 Interfaces between the Lation text editor and the EziText API.
                        
********************************************************************************
 $History: MmiDictionary.h

	
	   
 $End

*******************************************************************************/
//#include "STDDEFS.H"
#define ENGLISH 1

//initialising dictionary given language and entry mode e.g. stroke, pinyin
int Initialize_Dict(UBYTE language, UBYTE entry_mode);

//add a char to end of word, returns first matching candidate
int DictAlphaKeyPress(UBYTE keycode, char* first_candidate);


//candidate selected
//need to reset everything.
int  ResetDictSearch();

//get the next set of candidates
//At the moment we only get one candidate at a time, but this allows us to get more than one
int MoreCandidates(char* first_candidate, int* no_of_candidates);

//takes in a string and converts it to keypresses, getting the first matching candidate back
int ConvertToKeyPresses(char* string, char* first_candidate);

//deletes the last char off the current word
int DictBackSpace(char* first_candidate);

#endif
