#ifndef AUITEXTENTRY_H
#define AUITEXTENTRY_H

/* Key entry information*/
	
typedef struct
{
	T_MFW_HND			win;				/* The editor window */
	U32					update;				/* Event to be sent to the window to update it */
	T_ED_DATA			*editor;			/* Pointer to editor data */
	UBYTE				Level;            	/* Alpha level selected     */
	UBYTE				Key;                /* Alpha mode current key   */
	T_MFW_HND			editTim;			/* The timer for multi-tap */
}
T_AUI_ENTRY_DATA;


T_AUI_ENTRY_DATA 	*AUI_entry_Create(T_MFW_HND win, T_ED_DATA *editor, U32 update);
void 				AUI_entry_Destroy(T_AUI_ENTRY_DATA *entry_data);
USHORT 				AUI_entry_EditChar (T_AUI_ENTRY_DATA *entry_data, USHORT alphachar, BOOL multitap);
USHORT				AUI_entry_EditDigit (T_AUI_ENTRY_DATA *entry_data, UBYTE code);
USHORT				AUI_entry_EventKey (T_AUI_ENTRY_DATA *entry_data, MfwEvt event, MfwKbd *key);
USHORT				AUI_entry_GetKeyChar(T_AUI_ENTRY_DATA *entry_data, T_ED_CASE_PREF casePref);
void 				AUI_entry_StopMultitap(T_AUI_ENTRY_DATA *entry_data);
#endif
