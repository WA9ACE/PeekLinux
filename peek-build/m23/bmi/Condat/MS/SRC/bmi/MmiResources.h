#ifndef _MMIRESOURCES_H_
#define _MMIRESOURCES_H_ 1


/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   Resource Manager
 $File:       MmiResources.h
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

   Define a simple get text routine to fetch a pointer
   to a textual resource given a resource identifier.

   The resource identifiers are given in the MmiBlkResources.h
   module header

********************************************************************************
 $History: MmiResources.h

    May 15, 2006 DR: OMAPS00075852 - xreddymn
    Added new display zone which is located one line above the softkeys.

    May 10, 2004    REF: CRR 15753  Deepa M.D 
    Menuitem provided to set the validity period to maximum.

    25/10/00      Original Condat(UK) BMI version.
    03/10/02		Replaced most of the file with version on branch for issue 1150 (MC, SPR 1242)

 $End

*******************************************************************************/

#include "MmiBlkLangDB.h"


/********************************/
/* Positions depending on the language */
//Positions are now in mmiResources.C

#define LAST_LINE (-1)
#define LAST_LINE_TOP (-51)
#define SECOND_LAST_LINE_TOP (-52)

//#define SCREEN_SIZE_X 160
//#define SCREEN_SIZE_Y 120
//#define CHINESE_MENU_VERTICAL_SIZE 84
//#define STANDARD_MENU_VERTICAL_SIZE 84

//#if ((NBRD == 40) || (NBRD == 41))
//#define COLOURDISPLAY
//#endif


#ifndef LSCREEN
#define SCREEN_SIZE_X 				84
#define SCREEN_SIZE_Y 				48 
#define LINE_HEIGHT 				8
#define CHAR_WIDTH	 				6
#define LINE_HEIGHT_LATIN			(LINE_HEIGHT)
#define LINE_HEIGHT_CHINESE 		12

#define ALLOFFSET					(0)
#define MNUOFFSET					(0)

#else
// by nitin for  QVGA LCD
#if (BOARD==71)
#define SCREEN_SIZE_X 				320
#define SCREEN_SIZE_Y 				240
#else
#define SCREEN_SIZE_X 				176	
#define SCREEN_SIZE_Y 				220	
#endif //(BOARD==61)

#define LINE_HEIGHT 				16
#define CHAR_WIDTH	 				6
#define LINE_HEIGHT_CHINESE 		(LINE_HEIGHT)
#define LINE_HEIGHT_LATIN			(LINE_HEIGHT)

#define ALLOFFSET					(LINE_HEIGHT*2)
#define MNUOFFSET					(LINE_HEIGHT)

#endif
#define ALLBUT1LINE (SCREEN_SIZE_Y-1*LINE_HEIGHT+ALLOFFSET)
#define ALLBUT2LINE (SCREEN_SIZE_Y-2*LINE_HEIGHT+ALLOFFSET)
#define ALLBUT3LINE (SCREEN_SIZE_Y-3*LINE_HEIGHT+ALLOFFSET)
#define MNU3LINE (LINE_HEIGHT*3+MNUOFFSET)



extern int Easy_Text_Available;
//////////////////////////
// Common window sizes	//
//////////////////////////

#define BOOKSEARCH_EDITOR			(ZONE_BOTTOM_LINE)	

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
/* SH - moved here from MmiEditor.h */
//Zone data for drawing windows
#define ZONE_FULL_HEIGHT				0x000F
#define ZONE_TOPHALF_HEIGHT				0x000C
#define ZONE_MIDDLE_HEIGHT				0x0006
#define ZONE_BOTTOMHALF_HEIGHT			0x0003
/* xreddymn OMAPS00075852 May-15-2006
 * This display zone is located one line above the softkeys.
 */
#define ZONE_MIDDLETWOLINES_HEIGHT		0x0002
#define ZONE_BOTTOMTWOLINES_HEIGHT		0x0001
#define ZONE_FULL_WIDTH					0x00F0
#define ZONE_LEFTHALF_WIDTH				0x00C0
#define ZONE_MIDDLE_WIDTH				0x0060
#define ZONE_RIGHTHALF_WIDTH			0x0030

#define ZONE_ICONS						0x0100
#define ZONE_TITLE						0x0200
#define ZONE_CASE_ABC					0x0400
#define ZONE_SOFTKEYS					0x1000
#define ZONE_SCROLLBAR					0x2000


#define ZONE_FULLSCREEN					(ZONE_FULL_HEIGHT | ZONE_FULL_WIDTH)
#define ZONE_FULLICONS					(ZONE_ICONS|ZONE_FULL_HEIGHT | ZONE_FULL_WIDTH)
#define ZONE_FULLTITLE					(ZONE_TITLE|ZONE_FULL_HEIGHT | ZONE_FULL_WIDTH)
#define ZONE_FULLSOFTKEYS				(ZONE_SOFTKEYS|ZONE_FULL_HEIGHT | ZONE_FULL_WIDTH)
#define ZONE_FULL_SK_SCROLL				(ZONE_SOFTKEYS|ZONE_FULL_HEIGHT | ZONE_FULL_WIDTH | ZONE_SCROLLBAR)

#define ZONE_FULL_SK_TITLE				(ZONE_TITLE|ZONE_SOFTKEYS|ZONE_FULL_HEIGHT | ZONE_FULL_WIDTH)
#define ZONE_FULL_SK_TITLE_ALPHA		(ZONE_TITLE|ZONE_SOFTKEYS|ZONE_FULL_HEIGHT | ZONE_FULL_WIDTH | ZONE_CASE_ABC)
#define ZONE_FULL_SK_TITLE_SCROLL		(ZONE_TITLE|ZONE_SOFTKEYS|ZONE_FULL_HEIGHT | ZONE_FULL_WIDTH | ZONE_SCROLLBAR)
#define ZONE_WAP_EDITOR					(ZONE_TITLE|ZONE_SOFTKEYS|ZONE_FULL_HEIGHT | ZONE_FULL_WIDTH)
#define ZONE_MIDDLE_EDITOR				(ZONE_TITLE|ZONE_SOFTKEYS|ZONE_MIDDLE_WIDTH|ZONE_MIDDLE_HEIGHT)

//Specific editor sizes
#define ZONE_SMALL_EDITOR				(0xFF00)	/* editor for small (one line) editors */
#define ZONE_BORDER_EDITOR 				(0xFE00)    /* editors with a border to the left/right - sim toolkit */
#define ZONE_BOTTOM_LINE	 			(0xFD00)    /* editors for the bottom line */
#define ZONE_BOTTOM_LINE_FIND 			(0xFC00)    /* editors on the bottom line after "Find:" */
#endif /* NEW_EDITOR */

#ifndef LSCREEN
#define BOOKDETAILS_EDITOR			0xFF01
#define NUMBER_EDITOR				0xFF02
#define PHONEBOOK_DLG_EDITOR		0xFF03
#define BOOKSDN_EDITOR				0xFF04

//MMIbooksearchwindow
//#define BOOKSEARCH_EDITOR			0xFF05
#define CALL_EDITOR 				0xFF06
#define CALLSCRATCHPAD_EDITOR		0xFF07
#define SERVICE_CREATE_EDITOR		0xFF08
#define PASSWORD_EDITOR				0xFF09
#define EM_CREATE_EDITOR		    0xFF0A
#define WIN_DIALLING				0xFF30
#define WIN_DIALLING_CB				0xFF31
#define WIN_DIALLING_SMS			0xFF32
#define WIN_DIALLING_SAT			0xFF33

#define PHNO_EDITOR					0xFF0C

#define CENTRE_EDITOR				0xFF0D

//And for MMIServices - moved here from mmiservices.c
#define SAT_EDITOR					0xFF0E

#define RPWD_EDITOR					0xFF0F

//MMIBookCallDetails
#define PHB_EDITOR 					0xFF10

//MMIPins
#define PIN_EDIT_RESET				0xFF13

//MMITimeDate
#define TIMEDATE_EDITOR_AREA		0xFF15

//#define TIMEDATE_EDITOR_AREA 0,20,70,20
#else
#define BOOKDETAILS_EDITOR			ZONE_FULL_SK_TITLE	
#define NUMBER_EDITOR				ZONE_FULL_SK_TITLE	
#define PHONEBOOK_DLG_EDITOR		ZONE_FULL_SK_TITLE	
#define BOOKSDN_EDITOR				ZONE_FULL_SK_TITLE	

//MMIbooksearchwindow
#define CALL_EDITOR 				ZONE_FULL_SK_TITLE	
#define CALLSCRATCHPAD_EDITOR		ZONE_FULL_SK_TITLE	
#define SERVICE_CREATE_EDITOR		ZONE_FULL_SK_TITLE	
#define EM_CREATE_EDITOR		    ZONE_FULL_SK_TITLE	
#define PASSWORD_EDITOR				ZONE_FULL_SK_TITLE
/* SPR#1428 - SH - New Editor changes - change these so they look better */
#ifdef NEW_EDITOR
#define WIN_DIALLING				(ZONE_ICONS|ZONE_SOFTKEYS|ZONE_FULL_WIDTH|ZONE_BOTTOMTWOLINES_HEIGHT)
/* xreddymn OMAPS00075852 May-15-2006: Changed display zone */
#define WIN_DIALLING_CB				(ZONE_ICONS|ZONE_SOFTKEYS|ZONE_FULL_WIDTH|ZONE_MIDDLETWOLINES_HEIGHT)
#define WIN_DIALLING_SMS			(ZONE_ICONS|ZONE_SOFTKEYS|ZONE_FULL_WIDTH|ZONE_BOTTOMHALF_HEIGHT)
/* xreddymn OMAPS00075852 May-15-2006: Changed display zone */
#define WIN_DIALLING_SAT			(ZONE_ICONS|ZONE_SOFTKEYS|ZONE_FULL_WIDTH|ZONE_MIDDLETWOLINES_HEIGHT)
#else /* NEW_EDITOR */
#define WIN_DIALLING				(ZONE_ICONS|ZONE_SOFTKEYS|ZONE_MIDDLE_WIDTH|ZONE_MIDDLE_HEIGHT)
#define WIN_DIALLING_CB				(ZONE_ICONS|ZONE_SOFTKEYS|ZONE_MIDDLE_WIDTH|ZONE_MIDDLE_HEIGHT)
#define WIN_DIALLING_SMS			(ZONE_ICONS|ZONE_SOFTKEYS|ZONE_FULL_WIDTH|ZONE_BOTTOMHALF_HEIGHT)
#define WIN_DIALLING_SAT			(ZONE_ICONS|ZONE_SOFTKEYS|ZONE_FULL_WIDTH|ZONE_BOTTOMTWOLINES_HEIGHT)
#endif /* NEW_EDITOR */

#define PHNO_EDITOR					ZONE_FULL_SK_TITLE	

#define CENTRE_EDITOR				ZONE_FULL_SK_TITLE	

//And for MMIServices - moved here from mmiservices.c
#define SAT_EDITOR					ZONE_FULL_SK_TITLE	

#define RPWD_EDITOR					ZONE_FULL_SK_TITLE	

//MMIBookCallDetails
#define PHB_EDITOR 					ZONE_FULL_SK_TITLE	

//MMIPins
#define PIN_EDIT_RESET				ZONE_FULL_SK_TITLE

//MMITimeDate
#define TIMEDATE_EDITOR_AREA		ZONE_FULL_SK_TITLE	
//#define TIMEDATE_EDITOR_AREA 0,20,70,20

#endif

//For displaying call information
#define CALLINFO_X_AH		8
#define CALLINFO_X_TAGPOS 35
#define INCALLTIMER_X 24
//For autoredial (no chinese)
#define CALL_AUTOREDIAL_X 20
#define CALL_AUTOREDIAL_Y1 16
#define CALL_AUTOREDIAL_Y2 26
#define CALL_AUTOREDIAL_Y3 36
//Information
#define CALL_INFORMATION_X 24
#define CALL_INFORMATION_Y 16
//txt Please Wait
#define TXTMESSAGE_X 20
#define TXTMUTED_X 8


//From mmidynamenu
// default menu dimensions
#define MENU_LEFT	  0
#define MENU_INDENT  10
#define MENU_TOP	  0
#define MENU_RIGHT   96
#define MENU_BOTTOM  50

// scroll bar dimensions
#define SCROLL_BAR_LEFT 87
#define SMALL_RECT 91
#define ELEVAT_RECT (SMALL_RECT-2)

//MMIIcons
#define TXTCHARGE_X 16

//MMIIdle
//indent for text on idle screen
	#define IDLEMENUTXT1_X 16
	#define IDLEMENUTXT2_X TXTCHARGE_X
	

//MMIPins
#define INFO_TEXT_X     0
#define INFO_TEXT_Y     6

	

extern MfwRect selected_area;

//MMIEditor
#define EDITOR_TOP_LEFT_POS_X   4
#define EDITOR_TOP_LEFT_POS_Y   (Mmi_layout_line_height())
#define EDITOR_HORIZONTAL_SIZE  (SCREEN_SIZE_X - EDITOR_TOP_LEFT_POS_X)
#define EDITOR_VERTICAL_SIZE    (SCREEN_SIZE_Y - 2*Mmi_layout_line_height()) 
#define EDITOR_COLOR            0
#define EDITOR_FONT             0
#define EDITOR_CONTROLS         0

//MMIGame - connect 4
#ifndef LSCREEN
#define GAME_POS_X		0
#define GAME_POS_Y		8
#define GAME_SCALE_X	5
#define GAME_SCALE_Y	5
#define GAME_NAMEPOSX1 56
#define GAME_NAMEPOSY1 6
#define GAME_NAMEPOSX2 56
#define GAME_NAMEPOSY2 (6+21)
#define GAME_NAMEPOSX3 56
#define GAME_NAMEPOSY3 (6+21)
#define GAME_WINPOSX1 56
#define GAME_WINPOSY1 6
#define GAME_WINPOSX2 56
#define GAME_WINPOSY2 (6+21)

#else
#define GAME_POS_X		((SCREEN_SIZE_X / 2) - (MAX_X_BOARD*GAME_SCALE_X/2))
#define GAME_POS_Y		(27+15)
#define GAME_SCALE_X	13
#define GAME_SCALE_Y	15

#define GAME_NAMEPOSX1 (SCREEN_SIZE_X/2-36)
#define GAME_NAMEPOSX2 (SCREEN_SIZE_X/2-12)
#define GAME_NAMEPOSX3 (SCREEN_SIZE_X/2+12)
#define GAME_NAMEPOSY1  (0)
#define GAME_NAMEPOSY2  (0)
#define GAME_NAMEPOSY3  (0)
#define GAME_WINPOSX1 (SCREEN_SIZE_X/2-12)
#define GAME_WINPOSX2 (SCREEN_SIZE_X/2+12)
#define GAME_WINPOSY1 	(0)
#define GAME_WINPOSY2 	(0)


#endif

//#ifndef COLOURDISPLAY
#define WELCOME_ANI_POS_X (SCREEN_SIZE_X  - 56 )/2
#define WELCOME_ANI_POS_Y (SCREEN_SIZE_Y - (42+4))/2
//#else
//#define WELCOME_ANI_POS_X ((SCREEN_SIZE_X  - 240 )/2)
//#define WELCOME_ANI_POS_Y ((SCREEN_SIZE_Y - 106)/3) //top 1/3 of screen
//#endif
//Definitions for 'pop-up' windows with 1/2 lines of text
#define DLG_WINDOW1_LINE 10,20,60,30, 0
#define DLG_WINDOW2_LINE 10,20,60,45, 0

/*NDH - 13/01/04 - CQ 16753 - Redefine these values in accordance with GSM 03.40, para 9.2.3.12.1 */
/*API - 28/07/03 - CQ10203 - Add these defines for the Validity Period of an SMS*/
#define VP_REL_1_HOUR 		11
#define VP_REL_12_HOURS	143
#define VP_REL_24_HOURS	167
#define VP_REL_1_WEEK		173
#define VP_REL_MAXIMUM		255    //  May 10, 2004    REF: CRR 15753  Deepa M.D 
/*API - CQ10203 END*/
/* NDH - CQ16753 - END */

extern MfwRect menuArea;
extern MfwRect menuNamesArea;
extern MfwRect menuListArea;
extern MfwRect editor_menuArea;
extern MfwRect network_menuArea;
extern MfwRect sat_setup_menuArea;
extern MfwRect sat_select_menuArea;
extern MfwRect smscb_M_CBArea;
extern MfwRect smscb_M_CB_ACTArea;
extern MfwRect smscb_M_CB_TOPArea;
extern MfwRect smscb_M_CB_TOP_CONSArea;
extern MfwRect smscb_M_CB_TOP_CONS_SELArea;
extern MfwRect smscb_CBArea;
extern MfwRect smscb_CB_OPTArea;
extern MfwRect readSMS_menuArea;

#ifdef FF_MMI_FILEMANAGER
extern MfwRect FileType_menuArea;
#endif

extern MfwRect SmsRead_R_OPTArea;
extern MfwRect SmsSend_R_OPTArea;
extern MfwRect melody_menuArea;
extern MfwRect MAINICONarea;
extern MfwRect MAINICONareaColour;	//For the larger colour main icons



/********************************/



/* Define a simple get text routine to fetch a pointer
   to a textual resource given a resource identifier.

   The resource identifiers are given in the MmiBlkResources.h
   module header
*/


char *MmiRsrcGetText( int Id );

void MmiRsrcSetLang(int lang );/*MC SPR 1150 */
void Mmi_Set2English( void );
void Mmi_Set2German( void );
void Mmi_Set2Chinese( void );
#ifdef LANG_CHANGE_TEST
void Mmi_Set2Martian( void );/*MC SPR 1150 */
#endif
void Mmi_Set2Language(int lang);
UBYTE Mmi_getCurrentLanguage(void);

UBYTE Mmi_layout_line(int lineNo);	//GW 
UBYTE Mmi_getCurrentLanguage(void); //GW
//The following are redundant - maintain for compatibility only
#define Mmi_layout_first_line(	)	Mmi_layout_line(1)
#define Mmi_layout_second_line(	)	Mmi_layout_line(2)
#define Mmi_layout_third_line(	)	Mmi_layout_line(3)
#define Mmi_layout_fourth_line(	)	Mmi_layout_line(4)
#define Mmi_layout_fifth_line(	)	Mmi_layout_line(5)

UBYTE Mmi_number_of_lines_without_icons_on_top(void);
UBYTE Mmi_number_of_lines_with_icons_on_top(void);

UBYTE Mmi_layout_softkeyHeight( void );
UBYTE Mmi_layout_line_height(void);
UBYTE Mmi_layout_softkeyArea( MfwRect* rect );
UBYTE Mmi_layout_TitleHeight( void );
UBYTE Mmi_layout_IconHeight( void );
UBYTE Mmi_layout_line_icons(int lineNo);


void Mmi_layout_areas(void);

void Mmi_set_area_attr(MfwRect* area,U16 px,U16 py,U16 sx,U16 sy);
void resources_destroy (T_MFW_HND own_window);
BOOL resources_reset_language(char* string);
typedef struct {
	U16 id;
	U16 count;
	U32 attributes;
} T_RESOURCE_DIRECTORY_HEADER;

// This type identifies the various resources that are listable in the file. 
typedef enum
{
	RES_FIRST_RESOURCE_TYPE=0,
	RES_LANGUAGE_TABLE, 
	RES_LANGUAGE, 
	RES_STRING,
	RES_MELODY_TABLE,
	RES_MELODY,
	RES_LAST_RESOURCE_TYPE
} res_ResourceID_type;

typedef const T_RESOURCE_DIRECTORY_HEADER * RESOURCE_DIRECTORY_HEADER;

typedef unsigned char ST_DIMENSION ;

/*MC SPR 1111*/
typedef struct /*Entry in table of country codes*/
{
	char* country_dialling_code; /*Dialling code(4 digits)*/
	UBYTE 	language;			/*language (e.g. ENGLISH_LANGUAGE defined above)*/
}CountryLangEntry;

typedef struct /*Table of country codes and associated languages*/
{
	UBYTE no_of_entries;
	CountryLangEntry* entries;
} CountryLanguageTable;

/*MC end*/
void resources_setColour(unsigned int colIndex);
void resources_setSKColour(unsigned int colIndex);
void resources_setHLColour(unsigned int colIndex);
void resources_setTitleColour(unsigned int colIndex);

void resources_restoreColour( void );
void resources_restoreMnuColour(void);

void resources_setColourMnuItem( int txtId);

int res_getTitleHeight( void );

void res_set_SIMToolkit_title_string(char* SAT_string);

int resources_truncate_to_screen_width(char* IpStr, int IpLen, char* OpStr, int OutLen,
	int ScreenWidth, BOOL unicode);

void resetFactoryColour ( void );
// API - 13-01-03 - 1550
void resetFactoryBackground( void );

/*API - 27/07/03 - CQ10203 - Function Definition for SMS VP reset*/
void resetSMSValidityPeriod( void );
/*API - CQ10203 - END*/

void colour_initial_colours (void);

/* Added to remove warnings Aug-11*/
EXTERN U32 dspl_SetBgdColour (U32 inColour);
EXTERN U32 dspl_SetFgdColour (U32 inColour);
EXTERN void icon_setMainBgdBitmap (int bmpId);
EXTERN void info_screen (T_MFW_HND win, USHORT TextId, USHORT TextId2,T_VOID_FUNC  Callback);
/* End - remove warning Aug -11 */

/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

#endif
