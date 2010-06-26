/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiUserData.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    22/02/01                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

  

********************************************************************************

 $History: MmiUserData.c

	25/10/00			Original Condat(UK) BMI version.	
	   
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
#include "mfw_sys.h"
#include "mfw_mfw.h"
#include "mfw_win.h"

#include "MmiMmi.h"
#include "MmiUserData.h"


typedef struct uDe
{
    UserKey     key;
    void       *data;
    struct uDe *next;
} UserDataElement, *UserDataLink;


static UserDataLink userDataFind( UserDataLink first, UserKey key)
{
    while( first != NULL && first->key != key )
    {
        first = first->next;
    }

    return first;
}


static UserDataLink userDataPrevious( UserDataLink first,
                                      UserDataLink beforeThis)
{
    if( first == beforeThis )  // <beforeThis> is the first element in the list.
    {
        return NULL;
    }

    while( first->next != beforeThis )
    {
        first = first->next;
    }

    return first;
}


/*
 * Sets the user data for <window> with <key> to <data>,
 * and returns the previous contents of the user data.
 * If <data> itself is returned no user data with <key>
 * existed and a new user data element with <key> has been created
 * and set to <data>.
 * If NULL is returned, either <window> was invalid or a new user data
 * element could not be created.
 */
void *userDataHndSet( MfwHnd window, UserKey key, void *data)
{
	if( window == NULL ) { return NULL; }
	
	return userDataWinSet( (MfwWin *)((MfwHdr *)window)->data, key, data);
}


void *userDataWinSet( MfwWin *window, UserKey key, void *data)
{
    UserDataLink thisOne;

    if( window == NULL ) { return NULL; }

    thisOne = userDataFind( (UserDataLink)window->user, key);

    if( thisOne == NULL ) // element with <key> does not exist: create.
    {
        thisOne = (UserDataLink)ALLOC_MEMORY(sizeof(UserDataElement));

        if( thisOne == NULL )
        {
            return NULL;
        }

        thisOne->key  = key;
        thisOne->data = data;
        thisOne->next = (UserDataLink)window->user;

        window->user  = (void *)thisOne;

        return data;
    }
    else
    {
        void *oldData = thisOne->data;

        thisOne->data = data;

        return oldData;
    }
}


/*
 * Returns the user data for <window> with <key>.
 * If NULL is returned, either <window> was invalid or no user data
 * with <key> existed.
 */
void *userDataHndGet( MfwHnd window, UserKey key)
{
	if( window == NULL ) { return NULL; }
	
	return userDataWinGet( (MfwWin *)((MfwHdr *)window)->data, key);
}


void *userDataWinGet( MfwWin *window, UserKey key)
{
    UserDataLink thisOne;
	
	if( window == NULL ) { return NULL; }

    thisOne = userDataFind( (UserDataLink)window->user, key);
	
	if( thisOne == NULL ) { return NULL; }
	
	return thisOne->data;
}


/*
 * Deletes the user data for <window> with <key>, and returns the
 * contents of the user data.
 * If NULL is returned, either <window> was invalid or no user data
 * with <key> existed.
 */
void *userDataHndDelete( MfwHnd window, UserKey key)
{
	if( window == NULL ) { return NULL; }
	
	return userDataWinDelete( (MfwWin *)((MfwHdr *)window)->data, key);
}


void *userDataWinDelete( MfwWin *window, UserKey key)
{
    UserDataLink doomed;

    if( window == NULL ) { return NULL; }

    doomed = userDataFind( (UserDataLink)window->user, key);

    if( doomed == NULL ) // element with <key> does not exist.
    {
        return NULL;
    }
    else
    {
        void *       oldData = doomed->data;
        UserDataLink previous;

        previous = userDataPrevious( (UserDataLink)window->user, doomed);

        if( previous == NULL ) // <doomed> is the first element in the list.
        {
			window->user = (void *)doomed->next;
        }
        else
        {
            previous->next = doomed->next;
        }

        FREE_MEMORY( (void *)doomed, sizeof(UserDataElement));

        return oldData;
    }
}

