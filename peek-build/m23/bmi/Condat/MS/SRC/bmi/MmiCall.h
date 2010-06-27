/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Call
 $File:		    mmiCall.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
                        
********************************************************************************

 $History: mmiCall.h

 
	August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat)
	Description:	  COMBO PLUS:	Voice can not be heard in a call when MP3 tone was set
								 Have an MP3 tone set as Ringer. Make a call to the target.
								 Accept the same, no voice can be heard from the calling 
								 device or from the board that received 
	Issue:		Timing Synchronization Issue between BMI-ACI-L1.
				BMI calls sAT_A to accept the call. ACI then sends the RING_OFF signal
				to BMI, wherein BMI does a Ringer Stop. After this, ACI calls Vocoder
				Enable. However, the Ringer Stop goes to L1 much later after Vocoder
				Enable reaches L1 due to which the Vocoder eventually gets disabled.
	Solution: 		The Vocoder Enable Implementation is removed from ACI and
				shall be called by BMI on receipt of AS_STOP_IND
				(AS_STOP_IND is the message sent by AS to BMI when BMI invokes
				 as_stop for ringer_stop). This way, it is assured that the Vocoder
				 Enable Request reaches L1 only after Ringer Stop is completely done
					 
      Oct 8 2006 OMAPS00097714 x0039928(sumanth)
      TTY HCO/VCO options in MMI
      
 	Mar 11, 2006   REF:DR:OMAPS00061467  x0035544
	Description:   27.22.4.13.3 SET UP CALL (display of icons) fails.
	Solution: created a variable of type T_SAT_IconData at the structure T_call to store the Icon data
	inorder to display it on calling screen.

  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
 	Description:	Support for various audio profiles
 	Solution:	The support for audio device's carkit/headset/loudspeaker/handheld.
 			The audio device is enabled as per the user actions.
 			
	25/10/00			Original Condat(UK) BMI version.	


    Mar 28, 2006 REF:ER OMAPS00071798  x0043642
    Added new options in BMI to accept the value for tty audio mode,
    and the accepted value is passed as parameter in function call sAT_PercentCTTY

 $End

*******************************************************************************/

#ifndef _DEF_MMI_CALL_H_
#define _DEF_MMI_CALL_H_

#define ALLOC_MEMORY mfwAlloc
#define FREE_MEMORY  mfwFree

//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
#include "mfw_mme.h"
#endif
#include "mfw_phb.h"
#include "mfw_cm.h"
#include "MmiMmi.h"
#include "MmiDialogs.h" //GW 29/11/01 - Added 
#include "MmiIdle.h" // SPR#0630 - SH - EDITOR_SIZE now defined in MmiIdle.h

typedef enum
{
    CallNone=CallDummy,
    CallNormal,
    CallDone,
	CallNew,
    CallSingle,
    CallTwo,
    CallConference,
	CallPrivateCall,
	CallSingleMultiparty,
	CallMultipartySingle

} E_CALL_TYPE;/*a0393213 warnings removal-typedef name added*/

typedef enum
{
    ViewConnect,
    ViewEnd,
    ViewMenu,
    ViewCalling,
    ViewIncoming,
    ViewDTMF,
	ViewDTMFMenu,
	ViewCallError,
	ViewSelectPrivateCall,
	ViewScratchPad,
	ViewWaitingAnswer,
	ViewAutoRedial,
	ViewSendingDTMF,
	ViewShortWait,		//GW Added 
	ViewMulticallControl, 
	ViewMulticallControlCallWaiting,
	ViewMulticallControlIncomingCall

} E_CALL_MESSAGE; /*a0393213 warnings removal-typedef name added*/

typedef enum
{
    CallActive,
    CallInactive
    
} E_CALL_STATUS; /*a0393213 warnings removal-typedef name added*/

extern UBYTE callStatus;
extern int idwithheld;

#define true 1
#define false 0

#define MAX_CALLS 6 

/*mc, SPR 1319*/
#ifdef LSCREEN
#define MAX_CHAR PHB_MAX_LEN	/*MC SPR 1442*/
#else
#define MAX_CHAR 12
#endif
#define MAX_LINE 14


#define FIRST_TYPED  20000

typedef struct{
    short id;
    char number[MAX_CHAR];
    char name[MAX_CHAR];
    UBYTE state;
} tMmiCall;

typedef struct{
    int mode;
    UBYTE muted;
    UBYTE view;
    UBYTE numCalls;
    SHORT selected;
	long time[MAX_CALLS];
    T_MFW_CM_STATUS status[MAX_CALLS];

} tMmiCallTable;

typedef enum
{
	CALL_INIT,
	CALL_SCRATCHPAD_INIT,
	CALL_CALLSELECT_INIT,		//GW Added ????
	CALL_DESTROY_WAITING,
	CALL_OUTGOING_SAT,
	CALL_DESTROY_CALLING_WINDOW
} E_CALL_EVENTS; /*a0393213 warnings removal-typedef name added*/



/* SPR#1352 - SH - Possible status values for TTY */
#ifdef MMI_TTY_ENABLED
typedef enum
{
	CALL_TTY_OFF = 0,	/* TTY always off */
	CALL_TTY_ALWAYSON,	/* TTY always on */
	CALL_TTY_ONNEXTCALL	/* TTY on for next call only */
} E_CALL_TTY; /*a0393213 warnings removal-typedef name added*/
#endif
/* end SH */

/*
** This is a tri-state flag, to ensure that the Vocoder is not disabled
** when a waiting call is notified while already in call.
*/
typedef enum
{
	CALL_VOCODER_IDLE = 0,		/* BMI in Idle State, Vocoder not under BMI control */
	CALL_VOCODER_DISABLED,	/* Vocoder is disabled prior to the call being connected */
	CALL_VOCODER_INCALL		/* Vocoder is enabled and In-Call */
} tMmiVcState;

struct QUEUETYPE
{
  SHORT          call_number;
  struct QUEUETYPE*   next;	
};

typedef struct QUEUETYPE TYPEQUEUE;

typedef struct
{
    T_MMI_CONTROL   mmi_control;
	T_MFW_HND win;     // window handle
	T_MFW_HND kbd;
    T_MFW_HND kbd_long;
	T_MFW_HND win_search;
	T_MFW_HND cm;
	T_MFW_HND win_incoming;
	T_MFW_HND win_calling;
	T_MFW_HND win_ending;
	T_MFW_HND win_menu;
	T_MFW_HND win_waiting;
	T_MFW_HND win_sending_dtmf;
#ifdef NEW_EDITOR
	T_ED_DATA *editorCall;	/* SPR#1428 - SH - New Editor changes */
#else /* NEW_EDITOR */
	T_MFW_HND editCall;
#endif /* NEW_EDITOR */
	T_MFW_HND win_redial;
    T_MFW_HND timer;

	/*SPR 1392*/
	T_MFW_HND win_deflectEdit; 		/*handle for call deflect editor.*/
	T_MFW_HND win_deflecting; 		/*"Deflecting to..." window*/
	T_MFW_HND win_deflectSearch; 	/*Call deflection phonebook search window*/
	char editor_buffer[20];			/*call deflection number buffer*/
	T_DISPLAY_DATA inc_call_data;	/*incoming call dialogue display info*/
	/*1392 end*/
	T_MFW_PHB_ENTRY entry;
	int current_command;
	UBYTE callStatus;
	tMmiCallTable calls;
	int accessEnd;			// controls when several calls are to be ended
	int callWaitingEnd;
	int emergencyCall;
	int callsToEnd;
	long timeEnded;
	TYPEQUEUE *endQueue;
	SHORT incCall;
	SHORT outCall;
	int refreshConnect;
	int firstChar;
	int currentModule;
	int currentVolume;
	SHORT singleCall;
	unsigned char charSent;
	char sendingDTMF;
	T_MFW_CM_CW_INFO globalCWInfo;   /* information about incoming call */
	char phbNameNumber[EDITOR_SIZE];
	char edtBufDTMF[EDITOR_SIZE];    /* edit buffer              */
#ifdef NEW_EDITOR
	T_ED_ATTR editorAttrCall;	/* SPR#1428 - SH - New Editor changes */
	T_ED_ATTR editorAttrDTMF;
#else /* NEW_EDITOR */
	MfwEdtAttr edtAttrCall;
	MfwEdtAttr edtAttrDTMF;          /* edit attributes:         */
#endif /* NEW_EDITOR */
	T_MFW_CM_AOC_INFO aocInfo;
	USHORT ignore_disconnect;
	T_MFW_CM_CALL_DIR call_direction;
	UBYTE colp_number[MFW_NUM_LEN];  /* colp number     */
	int animationScreen;
#ifdef MMI_TTY_ENABLED
	UBYTE tty;	/* SPR#1352 - SH - TTY status */
#endif
//x0035544 Mar 09, 2006 DR:OMAPS00061467
#ifdef FF_MMI_SAT_ICON
  T_SAT_IconData       IconData; //Sat icon information
#endif
	tMmiVcState vocoderState;
} T_call;
/*x0039928 OMAPS00097714 HCO/VCO options - added to differenciate between normal/HCO/VCO tty option*/
#ifdef FF_TTY_HCO_VCO
typedef enum
{
	TTY_ONLY_PF = 0,	/* TTY only */
	TTY_VCO_PF,	/* TTY HCO */
	TTY_HCO_PF 	/* TTY VCO  */
}
T_MMI_TTY_PF_TYPE;
#endif

/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- Start */

typedef enum
{
	BMI_INCOMING_CALL_NONE = 0,
	BMI_INCOMING_CALL_RINGERSTOPPED
}
E_IN_CALL_RINGER_STATUS;

void bmi_incomingcall_set_ringer_status(E_IN_CALL_RINGER_STATUS eStatus);
void bmi_incomingcall_get_ringer_status(E_IN_CALL_RINGER_STATUS *peStatus);

/*August 01, 2007 DR: OMAPS00137399 x0080701 (Bharat) -- End  */

                                        /* PROTOTYPES               */
void callInit (MfwHnd parent);
void callExit (void);

T_MFW_HND call_create (T_MFW_HND parent_window);
void call_destroy (T_MFW_HND own_window);


void callExec (int reason, MmiState next);

void callNumber(UBYTE* number);

UBYTE call_status(void);
MfwHnd call_get_window(void);

void StartsendDTMFString(char * String);
int menuInCallTimerOn(MfwMnu* m, MfwMnuItem* i);
int menuInCallTimerOff(MfwMnu* m, MfwMnuItem* i);

int call_ccbs(MfwMnu* menu, MfwMnuItem* item); /* Marcus: CCBS: 1/11/2002 */

/*MC, SPR 1392*/
int call_deflection_on(MfwMnu* m, MfwMnuItem* i);
int call_deflection_off(MfwMnu* m, MfwMnuItem* i);
/*end*/
/* SPR#1352 - SH - TTY */
#ifdef MMI_TTY_ENABLED
void call_tty_init(void);
UBYTE call_tty_get(void);
void call_tty_set(UBYTE tty);
int call_tty_menu(MfwMnu* menu, MfwMnuItem* item);
int call_tty_menuselect(MfwMnu* menu, MfwMnuItem* item);
void call_tty_statuswin(void);
void call_tty_revert(void);
#endif
/* end SH */
//  	xpradipg - LOCOSTO-ENH-31895 : 23 June 2005
#ifdef FF_MMI_AUDIO_PROFILE
USHORT item_headset_status( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT item_handheld_status( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT item_loudspeaker_status( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
USHORT item_carkit_status( struct MfwMnuTag *m, struct MfwMnuAttrTag *ma, struct MfwMnuItemTag *mi );
int M_exeHandheld (MfwMnu* m, MfwMnuItem* i);
int M_exeLoudspeaker (MfwMnu* m, MfwMnuItem* i);
int M_exeHeadset (MfwMnu* m, MfwMnuItem* i);
int M_exeCarkit (MfwMnu* m, MfwMnuItem* i);
void mmi_hook_handle();
#endif

/* Mar 27, 2006    REF:ER OMAPS00071798 */
#ifdef NEPTUNE_BOARD  
int ctty_audio_mode_text(MfwMnu* m, MfwMnuItem* i);
int ctty_audio_mode_vco(MfwMnu* m, MfwMnuItem* i);
int ctty_audio_mode_unknown(MfwMnu* m, MfwMnuItem* i);
#endif
/*x0039928 OMAPS00097714 HCO/VCO option - fn prototypes of menu handlers*/
#ifdef FF_TTY_HCO_VCO
int call_tty_co_menuselect_always(MfwMnu* menu, MfwMnuItem* item);
int call_tty_co_menuselect_nextcall(MfwMnu* menu, MfwMnuItem* item);
#endif

#endif
