
/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiPins.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    MMI PIN / PUK entry handling

                        
********************************************************************************

 $History: MmiPins.h

 	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation 

  	Nov 13, 2006 DR: OMAPS00103356  x0pleela
 	Description: SIMP:Master unlock failed through MMI
 	Solution: Defined new enums SMLK_SHOW_WRONG_PWD: if Master unlocking password is wrong
 			and SMLK_SHOW_DEP_CATS_UNLOCKE: if Master unlocking operation is successful

   	Sep 26, 2006 DR: OMAPS00095524 x0pleela
 	Description:Implement 2 missing SIM Lock types
 	Solution: Made the following changes
 			- Added new enum INPUT_PBLOCK_PIN_REQ to send request for entering 
 				Blocked Network Password
			- Added prototype of functions setLockTypePb(), simlock_check_PBCatLock()
 	xashmic 21 Sep 2006, OMAPS00095831
 	Description: USB enumeration does not happen in PIN/PUK/Insert SIM scenarios
 	Solution: When booting up without SIM card, or prompting for PIN/PUK/SIMP entry, 
 	enumerate all the USB ports automatically 
 
 	Sep 08, 2006 DR: OMAPS00091250 x0pleela
 	Description:The phone resets if no MEPD data is presented
 	Solution: Added new enums NO_MEPD, NO_MEPD_DATA,	TXT_NO_MEPD to display the string 
 			MEPD not present

	July 31, 2006 ER: OMAPS00087586, OMAPS00087587 x0pleela
	Description: 	OMAPS00087586: MasterKey funtion in TCS3.2 SW
				OMAPS00087587: Time Penaly feature extend handset security 
	Solution:Made the following changes
	1) Added new event  SIM_LOCK_BUSY_UNBLOCK
	
	July 21, 2006 ER: OMAPS00087586, OMAPS00087587 x0pleela
	Description: 	OMAPS00087586: MasterKey funtion in TCS3.2 SW
				OMAPS00087587: Time Penaly feature extend handset security 
	Solution:Made the following changes
		1) Defined new enumerations SMLK_MASTERKEY,INPUT_MASTER_KEY_REQ,
			SIM_LOCK_REQ_MKPWD,TXT_ENTER_MKPWD,INPUT_SIM_MKPWD,
			SIM_MKPWD_END,SIM_LOCK_MKPWD_FAIL,SIM_LOCK_MKPWDSUCC,
			SIM_LOCK_BUSY,SIM_LOCK_BUSY_BOOTUP,SMLK_SHOW_MK_FAIL,
			SMLK_SHOW_MK_SUCC
		2) Added prototypes of the following functions:
			SIMLock_MasterKeyUnlock(), SIMLock_MasterKeyUnlock_bootup(), simlock_check_masterkey(), 
			simlock_check_NWCatLock(), simlock_check_NSCatLock(), simlock_check_SPCatLock(), 
			simlock_check_CPCatLock(), simlock_check_SIMCatLock(), simlock_check_CatBlock(), 
			SIMLock_unblock_ME(), mmi_simlock_category_menus(), mmi_simlock_master_unlock(), 
			simlock_get_masterkey_status(), simp_unlock_Category()


	May 13, 2006 DR: OMAPS00067919 x0pleela
	Description: SIMP:On repeat Depersonalisation with invalid password MEPD unblock key should 
				be asked not Puk1.
	Solution: Removed new enumerations UNBLOCK_CODE_REQ, UNBLOCK_REQ_ACT_INFO, 
			TXT_UNBLOCK_CODE, UNBLOCK_CODE to handle unblock code for SIMP feature
	
	Apr 19, 2006 DR:OMAPs00067912 x0pleela
	Description: SIMP:On repeat Depersonalisation with invalid password MEPD unblock key should 
				be asked not Puk1.
	Solution: Defined new enumerations UNBLOCK_CODE_REQ, UNBLOCK_REQ_ACT_INFO, 
			TXT_UNBLOCK_CODE, UNBLOCK_CODE to handle unblock code for SIMP feature
	
	Nov 24, 2005 DR: OMAPS00045909 - Shashi Shekar B.S.
	Description: Improve IMEI control mechanism
	Solution : When MMI calls sAT_PLUSCFUN, if an IMEI invalid error is returned,
		we will block on that screen & will not allow the user to browse menus further,
		since the PS will not be booted at all!!!

	July 19, 2005 REF: CRR LOCOSTO-ENH-28173 xpradipg
  	Description: To provide MMI Support to enable/disable/change password and 
  				query all the Personalization locks
  	Solution: Integration of the changes for the same provided by the soldel 
  			  team
  			  
  	June 16, 2005  REF: CRR 31267  x0021334
	Description: Handset ignore the initializtion of the PIN1/PIN2
	Fix:	Cheking is done to ascertain if PIN1/PIN2 are initialised. If not, appropriate
	       message is displayed to the user.

	25/10/00			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/
#ifndef _DEF_MMI_PINS_H_
#define _DEF_MMI_PINS_H_

#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY  mfwFree

//API - 30/10/02 Move these defines to the header file 1162
#define MAX_DIG 		30			/* maximum number of digits */
#define MAX_PIN 	 	16			/* maximum number of PIN/PUK, SPR1351, simlock passwords can be up to 16 chars */
#define MAX_PIN_EN_DIS 	8			/*SPR 2145 maximum string length for PIN1 when enabling/disabling PIN1 */
#define MIN_PIN 	 	4
#define TIMERESULT		500			/*   time for results(later to change!!*/
#define TIMEOUT 		30000		/* time for entering	     */

//x0pleela 09 Mar, 2007 ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
#define PH_MAX_DIG 	4 /*MAX digits for Phone unlock code*/
#endif

/* typedefs */

typedef enum
{
	INPUT_PIN1 = PinsDummy,
	INPUT_PIN2,
	INPUT_PUK1,
#ifdef SIM_PERS
	PERM_BLK1,
#endif
    INPUT_PUK2,
	INPUT_NEW_PIN,
	INPUT_NEW_PIN_AGAIN,
	INPUT_NEW_PIN2,
	INPUT_NEW_PIN2_AGAIN,	
#ifdef SIM_PERS
	INPUT_SIM_PLOCK,
	TXT_ENTER_PLOCK,
	SIM_LOCK_STATUS_END,
       SIM_PLOCK_END,
	SIM_LOCK_ENDSUCC,
       SIM_LOCK_ENDIS_FAIL,   
       SIM_LOCK_REQ_OLD,
       SIM_LOCK_STATUS,
       SIM_LOCK_REQ_NEW,
       TXT_ENTER_NEW_SIMLOCK,
       TXT_ENTER_NEW_SIMLOCK_AGAIN,
       INPUT_NEW_SIM_PLOCK,
       INPUT_NEW_SIM_PLOCK_AGAIN,
       NEW_SIM_PLOCK_END,
       NEW_SIM_PLOCK_FAIL,
       SIM_LOCK_NEW_ENDISFAIL,
       SIM_LOCK_NEW_ENDSUCC,
	SIM_LOCK_REQ_FCPWD,
	SIM_LOCK_FCPWD_FAIL,
	SIM_LOCK_FCPWDSUCC,
	TXT_ENTER_FCPWD,
	INPUT_SIM_FCPWD,
	SIM_FCPWD_END,
	FAILURE_PASS_CHG,
	 SIM_LOCK_NEW_ENDSUCC_CKEY,
	 INPUT_SIM_PIN_REQ,
	INPUT_NLOCK_PIN_REQ,
	INPUT_NSLOCK_PIN_REQ,
	INPUT_SPLOCK_PIN_REQ,
	INPUT_CLOCK_PIN_REQ,
	SIM_LOCK_ALREADY_ENDIS,
	SIM_LOCK_PERS_CHK_OK,
	SIM_LOCK_SIM_REM,
	SMLK_PUK,
	SMLK_SHOW_FC_FAIL,
	SMLK_SHOW_FC_SUCC,
	SMLK_SHOW_FC_FAIL1,
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
	SMLK_MASTERKEY,
	INPUT_MASTER_KEY_REQ,
	SIM_LOCK_REQ_MKPWD,
	TXT_ENTER_MKPWD,
	INPUT_SIM_MKPWD,
	SIM_MKPWD_END,
	SIM_LOCK_MKPWD_FAIL,
	SIM_LOCK_MKPWDSUCC,
	SIM_LOCK_BUSY,
	SIM_LOCK_BUSY_BOOTUP,
	SMLK_SHOW_MK_FAIL,
	SMLK_SHOW_MK_SUCC,
	SIM_LOCK_BUSY_UNBLOCK,
	INPUT_PBLOCK_PIN_REQ, //x0pleela 25 Sep, 2006 ER: OMAPS00095524
	SMLK_SHOW_WRONG_PWD, //x0pleela 13 Nov, 2006 DR: OMAPS00103356  if Master unlocking password is wrong
	SMLK_SHOW_DEP_CATS_UNLOCKED, //x0pleela 13 Nov, 2006 DR: OMAPS00103356  if Master unlocking operation is successful
#endif
	INSERT_CARD,
	CARD_REJECTED,
	PIN_OK,
	PIN2_OK,
	PIN1_OK,
	PIN1_STATUS,	
	PIN1_STATUS_END,	
	PIN2_ABORT,
	PIN1_ABORT,
	PIN1_REQ,
	PIN2_REQ,
	PIN1_REQ_ATT,         
	PUK1_REQ_ACT_INFO,    
	PUK1_REQ,
#ifdef SIM_PERS
	PERM_BLK,
	PUK_FCM_REQ,
#endif
	PUK1_OK_END,
    PIN1_REQ_OLD,
    PIN2_REQ_OLD,	
	GSM_FAIL,             
	NEW_PIN_FAIL,
	NEW_PIN2_FAIL,
	PUK1_REQ_VER_INFO,    	
	PUK1_UNBL_FAIL_INFO,  	
	PUK1_UNBL_FATAL_ERROR,	
    INVALID_CARD,	
	NO_SIM_CARD,          			
	SIM_UNLOCK_OK,        	
	SIM_LOCK_ERR,
	SIM_PLOCK_REQ,
	SIM_NLOCK_REQ,						
	SIM_NSLOCK_REQ,						
	SIM_SPLOCK_REQ,						
	SIM_CLOCK_REQ,						
	SIM_PLOCK,						
	SIM_NLOCK,
	SIM_NSLOCK,
	SIM_SPLOCK,
	SIM_CLOCK,
    TXT_ENTER_PIN1,
	TXT_ENTER_PIN2,
    TXT_ENTER_PUK1,
#ifdef SIM_PERS
    ME_PERM_BLK,
#endif
	TXT_ENTER_PUK2,
    TXT_ENTER_NEW_PIN,
	TXT_ENTER_NEW_PIN2,
    TXT_ENTER_NEW_PIN_AGAIN,
	TXT_ENTER_NEW_PIN_AGAIN2,
    TXT_INVALID_CARD,
    TXT_NO_CARD,
	TXT_INSERT_CARD,
    PIN1_END,
	PIN2_END,
    PUK1_END,
    PIN_OK_INFO,
	PIN_OK_END,
    TXT_SIM_LOCK_ALL,
	TXT_SIM_PLOCK,
	TXT_SIM_NLOCK,
	TXT_SIM_NSLOCK,
	TXT_SIM_SPLOCK,
	TXT_SIM_CLOCK,
    PUK1_UNBL_SUCC,
    SIM_REMOVED,
    SIM_LOCK_ALL,
	PIN1_CH_END,
	PIN2_CH_END,
    UNBL_OK,
	FDN_UNBL_OK,
	PIN2_REQ_ATT,
	PIN1_CH_SUCC,
	PIN2_CH_SUCC,
	PUK2_REQ,
	NEW_PIN_END,
	NEW_PIN2_END,
	PIN1_ENAB_END,
	PIN1_DISAB_END,
	PIN1_ENDIS_FAIL,
	SETT_ABORT,
	FDN_ACT_END,
	FDN_ACTIVATED,
	ADN_ACTIVATED,
	FDN_FAIL,
	FDN_WAIT,
	FDN_DEACT_END,
	IDLE_GSM_FAIL,
	PIN2_SIM_FAILURE,    // Marcus: Issue 1419: 16/12/2002
	SIM_LOCKED, // June 16, 2005  REF: CRR 31267  x0021334
	SIM_LOCKED_EMR_EDITOR, // June 16, 2005  REF: CRR 31267  x0021334
	INVALID_IMEI, // Nov 24, 2005, a0876501, DR: OMAPS00045909
	SHOW_IMEI_INVALID,// Nov 24, 2005, a0876501, DR: OMAPS00045909
	TXT_INVALID_IMEI // Nov 24, 2005, a0876501, DR: OMAPS00045909
//x0pleela 18 Oct, 2006 DR: OMAPS00099660
//Flagging the following enums
#ifdef SIM_PERS	
	//x0pleela 30 Aug, 2006 DR: OMAPS00091250
	, NO_MEPD,
	NO_MEPD_DATA,
	TXT_NO_MEPD	
#endif
//x0pleela 05 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
	,PH_LOCK_ALREADY_ENDIS,
	PH_LOCK_STATUS,
	PH_LOCK_REQ_OLD,
	TXT_ENTER_PHLOCK,
	INPUT_PH_LOCK,
	PH_LOCK_END,
	PH_LOCK_REQ_NEW,
	TXT_ENTER_NEW_PHLOCK,
	INPUT_NEW_PH_LOCK,
	INPUT_NEW_PH_LOCK_AGAIN,
	TXT_ENTER_NEW_PHLOCK_AGAIN,
	NEW_PH_LOCK_END,
	NEW_PH_LOCK_FAIL,
	PH_LOCK_NEW_ENDISFAIL,
	PH_LOCK_NEW_ENDSUCC,
	TXT_PH_LOCK_ALL,
	PH_LOCK_ENDIS_FAIL,
	PH_LOCK_ENDSUCC,
	AUTO_PH_LOCK_ENDSUCC,
	PH_LOCK_SUCCESS,
	PH_LOCK_FAILURE,
	PH_UNLOCK_REQ,
	PH_LOCK_EN_FAIL,
	PH_LOCK_SUCC,
	PH_LOCK_AUTO_SUCC,
	PH_LOCK_SETT_ABORT
#endif /*FF_PHONE_LOCK*/	 

} PIN_CASE;
//xashmic 21 Sep 2006, OMAPS00095831
typedef enum
{
	BOOTUP_STATE_NONE = -1,
	BOOTUP_STATE_INPROGRESS,
	BOOTUP_STATE_COMPLETED	
}T_BOOTUP_STATE;
typedef enum
{
	DEFAULT = 0,
	ENABLE,
	DISABLE,
	FDN_ACTIV,
	FDN_DEACTIV,
	CHANGE_PIN,
	CHANGE_PIN2,
	IDLE_GSM,
	PIN2_CHECK,
	PIN1_CHECK
}SET_CASE;

#define FOCUSSED_PINS   1
#define NOT_FOCUSSED_PINS 0

//API define for a flag for PIN Emergency Entry
extern char pin_emerg_call[MIN_PIN];

void pin_init (T_MFW_HND parent_window);
T_MFW_HND pin_create (T_MFW_HND parent_window);
void pin_destroy  (T_MFW_HND own_window);
int sim_event_cb_main (T_MFW_EVENT event, T_MFW_HND para);
T_MFW_HND pin_edt_create (T_MFW_HND parent_window);
void pin_edt_destroy  (T_MFW_HND own_window);
T_MFW_HND pin_mess_create (T_MFW_HND parent_window);
void pin_mess_destroy(T_MFW_HND own_window);
void pin_messages(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
int pin_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reasons);
void sim_rem_init (T_MFW_HND parent_window);
void sim_rem_exit(T_MFW_HND parentWindow);
T_MFW_HND sim_rem_create (T_MFW_HND parent_window);
void sim_rem_destroy (T_MFW_HND own_window);
int sim_rem_cb_main (T_MFW_EVENT event, T_MFW_HND para);
int pinsIsFocussed(void);
void setting_pin_init (T_MFW_HND parent_window);
T_MFW_HND setting_pin_create (T_MFW_HND parent_window);
void setting_pin_destroy  (T_MFW_HND own_window);
int sim_event_cb_setting (T_MFW_EVENT event, T_MFW_HND para);
U16 set_pin_ch_item_flag (struct MfwMnuTag * m,struct MfwMnuAttrTag * ma,struct MfwMnuItemTag * mi);
U16 set_pin_en_item_flag (struct MfwMnuTag * m,struct MfwMnuAttrTag * ma,struct MfwMnuItemTag * mi);
U16 set_pin_dis_item_flag (struct MfwMnuTag * m,struct MfwMnuAttrTag * ma,struct MfwMnuItemTag * mi);
U16 set_pin2_ch_item_flag (struct MfwMnuTag * m,struct MfwMnuAttrTag * ma,struct MfwMnuItemTag * mi);
void set_pin_info_cb(T_MFW_HND win, USHORT identifier, UBYTE reasons);
void set_pin_messages(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
void set_pin_mess_destroy  (T_MFW_HND own_window);
T_MFW_HND set_pin_mess_create(T_MFW_HND parent_window);
void set_pin_edt_destroy  (T_MFW_HND own_window);
T_MFW_HND set_pin_edt_create (T_MFW_HND parent_window);
int pin_change (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int pin2_change (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
void setting_pin_main(T_MFW_HND win, USHORT event, SHORT value, void * parameter);
int pin_disable (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int pin_enable (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int pin_status (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int pin2_fdn_activate (void);
int pin2_fdn_deactivate (void);
#ifdef SIM_PERS
void setLockActionLock(void);
void setLockActionUnlock(void);
void setLockActionChPwd(void);
void setLockActionChkStat(void);
int SIMLock_resetFC (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int setLockTypeNs (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int setLockTypeSp (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int setLockTypeCp (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int setLockTypePs (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int setLockTypeNw (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int get_fc_attempts_left (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int get_fc_max_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int get_fail_reset_fc_max_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int get_succ_reset_fc_max_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int get_fail_reset_fc_attempts_left (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int get_succ_reset_fc_attempts_left (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int get_timer_flag_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int get_etsi_flag_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int get_airtel_ind_flag_value (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
int SIMLock_MasterKeyUnlock (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int SIMLock_MasterKeyUnlock_bootup (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
//x0pleela 25 Sep, 2006 ER: OMAPS00095524
int setLockTypePb (struct MfwMnuTag * m, struct MfwMnuItemTag * i);

USHORT	simlock_check_masterkey(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi);
USHORT	simlock_check_NWCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi);
USHORT	simlock_check_NSCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi);
USHORT	simlock_check_SPCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi);
USHORT	simlock_check_CPCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi);
USHORT	simlock_check_SIMCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi);				
USHORT	simlock_check_CatBlock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi);

//x0pleela 25 Sep, 2006 ER: OMAPS00095524
USHORT	simlock_check_PBCatLock(struct MfwMnuTag *m, 
									   struct MfwMnuAttrTag *ma, 
									   struct MfwMnuItemTag *mi);
int SIMLock_unblock_ME(struct MfwMnuTag * m, struct MfwMnuItemTag * i);
T_MFW_HND mmi_simlock_category_menus(T_MFW_HND parent);
static void mmi_simlock_master_unlock(T_MFW_HND win, UBYTE MK_Unblock_flag);
UBYTE simlock_get_masterkey_status(void);
int simp_unlock_Category(struct MfwMnuTag * m, struct MfwMnuItemTag * i);
#endif
void backpinFDNactdeact(void);
int pin2_check (T_MFW_HND parent_window);
int pin1_check (T_MFW_HND parent_window);
int pinsSetIsFocussed(void);
int fdnActivate(struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int fdnDeactivate(struct MfwMnuTag * m, struct MfwMnuItemTag * i);
U16 set_fdn_on_item_flag (struct MfwMnuTag * m, struct MfwMnuAttrTag * ma, struct MfwMnuItemTag * mi);
U16 set_fdn_off_item_flag (struct MfwMnuTag * m, struct MfwMnuAttrTag * ma, struct MfwMnuItemTag * mi);
int gsm_idle (T_MFW_HND parent_window,char * string);
void pin_exit (void);
int mmiPinsEmergencyCall(void);
void mmiPinsResetEmergencyCall(void);

T_BOOTUP_STATE getBootUpState(void);//xashmic 21 Sep 2006, OMAPS00095831

//x0pleela 06 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
int phLock_Enable_Lock (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int phLock_Automatic_On (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int phLock_Automatic_Off (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int phLock_Change_UnlockCode (struct MfwMnuTag * m, struct MfwMnuItemTag * i);
int phLock_UnLock (void);
int phLock_Lock (void);
#endif /* FF_PHONE_LOCK */
#endif

