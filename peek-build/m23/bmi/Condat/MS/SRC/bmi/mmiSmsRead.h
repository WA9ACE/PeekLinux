#ifndef _DEF_MMI_SMSREAD_H_
#define _DEF_MMI_SMSREAD_H_

#undef GLOBAL_EXT
#if defined (MMI_SMSREAD_C)
#define GLOBAL_EXT
#else
#define GLOBAL_EXT extern
#endif

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		SMS
 $File:		    mmiSmsRead.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    This module provides definitions of the types and
    constants which are shared across the phone book
    application modules.
   
********************************************************************************
 $History: mmiSmsRead.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/

/*******************************************************************************
                                                                              
                                Include Files
                                                                              
*******************************************************************************/

#include "mfw_mfw.h"

/*******************************************************************************
                                                                              
                        Shared Structure Definitions
                                                                              
*******************************************************************************/

/*SPR 1453*/
#ifdef LSCREEN
#define MENU_TEXT_LENGTH 50	
#else
#define MENU_TEXT_LENGTH 16	//TBD: to include in common .h
#endif
#define MENU_HEADER_LENGTH (MENU_TEXT_LENGTH - 4) //TBD: to include in common .h

// Length = to len("from ") + len(number) + len(sms text) + 4 (for \0 \n and optional ionternational character +)
#define MAX_EDITOR_LEN (5+MFW_NUM_LEN+MAX_MSG_LEN+4)


typedef enum
{
	MMI_SMS_MO,
	MMI_SMS_MT,
	MMI_SMS_CB
} T_MMI_SMS_STAT;	

/*******************************************************************************
                                                                              
                                Function Prototypes
                                                                              
*******************************************************************************/


T_MFW_HND SmsRead_R_TEXTEDT2_start(T_MFW_HND parent_window, MfwMnuAttr *data);
T_MFW_HND SmsRead_R_OPT_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr);


void SmsRead_DisplayStandardDialog(T_MFW_HND win, int TextId, ULONG duration);



/*******************************************************************************
                                                                              
                              DUMMY DECLARATIONS	
                                                                              
*******************************************************************************/

//Dummy defines
#define SmsRead_R_ID 			100
#define SmsRead_R_TEXTEDT_ID 	101
#define SmsRead_R_TEXTEDT2_ID 	101	// Specific for approval
#define SmsRead_R_OPT_ID 		102
#define SmsRead_SMS_DISPLAY_ID	103
#define SmsRead_R_READSMS_OPT_ID 104

#define DEFAULT_FONT -1

#define SMS_APPROVAL 1

// MZ 6/2/01 temp typedefs.
typedef char * wstring_t;
typedef char * charw_t;
typedef ListCbFunc ListCbFunc_t;




#endif /* _DEF_MMI_SMSMENU_H_ */
