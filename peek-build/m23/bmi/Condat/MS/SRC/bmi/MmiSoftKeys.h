/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Softkeys
 $File:		    MmiSoftKeys.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
	
********************************************************************************
 $History: MmiSoftkeys.h

       xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357
       To display '?' to indicate to the user that help is available for a STK menu

	25/10/00			Original Condat(UK) BMI version.	

	   
 $End

*******************************************************************************/


#ifndef _DEF_MMI_SOFTKEYS_H_
#define _DEF_MMI_SOFTKEYS_H_

#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY  mfwFree


#include "MmiResources.h"


typedef enum
{
    SOFTKEYS_FAILURE,
    SOFTKEYS_CREATED,
    SOFTKEYS_CHANGED
} SoftKeysResult;

typedef struct
{
    LangTxt leftKeyLabel;
    LangTxt rightKeyLabel;
    MfwCb   leftKeyCallBack;
    MfwCb   rightKeyCallBack;
} SoftKeysSetup;

#ifdef FF_MMI_FILEMANAGER
typedef enum
{
	MENU_KEY_UP_DOWN_CENTER = 0,
	MENU_KEY_UP_DOWN_PLAY ,
	MENU_KEY_UP_DOWN_STOP,
	MENU_KEY_ALL,
	MENU_KEY_WAIT
}T_MENU_KEY;
#endif

//Softkey format definitions
#define SK_TALLBGD  	0x0001	/* blank 2 lines of text over width of display */

#define SK_ROUNDBOX  	0x0010	/* draw rounded rectangle round text*/
#define SK_FILLBOX  	0x0020	/* draw filled rectangle round text*/

#define SK_DEFAULTCOL	0x0100	/* use default colour (i.e. do not change current colours) */ 

SoftKeysResult softKeysAttributes( U8 font, UBYTE displayAttibutes);
/*
 * Change the <font> and <displayAttibutes> for _all_ softkeys.
 * All consecutive calls of softKeysUpdate() will reflect the change.
 * Returns SOFTKEYS_CHANGED when everything went fine,
 * or SOFTKEYS_FAILURE on failure.
 */


/*
 * Please note that all functions below come in a softKeysHnd... and softKeysWin...
 * version, which have identical functionality, but get the <window> passed
 * in MfwHnd or MfwWin pointer form respectively.
 * You can just use either one depending on what you got hold of at the moment.
 */


SoftKeysResult softKeysHndCreate( MfwHnd window, SoftKeysSetup *setup);

SoftKeysResult softKeysWinCreate( MfwWin *window, SoftKeysSetup *setup);
/*
 * Creates the softkeys for <window>, which will display the
 * <leftKeyLabel> and <rightKeyLabel> in the softkey area
 * on the screen, and invoke <leftKeyCallBack> or <rightKeyCallBack>
 * when the approrpiate key is pressed. Returns SOFTKEYS_CREATED when
 * everything went fine, or SOFTKEYS_FAILURE on failure.
 */


SoftKeysResult softKeysHndSet( MfwHnd window, SoftKeysSetup *changes);

SoftKeysResult softKeysWinSet( MfwWin *window, SoftKeysSetup *changes);
/*
 * Change the setup of the softkeys for <window>. Use TxtNull if you
 * don't want to change a ...Label, and NULL if you don't want to change
 * a ...CallBack. Returns SOFTKEYS_CHANGED when everything went fine,
 * or SOFTKEYS_FAILURE on failure.
 */


void softKeysHndUpdate(MfwHnd window);

void softKeysWinUpdate(MfwWin *window);
/*
 * You will need to call this whenever <window> is updated.
 */


void softkeysHndDelete(MfwHnd window);

void softkeysWinDelete(MfwWin *window);
/*
 * Deletes the softkeys for <window>.
 */


void softKeys_displayId(int leftSoftKey,int rightSoftKey, int format, unsigned int colIndex);
void softKeys_displayStr(char* leftSoftKey_str,char* rightSoftKey_str, int format, unsigned int colIndex);
void softKeys_displayStrXY(char* leftSoftKey_str,char* rightSoftKey_str, int format, unsigned int colIndex, MfwRect* rect );



void displaySoftKeys(int leftSoftKey, int rightSoftKey);

//Allow sk to be placed at locations other than the bottom of the screen
void displaySoftKeysXY(int leftSoftKey,int rightSoftKey,int lskX,int rskX, int lpos);

void displaySoftKeysBox(int leftSoftKey,int rightSoftKey, U32 fCol, U32 bCol, int format);

void displaySoftKeys_edition(int leftSoftKey,int index, int rightSoftKey);

// SH - 25/5/01
// This function provided for WAP, to allow the printing of custom softkeys
// from strings provided (otherwise same as displaySoftKeys).

void displayCustSoftKeys(char *LeftSoftKey, char *RightSoftKey);

void displayHelpSymbol(void);     //xrashmic 5 Oct, 2005 MMI-SPR-29356, MMI-SPR-29357

void displayCameraIcon(void);

#endif

