#ifndef _DEF_MMI_SMSIDLE_H_
#define _DEF_MMI_SMSIDLE_H_
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		SMS
 $File:		    mmiSmsIdle.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description: Header file for the SMS idle.
   
********************************************************************************
 $History: mmiSmsIdle.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/

/*******************************************************************************
                                                                              
                                Include Files
                                                                              
*******************************************************************************/

#include "mfw_mfw.h"

#define SMS_PROMPTX      5
#define SMS_PROMPTY     45
#define PROMPTX          7
#define PROMPTY         17

/*NM p029*/
#define TEXT_COMPRESSED 	0x20
/* Marcus: Issue 1650: 28/01/2003: Start */
#define TEXT_CLASS_GIVEN    0x10
#define TEXT_CLASS_MASK     0x03
#define TEXT_CLASS_0        0x00
/* Marcus: Issue 1650: 28/01/2003: Start */

/* this should keep only one for all SMS Parts */
typedef enum
{
    E_INIT = 0x0801,
    E_BACK,
    E_ABORT,
    E_EXIT,
	E_RETURN, 
	E_OK
} e_M_events;


T_MFW_HND   smsidle_start(T_MFW_HND parent_window, void *param);
void smsidle_destroy (void);

BOOL smsidle_get_memory_full(void);
BOOL smsidle_get_ready_state(void);

// Used to count the number of deleted messages in delete all
extern SHORT g_ListLength2;


/* for backward compatibility only */
#define msgsNewMessageStatus  smsidle_get_unread_sms_available
#define msgsShowStatus         smsidle_display_sms_status

#define smsCbNewMessageStatus smsidle_get_unread_smscb_available
#define smsCbShowStatus       smsidle_display_smscb_status

BOOL smsidle_get_unread_sms_available(void);
int smsidle_display_sms_status(int* noOfMsgs);

BOOL smsidle_get_unread_smscb_available(void);
void smsidle_display_smscb_status(char * text);
//GW SPR#1035 - Added SAT changes
void smsidle_unset_ready_state(void);
/*SPR2512, function to format message information into a text string*/
void smsidle_message_to_text(T_MFW_SMS_MT* message, char* text_buffer);
#endif /* _DEF_MMI_SMSIDLE_H_ */
