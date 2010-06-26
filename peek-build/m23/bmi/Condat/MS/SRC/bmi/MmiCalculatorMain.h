#ifndef _MMI_CALC_MAIN_WINDOW_H_
#define _MMI_CALC_MAIN_WINDOW_H_
/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   Calculator
 $File:       MmCalculatorMain.h
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

     This provides the main calculator functionality

********************************************************************************
 $History: MmiCalculatorMain.h

  25/10/00      Original Condat(UK) BMI version.

 $End

*******************************************************************************/


/*******************************************************************************

                                Include files

*******************************************************************************/


#include "MmiBookShared.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif


/*******************************************************************************

                               Data Types

*******************************************************************************/
typedef enum
{
    CALC_INIT,
    CALC_DISPLAY_RESULT,
    CALC_ENTER_OPERAND
} calc_events;

typedef enum
{
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE
} calc_ops;

typedef struct _tCalcData_
{
    /* administrative data */

    T_MMI_CONTROL     mmi_control;
    T_MFW_HND         win;
    T_MFW_HND         parent_win;
    T_MFW_HND         input_number_win;	/* SH - put this back in to handle '*' as 'equals' */
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA	editor_data;	/* SPR#1428 - SH - New Editord data */
#else
 	T_EDITOR_DATA   editor_data;
#endif

    /* associated handlers */

    T_MFW_HND         kbd;
    T_MFW_HND         kbd_long;
    T_MFW_HND         menu;
  T_MFW_HND         menu_tim;
  UBYTE           status_of_timer; /* use for the keypadlock */
    /* internal data */
  char        buffer[22];
    UBYTE       operation;
     T_MFW_HND menu_options_win;
     T_MFW_HND editor_win;

} tCalcData;

/*******************************************************************************

                                Public Methods

*******************************************************************************/


int calculator(MfwMnu* m, MfwMnuItem* i);
int calcPlus(MfwMnu* m, MfwMnuItem* i);
int calcMinus(MfwMnu* m, MfwMnuItem* i);
int calcDivide(MfwMnu* m, MfwMnuItem* i);
int calcMultiply(MfwMnu* m, MfwMnuItem* i);
int calcEquals(MfwMnu* m, MfwMnuItem* i);
void calcSetEditor(T_MFW_HND win);
#endif
