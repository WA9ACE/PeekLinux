#ifndef _MMIICONS_H_
#define _MMIICONS_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Icons
 $File:		    Mmiicons.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the implementation of the icon handling in the basic MMI
                        
********************************************************************************
 $History: Mmiicons.h

       Jun 09, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)
       Description : CPHS Roaming indication feature implementation
       Solution     : As part of the implementation, in this file, Roaming icon is added to the list of icons

 	Apr 17, 2006   REF: ER: OMAPS00075178  x0pleela
 	Description: When the charger connect to phone(i-sample), the charger driver works, but the 
 				MMI and App don't work
   	Solution: Added prototype for the function BattChargeEventTim which is the callback called 
   				for the timeout of battery charging timer 
   	
	Apr 27, 2004    REF: ENH 12646  Deepa M.D 
	Fix:		When the SMS memory is full,the SMS icon is made to blink.	   
	25/10/00			Original Condat(UK) BMI version.	
   
 $End

*******************************************************************************/

#include "MmiResources.h" //For display definition



/*******************************************************************************
                                                                              
                                Required Definitions
                                                                              
*******************************************************************************/

#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY  mfwFree




/*******************************************************************************
                                                                              
                                Public Enum types
                                                                              
*******************************************************************************/

/* GW #1471 11/12/02 - Add TI logo as powerdown animation */
#ifdef COLOURDISPLAY
#define animwelcome 10
#define animGoodbye 10
#define CALLANIMATION 4
#else
#define CALLANIMATION 4
#define animwelcome 2
#define animGoodbye 2
#endif
#define screenparts 5

typedef enum                            /* AVAILABLE ICONS IDLE MODE*/
{
    iconIdSMS = 0,                     /* icon New SMS message     */
    iconIdVoice,                        /* icon New Voice mail      */
    iconIdCallFor,                      /* icon Call forward uncond */
    iconIdSilRin,                       /* icon silent ringer       */
    iconIdRoaming,		/*icon Roaming Indicator */ /*Jun 09, 2006 REF:OMAPS00079650  a0393213 (R.Prabakar)*/
    iconIdKeyplock,                     /* icon Keypad locked       */
    iconIdAppProtoActiv,                        /* icon Ciphering              */        
    iconIdAppProtoDeActiv,                        /* icon Ciphering              */            
	iconIdRinger,                       /* icon ringer              */
	iconIdVibrator,                     /* icon vibrator            */
	iconIdRingVibr,                     /* icon ringer + vibrator   */
	iconIdAlarm,                        /* icon alarm on            */
    iconSIMTool,                        /* icon SIM Toolkit         */
    iconMessages,                       /* icon Messages            */
    iconOrganiser,                      /* icon Organiser           */
    iconSounds,                         /* icon Sounds              */
    iconSettings,                       /* icon Settings            */
    iconServices,                       /* icon Services            */
    iconExtra,                          /* icon Extra               */
    iconWAP,							/* icon WAP			   */
    iconIdGPRSOn,						/* icon GPRS ON  (sbh 18/01/02) */
    iconIdHomezone,						/* icon Homezone  (SH 29/11/01) */
    iconIdTTY,							/* SPR#xxx- SH - icon TTY */ 
#ifdef NEPTUNE_BOARD
/* OMAPS00074454 - Edge icon - 24-04-2006 */
    iconIdEdgeOn,                                           /*EDGE Icon */
/* END  */
#endif    
    idlIdMax                            /* number of icons          */
} IconId;

typedef enum                            /* AVAILABLE ICONS INPUT MODE*/
{
    iconAMBLOW = 0,                     /* icon ambigous mode low   */
    iconAMBUPP,                         /* icon ambigous mode upper */
    iconAMBNUM,                         /* icon ambigous mode numeric */
    iconMTLOW,                          /* icon multitap mode low   */
    iconMTUPP,                          /* icon multitap mode upper */
    iconMTNUM,                          /* icon multitap mode numeric */
    inputIdMax                          /* number of icons          */
} IconInputId;

typedef enum                            /* AVAILABLE ICONS BATTERY  */
{
	iconBattery4 = 0,                   /* icon Battery 0-4%        */
	iconBattery14,                      /* icon Battery 5-14%       */
	iconBattery24,                      /* icon Battery 15-24%      */
	iconBattery49,                      /* icon Battery 25-49%      */
	iconBattery100,                     /* icon Battery 50-100%     */
	iconBatteryUnknown,		    /* icon Battery ? 		*/
	iconBattMax                         /* number of batterie icon  */
}IconBattId;

typedef enum                            /* AVAILABLE ICONS BATTERY  */
{
        iconBatteryCharge4 = 0,                   /* icon Battery 0-4%        */
        iconBatteryCharge14,                      /* icon Battery 5-14%       */
        iconBatteryCharge24,                      /* icon Battery 15-24%      */
        iconBatteryCharge49,                      /* icon Battery 25-49%      */
        iconBatteryCharge100,                     /* icon Battery 50-100%     */
        iconBattChargeMax,                  /* icon Battery ?           */
}IconBattChargeId;

typedef enum                            /* AVAILABLE ICONS SIGNAL */
{
	iconSigStat49 = 0,                  /* icon Signal 0-49         */
	iconSigStat99,                      /* icon Signal 50-99        */
	iconSigStat149,                     /* icon Signal 100-149      */
	iconSigStat199,                     /* icon Signal 150-199      */
	iconSigStat255,                     /* icon Signal 200-255      */
	iconSignalMax                       /* number of signal icon    */
}IconSignId;



/*******************************************************************************
                                                                              
                                Externally declared data elements
                                                                              
*******************************************************************************/

extern const MfwIcnAttr idl_Attr[ idlIdMax ];
extern const MfwIcnAttr batt_Attr[ iconBattMax ];
extern const MfwIcnAttr sign_Attr[ iconSignalMax ];
extern const MfwIcnAttr goodbye_Attr[ animGoodbye ];
extern const MfwIcnAttr welcome_Attr[ animwelcome];
#ifdef COLOURDISPLAY
extern const MfwIcnAttr incoming_call_animate [CALLANIMATION];
#endif
extern  MfwIcnAttr callBackDrop;
extern  MfwIcnAttr mainIconSimToolkit;
extern  MfwIcnAttr mainIconCalculator;
extern  MfwIcnAttr mainIconSettings;
extern  MfwIcnAttr mainIconRecentCalls;
extern  MfwIcnAttr mainIconCallDivert;
extern  MfwIcnAttr mainIconSecurity;
extern  MfwIcnAttr mainIconMessages;
extern  MfwIcnAttr mainIconPhonebook;
extern  MfwIcnAttr mainIconWAP;
extern  MfwIcnAttr IconTexteZiText;
extern  MfwIcnAttr IconTexteZiTextSmall;

extern  MfwIcnAttr mainIconBackground;

extern const  MfwIcnAttr IconCol1Attr;
extern const  MfwIcnAttr IconCol2Attr;
extern const  MfwIcnAttr IconCol3Attr;
extern const  MfwIcnAttr IconCol4Attr;
extern MfwIcnAttr IconLogo;

//gb icons for game(four in a row)
extern const unsigned char white_stone_bw[];
extern const unsigned char black_stone_bw[];
extern const unsigned char game_cursor[];

extern const unsigned char game_name1[];
extern const unsigned char player_win1[];
extern const unsigned char player_lost1[];
extern const unsigned char board_full1[];

extern const unsigned char game_name2[];
extern const unsigned char player_win2[];
extern const unsigned char player_lost2[];
extern const unsigned char board_full2[];

//gb icons for melodygenerator
extern const MfwIcnAttr melodygenerator_Attr[ screenparts];
//active frame
extern const unsigned char akt_frame[];
//begin of noteline
extern const unsigned char noteline_begin[];
//notelines
extern const unsigned char noteline_background[];
//end of noteline
extern const unsigned char noteline_end[];
//all the possible musical notes
//all notes with flag up
extern const unsigned char n1[];
extern const unsigned char n1_16[];
extern const unsigned char n1_32[];
extern const unsigned char n1_8[];
extern const unsigned char n1_f[];
extern const unsigned char n1_h[];
//all notes with flag down
extern const unsigned char n5[];
extern const unsigned char n5_16[];
extern const unsigned char n5_32[];
extern const unsigned char n5_8[];
extern const unsigned char n5_h[];
//all musical pauses
extern const unsigned char n4_b[];
extern const unsigned char n8_b[];
extern const unsigned char n16_b[];
extern const unsigned char n32_b[];
extern const unsigned char nf_b[];
extern const unsigned char nh_b[];
//musical sign
extern const unsigned char n_point[];
extern const unsigned char n_hash[];
extern const unsigned char screenpart0[];
extern const unsigned char screenpart1[];
extern const unsigned char screenpart2[];
extern const unsigned char screenpart3[];
extern const unsigned char screenpart4[];
//multicaller icon
extern const unsigned char multi_pointer[];

#define T_BITMAP MfwIcnAttr

enum {
	BGD_NONE,
	BGD_BUBBLES,
	BGD_CLOUD,
	BGD_SQUARE,
	BGD_O2,
	BGD_TI_LOGO,
	BGD_TREE,
	BGD_SUNSET,
	BGD_EDINBURGH1,
	BGD_EDINBURGH2,
	BGD_EDINBURGH3,
	BGD_EDINBURGH4,
	BGD_INCALL,
	BGD_LAST
};

/*******************************************************************************
                                                                              
                                Public method prototypes
                                                                              
*******************************************************************************/

void iconsInit( void );
void iconsExit( void );

void iconsSetState( int icon );
void iconsDeleteState( int icon );
U8 iconsGetState( int icon );

void iconsShow( void );
int  iconsShowBattery( U8 batterystatus );
int  iconsShowSignal( U8 signalstatus );
void iconsInputShow( int IconInputId );

void iconsMainIconsAreaSet (MfwRect selected_area);
void iconShowGoodbye(void); /* Warning Correction */
//  Apr 27, 2004    REF: ENH 12646  Deepa M.D
int iconsShowSmsFull(void ); //to start the timer for blinking SMS Icon 
//x0pleela 11 Apr, 2006  ER: OMAPS00075178
//Callback for battery charging animation timer
int BattChargeEventTim( MfwEvt e, MfwTim *tc );

T_BITMAP* icon_getNetworkIcon( UBYTE* networkNumber);
T_BITMAP* icon_getBgdBitmap( int bgdId );
T_BITMAP *icon_getBitmapFromId( int bgdId, T_BITMAP *bmpData);	
//  Apr 27, 2004    REF: ENH 12646  Deepa M.D
int smsFullEventTim( MfwEvt e, MfwTim *tc ); //Timer callback for blinking SMS Icon(sms_animate)

int icon_networkShow (MfwMnu* m, MfwMnuItem* i);
int icon_networkDontShow (MfwMnu* m, MfwMnuItem* i);

#define ICON_VISIBLE	(0)
#define ICON_INVISIBLE	(0xFF)
/*******************************************************************************

  End Of File
  
*******************************************************************************/

#endif

