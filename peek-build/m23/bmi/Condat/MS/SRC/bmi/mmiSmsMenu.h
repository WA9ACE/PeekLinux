#ifndef _DEF_MMI_SMSMENU_H_
#define _DEF_MMI_SMSMENU_H_


/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		SMS
 $File:		    mmiSmsMenu.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    header of template for MMI SMS menu dynamic event handling


                        
********************************************************************************

 $History: mmiSmsMenu.h

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/
#undef GLOBAL_EXT
#if defined (MMI_SMSMENU_C)
	#define GLOBAL_EXT
#else
	#define GLOBAL_EXT extern
#endif


#include "mfw_mfw.h"




#define TIMER_EXIT  0x0BB8             /* Default SMS timer period - 3 secs.  */
#define SHORT_SECS  0x01F4				/* Information dialogue display times. */
#define THREE_SECS  0x0BB8             /* 3000 milliseconds.                  */
#define FIVE_SECS   0x1388             /* 5000 milliseconds.                  */
#define TEN_SECS    0x2710             /* 10000 milliseconds.                 */
#define TWO_MIN     0x1D4C0            /* 2 min                               */
#define FOREVER     0xFFFF             /* Infinite time period.               */


/* This structure must be used by the different modules to pass parameters for SMS sending */
typedef struct
{
    UBYTE TextBuffer[MAX_MSG_LEN_ARRAY];
    UBYTE NumberBuffer[NUMBER_LENGTH];
    UBYTE CentreBuffer[NUMBER_LENGTH];
     /* FTA 34.2.8 
        we have to know from where we are calling the 
        sms sending part. We should use the Service
        CenterNumber from the Caller in the case of REPLYING
      */
    UBYTE called_from_reply_item;  
} T_SmsUserData;

//x0pleela 20 Sep conc sms
EXTERN USHORT ConcatenateDisplay( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );



// Entry points for the different SMS blocs
GLOBAL_EXT int M_exeSendNew(MfwMnu* m, MfwMnuItem* i);
GLOBAL_EXT int M_exeRead(MfwMnu* m, MfwMnuItem* i);
GLOBAL_EXT int M_exeDelAll(MfwMnu* m, MfwMnuItem* i);
GLOBAL_EXT int M_exeSendSaved(MfwMnu* m, MfwMnuItem* i);
GLOBAL_EXT int M_exeVoice(MfwMnu* m, MfwMnuItem* i);

/*SPR 1920 removed obsolete function*/

// Dialog window callback function
GLOBAL_EXT void SmsMenu_loadDialogDefault(T_DISPLAY_DATA * DisplayInfo);

int voice_mail_init (void);

/***************************************************
****											****
****				DUMMY DECLARATIONS			****
****											****
***************************************************/

//Dummy defines
#define TxtNotReady			1
#define TxtNotDeleted		2
#define TxtAllDeleted		3
#define TxtEnterVoiceNumber 4
//Dummy functions (stubs)
void SmsRead(void); // RAVI
void call_voice(void); // RAVI
void sms_send(T_SmsUserData *);
int sms_get_ready_state(void); // RAVI

int newSmsMenu_Start(MfwMnu* m, MfwMnuItem* i);
int ExeSms_sendM_SND(MfwMnu *, MfwMnuItem *);
int ExeM_SND(MfwMnu *, MfwMnuItem *);

/* SPR#1428 - SH - New Editor: These functions moved here from MmiEditor.c */
#ifdef NEW_EDITOR
#ifdef EASY_TEXT_ENABLED
int showeZiTextIcon(MfwMnu* m, MfwMnuItem* i);
 int Easy_Activate(MfwMnu* m, MfwMnuItem* i);
 int Easy_DeActivate(MfwMnu* m, MfwMnuItem* i);
#endif /* EASY_TEXT_ENABLED */

#ifdef TI_PS_FF_CONC_SMS 
 //API - 01/10/02 - Concatenation Switch Function Definition
 int Concatenate_DeActivate(MfwMnu* m, MfwMnuItem* i);
 int Concatenate_Activate(MfwMnu* m, MfwMnuItem* i);
 int Concatenate_Status(MfwMnu* m, MfwMnuItem* i);
#endif /*TI_PS_FF_CONC_SMS*/
#endif /* NEW_EDITOR */

#endif /* _DEF_MMI_SMSMENU_H_ */
