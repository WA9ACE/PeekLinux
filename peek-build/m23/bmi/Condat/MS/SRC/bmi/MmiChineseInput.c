/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MmiChineseInput
 $File:		    MmiChineseInput.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:MMI Chinese Input handling.
 

                        
********************************************************************************

 $History: MmiChineseInput.c
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

	25/10/00			Original Condat(UK) BMI version.	
03/10/02		Replaced most of the file with version on 3.3.3 line (MC, SPR 1242)
	   
 $End

*******************************************************************************/


/*******************************************************************************
                                                                              
                                Include Files
                                                                              
*******************************************************************************/
#include <stdio.h>
#include <string.h>

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


#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "mfw_sms.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_lng.h"
#include "mfw_sat.h"
#include "mfw_kbd.h"
#include "mfw_nm.h"

#include "dspl.h"

#include "ksd.h"
#include "psa.h"

#include "MmiMain.h"
#include "MmiDummy.h"
#include "MmiMmi.h"

#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
#include "MmiChineseInput.h"

#include "mmiSmsBroadcast.h"
#include "mmiSmsMenu.h"
#include "mmismssend.h"
#include "mmiColours.h"

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#include "AUISymbol.h"
#else
#include "MmiLatinPredText.h"
#endif

#define CHINESE_KEYPAD_TIME 			1000	/*one second*/
#define	MAX_PHONEBOOK_ENTRY_SIZE		10  
#define	LOWER_CASE_ASCII_OFFSET			97		/*ASCII code for 'a'*/
#define  EZITEXT_OUTPUT_OFFSET			0xF400 	/*Add to ASCII to get display code for Jospeh's pinyin font*/

#define CHI_FONT_WIDTH				12/*mc SPR1319*/

/* SPR#1428 - SH - New Editor: don't use SymbolChar */
#ifndef NEW_EDITOR
extern int SymbolChar;
#endif

/*MC 15/07/02 Keypad counters; keep track of how many times a key has been pressed*/
static char Counter[KCD_STAR];




static const unsigned char TextIconeZiTextSmall[] =
{
	0x03,
	0x03,
	0x18,
	0x7c,
	0xcc,
	0x08,
	0x10,
	0x0c
};
USHORT  TEXT_BUFFER_EDIT_SIZE;

/*mc SPR1319, makes it easier to keep track of the mfw editor*/
#ifdef LSCREEN
/* SPR#1428 - SH - New Editor changes */
  #ifdef NEW_EDITOR
	 T_ED_DATA *editor_pointer;
  #else /* NEW_EDITOR */
	 T_MFW_HND* editor_pointer;
  #endif /* NEW_EDITOR */
#endif

const char	StrokePrompt[]	= {0x7B, 0x14, 0x52, 0x12, 0x00, 0x3A, 0x00, 0x00};
const unsigned char	PinYinPrompt[]	= {0x62, 0xFC, 0x97, 0xF3, 0x00, 0x3A, 0x00, 0x00};
ZI8WCHAR const ZiStrokeKeyMap[10][3] =   /* table for PINYIN */
{
  {0,	ZI8_STROKE_MORE},
  {1, 	ZI8_STROKE_OVER},
  {2, 	ZI8_STROKE_DOWN},
  {3, 	ZI8_STROKE_LEFT},
  {4,  	ZI8_STROKE_DOT},
  {5, 	ZI8_STROKE_OVER_DOWN},
  {6, 	ZI8_STROKE_CURVED_HOOK},
  {7, 	ZI8_STROKE_DOWN_OVER},
  {8, 	ZI8_STROKE_MORE},
  {9, 	ZI8_STROKE_OVER_DOWN_OVER},
};

ZI8WCHAR const ZiPinYinKeyMap[9][6] =   /* table for PINYIN */
{
  {ZI8_CODE_TONES_1,	ZI8_CODE_TONES_2, 	ZI8_CODE_TONES_3, 	ZI8_CODE_TONES_4,	ZI8_CODE_TONES_5,	5},
  {ZI8_CODE_PINYIN_A, 	ZI8_CODE_PINYIN_B, 	ZI8_CODE_PINYIN_C, 	0, 					0, 					3},
  {ZI8_CODE_PINYIN_D, 	ZI8_CODE_PINYIN_E, 	ZI8_CODE_PINYIN_F, 	0,					0,					3},
  {ZI8_CODE_PINYIN_G, 	ZI8_CODE_PINYIN_H, 	ZI8_CODE_PINYIN_I, 	0,					0,					3},
  {ZI8_CODE_PINYIN_J,  	ZI8_CODE_PINYIN_K, 	ZI8_CODE_PINYIN_L, 	0,					0,					3},
  {ZI8_CODE_PINYIN_M, 	ZI8_CODE_PINYIN_N, 	ZI8_CODE_PINYIN_O, 	0,					0,					3},
  {ZI8_CODE_PINYIN_P, 	ZI8_CODE_PINYIN_Q, 	ZI8_CODE_PINYIN_R, 	ZI8_CODE_PINYIN_S,	0, 					4},
  {ZI8_CODE_PINYIN_T, 	ZI8_CODE_PINYIN_U, 	ZI8_CODE_PINYIN_V, 	0,					0, 					3},
  {ZI8_CODE_PINYIN_W, 	ZI8_CODE_PINYIN_X, 	ZI8_CODE_PINYIN_Y,	ZI8_CODE_PINYIN_Z,	0, 					4},
};

typedef struct _IME_DATA
{
    int				CandidateCursor;	/* the indication of the current candidate highlighted */
    unsigned short 	TextBufferCount;         // Total number of characters contained in the wszTextBuffer.
    unsigned char	candidates;			/* last returned number of candidates */
    int				TextCursor;			/* the current test cursor */
    PZI8WCHAR		pTextBuffer;		/* the start of the text buffer */
    int				TextBufferEditSize;	/*  */
    int				TextBufferSize;		/*  */
    unsigned char	moreCount;			/* keeps track of the more count */
    PZI8WCHAR		pDisplayElementBuffer;
    ZI8UCHAR		EditorMode;			     // To store current editor mode i.e. stroke, PinYin
    unsigned char	InputScreenType;        // This variable is to state the different screen type. 
    PZI8GETPARAM	pGetParam;			/* pointer to Zi8GetParam */
} IME_DATA, * PIME_DATA;

typedef struct
{
    T_MMI_CONTROL		mmi_control;
    T_MFW_HND 		    parent_win;
    T_MFW_HND       	input_win;
    T_MFW_HND       	input_kbd;
    USHORT          	Identifier;

    ZI8UCHAR 		   	version;	
    ZI8WCHAR* 		   	wszTextBuffer;/*[TEXT_BUFFER_EDIT_SIZE + 1];*/	/* TEXT_BUFFER_EDIT_SIZE = 80 */
    ZI8WCHAR 		   	DisplayElementBuffer[TEXT_BUFFER_EXTRA_SPACE + 1];	/* TEXT_BUFFER_EXTRA_SPACE = 20 */
    ZI8WCHAR 		   	wszElementBuffer[TEXT_BUFFER_EXTRA_SPACE + 1];		/* TEXT_BUFFER_EXTRA_SPACE = 20 */
    ZI8GETPARAM 	   	GetParam;
    IME_DATA 		   	Ime;
    ZI8WCHAR 		   	wcCandidates[CANDIDATES_PER_GROUP+1];			/* CANDIDATES_PER_GROUP = 7 */
	ZI8WCHAR			StartOfDisplayIndex;
    ZI8WCHAR		   	HighlightedCandidate[2];  // This arrary is used to store the highlighted character pointed by CandidateCursor
	ZI8WCHAR		   	CursorPosition[2];  //Used to show the cursor Position in the composed text.
	ZI8UCHAR			NumberOfKeyPress;
	CHAR				PerviousKeyPress;
	T_MFW_HND		    options_win;
    /* Start of variable decleared for dummy functions. This is to be removed when eZiText code is received*/
    /* End */
  	 T_CHINESE_DATA		Callback_Data;
	/*MC 15/07/02 keypad timer for Pinyin*/
	T_MFW_HND			KeyPad_Timer_Handle;
#ifdef LSCREEN
  #ifdef NEW_EDITOR	/* SPR#1428 - SH - New Editor changes */
	T_ED_DATA			*editor;
	T_ED_ATTR			editor_attr;
  #else /* NEW_EDITOR */
	T_MFW_HND			editor;/*mc SPR1319, using mfw_editor for colour display*/
	MfwEdtAttr			editor_attr;
  #endif /* NEW_EDITOR */
#endif /* LSCREEN */
} T_chinese_input;


static T_MFW_HND 	chinese_input_create 		(T_MFW_HND  parent_win);

static void	chinese_input_exec			(T_MFW_HND  win, USHORT event, SHORT value, void * parameter);
static int	chinese_input_win_cb		(T_MFW_EVENT event, T_MFW_WIN * win);
static int	chinese_input_kbd_cb		(T_MFW_EVENT event, T_MFW_KBD * keyboard);

/*mc 15/07/02*/
static int chinese_pinyin_timer_cb (MfwEvt e, MfwTim *t);

/* Initialize the defined structure */
void IMEInitialize(PIME_DATA pIME, PZI8GETPARAM pGetParam, PZI8WCHAR wszTextBuffer, int TextBufferEditSize,
				  int TextBufferSize, PZI8WCHAR wcCandidates, PZI8WCHAR wszElementBuffer, PZI8WCHAR DisplayElementBuffer);
void IMECursorLeft(PIME_DATA pIME); /* move the cadidate cursor left */
void IMECursorRight(PIME_DATA pIME); /* move the cadidate cursor right */
void IMESelectCandidate(PIME_DATA pIME, PZI8WCHAR wszElementBuffer); /* Select the candidate at sursor */
void IMEMore(PIME_DATA pIME); /* get the next group of candidates */
void IMEKeyPress(PIME_DATA pIME, ZI8WCHAR wcStroke); /* enter the stroke and get new candidates */
void IMEClear(PIME_DATA pIME, PZI8WCHAR wszElementBuffer); /* Clear the last stroke entered */
void CandidateHighlightDisplayContorl(PIME_DATA pIME, PZI8WCHAR HighlightedCandidate);
void TextCursorDisplayContorl(PIME_DATA pIME, PZI8WCHAR CursorPosition, int InsertLine);
void IMEChangeMode(PIME_DATA pIME, PZI8GETPARAM pGetParam);
void DataConvert(PIME_DATA pIME);
ZI8WCHAR Uni_strlen(PZI8WCHAR String);

/*****************************Functions*****************************************/
/*******************************************************************************

 $Function:    	LatinChar

 $Description:	Returns true if character is in ASCII range
 
 $Returns:		UBYTE (TRUE/FALSE)	

 $Arguments:	Wide char in Ezitext format (not our display format)
 
*******************************************************************************/
UBYTE LatinChar(ZI8WCHAR char_code)
{	//if last byte is 0x00 and first byte is between 0x20 and 0x7E
	if ((char_code% 0x100 == 0)&&(char_code>=0x2000) && (char_code<0x7F00))
		return TRUE;
	else
		return FALSE;
}

/*******************************************************************************

 $Function:    	chinese_input

 $Description:	starts chinese editor
 
 $Returns:		Window handle

 $Arguments:	parebt window, editor attribute structure
 
*******************************************************************************/
T_MFW_HND chinese_input (T_MFW_HND parent_win, T_CHINESE_DATA* chinese_data)
{
    T_MFW_HND	win;
    win = chinese_input_create (parent_win);
    if(win NEQ NULL)
    {
        SEND_EVENT (win, CHINESE_INPUT_INIT, 0, chinese_data);
    }
    return win;
}


/*******************************************************************************

 $Function:    	chinese_input_create

 $Description:	Creates chinese editor window
 
 $Returns:		Dialogue info win

 $Arguments:	parent win
 
*******************************************************************************/
static T_MFW_HND chinese_input_create (T_MFW_HND parent_win)
{
    T_MFW_WIN     *  win_data;
    T_chinese_input  *  data = (T_chinese_input *)ALLOC_MEMORY (sizeof (T_chinese_input));

    TRACE_EVENT ("chinese_input_create()");

    data->input_win = win_create (parent_win, 0, E_WIN_VISIBLE, (T_MFW_CB)chinese_input_win_cb);
    if(data->input_win EQ 0)
        return 0;
  	/* Create window handler */
    data->mmi_control.dialog	= (T_DIALOG_FUNC)chinese_input_exec;
    data->mmi_control.data		= data;
    data->parent_win    		= parent_win;
    win_data             		= ((T_MFW_HDR *)data->input_win)->data;
    win_data->user        		= (void *)data;

    /*MC 15/07/02 create timer for pinyin multitap*/
    data->KeyPad_Timer_Handle	= tim_create(data->input_win, CHINESE_KEYPAD_TIME, (MfwCb)chinese_pinyin_timer_cb);
    memset( Counter, 0, sizeof(char)*KCD_STAR);/*MC set counters to 0*/
   
  	/*return window handle */
    return data->input_win;
}

/*******************************************************************************

 $Function:    	chinese_pinyin_timer_cb

 $Description:	Keypad timer callback
 
 $Returns:		int

 $Arguments:	Event mask and timer structure
 
*******************************************************************************/
static int chinese_pinyin_timer_cb (MfwEvt e, MfwTim *t)
{  
    int i;
    TRACE_EVENT("chinese_pinyin_timer_cb()");
  	for(i=0; i< KCD_STAR; i++)//MC, set all counters to 0
  		 Counter[i] = 0;
	return MFW_EVENT_CONSUMED;
}



/*******************************************************************************

 $Function:    	chinese_input_destroy

 $Description:	Destruction of an information dialog
 
 $Returns:		void

 $Arguments:	win
 
*******************************************************************************/
void chinese_input_destroy (T_MFW_HND own_window)
{
    T_MFW_WIN     * win  = ((T_MFW_HDR *)own_window)->data;
    T_chinese_input  * data = (T_chinese_input *)win->user;
    TRACE_EVENT ("chinese_input_destroy()");

	if (own_window == NULL)
	{
		TRACE_EVENT ("Error :- Called with NULL Pointer");
		return;
	}

    if(data)
  	{
    	/* Exit TIMER & KEYBOARD Handle */
        kbd_delete (data->input_kbd);
    	/* Delete WIN Handler */
        win_delete (data->input_win);
        /* Free Memory */
        /*MC 15/07/02, delete keyboard timer*/
        tim_delete(data->KeyPad_Timer_Handle);
        data->KeyPad_Timer_Handle= NULL;

		/* SPR#1428 - SH - New Editor changes */
#ifdef LSCREEN
#ifdef NEW_EDITOR
        if (data->editor)
        {
        	ATB_edit_Destroy(data->editor);
        }
#endif
#endif

        FREE_MEMORY ((void *)data, sizeof (T_chinese_input));
  	}
    else
    {
	    TRACE_EVENT ("chinese_input_destroy() - data already destroyed") ;
    }
}


/*******************************************************************************

 $Function:    	chinese_input

 $Description:	

 $Returns:		void

 $Arguments:	win, window handle event, value, parameter

*******************************************************************************/
void chinese_input_exec (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN  		* win_data   	= ((T_MFW_HDR *)win)->data;
    T_chinese_input	* data       	= (T_chinese_input *)win_data->user;
    T_CHINESE_DATA	* chinese_data	= (T_CHINESE_DATA *)parameter;
#ifdef NEW_EDITOR	/* SPR#1428 - SH - New Editor changes */
	T_AUI_EDIT_CB	Callback		= data->Callback_Data.Callback;
#else /* NEW_EDITOR */
  	T_EDIT_CB 	Callback   = data->Callback_Data.Callback;
 #endif /* NEW_EDITOR */
  	T_MFW_HND       parent_win = data->parent_win;
	USHORT          Identifier = data->Callback_Data.Identifier;

	TRACE_EVENT ("chinese_input_exec()");
    switch (event)
    {	
        case CHINESE_INPUT_INIT:
        	data->Callback_Data = *chinese_data;
        	TEXT_BUFFER_EDIT_SIZE = data->Callback_Data.EditorSize;
      	    /* Initialize dialog */
      	    /* Create timer and keyboard handler */
            data->input_kbd = kbd_create (win, KEY_ALL, (T_MFW_CB)chinese_input_kbd_cb);
	        /* Initialize all data structure and veriables to zero */
	        memset(&data->Ime, 0, sizeof(data->Ime));
	        memset(&data->GetParam, 0, sizeof(data->Ime));
	        memset(&data->wcCandidates, 0, sizeof(data->wcCandidates));
	        memset(&data->HighlightedCandidate, 0, sizeof(data->HighlightedCandidate));
	        memset(&data->CursorPosition, 0, sizeof(data->CursorPosition));
	        memset(&data->DisplayElementBuffer, 0,sizeof(data->DisplayElementBuffer));
			data->StartOfDisplayIndex	= 0;
            data->NumberOfKeyPress		= 0;
            data->PerviousKeyPress		= 0;/*a0393213 warnings removal - -1 changed to 0*/

            /* Now intitialize the Zi8 Core Engine */
            Zi8Initialize();
            /* Now initialize the IME */
            data->wszTextBuffer = (ZI8WCHAR*)&chinese_data->TextString[2]; //ignore first char (Unicode marker)
            IMEInitialize(&data->Ime, &data->GetParam, data->wszTextBuffer, TEXT_BUFFER_EDIT_SIZE,
                       sizeof(data->wszTextBuffer), data->wcCandidates, data->wszElementBuffer, data->DisplayElementBuffer);
            /* Store any other data */
    #ifdef LSCREEN /* SPR 1319 create an mfw editor to display editor content*/

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_SetAttr(&data->editor_attr, 0, COLOUR_EDITOR, EDITOR_FONT, ED_MODE_ALPHA, ED_CURSOR_BAR, ATB_DCS_UNICODE, (UBYTE *)chinese_data->TextString, TEXT_BUFFER_EDIT_SIZE+1); 
			data->editor_attr.win_size.sy = Mmi_layout_line_height()*8;/*Don't want to cover whole screen*/
			data->editor = ATB_edit_Create(&data->editor_attr, 0);
			ATB_edit_Init(data->editor);
			ATB_edit_Char(data->editor, ctrlBottom, TRUE);
#else /* NEW_EDITOR */
    		editor_attr_init(&data->editor_attr, 0, edtCurBar1, NULL, chinese_data->TextString,TEXT_BUFFER_EDIT_SIZE*2+2 , COLOUR_EDITOR );
            data->editor_attr.win.sy = Mmi_layout_line_height()*8;/*Don't want to cover whole screen*/
    		data->editor =edtCreate(data->input_win, &data->editor_attr, 0, 0);
    		edtChar(data->editor, ecBottom);
    		edtUnhide (data->editor);
#endif /* NEW_EDITOR */
    		editor_pointer = data->editor;
    #endif
            win_show (win);
            break;

            /*MC SPR 1752  same as E_ED_DESTROY*/
        case CHINESE_INPUT_DESTROY:
        	if (data)
	        {
	        		chinese_input_destroy(win);
	        }
        	break;

        /*MC SPR 1752, new event*/
        case CHINESE_INPUT_DEINIT:
        {/* on DEINIT => selfdestroy; sendevent <value> */
        	

  	
			//GW-SPR-#1068-After the callback, ensure 'data' is up-to-date	
		     if (Callback)
		      {	
	    	    	(Callback) (parent_win, Identifier, value);
	    	    	//Callback may have freed data - if so reinit pointer
		    		data = (T_chinese_input *)win_data->user;
		      }

	        if (data)
	        {
		        if (data->Callback_Data.DestroyEditor == TRUE)
		        {	
	        		chinese_input_destroy(win);
		        }
	        }
    	}
        	break;
       case ADD_SYMBOL:/*adding a symbol to chinese editor from symbol selection screen*/
       		if (data->Ime.TextBufferCount < TEXT_BUFFER_EDIT_SIZE)
       		{	
#ifndef LSCREEN

       			int i=0;
       			int j=0;
       			
	       		if(data->Ime.TextCursor < data->Ime.TextBufferCount) /* locate the text cursor position for correct character insertion */
		    	{/* shift and move characters to make room to insert the selected character */
		    		for (i=data->Ime.TextCursor; i<data->Ime.TextBufferCount; i++) 
		    		{
		    			data->Ime.pTextBuffer[data->Ime.TextBufferCount - j]=data->Ime.pTextBuffer[data->Ime.TextBufferCount - j - 1];
		    			j++;
		    		}
		    	}
		    	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				/* Now, value we get is already unicode */
				data->Ime.pTextBuffer[data->Ime.TextCursor++]= (ZI8WCHAR)((USHORT)value);
#else
       			/*Shift char up into Unicode from ASCII*/
       			data->Ime.pTextBuffer[data->Ime.TextCursor++]= (ZI8WCHAR)(SymbolChar << 8);
#endif
       			data->Ime.TextBufferCount++;/*increase char count by one*/
#else			/*SPR 1319*/

				/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				ATB_edit_Char(data->editor, (USHORT)value, TRUE);
#else /* NEW_EDITOR */
				edtChar(data->editor, SymbolChar<<8);/*pop the symbol in the editor*/
#endif /* NEW_EDITOR */
				data->Ime.TextBufferCount++;/*increase char count by one*/
#endif /* COLOURDISPLAY */
       		}
       		win_show(win);
       		break;
       	case LATIN_INPUT:/*return from Latin editor*/
       		/*update size of buffer*/
       		data->Ime.TextBufferCount = Uni_strlen(data->Ime.pTextBuffer);
       		data->Ime.TextCursor = data->Ime.TextBufferCount;/*move cursor to end*/
#ifdef LSCREEN
#ifdef NEW_EDITOR
			resources_setColour(COLOUR_EDITOR);
       		ATB_edit_Refresh(data->editor);
			ATB_edit_MoveCursor(data->editor, ctrlBottom, TRUE);
#endif
#endif
       		//win_show(data->input_win);
       		break;
    }
}


/*******************************************************************************

 $Function:    	chinese_input_win_cb

 $Description:	
 
 $Returns:		void

 $Arguments:	window handle event, win
*******************************************************************************/
static int chinese_input_win_cb (T_MFW_EVENT event, T_MFW_WIN * win)
{
    T_chinese_input	* data = (T_chinese_input *)win->user;
	int i = 0;
	
	PZI8WCHAR	Temp_ptr;
    TRACE_EVENT ("chinese_input_win_cb()");

    if(data EQ 0)
        return 1;
    switch (event)
    {
        case E_WIN_VISIBLE:
   	        if (win->flags & E_WIN_VISIBLE)
   	        {
   	        	/*
   	        	** Initialise the colours for the editor
   	        	*/
   	        	resources_setColour(COLOUR_EDITOR);

   	            /* Print the information dialog */
   	            /* Clear Screen */
   	            dspl_ClearAll();
   	            switch (data->Ime.InputScreenType)
   	            {
   	                case TEXT_DISPLAY_SCREEN:
   	                {

   	                	/* This section of the code was written to display the data stored in the text buffer. The text cursor position */
   	                	/* was also taken into consideration to display the correct portion of text during editing (when data stored in */
   	                	/* the text buffer is more than two line display.   */
						ZI8WCHAR TempArray[CANDIDATES_PER_GROUP*2]; //maximum no of chars possible in a line

                    	dspl_BitBlt(75,0,8,8,0,(char*)TextIconeZiTextSmall,0); 
                    
   	                	memset(TempArray, 0, CANDIDATES_PER_GROUP*2);
   	                	if(data->Ime.TextBufferCount == 0) //If text buffer is empty
   	                	{
   	                		displaySoftKeys(data->Callback_Data.LeftSoftKey,TxtSoftBack); 	/* Softkey display*/
   	                	}
   	                	else 

 #ifndef LSCREEN
 						{
 						/*a0393213 warnings removal-variable definition for TempArray2, i and j moved from outer scope*/
 						ZI8WCHAR TempArray2[CANDIDATES_PER_GROUP+1];
						int i;
						int j = 0;
 						if(data->Ime.TextBufferCount <= (CANDIDATES_PER_GROUP * 2)) // data in text buffer is within 2 lines
   	                	{	
   	                		
							for(i=0; i<CANDIDATES_PER_GROUP; i++)
								TempArray[i]= data->wszTextBuffer[data->StartOfDisplayIndex+ i];
							TempArray[CANDIDATES_PER_GROUP] = 0x0000;
							
   	                		dspl_TextOut(1,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char*)TempArray);
   	                		if(data->Ime.TextBufferCount > CANDIDATES_PER_GROUP)
   	                		{
   	                			for(i=0; i<CANDIDATES_PER_GROUP; i++)
								TempArray2[i]= data->wszTextBuffer[data->StartOfDisplayIndex+ i+CANDIDATES_PER_GROUP];
   	                			TempArray2[CANDIDATES_PER_GROUP] = 0x0000;
								dspl_TextOut(1,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE,(char *)TempArray2);
   	                		}
							j = (data->Ime.TextCursor / CANDIDATES_PER_GROUP) + 1;
							TextCursorDisplayContorl(&data->Ime, data->CursorPosition, j);
							displaySoftKeys(data->Callback_Data.LeftSoftKey,TxtDelete); 	/* Softkey display*/
   	                	}
   	                	else // data in text buffer is more than 14 characters. 
   	                	{
							if(data->Ime.TextCursor == data->StartOfDisplayIndex)
							{
								for(i=0; i<CANDIDATES_PER_GROUP; i++)
									TempArray[i]= data->wszTextBuffer[data->StartOfDisplayIndex+ i];
								TempArray[CANDIDATES_PER_GROUP] = 0x0000;
   	                			dspl_TextOut(1,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char *)TempArray);
   	                			for(i=0; i<CANDIDATES_PER_GROUP; i++)
   	                				TempArray2[i]= data->wszTextBuffer[data->StartOfDisplayIndex+ i +CANDIDATES_PER_GROUP];
   	                			TempArray2[CANDIDATES_PER_GROUP] = 0x0000;
   	                			dspl_TextOut(1,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE,(char *)TempArray2);
								TextCursorDisplayContorl(&data->Ime, data->CursorPosition, 1); 
							}
							else if(data->Ime.TextCursor > data->StartOfDisplayIndex)
							{
   	                			while(data->Ime.TextCursor - data->StartOfDisplayIndex >= CANDIDATES_PER_GROUP*2)
   	                			{
   	                				data->StartOfDisplayIndex = data->StartOfDisplayIndex + CANDIDATES_PER_GROUP;
   	                			}
   	                			for(i=0; i<CANDIDATES_PER_GROUP; i++)
   	                				TempArray[i]= data->wszTextBuffer[data->StartOfDisplayIndex+ i];
   	                			TempArray[CANDIDATES_PER_GROUP] = 0x0000;
   	                			dspl_TextOut(1,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char *)TempArray);
   	                			
   	                			for(i=0; i<CANDIDATES_PER_GROUP; i++)
   	                				TempArray2[i]= data->wszTextBuffer[data->StartOfDisplayIndex+ i +CANDIDATES_PER_GROUP];
   	                			TempArray2[CANDIDATES_PER_GROUP] = 0x0000;
   	                			dspl_TextOut(1,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE,(char *)TempArray2);
   	                			
   	                			if((data->Ime.TextCursor - data->StartOfDisplayIndex) < CANDIDATES_PER_GROUP)
									j = 1;
								else if((data->Ime.TextCursor - data->StartOfDisplayIndex) < CANDIDATES_PER_GROUP * 2)
									j = 2;
								TextCursorDisplayContorl(&data->Ime, data->CursorPosition, j);
							}
							else if(data->Ime.TextCursor < data->StartOfDisplayIndex && data->StartOfDisplayIndex != 0)
							{
								data->StartOfDisplayIndex = data->StartOfDisplayIndex - CANDIDATES_PER_GROUP;
								for(i=0; i<CANDIDATES_PER_GROUP; i++)
   	                				TempArray[i]= data->wszTextBuffer[data->StartOfDisplayIndex+ i ];
								TempArray[CANDIDATES_PER_GROUP] = 0x0000;
   	                			dspl_TextOut(1,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char *)TempArray);
   	                			for(i=0; i<CANDIDATES_PER_GROUP; i++)
   	                				TempArray2[i]= data->wszTextBuffer[data->StartOfDisplayIndex+ i +CANDIDATES_PER_GROUP];
   	                			TempArray2[CANDIDATES_PER_GROUP] = 0x0000;
   	                			dspl_TextOut(1,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE,(char *)TempArray2);
   	                			if((data->Ime.TextCursor - data->StartOfDisplayIndex) < CANDIDATES_PER_GROUP)
									j = 1;
								else if((data->Ime.TextCursor - data->StartOfDisplayIndex) < CANDIDATES_PER_GROUP * 2)
									j = 2;
								TextCursorDisplayContorl(&data->Ime, data->CursorPosition, j);
							}	
   	                	

							displaySoftKeys(data->Callback_Data.LeftSoftKey,TxtDelete); 	/* Softkey display*/
   	                }
 	}
 #else					/*SPR 1319, show editor and softkeys*/
 /* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
						ATB_edit_Unhide(data->editor);
						ATB_edit_Show(data->editor);
#else /* NEW_EDITOR */
 						edtUnhide(data->editor);
						edtShow(data->editor);
#endif /* NEW_EDITOR */
 						displaySoftKeys(data->Callback_Data.LeftSoftKey,TxtDelete); 
#endif
   	                	}
   	                    break;
                    case STROKE_INPUT_SCREEN:
#ifndef LSCREEN
       			        /* Display the correct portion composed text stored in wszTextBuffer. If the text cursor is not pointing at */
       			        /* the end of the text buffer, the display content is then adjusted to display the correct portion of the   */
       			        /* text buffer.																				  */
       			        if(data->Ime.TextBufferCount <=CANDIDATES_PER_GROUP)  //check text buffer count
   	                	{
   	                		if(data->Ime.TextBufferCount == data->Ime.TextCursor)
   	                		{
			    				dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char*)data->wszTextBuffer);
   	                		}
   	                		else
   	                		{
   	                			dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char*)data->wszTextBuffer);
   	                			TextCursorDisplayContorl(&data->Ime, data->CursorPosition, 0);
   	                		}
       			        }
       			        else //when data->Ime.TextBufferCount > CANDIDATES_PER_GROUP
       			        {
							if(data->Ime.TextBufferCount == data->Ime.TextCursor) 
	   	                	{
								Temp_ptr = data->wszTextBuffer;
	   	                		Temp_ptr = Temp_ptr + data->Ime.TextBufferCount - CANDIDATES_PER_GROUP;
	   	                		dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char *)Temp_ptr);
	   	                	}
							else if(data->Ime.TextCursor < CANDIDATES_PER_GROUP)
							{
								dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char*)data->wszTextBuffer);
   	                			TextCursorDisplayContorl(&data->Ime, data->CursorPosition, 0);
							}
							else
							{
	   	                		Temp_ptr = &data->wszTextBuffer[data->Ime.TextCursor] - CANDIDATES_PER_GROUP + 1;
	   	                		dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char *)Temp_ptr);
	   	                		dspl_TextOut(73,0,DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)data->CursorPosition);
	   	                	}
       			        }
#else					/*SPR 1319, show editor*/
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
						ATB_edit_Unhide(data->editor);
						ATB_edit_Show(data->editor);
#else /* NEW_EDITOR */
 						edtUnhide(data->editor);
						edtShow(data->editor);
#endif /* NEW_EDITOR */
#endif
#ifndef LSCREEN
			    		dspl_TextOut(0,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char*)StrokePrompt);/* Display input prompt */
#else
						/*SPR 1319 show stroke prompt*/
						dspl_TextOut(0,SCREEN_SIZE_Y-Mmi_layout_line(4),DSPL_TXTATTR_UNICODE,(char*)StrokePrompt);/* Display input prompt */
#endif
			    		/* Display input sequence for stroke */
			    		i = Uni_strlen(data->DisplayElementBuffer);
			    		if( i <= 4)
			    		{
#ifndef LSCREEN
			    			dspl_TextOut(CHI_FONT_WIDTH*3,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE,(char*)data->DisplayElementBuffer);//data->wszElementBuffer);
#else
						/*mc SPR1319*/
							dspl_TextOut(CHI_FONT_WIDTH*3,SCREEN_SIZE_Y- Mmi_layout_line(4),DSPL_TXTATTR_UNICODE,(char*)data->DisplayElementBuffer);//data->wszElementBuffer);
#endif
			    		}
			    		else
			    		{
			    			Temp_ptr = data->DisplayElementBuffer;
			    			Temp_ptr = Temp_ptr + i - 4;
#ifndef LSCREEN
			    			dspl_TextOut(CHI_FONT_WIDTH*3,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char*)Temp_ptr);//data->wszElementBuffer);
#else
							/*mc SPR1319*/
							dspl_TextOut(CHI_FONT_WIDTH*3,SCREEN_SIZE_Y-Mmi_layout_line(4),DSPL_TXTATTR_UNICODE,(char*)Temp_ptr);//data->wszElementBuffer);
#endif
			    		}

#ifndef LSCREEN
			    		/* Display candidate characters returned from eZiText */
			    		dspl_TextOut(1,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE,(char*)data->wcCandidates);
#else
						/*mc SPR1319*/
						dspl_TextOut(1,SCREEN_SIZE_Y-Mmi_layout_line(3),DSPL_TXTATTR_UNICODE,(char*)data->wcCandidates);
#endif

			    		/* Highlight the candidate character pointed by Ime.CandidateCursor */
			    		CandidateHighlightDisplayContorl(&data->Ime, data->HighlightedCandidate);
			    		if (Uni_strlen(data->DisplayElementBuffer) > 0)
			    			displaySoftKeys(TxtSoftSelect,TxtDelete);	/* Softkey display*/
			    		else
			    			displaySoftKeys(TxtSoftSelect,TxtSoftOK);	/* Softkey display*/
			    	
       					break;
       				case PINYIN_INPUT_SCREEN:
       					/* Display the correct portion composed text stored in wszTextBuffer. If the text cursor is not pointing at */
       			        /* the end of the text buffer, the display content is then adjusted to display the correct portion of the   */
       			        /* text buffer.	
       			      	*/
#ifndef LSCREEN
       			        if(data->Ime.TextBufferCount <=CANDIDATES_PER_GROUP)  //check text buffer count
   	                	{
   	                		if(data->Ime.TextBufferCount == data->Ime.TextCursor)
   	                		{
			    				dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char*)data->wszTextBuffer);
   	                		}
   	                		else
   	                		{
   	                			dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char*)data->wszTextBuffer);
   	                			TextCursorDisplayContorl(&data->Ime, data->CursorPosition, 0);
   	                		}
       			        }
       			        else //when data->Ime.TextBufferCount > CANDIDATES_PER_GROUP
       			        {
							if(data->Ime.TextBufferCount == data->Ime.TextCursor) 
	   	                	{
								Temp_ptr = data->wszTextBuffer;
	   	                		Temp_ptr = Temp_ptr + data->Ime.TextBufferCount - CANDIDATES_PER_GROUP;
	   	                		dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char *)Temp_ptr);
	   	                	}
							else if(data->Ime.TextCursor < CANDIDATES_PER_GROUP)
							{
								dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char*)data->wszTextBuffer);
   	                			TextCursorDisplayContorl(&data->Ime, data->CursorPosition, 0);
							}
							else
							{
	   	                		Temp_ptr = &data->wszTextBuffer[data->Ime.TextCursor] - CANDIDATES_PER_GROUP + 1;
	   	                		dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char *)Temp_ptr);
	   	                		dspl_TextOut(73,0,DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)data->CursorPosition);
	   	                	}
       			        }
#else
						/*mc SPR1319*/
						/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
						ATB_edit_Unhide(data->editor);
						ATB_edit_Show(data->editor);
#else /* NEW_EDITOR */
 						edtUnhide(data->editor);
						edtShow(data->editor);
#endif /* NEW_EDITOR */
#endif
#ifndef LSCREEN
       					dspl_TextOut(0,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char*)PinYinPrompt);/* Display input prompt */
#else
						/*mc SPR1319*/
						dspl_TextOut(0,SCREEN_SIZE_Y - Mmi_layout_line(4),DSPL_TXTATTR_UNICODE,(char*)PinYinPrompt);/* Display input prompt */
#endif
       					/* Display input sequence for Pin-yin input */
			    		i = Uni_strlen(data->DisplayElementBuffer);
			    		if( i <= 4)
			    		{
#ifndef LSCREEN
			    			dspl_TextOut(CHI_FONT_WIDTH*3,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char*)data->DisplayElementBuffer);//data->wszElementBuffer);
#else
							/*mc SPR1319*/
			    			dspl_TextOut(CHI_FONT_WIDTH*3,SCREEN_SIZE_Y-Mmi_layout_line(4),DSPL_TXTATTR_UNICODE,(char*)data->DisplayElementBuffer);//data->wszElementBuffer);
#endif
			    		}
			    		else
			    		{
			    			Temp_ptr = data->DisplayElementBuffer;
			    			Temp_ptr = Temp_ptr + i - 4;
#ifndef LSCREEN
			    			dspl_TextOut(CHI_FONT_WIDTH*3,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char*)Temp_ptr);//data->wszElementBuffer);
#else
							/*mc SPR1319*/
							dspl_TextOut(CHI_FONT_WIDTH*3,SCREEN_SIZE_Y -Mmi_layout_line(4),DSPL_TXTATTR_UNICODE,(char*)Temp_ptr);//data->wszElementBuffer);
#endif
			    		}
			    		/* Display candidate characters returned from eZiText */
#ifndef LSCREEN
			    		dspl_TextOut(1,Mmi_layout_second_line(),DSPL_TXTATTR_UNICODE,(char*)data->wcCandidates);
#else
						/*mc SPR1319*/
						dspl_TextOut(1,SCREEN_SIZE_Y-Mmi_layout_line(3),DSPL_TXTATTR_UNICODE,(char*)data->wcCandidates);
#endif
			    		/* Highlight the candidate character pointed by Ime.CandidateCursor */
			    		CandidateHighlightDisplayContorl(&data->Ime, data->HighlightedCandidate);
			    		/*MC should have "Delete" softkey when entering pinyin letters*/
			    		if (Uni_strlen(data->DisplayElementBuffer) > 0)
			    			displaySoftKeys(TxtSoftSelect,TxtDelete);	/* Softkey display*/
			    		else
			    			displaySoftKeys(TxtSoftSelect,TxtSoftOK);	/* Softkey display*/
       					break;
       				case WORD_ASSOCIATION_SCREEN:
       					/* Display the correct portion composed text stored in wszTextBuffer. If the text cursor is not pointing at */
       			        /* the end of the text buffer, the display content is then adjusted to display the correct portion of the   */
       			        /* text buffer.	
       			        */
#ifndef LSCREEN
       			        if(data->Ime.TextBufferCount <=CANDIDATES_PER_GROUP)  //check text buffer count
   	                	{
   	                		if(data->Ime.TextBufferCount == data->Ime.TextCursor)
   	                		{
			    				dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char*)data->wszTextBuffer);
   	                		}
   	                		else
   	                		{
   	                			dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char*)data->wszTextBuffer);
   	                			TextCursorDisplayContorl(&data->Ime, data->CursorPosition, 0);
   	                		}
       			        }
       			        else //when data->Ime.TextBufferCount > CANDIDATES_PER_GROUP
       			        {
							if(data->Ime.TextBufferCount == data->Ime.TextCursor) 
	   	                	{
								Temp_ptr = data->wszTextBuffer;
	   	                		Temp_ptr = Temp_ptr + data->Ime.TextBufferCount - CANDIDATES_PER_GROUP;
	   	                		dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char *)Temp_ptr);
	   	                	}
							else if(data->Ime.TextCursor < CANDIDATES_PER_GROUP)
							{
								dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char*)data->wszTextBuffer);
   	                			TextCursorDisplayContorl(&data->Ime, data->CursorPosition, 0);
							}
							else
							{
	   	                		Temp_ptr = &data->wszTextBuffer[data->Ime.TextCursor] - CANDIDATES_PER_GROUP + 1;
	   	                		dspl_TextOut(1,0,DSPL_TXTATTR_UNICODE,(char *)Temp_ptr);
	   	                		dspl_TextOut(73,0,DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)data->CursorPosition);
	   	                	}
       			        }
#else
						/*mc SPR1319*/
						/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
						ATB_edit_Unhide(data->editor);
						ATB_edit_Show(data->editor);
#else /* NEW_EDITOR */
 						edtUnhide(data->editor);
						edtShow(data->editor);
#endif /* NEW_EDITOR */
#endif
       					if(data->Ime.EditorMode == STROKE_INPUT_MODE)
       					{
#ifndef LSCREEN
			    			dspl_TextOut(0,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char*)StrokePrompt);	/* Display input prompt */
			    			/* Display input sequence for stroke */
#else
							/*mc SPR1319*/
							dspl_TextOut(0,SCREEN_SIZE_Y -Mmi_layout_line(4),DSPL_TXTATTR_UNICODE,(char*)StrokePrompt);	/* Display input prompt */
			    			/* Display input sequence for stroke */

#endif
       					}
       					else if(data->Ime.EditorMode == PINYIN_INPUT_MODE)
       					{	
#ifndef LSCREEN
       						dspl_TextOut(0,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE,(char*)PinYinPrompt);	/* Display input prompt */
       						/* Display input sequence for Pin-yin input */
#else
		      				dspl_TextOut(0,SCREEN_SIZE_Y-Mmi_layout_line(4),DSPL_TXTATTR_UNICODE,(char*)PinYinPrompt);	/* Display input prompt */
       						/* Display input sequence for Pin-yin input */
#endif
       					}
#ifndef LSCREEN
			    		/* Display candidate charaters returned from eZiText */
			    		dspl_TextOut(1,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE,(char*)data->wcCandidates);
#else

						dspl_TextOut(1,SCREEN_SIZE_Y-Mmi_layout_line(3),DSPL_TXTATTR_UNICODE,(char*)data->wcCandidates);
#endif
			    		/* Highlight the candidate character pointed by Ime.CandidateCursor */
						CandidateHighlightDisplayContorl(&data->Ime, data->HighlightedCandidate);

						displaySoftKeys(TxtSoftSelect,TxtSoftOK);	/* Softkey display*/
       					break;
	    			default:
	       				break;
   	            	}
			}
   	        
	    	break;
		default:
	   	return 0;
    	}
   
    return 1;
}


/*******************************************************************************

 $Function:    	chinese_input_kbd_cb

 $Description:	Keyboard event handler

 $Returns:		status int

 $Arguments:	window handle event, keyboard control block

*******************************************************************************/

static int chinese_input_kbd_cb (T_MFW_EVENT event, T_MFW_KBD *keyboard)
{
  	T_MFW_HND       	win  		= mfw_parent (mfw_header());
  	T_MFW_WIN    	*	win_data	= ((T_MFW_HDR *)win)->data;
  	T_chinese_input	* 	data 		= (T_chinese_input *)win_data->user;

  //  	void (* Callback)() = data->Callback;

  	TRACE_EVENT("chinese_input_kbd_cb");
/***************************Go-lite Optimization changes Start***********************/
//Aug 16, 2004    REF: CRR 24323   Deepa M.D
TRACE_EVENT_P6("Counters %d:%d:%d:%d:%d:%d",  Counter[0],  Counter[1],  Counter[2],  Counter[3],
	 Counter[4],  Counter[5]);
/***************************Go-lite Optimization changes end***********************/
  	switch (keyboard->code)
  	{
    	case KCD_LEFT:
    		switch(data->Ime.EditorMode)
    		{
    			case STROKE_INPUT_MODE:
		    		switch (data->Ime.InputScreenType)
					{
		    			case TEXT_DISPLAY_SCREEN:
		    				//access to the option list.
						SEND_EVENT(win, CHINESE_INPUT_DEINIT/*SPR 1752*/, INFO_KCD_LEFT, NULL);
		       				break;
		       			case STROKE_INPUT_SCREEN:
		       			case WORD_ASSOCIATION_SCREEN:
		       				IMESelectCandidate(&data->Ime, data->wszElementBuffer);
		       				data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
		       				break;
			    		default:
			       			break;
		    		}
		    		winShow(win);
		    		break;
		    	
		    	case PINYIN_INPUT_MODE:
		    		switch (data->Ime.InputScreenType)
					{
		    			case TEXT_DISPLAY_SCREEN:
		    				SEND_EVENT(win, CHINESE_INPUT_DEINIT/*SPR 1752*/, INFO_KCD_LEFT, NULL);
		    				//access to the option list.
		       				break;
		       			case PINYIN_INPUT_SCREEN:
		       			case WORD_ASSOCIATION_SCREEN:
		       				memset( Counter, 0, sizeof(char)*KCD_STAR);/*MC set counters to 0*/
		       				IMESelectCandidate(&data->Ime, data->wszElementBuffer);
		       				data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];

		       				break;
			    		default:
			       			break;
		    		}
		    		winShow(win);
		    	default:
			       	break;
    		}

       		break;
	    case KCD_RIGHT:
	    	switch(data->Ime.EditorMode)
    		{
    			case STROKE_INPUT_MODE:
			    	switch (data->Ime.InputScreenType)
					{
		    			case TEXT_DISPLAY_SCREEN:
		    				if(data->Ime.TextBufferCount == 0)
							{	/* When there is no stored data, return back to the menu*/
								//GW-SPR-#1068-Destroy using event - execute callback of the function that created the editor	
								SEND_EVENT(win, CHINESE_INPUT_DEINIT/*SPR 1752*/, INFO_KCD_RIGHT, NULL);
							}
		    				else
		    				{
		    					IMEClear(&data->Ime, data->wszElementBuffer);
		    				}
		       				break;
		       			case STROKE_INPUT_SCREEN:
		       				IMEClear(&data->Ime, data->wszElementBuffer);
		       				data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
		       				break;
		       			case WORD_ASSOCIATION_SCREEN:
		       				data->Ime.InputScreenType = TEXT_DISPLAY_SCREEN;
		       				break;
			    		default:
			       			break;
			  		}
			    	winShow(win);
			    	break;
		    	case PINYIN_INPUT_MODE:
		    		switch (data->Ime.InputScreenType)
					{
		    			case TEXT_DISPLAY_SCREEN:
		    				if(data->Ime.TextBufferCount == 0)
							{	/* When there is no stored data, return back to the menu*/
								//GW-SPR-#1068-Destroy using event - execute callback of the function that created the editor	
								SEND_EVENT(win, CHINESE_INPUT_DEINIT/*SPR 1752*/, INFO_KCD_RIGHT, NULL);
							}
		    				else
		    				{
		    					IMEClear(&data->Ime, data->wszElementBuffer);
		    					data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
		    				}
		       				break;
		       			case PINYIN_INPUT_SCREEN:
		       				memset( Counter, 0, sizeof(char)*KCD_STAR);/*MC set counters to 0*/
		       				IMEClear(&data->Ime, data->wszElementBuffer);
		       				data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
		       				break;
		       			case WORD_ASSOCIATION_SCREEN:
		       				data->Ime.InputScreenType = TEXT_DISPLAY_SCREEN;
		       				break;
			    		default:
			       			break;
		    		}
		    		winShow(win);
		    		break;
	
		    	default:
			       	break;
    		}	
	       	break;
	    case KCD_HUP:
			//GW-SPR-#1068-Destroy using event - execute callback of the function that created the editor	
			SEND_EVENT(win, CHINESE_INPUT_DEINIT/*SPR 1752*/, INFO_KCD_HUP, NULL);
	       	break;
	    case KCD_MNUUP:
	    	switch(data->Ime.EditorMode)
    		{
    			case STROKE_INPUT_MODE:
			    	switch (data->Ime.InputScreenType)
					{
		    			case TEXT_DISPLAY_SCREEN:
		    				//update the text cursor
		    				if(data->Ime.TextCursor > 0 && data->Ime.TextCursor <= data->Ime.TextBufferCount)
		    				{
		    					data->CursorPosition[0] = data->wszTextBuffer[data->Ime.TextCursor - 1];
		    					data->Ime.TextCursor--;
		    				}
		    #ifdef LSCREEN /*SPR 1319 move left along editor*/
		    /* SPR#1428 - SH - New Editor changes */
		 	#ifdef NEW_EDITOR
		 					ATB_edit_MoveCursor(data->editor, ctrlLeft, TRUE);
		 	#else /* NEW_EDITOR */
		    				edtChar(data->editor, ecLeft);
		    #endif /* NEW_EDITOR */
		    #endif
		    				break;
		       			case STROKE_INPUT_SCREEN:
		       			case WORD_ASSOCIATION_SCREEN:
		       				IMECursorLeft(&data->Ime);
							data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
		       				break;
			    		default:
			       			break;
			  		}
			    	winShow(win);
			        break;
		
			  	case PINYIN_INPUT_MODE:
			  		switch (data->Ime.InputScreenType)
					{
		    			case TEXT_DISPLAY_SCREEN:
		    				//update the text cursor
		    				if(data->Ime.TextCursor > 0 && data->Ime.TextCursor <= data->Ime.TextBufferCount)
		    				{
		    					data->CursorPosition[0] = data->wszTextBuffer[data->Ime.TextCursor - 1];
		    					data->Ime.TextCursor--;
	   
		    				}
		#ifdef LSCREEN/*SPR 1319 move left along editor*/
		/* SPR#1428 - SH - New Editor changes */
		#ifdef NEW_EDITOR
							ATB_edit_MoveCursor(data->editor, ctrlLeft, TRUE);
		#else /* NEW_EDITOR */
		    				edtCharUnicode(data->editor, ecLeft);
		#endif /* NEW_EDITOR */
		#endif
		       				break;
		       			case PINYIN_INPUT_SCREEN:
		       			case WORD_ASSOCIATION_SCREEN:
		       				memset( Counter, 0, sizeof(char)*KCD_STAR);/*MC set counters to 0*/
		       				IMECursorLeft(&data->Ime);
							data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
		       				break;
			    		default:
			       			break;
		    		}
			  		winShow(win);
		    		break;
		    	default:
			       	break;
    		}	
	       	break;
	    case KCD_MNUDOWN:
	    	switch(data->Ime.EditorMode)
    		{
    			case STROKE_INPUT_MODE:
					switch (data->Ime.InputScreenType)
					{
		    			case TEXT_DISPLAY_SCREEN:
		    				//update the text cursor 
		    				if(data->Ime.TextCursor < data->Ime.TextBufferCount)
		    				{
		    					data->Ime.TextCursor++;
		    					data->CursorPosition[0] = data->wszTextBuffer[data->Ime.TextCursor];
		    					
		    				}
#ifdef LSCREEN/*SPR 1319 move right along editor*/
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
							ATB_edit_MoveCursor(data->editor, ctrlRight, TRUE);
#else /* NEW_EDITOR */
		    				edtCharUnicode(data->editor, ecRight);
#endif /* NEW_EDITOR */
#endif
		    				break;
		       			case STROKE_INPUT_SCREEN:
		       			case WORD_ASSOCIATION_SCREEN:
		       				IMECursorRight(&data->Ime);
			    			data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
		       				break;
			    		default:
			       			break;
			  		}
			    	winShow(win);
			        break;
			  
	        	case PINYIN_INPUT_MODE:
	        		switch (data->Ime.InputScreenType)
					{
		    			case TEXT_DISPLAY_SCREEN:
		    				//update the text cursor 
		    				if(data->Ime.TextCursor < data->Ime.TextBufferCount)
		    				{
		    					data->Ime.TextCursor++;
		    					data->CursorPosition[0] = data->wszTextBuffer[data->Ime.TextCursor];		    					
		    				}
#ifdef LSCREEN	/*SPR 1319 move right  along editor*/
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
							ATB_edit_MoveCursor(data->editor, ctrlRight, TRUE);
#else /* NEW_EDITOR */
		    				edtCharUnicode(data->editor, ecRight);
#endif /* NEW_EDITOR */
#endif
		       				break;
		       			case PINYIN_INPUT_SCREEN:
		       			case WORD_ASSOCIATION_SCREEN:
		       				memset( Counter, 0, sizeof(char)*KCD_STAR);/*MC set counters to 0*/
		       				IMECursorRight(&data->Ime);
			    			data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
		       				break;
			    		default:
			       			break;
		    		}
	        		winShow(win);
		    		break;
		    	default:
			       	break;
    		}	
	       	break;
	    case KCD_CALL:
			break;
		case KCD_0:
			if (data->Ime.TextBufferCount < TEXT_BUFFER_EDIT_SIZE)/*MC*/
			{
				switch(data->Ime.EditorMode)
	    		{
	    			case STROKE_INPUT_MODE:
		        		data->Ime.InputScreenType = STROKE_INPUT_SCREEN;
						IMEMore(&data->Ime);
						data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
						winShow(win);
			    		break;
			    	case PINYIN_INPUT_MODE:
			    		memset( Counter, 0, sizeof(char)*KCD_STAR);/*MC set counters to 0*/
			    		data->Ime.InputScreenType = PINYIN_INPUT_SCREEN;
			    		IMEMore(&data->Ime);
						data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
						winShow(win);
			    		break;
			 
			    	default:
				       	break;
	    		}
			}
	       	break;
		case KCD_8:
			if (data->Ime.TextBufferCount < TEXT_BUFFER_EDIT_SIZE)/*MC*/
			{
				switch(data->Ime.EditorMode)
	    		{
	    			case STROKE_INPUT_MODE:
		        		data->Ime.InputScreenType = STROKE_INPUT_SCREEN;
						IMEMore(&data->Ime);
						data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
						winShow(win);
			    		break;
			    	case PINYIN_INPUT_MODE:
						data->Ime.InputScreenType = PINYIN_INPUT_SCREEN;
						/* The unclear of eZiText document, it is assumed that multi-key press may required to get to the required letter */
						/* To do that a ZiPinYinKeyMap array is defined in ChineseInput.h header file.                                 */
						/* When number of keypress from the user is given from the keypad hadler, the key press count can then be    */
						/* used to work out the relevent code to insert into the element buffer for eZiText search. The variable          */
						/* data->NumberOfKeyPress is decleared to store the key press count. 								  */

						//temp = data->NumberOfKeyPress % ZiPinYinKeyMap[(keyboard->code)-1][5];
						//IMEKeyPress(&data->Ime, (ZI8WCHAR) ZiPinYinKeyMap[(keyboard->code)-1][temp - 1]);

						/* due to not able to receive key press count, it is assume the first letter assign to the key is selected when key */
						/* is pressed.	*/
						/*MC, multitap handling*/
						/*if time out or new key pressed*/
						/***************************Go-lite Optimization changes Start***********************/
						//Aug 16, 2004    REF: CRR 24323   Deepa M.D
						TRACE_EVENT_P1("Keypad Counter : %d",  Counter[keyboard->code]);
						/***************************Go-lite Optimization changes end***********************/
						if ( Counter[keyboard->code] == 0)
						{	tim_stop(data->KeyPad_Timer_Handle);/*stop timer*/
							memset( Counter, 0, sizeof(char)*KCD_STAR);/*reset all key counters*/
							IMEKeyPress(&data->Ime, (ZI8WCHAR) ZiPinYinKeyMap[(keyboard->code)-1][0]);
							 Counter[keyboard->code]++; //increment counter
							tim_start(data->KeyPad_Timer_Handle);/*restart timer*/
						}
						else /*if counter is less than MAX for that key*/
						{	ZI8WCHAR last_element; 

							if ( Counter[keyboard->code] <ZiPinYinKeyMap[(keyboard->code)-1][5])
							{	tim_stop(data->KeyPad_Timer_Handle);/*stop timer*/
								//if last element is the previous char for this key
								last_element = data->Ime.pGetParam->pElements[data->Ime.pGetParam->ElementCount -1];
								//should put in check
								if (last_element== ZiPinYinKeyMap[(keyboard->code)-1][ Counter[keyboard->code]-1])
								{	//MC e.g. if we're trying to enter 'I' and the last char was 'H'
									IMEClear(&data->Ime, data->Ime.pGetParam->pElements);/*remove last char from elements*/
									data->Ime.InputScreenType = PINYIN_INPUT_SCREEN;/*this prevents display problems*/
								}
								/*so we can add the next char for this key*/
								IMEKeyPress(&data->Ime, (ZI8WCHAR) ZiPinYinKeyMap[(keyboard->code)-1][ Counter[keyboard->code]]);
								 Counter[keyboard->code]++; //increment counter
								tim_start(data->KeyPad_Timer_Handle);/*restart timer*/
							}
						}
						
						data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
						winShow(win);
			    		break;
			    	default:
				       	break;
	    		}
			}
	       	break;
		case KCD_1:
		case KCD_2:
		case KCD_3:
		case KCD_4:
		case KCD_5:
		case KCD_6:
		case KCD_7:
		case KCD_9:	
			switch(data->Ime.EditorMode)
    		{
    			case STROKE_INPUT_MODE:
    				if (data->Ime.TextBufferCount < TEXT_BUFFER_EDIT_SIZE)/*MC*/
					{	data->Ime.InputScreenType = STROKE_INPUT_SCREEN;
						IMEKeyPress(&data->Ime, (ZI8WCHAR) ZiStrokeKeyMap[keyboard->code][1] + ZI8_BASE_STROKES);
						data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
						winShow(win);
    				}
					break;
					
				case PINYIN_INPUT_MODE:
					if (data->Ime.TextBufferCount < TEXT_BUFFER_EDIT_SIZE)/*MC*/
					{
						data->Ime.InputScreenType = PINYIN_INPUT_SCREEN;
						/*MC, multitap handling*/
						/***************************Go-lite Optimization changes Start***********************/
						//Aug 16, 2004    REF: CRR 24323   Deepa M.D
						TRACE_EVENT_P1("Keypad Counter : %d",  Counter[keyboard->code]);
						/***************************Go-lite Optimization changes end***********************/
						/*if time out or new key pressed*/		
						if ( Counter[keyboard->code] == 0)
						{	tim_stop(data->KeyPad_Timer_Handle);/*stop timer*/
							memset( Counter, 0, sizeof(char)*KCD_STAR);/*reset all key counters*/
							IMEKeyPress(&data->Ime, (ZI8WCHAR) ZiPinYinKeyMap[(keyboard->code)-1][0]);
							 Counter[keyboard->code]++; //increment counter
							tim_start(data->KeyPad_Timer_Handle);/*restart timer*/
						}
						else /*if counter is less than MAX for that key*/
						{	ZI8WCHAR last_element; 

							if ( Counter[keyboard->code] <ZiPinYinKeyMap[(keyboard->code)-1][5])
							{	tim_stop(data->KeyPad_Timer_Handle);/*stop timer*/
								last_element = data->Ime.pGetParam->pElements[data->Ime.pGetParam->ElementCount -1];
								//should put in check
								//e.g. if last char was 'a' and we're entering 'b'
								if (last_element== ZiPinYinKeyMap[(keyboard->code)-1][ Counter[keyboard->code]-1])
								{	IMEClear(&data->Ime, data->Ime.pGetParam->pElements);/*remove last char from elements*/
									data->Ime.InputScreenType = PINYIN_INPUT_SCREEN;/*this prevents display problems*/
								}
								/*so we can add the next char for this key*/
								IMEKeyPress(&data->Ime, (ZI8WCHAR) ZiPinYinKeyMap[(keyboard->code)-1][ Counter[keyboard->code]]);
								 Counter[keyboard->code]++; //increment counter
								tim_start(data->KeyPad_Timer_Handle);/*restart timer*/
							}
						}
						//IMEKeyPress(&data->Ime, (ZI8WCHAR) ZiPinYinKeyMap[(keyboard->code)-1][/*0*/2]);
						data->HighlightedCandidate[0] = data->wcCandidates[data->Ime.CandidateCursor];
						winShow(win);
						}
					break;
		
		    	default:
			       	break;
			}
			break;
		case KCD_STAR:
			if (data->Ime.TextBufferCount < TEXT_BUFFER_EDIT_SIZE)/*MC*/
			{
				switch(data->Ime.EditorMode)
	    		{
	    			case STROKE_INPUT_MODE:
	    			/* SPR#1428 - SH - New Editor uses new symbol selector */
#ifdef NEW_EDITOR
						AUI_symbol_Start(win, ADD_SYMBOL);
#else /* NEW_EDITOR */
	    				SymbolScreenstart(win, NULL);
#endif /* NEW_EDITOR */
						break;
					case PINYIN_INPUT_MODE:
						memset( Counter, 0, sizeof(char)*KCD_STAR);/*MC set counters to 0*/
					/* SPR#1428 - SH - New Editor uses new symbol selector */
#ifdef NEW_EDITOR
						AUI_symbol_Start(win, ADD_SYMBOL);
#else /* NEW_EDITOR */
	    				SymbolScreenstart(win, NULL);
#endif /* NEW_EDITOR */
						break;
			    	default:
				       	break;
				}
			}
			
			break;
	    case KCD_HASH:
	    if (data->Ime.TextBufferCount < TEXT_BUFFER_EDIT_SIZE)/*MC*/
		{
	    	switch(data->Ime.EditorMode)
    		{
    		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
				T_AUI_EDITOR_DATA	editor_data;
#else /* NEW_EDITOR */
    			T_EDITOR_DATA  editor_data;
#endif /* NEW_EDITOR */
    			case STROKE_INPUT_MODE:
    				data->Ime.EditorMode = PINYIN_INPUT_MODE;
					break;
				case PINYIN_INPUT_MODE:
					memset( Counter, 0, sizeof(char)*KCD_STAR);/*MC set counters to 0*/
					/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					AUI_edit_SetDefault(&editor_data);
#else /* NEW_EDITOR */
					editor_attr_init(&editor_data.editor_attr, NULL, edtCurBar1, 0, 0, 0, 0);
					editor_data.hide									= FALSE;
					editor_data.TextId               = '\0';  
					editor_data.TextString           = NULL;  
					editor_data.timeout			  = FOREVER;  // Avoid to return empty strings
#endif /* NEW_EDITOR */			
					//set mode to stroke input when Latin editor exited
					data->Ime.EditorMode = STROKE_INPUT_MODE;
					/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
					AUI_edit_SetDisplay(&editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR, EDITOR_FONT);
					AUI_edit_SetBuffer(&editor_data, ATB_DCS_UNICODE, (UBYTE *)data->Callback_Data.TextString, data->Callback_Data.EditorSize*2 + 3);
					AUI_edit_SetMode(&editor_data, ED_MODE_ALPHA, ED_CURSOR_BAR);
					AUI_edit_SetTextStr(&editor_data, TxtChinese, TxtDelete, TxtNull, NULL);
					AUI_edit_SetEvents(&editor_data, LATIN_INPUT, TRUE, FOREVER, (T_AUI_EDIT_CB)chinese_input_exec);
					AUI_edit_Start(data->input_win, &editor_data);
#else /* NEW_EDITOR */
					editor_data.editor_attr.text     = (char *)data->Callback_Data.TextString;  
					editor_data.editor_attr.size     = data->Callback_Data.EditorSize*2 + 3; 
					editor_data.min_enter						 = 0; 
	 				editor_data.mode = ALPHA_MODE;				
					editor_data.LeftSoftKey          = TxtChinese;
					editor_data.AlternateLeftSoftKey = TxtSoftBack;
					editor_data.RightSoftKey         = TxtDelete;
					editor_data.Identifier 			= LATIN_INPUT;
					editor_data.Callback             = (T_EDIT_CB)chinese_input_exec/*NULL*/;
					editor_data.destroyEditor 		 = TRUE;
					editor_start(data->input_win, &editor_data);
#endif /* NEW_EDITOR */
					break;
		    	default:
	    		
			       	break;
			}

	    	memset(&data->DisplayElementBuffer, 0,sizeof(data->DisplayElementBuffer));

	    	IMEChangeMode(&data->Ime,&data->GetParam);
	    	winShow(win);
	    }
			break;
	    default:
	       break;
	  }
	  return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:  	

 $Description:	
 $Returns:		

 $Arguments:	menu and menu item (neither used)
 
*******************************************************************************/
int chinese_input_menu(MfwMnu* m, MfwMnuItem* i)
{	
	T_MFW_HND       parent        = mfwParent( mfw_header() );
    chinese_input( parent, NULL );
    return MFW_EVENT_CONSUMED;
}


/*******************************************************************************

 $Function:    	IMEChangeMode

 $Description:	This function change some of the setting when user selected to use 
                different input method. E.g change from Stroke to Pin-yin input.

 $Returns:		void

 $Arguments:	

*******************************************************************************/
void IMEChangeMode(PIME_DATA pIME, PZI8GETPARAM pGetParam)
{
	/* First initialize the members that will be constant */
	switch(pIME->EditorMode)
    {
    	case STROKE_INPUT_MODE:
			pGetParam->Language			= ZI8_LANG_ZH;
			pGetParam->GetMode			= ZI8_GETMODE_STROKES;
			pGetParam->SubLanguage		= ZI8_SUBLANG_ZH;
			/*MC, check size of editor to work out context*/
			if (TEXT_BUFFER_EDIT_SIZE <= MAX_PHONEBOOK_ENTRY_SIZE)
				pGetParam->Context		= ZI8_GETCONTEXT_SURNAME;
			else
				pGetParam->Context		= ZI8_GETCONTEXT_SMS;
			pGetParam->GetOptions		= ZI8_GETOPTION_ALLCOMPONENTS;
			break;
		case PINYIN_INPUT_MODE:
			pGetParam->Language			= ZI8_LANG_ZH;
			pGetParam->GetMode			= ZI8_GETMODE_PINYIN;
			pGetParam->SubLanguage		= ZI8_SUBLANG_ZH/*ZI8_SUBLANG_ZH_ALL*/;
			/*MC, check size of editor to work out context*/
			if (TEXT_BUFFER_EDIT_SIZE <= MAX_PHONEBOOK_ENTRY_SIZE)
				pGetParam->Context		= ZI8_GETCONTEXT_SURNAME;
			else
				pGetParam->Context		= ZI8_GETCONTEXT_SMS;
			pGetParam->GetOptions		= ZI8_GETOPTION_NOCOMPONENTS/*ZI8_GETOPTION_SAMESOUND*/;
			break;
		case LATIN_INPUT_MODE:
			pIME->InputScreenType = TEXT_DISPLAY_SCREEN;
			break;
		default:
		   	break;
	}
	pIME->InputScreenType = TEXT_DISPLAY_SCREEN;
	memset(pGetParam->pElements, 0, sizeof(pGetParam->pElements));
	memset(pIME->pDisplayElementBuffer, 0,sizeof(pIME->pDisplayElementBuffer));
	/* save the pointer to the ZI8GETPARAM structure in the IME data */
	pIME->pGetParam				= pGetParam;
	/* now initialize the text buffer and element buffer */
	pIME->moreCount				= 0;
	pGetParam->ElementCount 	= 0;
	/* initilalize the WordCharacterCount to zero */
	pGetParam->WordCharacterCount = 0;
	/* initialize the saved number of candidates to zero */
	pIME->candidates = 0;
	/* initialize the candidate cursor to zero */
	pIME->CandidateCursor = 0;
}


/*******************************************************************************

 $Function:    	IMEInitialize

 $Description:	This function initializes the ZI8GETPARAM structure defined in the API 
 				and the various members of the IME_DATA structure defined in to keep 
 				track of the IME.

 $Returns:		void

 $Arguments:	

*******************************************************************************/
/*void IMEInitialize(PIME_DATA pIME, PZI8GETPARAM pGetParam, PZI8WCHAR wszTextBuffer, int TextBufferEditSize,
				 int TextBufferSize, PZI8WCHAR pCandidates)*/
void IMEInitialize(PIME_DATA pIME, PZI8GETPARAM pGetParam, PZI8WCHAR wszTextBuffer,
                  int TextBufferEditSize, int TextBufferSize, PZI8WCHAR pCandidates, 
                  PZI8WCHAR wszElementBuffer, PZI8WCHAR DisplayElementBuffer)
{
	pIME->EditorMode			= STROKE_INPUT_MODE;
	/* First initialize the members that will be constant */
	switch(pIME->EditorMode)
    {
    	case STROKE_INPUT_MODE:
			pGetParam->Language			= ZI8_LANG_ZH;
			pGetParam->GetMode			= ZI8_GETMODE_STROKES;
			pGetParam->SubLanguage		= ZI8_SUBLANG_ZH;
						/*MC, check size of editor to work out context*/
			if (TEXT_BUFFER_EDIT_SIZE <= MAX_PHONEBOOK_ENTRY_SIZE)
				pGetParam->Context		= ZI8_GETCONTEXT_SURNAME;
			else
				pGetParam->Context		= ZI8_GETCONTEXT_SMS;

			pGetParam->GetOptions		= ZI8_GETOPTION_ALLCOMPONENTS;
			break;
		case PINYIN_INPUT_MODE:
			pGetParam->Language			= ZI8_LANG_ZH;
			pGetParam->GetMode			= ZI8_GETMODE_PINYIN;
			pGetParam->SubLanguage		= ZI8_SUBLANG_ZH;
			if (TEXT_BUFFER_EDIT_SIZE <= MAX_PHONEBOOK_ENTRY_SIZE)
				pGetParam->Context		= ZI8_GETCONTEXT_SURNAME;
			else
				pGetParam->Context		= ZI8_GETCONTEXT_SMS;
			pGetParam->GetOptions		= ZI8_GETOPTION_NOCOMPONENTS/*ZI8_GETOPTION_SAMESOUND*/;
			break;
		case LATIN_INPUT_MODE:
			break;
		default:
		   	break;
	}
	/* save the pointer to the ZI8GETPARAM structure in the IME data */
	pIME->pGetParam				= pGetParam;
	/* now initialize the text buffer and element buffer */
	pIME->pTextBuffer			= wszTextBuffer;
	pIME->TextBufferEditSize	= TextBufferEditSize;
	pIME->TextBufferSize		= TextBufferSize;
	pIME->moreCount				= 0;
	pIME->TextBufferCount 		= Uni_strlen(wszTextBuffer);;
	pIME->TextCursor 			= pIME->TextBufferCount;
	pIME->pDisplayElementBuffer	= DisplayElementBuffer;
	pIME->InputScreenType	= TEXT_DISPLAY_SCREEN;
	
	pGetParam->pElements 		= wszElementBuffer;
	pGetParam->ElementCount 	= 0;
	/* initilalize the WordCharacterCount to zero */
	pGetParam->WordCharacterCount = 0;
	/*initial pointer to the candidate buffer*/
	pGetParam->pCandidates = pCandidates;
	/* initialize the saved number of candidates to zero */
	pIME->candidates = 0;
	/* initialize the candidate cursor to zero */
	pIME->CandidateCursor = 0;
}


/*******************************************************************************

 $Function:    	IMECursorLeft

 $Description:	Move the candidate cursor left

 $Returns:		void

 $Arguments:	

*******************************************************************************/
/*
GW SPR#1732 - Corrected cursor position from '6' to 'CANDIDATES_PER_GROUP - 1'
*/

void IMECursorLeft(PIME_DATA pIME)
{
	TRACE_EVENT ("IMECursorLeft()");
	if(pIME->CandidateCursor != 0)
	{
		pIME->CandidateCursor--;
		pIME->candidates = Zi8GetCandidates(pIME->pGetParam);
		DataConvert(pIME);
	}
	/* check ElementCount to see whether ElementBuffer is empty and check if where is any more tick in the element buffer. */
	else if(pIME->pGetParam->ElementCount != 0 && 
		   pIME->pGetParam->pElements[pIME->pGetParam->ElementCount-1] == ZI8_CODE_MORE) 
	{
		/* If the last input is a more tick, remove it */
		pIME->moreCount--;
		/* replace last element with zero */
		pIME->pGetParam->pElements[--pIME->pGetParam->ElementCount] = 0;
		/* get candidates with the new stroke */
		pIME->pGetParam->FirstCandidate = pIME->moreCount * CANDIDATES_PER_GROUP;
		pIME->pGetParam->MaxCandidates = CANDIDATES_PER_GROUP + 1;
		pIME->candidates = Zi8GetCandidates(pIME->pGetParam);
		DataConvert(pIME);
		/* initialize the candidate cursor the character position*/
		pIME->CandidateCursor =CANDIDATES_PER_GROUP - 1;
	}
}


/*******************************************************************************

 $Function:    	IMECursorRight

 $Description:	Move the candidate cursor right

 $Returns:		void

 $Arguments:	

*******************************************************************************/
/*
GW SPR#1732 - Corrected test for cursor position from '6' to 'CANDIDATES_PER_GROUP - 1'
*/
void IMECursorRight(PIME_DATA pIME)
{
	TRACE_EVENT("IMECursorRight()");
	/* If the candidate cursor is greater than the display ability, more tick is add to the element buffer */
	/* to request for next set of characters.												 */
	if(pIME->CandidateCursor >= CANDIDATES_PER_GROUP - 1)
	{
		pIME->moreCount++;
		pIME->pGetParam->pElements[pIME->pGetParam->ElementCount++] = ZI8_CODE_MORE;
		/* get new candidates */
		pIME->pGetParam->FirstCandidate = pIME->moreCount * CANDIDATES_PER_GROUP;
		pIME->pGetParam->MaxCandidates = CANDIDATES_PER_GROUP + 1;
		pIME->candidates = Zi8GetCandidates(pIME->pGetParam);
		DataConvert(pIME);
		/* initialize the candidate cursor to zero */
		pIME->CandidateCursor = 0;
	}
	/* Check number of available candidates before increment of candidate cursor is allowed */
	else if(pIME->candidates > (pIME->CandidateCursor + 1))
		{	pIME->CandidateCursor++;
			pIME->candidates = Zi8GetCandidates(pIME->pGetParam);
			DataConvert(pIME);
		}
}


/*******************************************************************************

 $Function:    	IMESelectCandidate

 $Description:	Select the cadidate at cursor.

 $Returns:		void

 $Arguments:	

*******************************************************************************/
void IMESelectCandidate(PIME_DATA pIME, PZI8WCHAR wszElementBuffer)
{
	int i = 0;
	int j = 0;
	ZI8WCHAR HighByte;
	ZI8WCHAR Temp;
	ZI8WCHAR LowByte;	

	TRACE_EVENT("IMESelectCandidate()");


	/* we can allow selection only if we have candidates*/
	if(pIME->candidates == 0)
		return;
	/* if we have elements in the test we must now remove them */
	while(pIME->pGetParam->ElementCount > 0)
	{
		pIME->pGetParam->pElements[--pIME->pGetParam->ElementCount] = 0;
	}
	/* the more count must be set to zero */
	pIME->moreCount = 0;

	/* check the limit of the text buffer. Stop user to enter character when text buffer is full */
	if(pIME->TextBufferCount >= TEXT_BUFFER_EDIT_SIZE)
    {
    	//messages can be added to inform user when text buffer is full.
    	return;
	}

	/* chek if the selected candidate is a component. To do that the storage format has to be change. */
	/* Followin bit shifting procedure is used the perfrom the task.						 */
	LowByte = pIME->pGetParam->pCandidates[pIME->CandidateCursor] & 0x00FF;
	HighByte = pIME->pGetParam->pCandidates[pIME->CandidateCursor] & 0xFF00;
	Temp = (LowByte << 8) & 0xFF00;
	HighByte = (HighByte >> 8) & 0x00FF;
	Temp = Temp | HighByte;
	
	if(Temp > ZI8_BASE_COMPONENTS && Temp < ZI8_BASE_BPMF) /* check if the selected candidate is a component */
	{
		memset(pIME->pDisplayElementBuffer, 0,sizeof(pIME->pDisplayElementBuffer));
		pIME->pDisplayElementBuffer[0] = pIME->pGetParam->pCandidates[pIME->CandidateCursor];
		
		pIME->pGetParam->pElements[0] = Temp;
		pIME->pGetParam->ElementCount = 1;
    }
	else    /* The selected character is character not a component */
	{
		if(pIME->TextCursor < pIME->TextBufferCount) /* locate the text cursor position for correct character insertion */
	    {
	    	for (i=pIME->TextCursor; i<pIME->TextBufferCount; i++) /* shift and move characters to make rooms to insert the selected character */
	    	{
	    		//pIME->pTextBuffer[pIME->TextBufferCount - i + 1]=pIME->pTextBuffer[pIME->TextBufferCount - i];
	    		pIME->pTextBuffer[pIME->TextBufferCount - j]=pIME->pTextBuffer[pIME->TextBufferCount - j - 1];
	    		j++;
	    	}
	    }	
#ifndef LSCREEN
		/* now added this character to the text */
		pIME->pTextBuffer[pIME->TextCursor++] = pIME->pGetParam->pCandidates[pIME->CandidateCursor];
#else	/*SPR 1319 Add candidate to editor*/
		/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		ATB_edit_Char(editor_pointer, (USHORT )pIME->pGetParam->pCandidates[pIME->CandidateCursor], TRUE);
#else /* NEW_EDITOR */
		edtCharUnicode(editor_pointer, pIME->pGetParam->pCandidates[pIME->CandidateCursor]);
#endif /* NEW_EDITOR */
		pIME->TextCursor++;
#endif
		/* Change input screen type */
		pIME->InputScreenType = WORD_ASSOCIATION_SCREEN;
		/* upate the element buffer to its new position */	
		pIME->pGetParam->pElements = wszElementBuffer;

		/* Adjust the word association. If the candidate we selected was a word candidat,	*/
		/* then we continue the word association. Otherwise this character begins a new word        */		
		if(pIME->pGetParam->wordCandidates > pIME->CandidateCursor)
		{
			pIME->pGetParam->WordCharacterCount++;
		}
		else
		{
			pIME->pGetParam->pCurrentWord = &pIME->pTextBuffer[pIME->TextCursor - 1];
			pIME->pGetParam->WordCharacterCount = 1;
		}
		/* get new candidates */
		pIME->pGetParam->FirstCandidate = pIME->moreCount * CANDIDATES_PER_GROUP;
		pIME->pGetParam->MaxCandidates = CANDIDATES_PER_GROUP + 1;
		//increase TextBufferCount by one
		pIME->TextBufferCount++;
		memset(pIME->pDisplayElementBuffer, 0, sizeof(pIME->pDisplayElementBuffer));
	}
	pIME->candidates = Zi8GetCandidates(pIME->pGetParam);
	DataConvert(pIME);
	/* initialize the candidate cursor to zero */
	pIME->CandidateCursor = 0;
}


/*******************************************************************************

 $Function:    	IMEMore

 $Description:	Get the next group of candidates. For this function, we add the more tick 
 				to the element buffer and increase the more count. Then we get the 
 				next group of candidates.

 $Returns:		void

 $Arguments:	

*******************************************************************************/
void IMEMore(PIME_DATA pIME)
{	
	TRACE_EVENT("IMEMore()");
	
	pIME->moreCount++;
	pIME->pGetParam->pElements[pIME->pGetParam->ElementCount++] = ZI8_CODE_MORE;
	/* get new candidates */
	pIME->pGetParam->FirstCandidate = pIME->moreCount * CANDIDATES_PER_GROUP;
	pIME->pGetParam->MaxCandidates = CANDIDATES_PER_GROUP + 1;
	pIME->candidates = Zi8GetCandidates(pIME->pGetParam);
	DataConvert(pIME);
	/* initialize the candidate cursor to zero */
	pIME->CandidateCursor = 0;
}


/*******************************************************************************

 $Function:    	IMEKeyPress

 $Description:	Enter the stroke and get new candidates. However, any more ticks from 
 				the element buffer must be removed before any stroke is to be added.

 $Returns:		void

 $Arguments:	

*******************************************************************************/
void IMEKeyPress(PIME_DATA pIME, ZI8WCHAR wcStroke)
{	
	PZI8WCHAR element_buffer;

	TRACE_EVENT ("IMEKeyPress()");

	/* remove any trailing more ticks and set the more count back to zero */
	while((pIME->pGetParam->ElementCount > 0) && pIME->pGetParam->pElements[pIME->pGetParam->ElementCount-1] == ZI8_CODE_MORE)
	{
		pIME->pGetParam->pElements[--pIME->pGetParam->ElementCount] = 0;
	}
	pIME->moreCount = 0;

	/* Check the limit of the element buffer. Stop user to enter stroke or Pin Yin letter */
	/* when element budder is full 												 */
	if(pIME->pGetParam->ElementCount >= TEXT_BUFFER_EXTRA_SPACE)
    {
    	return;
	}
	/* Insert the stroke or the Pin Yin letter into the element buffer for character search */
	pIME->pGetParam->pElements[pIME->pGetParam->ElementCount++] = wcStroke;

/*MC, let's check what's in the element buffer*/

	element_buffer = pIME->pGetParam->pElements;
	/***************************Go-lite Optimization changes Start***********************/
	//Aug 16, 2004    REF: CRR 24323   Deepa M.D
	TRACE_EVENT_P5("Element List: %c, %c, %c, %c, %c", (element_buffer[0]-ZI8_BASE_PINYIN+65), (element_buffer[1]-ZI8_BASE_PINYIN+65), (element_buffer[2]-ZI8_BASE_PINYIN+65), (element_buffer[3]-ZI8_BASE_PINYIN+65), (element_buffer[4]-ZI8_BASE_PINYIN+65));
	/***************************Go-lite Optimization changes end***********************/
/*MC --end debug code*/
	
	/* get candidates with the new stroke */
	pIME->pGetParam->FirstCandidate = pIME->moreCount * CANDIDATES_PER_GROUP;
	pIME->pGetParam->MaxCandidates = CANDIDATES_PER_GROUP + 1;
	pIME->candidates = Zi8GetCandidates(pIME->pGetParam);

	if (pIME->candidates > 0 || pIME->EditorMode == PINYIN_INPUT_MODE)//MC
	{	/* Convert the data storage format for display purpose */
		DataConvert(pIME);
		/* initialize the candidate cursor to zero */
		pIME->CandidateCursor = 0;
	}
	else
	{	//remove this keypress
		IMEClear(pIME, pIME->pGetParam->pElements);
	}
}


/*******************************************************************************

 $Function:    	IMEClear

 $Description:	Clear the last stroke entered. This function first handle the case where 
 				the last element is a more tick. If there are no elements, it removes 
 				the last text element.

 $Returns:		void

 $Arguments:	

*******************************************************************************/
//void IMEClear(PIME_DATA pIME)
void IMEClear(PIME_DATA pIME, PZI8WCHAR wszElementBuffer)
{
	

	TRACE_EVENT("IMEClear()");
	

	/* handle the condition where the last element is a more tick */
	if(pIME->pGetParam->ElementCount != 0) //check ElementCount to see whether ElementBuffer is empty
	{
		/* If the last input is a more tick, remove it */
		if(pIME->pGetParam->pElements[pIME->pGetParam->ElementCount-1] == ZI8_CODE_MORE)
		{
			pIME->moreCount--;

			/* replace last element with zero */
			pIME->pGetParam->pElements[--pIME->pGetParam->ElementCount] = 0;
		}
		else
		{
			pIME->pGetParam->pElements[--pIME->pGetParam->ElementCount] = 0;	
			pIME->pDisplayElementBuffer[pIME->pGetParam->ElementCount] = 0;
		}

		/* When ElementCount is zero and InputScreenType is STROKE_INPUT_SCREEN or PINYIN_INPUT_SCREEN, 	 */
		/* InputScreenType will be set to WORD_ASSOCIATION_SCREEN to get the associated character. If no associated */
		/* characters can be found, the most common used character in chinese writting will be provided.	 */
		if((pIME->pGetParam->ElementCount == 0 && pIME->InputScreenType == STROKE_INPUT_SCREEN) || 
		  (pIME->pGetParam->ElementCount == 0 && pIME->InputScreenType == PINYIN_INPUT_SCREEN))
		{
			pIME->InputScreenType = WORD_ASSOCIATION_SCREEN;
		}
	}
	else
	{
		/* remove the last character from text buffer */
		/* first update count for word association */
		if(pIME->pGetParam->WordCharacterCount > 0)
			pIME->pGetParam->WordCharacterCount--;

		/* remove the character at cursor position */
		if(pIME->TextCursor != 0)
		{
#ifndef LSCREEN
			/* replace last character with zero */
        	if(pIME->TextBufferCount==pIME->TextCursor)
            {
				pIME->pTextBuffer[--pIME->TextCursor] = 0;
                pIME->TextBufferCount--;
			}    
			else if(pIME->TextBufferCount > pIME->TextCursor)
			{
			    for (i=pIME->TextCursor; i<pIME->TextBufferCount; i++)
			    {
			        pIME->pTextBuffer[i-1]=pIME->pTextBuffer[i];
			    }
			    pIME->pTextBuffer[pIME->TextBufferCount-1]=0;
			    pIME->TextCursor--;
                pIME->TextBufferCount--;
			}
#else		/*SPR 1319 delete char in front of cursor */
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
		ATB_edit_DeleteLeft(editor_pointer, TRUE); /* SPR#2342 - SH */
#else /* NEW_EDITOR */
		edtChar(editor_pointer, ecBack);
#endif /* NEW_EDITOR */
		  pIME->TextBufferCount--;
#endif

		}
		/* update location of elements */
		pIME->pGetParam->pElements = wszElementBuffer;
	}
	/* get candidates */
	pIME->pGetParam->FirstCandidate = pIME->moreCount * CANDIDATES_PER_GROUP;
	pIME->pGetParam->MaxCandidates = CANDIDATES_PER_GROUP + 1;
	pIME->candidates = Zi8GetCandidates(pIME->pGetParam);
	/* Convert the data storage format for display purpose */
	DataConvert(pIME);
	/* initialize the candidate cursor to zero */
	pIME->CandidateCursor = 0;
}


/*******************************************************************************

 $Function:    	CandidateHighlightDisplayContorl

 $Description:	With given candidate cursor position, and character stored in the HighlightedCandidate,
 				this function will invert the colour of the candidate to indicate user the current selected
 				character.

 $Returns:		void

 $Arguments:	

*******************************************************************************/
void CandidateHighlightDisplayContorl(PIME_DATA pIME, PZI8WCHAR HighlightedCandidate)
{
	/* Highlight the candidate character pointed by Ime.CandidateCursor */
#ifndef LSCREEN
	if(pIME->CandidateCursor == 0)
		dspl_TextOut(1,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)HighlightedCandidate);
	else if(pIME->CandidateCursor == 1)
		dspl_TextOut(13,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)HighlightedCandidate);
	else if(pIME->CandidateCursor == 2)
		dspl_TextOut(25,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)HighlightedCandidate);
	else if(pIME->CandidateCursor == 3)
		dspl_TextOut(37,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)HighlightedCandidate);
	else if(pIME->CandidateCursor == 4)
		dspl_TextOut(49,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)HighlightedCandidate);
	else if(pIME->CandidateCursor == 5)
		dspl_TextOut(61,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)HighlightedCandidate);
	else
		dspl_TextOut(73,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)HighlightedCandidate);
#else
{
/*
SPR 1319 manually highlight selected candidate
GW SPR#1732 - Added 0x80, 0x00 to the start of the 2 byte unicode string
*/
	char hChar[10];
	memcpy(&hChar[2],HighlightedCandidate,8);
	hChar[0] = (char)0x80;	hChar[1] = 0x00;
	dspl_TextOut(pIME->CandidateCursor*CHI_FONT_WIDTH+1,SCREEN_SIZE_Y-Mmi_layout_line(3),DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)hChar);
}
#endif
}


/*******************************************************************************

 $Function:    	TextCursorDisplayContorl

 $Description:	To display the cursor position to user during text editing.

 $Returns:		void

 $Arguments:	

*******************************************************************************/
void TextCursorDisplayContorl(PIME_DATA pIME, PZI8WCHAR CursorPosition, int InsertLine)
{
	int TempTextCursor 	= 0;
	int x_position = 1;
	int i;
	int line;//position of char at start of line

	TempTextCursor = pIME->TextCursor % CANDIDATES_PER_GROUP;
	line = pIME->TextCursor - TempTextCursor;
	for (i=0; i < TempTextCursor; i++)
		
	{	//if last byte of word is 0x00, and first byte is between 0x20 and 0x7E
		if (LatinChar(pIME->pTextBuffer[line+i]))
			x_position +=6;
		else
			x_position+=CHI_FONT_WIDTH;/*SPR 1319 using macro rather than magic no*/
	}
	switch(pIME->InputScreenType)
	{
		case TEXT_DISPLAY_SCREEN:
		
			if(InsertLine == 1)
			{
				
			dspl_TextOut(x_position,Mmi_layout_line(1),DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)CursorPosition);
			

			}
			else
			{	dspl_TextOut(x_position,Mmi_layout_line(2),DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)CursorPosition);
				
			}
			break;
		case STROKE_INPUT_SCREEN:
		case WORD_ASSOCIATION_SCREEN:
			dspl_TextOut(x_position,0,DSPL_TXTATTR_UNICODE|DSPL_TXTATTR_INVERS,(char*)CursorPosition);
		
		default:
			break;
	}	
}


/*******************************************************************************

 $Function:    	DataConvert

 $Description:	To change to data storage format i.e if a unicode is 0x89D5.
 				The DataConvert function will change storage format to 0xD589 (This format 
 				is required to display UniCode on LCD). The data format is chaged by 
 				byte shifting.

 $Returns:		void

 $Arguments:	

*******************************************************************************/
void DataConvert(PIME_DATA pIME)
{
	ZI8WCHAR HighByte;
	ZI8WCHAR Temp;
	ZI8WCHAR LowByte;
	ZI8WCHAR tempString[25];
	int word_length;
	int i 		= 0;
	int count 	= 0;
	TRACE_EVENT("DataConvert");

	
	if((pIME->InputScreenType == STROKE_INPUT_SCREEN) ||
	  (pIME->InputScreenType == PINYIN_INPUT_SCREEN))
	{
		count = pIME->pGetParam->ElementCount;
		/* The more tick is not to be displayed on LCD therefore it is not considered. */
		while((count > 0) && pIME->pGetParam->pElements[count - 1] == ZI8_CODE_MORE)
		{
			count--;
		}

		/* Format of the data stored in the array pointed by pElement is converted and  */
		/* saved in the array pointed by pDisplayElementBuffer. Please note that the    */
		/* the data format stored in the array pointed by pElement was unchanged. This */
		/* is kept that way for eZiText character search.*/
			if (pIME->InputScreenType == STROKE_INPUT_SCREEN)
			{	for(i = 0; i < count; i++)
				{
					Temp = pIME->pGetParam->pElements[i];
					LowByte = pIME->pGetParam->pElements[i] & 0x00FF;
					HighByte = pIME->pGetParam->pElements[i] & 0xFF00;
					Temp = (LowByte << 8) & 0xFF00;
					HighByte = (HighByte >> 8) & 0x00FF;
					Temp = Temp | HighByte;
					pIME->pDisplayElementBuffer[i] = Temp;
				}	
				pIME->pDisplayElementBuffer[count] = 0x0000; //terminate string
			}
				
			if (pIME->InputScreenType == PINYIN_INPUT_SCREEN)	
			{ 	int no_of_display_chars;
				int i;
				word_length = count; //let's use user input for display
				/*word_length = */Zi8GetCharInfo(pIME->pGetParam->pCandidates[pIME->CandidateCursor], tempString, 12, ZI8_GETMODE_PINYIN);
				TRACE_EVENT((char*)tempString);/*MC Tracing first matched pinyin word*/

				/*MC convert input elements from EZi element codes to Ezi Pinyin spelling for display*/
				for (i=0; i<word_length; i++)
				{	tempString[i] = (((pIME->pGetParam->pElements[i]-ZI8_BASE_PINYIN)+LOWER_CASE_ASCII_OFFSET)) +EZITEXT_OUTPUT_OFFSET; 
				}

				if (word_length > 0)
				{
					if (word_length < count)
						no_of_display_chars = word_length;
					else
						no_of_display_chars = count;
					/***************************Go-lite Optimization changes Start***********************/
					//Aug 16, 2004    REF: CRR 24323   Deepa M.D
					TRACE_EVENT_P1("chars returned: %d", word_length);		
					/***************************Go-lite Optimization changes end***********************/
					for(i = 0; i < no_of_display_chars; i++)
					{
						Temp = tempString[i];
						LowByte = tempString[i] & 0x00FF;
						HighByte = tempString[i] & 0xFF00;
						Temp = (LowByte << 8) & 0xFF00;
						HighByte = (HighByte >> 8) & 0x00FF;
						Temp = Temp |HighByte;
						pIME->pDisplayElementBuffer[i] = Temp;
					}
					pIME->pDisplayElementBuffer[no_of_display_chars] = 0x0000;//terminate string
				}
				
			}
		}
			
	
	/* format of the data stored in the array pointed by pCandidates is coverted for display */
	for(i = 0; i < pIME->pGetParam->count; i++)
	{
		Temp = pIME->pGetParam->pCandidates[i];
		LowByte = pIME->pGetParam->pCandidates[i] & 0x00FF;
		HighByte = pIME->pGetParam->pCandidates[i] & 0xFF00;
		Temp = (LowByte << 8) & 0xFF00;
		HighByte = (HighByte >> 8) & 0x00FF;
		Temp = Temp | HighByte;
		pIME->pGetParam->pCandidates[i] = Temp;
	}
	pIME->pGetParam->pCandidates[pIME->candidates] = 0x0000;//terminate string
				
}


/*******************************************************************************

 $Function:    	Uni_strlen

 $Description:	To count the number of element from the input array

 $Returns:		number of element

 $Arguments:	

*******************************************************************************/
ZI8WCHAR Uni_strlen(PZI8WCHAR String)
{
	int i = 0;
	ZI8WCHAR count = 0;

	while(String[i] != 0)		
	{
		i++;
		count++;
	}
	return count;
}

