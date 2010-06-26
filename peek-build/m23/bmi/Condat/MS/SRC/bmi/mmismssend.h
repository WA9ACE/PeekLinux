#ifndef _DEF_MMI_SMSSEND_H_
#define _DEF_MMI_SMSSEND_H_

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

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   SMS
 $File:       mmismssend.h
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/10/00

********************************************************************************

 Description:

    header of template for MMI yyy dynamic event handling

********************************************************************************
 $History: mmismssend.h

  Feb 02, 2005   REF: CRR 28479   xnkulkar
  Bug: While saving a SMS, if the user presses LSK while the 'Saved' dialog is displayed, the 'Saved' 
  dialog doesn't get dismissed.
  Solution: Only RSK and Hangup keys dismiss the 'Saved' info screen and all other key presses 
  are ignored. 
  
  25/10/00      Original Condat(UK) BMI version.

 $End

*******************************************************************************/

/*******************************************************************************

                                Include Files

*******************************************************************************/

#include "mfw_mfw.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "MmiEditor.h" //GW Added for T_EDITOR structure
#endif


/*******************************************************************************

                        Shared Structure Definitions

*******************************************************************************/

typedef struct
{
  /* administrative data */

  T_MMI_CONTROL   mmi_control;
  T_MFW_HND       win;
  T_MFW_HND       parent;
  T_MFW_HND       child_dialog;/* to keep the window of dialog          */
  T_MFW_HND       sms_handler;
  SHORT           id;
  T_SmsUserData edt_data; /* pointer of text and number */
  T_MFW_HND   text_editor;
  T_MFW_HND   number_editor;
  T_MFW_HND   sc_number_editor;
  T_MFW_HND   options_win;
} T_SEND;

typedef enum
{
  /* Logical frames */
  SEND_ID,
  SAVE_ID,
  /* Extern windows */
  PHBK_ID,
  /* Editor windows */
  SMSSEND_ID_TEXTEDIT,
  SMSSEND_ID_NBEDIT,
  SMSSEND_ID_CENTREEDIT,
  SMSSEND_ID_CENTREEDIT_CHANGE,
  /* Dialog windows */
  SMSSEND_ID_WAITING,
  SMSSEND_ID_WAIT_SEND,
  SMSSEND_ID_SENT,
  SMSSEND_ID_NOT_SENT,
  SMSSEND_ID_WAIT_SAVE,
  SMSSEND_ID_SAVED,
  SMSSEND_ID_NOT_SAVED,
  SMSSEND_ID_LIST_FULL,
  SMSSEND_ID_SAVE_NOT_READY, //PATCH TB
  SMSSEND_ID_SC_NOT_READY, //PATCH TB
  SMSSEND_ID_NOT_ALLOWED

} sms_send_id;

/*******************************************************************************

                                Function Prototypes

*******************************************************************************/

GLOBAL_EXT T_MFW_HND SmsSend_SEND_start(T_MFW_HND parent_window, T_SmsUserData *UserData);
GLOBAL_EXT T_MFW_HND SmsSend_SAVE_start(T_MFW_HND parent_window, T_SmsUserData *UserData);
GLOBAL_EXT T_MFW_HND V_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr);
GLOBAL_EXT int M_callVoice(MfwMnu* m, MfwMnuItem* i);
//	Feb 02, 2005   REF: CRR 28479   xnkulkar
//	Prototype  of function mmi_dialog_information_screen_save_sms()
T_MFW_HND mmi_dialog_information_screen_save_sms(T_MFW_HND parent_win, int TxtId, char* text, T_VOID_FUNC call_back, USHORT identifier);
int SmsSend_R_OPTExeCentreEdit(MfwMnu* m, MfwMnuItem* i);

// Used im mmismsread to compare with SMS center number
GLOBAL_EXT UBYTE SmsSend_get_config_data(T_MFW_SMS_INFO *config_data);
GLOBAL_EXT void SmsSend_convert_to_inter(T_MFW_SMS_ADDR *source_number, char *dest_number);
UBYTE SmsSend_set_SrvCntr_number (T_MFW_SMS_INFO *config_data);
/* SPR#1428 - SH - New Editor: no longer required */
#ifndef NEW_EDITOR
//GW 29/11/01 - Added 
void		SmsSend_loadEditDefault(T_EDITOR_DATA * edit_info);
#endif /* NEW_EDITOR */
#endif /* _DEF_MMI_SMSSEND_H_ */
