/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Calculator
 $File:		    MmiCalculatorMain.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description

    This provides the main calculator functionality

  
********************************************************************************
 $History: MmiCalculatorMain.c

	25/10/00			Original Condat(UK) BMI version.	

	xrashmic 20 Oct, 2005 MMI-SPR-33845
	To display BACK softkey when no more character are left in the calulator editor

	Oct 20, 2004    REF: CRR 25755 xnkulkar
	Description: calculator -divide by zero operation is allowed.
	Fix:	Display "Not Allowed" message when user tries to divide a number by 0 and do not perform the 
			     division. Also, show the editor again so that user can enter a number other than 0.
	   
 $End

*******************************************************************************/
#define MAX_CALC_TOTAL 999999999
#define MIN_CALC_TOTAL -99999999

/******************************************************************************
                                                                              
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
#include "message.h"
#include "prim.h"
#include "aci_cmh.h"


#include "mfw_mfw.h"
#include "mfw_win.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_lng.h"
#include "mfw_sat.h"
#include "mfw_kbd.h"
#include "mfw_nm.h"
#include "mfw_sms.h"

#include "dspl.h"

#include "MmiMmi.h"
#include "MmiDummy.h"
#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiIdle.h"
#include "MmiSoftKeys.h"
#include "Mmiicons.h"
#include "MmiMenu.h"
#include "MmiMain.h"
#include "MmiStart.h"
#include "MmiPins.h"
#include "MmiSettings.h"

/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#include "AUICalcEditor.h"
#else
#include "MmiEditor.h"
#endif
#include "MmiBookShared.h"
#include "mmiSmsMenu.h"
#include "MmiCalculatorMain.h"
#include "mmiColours.h"

#include "cus_aci.h"

#include "pcm.h"


/*******************************************************************************
                                                                              
                                Function Prototypes
                                                                              
*******************************************************************************/
static T_MFW_HND calc_create(MfwHnd parent_window);
void calc_destroy(MfwHnd own_window);
static void calcGetNumberCB( T_MFW_HND win, USHORT Identifier, SHORT reason );
static void calcGetOperandCB( T_MFW_HND win, USHORT Identifier, SHORT reason );
static void calc_DialogCB( T_MFW_HND win, USHORT event, SHORT identifier, void *parameter);
T_MFW_HND calc_start(T_MFW_HND parent_window);
double calcGetRunningTotal(void);
char operatorSymbol(UBYTE operator);
static void calc_menu_cb(T_MFW_HND parent_win, UBYTE identifier, UBYTE reason);
/*******************************************************************************
                                                                              
                                Static Global Variable(s) 
                                                                              
*******************************************************************************/	

static double running_total;

static T_MFW_HND calculator_win = NULL;


// Oct 20, 2004    REF: CRR 25755 xnkulkar
// Added this flag to detect 'divide by zero' operation
BOOL divideByZero = FALSE; 

/*******************************************************************************
                                                                              
                                Public Functions
                                                                              
*******************************************************************************/
/*******************************************************************************

 $Function:    	calculator

 $Description:	Starts the calculator function on user selection
 
 $Returns:		MFW_EVENT_CONSUMED if event handled, otherwise
				MFW_EVENT_PASSED

 $Arguments:	menu, menu item
 
*******************************************************************************/
int calculator(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND	    	parent       = mfwParent( mfw_header());
    TRACE_FUNCTION("calculator()");
    calc_start(parent);
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	calcPlus

 $Description:	Handles the selction of the "Plus" option in the calculator menu
 
 $Returns:		MFW_EVENT_CONSUMED if event handled, otherwise
				MFW_EVENT_PASSED

 $Arguments:	menu, menu item
 
*******************************************************************************/
int calcPlus(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_WIN*			win_data;
	tCalcData*			data;

    TRACE_FUNCTION("calcPlus()");
	//if new window successfully created
    if (calculator_win NEQ NULL)
        {	win_data   = ( (T_MFW_HDR *) calculator_win )->data;
        	data = (tCalcData*)win_data->user;
        	data->operation = PLUS;//set the arithmetic operation
           	SEND_EVENT (calculator_win, CALC_ENTER_OPERAND, 0, 0);
        } 
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	calcMinus

 $Description:	Handles the selection of the "Minus" option in the calculator menu
 
 $Returns:		MFW_EVENT_CONSUMED if event handled, otherwise
				MFW_EVENT_PASSED

 $Arguments:	menu, menu item
 
*******************************************************************************/
int calcMinus(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_WIN*			win_data;
	tCalcData*			data;

	//if new window successfully created
    TRACE_FUNCTION("calcMinus()");
    if (calculator_win NEQ NULL)
        {	win_data   = ( (T_MFW_HDR *) calculator_win )->data;
        	data = (tCalcData*)win_data->user;
        	data->operation = MINUS;//set the arithmetic operation
           	SEND_EVENT (calculator_win, CALC_ENTER_OPERAND, 0, 0);
        } 
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	calcDivide

 $Description:	Handles the selection of the "Divide" option in the calculator menu
 
 $Returns:		MFW_EVENT_CONSUMED if event handled, otherwise
				MFW_EVENT_PASSED

 $Arguments:	menu, menu item
 
*******************************************************************************/
int calcDivide(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_WIN*			win_data;
	tCalcData*			data;

    TRACE_FUNCTION("calcDivide()");
	//if new window successfully created
    if (calculator_win NEQ NULL)
        {	win_data   = ( (T_MFW_HDR *) calculator_win )->data;
        	data = (tCalcData*)win_data->user;
        	data->operation = DIVIDE;//set the arithmetic operation
           	SEND_EVENT (calculator_win, CALC_ENTER_OPERAND, 0, 0);
        } 
    return MFW_EVENT_CONSUMED;
}
/*******************************************************************************

 $Function:    	calcMultiply

 $Description:	Handles the selection of the "Multiply" option in the calculator menu
 
 $Returns:		MFW_EVENT_CONSUMED if event handled, otherwise
				MFW_EVENT_PASSED

 $Arguments:	menu, menu item
 
*******************************************************************************/
int calcMultiply(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_WIN*			win_data;
	tCalcData*			data;

    TRACE_FUNCTION("calcMultiply()");
	//if new window successfully created
    if (calculator_win NEQ NULL)
        {	win_data   = ( (T_MFW_HDR *) calculator_win )->data;
        	data = (tCalcData*)win_data->user;
        	data->operation = MULTIPLY;//set the arithmetic operation
           	SEND_EVENT (calculator_win, CALC_ENTER_OPERAND, 0, 0);
        } 
    return MFW_EVENT_CONSUMED;
}

// Oct 20, 2004    REF: CRR 25755 xnkulkar
/*******************************************************************************

 $Function:    	info_screen_cb

 $Description:	Callback function for info screen
 
 $Returns:	nothing

 $Arguments:	Parent Window handle, identifier and reason
 
*******************************************************************************/

static void info_screen_cb(T_MFW_HND parent_win, UBYTE identifier, UBYTE reason)
{
	// Send CALC_ENTER_OPERAND event tp prompt the user to enter the operand again
 	SEND_EVENT (calculator_win, CALC_ENTER_OPERAND, 0, 0);
}

/*******************************************************************************

 $Function:    	calcEquals

 $Description:	Handles the selection of the "Equals" option in the calculator menu
 
 $Returns:		MFW_EVENT_CONSUMED if event handled, otherwise
				MFW_EVENT_PASSED

 $Arguments:	menu, menu item
 
*******************************************************************************/
int calcEquals(MfwMnu* m, MfwMnuItem* i)
{
    TRACE_FUNCTION("calcEquals()");

	// Oct 20, 2004    REF: CRR 25755 xnkulkar
	// if 'divide by zero' operation is attempted 
	if(divideByZero == TRUE)
	{
     		divideByZero = FALSE;	// disable the flag
       	info_screen(0, TxtNotAllowed, TxtNull,(T_VOID_FUNC) info_screen_cb); // display "Not Allowed" message
		return MFW_EVENT_CONSUMED; // consume the event
       }

       if (calculator_win NEQ NULL)
       {	
       	SEND_EVENT (calculator_win, CALC_DISPLAY_RESULT, 0, 0);
       } 

       return MFW_EVENT_CONSUMED;
}

/*******************************************************************************
                                                                              
                                Private Functions
                                                                              
*******************************************************************************/	
/*******************************************************************************

 $Function:    	calcGetRunningTotal

 $Description:	If the calculator's running total value ever needs to be accessed by 
 				code in another file, then this function ought to be made public to prevent 
 				to allow safe access;
 
 $Returns:		Calculator's running total (double)

 $Arguments:	none
 
*******************************************************************************/
double calcGetRunningTotal(void)
{
    TRACE_FUNCTION("calcGetRunningTotal()");
	return running_total;
}

/*******************************************************************************

 $Function:    	operatorSymbol

 $Description:	returns character corresponding to arithmetic operation 
 
 $Returns:		Operator character

 $Arguments:	Operator enum
 
*******************************************************************************/
char operatorSymbol(UBYTE operator)
{
    TRACE_FUNCTION("operatorSymbol()");
	switch (operator)
	{
		case (PLUS): 	return '+';  //  break;  // RAVI
		case (MINUS):	return '-'; // break;  // RAVI
		case (MULTIPLY): return '*'; // break;  // RAVI
		case (DIVIDE):	return '/'; // break;  // RAVI
		default: 		return '\0'; // break;  // RAVI
	}
		
}


/*******************************************************************************

 $Function:    	calc_start

 $Description:	Creates the calculator window and calls for it to be initialised 
 
 $Returns:		Window

 $Arguments:	Parent Window
 
*******************************************************************************/

T_MFW_HND calc_start(T_MFW_HND parent_window)
{
  	T_MFW_HND       	win           = calc_create(parent_window);
	TRACE_FUNCTION("calc_start()");
    if (win NEQ NULL)
        {
           	SEND_EVENT (win, CALC_INIT, 0, 0);
        } 

   return win;
}

/*******************************************************************************

 $Function:    	calc_create

 $Description:	Creates the calculator window and connect the dialog data to it
 
 $Returns:		Window

 $Arguments:	Parent Window
 
*******************************************************************************/	
static T_MFW_HND calc_create(MfwHnd parent_window)
{
	tCalcData     * data = (tCalcData *)ALLOC_MEMORY (sizeof (tCalcData ));
	T_MFW_WIN  * win;

    TRACE_FUNCTION("calc_create()");
	if (data EQ NULL)
	{
		return NULL;
	}

	// Create window handler
	data->win = win_create (parent_window, 0, E_WIN_VISIBLE, NULL);
	if (data->win EQ NULL)
	{
		
		return NULL;
	}
	// connect the dialog data to the MFW-window
	data->mmi_control.dialog = (T_DIALOG_FUNC)calc_DialogCB;
	data->mmi_control.data   = data;
	win                      = ((T_MFW_HDR *)data->win)->data;
	win->user                = (void *)data;
	data->parent_win         = parent_window;
	calculator_win = data->win;
	return data->win;
}

/*******************************************************************************

 $Function:    	calc_DialogCB

 $Description:	Callback function for the calculator window
 
 $Returns:		nothing

 $Arguments:	Window, event, identifier (not used), parameter (not used)
 
*******************************************************************************/
static void calc_DialogCB(T_MFW_HND win, USHORT e,  SHORT identifier, void *parameter)
{ 	
	T_MFW_WIN       	*win_data   = ( (T_MFW_HDR *) win )->data;
	tCalcData* data = (tCalcData *) win_data->user;
	char display_buffer[28] = "";
#ifdef NEW_EDITOR
//	T_AUI_EDITOR_DATA	editor_data;	/* SPR#1428 - SH - New Editor data */  // RAVI
#else
//	T_EDITOR_DATA		editor_data;   // RAVI
#endif
	char* running_total_out; 
//	char debug_buffer[50];  // RAVI
 	TRACE_FUNCTION("calc_DialogCB()");

	switch( e )
    {	//when window first created
    	case CALC_INIT:
    	{	//set running total to 0 and display in editor
			TRACE_EVENT("CALC-INIT-JGG");
    		running_total = 0;
    		data->menu_options_win = NULL;

    		/* SPR#1428 - SH - New editor: calc starts with 0 */
#ifdef NEW_EDITOR
    		sprintf(data->buffer, "%s", "0");	/* SH - calc starts off with '0'*/
#else
    		sprintf(data->buffer, "%s", "");	/* SH - calc starts off with nothing*/
#endif

			calcSetEditor(win);//set editor with default attribute values
			/* SPR#1428 - SH - New Editor changes: use calc editor */
#ifdef NEW_EDITOR
			AUI_edit_SetEvents(&data->editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)calcGetNumberCB);
			data->editor_win = AUI_calc_Start(win, &data->editor_data);
#else /* NEW_EDITOR */
			data->editor_data.Callback     		= (T_EDIT_CB)calcGetNumberCB;
			data->editor_win = editor_start(win, &(data->editor_data));  /* start the editor */ 
#endif /* NEW_EDITOR */
			winShow(data->win);
    	}
    	break;

    	//when an arithmetic operator has been selected from the calc menu
    	case CALC_ENTER_OPERAND:
    	{ 	
			TRACE_EVENT("CALC_ENTER_OPERAND");
    		/* This memory alloc seems to fix a problem where running_total_out
    		 * would become an empty string after sevaral running calculations */
    		running_total_out = (char*)ALLOC_MEMORY(22*sizeof(char));	/* SH - change malloc to ALLOC_MEMORY */
    		
			running_total_out[0] = '\0';
			
			/* Display the first operand and the operator symbol
			 * above the operand to be entered */
			if ( (long)calcGetRunningTotal() == calcGetRunningTotal())
    			sprintf((char*)running_total_out, "%d", (long)calcGetRunningTotal());
			else
				sprintf((char*)running_total_out, "%f", calcGetRunningTotal());

			if (running_total_out[8] == '.')
				running_total_out[8] = '\0';
			else
				running_total_out[9] = '\0';
				
    		sprintf((char*)display_buffer,"%s %c", running_total_out, operatorSymbol(data->operation));
    		FREE_MEMORY((void*)running_total_out, 22*sizeof(char));		/* SH - change free to FREE_MEMORY */

			/* SPR#1428 - SH - Calc now starts off with '0' */
#ifdef NEW_EDITOR
    		sprintf(data->buffer, "%s", "0");	/* SH - calc starts off with '0'*/
#else
    		sprintf(data->buffer, "%s", "");	/* SH - calc starts off with nothing*/
#endif

    		calcSetEditor(win);

    		/* SPR#1428 - SH - New Editor changes */
 #ifdef NEW_EDITOR
			AUI_edit_SetEvents(&data->editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)calcGetOperandCB);
			AUI_edit_SetTextStr(&data->editor_data, TxtSoftOptions, TxtDelete, TxtCalculator, (UBYTE *)display_buffer);
			AUI_edit_SetAltTextStr(&data->editor_data, 1, TxtSoftOptions, TRUE, TxtSoftBack);//x0035544 17-11-2005
			data->input_number_win = AUI_calc_Start(win, &data->editor_data);
#else /* NEW_EDITOR */
    		data->editor_data.TextString = display_buffer;
			data->editor_data.editor_attr.win.px = 0;
			data->editor_data.editor_attr.win.py = Mmi_layout_line(3); //30
			data->editor_data.editor_attr.win.sx = SCREEN_SIZE_X;
			data->editor_data.editor_attr.win.sy = numberLineHeight()+2;
	
			data->editor_data.Callback     		= (T_EDIT_CB)calcGetOperandCB;
			data->input_number_win = editor_start(win, &(data->editor_data));  /* start the editor */ 
#endif /* NEW_EDITOR */

			winShow(data->win);

            /* Marcus: Issue 1039: 05/09/2002: Start */
			if (data->menu_options_win != NULL)
			{
				bookMenuDestroy( data->menu_options_win );
				data->menu_options_win = NULL;
			}
            /* Marcus: Issue 1039: 05/09/2002: End */
    	}    	
    	break;
    	
    	//when "Equals" selected from calc menu
    	case CALC_DISPLAY_RESULT:
    	{	
    		//if running total out of display range, set it to 0
    		if ((calcGetRunningTotal() > MAX_CALC_TOTAL) || (calcGetRunningTotal() < MIN_CALC_TOTAL))
    			running_total =0;
    		//Convert running total double to string
    		//if integer total, don't display any decimal places
    		if ( (long)calcGetRunningTotal() == calcGetRunningTotal())
    			sprintf(data->buffer, "%d", (long) calcGetRunningTotal());
    		else //if a floating-point total, display as many decimal places as will fit
    			sprintf(data->buffer, "%f", (double) calcGetRunningTotal());
			//if last number in display is a decimal point
			if (data->buffer[8] == '.') 
				data->buffer[8] = '\0';//remove it
    		data->buffer[9] = '\0'; //ensure string is properly terminated
    		//ensure string is no longer than 9 chars
    		
    		calcSetEditor(win);

			/* SPR#1428 - SH - New Editor changes */
 #ifdef NEW_EDITOR
 			AUI_edit_SetEvents(&data->editor_data, 0, TRUE, FOREVER, (T_AUI_EDIT_CB)calcGetNumberCB);
 			AUI_calc_Start(win, &data->editor_data);
 #else /* NEW_EDITOR */
			data->editor_data.Callback     		= (T_EDIT_CB)calcGetNumberCB;
			editor_start(win, &(data->editor_data));  /* start the editor */ 
#endif /* NEW_EDITOR */

            /* Marcus: Issue 1039: 05/09/2002: Start */
			if (data->menu_options_win != NULL)
			{
				bookMenuDestroy( data->menu_options_win );
				data->menu_options_win = NULL;
			}
            /* Marcus: Issue 1039: 05/09/2002: End */

			winShow(data->win);
    	}
    	break;
    	default:
    	{
    			TRACE_EVENT("calc_DialogCB(): Unknown Event");
    	}
    	break;

    }
    	
   
}

/*******************************************************************************

 $Function:    	calcGetNumberCB

 $Description:	Callback function for the editor window
 
 $Returns:		nothing

 $Arguments:	Window,  identifier (not used), reason (not used)
 
*******************************************************************************/
static void calcGetNumberCB( T_MFW_HND win, USHORT Identifier, SHORT reason)
{
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tCalcData   *data       = (tCalcData *) win_data->user;   
    float after_decimal = 0;
    int digits_after_point = 0;
    int i;
	char* end;
//    char debug[40];  // RAVI

    
    TRACE_FUNCTION("calcGetNumberCB()");

	switch (reason )
    {
        case INFO_KCD_LEFT:
        {	TRACE_EVENT("Left button pressed in calculator");
        	//get the number entered before the decimal point
			running_total = strtol(data->buffer, &end, 10);
        	if (strlen(end) != 0)
			{	//if decimal point entered
				if ((end[0] == '.') && isdigit(end[1]))//get number after decimal point
				{	
					end++;
					digits_after_point = strlen(end);
					after_decimal = strtol(end, &end, 10);
				}

				//convert number after decimal point to it's actual fractional value
				for (i=0; i < digits_after_point; i++)
					after_decimal = after_decimal/10;
				//add whole number and fraction together
				running_total = running_total + after_decimal;
			}
			if (data->menu_options_win != NULL)
				bookMenuDestroy( data->menu_options_win );
			//start the calculator option menu and kill this window
			data->menu_options_win = bookMenuStart( data->win, calcOptionMenuAttributes(),0);
			 SEND_EVENT(data->menu_options_win, ADD_CALLBACK, NULL, (void *)calc_menu_cb);
			 
        }
		break;
		
		case INFO_KCD_HUP:
        case INFO_KCD_RIGHT:
        {
            TRACE_EVENT("Right button pressed in calculator");
			calc_destroy(win);
        }
		break;

        default:
        {
            /* otherwise no action to be performed
            */
            /*calc_destroy(win); */ /*SH - do not destroy calc for other key */
		    break;
        }
	}
}


/*******************************************************************************

 $Function:    	calcGetOperandCB

 $Description:	Callback function for the editor window, when second number in operation
 				is entered.
 
 $Returns:		nothing

 $Arguments:	Window,  identifier (not used), reason (not used)
 
*******************************************************************************/

static void calcGetOperandCB( T_MFW_HND win, USHORT Identifier, SHORT reason)
{
    T_MFW_WIN       *win_data   = ( (T_MFW_HDR *) win )->data;
    tCalcData   *data       = (tCalcData *) win_data->user;
//	char* buffer;  // RAVI
    float operand = 0;
    float after_decimal = 0;
    int digits_after_point = 0;
    int i;
    char* end;
    char debug[40];
    
    TRACE_FUNCTION("calcGetOperandCB()");
    TRACE_EVENT_P2("ID, reason: %d, %d", Identifier, reason);

	switch (reason )
    {
        case INFO_KCD_LEFT:
  /* SPR#1428 - SH - New Editor: Can use '*' or centre button as 'equals', to show result */
#ifdef NEW_EDITOR
#ifdef COLOURDISPLAY
		case INFO_KCD_SELECT:
#endif /* COLOURDISPLAY */
        case INFO_KCD_STAR:
#endif /* NEW_EDITOR */
        {	//Get whole number before decimal point
			operand = strtol(data->buffer, &end, 10);

			//if buffer not pointing at an empty string now
			if (strlen(end) != 0)
			{	//if decimal point entered
				if ((end[0] == '.') && isdigit(end[1]))//get number after decimal point
				{	
					end++;
					digits_after_point = strlen(end);
					after_decimal = strtol(end, &end, 10);
					sprintf(debug,"Digits after decimal: %f.", after_decimal);
				}
				//convert number after decimal point to an appropriate fraction
				for (i=0; i < digits_after_point; i++)
					after_decimal = after_decimal/10;
				//add whole and fraction together
				operand = operand + after_decimal;
			}

			//perform the arithmetic function requested
        	switch (data->operation)
        	{
        		case PLUS: running_total = running_total + operand; break;
        		case MINUS: running_total = running_total - operand;break;
        		case MULTIPLY: running_total = running_total * operand;break; 
        		case DIVIDE: 
				// Oct 20, 2004    REF: CRR 25755 xnkulkar
				// if the attempted division is by zero
				if(operand == 0)
				{    		
					divideByZero = TRUE; // enable the flag
				}
				else // else perform the division operation
				{
					running_total = running_total / operand;
				}
				break;
				default: TRACE_EVENT("Unknown calc operation"); break;
        	}
				
		/* SPR#1428 - SH - New Editor: Can use '*' as 'equals', to show result */
#ifdef NEW_EDITOR
#ifdef COLOURDISPLAY
		if (reason==INFO_KCD_STAR || reason==INFO_KCD_SELECT)
#else /* COLOURDISPLAY */
        	if (reason==INFO_KCD_STAR)
#endif /* COLOURDISPLAY */
        	{
        		TRACE_EVENT("Star key - show result");
        		/* Star didn't destroy window automatically, do it here */
        		if (data->menu_options_win != NULL)
        		{
					bookMenuDestroy( data->menu_options_win );
					data->menu_options_win = NULL;
				}
        		if (data->input_number_win)
        		{
 					AUI_calc_Destroy(data->input_number_win);
 					data->input_number_win = NULL;
				}
        		SEND_EVENT(data->win, CALC_DISPLAY_RESULT, 0, 0);
        	}
        	else
#endif /* NEW_EDITOR */
        	{
				//Show option menu
				if (data->menu_options_win != NULL)
					bookMenuDestroy( data->menu_options_win );
				data->menu_options_win = bookMenuStart( data->win, calcOptionMenuAttributes(),0);
			}
        }
		break;
		
		case INFO_KCD_HUP:
        case INFO_KCD_RIGHT:
        {
            TRACE_EVENT("Right button pressed in calculator");
            
			calc_destroy(win);
        }
		break;

        default:
        {
            /* otherwise no action to be performed
            */
		    break;
        }
	}
}


static void calc_menu_cb(T_MFW_HND parent_win, UBYTE identifier, UBYTE reason)
{
	T_MFW_WIN       	*win_data   = ( (T_MFW_HDR *) parent_win )->data;
	tCalcData* data = (tCalcData *) win_data->user;

    TRACE_FUNCTION("calc_menu_cb()");
	//set menu window to NULL to prevent any dealloc problems
	data->menu_options_win = NULL;
	calc_destroy(parent_win);


}

/*******************************************************************************

 $Function:    	calcSetEditor

 $Description:	Sets the editor attributes to defaults for the calculator module
 
 $Returns:		nothing

 $Arguments:	Window
 
*******************************************************************************/
void calcSetEditor(T_MFW_HND win)
{
	T_MFW_WIN       	*win_data   = ( (T_MFW_HDR *) win )->data;
	tCalcData* data = (tCalcData *) win_data->user;

	TRACE_FUNCTION("calcSetEditor()");
	
	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	{
		AUI_edit_SetDefault(&data->editor_data);
		AUI_edit_SetDisplay(&data->editor_data, ZONE_FULL_SK_TITLE, COLOUR_EDITOR, EDITOR_FONT);
		AUI_edit_SetBuffer(&data->editor_data, ATB_DCS_ASCII, (UBYTE *)data->buffer, 10);
		AUI_edit_SetMode(&data->editor_data, 0, ED_CURSOR_NONE);
		AUI_edit_SetEvents(&data->editor_data, 0, TRUE, FOREVER, NULL);
		AUI_edit_SetTextStr(&data->editor_data, TxtSoftOptions, TxtDelete, TxtCalculator, NULL);
		//xrashmic 20 Oct, 2005 MMI-SPR-33845 
		//Editor always starts with min one character "0" and 
		//alterRight softkey is set to "BACK" here.
		data->editor_data.min_enter=1;
		data->editor_data.AltRightSoftKey=TxtSoftBack ;
		AUI_edit_SetFormatAttr(&data->editor_data, DS_ALIGN_RIGHT);
	}
#else /* NEW_EDITOR */
	editor_attr_init(&((data->editor_data).editor_attr), 0, edtCurBar1, 0, data->buffer, 10, COLOUR_EDITOR);
	editor_data_init( &data->editor_data, NULL, TxtSoftOptions, TxtDelete, NULL, 1, CALC_MODE, FOREVER);
			data->editor_data.hide				= FALSE;
			data->editor_data.Identifier		= 0xFFFF ; 
			data->editor_data.destroyEditor		= TRUE;
#endif /* NEW_EDITOR */

	return;
}

/*******************************************************************************

 $Function:    	calc_destroy

 $Description:	Destroys the calculator editor window and frees up memory
 
 $Returns:		nothing

 $Arguments:	Window
 
*******************************************************************************/
void calc_destroy(MfwHnd own_window)
{
  T_MFW_WIN * win_data;
  tCalcData   * data = NULL; 

  TRACE_EVENT ("calc_destroy()");
  if (own_window)
	{  	

	win_data = ((T_MFW_HDR *)own_window)->data;
		if (win_data != NULL) //PATCH TB
			data = (tCalcData *)win_data->user;

	  if (data)
		{
			
			win_delete (data->win);
            /* Marcus: Issue 1039: 05/09/2002: Start */
			if (data->menu_options_win != NULL)
			{
				bookMenuDestroy( data->menu_options_win );
				data->menu_options_win = NULL;
			}
            /* Marcus: Issue 1039: 05/09/2002: End */
		  // Free Memory
			FREE_MEMORY ((void *)data, sizeof (tCalcData));
	
		}
	  else
		{
			TRACE_EVENT ("calc_destroy() called twice");
		}
	}
}




