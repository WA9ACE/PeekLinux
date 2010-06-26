#ifndef _DEF_MMI_USERDATA_H_
#define _DEF_MMI_USERDATA_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiUserData.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the root mofule for the basic MMI
                        
********************************************************************************
 $History: MmiUserData.h

	
	   
 $End

*******************************************************************************/




#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY  mfwFree



/*
 * The following enum is the _only_ place where keys for the userData mechanism
 * can be defined. Everybody _can_ and _must_ define their own keys for
 * _every_ particular use of userData.
 */
typedef enum
{
    UD_SOFTKEYS,
	UD_OLD_FOCUS,
    UD_SAT_BITS,
	UD_SAT_HANDLE,
    UD_SAT_KEYBOARD,
    UD_SAT_MENU,
    UD_SAT_MENU_ATTRIBUTES,
    UD_SAT_MENU_IDS,
    UD_SAT_MENU_STRINGPOOL,
    UD_SAT_SETUPMENU_COMMAND,
    UD_KBD,
	UD_PARENTWIN,
	UD_MENU
} UserKey;


/*
 * Please note that all functions below come in a userDataHnd... and userDataWin...
 * version, which have identical functionality, but get the <window> passed
 * in MfwHnd or MfwWin pointer form respectively.
 * You can just use either one depending on what you got hold of at the moment.
 */


void *userDataHndSet( MfwHnd window, UserKey key, void *data);

void *userDataWinSet( MfwWin *window, UserKey key, void *data);
/*
 * Sets the user data for <window> with <key> to <data>,
 * and returns the previous contents of the user data.
 * If <data> itself is returned no user data with <key>
 * existed and a new user data element with <key> has been created
 * and set to <data>.
 * If NULL is returned, either <window> was invalid or a new user data
 * element could not be created.
 */


void *userDataHndGet( MfwHnd window, UserKey key);

void *userDataWinGet( MfwWin *window, UserKey key);
/*
 * Returns the user data for <window> with <key>.
 * If NULL is returned, either <window> was invalid or no user data
 * with <key> existed.
 */


void *userDataHndDelete( MfwHnd window, UserKey key);

void *userDataWinDelete( MfwWin *window, UserKey key);
/*
 * Deletes the user data for <window> with <key>, and returns the
 * contents of the user data.
 * If NULL is returned, either <window> was invalid or no user data
 * with <key> existed.
 */


#endif

