/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_lng.h       $|
| $Author:: Kk  $ CONDAT GmbH           $Revision:: 2               $|
| CREATED: 27.08.99                     $Modtime:: 10.03.00 11:38   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_LNG

   PURPOSE : Language Handler types & constants

   EXPORT  :

   TO DO   :

   $History:: mfw_lng.h                                             $
 * 
 * *****************  Version 2  *****************
 * User: Kk           Date: 14.03.00   Time: 15:48
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * added handling of various texts
 * 
 * *****************  Version 1  *****************
 * User: Le           Date: 4.01.00    Time: 13:09
 * Created in $/GSM/Condat/MS/SRC/MFW
 * 
 * *****************  Version 2  *****************
 * User: Es           Date: 22.11.99   Time: 10:29
 * Updated in $/GSM/Condat/SND-MMI/MFW
 * 
 * *****************  Version 1  *****************
 * User: Es           Date: 18.11.99   Time: 16:35
 * Created in $/GSM/Condat/SND-MMI/MFW
 * Initial
 
 	Nov 07, 2006 ER:OMAPS00070661 R.Prabakar(a0393213)
	R99 network compliancy : Implemented Language Notification and Provide Local Information (Language setting)
	SAT proactive commands and Language Selection SAT event
*/

#ifndef _DEF_MFW_LNG_H_
#define _DEF_MFW_LNG_H_

#include "mfw_mfw.h"
#ifdef FF_MMI_R99_SAT_LANG
/*OMAPS00070661 (SAT-lang notification, local info and lang setting) a0393213(R.Prabakar) macros related to language
    moved from MmiBlkLangDB.h*/
    
//#define LANG_CHANGE_TEST

/*MC SPR 1150, moved language macros from MmiResources.h*/
/*There should be language definitions for each number from 1 to NO_OF_LANGUAGES*/
#define ENGLISH_LANGUAGE 1
#ifdef CHINESE_MMI
#define CHINESE_LANGUAGE  2
#define GERMAN_LANGUAGE  3
#else
#define GERMAN_LANGUAGE  2
#define CHINESE_LANGUAGE  3
#endif


#ifdef LANG_CHANGE_TEST /*MC test code*/
#define MARTIAN_LANGUAGE 2
#define CHINESE_LANGUAGE  3
#endif

/*MC, SPR  1150, allows easier addition of languages, just increment macro*/
#ifndef CHINESE_MMI
#define NO_OF_LANGUAGES 2
#else
#define NO_OF_LANGUAGES 2
#endif

#endif /*FF_MMI_R99_SAT_LANG*/

typedef struct LngInfoTag               /* Language information     */
{
    int language;                       /* selected language        */
    unsigned int nPrompts;              /* number of prompts & text */
    const char* const* const* prompts;  /* prompts and menu texts   */
} LngInfo;

typedef struct MfwLngTag                /* LANGUAGE CONTROL BLOCK   */
{
    MfwEvt map;                         /* selection of events      */
    MfwEvt evt;                         /* current event            */
    MfwCb handler;                      /* event handler            */
    LngInfo *lng;                       /* language info block      */
} MfwLng;

                                        /* EVENTS                   */

                                        /* PROTOTYPES               */
MfwRes lngInit   (void);
MfwRes lngExit   (void);
MfwHnd lngCreate (MfwHnd w, MfwEvt e, LngInfo *lng, MfwCb f);
MfwRes lngDelete (MfwHnd h);
char * lngText   (MfwHnd h, unsigned int t);
void   lngSignal (void/*char make, char key*/);

#endif

