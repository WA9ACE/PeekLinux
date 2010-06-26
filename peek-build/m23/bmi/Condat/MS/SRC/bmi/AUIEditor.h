#ifndef MMI_AUIEDITOR_H
#define MMI_AUIEDITOR_H

/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name:	Basic MMI
 $Project code:	BMI (6349)
 $Module:		MMI
 $File:			AUIEditor.h
 $Revision:		1.0

 $Author:		Condat(UK)
 $Date:			30/01/02

********************************************************************************

 Description: Global header file for AUI Editor

 REQUIRES: ATBCommon.h, ATBDisplay.h, ATBEditor.h

********************************************************************************
 $History: AUIEditor.h

  30/01/02      Original Condat(UK) BMI version.

 $End

*******************************************************************************/




/*******************************************************************************

                    Type definitions

*******************************************************************************/

/*
 * Definitions from common editor function
 */
#define INFO_TIMEOUT			1
#define INFO_KCD_LEFT			2
#define INFO_KCD_RIGHT			3
#define INFO_KCD_HUP			4
#define INFO_KCD_UP				5
#define INFO_KCD_DOWN			6
#define INFO_KCD_0_9			7
#define INFO_KCD_ALTERNATELEFT  8
#define INFO_KCD_STAR			9
#define INFO_KCD_SELECT			10
#define INFO_EMERGENCY			99

#define ED_PREDTEXT_MAX			16				/* Maximum length of predicted text string */
#define ED_SCROLLBAR_WIDTH		4				/* Width of scrollbar */

#define MAX_RO_EDITOR_LEN		1000

#ifndef FOREVER
#define TIMER_EXIT				0x0BB8			/* Default SMS timer period - 3 secs.  */
#define SHORT_SECS				0x01F4			/* Information dialogue display times. */
#define THREE_SECS				0x0BB8			/* 3000 milliseconds.                  */
#define FIVE_SECS				0x1388          /* 5000 milliseconds.                  */
#define TEN_SECS    			0x2710          /* 10000 milliseconds.                 */
#define TWO_MIN					0x1D4C0         /* 2 min                               */
#define FOREVER					0xFFFF          /* Infinite time period.               */
#endif /* FOREVER */


/****************************************************************************/
/*																			*/
/* TYPES																	*/
/*																			*/
/****************************************************************************/

/* External & internal events */

typedef enum
{
    E_ED_INIT,
    E_ED_DESTROY,/*MC SPR 1752*/
    E_ED_DEINIT,
    E_ED_UPDATE,
    E_ED_INSERT
} E_ED_EVENTS;

/* Callback type */

typedef void (*T_AUI_EDIT_CB) (T_MFW_HND, USHORT, SHORT);


/****************************************************************************/
/*																			*/
/* STRUCTURES																*/
/*																			*/
/****************************************************************************/

/* T_AUI_EDITOR_DATA */
 
typedef struct
{
	T_ED_ATTR		editor_attr;		/* The editor attributes */
	int				zone_id;			/* Type of editor window */
    
	USHORT			Identifier;			/* Identifier passed on to callback function */
	ULONG			timeout;			/* Time before editor calls callback */
	T_AUI_EDIT_CB	Callback;			/* Callback function */
	BOOL			destroyEditor;		/* TRUE if editor is to be destroyed on exit */
	UBYTE			min_enter;			/* Minimum number of characters necessary to enter */
	BOOL			change_rsk_on_empty; /* TRUE if text of right soft key changes when buffer is empty*/
	
	USHORT         TitleId;				/* Text ID for the title of the editor */
	UBYTE          	*TitleString;		/* String for title of editor (ignored if TitleId is non-null) */
	USHORT			LeftSoftKey;		/* Text ID for left soft key */
	USHORT			AltLeftSoftKey;		/* Text ID for soft key to appear before min_enter characters entered */
	USHORT			RightSoftKey;		/* Text ID for right soft key */
	USHORT			AltRightSoftKey;	/* Text ID for soft key to appear when buffer is empty */
	/* SPR#2672 - SH - Removed array */
} T_AUI_EDITOR_DATA;


/****************************************************************************/
/*																			*/
/* FUNCTION PROTOTYPES														*/
/*																			*/
/****************************************************************************/

/* General-purpose Editor */

T_MFW_HND	AUI_edit_Start(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data);
T_MFW_HND	AUI_edit_QuickStart(T_MFW_HND win, USHORT TitleId, UBYTE *TitleString, USHORT *buffer, USHORT len, T_AUI_EDITOR_DATA *editor_info, T_AUI_EDIT_CB editor_cb);
void		AUI_edit_Destroy(T_MFW_HND window);
void		AUI_edit_InsertString(T_MFW_HND win, UBYTE *string, T_ATB_DCS dcs);
void 		AUI_edit_InsertChar(T_MFW_HND win, USHORT character);

/* AUI Editor tools */

void AUI_edit_GetSpecificWindowSize(T_ED_ATTR* attr, int zone_id );
void AUI_edit_CalculateWindow(T_ED_ATTR* attr, int *zone_id );	/* SPR#1559 - SH - Change zone_id to a pointer to int */
void AUI_edit_SetDefault(T_AUI_EDITOR_DATA *editor_data);
void AUI_edit_SetDisplay(T_AUI_EDITOR_DATA *editor_data, int zone_id, int colour, UBYTE font );
void AUI_edit_SetEvents(T_AUI_EDITOR_DATA *editor_data, USHORT Identifier, BOOL destroyEditor, ULONG timeout, T_AUI_EDIT_CB Callback);
void AUI_edit_SetMode(T_AUI_EDITOR_DATA *editor_data, USHORT mode, USHORT cursor );
void AUI_edit_SetBuffer(T_AUI_EDITOR_DATA *editor_data, UBYTE dcs, UBYTE *text, USHORT size);
void AUI_edit_SetTextStr(T_AUI_EDITOR_DATA *editor_data, USHORT LeftSoftKey, USHORT RightSoftKey, USHORT TitleId, UBYTE *TitleString);
void AUI_edit_SetAltTextStr(T_AUI_EDITOR_DATA *editor_data, UBYTE min_enter, USHORT AltLeftSoftKey, BOOL change_rsk_on_empty, USHORT AltRightSoftKey);
void AUI_edit_SetFormatStr(T_AUI_EDITOR_DATA *editor_data, char *formatstr, BOOL preformat, char filler);
void AUI_edit_SetFormatAttr(T_AUI_EDITOR_DATA *editor_data, UBYTE attr);
void AUI_edit_SetAttr(T_ED_ATTR *attr, int zone_id, ULONG colour, UBYTE font, USHORT mode, USHORT cursor, T_ATB_DCS dcs, UBYTE *text, USHORT size);
void AUI_Edit_Display_Only(T_MFW_HND win, int LSK, int RSK, char *string1, char *string2, int Title);
#endif /* _DEF_MMI_ED_EDITOR */
