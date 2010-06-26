#ifndef _DEF_MMI_MAIN_H_
#define _DEF_MMI_MAIN_H_

/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   MMI
 $File:       MmiMain.h
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

    This provides the root mofule for the basic MMI

********************************************************************************
 $History: MmiMmi.h

  25/10/00      Original Condat(UK) BMI version.

       Jun 09, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Roaming indication feature implementation
       Solution     : As part of the implementation, in this file, type of GlobalIconStatus is changed       
       
 $End

*******************************************************************************/




/*******************************************************************************

                    Type definitions

*******************************************************************************/




#include "mfw_cphs.h"

#include "MmiResources.h"


#define PROMPT(x, y, a, id) dspl_TextOut(x, y, a|DSPL_TXTATTR_CURRENT_MODE, (char*)MmiRsrcGetText(id))
#define ALIGNED_PROMPT(x_a, y, a, id) displayAlignedPrompt(x_a, y, a, id)
#define GET_TEXT(id) MmiRsrcGetText( id )

#define ETRACE(str) \
{   char buf[64];##str##;\
  TRACE_EVENT(buf);}

#define FTRACE(str) \
{   char buf[64];##str##;\
  TRACE_EVENT(buf);}


/* typedefs */
typedef enum {
  NM_UNDEFINED,
  NM_FULL_SERVICE,
  NM_LIMITED_SERVICE,
  NM_NO_SERVICE,
  NM_SEARCHING,
  NM_NO_NETWORK
} MMI_NM_SERVICE;


typedef enum
{
    LEFT,
    RIGHT,
    CENTER
} T_MMI_ALIGN;


typedef enum {
  SIM_UNDEFINED,
  SIM_AVAIL,
  SIM_REJECTED,
  SIM_NOT_AVAIL
} MMI_SIM_STATUS;

typedef enum {
  TWELVE_HOUR,
  TWENTYFOUR_HOUR
} ClockFormats;

/*MC SPR 1725 removed date and clock definition*/


#define MAX_LINE 14

#define POS_BEGIN 0
#define POS_END 1
#define MAX_LINE 14
#define EDITOR_SIZE 41

                                       /* GLOBAL DATA              */
extern UBYTE editControls [KCD_MAX];
extern LngInfo mainLngInfo;             /* language control block   */
extern MfwHnd mainMmiLng;               /* global language handler  */

                                        /* PROTOTYPES               */
void mainInit (UBYTE InitCause);
void mainExit (UBYTE InitCause);

void setClockFormat(UBYTE format);
// Issue Number : MMI-SPR-12159 on 07/04/04 by Rashmi C N and Deepa M D
// change by Sasken ( Rashmi C N  and Deepa M D) on April 07th 2004
// Bug : The AM PM for 12 hour fomrat was not being shown
// Solution : Added the following function to get the current time format from flash
UBYTE getClockFormat(void);  // RAVI

/*SPR 1725, removed getClockString(), Clockset() and DateSet() function headers*/
void showCLOCK(void);
void showDATE(void);
/*SPR 1442 obsolete function*/
//void truncateString(char* strDest,char* strOrig,unsigned int truncLength,int pos,char* trunChar);
void displayAlignedText( T_MMI_ALIGN x_alignment, USHORT y_lineNum, UBYTE attrib, char* string );
void displayAlignedPrompt( T_MMI_ALIGN x_alignment, USHORT y_lineNum, UBYTE attrib, int string_id);
void mainShutdown(void);

extern char stringTrace[50];

/* Battery and Signal Status */
#define GlobalNothingIconFlag     0x00
#define GlobalBatteryIconFlag     0x01
#define GlobalSignalIconFlag      0x02
#define GlobalSMSIconFlag       0x04
#define GlobalVoiceMailIconFlag     0x08
#define GlobalCallForwardingIconFlag  0x10
#define GlobalSilentRingerIconFlag    0x20
#define GlobalKeyplockIconFlag      0x40
#define GlobalRingerIconFlag      0x80
#define GlobalVibratorIconFlag      0x100
#define GlobalRingVibrIconFlag      0x200
#define GlobalAlarmIconFlag         0x400
#define GlobalCipheringActivIconFlag       0x800
#define GlobalCipheringDeactivIconFlag       0x1000
/* SH 18/01/02.  Indication for GPRS */
#ifdef MMI_GPRS_ENABLED
#define GlobalGPRSOnIconFlag				0x2000
#endif
/* SPR877 - SH - Indication for Homezone */
#ifdef MMI_HOMEZONE_ENABLED
#define GlobalHomezoneIconFlag				0x4000
#endif

#ifdef MMI_TTY_ENABLED
    #ifndef GlobalHomezoneIconFlag				
        #define GlobalHomezoneIconFlag				0x4000
    #endif
#endif

/* SPR#1352 - SH - TTY indication */
#define GlobalTTYIconFlag					0x8000
/* end SH */

/*  Jun 09, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar) */     
#define GlobalRoamingIconFlag					0x10000

/*    Jun 09, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Roaming indication feature implementation
       Solution     : As part of the implementation, in this file, type of GlobalIconStatus is changed       */
extern U32 GlobalIconStatus;
extern T_MFW_CFLAG_STATUS DivertStatus;

/* It could be helpful to check the      */
/* global status where the user is :     */

/* It is also possible to be in several  */
/* status  !                             */

#define ModuleIdle       0x001  /*in Idle Screen           */
#define ModuleCall       0x002  /*in during a call         */
#define ModuleInCall     0x004  /*in Incoming call         */
#define ModuleMenu       0x008  /*in Menu                  */
#define ModulePhon       0x010  /*in Phonebook             */
#define ModuleSms        0x020  /*in SMS                   */
#define ModuleBattLow    0x040  /*in Battery Status < 5%   */
#define ModuleSAT        0x080  /*in Sim Toolkit, like Idle*/
#define ModuleAlarm      0x100  /*in Organiser Alarm       */
#define ModuleFax        0x200  /*in Fax/Data              */
#define ModuleDialling   0x400  /*in Dialling state      */

extern int MmiModule;




/* PROTOTYPES */

void MmiModuleSet (int modul); /* Set the status where the user is       */
void MmiModuleDel (int modul); /* Delete the status where the user was   */




/* Settings status*/

#define SettingsAnyKeyAnswerOn      0x01
#define SettingsAutoAnswerOn      0x02
#define SettingsKeyPadLockOn      0x04
#define SettingsKeypadClick       0x08
#define SettingsKeypadDTMF        0x10
#define SettingsSilentMode        0x20
#define SettingsCBDisable			0x80	/* SPR 2470 0x80- disable CB display 0-display CB */
//extern USHORT settingsStatus;

#define GlobalBatteryLesser5     1
#define GlobalBatteryGreater5    2
#define GlobalBatteryFull        3

extern UBYTE globalBatteryMode; /*  */


extern UBYTE globalBatteryPicNumber;
extern UBYTE globalSignalPicNumber;


#define GlobalTimeout1sec     0x03e8         /* 1 second for flash the battery icon  */
#define GlobalTimeout2min     0x01d4c0       /* 2 min for battery beep  */
#define GlobalTimeout15min    0x0dbba0       /* 15 min for battery beep  */


void editActivate (MfwHnd e,int AlphaMode);
void editHiddenActivate (char *buffer);
void editDeactivate (void);
int editEventKey (MfwEvt e, MfwKbd *k);
MfwHnd activeEditor(void);

// sbh - functions for format mode (WAP)

void setFormatPointers (char *format, int *index, int *index2);
void editCharFindNext(char character);
int editFindPrev(void); // RAVI

/* Added to remove warning Aug - 11 */
#ifdef NEPTUNE_BOARD
extern UBYTE getcurrentSMSTone(void);
#endif  /* NEPTUNE_BOARD */
/* End - remove warning Aug - 11 */

/*NM p032
this flag indicate when the TEST SIM
is inserted*/
extern UBYTE test_sim;

#endif
