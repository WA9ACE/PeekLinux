

#ifndef _DEF_MMI_CHINESE_H_
#define _DEF_MMI_CHINESE_H_
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMChineseInput
 $File:		    MMChineseInput.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

 
                        
********************************************************************************
 $History: MMChineseInput.h

	25/10/00			Original Condat(UK) BMI version.	
	 03/10/02		Replaced most of the file with version on 3.3.3 line (MC, SPR 1242)   
 $End

*******************************************************************************/
#include "zi8api.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif


#ifndef LSCREEN
#define CANDIDATES_PER_GROUP	7
#else
#define CANDIDATES_PER_GROUP 14
#endif
//#define TEXT_BUFFER_EDIT_SIZE	80  // size of 80 is defined for chinese SMS
#define TEXT_BUFFER_EXTRA_SPACE	20 //This is the zise allowed for element buffer.

//define for InputScreenType
#define TEXT_DISPLAY_SCREEN	  		1
#define STROKE_INPUT_SCREEN 		2
#define PINYIN_INPUT_SCREEN 		3
#define WORD_ASSOCIATION_SCREEN 	4
#define SYMBOL_INPUT_SCREEN			5

//define for EditorMode
#define STROKE_INPUT_MODE	  		10
#define PINYIN_INPUT_MODE 			11
#define LATIN_INPUT_MODE 			12

typedef enum
{
	CHINESE_INPUT_INIT, 
	CHINESE_INPUT_DESTROY, /*just destroy chinese editor window*/
	CHINESE_INPUT_DEINIT, 	/*SPR 1752, destroy chinese editor window and call callback function*/ 
	ADD_SYMBOL,
	LATIN_INPUT
} chinese_editor_events;


typedef struct
{
  char *       		TextString;
#ifdef NEW_EDITOR		/* SPR#1428 - SH - New Editor changes */
  T_AUI_EDIT_CB     	Callback;
#else /* NEW_EDITOR */
  T_EDIT_CB     	Callback;
#endif /* NEW_EDITOR */
  USHORT          	Identifier;
  int				LeftSoftKey;
  UBYTE				DestroyEditor;
  USHORT			EditorSize;
} T_CHINESE_DATA;

//typedef void (*T_CHINESE_EDIT_CB) (T_MFW_HND, USHORT, USHORT);


T_MFW_HND chinese_input (T_MFW_HND        parent_win,
                       T_CHINESE_DATA * chinese_data);
void	chinese_input_destroy		(T_MFW_HND  own_window);

int chinese_input_menu(MfwMnu* m, MfwMnuItem* i);

#endif



