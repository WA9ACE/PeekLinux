/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Game
 $File:		    Mmigame.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    03/07/01                                                      
                                                                               
********************************************************************************
                                                                              
 Description
	Aug 16, 2004    REF: CRR 24323   Deepa M.D
	Bug:Clenup of sprintf used for tracing
	Fix:Replace the char buf[]; sprintf (buf, "...", ...); TRACE_EVENT (buf); statements by TRACE_EVENT_PX

    This provides the main game (four in a row) functionality

  
********************************************************************************/


/*******************************************************************************
                                                                              
                                Include files
                                                                              
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

#include "cus_aci.h"

#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_lng.h"
#include "mfw_tim.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "mfw_sim.h"
#include "mfw_nm.h"
#include "mfw_sat.h"
#include "mfw_ss.h" /*for convert*/
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_cphs.h"
#include "mfw_sat.h"
#include "Mfw_band.h"
#include "mfw_ffs.h"
#include "Mmigame.h"

#include "dspl.h"

#include "MmiMmi.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiBand.h"
#include "MmiCPHS.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif
#include"MmiBookShared.h"




#include "font_bitmaps.h"
#include "mmiColours.h"
#include "MmiResources.h"

#include "Mmigame.h"
//GW 14/09/01 Disable game when not required.
#ifdef MMIGAME
void dspl_show_bitmap(int x,int y,t_font_bitmap* current_bitmap,U32 attr );	
/*******************************************************************************
                                                                              
                                internal data
                                                                              
*******************************************************************************/

#define FOUR_IN_A_ROW_INIT 121
#define MAX_X_BOARD 11
#define MAX_Y_BOARD 8
/*
*  The information related to every window must be encapsulated in such an structure
*/

typedef struct
{
    T_MMI_CONTROL   mmi_control;		// common control parameter
	T_MFW_HND win;
	T_MFW_HND kbd;
	T_MFW_HND	menu;
	T_MFW_HND 	parent_win;	
	T_MFW_HND info_win;
	char board_array[MAX_Y_BOARD][MAX_X_BOARD];	//the virtual board
} T_four_in_a_row;

typedef struct
{
    /* administrative data */

    T_MMI_CONTROL     mmi_control;
    T_MFW_HND         win;
    T_MFW_HND         parent_win;
/* SPR#1428 - SH - New editor data */
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA editor_data;
#else /* NEW_EDITOR */
    T_EDITOR_DATA   editor_data;
#endif /* NEW_EDITOR */

    /* internal data */
    char        buffer[80];
    UBYTE       status;

} tShowInfo;


/*
*  These are common functions xxx_create and xxx_destroy
*/
T_MFW_HND four_in_a_row_create(MfwHnd parent);
void four_in_a_row_destroy (T_MFW_HND);


/*
*  This dialog function (the same name as the window)
* is used to handle the comunication between different windows. The global macro SEND_EVENT can be used with parameter win
* and the corresponding events to send from one mmi dialog to another.
*/
void four_in_a_row (T_MFW_HND win, USHORT event, SHORT value, void * parameter);


/*
*  These are common optional functions handler
*/
int four_in_a_row_kbd_cb (MfwEvt e, MfwKbd *k);
int four_in_a_row_win_cb (MfwEvt e, MfwWin *w);

int Game_Result;


/*
*  This an optional function, used often to call, create and init a new dialog, with different parameters depending
* on the context
*/
T_MFW_HND four_in_a_row_start (T_MFW_HND win_parent,char *character);
int four_in_a_row_aktivate(MfwMnu* m, MfwMnuItem* i);

int check_column(T_four_in_a_row *data, int where_from);
void drop_stone(T_four_in_a_row *data);

int mobile_thinking(T_four_in_a_row *data);
int check_line(T_four_in_a_row *data, int x_value, int drop_position, int dx, int dy, BOOL where_from);
void check_for_winner(T_four_in_a_row *data, int check_out_position);
int random_scoring(void);  // RAVI

T_MFW_HND Game_Info(T_MFW_HND parent_window);
static T_MFW_HND ShowGame_Information(MfwHnd parent_window);
static void ShowGame_DialogCB(T_MFW_HND win, USHORT e,  SHORT identifier, void *parameter);
void ShowGameInfoEditor(T_MFW_HND win);
static void ShowInfoCB( T_MFW_HND win, USHORT Identifier,UBYTE reason);
void showGameInfo_destroy(MfwHnd own_window);

const char * info = "Welcome to Connect 4!";
const char * playAgain = "Play Again?";


//internal datas

int column;				//this is the current position of white player cursor
int x_runer;	//goes through the horizontal lines
int y_runer;	//goes through the vertical lines
char winner;	//describes the winner 0->nobody 'B'->black(mobile) 'W'->white(human player)
int column_black;	//this is the current position of black player cursor
int request_stone;	//helps to decide between black or white (for strategical things)



#ifndef BMP_FORMAT_BW_UNPACKED
#define BMP_FORMAT_BW_UNPACKED 1
#endif


t_font_bitmap  plWin1		={ 0, BMP_FORMAT_BW_UNPACKED, 21, 24, 0, (char*)player_win1};
t_font_bitmap  plWin2		={ 0, BMP_FORMAT_BW_UNPACKED, 21, 24, 0, (char*)player_win2};
t_font_bitmap  plLost1		={ 0, BMP_FORMAT_BW_UNPACKED, 21, 24, 0, (char*)player_lost1};
t_font_bitmap  plLost2		={ 0, BMP_FORMAT_BW_UNPACKED, 21, 24, 0, (char*)player_lost2};
t_font_bitmap  draw1		={ 0, BMP_FORMAT_BW_UNPACKED, 21, 24, 0, (char*)board_full1};
t_font_bitmap  draw2		={ 0, BMP_FORMAT_BW_UNPACKED, 21, 24, 0, (char*)board_full2};
t_font_bitmap  gameName1	={ 0, BMP_FORMAT_BW_UNPACKED, 18, 24, 0, (char*)game_name1};
t_font_bitmap  gameName2	={ 0, BMP_FORMAT_BW_UNPACKED, 24, 24, 0, (char*)game_name2};

#ifdef COLOURDISPLAY
char colBmp[256*4];
t_font_bitmap  allCol  ={ 0, BMP_FORMAT_256_COLOUR, 32, 32, 0, colBmp };
t_font_bitmap  stone_colour   ={ 0, BMP_FORMAT_BW_UNPACKED, 8, 8, 0, (char*)black_stone_bw};
t_font_bitmap  gameCursor = { 0, BMP_FORMAT_BW_UNPACKED, 8, 8, 0, (char*)game_cursor};
#else
#ifdef LSCREEN
t_font_bitmap  blackStone_bw ={ 0, BMP_FORMAT_BW_UNPACKED, 8, 8, 0, (char*)black_stone_bw};
t_font_bitmap  whiteStone_bw ={ 0, BMP_FORMAT_BW_UNPACKED, 8, 8, 0, (char*)white_stone_bw};
t_font_bitmap  gameCursor = { 0, BMP_FORMAT_BW_UNPACKED, 8, 8, 0, (char*)game_cursor};
#else
t_font_bitmap  blackStone_bw ={ 0, BMP_FORMAT_BW_UNPACKED, 4, 4, 0, (char*)black_stone_bw};
t_font_bitmap  whiteStone_bw ={ 0, BMP_FORMAT_BW_UNPACKED, 4, 4, 0, (char*)white_stone_bw};
t_font_bitmap  gameCursor = { 0, BMP_FORMAT_BW_UNPACKED, 6, 6, 0, (char*)game_cursor};
#endif
#endif

t_font_bitmap *blackStone, *whiteStone;

/*******************************************************************************

 $Function:    	four_in_a_row_create

 $Description:	 
 
 $Returns:		

 $Arguments:	
 				
*******************************************************************************/


T_MFW_HND four_in_a_row_create (T_MFW_HND parent_window)
{

    T_MFW_WIN     * win;
    
    /*
     * This window is dynamic, for that reason the associated data are allocated in the mfw heap
     */
	T_four_in_a_row *  data = (T_four_in_a_row *)ALLOC_MEMORY (sizeof (T_four_in_a_row));

	TRACE_FUNCTION ("four_in_a_row_create()");

    /*
     * Create window handler
     */

    data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)four_in_a_row_win_cb);

    if (data->win EQ 0)
      return 0;

	 /*
     * These assignments are necessary to attach the data to the window, and to handle the mmi event communication.
     */

	data->mmi_control.dialog    = (T_DIALOG_FUNC)four_in_a_row;
    data->mmi_control.data      = data;
	data->parent_win           = parent_window;
    win                         = ((T_MFW_HDR *)data->win)->data;
    win->user                   = (void *) data;

    /*
    * Create any other handler
    */

    data->kbd      = kbd_create (data->win,KEY_ALL,(T_MFW_CB)four_in_a_row_kbd_cb);

	column=0;	//set start value for game cursor
	winner ='0';			//game starts, so set winner to nobody

	/*clear the board*/
	for (y_runer=0;y_runer<MAX_Y_BOARD;y_runer++){
		for(x_runer=0;x_runer<MAX_X_BOARD;x_runer++){
			data->board_array[y_runer][x_runer] ='0';
		}
	}
	winShow(data->win);
  
  return data->win;
}

/*******************************************************************************

 $Function:    	four_in_a_rowr_destroy

 $Description:	 
 
 $Returns:		

 $Arguments:	
 				
*******************************************************************************/

void four_in_a_row_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win;
  T_four_in_a_row       * data;

	TRACE_EVENT("four_in_a_row_destroy");

  if (own_window)
  {
    win  = ((T_MFW_HDR *)own_window)->data;
    data = (T_four_in_a_row *)win->user;

    if (data)
    {
      /*
       * Exit Keyboard Handler
       */
      /*
       * Delete WIN Handler
       */
      win_delete (data->win);
    }

   /*
   *  In this case the data attached to window must be also deleted.
   */
	FREE_MEMORY ((void *)data, sizeof (T_four_in_a_row));

   column=0;			//set the current column back to 0

  }
}

/*******************************************************************************

 $Function:    	four_in_a_row_start

 $Description:	 This function just creates and inits the new dialog
 
 $Returns:		

 $Arguments:	
 				
*******************************************************************************/
T_MFW_HND four_in_a_row_start (T_MFW_HND win_parent,char *character)
{
  T_MFW_HND win; 
 /*
 MmiTetrisStart();
 return;
 */
  win = four_in_a_row_create (win_parent);
  TRACE_EVENT("four_in_a_row_start");

  if (win NEQ NULL)
  {
		SEND_EVENT(win,FOUR_IN_A_ROW_INIT,0,character);
  }
  return win;

}

/*******************************************************************************

 $Function:    	four_in_a_row

 $Description:	 
 
 $Returns:		

 $Arguments:	
 				
*******************************************************************************/



void four_in_a_row (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
{
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_four_in_a_row       * data = (T_four_in_a_row *)win_data->user;

    TRACE_FUNCTION ("four_in_a_row()");

   /*
   *  In this case the communication is very simple (only one intern event)
   */


    switch (event)
	{
		case FOUR_IN_A_ROW_INIT:
			softKeys_displayId(TxtSearchName,TxtNull,0,COLOUR_IDLE);			
			winShow(data->win);
		break;

	    default:
		return;
	}

}

/*******************************************************************************

 $Function:    	four_in_a_row_win_cb

 $Description:	win calback function of four in a row
 
 $Returns:		

 $Arguments:	
 				
*******************************************************************************/


int four_in_a_row_win_cb (MfwEvt e, MfwWin *w)
{
//	T_DISPLAY_DATA display_info;  // RAVI
	T_four_in_a_row * data = (T_four_in_a_row *)w->user;
	int xOfs,yOfs, xScale, yScale;
	int manOfsX,manOfsY;
	int xPos,yPos;
	int y_axis;																	//goes through the vertical lines
	int x_axis;																	//goes through the horizontal lines
//	int x,y,i,b;     // RAVI
	int displayPosX, temp;

    TRACE_FUNCTION ("four_in_a_row_win_cb()");
	
	switch (e)
    {
        case MfwWinVisible:
#ifdef COLOURDISPLAY
				whiteStone = &stone_colour;
				blackStone = &stone_colour;
				
#else
				whiteStone = &blackStone_bw;
				blackStone = &whiteStone_bw;
				
#endif
#ifdef LSCREEN
				manOfsX = GAME_POS_X + 3;
				manOfsY = GAME_POS_Y + 3;
#else
				manOfsX = GAME_POS_X + 1;
				manOfsY = GAME_POS_Y + 1;
#endif
        		xOfs = GAME_POS_X;
        		yOfs = GAME_POS_Y;
        		xScale = GAME_SCALE_X;
        		yScale = GAME_SCALE_Y;

        		//dspl_ClearAll(); //Clears to white, not BGD!
        		TRACE_EVENT("display clear");
		     	resources_setColour( COLOUR_GAME );
    	  		dspl_Clear( 0,0, SCREEN_SIZE_X,SCREEN_SIZE_Y);
#ifdef LSCREEN
    	    	softKeys_displayId(TxtHelp,TxtExit,0,COLOUR_LIST_SUBMENU);
#endif     			

		     	resources_setColour( COLOUR_GAME );
    	  		dspl_Clear( xOfs, yOfs, xOfs+MAX_X_BOARD*xScale, yOfs+MAX_Y_BOARD*yScale ); //Clears to white, not BGD!
        		//dspl_BitBlt(column*xScale+xOfs+1,yOfs-yScale,8,8,0,(void*)game_cursor,0);	//place the cursor bitmap
		        dspl_show_bitmap(column*xScale+xOfs+1,yOfs-yScale, &gameCursor, 0 );
         		for (y_axis=0;y_axis<=MAX_Y_BOARD;y_axis++)
       			{
					dspl_DrawLine(	xOfs, 						yOfs+y_axis*yScale,
									xOfs+MAX_X_BOARD*xScale,	yOfs+y_axis*yScale);
       			}
				for(x_axis=0;x_axis<=MAX_X_BOARD;x_axis++)
				{
					dspl_DrawLine(	xOfs+x_axis*xScale,	yOfs,
									xOfs+x_axis*xScale,	yOfs+MAX_Y_BOARD*yScale);
				}

        		//go through the board-array and check for W or B and set the stones
       			for (y_axis=0;y_axis<MAX_Y_BOARD;y_axis++)
       			{
					for(x_axis=0;x_axis<MAX_X_BOARD;x_axis++)
					{
		       			xPos = manOfsX+x_axis*xScale;
		       			yPos = manOfsY+y_axis*yScale;
						if(data->board_array[y_axis][x_axis] EQ 'W'){			//found white
							//set white_stone bitmap
			        		dspl_SetFgdColour( COL_R );
		        			dspl_show_bitmap(xPos,yPos, whiteStone, 0 );
						}
						if(data->board_array[y_axis][x_axis] EQ 'B'){			//found black
							//set black_stone bitmap
			        		dspl_SetFgdColour( COL_G ); //0x00404040
		        			dspl_show_bitmap(xPos,yPos, blackStone, 0 );
						}
					}
				}
        		dspl_SetFgdColour( COL_RB );
        		dspl_SetBgdColour( COL_RG );
        		dspl_show_bitmap(GAME_NAMEPOSX1, GAME_NAMEPOSY1, &gameName1, 0 );
        		displayPosX = (SCREEN_SIZE_X/2);
				temp = strlen((char*)playAgain);        		
       			//this if-clause is to set the bitmap, for win or lost game
       			if(winner EQ 'W'){
       				//human player wins, so set win bitmap
#ifdef LSCREEN
	        		dspl_SetFgdColour( COL_BLK );
	        		dspl_SetBgdColour( COL_TRANSPARENT );       				
       				Game_Result = TRUE;
					softKeys_displayId(TxtYes,TxtNo,0,COLOUR_LIST_SUBMENU);
				    dspl_TextOut((displayPosX-((temp*CHAR_WIDTH)/2)),170,DSPL_TXTATTR_NORMAL,(char*)playAgain);
#endif       			
	        		dspl_SetFgdColour( COL_RB );
    	    		dspl_SetBgdColour( COL_GB );			
					dspl_show_bitmap(GAME_WINPOSX1, GAME_WINPOSY1, &plWin1, 0 );
        			dspl_show_bitmap(GAME_WINPOSX2, GAME_WINPOSY2, &plWin2, 0 );        			
       			}
       			else
       			if(winner EQ 'B'){
       				//human player lost, so set lost bitmap
#ifdef LSCREEN
	        		dspl_SetFgdColour( COL_BLK );
	        		dspl_SetBgdColour( COL_TRANSPARENT );       				
       				Game_Result = TRUE;
					softKeys_displayId(TxtYes,TxtNo,0,COLOUR_LIST_SUBMENU);
				    dspl_TextOut((displayPosX-((temp*CHAR_WIDTH)/2)),170,DSPL_TXTATTR_NORMAL,(char*)playAgain);
#endif      
	        		dspl_SetFgdColour( COL_RB );
    	    		dspl_SetBgdColour( COL_GB );			
        			dspl_show_bitmap(GAME_WINPOSX1, GAME_WINPOSY1, &plLost1, 0 );
        			dspl_show_bitmap(GAME_WINPOSX2, GAME_WINPOSY2, &plLost2, 0 );        			
       			}
       			else
       			if(winner EQ 'N'){
					//board is full, nobody (N) wins
#ifdef LSCREEN
	        		dspl_SetFgdColour( COL_BLK );
					dspl_SetBgdColour( COL_TRANSPARENT );       				
       				Game_Result = TRUE;
					softKeys_displayId(TxtYes,TxtNo,0,COLOUR_LIST_SUBMENU);
				    dspl_TextOut((displayPosX-((temp*CHAR_WIDTH)/2)),170,DSPL_TXTATTR_NORMAL,(char*)playAgain);
#endif       				
					
	        		dspl_SetFgdColour( COL_R );
	        		dspl_SetBgdColour( COL_G );
        			dspl_show_bitmap(GAME_WINPOSX1, GAME_WINPOSY1, &draw1, 0 );
        			dspl_show_bitmap(GAME_WINPOSX2, GAME_WINPOSY2, &draw2, 0 );
       			}
       			else
       			{
        			dspl_SetFgdColour( COL_RB );
        			dspl_SetBgdColour( COL_RG );
        			dspl_show_bitmap(GAME_NAMEPOSX1, GAME_NAMEPOSY1, &gameName1, 0 );
        			dspl_show_bitmap(GAME_NAMEPOSX2, GAME_NAMEPOSY2, &gameName2, 0 );
        			dspl_show_bitmap(GAME_NAMEPOSX3, GAME_NAMEPOSY3, &gameName2, 0 );
       			}
        break;

        default:
        return 0;
    }

    return 1;
}

/*******************************************************************************

 $Function:    	four_in_a_row_kbd_cb

 $Description:	
 
 $Returns:		

 $Arguments:	
 				
*******************************************************************************/

int four_in_a_row_kbd_cb (MfwEvt e, MfwKbd *k)
{

	T_MFW_HND       win  = mfw_parent (mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_four_in_a_row         * data = (T_four_in_a_row *)win_data->user;
		
    TRACE_EVENT ("four_in_a_row_kbd_cb()");
	
	switch (k->code)
	{
			case KCD_HUP:						//quit the game "four in a row"         
			case KCD_RIGHT:	 //quit the game "four in a row" 
				TRACE_EVENT ("quit four_in_a_row");	
				dspl_ClearAll();				//clear mobile screen
				four_in_a_row_destroy (win);	//destroy win-handler		
			break;

			case KCD_MNULEFT:
			case KCD_1:
				if(winner EQ '0'){				//while nobody has won....
					TRACE_EVENT ("KCD_1");
					if(column > 0)
					{	//cursor must be greater than 0
						column=column-1;
					}
					winShow(data->win);
				}
			break;
			case KCD_MNUSELECT:
			case KCD_2:
				if(winner EQ '0'){				//while nobody has won....
					TRACE_EVENT ("KCD_2");
					drop_stone(data);			//drop stone
				}
				winShow(data->win);
			break;
			case KCD_MNURIGHT:
			case KCD_3:
				if(winner EQ '0'){				//while nobody has won....
					TRACE_EVENT ("KCD_3");
					if(column<MAX_X_BOARD-1 ){	//cursor must be less than 50
						column=column+1;
					}
					winShow(data->win);
				}
			break;
			case KCD_LEFT:
				if(Game_Result == TRUE)
				{
					Game_Result = FALSE;
					four_in_a_row_destroy(win);	//destroy win-handler						
					four_in_a_row_start(win,0);
				}
				else
					Game_Info(win);
				break;
	}
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	four_in_a_row_aktivate

 $Description:	Starts the game function on user selection
 
 $Returns:		MFW_EVENT_CONSUMED if event handled, otherwise
				MFW_EVENT_PASSED

 $Arguments:	menu, menu item
 
*******************************************************************************/
int four_in_a_row_aktivate(MfwMnu* m, MfwMnuItem* i)
{
	
    T_MFW_HND	    	parent       = mfwParent( mfw_header());
    TRACE_EVENT("four_in_a_row_aktivate()");
    four_in_a_row_start(parent,0);
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	drop_stone

 $Description:	decide whether a stone can be droped and where it can be droped
 
 $Returns:		

 $Arguments:	T_four_in_a_row 
 
*******************************************************************************/
void drop_stone(T_four_in_a_row *data)
{
	int drop_position;											//position (y in current board column) where to drop the stone
	
	TRACE_EVENT("drop_stone()");
	request_stone=0;											//white is on turn
	drop_position=check_column(data,request_stone);				//set the drop position for the white stone in the current white, board column
	if(drop_position>=0){										//if the current column is not filled up set stone
		data->board_array[drop_position][column]='W';			//white is first and set a 'W' in the virtual board for a white stone
		check_for_winner(data,drop_position);					//check whether the last move(white) was a win-move...
		if(winner EQ '0'){										//...if not, then the mobile(black) is on turn
			drop_position=mobile_thinking(data);				//set the drop position for the black stone in the current black, board column
			data->board_array[drop_position][column_black]='B';	//set a 'B' for in the virtual board for a black stone
			check_for_winner(data,drop_position);				//check whether the last move(black) was a win-move...
		}
	}
}

/*******************************************************************************

 $Function:    	check_for_winner

 $Description:	checks the current drop position for a win-position
 
 $Returns:		

 $Arguments:	T_four_in_a_row 
 
*******************************************************************************/
void check_for_winner(T_four_in_a_row *data, int check_out_position)
{
	int check_with_this_color;				//set the color to check for
	int color_regulator;					//helps to switch between both colors
	int check_this_column;					//declares the column to check for

	TRACE_EVENT("check_for_winner()");
	if(data==NULL || check_out_position<0 || check_out_position>=MAX_Y_BOARD)
		{
		TRACE_ERROR("check_for_winner() invalid parameters");
		return;
		}
	color_regulator=0;						//at first, use white to check for
	winner='0';								//at first we don't know who wins, so nobody is set as winner
	while(color_regulator<2){				//while checking for white(0) and black(1) is not ready....
		if(color_regulator EQ 0){			//if color_regulator is 0, the board will be checked for white stones ('W')
			check_with_this_color='W';
		}else{								//if color_regulator is 1, the board will be checked for black stones ('B')
			check_with_this_color='B';
		}
		
		if(request_stone EQ 0){				//if the board will be checked for white, use column as the current column
			check_this_column=column;
		}else{								//if the board will be checked for black, use column_black as the current column
			check_this_column=column_black;
		}
		if(check_this_column<0 || check_this_column>=MAX_X_BOARD)
			{
			TRACE_ERROR("check_for_winner() invalid index check_this_column");
			break;
			}
		/*Begin with the checks, the mobile checks for vertical, horizontal and diagonal possibilities*/
		//check row
		if(check_this_column>=3 && check_this_column<=10 &&
			data->board_array[check_out_position][check_this_column-3] EQ check_with_this_color && 
			data->board_array[check_out_position][check_this_column-2] EQ check_with_this_color && 
			data->board_array[check_out_position][check_this_column-1] EQ check_with_this_color && 
			data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else if(check_this_column>=2 && check_this_column<=9 &&
				data->board_array[check_out_position][check_this_column-1] EQ check_with_this_color && 
				data->board_array[check_out_position][check_this_column-2] EQ check_with_this_color && 
				data->board_array[check_out_position][check_this_column+1] EQ check_with_this_color && 
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else if(check_this_column>=0 && check_this_column<=7 &&
				data->board_array[check_out_position][check_this_column+1] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column+2] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column+3] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else if(check_this_column>=1 && check_this_column<=8 &&
				data->board_array[check_out_position][check_this_column-1] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column+1] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column+2] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
			//check column
		}else if(check_out_position<=4 &&
				data->board_array[check_out_position+1][check_this_column] EQ check_with_this_color &&
				data->board_array[check_out_position+2][check_this_column] EQ check_with_this_color &&
				data->board_array[check_out_position+3][check_this_column] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
			//check diagonal
		}else if(check_out_position<=4 && check_this_column>=3 && check_this_column<=10 &&
				data->board_array[check_out_position+1][check_this_column-1] EQ check_with_this_color &&
				data->board_array[check_out_position+2][check_this_column-2] EQ check_with_this_color &&
				data->board_array[check_out_position+3][check_this_column-3] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else if(check_out_position<=4 && check_this_column>=0 && check_this_column<=7 &&
				data->board_array[check_out_position+1][check_this_column+1] EQ check_with_this_color &&
				data->board_array[check_out_position+2][check_this_column+2] EQ check_with_this_color &&
				data->board_array[check_out_position+3][check_this_column+3] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else if(check_out_position>=3 && check_this_column>=0 && check_this_column<=7 &&
				data->board_array[check_out_position-1][check_this_column+1] EQ check_with_this_color &&
				data->board_array[check_out_position-2][check_this_column+2] EQ check_with_this_color &&
				data->board_array[check_out_position-3][check_this_column+3] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else if(check_out_position>=3 && check_this_column>=3 && check_this_column<=10 &&
				data->board_array[check_out_position-1][check_this_column-1] EQ check_with_this_color &&
				data->board_array[check_out_position-2][check_this_column-2] EQ check_with_this_color &&
				data->board_array[check_out_position-3][check_this_column-3] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else if(check_out_position<=6 && check_out_position>=2 && check_this_column>=1 && check_this_column<=8 &&
				data->board_array[check_out_position+1][check_this_column-1] EQ check_with_this_color &&
				data->board_array[check_out_position-1][check_this_column+1] EQ check_with_this_color &&
				data->board_array[check_out_position-2][check_this_column+2] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else if(check_out_position<=5 && check_out_position>=1 && check_this_column>=2 && check_this_column<=9 &&
				data->board_array[check_out_position+1][check_this_column-1] EQ check_with_this_color &&
				data->board_array[check_out_position+2][check_this_column-2] EQ check_with_this_color &&
				data->board_array[check_out_position-1][check_this_column+1] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else if(check_out_position<=5 && check_out_position>=1 && check_this_column>=1 && check_this_column<=8 &&
				data->board_array[check_out_position-1][check_this_column-1] EQ check_with_this_color &&
				data->board_array[check_out_position+1][check_this_column+1] EQ check_with_this_color &&
				data->board_array[check_out_position+2][check_this_column+2] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else if(check_out_position>=2 && check_out_position<=6 && check_this_column>=2 && check_this_column<=9 &&
				data->board_array[check_out_position-1][check_this_column-1] EQ check_with_this_color &&
				data->board_array[check_out_position-2][check_this_column-2] EQ check_with_this_color &&
				data->board_array[check_out_position+1][check_this_column+1] EQ check_with_this_color &&
				data->board_array[check_out_position][check_this_column] EQ check_with_this_color){
			winner=check_with_this_color;	//set the current color to check for as the winner
			break;
		}else{
			winner='0';						//no win-moves found, so nobody is the winner and the game goes on
		}
		color_regulator++;					//set the check color one up (to black)
	}
}
/*******************************************************************************

 $Function:    	check_column

 $Description:	checks for already set stones
 
 $Returns:		int unused_position

 $Arguments:	T_four_in_a_row 
 
*******************************************************************************/
int check_column(T_four_in_a_row *data, int where_from)
{
	int unused_position;							//is for a free drop position
	int column_for_check;							//column (vertical board line) to check for
		
	if(where_from EQ 0){							//is the current game color(player) 0 then use column
		column_for_check=column;
	}else{											//is the current game color(player) 1 then use column_black
		column_for_check=column_black;
	}
	unused_position=-9999;							
	for(y_runer=0;y_runer<MAX_Y_BOARD;y_runer++){	//test all vertical fields whether they are empty('0') or not
		if(data->board_array[y_runer][column_for_check] EQ '0'){
			unused_position=y_runer;				//.this field is empty, so it is a unused position
		}
	}
	return unused_position;							//return the unused position
}


/*******************************************************************************

 $Function:    	mobile_thinking

 $Description:	mobile moves
 
 $Returns:		drop position of black stone (drop_me)

 $Arguments:	T_four_in_a_row 
 
*******************************************************************************/
int mobile_thinking(T_four_in_a_row *data)
{
	int position_score[11];									//this array takes the scoring values of each column in the game board
	int drop_position;										//drop position for stone
	int drop_me;											//buffer value
	int scores[4];											//this array keeps the scores for row, column and diagonals
	int add_random_score;

	TRACE_EVENT("mobile_thinking()");

	request_stone=1;										//black is
	//check for good spots to drop the black stone
	for(x_runer=0;x_runer<MAX_X_BOARD;x_runer++){
		//found the possible drop position
		column_black=x_runer;								//set the x_runer to the current column (column_black)
		drop_position=check_column(data,request_stone);		//check for possible drop positions
		if(drop_position>=0){								//if there is a possible drop position.....
			//check the scoring of the current point
			add_random_score=random_scoring();
			data->board_array[drop_position][x_runer]='B';				//set a test black stone 
			scores[0]=check_line(data,x_runer,drop_position,1,0,0);		//scores row
			scores[1]=check_line(data,x_runer,drop_position,1,1,0);		//scores diagonal right-up/left-down
			scores[2]=check_line(data,x_runer,drop_position,1,-1,0);	//scores diagonal left-up/right-down		
			scores[3]=check_line(data,x_runer,drop_position,0,1,0);		//scores down(column)
			position_score[x_runer]=scores[0]+scores[1]+scores[2]+scores[3];	//count all scores together to one total score
			if(drop_position-1 >=0){	//Is it possible to set a future stone here? If Yes go on...
				data->board_array[drop_position-1][x_runer]='W';		//set a test white stone for a possible next opponent move
				scores[0]=check_line(data,x_runer,drop_position-1,1,0,1);	//scores row
				scores[1]=check_line(data,x_runer,drop_position-1,1,1,1);	//scores diagonal right-up/left-down
				scores[2]=check_line(data,x_runer,drop_position-1,1,-1,1);	//scores diagonal left-up/right-down		
				scores[3]=check_line(data,x_runer,drop_position-1,0,1,1);	//scores down(column)
				position_score[x_runer]=position_score[x_runer]-scores[0]-scores[1]-scores[2]-scores[3]+add_random_score;	//substraction of the future score from the normal score
				data->board_array[drop_position-1][x_runer]='0';		//remove test white stone
			}
			data->board_array[drop_position][x_runer]='0';				//remove test black stone
		}else{												//if there is no possible drop position set the score to -1
			position_score[x_runer]=-9999;	
		}
	}
	
	drop_me=-9999;											//buffer is set to absolutely minus
	column_black=-9999;										//position of black cursor is left outer side of board
	for(x_runer=0;x_runer<MAX_X_BOARD;x_runer++){			//goes through the scoring array to find the highest score
		if(position_score[x_runer]>drop_me){
			drop_me=position_score[x_runer];
			column_black=x_runer;							//set the x_runer to the current column(column_black)
			drop_position=check_column(data,request_stone);	//this is the best drop position for the mobile
		}
	}
	if(column_black EQ -9999){								//if the column is -1, then the game board is filled up and nobody has won
		TRACE_EVENT("Board Full!!!");
		winner='N';											//set winner to nobody (N)
	}
	return drop_position;									//return the mobile drop position
}

/*******************************************************************************

 $Function:    	check_line

 $Description:	set score points for the mobile, so the mobile knows where to set the next stone
 
 $Returns:		total_score

 $Arguments:	T_four_in_a_row, x_value, drop_position, dx, dy, where_from
 
*******************************************************************************/
int check_line(T_four_in_a_row *data, int x_value, int y_value, int dx, int dy, BOOL where_from)
{
	int lSc,nSc,oSc,total_score,sx,sy,i,j;
	int pl_nSc; //no of empty squares that have a man under them (i.e. we/opponent can use next shot)
	total_score=0;
	for (i=0;i<4;i++){
		sx = x_value-i*dx;
		sy = y_value-i*dy;
		lSc=0;
		nSc=0;	pl_nSc=0;
		oSc=0;
		for (j=0;j<4;j++){
			if ((sx<0) || (sx>=MAX_X_BOARD) ||(sy<0) || (sy>=MAX_Y_BOARD)){ /*a0393213 lint warnings removal - 'Possible access of out-of-bounds pointer'*/
				lSc = -1;
			}else if (lSc>=0){
				if(where_from EQ 0){			
					if (data->board_array[sy][sx] EQ 'B'){
							lSc++;
					}else if (data->board_array[sy][sx] EQ '0'){
						nSc++;
						if ((sy EQ (MAX_Y_BOARD-1)) || (data->board_array[sy+1][sx] NEQ '0'))	/*a0393213 lint warnings removal - 'Possible access of out-of-bounds pointer'*/	
							pl_nSc++;
					}else if (data->board_array[sy][sx] EQ 'W'){
							oSc++;
						
					}else //edge of board found
						lSc=-1;
					sx = sx+dx;
					sy = sy+dy;
				}else{
					if (data->board_array[sy][sx] EQ 'W'){
							lSc++;
					}else if (data->board_array[sy][sx] EQ '0'){
						nSc++;
						if ((sy EQ (MAX_Y_BOARD-1)) || (data->board_array[sy+1][sx] NEQ '0'))	/*a0393213 lint warnings removal - 'Possible access of out-of-bounds pointer'*/	
							pl_nSc++;
					}else if (data->board_array[sy][sx] EQ 'B'){
							oSc++;
						
					}else //edge of board found
						lSc=-1;
					sx = sx+dx;
					sy = sy+dy;
				}
			}
		}
		if (lSc >= 0){
			if (lSc EQ 4){
				return(9999);//got 4 in a row.
			}else if (oSc EQ 3) //and ISc==1
			{
				// 3 opp men - good spot!
				if(where_from EQ 0){
					total_score = total_score + 1000; //this is for the normal, current position score
				}else{
					total_score = -total_score - 1000; //this is for the possible future score, it must be minus, because it will be substracted from normal score
				}
			}else if ((lSc EQ 3) && (nSc EQ 1)){
				//playing here will form 3 in a row and a blank - good spot
				if (pl_nSc EQ 0) //can't reach the 4th position
					total_score = total_score + 75;
				else
					total_score = total_score + 125;//better if we can reach 4th position
			}else if ((oSc EQ 2) && (nSc EQ 1)){
				// 2 opp men + 1 blank- good spot!
				if (pl_nSc EQ 0) //can't reach the 4th position-not too bad
					total_score = total_score + 50;
				else
					total_score = total_score + 200;//can get to both positions - have to block.
			}else if ((lSc EQ 2) && (nSc EQ 2)){
				// 2 in a row and 2 blanks - OK spot
				if (pl_nSc EQ 0) //can't reach the 3rd or 4th position-not too good
					total_score = total_score + 20;
				else if (pl_nSc EQ 1) //can reach one of the 3rd or 4th position - good
					total_score = total_score + 45;
				else if (pl_nSc EQ 2) //can reach both 3rd and 4th position - very good
					total_score = total_score + 95;
			}else if ((lSc EQ 1) && (nSc EQ 3)){
				// 1 in a row and 3 blanks - OK spot, better if we can reach the other 3 points.
				total_score = total_score + 2*(pl_nSc+1);	//pl_nSc=0,1,2 or 3 sc=sc+2,4,6 or 8
			}
			//else //line is neither good nor bad.
		}
		else //line is not valid
			total_score = total_score - 1;
	}
	return(total_score);
}
/*******************************************************************************

 $Function:    	check_line

 $Description:	generates a random number to add on the field scores
 
 $Returns:		random_score_result

 $Arguments:	
 
*******************************************************************************/
int random_scoring(void)
{
	return(rand()%100);
}



#define MAX_TET_X 11
#define MAX_TET_Y 20

typedef struct
{

	T_MMI_CONTROL   mmi_control;		// common control parameter
	T_MFW_HND win;
	T_MFW_HND hKbd;
	T_MFW_HND	menu;
	T_MFW_HND 	parent_win;	
	T_MFW_HND       info_tim;

	int dropTime;
	int maxDropTime;
	int timeStep;

	int xPos;
	int yPos;

	int px;
	int py;
	int shapeId;
	char shapeStr[20];
	int rotateId;

	int score;
	int level;
	int linesToGo;

	int resetSpeed;

	int gameOver;

	char tetrisScreen[MAX_TET_X][MAX_TET_Y];

} T_TETRIS;


static MfwHnd TetrisCreate(MfwHnd hParentWin);
static void TetrisExecCb (MfwHnd hWin, USHORT uiE, SHORT iValue, void *pParameter);
static int TetrisKbdCb (MfwEvt uiE, MfwKbd *psK);
static int  tetris_tim_cb(MfwEvt e, MfwTim *t);
static int TetrisWinCb (MfwEvt uiE, MfwWin *psWin);


#define E_INIT 0x5000
#define E_EXIT 0x5001

void initTetGame(T_TETRIS *data)
{
	int x,y;
	for (y=0;y<MAX_TET_Y;y++)
	{
		for (x=0;x<MAX_TET_X;x++)
			data->tetrisScreen[x][y]=' ';
		data->tetrisScreen[0][y]='#';
		data->tetrisScreen[MAX_TET_X-1][y]='#';

	}
	for (x=0;x<MAX_TET_X;x++)
		data->tetrisScreen[x][MAX_TET_Y-1]='#';
}

#define MAX_SHAPE 7
void tetris_getShape(int shapeId, int rotate, char* shapeStr)
{
	int j,k;
	int index;
	const char* tetShape[4*4*MAX_SHAPE] = 
		{	"    ", " A  ",	"  B ",	" C  ",	"    ",	"F   ",	"  G ",	
			"EEEE", " AA ",	" BB ",	"CCC ",	" DD ",	"FFF ",	"GGG ",
			"    ", "  A ",	" B  ",	"    ",	" DD ",	"    ",	"    ", 
			"    ", "    ",	"    ",	"    ",	"    ",	"    ",	"    ",
			" E  ", " AA ",	"BB  ",	" C  ",	"    ",	" FF ",	" G  ",	
			" E  ", "AA  ",	" BB ",	" CC ",	" DD ",	" F  ",	" G  ",
			" E  ", "    ",	"    ",	" C  ",	" DD ", " F  ",	" GG ", 
			" E  ", "    ",	"    ",	"    ",	"    ",	"    ",	"    ",
			"    ", " A  ",	"  B ",	"    ",	"    ",	"    ",	"    ",	
			"EEEE", " AA ",	" BB ",	"CCC ",	" DD ",	"FFF ",	"GGG ",
			"    ", "  A ",	" B  ",	" C  ",	" DD ",	"  F ",	"G   ", 
			"    ", "    ",	"    ",	"    ",	"    ",	"    ",	"    ",
			" E  ", " AA ",	"BB  ",	" C  ",	"    ",	" F  ",	"GG  ",	
			" E  ", "AA  ",	" BB ",	"CC  ",	" DD ",	" F  ",	" G  ",
			" E  ", "    ",	"    ",	" C  ",	" DD ",	"FF  ",	" G  ", 
			" E  ", "    ",	"    ",	"    ",	"    ",	"    ",	"    "
		};


	for (k=0;k<4;k++)
	{
		index = rotate*4*MAX_SHAPE+k*MAX_SHAPE;
		for (j=0;j<4;j++)
			shapeStr[j+k*4] = tetShape[index+shapeId][j];
	}
}

int tetris_getNextShapeId( void )
{
	static int shapeIndex = 0;
	shapeIndex = (shapeIndex+1) % MAX_SHAPE;
	return (shapeIndex);
}



void tetris_getNextShape(T_TETRIS *data)
{
	data->shapeId  = tetris_getNextShapeId( );
	data->rotateId = 0;
	tetris_getShape(data->shapeId, data->rotateId, data->shapeStr);
	data->px = (MAX_TET_X-2)/2;
	data->py = 0;
}

void tetris_getNextLevel(T_TETRIS *data)
{
	initTetGame(data);
	tetris_getNextShape(data);
	data->level = data->level+1;
	data->linesToGo = 5*(data->level+1);
    data->resetSpeed = 0;
   	data->maxDropTime = (120-data->level*5);    		
}

void initTetrisData( T_TETRIS *data )
{
	data->level = 0;
	data->score = 0;
	data->gameOver = 0;
	tetris_getNextLevel(data);
}

void dspl_FillRect( int x1, int y1, int sx, int sy)
{
	dspl_DrawFilledBgdRect(x1,y1,x1+sx,y1+sy);
}

void tetris_drawScreen( int allScreen, T_TETRIS *data)
{
	int x1,y1,x2,y2;
	int bCol;
	int xOfs =5; 
	int yOfs=10;
	int xOfs2=30;
	int yOfs2=10;
	int sx,sy;
	int px,py;
//	U32 oldfCol=0;   // RAVI
//	U32 oldbCol=0;  // RAVI
	int x = data->px;
	int y = data->py;
	
//	char dbg[80];  // RAVI

	
#ifdef LSCREEN
	//Calculate size based on the screen properties
	sx = (SCREEN_SIZE_X-xOfs-xOfs2)/MAX_TET_X;
	sy = (SCREEN_SIZE_Y-yOfs-yOfs2-Mmi_layout_softkeyHeight())/MAX_TET_Y;
#else
	//Screen is very small - ensure shapes are as large as possible
	sy = SCREEN_SIZE_Y/MAX_TET_Y;
	sx = SCREEN_SIZE_X/MAX_TET_X;
	if (sx >sy+1)
		sx = sy+1;
	xOfs = 5;
	xOfs2 = SCREEN_SIZE_X - (sx*MAX_TET_X+xOfs);
	yOfs = 0;
	yOfs2 = SCREEN_SIZE_Y - (sy*MAX_TET_Y+xOfs);
#endif
//allScreen= 1;

	resources_setColour(COLOUR_GAME);

	if (allScreen)
	{	//redraw everything
		x1 = 0;
		y1 = 0;
		x2 = MAX_TET_X;
		y2 = MAX_TET_Y;
	
	}
	else //just draw+1 box around current man
	{
		x1 = x-1;
		y1 = y-1;
		x2 = x+5;
		y2 = y+4;
		if (x1<1)x1=1;
		if (y1<0)y1=0;
		if (x2>MAX_TET_X-1) x2=MAX_TET_X-1;
		if (y2>MAX_TET_Y-1) y2=MAX_TET_Y-1;
	}
	
#ifndef COLOURDISPLAY
	dspl_Clear  (xOfs+x1*sx,yOfs+y1*sy,xOfs+x2*sx-1,yOfs+y2*sy-1);
#endif
	for (px = x1;px<x2;px++)
	{
		for (py = y1;py<y2;py++)
		{
			char boardChr = data->tetrisScreen[px][py];
			if ((px >= x ) && (px < x+4) && 
			    (py >= y ) && (py < y+4)) 
			{
				int shapeX,shapeY;
				shapeX = px-x;
				shapeY = py-y;
				if (data->shapeStr[shapeX+shapeY*4] != ' ')
					boardChr = data->shapeStr[shapeX+shapeY*4];
			}
#ifdef COLOURDISPLAY
			switch (boardChr)
			{
			case '#':	bCol = 0x00FFFFFF;	break;
			case ' ':	bCol = 0x00404040;	break;
			case 'A':	bCol = 0x000000FF;	break;
			case 'B':	bCol = 0x0000FF00;	break;
			case 'C':	bCol = 0x00FF0000;	break;
			case 'D':	bCol = 0x0000FFFF;	break;
			case 'E':	bCol = 0x00FF00FF;	break;
			case 'F':	bCol = 0x00FFFF00;	break;
			default:	bCol = 0x00FFFFFF;	break;
			}
			dspl_SetBgdColour(bCol);
			dspl_FillRect(xOfs+px*sx,yOfs+py*sy,sx,sy);
//			dspl_SetFgdColour(0x00802040);
			if (boardChr != ' ')
				dspl_DrawRect (xOfs+px*sx,yOfs+py*sy,xOfs+px*sx+sx-1,yOfs+py*sy+sy-1);
#else
			if (boardChr != ' ')
				dspl_DrawRect (xOfs+px*sx,yOfs+py*sy,xOfs+px*sx+sx-1,yOfs+py*sy+sy-1);
#endif
		}
	}
	if (data->gameOver)
	{
		int oldfCol = dspl_SetFgdColour(0x00FFFF00);
		dspl_SetBgdColour(0x008000FF);
		dspl_FillRect(xOfs+sx*3-5,yOfs+sy*3-5, xOfs+sx*3+5+6*4,yOfs+sy*3+5+18);
		dspl_TextOut(xOfs+sx*3,yOfs+sy*3,    0, "GAME");
		dspl_TextOut(xOfs+sx*3,yOfs+sy*3+Mmi_layout_line_height()+4, 0, "OVER");
		dspl_SetFgdColour(oldfCol);

	}
	resources_restoreColour();
}



int tetris_addShapeToScreen(T_TETRIS *data)
{
	int x,y;
	for (x=0;x<4;x++)
		for (y=0;y<4;y++)
			if (data->shapeStr[x+y*4] != ' ')
			{
				if (data->tetrisScreen[data->px+x][data->py+y]!= ' ')
					return(1);
				else
					data->tetrisScreen[data->px+x][data->py+y] = data->shapeStr[x+y*4];
			}
	return(0);
	
}

int tetris_testForCompleteLines( T_TETRIS *data )
{
	int nLines=0;
	int x,y;
	int blanks;
	for (y=MAX_TET_Y-2;y>0;y--)
	{
		blanks = 0;
		
		for (x=1;x<MAX_TET_X-1;x++)
		{
			if (data->tetrisScreen[x][y] == ' ')
				blanks++;
			if (nLines >0)
				data->tetrisScreen[x][y+nLines] = data->tetrisScreen[x][y];
		}
		if (blanks==0)
			nLines++;

	}
	return (nLines);
}


void tetris_testGameOver( T_TETRIS *data )
{
	int linesComplete;

	data->gameOver = tetris_addShapeToScreen(data);
	if (!data->gameOver)
	{

		linesComplete = tetris_testForCompleteLines(data);
		if (linesComplete >0)
		{
			data->score		= data->score + linesComplete*linesComplete;
			data->linesToGo = data->linesToGo - linesComplete;
			if (data->linesToGo <0)
				tetris_getNextLevel(data);
		}
		tetris_getNextShape(data);	
	}
}





void tetris_destroy (T_MFW_HND own_window)
{
  T_MFW_WIN     * win  = ((T_MFW_HDR *)own_window)->data;
  T_TETRIS  * data   = (T_TETRIS *)win->user;

  TRACE_EVENT ("tetris_destroy()");

  if (own_window == NULL)
  {
	TRACE_EVENT ("Error :- Called with NULL Pointer");
	return;
  }

  if (data)
  {
       /*
       * Exit ICON & KEYBOARD Handle
       */
       kbdDelete (data->hKbd);
       timDelete(data->info_tim);
       /*
       * Delete WIN Handler
       */
       winDelete (data->win);
       /*
       * Free Memory
       */
       FREE_MEMORY ((void *)data, sizeof (T_TETRIS));
 }
}



int MmiTetrisStart(void )
{
    	MfwHnd hWin;

	MfwHnd hParentWin = mfwParent(mfwHeader());
	
    	TRACE_FUNCTION ("TetrisStart()");

    	hWin = TetrisCreate (hParentWin);

   	if (hWin NEQ NULL)
	{
	    	SEND_EVENT (hWin, E_INIT, NULL, NULL);
	}
	
    	return 1;
}

void initTetrisData( T_TETRIS *data );
static MfwHnd TetrisCreate(MfwHnd hParentWin)
{
	T_TETRIS *psData = (T_TETRIS *)ALLOC_MEMORY (sizeof( T_TETRIS));
    MfwWin *psWin;
	MfwWinAttr *win_attr;

   	TRACE_FUNCTION ("TetrisCreate()");

   	if (psData == NULL)
   	{
		TRACE_EVENT ("ALLOC_MEMORY() failed");
   		return NULL;
   	}

	win_attr = (MfwWinAttr *) ALLOC_MEMORY (sizeof(MfwWinAttr));
	if (win_attr)
	{
		win_attr->win.px = 0;
		win_attr->win.py = 0;
		win_attr->win.sx = SCREEN_SIZE_X;
		win_attr->win.sy = SCREEN_SIZE_Y;
	}
   	psData->win = winCreate (hParentWin, win_attr, E_WIN_VISIBLE, (T_MFW_CB)TetrisWinCb);
    if (psData->win == NULL)
	{
		mfwFree((U8*) win_attr, sizeof(MfwWinAttr));
		FREE_MEMORY((void *)psData, sizeof (T_TETRIS));
	    	return NULL;
	}
    	psData->mmi_control.dialog = (T_DIALOG_FUNC)TetrisExecCb;
    	psData->mmi_control.data   = psData;
    	psData->timeStep = 150;
    	psData->maxDropTime = 120;
    	psData->dropTime = psData->maxDropTime;
    	psWin = ((MfwHdr *)psData->win)->data;
    	psWin->user = (void *)psData;
    	psData->parent_win = hParentWin;
		initTetrisData( psData );

    	return psData->win;
}

static int TetrisWinCb (MfwEvt uiE, MfwWin *psWin)
{
    	T_TETRIS *psData = (T_TETRIS *)psWin->user;
	
   	TRACE_FUNCTION ("TetrisWinCb()");
	
    switch (uiE)
	{
       case MfwWinVisible:  
			dspl_ResetWindow();    			
			dspl_SetBgdColour(0x000000FF);
			dspl_Clear(0,0,SCREEN_SIZE_X-1,SCREEN_SIZE_Y-1 );
			tetris_drawScreen( 1, psData);

	    	break;
	    	
	case MfwWinFocussed: 
	case MfwWinDelete:   
       default:
	    	return MFW_EVENT_REJECTED;
	}
   TRACE_FUNCTION ("TetrisWinCb() - end");
	return MFW_EVENT_CONSUMED;
}

static void TetrisExecCb (MfwHnd hWin, USHORT uiE, SHORT iValue, void *pParameter)
{
	MfwWin *psWin = ((MfwHdr *) hWin)->data;
	T_TETRIS *psData = (T_TETRIS *)psWin->user;

	TRACE_FUNCTION ("TetrisExecCb()");

	switch (uiE)
	{
	case E_INIT:
		
          psData->info_tim  = timCreate(hWin, psData->timeStep, (MfwCb)tetris_tim_cb);
		  psData->hKbd = kbdCreate(psData->win, KEY_ALL, (MfwCb)TetrisKbdCb);
	      timStart(psData->info_tim);

		/*  */
	       winShow(hWin);
		
	       break;

    case E_EXIT:
            tetris_destroy(hWin);
            break;
		    	
	default:
	       return;
	}

	return;
}


static int tetris_checkPos( T_TETRIS *data, int px, int py, char* shape)
{
	int x,y;
	for (x=0;x<4;x++)
		for (y=0;y<4;y++)
			if (shape[x+y*4] != ' ')
			{
				if (data->tetrisScreen[px+x][py+y]!= ' ')
					return(1);
			}
	return(0);
}

static int tetris_moveDown(T_TETRIS *data,int *px, int *py)
{
	int cannotMove;
	cannotMove = tetris_checkPos(data,*px,*py+1,data->shapeStr);
	if (cannotMove)
		return (1);
	else
		*py = *py+1;
	return (0);
}
static int tetris_moveLeft(T_TETRIS *data, int *px, int *py)
{
	int cannotMove;
	cannotMove = tetris_checkPos(data,*px-1,*py,data->shapeStr);
	if (cannotMove)
		return (1);
	else
		*px = *px-1;
	return (0);
}
static int tetris_moveRight( T_TETRIS *data, int *px, int *py)
{
	int cannotMove;
	cannotMove = tetris_checkPos(data,*px+1,*py,data->shapeStr);
	if (cannotMove)
		return (1);
	else
		*px = *px+1;
	return (0);
}

static int tetris_rotateLeft( T_TETRIS *data, int *px, int *py )
{
	int i;
	int cannotMove;
	char tmpShape[20];
	tetris_getShape(data->shapeId,(data->rotateId+1)%4,tmpShape);
	cannotMove = tetris_checkPos(data,*px,*py,tmpShape);
	if (cannotMove)
		return (1);
	for (i=0;i<16;i++)
		data->shapeStr[i] = tmpShape[i];
	data->rotateId = (data->rotateId+1)%4;
	return (0);
}

static int tetris_rotateRight( T_TETRIS *data, int *px, int *py )
{
	int i;
	int cannotMove;
	char tmpShape[20];
	tetris_getShape(data->shapeId,(data->rotateId+3)%4,tmpShape);
	cannotMove = tetris_checkPos(data,*px,*py,tmpShape);
	if (cannotMove)
		return (1);
	for (i=0;i<16;i++)
		data->shapeStr[i] = tmpShape[i];
	data->rotateId = (data->rotateId+3)%4;
	return (0);
}
#define KEY_TET_MOVELEFT    KCD_1
#define KEY_TET_MOVERIGHT   KCD_3
#define KEY_TET_ROTATELEFT  KCD_4
#define KEY_TET_ROTATERIGHT KCD_6
#define KEY_TET_DROPALL		  KCD_5
#define KEY_TET_DROP		  KCD_2


static int TetrisKbdCb (MfwEvt uiE, MfwKbd *psK)
{
    	MfwHnd hWin = mfwParent(mfwHeader());
    	MfwWin *psWin = ((MfwHdr *)hWin)->data;
    	T_TETRIS *psData = (T_TETRIS *)psWin->user;
    	int cannotMove=0;


    	TRACE_FUNCTION ("TetrisKbdCb()");
		/***************************Go-lite Optimization changes Start***********************/
		//	Aug 16, 2004    REF: CRR 24323   Deepa M.D
		TRACE_EVENT_P1 ("Code : %d",(int)psK->code);
		/***************************Go-lite Optimization changes Start***********************/


	switch(psK->code )
	{
		case KEY_TET_MOVELEFT:
		case KCD_MNULEFT:
			tetris_moveLeft(psData,&psData->px,&psData->py);
			break;
		case KEY_TET_MOVERIGHT:
		case KCD_MNURIGHT:
			tetris_moveRight(psData,&psData->px,&psData->py);
			break;
		case KEY_TET_ROTATELEFT:
			tetris_rotateLeft(psData,&psData->px,&psData->py);
			break;
		case KCD_MNUSELECT:
		case KEY_TET_ROTATERIGHT:
			tetris_rotateRight(psData,&psData->px,&psData->py);
			break;
		case KEY_TET_DROP:
			cannotMove = tetris_moveDown(psData,&psData->px,&psData->py);
			if (cannotMove)
				tetris_testGameOver( psData );
			break;
		case KEY_TET_DROPALL:
			while(tetris_moveDown(psData,&psData->px,&psData->py) ==0)
			{ //loop until we hit the bottom
			
			}
			cannotMove = 1;
			tetris_testGameOver( psData );
			break;


		case KCD_RIGHT: 	
            SEND_EVENT (hWin, E_EXIT, 0, 0);
			return MFW_EVENT_CONSUMED;

		default: 
			
		    	return MFW_EVENT_CONSUMED;
	}
	tetris_drawScreen(cannotMove, psData);
    	
    	TRACE_FUNCTION ("TetrisKbdCb()-end");
    	return MFW_EVENT_CONSUMED;
}

static int  tetris_tim_cb(MfwEvt e, MfwTim *t)
{
      T_MFW_HND       win  = mfw_parent (mfw_header());
      T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
      T_TETRIS   * data = (T_TETRIS *)win_data->user;
  	  static int nCycles = 0;
	  int cannotMove;

  		char bfr[80];
  		nCycles = nCycles+ data->timeStep;
  		if (nCycles > 1000)
  		{
  			nCycles = nCycles - 1000;
  			data->maxDropTime = data->maxDropTime-1;
		}
      TRACE_EVENT("tetris_tim_cb");
    	data->dropTime = data->dropTime - data->timeStep;
    	if (data->dropTime < 0)
    	{
    		if (data->maxDropTime > 90)
		    	data->dropTime = data->dropTime+75*10;
    		else if (data->maxDropTime > 60)
		    	data->dropTime = data->dropTime+(60+(data->maxDropTime-60)/2)*10;
    		else if (data->maxDropTime > 30)
		    	data->dropTime = data->dropTime+data->maxDropTime*10;
    		else 
		    	data->dropTime = data->dropTime+(data->maxDropTime+90)/4*10;
    			
    		if (data->dropTime < data->timeStep) 
    			data->dropTime = data->timeStep;
    		else if (data->dropTime < data->timeStep) 
    			data->dropTime = data->timeStep;
    		
			resources_setColour(COLOUR_GAME);
			sprintf(bfr,"%d ",data->score);
			dspl_TextOut(SCREEN_SIZE_X-30 ,10, 0, bfr);
			sprintf(bfr,"%d ",data->linesToGo);
			dspl_TextOut(SCREEN_SIZE_X-30 ,12+Mmi_layout_line_height(), 0, bfr);
			resources_restoreColour();
			cannotMove = tetris_moveDown(data,&data->px,&data->py);
			tetris_getNextShapeId( );//To randomize it
	
			if (cannotMove)
			{
				tetris_testGameOver( data );
			}
			tetris_drawScreen(cannotMove, data);
		}
   		
      timStart(data->info_tim);
      TRACE_EVENT("tetris_tim_cb-end");
      return 1;
}

T_MFW_HND Game_Info(T_MFW_HND parent_window)
{
  	T_MFW_HND       	win           = ShowGame_Information(parent_window);
	TRACE_FUNCTION("ShowVerion()");
    if (win NEQ NULL)
        {
           	SEND_EVENT (win, SHOWGAMEINFO_INIT, 0, 0);
        }

   return win;
}

static T_MFW_HND ShowGame_Information(MfwHnd parent_window)
{
	tShowInfo* data = (tShowInfo*)ALLOC_MEMORY (sizeof (tShowInfo));
	T_MFW_WIN* win;

	if (data EQ NULL)
	{
		return NULL;
	}

	// Create the window handler
	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, NULL);
	if (data->win EQ NULL)
	{
		return NULL;
	}

	// connect the dialog data to the MFW window
	data->mmi_control.dialog = (T_DIALOG_FUNC)ShowGame_DialogCB;
	data->mmi_control.data  = data;

	win                    = ((T_MFW_HDR *)data->win)->data;
	win->user              = (void *)data;
	data->parent_win        = parent_window;

	return data->win;
}

static void ShowGame_DialogCB(T_MFW_HND win, USHORT e, SHORT identifier, void *parameter)
{
	T_MFW_WIN		*win_data = ((T_MFW_HDR *) win)->data;
	tShowInfo* data = (tShowInfo*) win_data->user;
	/* SPR#1428 - SH - New editor data */
#ifdef NEW_EDITOR
//	T_AUI_EDITOR_DATA editor_data;  // RAVI
#else /* NEW_EDITOR */
//	T_EDITOR_DATA editor_data;   // RAVI
#endif /* NEW_EDITOR */

	switch (e)
	{
		case SHOWGAMEINFO_INIT:
		{
			strcat(data->buffer, info);

			ShowGameInfoEditor(win);

			/* SPR#1428 - SH - New editor changes */
#ifdef NEW_EDITOR
			data->editor_data.Callback	= (T_AUI_EDIT_CB)ShowInfoCB;
			AUI_edit_Start(win, &(data->editor_data));
#else /* NEW_EDITOR */
			data->editor_data.Callback	= (T_EDIT_CB)ShowInfoCB;
			editor_start(win, &(data->editor_data));
#endif /* NEW_EDITOR */

			winShow(data->win);
		}
		break;

		default:
		{
			TRACE_EVENT("Show Game Info : Unknown Event");
		}
		break;
	}
}

void ShowGameInfoEditor (T_MFW_HND win)
{
	T_MFW_WIN *win_data = ((T_MFW_HDR *) win)->data;
	tShowInfo* data = (tShowInfo*) win_data->user;

/* SPR#1428 - SH - New editor changes */
#ifdef NEW_EDITOR
	AUI_edit_SetDefault(&data->editor_data);
	AUI_edit_SetDisplay(&data->editor_data, NULL, COLOUR_EDITOR, EDITOR_FONT);
	AUI_edit_SetTextStr(&data->editor_data, TxtSoftBack, TxtNull, TxtNull, NULL);
	AUI_edit_SetEvents(&data->editor_data, 0, TRUE, FOREVER, NULL);
	AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE *)data->buffer, 35);
	AUI_edit_SetMode(&data->editor_data, ED_MODE_READONLY, ED_CURSOR_NONE);
#else /* NEW_EDITOR */
	editor_attr_init(&((data->editor_data).editor_attr), NULL, edtCurNone, 0, (char*)data->buffer, 35, COLOUR_EDITOR);
	editor_data_init(&data->editor_data, NULL, TxtSoftBack, TxtNull, 0, 1, READ_ONLY_MODE, FOREVER);

	data->editor_data.hide			= FALSE;
	data->editor_data.Identifier	= 0;
	data->editor_data.TextString	= NULL;
	data->editor_data.destroyEditor	= TRUE;
#endif /* NEW_EDITOR */
}

static void ShowInfoCB( T_MFW_HND win, USHORT Identifier,UBYTE reason)
{
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tShowInfo*data       = (tShowInfo*) win_data->user;   

    
    TRACE_FUNCTION("ShowVerionCB()");

	switch (reason )
    {
      case INFO_KCD_LEFT:
	  case INFO_KCD_HUP:
      case INFO_KCD_RIGHT:

        default:
		showGameInfo_destroy(data->win);
		break;
	}
}

/*******************************************************************************

 $Function:    	showVersion_destroy

 $Description:	Destroys the editor
 		
 $Returns:		

 $Arguments:	
 				
*******************************************************************************/
void showGameInfo_destroy(MfwHnd own_window)
{
  T_MFW_WIN * win_data;
  
  tShowInfo* data = NULL;

	if (own_window == NULL)
	{
		TRACE_EVENT ("Error : showGameInfo_destroy called with NULL Pointer");
		return;
	}


	win_data = ((T_MFW_HDR *)own_window)->data;
		if (win_data != NULL)
			data = (tShowInfo*)win_data->user;

	  if (data)
		{
		TRACE_EVENT ("calc_destroy()");

			win_delete (data->win);
		  // Free Memory
			FREE_MEMORY ((void *)data, sizeof (tShowInfo));

		}
	  else
		{
			TRACE_EVENT ("calc_destroy() called twice");
		}
}
#endif

