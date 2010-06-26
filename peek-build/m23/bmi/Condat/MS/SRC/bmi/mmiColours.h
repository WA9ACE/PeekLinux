#ifndef _MMI_Colours_H_
#define _MMI_Colours_H_
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		
 $File:		    mmiColours.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/01/02                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
	This header file provides definitions for various screen colours to allow us to
	set different parts of the display to foreground and background colours.
	
                        
********************************************************************************

 $History:

	25/01/02			Original Condat(UK) BMI version.	
	28/11/02 -	Corrected definitions for some colours
				Removed unused definitions/commented out code.
				
 $End

*******************************************************************************/

#define COL_R  0x00FF0000
#define COL_RG 0x00FFFF00
#define COL_G  0x0000FF00
#define COL_GB 0x0000FFFF
#define COL_B  0x000000FF
#define COL_BR 0x00FF00FF
#define COL_W	0x00FEFEFE
#define COL_BLK	0x00010101

#define COL_R2  	0x00800000
#define COL_RG2 	0x00808000
#define COL_G2  	0x00008000
#define COL_GB2 	0x00008080
#define COL_B2  	0x00000080
#define COL_BR2 	0x00800080
#define COL_W2		0x00808080

#define COL_TRANSPARENT	0xFF000000

#define COL_BG COL_GB
#define COL_RB COL_BR
#define COL_GR COL_RG

#define COL_Black		COL_BLK
#define COL_Red 		COL_R
#define COL_Green		COL_G
#define COL_Blue 		COL_B
#define COL_Magenta		COL_RB
#define COL_Cyan 		COL_GB
#define COL_Yellow 		COL_RG
#define COL_White		COL_W
#define COL_Pink		0x00FF4040 //0x00FF80FF
#define COL_Orange		0x00FF8000
#define COL_LightBlue	0x004040FF //0x008080FF
#define COL_LightGreen	0x0040FF40 //0x0080FF80
#define COL_PaleYellow	0x00FFFF40 //0x00FFFF80
#define COL_MidGrey		0x00404040
#define COL_Transparent		0xFF000000
/***************************Go-lite Optimization changes Start***********************/
//Aug 25, 2004    REF: CRR 24297  Deepa M.D 
//Define  colors for Go-lite
#define COL_Lite_Black      0x00
#define COL_Lite_White      0x01
/***************************Go-lite Optimization changes end***********************/


enum {
	FIRST_COLOUR_DEF,

	//generic status/popup windows
	COLOUR_STATUS,	//generic status window
	COLOUR_POPUP,	//generic status window

	//Colour for all lists (unless otherwise specifically defined)
	COLOUR_LIST_MAIN,			//main icons
	COLOUR_LIST_SUBMENU,		//sub-menu
	//The following are currently unused but can be used for other sub-menus
	COLOUR_LIST_SUBMENU1,	
	COLOUR_LIST_SUBMENU2,	
	COLOUR_LIST_SUBMENU3,
	COLOUR_LIST_SUBMENU4,

	//Special define when list elements have their own associated colour.
	COLOUR_LIST_COLOUR,

	//colour for all editors
	COLOUR_EDITOR,
	
	//Colours for specific displays
	COLOUR_GAME,
	COLOUR_IDLE,
	COLOUR_INCALL,

	//Colours for opening/closing animation 
	COLOUR_STATUS_WELCOME,
	COLOUR_STATUS_GOODBYE,


	LAST_COLOUR_DEF
};
/*
 #define COLOUR_XX 						COLOUR_LIST_SUBMENU
 #define COLOUR_POPUP_XX 				COLOUR_POPUP
*/

#define COLOUR_LIST_XX					COLOUR_LIST_SUBMENU
#define COLOUR_ICON_XX					COLOUR_IDLE
#define COLOUR_EDITOR_XX				COLOUR_EDITOR


#define COLOUR_STATUS_INCOMINGCALL	 COLOUR_INCALL
#define COLOUR_STATUS_CALLING			COLOUR_INCALL
#define COLOUR_STATUS_CCNOTIFY			COLOUR_INCALL

#define COLOUR_STATUS_WAIT				COLOUR_STATUS
#define COLOUR_STATUS_EZITEXT			COLOUR_STATUS
#define COLOUR_STATUS_NETWORKS			COLOUR_STATUS
#define COLOUR_STATUS_PINS				COLOUR_STATUS
#define COLOUR_AOC						COLOUR_STATUS

#define COLOUR_WAP_POPUP				COLOUR_POPUP	
#define COLOUR_BOOK_POPUP				COLOUR_POPUP

#define COLOUR_LIST_SETTINGS			COLOUR_LIST_SUBMENU
#define COLOUR_LIST_SMS					COLOUR_LIST_SUBMENU
#define COLOUR_LIST_PHONEBOOK			COLOUR_LIST_SUBMENU
#define COLOUR_LIST_CALL				COLOUR_LIST_SUBMENU
#define COLOUR_LIST_DIVERT				COLOUR_LIST_SUBMENU
#define COLOUR_LIST_SECURITY			COLOUR_LIST_SUBMENU
#define COLOUR_LIST_APPLICATIONS		COLOUR_LIST_SUBMENU
#define COLOUR_LIST_SIM_TOOLKIT			COLOUR_LIST_SUBMENU
#define COLOUR_LIST_NETWORK				COLOUR_LIST_SUBMENU

#define COLOUR_WAP						COLOUR_LIST_MAIN


#define COLOUR_STATUS_OKRESET			COLOUR_STATUS

//Default colour for creating windows 
//#define DEFAULT_COL   COL_RG, COL_B2

//These definitions match dspl.h
#define ICON_TYPE_1BIT_PACKED 		0x00
#define ICON_TYPE_256_COL 			0x02
#define ICON_TYPE_32BIT_COL 		0x03
#define ICON_TYPE_16BIT_COL 		0x04
#define ICON_TYPE_1BIT_UNPACKED 	0x01


#endif


