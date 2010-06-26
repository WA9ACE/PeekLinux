/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Resource Manager
 $File:		    MmiResources.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

   Simple wrapper for the MMI Block Resources

   This module provides a simple interface to the block
   resources module, giving access to a single language,
   which can be used to get strings by Id.

   This module is a hack to provide an interface to the
   resource manager which can be easily used by a single
   process. If an application needs to use the resource
   manager it should call it directly, initialising it's
   own context safe handlers.

   This module is designed to wrap the resource API
   functions in a manner that gives the easiest and
   fastest implementation. It is therefore not a
   reentrant module, however given it only performs
   read only actions this should not be a problem.
                        
********************************************************************************
 $History: MmiResources.c

	Sep 14, 2007	REF: OMAPS00145860  Adrian Salido
	Description:	FT - MMI: Wrong trace class of new_getDataColour
	Solution:		changed event traces to function traces because the information content of 
				this is low for non-MMI people and against TI coding convention.
				
	Nov 07, 2006 ER:OMAPS00070661 R.Prabakar(a0393213)
	R99 network compliancy : Implemented Language Notification and Provide Local Information (Language setting)
	SAT proactive commands and Language Selection SAT event
	
	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			a) Modified "MmiRsrcGetText" function to return the string from FFS for the current active line
			
 	March 01, 2005    REF: ENH 29313    Deepa M.D
	Isolating colour build related changes from MMI LITE flag.
 	The proper  Feature flag  COLORDISPLAY was used for all color 
   	related changes.

  	Oct 19, 2004 REF: CRR MMI-SPR-26002 xkundadu.
  	Issue description:  Golite booting problem.
  	Solution: If the FFS is not formatted, prevent writing into the FFS.


	Aug 25, 2004    REF: CRR 24904   Deepa M.D
	Bug:cleanup compile switches used for Go-lite
	Fix:COLOURDISPLAY compiler switch has been added to the functions which are 
	used only for the normal color build.


	Aug 25, 2004    REF: CRR 24297  Deepa M.D 
	Bug:Reduction of the size of the variable ColArray
	Fix:The size of the structure T_COLDATA has been reduced for the Go-lite version.
	For Go-lite only two colors are being used(Black&White), hence the members of the
	T_COLDATA structure can be reduced to U8.


	25/10/00			Original Condat(UK) BMI version.	
	03/10/02		Replaced most of the file with version on branch for issue 1150 (MC, SPR 1242) 
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

#include "prim.h"


#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_tim.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_cm.h"
#include "mfw_sat.h"
#include "mfw_phb.h"
#include "mfw_sms.h"
#include "mfw_ffs.h" /*MC, SPR 1111*/
//Include the following  ensure that the function definitions within it match the
//definitions in this file.
#include "mfw_mmi.h"
#include "dspl.h"

#include "ksd.h"
#include "psa.h"

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#endif

#include "MmiResources.h"
#include "MmiBlkLangDB.h"
#include "MmiBlkManager.h"
#include "MmiBlkResources.h"
#include "MmiBookShared.h"

#include "MmiDummy.h"
#include "MmiMmi.h"
#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"


#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiDialogs.h"
#include "Mmiicons.h"	//GW
#include "mfw_ffs.h"
// this is for traces
#include "cus_aci.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif



#include "MmiLists.h"
#include "mmiColours.h"

//#define COLOURDISPLAY
//#define TRACE_EVENT_COL TRACE_EVENT

#ifndef TRACE_EVENT_COL
#define TRACE_EVENT_COL
#endif

#ifndef LSCREEN
#define FIRST_LINE 6
#define SECOND_LINE 14
#define THIRD_LINE 24
#define FOURTH_LINE 32
#define FIFTH_LINE 40
#define SIXTH_LINE 48
#define LINE_HEIGHT 8
#define FIRST_LINE_CHINESE 12
#define SECOND_LINE_CHINESE 24
#define THIRD_LINE_CHINESE 36
#define FOURTH_LINE_CHINESE 48
#define LINE_HEIGHT_CHINESE 12

#define FIRST_LINE_ICON 6
#define SECOND_LINE_ICON 14
#define THIRD_LINE_ICON 20


#else
//#define LINE_HEIGHT 12
//Colour display - all lines are the same height
//move to header for the moment #define LINE_HEIGHT 12
#endif
//x33x-end*/

#ifndef LSCREEN
#define SK_HEIGHT_LATIN				(LINE_HEIGHT_LATIN)
#define SK_HEIGHT_CHINESE			(LINE_HEIGHT_CHINESE)
#define TITLE_HEIGHT_LATIN			(LINE_HEIGHT_LATIN)
#define TITLE_HEIGHT_CHINESE		(LINE_HEIGHT_CHINESE)

#define NLINES_LATIN				(SCREEN_SIZE_Y/LINE_HEIGHT_LATIN)	//6
#define NLINES_CHINESE				(SCREEN_SIZE_Y/LINE_HEIGHT_CHINESE)	//4
#define CHINESE_MENU_VERTICAL_SIZE 		((NLINES_CHINESE-1)*LINE_HEIGHT_CHINESE)
#define STANDARD_MENU_VERTICAL_SIZE ((NLINES_LATIN-1)*LINE_HEIGHT_LATIN)

#define ICON_HEIGHT					(6)
#else
#define SK_HEIGHT_LATIN				(LINE_HEIGHT+8)
#define SK_HEIGHT_CHINESE			(LINE_HEIGHT+8)
#define TITLE_HEIGHT_LATIN			(LINE_HEIGHT_LATIN+4)
#define TITLE_HEIGHT_CHINESE		(LINE_HEIGHT_CHINESE+4)
#define ALLOFFSET					(LINE_HEIGHT*2)
#define MNUOFFSET					(LINE_HEIGHT)

#define NLINES_LATIN				(SCREEN_SIZE_Y/LINE_HEIGHT_LATIN)	
#define NLINES_CHINESE				(SCREEN_SIZE_Y/LINE_HEIGHT_CHINESE)	
#define CHINESE_MENU_VERTICAL_SIZE 		((NLINES_CHINESE*LINE_HEIGHT_CHINESE)-SK_HEIGHT_CHINESE)
#define STANDARD_MENU_VERTICAL_SIZE ((NLINES_LATIN*LINE_HEIGHT_LATIN)-SK_HEIGHT_LATIN)

#define ICON_HEIGHT					(22)
#endif


/*******************************************************************************
                                                                              
                                Static data elements
                                                                              
*******************************************************************************/

/* Define static data elements to give the global
   handles. These are not re-entrant, but what the
   hell, there will only be one of these in the
   system
*/
static tBlkHandle BlkManager = NULL;
UBYTE currentLanguage		= NULL;
static tBlkHandle Language   = NULL;
static long int   ModuleKey  = -1;


/* Define a unique key value to determine if the constructor
   has been called. This is slightly dangerous as there is 
   a chance that the key value powers up in this state, however
   this is unlikely in a cold boot situation. (In a warm boot,
   if the RAM contents are preserved then the block resource
   manager will also be preserved, so there should be no
   problem there).
*/
#define KEYVALUE 0x00FADE00


/* We know that the resource manager deals with two languages
   at present, English and German, The first element in the
   lists will indicate which is which in a later version, however
   for now, we will just define constant settings for these.

   Note that the current MMI doesn't fully support German
   characters, but the resource manager does.
*/

#define SHOW_CONFIRM     3
int Easy_Text_Available = TRUE;

 char* SIMToolKitString=NULL;


/*SPR 1111, country code and langauge tables*/
/*MC, if English and Chinese Available*/
#ifdef CHINESE_MMI
static const CountryLangEntry LanguageTableEntries[7]=
{ 
	{"001", ENGLISH_LANGUAGE}, /*USA*/
	{"044", ENGLISH_LANGUAGE},	/*UK*/
	{"061", ENGLISH_LANGUAGE},	/*Australia*/
	{"353", ENGLISH_LANGUAGE},	/*Ireland*/
	{"064", ENGLISH_LANGUAGE},	/*New Zealand*/
	{"086", CHINESE_LANGUAGE}, /*PRChina*/
	{"065", CHINESE_LANGUAGE} /*Singapore*/
};

static const CountryLanguageTable LanguageTable =
{ 7, (CountryLangEntry*)LanguageTableEntries};
#else	/*MC, if English and German Available*/
static const CountryLangEntry LanguageTableEntries[7]=
{ 
	{"001", ENGLISH_LANGUAGE}, /*USA*/
	{"044", ENGLISH_LANGUAGE},	/*UK*/
	{"061", ENGLISH_LANGUAGE},	/*Australia*/
	{"353", ENGLISH_LANGUAGE},	/*Ireland*/
	{"064", ENGLISH_LANGUAGE},	/*New Zealand*/
	{"043", GERMAN_LANGUAGE}, /*Austria*/
	{"049", GERMAN_LANGUAGE} /*Germany*/
};

static const CountryLanguageTable LanguageTable =
{ 7, (CountryLangEntry*)LanguageTableEntries};


#endif

typedef struct 
{
	// cp_resource_file_t rf;
	RESOURCE_DIRECTORY_HEADER language_table;
	RESOURCE_DIRECTORY_HEADER melody_table;
	RESOURCE_DIRECTORY_HEADER glyph_table;
	RESOURCE_DIRECTORY_HEADER zone_table;

	UBYTE langid;		// current language_table
	UBYTE fontid;		// current font
	ST_DIMENSION    fontHeight;	// current font height

}  res_ResMgr_type;




typedef struct
{
  T_MMI_CONTROL    mmi_control;
  T_MFW_HND        resources_win;        /* MFW win handler      */
} T_resources;

T_resources resources_data;

enum {
	COL_TYPE_SCREEN=0,
	COL_TYPE_SK,
	COL_TYPE_HL,
	COL_TYPE_TITLE,
	COL_TYPE_MAX
};
/***************************Go-lite Optimization changes Start***********************/
//Aug 25, 2004    REF: CRR 24297  Deepa M.D 
//T_COLDATA structure for the Go-lite has been reduced in size.
//March 01, 2005    REF: ENH 29313    Deepa M.D
// Isolating colour build related changes from MMI LITE flag.
// The proper  Feature flag  COLORDISPLAY was used for all color 
//related changes.
#ifdef COLOURDISPLAY 
typedef struct {
	U32 fgd;
	U32 bgd;
} T_COLDATA;
#else
typedef struct {
	U8 fgd;
	U8  bgd;
} T_COLDATA;
#endif
/***************************Go-lite Optimization changes end***********************/
const char * colourFileDir = "/mmi";
const char * colourFileName = "MmiColour";

static T_COLDATA colArray[LAST_COLOUR_DEF][COL_TYPE_MAX];

#define FGD_COL 1
#define BGD_COL 2
#define NO_COL 3

#ifndef LSCREEN
#define NLINES 4
#define NLINES_MAX 5
#else
#define NLINES ((SCREEN_SIZE_Y-SK_HEIGHT_LATIN)/LINE_HEIGHT)
#define NLINES_MAX ((SCREEN_SIZE_Y-SK_HEIGHT_LATIN)/LINE_HEIGHT)
#endif

/*Data should be reset */
MfwRect menuArea 							= {0,0,10,10};
MfwRect menuNamesArea	 					= {0,0,10,10};
MfwRect menuListArea 						= {0,0,10,10};
MfwRect editor_menuArea						= {0,0,10,10};		/* menus display area      */
MfwRect network_menuArea 					= {0,0,10,10};			/* menus display area      */
MfwRect sat_setup_menuArea					= {0,0,10,10};			/* menus display area      */
MfwRect sat_select_menuArea 				= {0,0,10,10};			/* menus display area      */
MfwRect smscb_M_CBArea 						= {0,0,10,10}; /* menus display area      */
MfwRect smscb_M_CB_ACTArea					= {0,0,10,10}; /* menus display area      */
MfwRect smscb_M_CB_TOPArea 					= {0,0,10,10};/* menus display area      */
MfwRect smscb_M_CB_TOP_CONSArea				= {0,0,10,10}; /* menus display area      */
MfwRect smscb_M_CB_TOP_CONS_SELArea			= {0,0,10,10}; /* menus display area      */
MfwRect smscb_CBArea						= {0,0,10,10}; /* menus display area      */
MfwRect smscb_CB_OPTArea					= {0,0,10,10}; /* menus display area      */
MfwRect readSMS_menuArea					= {0,0,10,10}; /* menus display area      */
#ifdef FF_MMI_FILEMANAGER
MfwRect FileType_menuArea					= {0,0,10,10}; /* menus display area      */  // RAVI - 19-05-2005
#endif
MfwRect SmsRead_R_OPTArea					= {0,0,10,10};/* menus display area      */
MfwRect SmsSend_R_OPTArea					= {0,0,10,10}; /* menus display area      */
MfwRect melody_menuArea 					= {0,0,10,10}; /* menus display area      */
MfwRect MAINICONarea						= {0,0,10,10};
MfwRect MAINICONareaColour 					= {0,0,10,10};	//For the larger colour main icons

static int resources_win_cb (T_MFW_EVENT event, T_MFW_WIN * win);
static void resources_language_confirm(void);
void resources_dialog_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason);
void resources (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
T_MFW_HND resources_Init (T_MFW_HND parent_window);
void resources_Exit (T_MFW_HND own_window);
T_MFW_HND resources_create (T_MFW_HND parent_window);
void initDefaultDataColour (void);

// Extern Function ADDED - RAVI - 25-11-2005
EXTERN int flash_formatted(void);
EXTERN void idle_setBgdBitmap(int bmpId);
// END - RAVI



/*******************************************************************************
                                                                              
                                Private Routines
                                                                              
*******************************************************************************/

static void LanguageEnable( int newLanguage )
{
    /* Initialise if required
    */
	currentLanguage = newLanguage;
	if ( ModuleKey != KEYVALUE )
	{
		/* Nope, then go do it, If the mmibm_* routines fail to initialise
		   they will return NULL, and they both deal with bad input values
		   cleanly, so we don't need error checking here.
		*/
        BlkManager = mmibm_Initialise( Mmir_BaseAddress(), Mmir_NumberOfEntries() );
		ModuleKey  = KEYVALUE;
	}

    /* Set up the new language based on the incoming value
    */
    /*MC, SPR 1150, simplified for easier addition of new languages*/
    if (newLanguage> 0 && newLanguage <= NO_OF_LANGUAGES)
    	Language   = mmibm_SupplyResourceHandler( BlkManager, newLanguage );
}


/*******************************************************************************
                                                                              
                                Public Routines
                                                                              
*******************************************************************************/

/*******************************************************************************
 $Function:    	MmiRsrcGetText

 $Description:	Define access routine for the actual get text
                routine. In order to make sure the routine can
				be invoked and will automatically start, we will
				use the key value above to ensure we detect the
				startup conditions.
				
				The calling routine for this function should check
				for NULL values which will be returned if the 

 $Returns:		Pointer to resource indicated by Id (Or NULL)

 $Arguments:	Id of requested resource
 
*******************************************************************************/

char *MmiRsrcGetText( int Id )
{
	/* Check if we have already initialised
	*/
	if ( ModuleKey != KEYVALUE )
        LanguageEnable( ENGLISH_LANGUAGE );

    /* JVJ #1576 SAT is an exception, since the Menu string is variable depending on the SIM*/
    /* but the Languages table is constant. Therefore we need this workaround for the Mainmenu*/
    /* to display the SAT header */
    if ((Id == TxtSimToolkit)&&(SIMToolKitString))
        return (char*)SIMToolKitString;

//x0pleela 23 May, 2006  DR: OMAPS00070657
#ifdef FF_CPHS
    if( (Id==TxtLine1) && strlen(FFS_flashData.line1))
	return (char*)FFS_flashData.line1;

    else if( (Id==TxtLine2) && strlen(FFS_flashData.line2))
	return (char*)FFS_flashData.line2;
#endif
    /* use the currently set up resource handler for the English
	   language to get the text associaed with the Id, again if
	   there has been a problem with the initialisation of the
	   English handler, then the fetch routine will deal with it
	   cleanly and return NULL.
	*/
	return (char *) mmibr_Fetch( Language, Id );
}

/*MC, SPR 1150 new function, don't need to write a new function for each new langauge now*/
void MmiRsrcSetLang( int lang )
{	if (lang == CHINESE_LANGUAGE)
		dspl_set_char_type(DSPL_TYPE_UNICODE);
	else
		dspl_set_char_type(DSPL_TYPE_ASCII);
    LanguageEnable( lang );
	Mmi_layout_areas();
	FFS_flashData.language = lang;

//  Oct 19, 2004 REF: CRR MMI-SPR-26002 xkundadu 
//  Issue description:  Golite booting problem.
//  Solution: If the FFS is not formatted, prevent writing into the FFS.
	if(flash_formatted() == TRUE)
	{
		flash_write();
	}


}
/*MC SPR 1150, Simplified menu interface functions.  
It should be simpler to add new functions for new languages
We still need to write individual functions for each language to interface with the menus as
the menu architecture does not allow passing parmeters*/
/*OMAPS00070661 (SAT-Lang selection) a0393213(R.Prabakar) language selection sent to mfw*/
void Mmi_Set2English( void )
{
  TRACE_EVENT("English Language Selected");    
  Mmi_Set2Language(ENGLISH_LANGUAGE); 
  #ifdef FF_MMI_R99_SAT_LANG
  satLanguageSelected (ENGLISH_LANGUAGE);
  #endif
}

void Mmi_Set2German( void )
{	
   TRACE_EVENT("German Language Selected");
   Mmi_Set2Language(GERMAN_LANGUAGE); 
   #ifdef FF_MMI_R99_SAT_LANG 
   satLanguageSelected (GERMAN_LANGUAGE);
   #endif
}

void Mmi_Set2Chinese( void )
{
  TRACE_EVENT("Chinese Language Selected");   
  Mmi_Set2Language(CHINESE_LANGUAGE); 
  #ifdef FF_MMI_R99_SAT_LANG
  satLanguageSelected (CHINESE_LANGUAGE);  
  #endif
}

#ifdef LANG_CHANGE_TEST
void Mmi_Set2Martian( void )
{
 TRACE_EVENT("Martian Language Selected");   
  Mmi_Set2Language(MARTIAN_LANGUAGE); 
 #ifdef FF_MMI_R99_SAT_LANG
 satLanguageSelected (MARTIAN_LANGUAGE);    
 #endif
}
#endif

/*MC SPR 1150,new generic langauge change function*/
void Mmi_Set2Language(int lang)
{
    T_MFW_HND win = mfw_parent(mfw_header());
    #ifndef FF_MMI_R99_SAT_LANG /*OMAPS00070661(SAT-lang notify) a0393213(R.Prabakar)*/
		resources_Init(win);
		MmiRsrcSetLang(lang);
		SEND_EVENT (resources_data.resources_win, SHOW_CONFIRM, 0, 0);
    #else	
	      /*OMAPS00070661 The sequence is changed to prevent the crash when the language is changed in quick succession*/
		MmiRsrcSetLang(lang);
		if(!resources_data.resources_win)
		{
		resources_Init(win);
		SEND_EVENT (resources_data.resources_win, SHOW_CONFIRM, 0, 0); 
		}
     #endif

}

UBYTE Mmi_getCurrentLanguage(void)
{
	return currentLanguage;
}

//GW Created to return height of a line with just numbers on it
UBYTE numberLineHeight(void)
{
#ifndef LSCREEN
	//All numbers are 8 bits high (whether chinese or latin chars)
	return (8);
#else 
	//all text is 12 bits high
	return (12);
#endif
}
//GW Created
UBYTE Mmi_layout_line(int lineNo)
{
#ifndef LSCREEN
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
	{
		switch (lineNo)
		{
		case 1:		return FIRST_LINE_CHINESE;

		case SECOND_LAST_LINE_TOP:
		case 2:		return SECOND_LINE_CHINESE;

		case LAST_LINE_TOP:
		case 3:		return THIRD_LINE_CHINESE;

		case LAST_LINE:
		case 4:		return FOURTH_LINE_CHINESE;
		//We are now asking for an invalid line
		default:	return FOURTH_LINE_CHINESE;
		
		}
	}	
	else
	{
		switch (lineNo)
		{
		case 1:		return FIRST_LINE;
		case 2:		return SECOND_LINE;
		case 3:		return THIRD_LINE;

		case SECOND_LAST_LINE_TOP:
		case 4:		return FOURTH_LINE;

		case LAST_LINE_TOP:		//y-coord of the top of last line
		case 5:		return FIFTH_LINE;

		case LAST_LINE:
		case 6:		return SIXTH_LINE;
		//We are now asking for an invalid line
		default:	return SIXTH_LINE;
		}
	}	
#else
	//assume both fonts are now the same height.
	switch (lineNo)
	{
		case SECOND_LAST_LINE_TOP:	return ( (UBYTE)(SCREEN_SIZE_Y-Mmi_layout_softkeyHeight()-LINE_HEIGHT));
		case LAST_LINE_TOP:			return ( (UBYTE)(SCREEN_SIZE_Y-Mmi_layout_softkeyHeight()));
		case LAST_LINE:				return ( (UBYTE) SCREEN_SIZE_Y);
		
		default:	return (lineNo*LINE_HEIGHT);
	}
#endif
}
UBYTE Mmi_layout_line_icons(int lineNo)
{
#ifndef LSCREEN
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
	{
		switch (lineNo)
		{
		case 1:		return FIRST_LINE_CHINESE;

		case SECOND_LAST_LINE_TOP:
		case 2:		return SECOND_LINE_CHINESE;

		case LAST_LINE_TOP:
		case 3:		return THIRD_LINE_CHINESE;

		case LAST_LINE:
		case 4:		return FOURTH_LINE_CHINESE;
		//We are now asking for an invalid line
		default:	return FOURTH_LINE_CHINESE;
		
		}
	}	
	else
	{
		switch (lineNo)
		{
		case 1:		//nobreak;
		case 2:		//nobreak;
		case 3:		//nobreak;
		default:	return (FIRST_LINE_ICON+(lineNo-1)*LINE_HEIGHT);

		case SECOND_LAST_LINE_TOP:
		case 4:		return FOURTH_LINE;

		case LAST_LINE_TOP:		//y-coord of the top of last line
		case 5:		return FIFTH_LINE;

		case LAST_LINE:
		case 6:		return SIXTH_LINE;
		}
		
	}
#else
		switch (lineNo)
		{
		case SECOND_LAST_LINE_TOP:		return((UBYTE)(SCREEN_SIZE_Y-(Mmi_layout_softkeyHeight()+Mmi_layout_line_height())));
		case LAST_LINE_TOP:				return((UBYTE)(SCREEN_SIZE_Y-(Mmi_layout_softkeyHeight())));
		case LAST_LINE:					return((UBYTE)(SCREEN_SIZE_Y));
		default:						return(Mmi_layout_IconHeight()+Mmi_layout_line_height()*lineNo);
		
		}
#endif
}

UBYTE Mmi_layout_line_height(void)
{
#ifndef LSCREEN
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
		return LINE_HEIGHT_CHINESE;
	else
		return LINE_HEIGHT;
#else
	return (LINE_HEIGHT);
#endif
}

UBYTE Mmi_layout_softkeyHeight( void )
{
#ifndef LSCREEN
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
		return SK_HEIGHT_CHINESE;
	else
		return SK_HEIGHT_LATIN;
#else
	return (SK_HEIGHT_LATIN);
#endif
}
UBYTE Mmi_layout_softkeyArea( MfwRect* rect )
{
	if (!rect)
		return (0);
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
		rect->sy = SK_HEIGHT_CHINESE;
	else
		rect->sy = SK_HEIGHT_LATIN;
	rect->px = 0;
	rect->sx = SCREEN_SIZE_X;
	rect->py = SCREEN_SIZE_Y-rect->sy;
	return(1);
}
UBYTE Mmi_layout_TitleHeight( void )
{
#ifndef LSCREEN
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
		return TITLE_HEIGHT_CHINESE;
	else
		return TITLE_HEIGHT_LATIN;
#else
	return (TITLE_HEIGHT_LATIN);
#endif
}
UBYTE Mmi_layout_IconHeight( void )
{
	return (ICON_HEIGHT);
}

UBYTE Mmi_number_of_lines_with_icons_on_top(void)
{
#ifndef LSCREEN
{
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
		return 2;
	else
		return 4;
}
#else 
	return NLINES_MAX-1;
#endif
}


UBYTE Mmi_number_of_lines_without_icons_on_top(void)
{
#ifndef LSCREEN
{
	if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
		return 3;
	else
		return 5;
}
#else //assume a single font of 12 pixels high 10 lines x 12 pixels
	return NLINES_MAX;
#endif
}

//GW 10/02/03	Modified code - icon position is now sorted out by 'iconsMainIconsAreaSet' - the code 
//				here only supplies the available screen size
void Mmi_layout_areas(void)
{
	MAINICONarea.px = 0;
	MAINICONarea.py =Mmi_layout_line_height();

	MAINICONarea.sx = SCREEN_SIZE_X;
	MAINICONarea.sy =SCREEN_SIZE_Y-Mmi_layout_line_height()*2-1;

	Mmi_set_area_attr(&menuArea,						0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&menuNamesArea,					0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-LINE_HEIGHT-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&menuListArea,					0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&editor_menuArea,Mmi_layout_line(1),0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&network_menuArea,				0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-/*Mmi_layout_IconHeight()-*/Mmi_layout_softkeyHeight()); /* SPR#2354 - SH - icons aren't shown */
	Mmi_set_area_attr(&sat_setup_menuArea,				0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&sat_select_menuArea,				0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&smscb_M_CBArea,					0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&smscb_M_CB_ACTArea,				0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&smscb_M_CB_TOPArea,				0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight()); 
	Mmi_set_area_attr(&smscb_M_CB_TOP_CONSArea,			0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&smscb_M_CB_TOP_CONS_SELArea,		0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&smscb_CBArea,					0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight()); 
	Mmi_set_area_attr(&smscb_CB_OPTArea,				0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&readSMS_menuArea,				0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
#ifdef FF_MMI_FILEMANAGER
	Mmi_set_area_attr(&FileType_menuArea,				0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());	 // RAVI - 27-05-2005
#endif
	Mmi_set_area_attr(&SmsRead_R_OPTArea,				0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&SmsSend_R_OPTArea,				0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	Mmi_set_area_attr(&melody_menuArea,					0,0,SCREEN_SIZE_X,SCREEN_SIZE_Y-Mmi_layout_softkeyHeight());
	iconsMainIconsAreaSet (MAINICONarea);

}

void Mmi_set_area_attr(MfwRect* area,U16 px,U16 py,U16 sx,U16 sy)
{

	/* pass in top-left/bottom-right coordinates*/
	area->px = px;
	area->py = py;
	area->sx = sx-px;
	area->sy = sy-py;
}

/*******************************************************************************

 $Function:    	resources_Init

 $Description:	
 
 $Returns:		window

 $Arguments:	parent window
 
*******************************************************************************/
T_MFW_HND  resources_Init(T_MFW_HND parent_window)
{
  return (resources_create (parent_window));
}

// API - 1550 - 13-01-03 - Add this function to reset the backgrounds of Idle and Main Menus.
void resetFactoryBackground ( void )
{
	idle_setBgdBitmap(BGD_TI_LOGO);
	FFS_flashData.IdleScreenBgd = BGD_TI_LOGO;

	icon_setMainBgdBitmap(BGD_SQUARE);
	FFS_flashData.MainMenuBgd = BGD_SQUARE;

	FFS_flashData.ProviderNetworkShow = TRUE;
}

void resetFactoryColour ( void )
{
  	initDefaultDataColour();
  	if(flash_data_write(colourFileDir, colourFileName, colArray, sizeof(colArray)) >= EFFS_OK)
  	{
  		TRACE_EVENT("File Created in FFS");
  	}
  	else
  	{
  		TRACE_EVENT("File Not Created in FFS");  
  	}
	
}
void colour_initial_colours (void)
{
/* API/GW - Added to read the flash structure to see if the file structure to save the colour data in has been created
   			 and if so save the initialised data to flash or load the saved values out of flash
*/
  if (flash_data_read(colourFileDir, colourFileName, colArray, sizeof(colArray)) == sizeof(colArray))
    /* Marcus: Issue 1719: 11/02/2003: Altered comparison in condition */
  {
  	TRACE_EVENT("Correctly Read in!");
  }
  else
  {
  	resetFactoryColour();
  }
}
/*******************************************************************************

 $Function:    	resources_Exit

 $Description:	exit network handling (Backward Compatibility interface)
 
 $Returns:		none

 $Arguments:	window
 
*******************************************************************************/
void resources_Exit (T_MFW_HND own_window)
{

    T_MFW_WIN   * win_data = ((T_MFW_HDR *)own_window)->data;
    T_resources * data     = (T_resources *)win_data->user;

  resources_destroy (data->resources_win);
}




/*******************************************************************************

 $Function:    	resources_create

 $Description:	create network top window
 
 $Returns:		window

 $Arguments:	parent_window
 
*******************************************************************************/
T_MFW_HND resources_create (T_MFW_HND parent_window)
{
  T_resources * data = &resources_data;
  T_MFW_WIN   * win;

  TRACE_FUNCTION ("resources_create()");

  data->resources_win = win_create (parent_window, 0, 0, (T_MFW_CB)resources_win_cb);

  if (data->resources_win EQ 0)
    return 0;

  /*
   * Create window handler
   */
  data->mmi_control.dialog = (T_DIALOG_FUNC)resources;
  data->mmi_control.data   = data;
  win                      = ((T_MFW_HDR *)data->resources_win)->data;
  win->user                = (void *) data;

 
  winShow(data->resources_win);
  /*
   * return window handle
   */
  return data->resources_win;
}



/*******************************************************************************

 $Function:    	resources_destroy

 $Description:	
 
 $Returns:		none

 $Arguments:	window
 
*******************************************************************************/
void resources_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN   * win;
  T_resources * data;

  TRACE_FUNCTION ("resources_destroy()");

  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_resources *)win->user;

    if (data)
    {

      /*
       * Delete WIN handler
       */
      win_delete (data->resources_win);
      data->resources_win = 0;
    }
  }
}


/*******************************************************************************

 $Function:    	resources_win_cb

 $Description:	network top window
 
 $Returns:		status int

 $Arguments:	event, window
 
*******************************************************************************/
static int resources_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
  /*
   * Top Window has no output
   */
  return 1;
}





/*******************************************************************************

 $Function:    	resources_language_confirm

 $Description:	
 
 $Returns:		void

 $Arguments:	void
 
*******************************************************************************/
static void resources_language_confirm(void)
{
  T_DISPLAY_DATA display_info;

  TRACE_FUNCTION ("resources_language_confirm()");

	dlg_initDisplayData_TextId( &display_info, TxtNull, TxtNull, TxtDone, TxtNull , COLOUR_STATUS_PINS);
	dlg_initDisplayData_events( &display_info, (T_VOID_FUNC)resources_dialog_cb, THREE_SECS, KEY_CLEAR | KEY_RIGHT );
	
  /*
   * Call Info Screen
   */
  info_dialog (resources_data.resources_win, &display_info);

}

/*******************************************************************************

 $Function:    	resources_dialog_cb

 $Description:	
 
 $Returns:		void

 $Arguments:	window, identifier, reason
 
*******************************************************************************/
void resources_dialog_cb(T_MFW_HND win, UBYTE identifier, UBYTE reason)
{
	TRACE_FUNCTION ("resources_dialog_cb()");

      switch (reason) 
      {
        case INFO_TIMEOUT:
          /* no break; */
        case INFO_KCD_HUP:
          /* no break; */
        case INFO_KCD_LEFT:
          /* no break; */
        case INFO_KCD_RIGHT:
            resources_Exit(win);
          break;
      }
}



/*******************************************************************************

 $Function:    	resources

 $Description:	
 
 $Returns:		void

 $Arguments:	window, event, value, parameter
 
*******************************************************************************/
void resources (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
 // T_MFW_WIN         * win_data = ((T_MFW_HDR *) win)->data;  // RAVI
 // T_resources       * data     = (T_resources *)win_data->user;  // RAVI

 TRACE_FUNCTION("resources()");

 switch (event)
  {
    case SHOW_CONFIRM:
		resources_language_confirm();
    break;
  }

}
/*MC added for SPR 1111*/
/*******************************************************************************

 $Function:    	resources_reset_language

 $Description:	Reset language using Pseudo-SS string
 
 $Returns:		TRUE if reset successful, FALSE otherwise

 $Arguments:	Pseudo-SS string
 
*******************************************************************************/
BOOL resources_reset_language(char* string)
{
	char country_code[4];
	int i;
	/*extract country code from input string*/
	memcpy(country_code, &string[3], 3);
	country_code[3] = NULL;

	/*for each entry in Language table*/	
	for(i=0; i<LanguageTable.no_of_entries; i++)
	{		/*if country matches that of entry*/
		
	  	if (!strcmp(country_code, LanguageTable.entries[i].country_dialling_code))
	  	{	/*MC SPR 1150, check language fits in allowed range, rather than checking for each 
	  		and every language (easier to add more languages this way)*/
	  		if (LanguageTable.entries[i].language>0 && LanguageTable.entries[i].language <= NO_OF_LANGUAGES)
	  		{	MmiRsrcSetLang(LanguageTable.entries[i].language);
				return TRUE;
	  		}
  		}
  	}
  	/*if no matches found*/
  	return FALSE; 		
}

/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History
  GW 28/11/02 - 

*******************************************************************************/
#define NO_COLOUR_DEFINED (0x01010101)
/***************************Go-lite Optimization changes Start***********************/
//Aug 25, 2004    REF: CRR 24297  Deepa M.D 
//This function is called only for Color build.
#ifdef COLOURDISPLAY
void init_getDataColour(int screenType, int colType, U32 *fgd, U32 *bgd)
{
	U32 fgColor;
	U32 bgColor;
	U32 fgHLColor = NO_COLOUR_DEFINED;
	U32 bgHLColor = NO_COLOUR_DEFINED;
	U32 fgSKColor = NO_COLOUR_DEFINED;
	U32 bgSKColor = NO_COLOUR_DEFINED;
	U32 fgTColor = NO_COLOUR_DEFINED;
	U32 bgTColor = NO_COLOUR_DEFINED;

	TRACE_EVENT("init_getDataColour");

	switch (screenType)
	{
	case COLOUR_STATUS_WELCOME:	//start screen
	case COLOUR_STATUS_GOODBYE:	//end screen
		fgColor = 		COL_Black; 
		bgColor = 		COL_White; 
		fgSKColor = 	COL_Blue;
		bgSKColor = 	COL_Yellow;
		break;
		
	/* GW#2294 Use a more legible colour as default background colour.*/
	case COLOUR_POPUP:
	case COLOUR_STATUS:	//generic status window
		fgColor = 		COL_Red; //0x0000FF00;
		bgColor = 		COL_Yellow;//0x00FFFFFF;
		fgSKColor = 	COL_Red;
		bgSKColor = 	COL_TRANSPARENT;
		break;
	
	case COLOUR_EDITOR:
		fgTColor  = 	COL_White;
		bgTColor  = 	COL_Blue;
		fgColor = 		COL_Blue; // 0x00FFFF00;
		bgColor = 		COL_Yellow;//COL_White;//0x00FFFFFF;
		fgSKColor = 	COL_Blue;
		bgSKColor = 	COL_PaleYellow;	//0x00FFFF00;
		break;

	case COLOUR_LIST_MAIN:
		fgTColor  = 	COL_White;
		bgTColor  = 	COL_BLK;
		fgColor = 		COL_Yellow ; 
		bgColor = 		COL_Blue;
		fgColor = 		COL_Black ; 
		bgColor = 		COL_White;
		fgSKColor = 	COL_W;
		bgSKColor = 	COL_BLK;	//0x00FFFF00;
		bgHLColor = 	COL_MidGrey;	
		break;
		
	case COLOUR_LIST_SUBMENU:
	case COLOUR_LIST_SUBMENU1:
	case COLOUR_LIST_SUBMENU2:
	case COLOUR_LIST_SUBMENU3:
	case COLOUR_LIST_SUBMENU4:
		fgTColor  = 	COL_Blue;
		bgTColor  = 	COL_PaleYellow;
		fgColor = 		COL_Black ; 
		bgColor = 		COL_White;
		bgHLColor = 	COL_MidGrey;	
		fgSKColor = 	COL_Blue;
		bgSKColor = 	COL_PaleYellow;	//0x00FFFF00;
		break;

	case COLOUR_LIST_COLOUR:
		fgColor 	=	COL_RG2; 
		bgColor 	=	COL_W2;
		fgSKColor 	=	COL_W; 
		bgSKColor 	=	COL_B; 
		fgTColor  	=	COL_RG;
		bgTColor  	=	COL_B;
		fgHLColor 	=	dspl_GetBgdColour();	
		bgHLColor 	=	dspl_GetFgdColour();	
		break;
		
	//Colours for specific displays
	case COLOUR_GAME:
		fgColor = 	COL_BLK ; 
		bgColor = 	COL_W;
		fgSKColor = COL_BLK ; 
		bgSKColor = COL_Orange;
		fgHLColor = COL_B;	
		bgHLColor = COL_W;	
		fgTColor  = COL_RB;
		bgTColor  = COL_G;
		break;
		
	case COLOUR_IDLE:
		fgColor = 		COL_BLK;//Green ; 
		bgColor = 		COL_TRANSPARENT;//White ;
		fgSKColor = 	COL_Blue; 
		bgSKColor =		COL_TRANSPARENT;
		//Colours are for the icons
		fgTColor  = 	COL_RB;
		bgTColor  = 	COL_G;
		break;
		
	case COLOUR_INCALL:
		fgColor = 	COL_RG ; 
		bgColor = 	COL_BLK;
		fgSKColor = COL_B ; 	/* SPR#1699 - SH - Modified so softkeys visible */
		bgSKColor = COL_TRANSPARENT;
		fgHLColor = COL_B;	
		bgHLColor = COL_W;	
		fgTColor  = COL_RB;
		bgTColor  = COL_G;
		break;

				
	default:
		fgColor = 	0x002020FF;
		bgColor = 	COL_GR;//0x00FFFFFF;
		fgSKColor = 	0x0000FF01;
		bgSKColor = 	COL_GR;//0x00FF0000;
		break;
	}
	
	/*If we have no preference for the other colours - set to default*/
	if (fgHLColor == NO_COLOUR_DEFINED) 
		fgHLColor = bgColor;
	if (bgHLColor == NO_COLOUR_DEFINED) 
		bgHLColor = fgColor;
	
	if (fgSKColor == NO_COLOUR_DEFINED) 
		fgSKColor = fgColor;
	if (bgSKColor == NO_COLOUR_DEFINED) 
		bgSKColor = bgColor;
	if (fgTColor == NO_COLOUR_DEFINED) 
		fgTColor = fgColor;
	if (bgTColor == NO_COLOUR_DEFINED) 
		bgTColor = bgColor;

	switch (colType)
	{
	case	COL_TYPE_SCREEN:
	default:
			*fgd 	= fgColor;
			*bgd 	= bgColor;
			break;
	case	COL_TYPE_SK:
			*fgd 	= fgSKColor;
			*bgd 	= bgSKColor;
			break;
	case	COL_TYPE_HL:
			*fgd 	= fgHLColor;
			*bgd 	= bgHLColor;
			break;
	case	COL_TYPE_TITLE:
			*fgd 	= fgTColor;
			*bgd 	= bgTColor;
			break;
	
	}
	if (*fgd == *bgd)
	{
		if (*fgd != COL_B)
			*fgd = COL_B;
	}
}
#endif
/***************************Go-lite Optimization changes end***********************/
//This array will be used to store all the colour info for each colour type. 
//This will allow us to let the user change the colour selected
/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History

*******************************************************************************/

/***************************Go-lite Optimization changes Start***********************/
//Aug 25, 2004    REF: CRR 24904   Deepa M.D
//This function is used only for the Color build.Hence it is put under the 
//COLOURDISPLAY compiler switch
#ifdef COLOURDISPLAY
void setDataColour(int screenType, int colType, U32 fgdORbgd, U32 newColour)
{

	TRACE_EVENT("setDataColour");
	
	if ((newColour == NO_COLOUR_DEFINED) || (fgdORbgd ==NO_COL))
		return;
	if ((screenType < LAST_COLOUR_DEF) &&
		(screenType >=0) &&
		(colType < LAST_COLOUR_DEF) &&
		(colType >=0))
	{
		if (fgdORbgd == FGD_COL)
		{
			//Do not allow transparent foreground colours
			if ((newColour & 0xFF000000) != 0xFF000000)
				colArray[screenType][colType].fgd = newColour;
		}
		else
		{
			colArray[screenType][colType].bgd = newColour;
		}
	}
}
#endif
/***************************Go-lite Optimization changes Start***********************/


/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History

*******************************************************************************/
void initDefaultDataColour (void)
{

	int i,j;

		for (i=0; i<LAST_COLOUR_DEF;i++)
		{
			for (j=0; j<COL_TYPE_MAX;j++)
			{
				/***************************Go-lite Optimization changes Start***********************/
				//Aug 25, 2004    REF: CRR 24297  Deepa M.D 
				//For Go-lite build, do the initialization of the colArray with black and white.
				//March 01, 2005    REF: ENH 29313    Deepa M.D
				// Isolating colour build related changes from MMI LITE flag.
				// The proper  Feature flag  COLORDISPLAY was used for all color 
				//related changes.
#ifdef COLOURDISPLAY 
				init_getDataColour(i, j, &colArray[i][j].fgd, &colArray[i][j].bgd);	
#else
				colArray[i][j].fgd  = COL_Lite_Black;
				colArray[i][j].bgd = COL_Lite_White;
#endif
				/***************************Go-lite Optimization changes end***********************/
			}
		}
}

void getDataColour(int screenType, int colType, U32 *fgd, U32 *bgd)
{

	
	TRACE_EVENT("getDataColour");
	
	if ((screenType == COLOUR_LIST_COLOUR) && (colType ==COL_TYPE_HL))
	{
	/* x0083025 on Sep 14, 2007 for OMAPS00145860 (adrian) */
	MMI_TRACE_EVENT("new_getDataColour IF");	

		*fgd	= dspl_GetBgdColour();	
		*bgd	= dspl_GetFgdColour();	
	}
	else if ((screenType < LAST_COLOUR_DEF) &&
		(screenType >=0) &&
		(colType < COL_TYPE_MAX) &&
		(colType >=0))
	{
		/* x0083025 on Sep 14, 2007 for OMAPS00145860 (adrian) */
		MMI_TRACE_EVENT("new_getDataColour ELSE IF");
		*fgd = colArray[screenType][colType].fgd;
		*bgd = colArray[screenType][colType].bgd;
		/***************************Go-lite Optimization changes Start***********************/
		//Aug 25, 2004    REF: CRR 24297  Deepa M.D 
		//Set the fgd and bgd color for Go-lite version based 
		//on the value stored in the colArray
		//March 01, 2005    REF: ENH 29313    Deepa M.D
		// Isolating colour build related changes from MMI LITE flag.
		// The proper  Feature flag  COLORDISPLAY was used for all color 
		//related changes.
#ifndef COLOURDISPLAY 
		if(*fgd==COL_Lite_Black)
			*fgd=COL_Black;
		else
			*fgd=COL_White;
		if(*bgd==COL_Lite_Black)
			*bgd=COL_Black;
		else
			*bgd=COL_White;
#endif		
		/***************************Go-lite Optimization changes end***********************/
	}
	else
	{
		/* x0083025 on Sep 14, 2007 for OMAPS00145860 (adrian) */
		MMI_TRACE_EVENT("new_getDataColour ELSE");
		*fgd = colArray[0][0].fgd;
		*bgd = colArray[0][0].bgd;	
	/***************************Go-lite Optimization changes Start***********************/	
		//Aug 25, 2004    REF: CRR 24297  Deepa M.D 
		//Set the fgd and bgd color for Go-lite version based
		//on the value stored in the colArray
		//March 01, 2005    REF: ENH 29313    Deepa M.D
		// Isolating colour build related changes from MMI LITE flag.
		// The proper  Feature flag  COLORDISPLAY was used for all color 
		//related changes.
#ifndef COLOURDISPLAY 
		if(*fgd==COL_Lite_Black)
			*fgd=COL_Black;
		else
			*fgd=COL_White;
		if(*bgd==COL_Lite_Black)
			*bgd=COL_Black;
		else
			*bgd=COL_White;
#endif
		/***************************Go-lite Optimization changes end***********************/
TRACE_EVENT_P1("fdg color is %d",*fgd);
		TRACE_EVENT_P1("bdg color is %d",*bgd);

	}
}

/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History

*******************************************************************************/
static U32 oldFgdCol,oldBgdCol;
static U32 oldmnuFgdCol,oldmnuBgdCol;
void resources_setColour(unsigned int colIndex)
{
	U32 fgdColour,bgdColour;
	getDataColour(colIndex, COL_TYPE_SCREEN, &fgdColour,  &bgdColour);
	oldFgdCol = dspl_SetFgdColour( fgdColour);
	oldBgdCol = dspl_SetBgdColour( bgdColour);
	if (colIndex==0xFFFF)
	{//disabled for the moment
		char str[50];
		sprintf(str,"Id=%d (%x,%x)",colIndex,fgdColour,bgdColour);
		dspl_ScrText (0, 180, str, 0);
	}
}
/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History

*******************************************************************************/
void resources_setSKColour(unsigned int colIndex)
{
	U32 fgdSKColour,bgdSKColour;
	getDataColour(colIndex, COL_TYPE_SK, &fgdSKColour,  &bgdSKColour );
	oldmnuFgdCol = dspl_SetFgdColour( fgdSKColour);
	oldmnuBgdCol = dspl_SetBgdColour( bgdSKColour);
}
/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History

*******************************************************************************/
void resources_setHLColour(unsigned int colIndex)
{
	U32 fgdColour,bgdColour;
#ifdef COLOURDISPLAY
	getDataColour(colIndex, COL_TYPE_HL, &fgdColour,  &bgdColour );
#else
	//For b+w highlight, get fgd colour as bgd and vice versa
	fgdColour = dspl_GetBgdColour();
	bgdColour = dspl_GetFgdColour();
	//If one colour is black/white, make other colour the opposite
	if (fgdColour == COL_Black)
		bgdColour = COL_White; 
	else if (bgdColour == COL_Black)
		fgdColour = COL_White; 
	else if (fgdColour == COL_White)
		bgdColour = COL_Black; 
	else if (bgdColour == COL_White)
		fgdColour = COL_Black;
	else
	{	//Default : assume the rest of the display is black on white=> highlight is white on black.
		fgdColour = COL_White;
		bgdColour = COL_Black; 	
	}

#endif
	oldmnuFgdCol = dspl_SetFgdColour( fgdColour);
	oldmnuBgdCol = dspl_SetBgdColour( bgdColour);
}
/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History

*******************************************************************************/
void resources_setTitleColour(unsigned int colIndex)
{
	U32 fgdColour,bgdColour;
	getDataColour(colIndex, COL_TYPE_TITLE, &fgdColour,  &bgdColour );
	oldmnuFgdCol = dspl_SetFgdColour( fgdColour);
	oldmnuBgdCol = dspl_SetBgdColour( bgdColour);
}
/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History

*******************************************************************************/
void resources_restoreColour(void)
{
	dspl_SetFgdColour( oldFgdCol);
	dspl_SetBgdColour( oldBgdCol);
}
/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History
  GW 28/11/02 - 

*******************************************************************************/
void resources_restoreMnuColour(void)
{
	dspl_SetFgdColour( oldmnuFgdCol);
	dspl_SetBgdColour( oldmnuBgdCol);
}


/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History
  GW 28/11/02 - 

*******************************************************************************/
void getDisplayData(int txtId,int *colDisplayType, int *colType, int *colFgdSelect)
{
	*colFgdSelect = NO_COL;

/**/
	switch (txtId)
	{

default:	
				*colFgdSelect = NO_COL;	
				break;

case    TxtIdleScreenBackground:	
				*colDisplayType = 	COLOUR_IDLE;
				*colType = 			COL_TYPE_SCREEN;
				*colFgdSelect = 	BGD_COL;
				break;								
case	TxtIdleScreenSoftKey:
				*colDisplayType = 	COLOUR_IDLE;
				*colType = 			COL_TYPE_SK;
				*colFgdSelect = 	FGD_COL;
				break;								
case	TxtIdleScreenText:
				*colDisplayType = 	COLOUR_IDLE;
				*colType = 			COL_TYPE_SCREEN;
				*colFgdSelect = 	FGD_COL;
				break;								
case	TxtIdleScreenNetworkName:  
				*colDisplayType = 	COLOUR_IDLE;
				*colType = 			COL_TYPE_SCREEN;
				*colFgdSelect = 	FGD_COL;
				break;								
case	TxtGeneralBackground:		
				*colDisplayType = 	COLOUR_LIST_MAIN;
				*colType = 			COL_TYPE_SCREEN;
				*colFgdSelect = 	BGD_COL;
				break;								
case	TxtMenuSoftKeyForeground: 
				*colDisplayType = 	COLOUR_LIST_MAIN;
				*colType = 			COL_TYPE_SK;
				*colFgdSelect = 	FGD_COL;
				break;								
case	TxtMenuSoftKeyBackground:  	
				*colDisplayType = 	COLOUR_LIST_MAIN;
				*colType = 			COL_TYPE_SK;
				*colFgdSelect = 	BGD_COL;
				break;								
case	TxtSubMenuHeadingText:  	
				*colDisplayType = 	COLOUR_LIST_SUBMENU;
				*colType = 			COL_TYPE_TITLE;
				*colFgdSelect = 	FGD_COL;
				break;								

case	TxtSubMenuHeadingBackground:  
				*colDisplayType = 	COLOUR_LIST_SUBMENU;
				*colType = 			COL_TYPE_TITLE;
				*colFgdSelect = 	BGD_COL;
				break;								
case	TxtSubMenuTextForeground:  	
				*colDisplayType = 	COLOUR_LIST_SUBMENU;
				*colType = 			COL_TYPE_SCREEN;
				*colFgdSelect = 	FGD_COL;
				break;								

case	TxtSubMenuTextBackground:  	
				*colDisplayType = 	COLOUR_LIST_SUBMENU;
				*colType = 			COL_TYPE_SCREEN;
				*colFgdSelect = 	BGD_COL;
				break;								
case	TxtSubMenuSelectionBar:  	
				*colDisplayType = 	COLOUR_LIST_SUBMENU;
				*colType = 			COL_TYPE_HL;
				*colFgdSelect = 	BGD_COL;
				break;								
case	TxtPopUpTextForeground: /* GW#2294 COLOUR_STATUS has been used for all popups - change this colour instead of COLOUR_POPUP .*/
				*colDisplayType = 	COLOUR_STATUS;
				*colType = 			COL_TYPE_SCREEN;
				*colFgdSelect = 	FGD_COL;
				break;								
case	TxtPopUpTextBackground: /* GW#2294 COLOUR_STATUS has been used for all popups - change this colour instead of COLOUR_POPUP .*/
				*colDisplayType = 	COLOUR_STATUS;
				*colType = 			COL_TYPE_SCREEN;
				*colFgdSelect = 	BGD_COL;
				break;								
		}
/**/
}
/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History

*******************************************************************************/
//This is called to set the colour of menu items controlled by the text id's
int resources_getColourFromText(int txtId)
{
	U32 col;
	switch (txtId)
	{
case	TxtColourBlack:			col = COL_Black;		break;
case	TxtColourBlue:			col = COL_Blue;			break;
case	TxtColourCyan:			col = COL_Cyan;			break;
case	TxtColourGreen:			col = COL_Green;		break;
case	TxtColourMagenta:		col = COL_Magenta;		break;
case	TxtColourYellow:		col = COL_Yellow;		break;
case	TxtColourRed:			col = COL_Red;			break;
case	TxtColourWhite:			col = COL_White;		break;
case	TxtColourPink:			col = COL_Pink;			break;//	0x00FF4040;		break;
case	TxtColourOrange:		col = COL_Orange;		break;
case	TxtColourLightBlue:		col = COL_LightBlue;	break;//	0x004040FF;	break;
case	TxtColourLightGreen: 	col = COL_LightGreen;	break;//	0x0040FF40;	break;
case	TxtColourPaleYellow:	col = COL_PaleYellow;	break;//	0x00FFFF40;	break;
case	TxtColourTransparent:	col = COL_Transparent;	break;
default:	col = NO_COLOUR_DEFINED;
		}
	return (col);
}



/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History
  GW 28/11/02 - 

*******************************************************************************/
void resources_setColourMnuItem( int txtId)
{
U32 col = NO_COLOUR_DEFINED;
int colType;
int colFgdSelect = FALSE;
int colDisplayType;
U32 fgd,bgd;

	col = resources_getColourFromText(txtId);
	if (col == NO_COLOUR_DEFINED)
	{
		getDisplayData(txtId, &colDisplayType, &colType,&colFgdSelect);
		getDataColour(colDisplayType, colType, &fgd, &bgd);
		switch (colFgdSelect)
		{
				case FGD_COL:	col = fgd;	break;
				case BGD_COL:	col = bgd;	break;
				default:					break;
		}
	}
	if (col != NO_COLOUR_DEFINED)
	{
		if (col!=COL_Transparent)
		{
			dspl_SetFgdColour( col );
			dspl_SetBgdColour( (~col) & 0x00FFFFFF );
		}
		else
		{
			dspl_SetFgdColour( COL_Black );
			dspl_SetBgdColour( COL_Magenta );
			
		}
	}		
}



/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History

*******************************************************************************/
int res_getTitleHeight( void )
{
	if (Mmi_getCurrentLanguage() != CHINESE_LANGUAGE)
	{
		return(TITLE_HEIGHT_LATIN);
	}
	else
	{
		return(TITLE_HEIGHT_CHINESE);
	}
}
/*Function added for SPr 1442*/
/*******************************************************************************
 $Function:    	string_len_in_bytes

 $Description:	internal function which returns th length of a string in bytes

 $Returns:		length of string

 $Arguments:	string, maximum possibel length, whether unicode or not
 
*******************************************************************************/
int string_len_in_bytes(char* string, int max_len, int unicode)
{	int i;
	int length=0;
	/* if unicode, nee to check for double-NULL termination*/
	if (string[0] == (char)0x80 || unicode== TRUE) /*a0393213 lint warnings removal - typecast done*/
	{
		for(i=0; i < max_len; i+=2)
		{	if (string[i] == 0 && string[i+1]==0)
			{	length = i;
				break;
			}
		}
		return length;

	}

	else/*just use standard function*/
	{	return strlen(string);
	}

}

#ifdef LSCREEN
#define TRUNC_LENGTH 3 /*... at end of string in D-sample*/
#else
#define TRUNC_LENGTH 1/* . at end of string in c-sample*/
#endif

/*Function added for SPr 1442*/
/*******************************************************************************
 $Function:    	resources_truncate_to_screen_width

 $Description:	Copies and truncates the input string to the passed screen width 
 				and puts it into the output string.
 				Note: this function only works for Unicode strings which have a unicode
 				tag at the beginning, or if the unicode parameter is set to TRUE.  
 				If the input length is zero, then this function calculates 
 				the actual length of the input string.

 $Returns:		length of truncated string

 $Arguments:	input sctring and it's length, ouput string and it's length, 
 				screen width desired (pixels), unicode flag (TRUE/FALSE)
 
*******************************************************************************/
//GW #1541 16/01/03 - Modified code to first test if string will fit and return string if it does. 
int resources_truncate_to_screen_width(char* IpStr, int IpLen, char* OpStr, int OpLen,
	int ScreenWidth, BOOL unicode)

{	
	int start_position = 0;
	#ifdef LSCREEN
	char* ellipse_str = "...";
	#else
	char* ellipse_str = ".";
	#endif
	int ellipsis_len = dspl_GetTextExtent(ellipse_str, 0);
	int i;
	int j;
	int calc_input_len = 0;
	int no_of_chars = 0;
	int len;
	
	memset(OpStr, 0, OpLen);

	/*if input string length 0, we calculate the string len ourselves*/
	if (IpLen == 0)
	{ calc_input_len = string_len_in_bytes(IpStr, 256, unicode);
	}
	else
	{	calc_input_len = IpLen;
	}	/*Test for string alrady fitting */
	
	if (dspl_GetTextExtent(IpStr, 0) <  ScreenWidth)
	{
		for (i=0; i<OpLen && i<calc_input_len; )
		{	//if length of string less that width of screen
			//if unicode copy two chars
			if (IpStr[0] == (char)0x80 || unicode == TRUE)/*a0393213 lint warnings removal-typecasting done*/
			{	
				OpStr[i] = IpStr[i];
				OpStr[i+1] = IpStr[i+1];
				i+=2;
			}
			else
			{	OpStr[i] = IpStr[i];
				i++;
			}
		}
		return string_len_in_bytes(OpStr, OpLen, unicode);		
	}
	/*SPR 1541, use function dspl_GetNcharToFit rather than loop to work 
	out how many chars will fit in line*/
	no_of_chars = dspl_GetNcharToFit (IpStr,  ScreenWidth - ellipsis_len);
	/*copy characters*/
	if (no_of_chars < OpLen)
		memcpy(OpStr, IpStr, no_of_chars);
	else
		memcpy(OpStr, IpStr, OpLen);
		
	/*SPR 1541, end*/
	TRACE_EVENT_P1("trunc string len:%d", dspl_GetTextExtent(OpStr, 0));
	/*we need to add the ellipsis if we've truncated the string*/
	if(string_len_in_bytes(OpStr, OpLen, unicode) < calc_input_len)
	{	/*if unicode*/
		if (OpStr[0] == (char)0x80|| unicode == TRUE) /*a0393213 lint warnings removal - typecasting done*/
		{	
			if (string_len_in_bytes(OpStr, OpLen, TRUE) < (OpLen -2*TRUNC_LENGTH+2))
			{  start_position = string_len_in_bytes(OpStr, OpLen, unicode);
			}
			else
			{	start_position = OpLen - 2*TRUNC_LENGTH+2;}
		TRACE_EVENT_P1("Adding chars at %d", start_position);
			for(j=start_position; j < start_position+2*TRUNC_LENGTH; j+=2)
			{ 	OpStr[j] = 0;
				OpStr[j + 1] = '.';
			}
			/*add NULL termination*/
			OpStr[start_position+ 2*TRUNC_LENGTH] = NULL;
			OpStr[start_position+ 2*TRUNC_LENGTH+1] = NULL;
		}
		else  /*if ASCII*/
		{	len = strlen(OpStr); 
			if (len < (OpLen -(TRUNC_LENGTH+1)))
			{  start_position = string_len_in_bytes(OpStr, OpLen, FALSE);
			}
			else
			{	start_position = OpLen - TRUNC_LENGTH;}
			memcpy(&OpStr[start_position], ellipse_str, TRUNC_LENGTH);

		}
	}

TRACE_EVENT_P1("trunc string len post ellipsis:%d", dspl_GetTextExtent(OpStr, 0));
	
	return string_len_in_bytes(OpStr, OpLen, unicode);

}

/*******************************************************************************

 $Function:    	

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
 $History
  GW 28/11/02 - 

*******************************************************************************/
/***************************Go-lite Optimization changes Start***********************/
//Aug 25, 2004    REF: CRR 24904   Deepa M.D
//This function is used only for the Color build.Hence it is put under the 
//COLOURDISPLAY compiler switch
#ifdef COLOURDISPLAY
int colMenuSelected(MfwMnu* mnu, MfwMnuItem* mnuItem)
{
	T_MFW_HND win = mfw_parent(mfw_header());
	int mnuId;
	int colDisplayType;
	int colType;
	int colFgdSelect = FALSE;
	int mnuColour;
	int storedMenuId;

	//GW Read the stored menu Id from generic procedure
	storedMenuId = MmiBook_getStoredOption();
	mnuId = (int)mnuItem->str;

	mnuColour=resources_getColourFromText(mnuId);

	getDisplayData(storedMenuId, &colDisplayType, &colType,&colFgdSelect);
	
	setDataColour(colDisplayType, colType, colFgdSelect,mnuColour);

	info_screen(win, TxtColour, TxtSelected, 0);

	// API/GW - Saved the selected colour so the flash file structure after selected
	flash_data_write(colourFileDir, colourFileName, colArray, sizeof(colArray));
	
	return(0);
}

#endif 
/***************************Go-lite Optimization changes Start***********************/


/*******************************************************************************
 $Function:    	set_SIMToolkit_title_string

 $Description:	Changes the value of the SAT main menu entry title.

 $Returns:		

 $Arguments:	New SAT Title.
 
*******************************************************************************/


void res_set_SIMToolkit_title_string(char* SAT_string)
{
    SIMToolKitString = SAT_string;        
}

/*API - 27/07/03 - CQ10203 - New function to reset the SMS VP in flash on factory reset*/
/*******************************************************************************
 $Function:    	resetSMSValidityPeriod

 $Description:	Resets the value of the SMS VP in flash
 
 $Returns:		

 $Arguments:	None
 
*******************************************************************************/
void resetSMSValidityPeriod()
{
/* x0045876, 14-Aug-2006 (WR - "sms_settings" was set but never used) */
#ifndef NEPTUNE_BOARD
	T_MFW_SMS_INFO sms_settings;  
#endif
	
	TRACE_EVENT("resetSMSValidityPeriod()");

	if ( FFS_flashData.vp_rel != VP_REL_24_HOURS)
		FFS_flashData.vp_rel = VP_REL_24_HOURS;

/* x0045876, 14-Aug-2006 (WR - "sms_settings" was set but never used) */
#ifndef NEPTUNE_BOARD
	/*NDH - 13/01/04 - CQ16753 - Propogate the Validity Period Settings to the ACI */
	sms_settings.vp_rel = FFS_flashData.vp_rel;
#endif

#ifdef NEPTUNE_BOARD
	//(void)sms_set_val_period(&sms_settings); /* NEPTUNE: Sumit: temp comment */
#else
	(void)sms_set_val_period(&sms_settings);
#endif
	/*NDH - 13/01/04 - CQ16753 - END */

}
/*API - CQ10203 - END*/
/*******************************************************************************
                                                                              
                                End of File
                                                                              
*******************************************************************************/

