#ifndef _MMI_CPHS_H_
#define _MMI_CPHS_H_

/*******************************************************************************

          CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.

********************************************************************************

 $Project name: Basic MMI
 $Project code: BMI (6349)
 $Module:   CPHS
 $File:       MmiCPHS.h
 $Revision:   1.0

 $Author:   Condat(UK)
 $Date:       25/01/02

********************************************************************************

 Description:

  Handles CPHS functionality


********************************************************************************
 $History: MmiCPHS.h

	May 30, 2006  DR: OMAPS00070657 x0pleela
	Description: CPHS feature on Locosto-Lite
	Solution: For ALS feature,
			Added prototypes for the functions cphs_edit_line1(), cphs_edit_line2(), 
			mmi_cphs_set_als_value(),  mmi_cphs_get_als_value( void ), 
			T_MFW_LINE_INDEX mmi_get_als_selectedLine( void)

//  May 31, 2004        REF: CRR 17291  xvilliva
//	Bug:	After power cycle the setting of the used line which have been made 
//			from the BMI is lost.
//	Fix:	The global variable g_Pin2 is declared here.

  25/01/02      Original Condat(UK) BMI version.

 $End

*******************************************************************************/
#include "mfw_cphs.h"
#include "mfw_mfw.h"
#include "MmiBookShared.h"
#include "mfw_mnu.h"

typedef enum
{
  LINE1,
  LINE2,
  FAX,
  DATA
} ALS_lines;
#define VOICEMAIL 0x7070
typedef enum
{
  MAILBOX,
  CHECK_STATUS
} T_CPHS_FUNCS;


#ifdef FF_CPHS_REL4
typedef enum
{
  SRV_CHV1_Disable = 1,  /* #1  CHV1 disable function                                     */
  SRV_ADN,               /* #2  Abbreviated Dialling Numbers (ADN)                        */
  SRV_FDN,               /* #3  Fixed Dialling Numbers (FDN)                              */
  SRV_SMS_Storage,       /* #4  Short Message Storage (SMS)                               */
  SRV_AOC,               /* #5  Advice of Charge (AoC)                                    */
  SRV_CCP,               /* #6  Capability Configuration Parameters (CCP)                 */
  SRV_PLMN_Select,       /* #7  PLMN selector                                             */
  SRV_RFU1,              /* #8  RFU                                                       */
  SRV_MSISDN,            /* #9  MSISDN                                                    */
  SRV_EXT1,              /* #10 Extension1                                                */
  SRV_EXT2,              /* #11 Extension2                                                */
  SRV_SMS_Parms,         /* #12 SMS Parameters                                            */
  SRV_LDN,               /* #13 Last Number Dialled (LND)                                 */
  SRV_CBM_Ident,         /* #14 Cell Broadcast Message Identifier                         */
  SRV_GrpLvl1,           /* #15 Group Identifier Level 1                                  */
  SRV_GrpLvl2,           /* #16 Group Identifier Level 2                                  */
  SRV_SrvProvName,       /* #17 Service Provider Name                                     */
  SRV_SDN,               /* #18 Service Dialling Numbers (SDN)                            */
  SRV_EXT3,              /* #19 Extension3                                                */
  SRV_RFU2,              /* #20 RFU                                                       */
  SRV_VCGS,              /* #21 VGCS Group Identifier List (EFVGCS and EFVGCSS)           */
  SRV_VBS,               /* #22 VBS Group Identifier List (EFVBS and EFVBSS)              */
  SRV_EMLPP,             /* #23 enhanced Multi Level Precedence and Pre emption Service   */
  SRV_AutoEMLPP,         /* #24 Automatic Answer for eMLPP                                */
  SRV_DtaDownlCB,        /* #25 Data download via SMS CB                                  */
  SRV_DtaDownlPP,        /* #26 Data download via SMS PP                                  */
  SRV_MnuSel,            /* #27 Menu selection                                            */
  SRV_CalCntrl,          /* #28 Call control                                              */
  SRV_ProActSIM,         /* #29 Proactive SIM                                             */
  SRV_CBMIdRnge,         /* #30 Cell Broadcast Message Identifier Ranges                  */
  SRV_BDN,               /* #31 Barred Dialling Numbers (BDN)                             */
  SRV_EXT4,              /* #32 Extension4                                                */
  SRV_DePersCK,          /* #33 De personalization Control Keys                           */
  SRV_CoOpNwL,           /* #34 Co operative Network List                                 */
  SRV_SMS_StatRep,       /* #35 Short Message Status Reports                              */
  SRV_NwIndAlMS,         /* #36 Network's indication of alerting in the MS                */
  SRV_MOSMCtrlSIM,       /* #37 Mobile Originated Short Message control by SIM            */
  SRV_GPRS,              /* #38 GPRS                                                      */
  SRV_RFU3,              /* #39 Image (IMG)                                               */
  SRV_RFU4,              /* #40 SoLSA (Support of Local Service Area)                     */
  SRV_USSDsupportInCC,   /* #41 USSD string data object supported in Call Control         */
  SRV_No_42,             /* #42 RUN AT COMMAND command                                    */
  SRV_No_43,             /* #43 User controlled PLMN Selector with Access Technology      */
  SRV_No_44,             /* #44 Operator controlled PLMN Selector with Access Technology  */
  SRV_No_45,             /* #45 HPLMN Selector with Access Technology                     */
  SRV_No_46,             /* #46 CPBCCH Information                                        */
  SRV_No_47,             /* #47 Investigation Scan                                        */
  SRV_No_48,             /* #48 Extended Capability Configuration Parameters              */
  SRV_No_49,             /* #49 MExE                                                      */
  SRV_No_50,             /* #50 RPLMN last used Access Technology                         */
  SRV_PNN,               /* #51 PLMN Network Name                                         */
  SRV_OPL,               /* #52 Operator PLMN List                                        */
  SRV_No_53,             /* #53 Mailbox Dialling Numbers                                  */ 
  SRV_No_54,             /* #54 Message Waiting Indication Status                         */
  SRV_No_55,             /* #55 Call Forwarding Indication Status                         */
  SRV_No_56,             /* #56 Service Provider Display Information                      */
  SRV_No_57,             /* #57 Multimedia Messaging Service (MMS)                        */
  SRV_No_58,             /* #58 Extension 8                                               */
  SRV_No_59              /* #59 MMS User Connectivity Parameters                          */
} T_SIM_SRV;
#endif

//xvilliva SPR17291
extern char g_pin2[MAX_PIN+1];

/*a0393213 cphs rel4*/
#ifdef FF_CPHS_REL4
extern T_MFW_CPHS_REL4_DV_STAT call_forwarding_status_rel4;
extern BOOL isCFISFilePresent();
#endif

//initialise CPHS
T_MFW_HND mmi_cphs_init(void); // RAVI

//call mailbox numbers
int menu_cphs_call_mailbox_number(MfwMnu* m, MfwMnuItem* i);

//edit mailbox numbers
int menu_cphs_edit_mailbox_number(MfwMnu* m, MfwMnuItem* i);

//get operator name from CPHS SIM
T_MFW_CPHS_OP_NAME* CphsRequestOperatorName(void); // RAVI

//returns TRUE if present
UBYTE CphsPresent(void); // RAVI

//returns TRUE if SIM configured for mailbox number
USHORT CphsMailboxPresent(void /* struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi*/ ); // RAVI

//Get voicemail status, returns status of line requested in parameter
T_MFW_CFLAG_STATUS GetCphsVoicemailStatus(UBYTE line);

//get call forwarding status of specified line
T_MFW_CFLAG_STATUS GetCphsDivertStatus(UBYTE line);

//get current enabled ALS line
int cphs_als_status(MfwMnu* m, MfwMnuItem* i);

#ifdef FF_CPHS_REL4  
int build_profile_list(MfwMnu* m, MfwMnuItem* i);
int build_profile_list1(MfwMnu* m, MfwMnuItem* i);
USHORT MSPPresent( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
int item_flag_isMWISFilePresent(struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi);
int item_flag_isMBDNFileNotPresent(struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi);
int item_flag_isMBDNFilePresent(struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi);
int menu_cphs_rel4_show_mailbox_number(MfwMnu* m, MfwMnuItem* i);
BOOL isMWISFilePresent(void);
BOOL isMBDNFilePresent(void);
#endif

// Set the current CPHS ALS lock status
int cphs_lock_als(MfwMnu* m, MfwMnuItem* i);
int cphs_unlock_als(MfwMnu* m, MfwMnuItem* i);

//Set ALS line to Line 1
int cphs_line1_select(MfwMnu* m, MfwMnuItem* i);

//Set ALS line to Line 2
int cphs_line2_select(MfwMnu* m, MfwMnuItem* i);

//Display any info numbers
int InfoNumbers( MfwMnu *m, MfwMnuItem *i );

//Pop up a dialogue
void ShowMessage(T_MFW_HND win, USHORT TextId1, USHORT TextId2);

//check if ALS
USHORT ALSPresent( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT CphsALSUnlocked( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT CphsALSLocked( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );

//check if any info nums
USHORT InfoNumbersPresent( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
void mmi_cphs_refresh(void); // RAVI
UBYTE GetAlsLine(void); // RAVI

#ifdef FF_CPHS
//x0pleela 25 May, 2006  DR: OMAPS00070657
int  cphs_edit_line1(MfwMnu* m, MfwMnuItem* i);
int  cphs_edit_line2(MfwMnu* m, MfwMnuItem* i);

//x0pleela 29 May, 2006  DR: OMAPS00070657
void mmi_cphs_set_als_value( UBYTE als_val);
UBYTE  mmi_cphs_get_als_value( void );
T_MFW_LINE_INDEX mmi_get_als_selectedLine( void);
#endif

#endif
