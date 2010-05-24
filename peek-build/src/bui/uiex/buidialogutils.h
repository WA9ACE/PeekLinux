
#ifndef UIDIALOGUTILS_H
#define UIDIALOGUTILS_H 
/******************************************************************************
* 
* FILE NAME   : buidialogutils.h
*
* DESCRIPTION :
*
*   This is the interface for the dialog utils define.
*
* HISTORY     :
*
*   See Log at end of file
*
* Copyright (c) 2007, BTC Technologies, Inc.
******************************************************************************/
#include "buidialog.h"

#define DlgMStyleT DlgStyleT
#define DlgPStyleT DlgStyleT
#define DlgCStyleT DlgStyleT

typedef enum
{
  PROGRESS_LEVEL_0,
  PROGRESS_LEVEL_1,
  PROGRESS_LEVEL_2,
  PROGRESS_LEVEL_3,
  PROGRESS_LEVEL_4,
  PROGRESS_LEVEL_5,
  PROGRESS_LEVEL_6,
  PROGRESS_LEVEL_7,
  PROGRESS_LEVEL_8,
  PROGRESS_LEVEL_9,
  PROGRESS_LEVEL_MAX
}PorgressLevelT;

typedef enum
{
    DLG_STYLE_BEGIN = 0,
    //---Menu Type Define ---
    DLG_MSTYLE_BEGIN,
    // For Popup
    DLG_MSTYLE0,
    // Screen show 3row
    DLG_MSTYLE1_T,
    DLG_MSTYLE2_ITT,
    DLG_MSTYLE3_TT,
    DLG_MSTYLE4_TT,
    // Screen Show 2row
    DLG_MSTYLE5_ITTT,
    DLG_MSTYLE6_ITTT,
    // Screen Show >2row
    DLG_MSTYLE7_T,
    DLG_MSTYLE8_TI,
    DLG_MSTYLE9_IT,
    //For Contact View
    DLG_MSTYLE10_TI,
    DLG_MSTYLE11_TI,
    DLG_MSTYLE12_TIIT,
    DLG_MSTYLE13_TITI,

    DLG_MSTYLE14_I,
    DLG_MSTYLE15_TI,
    //Screen show 1row
    DLG_MSTYLE16_TT,
    DLG_MSTYLE17_TITI,
    DLG_MSTYLE18_TIIT,
    DLG_MSTYLE19_TT,
    //Screen show 2row
    DLG_MSTYLE20_TITI,
    DLG_MSTYLE21_TIT,
    DLG_MSTYLE22_TITI,
    //Screen show 3row
    DLG_MSTYLE23_IT,
    DLG_MSTYLE24_T,
    DLG_MSTYLE25_TI,
    DLG_MSTYLE26_ITT,
    //Screen show 5row
    DLG_MSTYLE27_TI,
    //Screen show 2row
    DLG_MSTYLE28_TI,
    DLG_MSTYLE29_TI,
    DLG_MSTYLE30_T,
    DLG_MSTYLE31_T,
    //Screen show 3row
    DLG_MSTYLE32_ITT,
    //5row
    DLG_MSTYLE33_T,
    //  4 line text  add by zhanglanlan
    DLG_MSTYLE34_TTTT,
   // one line text
    DLG_MSTYLE35_T,
    // four and one line of two columns
    DLG_MSTYLE36_TTTTT,
    DLG_MSTYLE37_CTTT,
    
    //8row   //added by hebo 2008.03.12
    DLG_MSTYLE40_T,
    DLG_MSTYLE41_TT,
    DLG_MSTYLE42_TTT,
    DLG_MSTYLE43_ITT,
    
   //two rows : added by alq 20080625
    DLG_MSTYLE44_TC,
    //8rows: added by alq 20080716
    DLG_MSTYLE45_CTT,
    
    //For Grid
    DLG_MSTYLE98_GRID_MORE_IT,
    DLG_MSTYLE99_GRID_IT,
    
    DLG_MSTYLE_END,

    //---Popup Type Define ---
    //DLG_PSTYLE_BEGIN,    /* move down for qwert  */
    /*not in use -- please no to use*/
    DLG_PSTYLE_HELPTEXT,
	  //qln.zne.general.note.1
    DLG_PSTYLE_ERROR,//DLG_PSTYLE_INFO_ERROR_NOTE,
	  DLG_PSTYLE_WARNING,//DLG_PSTYLE_INFO_WARNING_NOTE,
    DLG_PSTYLE_INFO,//DLG_PSTYLE_INFO_INFORMATIONG_NOTE,
	  DLG_PSTYLE_CONF_NOTE,//DLG_PSTYLE_INFO_CONFIRMATION_NOTE,
	  DLG_PSTYLE_INFO_PERMANENT_NOTE,
     //qln.zne.general.note.2
    DLG_PSTYLE_ERROR2,//DLG_PSTYLE_INFO2_ERROR_NOTE,
	  DLG_PSTYLE_WARNING2,//DLG_PSTYLE_INFO2_WARNING_NOTE,
    DLG_PSTYLE_INFO2,//DLG_PSTYLE_INFO2_INFORMATIONG_NOTE,
	  DLG_PSTYLE_CONF_NOTE2,//DLG_PSTYLE_INFO2_CONFIRMATION_NOTE,
	  DLG_PSTYLE_INFO2_PERMANENT_NOTE,
    DLG_PSTYLE_SOFTNOTIFY1,
    DLG_PSTYLE_SOFTNOTIFY2,
    DLG_PSTYLE_QUERY1,
    DLG_PSTYLE_QUERY2,
    DLG_PSTYLE_QUERY3,
    DLG_PSTYLE_QUERY4,
    DLG_PSTYLE_QUERY5,
	  DLG_PSTYLE_QUERY6,
	  DLG_PSTYLE_QUERY6_2,
    //--
    // DLG_PSTYLE_CONF_NOTE,    
    DLG_PSTYLE_PROGRESS,
    DLG_PSTYLE_WAIT,
    DLG_PSTYLE_ALL,
    /*not in use -- end*/
    //add by zhanglei for qwert
    DLG_PSTYLE_BEGIN,
    DLG_PSTYLE_NONE_BTN,
    DLG_PSTYLE_NONE_BTN_BIG,
    DLG_PSTYLE_ONE_BTN,
    DLG_PSTYLE_ONE_BTN_BIG,
    DLG_PSTYLE_TWO_BTN,
    DLG_PSTYLE_TWO_BTN_BIG,
    DLG_PSTYLE_THREE_BTN,
    DLG_PSTYLE_THREE_BTN_BIG,
    //end by zhanglei
    DLG_PSTYLE_END,
    
    //---Cust Type Define ---
    DLG_CSTYLE_BEGIN,
    DLG_CSTYLE_PASSWORD,
    DLG_CSTYLE_FULLEDIT,
    DLG_CSTYLE_FULLSTATIC,
    DLG_CSTYLE_PROGRESS,
    DLG_CSTYLE_HELPTEXT,
    DLG_CSTYLE_END,
    
    DLG_STYLE_END
    
/////////////////////////////////////
}DlgStyleT; 

#define DLG_PSTYLE_CONF DLG_PSTYLE_CONF_NOTE

DialogC* CreateDialogByType(DlgStyleT type,ResourceIdT ResDataId,void* ExtraDataP,uint32 WinId,uint32 Style = WINSTYLE_SKBAR,DispLCDTypeT LcdType = LCD_MAIN);

/*****************************************************************************
* $Log: uipopupex.h $
* Revision 1.1  2007/09/17 09:35:19  zli
* Initial revision
*****************************************************************************/


#endif

