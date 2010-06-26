/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name:	Basic MMI
 $Project code:	BMI (6349)
 $Module:		MMI
 $File:			AUIEditor_i.h
 $Revision:		1.0

 $Author:		Condat(UK)
 $Date:			13/11/02

********************************************************************************

 Description:

    This is the internal header file for AUIEditor.c - that is, this header file should
    only be included by MmiEmsEditor.c.  The wider-purpose header file is
    AUIEditor.h.

********************************************************************************
 $History: AUIEditor_i.h

  13/11/02      Original Condat(UK) BMI version.

 $End

*******************************************************************************/




/*******************************************************************************

                    Type definitions

*******************************************************************************/

#ifndef MMI_AUI_EDITOR_I_H
#define MMI_AUI_EDITOR_I_H

/* 
 * Internal structure
 */
 
typedef struct
{
    T_MMI_CONTROL		mmi_control;
    T_MFW_HND			parent;				/* The parent window */
    T_MFW_HND			win;				/* The edit window */
    T_MFW_HND			kbd;				/* The keyboard handler */
    T_MFW_HND			kbd_long;			/* The longpress keyboard handler */
    T_MFW_HND			timer;				/* The timer for timeouts */
    T_MFW_HND			title_timer;		/* Timer used for scrolling title */
    USHORT				title_pos;			/* Start position to display title text */
    USHORT				title_next_pos;		/* The next position to display the title text */

	T_ED_DATA			*editor;			/* The ATB editor */
    T_AUI_EDITOR_DATA	editor_data;		/* The MMI Editor data */
	T_AUI_ENTRY_DATA	*entry_data;		/* Key entry data */
	
    T_ATB_TEXT			title;				/* Title of editor */
    BOOL				hasTitle;			/* TRUE if title is provided */

	BOOL				doNextLongPress;	/* Flag used to notice/not notice long keypress */

	T_ATB_TEXT			predText;			/* Predicted word buffer    */
	T_ED_CHAR			predTextChar;		/* Current character	*/

	char				*displayBuffer;		/* SPR#2672 - SH - Pointer to display buffer */
}
T_AUI_EDITOR_INFO;

/*******************************************************************************

 Internally used functions

*******************************************************************************/

static T_MFW_HND AUI_edit_Create(T_MFW_HND parent, T_AUI_EDITOR_DATA *editor_data);
static void AUI_edit_Init(T_MFW_HND window);
static void AUI_edit_ExecCb(T_MFW_HND win, USHORT event, USHORT value, void *parameter);
static int AUI_edit_WinCb(T_MFW_EVENT event, T_MFW_WIN *win);
static int AUI_edit_KbdCb(T_MFW_EVENT event, T_MFW_KBD *keyboard);
static int AUI_edit_KbdLongCb(T_MFW_EVENT event, T_MFW_KBD *keyboard);
static int AUI_edit_TimerCb (T_MFW_EVENT event, T_MFW_TIM *timer);
static int AUI_edit_TitleTimerCb (T_MFW_EVENT event, T_MFW_TIM *timer);
static void AUI_edit_CalcPredText(T_AUI_EDITOR_INFO *data);
#endif

