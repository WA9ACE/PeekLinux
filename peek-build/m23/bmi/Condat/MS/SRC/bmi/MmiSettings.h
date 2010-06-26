#ifndef _DEF_MMI_SETTINGS_H_
#define _DEF_MMI_SETTINGS_H_

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Settings
 $File:		    MmiSettings.h

 	Jul 28, 2004 REF: CRR 20899 xkundadu
	Description: No identification for user whether 'Any-key-Answer' is On/Off
	Solution: Added the function settingsFocusCurSelAnyKeyAns
			prototype.
			
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
                        
********************************************************************************
      Oct 30, 2006 ER:OMAPS00091029 x0039928(sumanth)
      Bootup time measurement
      
	May 04, 2005  REF: CRR 30285  x0021334
       Description: SMS Status Request in idle and dedicated mode (using Menu) setting does not work
       Solution: status of sms status request is read and focus is set accrodingly.
    
       March 2, 2005    REF: CRR 11536 x0018858
       Description: Reply path not supported while sending a reply.
       Solution: Added menu option for the reply path.

	Jul 22,2004 CRR:20896 xrashmic - SASKEN
	Description: The current time format is not highlighted
	Fix: Instead of providing the submenu directly we call a function 
	where we set the menu and also higlight the previously selected menu item
	 
 	Jul 29, 2004 REF = CRR 20898 xkundadu
	Description: No identification for user whether 'Auto-Answer' is On/Off
	Solution: Added the function settingsFocusCurSelAutoAns prototype.

       Issue Number : SPR#12822  on 31/03/04 by v vadiraj

*******************************************************************************
                                                                              
                                Required Include Files
                                                                              
*******************************************************************************/

							          
/* PROTOTYPES               */


//  Jul 29, 2004 REF = CRR 20898 xkundadu
// Added the function settingsFocusCurSelAutoAns() prototype.
void settingsFocusCurSelAutoAns(void);
//  Jul 28, 2004 REF: CRR 20899 xkundadu
// Added the function settingsFocusCurSelAnyKeyAns() prototype.
void settingsFocusCurSelAnyKeyAns(void);

T_MFW settingsRecReportControl(UBYTE ReportStatus);
int settingsGetStatus(int flagBit);

int settingsSilentModeOff	(void);
int settingsSilentModeOn	(void);
int settingsKeyPadLockOn	(void);
int settingsKeyPadLockOff	(void);
int settingsAutoRedialOn	(MfwMnu* m, MfwMnuItem* i);
int settingsAutoRedialOff	(MfwMnu* m, MfwMnuItem* i);
int settingsAutoAnswerOn	(MfwMnu* m, MfwMnuItem* i);
int settingsAutoAnswerOff	(MfwMnu* m, MfwMnuItem* i);
int settingsAnyKeyAnswerOn	(MfwMnu* m, MfwMnuItem* i);
int settingsAnyKeyAnswerOff	(MfwMnu* m, MfwMnuItem* i);
int factoryReset			(MfwMnu* m, MfwMnuItem* i);
int settingsVoiceMail		(MfwMnu* m, MfwMnuItem* i);
int settingsScNumber		(MfwMnu* m, MfwMnuItem* i);
int SmsValidityPeriod          (MfwMnu* m, MfwMnuItem* i);
int SmsStatusReportRequest(MfwMnu* m, MfwMnuItem* i); /* Marcus: Issue 1170: 30/09/2002 */
//March 2, 2005    REF: CRR 11536 x0018858
int SmsReplyPathRequest(MfwMnu* m, MfwMnuItem* i); /*Added function declaration for 
                                                     setting reply path*/
void settingsInit (void);
void settingsExit (void);
/* GW Added as external to initialise FFS data when FFS file does not exist*/
void setUserSettings2Default (void);
void settingsShowStateAnykeyAnswer (void);
void settingsShowStateAutoAnswer (void);
void settingsShowStateAutoRedial(void);
void settingsShowStateCBDisable(void);
void settingsSetStatus( unsigned int statusId, int setOrClear ); /*x0039928 - Lint warning fix */
// change by Sasken (VVADIRAJ) on March 31th 2004
// Issue Number : MMI-SPR-12822
void settingsFocusNwIconCurSel(void);

//Jul 22,2004 CRR:20896 xrashmic - SASKEN
void settingsFocusTimeFormatCurSel(void); 

// May 04, 2005  REF: CRR 30285  x0021334
void settingsSmsStatusReportCurSel(void);
int menuBootTimeMsmt(MfwMnu* m, MfwMnuItem* i); /*OMAPS00091029 x0039928(sumanth)*/
#ifdef NEPTUNE_BOARD
#ifdef MMI_POWER_MANAGEMENT_TEST

/* Used to select the power Management */
void mmi_PowerManagement(void);

/* Used to select the power management Duration */
int mmi_PMDuration(MfwMnu* m, MfwMnuItem* i);
#endif
#endif
#define SET_SETTING		(1)
#define CLEAR_SETTING	(0)

#endif

