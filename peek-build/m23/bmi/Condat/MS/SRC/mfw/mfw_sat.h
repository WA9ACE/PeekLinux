/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_sat.h       $|
| $Author:: Xsp $ CONDAT GmbH           $Revision:: 5               $|
| CREATED: 07.05.99                     $Modtime:: 29/02/00 18:19   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_SAT

   PURPOSE : SIM Application Toolkit Handling

   EXPORT  :

   TO DO   :

   $History:: mfw_sat.h                                             $
      Apr 18, 2006 Ref:OMAPS00116309 R.Prabakar (a0393213)
      CT-GCF[27.22.4.7.1]- MMI crashes on REFERSH seq 1.2
      Solution : Data type mismatch between ACI and MMI sorted out
      
	Nov 07, 2006 ER:OMAPS00070661 R.Prabakar(a0393213)
	R99 network compliancy : Implemented Language Notification and Provide Local Information (Language setting)
	SAT proactive commands and Language Selection SAT event
   
	Sep 12, 2006 DR: OMAPS00083156 - x0039928
	Description: CT-GCF/PTCRB-LL[27.22.4.8.1]-MS does not display the complete text
	Solution : The cmd fill array size is increased from 448 to 750 if sat icon is enabled to 
	accomodate complete information.
	
       Shashi Shekar B.S., a0876501, 16 Mar, 2006, OMAPS00061462
       Icon support for SetupMenu & Select item.

	Jan 16, 2006 DR: OMAPS00061460 - Shashi Shekar B.S.
	Description: SAT Icon support
	Solution : SAT icon support added.

*
* Jun 16, 2005    REF: CRR 31543   x0021334
* Issue: SATK: cannot get TR for send ussd command
* Solution: When the user presses "Hang up" key when   
*              the SAT USSD session is in progress,a call is
*             made to function 'sAT_end_ussd()' to end the session.
*
 *****************  Version 5  *****************
 * User: Xsp          Date: 3/03/00    Time: 12:51
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Changfed type of xxxWindow fields in MfwSat struct...
 * 
 * *****************  Version 4  *****************
 * User: Xsp          Date: 14/02/00   Time: 18:11
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Solved compiler errors...
 * 
 * *****************  Version 3  *****************
 * User: Xsp          Date: 14/02/00   Time: 14:43
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added window pointers to sat structure...
 * 
 * *****************  Version 2  *****************
 * User: Fk           Date: 11.02.00   Time: 15:40
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * New event 'SAT Session  End' derived from empty command
 * 
 * *****************  Version 1  *****************
 * User: Es           Date: 6.07.99    Time: 12:42
 * Created in $/GSM/DEV/MS/SRC/MFW
 * SIM application toolkit handler
 * Initial
*/

#ifndef _DEF_MFW_SAT_H_
#define _DEF_MFW_SAT_H_


#include "mfw_win.h"
#include "mfw_phb.h" /* top get aci_cmh.h */
#include "mfw_ss.h"  // Jun 16, 2005    REF: CRR 31543   x0021334

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	#include "mfw_sima.h"
#endif

#define MFW_MAX_NT_SIZE 6        /* save changed files notification */

#define MFW_MAX_PFP_ELEM 15

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
typedef struct
{							/* SAT ICON DATA   */
	U8 width;				/* SAT Icon width  */
	U8 height;				/* SAT Icon height */
	U8 pad1;				/* Sep 12, 2006 DR: OMAPS00083156 - x0039928 - Padding*/
	U8 pad2;				/* Sep 12, 2006 DR: OMAPS00083156 - x0039928 - Padding*/
	char* dst;				/* Icon data	   */
} SatIconInfo;
#endif

typedef struct
{                                       /* SAT ICON IDENTIFIER      */
    U8 qual;                            /* icon qualifier           */
    U8 id;                              /* icon identifier          */
    U8 pad1;
    U8 pad2;
} SatIconId;

typedef struct
{                                       /* SAT TEXT STRING          */
    SatIconId icon;                     /* icon identifier          */
    U16 text;                           /* offset to text string    */
    U8 responseFlag;                    /* immediate response       */
    U8 code;                            /* data coding scheme       */
    U8 len;                             /* length of text           */
    U8 pad1;
    U8 pad2;
    U8 pad3;
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
	SatIconInfo iconInfo;
#endif
} SatTxt;

typedef struct
{                                       /* SAT ADDRESS              */
    U16 number;                         /* offset to dial number    */
    U8 ton;                             /* type of number, npi      */
    U8 len;                             /* length of dial number    */
} SatAddress;

typedef struct
{                                       /* SAT MISCELLEANEOUS DATA  */
    U16 data;                           /* offset to data           */
    U8 len;                             /* length of data           */
    U8 pad;
} SatData;


typedef struct
{                                       /* SAT LIST DATA            */
    U8 len;                             /* number of bytes in list  */
    U8 elem [1];                        /* list elements            */
} SatList;

typedef struct
{                                       /* SAT SMS TPDU             */
    U16 data;                           /* offset to pdu data       */
    U8 len;                             /* pdu length               */
    U8 pad;
} SatSmsPdu;

typedef struct
{                                       /* SAT MENU ITEM            */
    U16 text;                           /* offset to text data      */
    U8 id;                              /* item identifier          */
    U8 action;                          /* next action id           */
    U8 icon;                            /* items icon               */
    U8 len;                             /* length of text           */
    U8 pad1;
    U8 pad2;
// Shashi Shekar B.S., a0876501, Feb 4, 2006, DR: OMAPS00061462
#ifdef FF_MMI_SAT_ICON
	SatIconInfo iconInfo;
#endif
} SatItem;

typedef struct
{                                       /* SAT INPUT USER STRING    */
    SatTxt prompt;                      /* prompt string            */
    SatTxt defRsp;                      /* default response         */
    U8 rspMin;                          /* minimal response length  */
    U8 rspMax;                          /* maximal response length  */
    U8 pad1;
    U8 pad2;
} SatInput;

typedef struct
{                                       /* SAT PLAY TONE            */
    SatTxt alpha;                       /* tones alpha tag          */
    U8 tone;                            /* code for tone            */
    U8 durUnit;                         /* code for duration unit   */
    U8 durValue;                        /* duration value           */
    U8 pad;
} SatTone;

typedef struct
{                                       /* SAT SETUP MENU           */
    SatTxt header;                      /* menu header              */
    U8 itemIconQual;                    /* item icon qualifier      */
    U8 nItems;                          /* number of menu items     */
    U8 pad1;
    U8 pad2;
    SatItem items [1];                  /* menu items (min. one)    */
} SatMenu;

typedef struct
{                                       /* SAT SEND SMS             */
    SatTxt info;                        /* user information         */
    SatAddress addr;                    /* address                  */
    SatSmsPdu sms;                      /* SMS TPDU                 */
} SatSMS;

typedef struct
{                                       /* SAT SEND SS              */
    SatTxt info;                        /* user information         */
    SatAddress ss;                      /* SS string                */
} SatSS;

typedef struct
{                                       /* SAT SETUP CALL           */
    SatTxt info;                        /* user information         */
    SatTxt info2;                       /* setup information        */
    SatAddress addr;                    /* address                  */
    SatData bc;                         /* bearer capabilities      */
    SatData sa;                         /* subaddress               */
    U8 durUnit;                         /* redial time unit         */
    U8 durValue;                        /* redial max time          */
    U8 pad1;
    U8 pad2;
} SatCall;

typedef struct
{                                       /* SAT EXEC AT/DTMF         */
    SatTxt info;                        /* user information         */
    SatData cmd;                        /* AT cmd / DTMF sequence   */
} SatExec;

typedef struct
{                                       /* CALL CONTROL RESULT      */
    SatAddress addr;                    /* address                  */
    SatAddress addr2;                   /* address on SMS control   */
    SatData bc;                         /* bearer capabilities      */
    SatData bc2;                        /* bearer capabilities 2    */
    SatData sa;                         /* subaddress               */
    SatTxt info;                        /* alpha information        */
    S32 redialTime;                     /* maximal redial time      */
    S16 callId;                         /* id of new call           */
    U8 bcRepInd;                        /* BC repeat indicator      */
    U8 result;                          /* call control result      */
} SatCcRes;

typedef struct
{                                       /* SAT REFRESH FILES        */
    /*a0393213 OMAPS00116309 Data type of 'files' changed from U16* to T_file_info*
	because of R99 change in ACI*/
    T_file_info* files;                          /* Pointer to the files list      */
    U16 files_nr;                         /* length of the files list       */
	U8 status;				             /*Status of the refresh procedure */
} SatRefreshFiles;						 /* -1 -> to be done  or       */
										 /* SIM_FU_ERROR,SIM_FU_SUCCESS,SIM_FU_SUCC_ADD */
#if defined(FF_WAP)

typedef struct
{
U8 prov_file_id_len;                               /*length of provisional file ids*/
U8* prov_file_id;                                  /*provisiona file ids */
} ProvFileRef;

typedef struct
{                                               /* SAT LAUNCH BROWSER COMMAND      */
U8 identity;  									/* Browser identity as in GSM 11.14 12.47 */
U8* url;								        /* Universal Res Loc as in GSM 11.14 12.48*/
U8  n_bearer;									/* Number of elements in the bearer list */
U8* bearer;								        /* Bearer list as in GSM 11.14 12.49 (Null terminated)*/
U8  n_prov_file_ref;							/* Number of elements in the prov. file ref. table. */
ProvFileRef prov_file_ref[MFW_MAX_PFP_ELEM];       /* Prov. file ref. list*/ 
SatTxt text_string;								/*Text string as in GSM 11.14 12.15 */
U8 alpha_len;									/*Alpha identifier length as in GSM 11.14 12.2 */
U8* alpha_id;                                   /*Alpha identifier as in GSM 11.14 12.2 */
} SatLaunchBrowser;						 

#endif /*FF_WAP*/


/* SPR#1786 - DS - SAT Class E */

typedef enum
{
    MFW_SAT_EST_NONE = 0,               /* No establishment type given */
    MFW_SAT_EST_IMMEDIATE,             /* Immediate channel establishment */
    MFW_SAT_EST_ONDEMAND             /* On-demand channel establishment */
    
} T_MFW_SAT_EST;

typedef enum
{
    MFW_SAT_CHN_NONE = 0,               /* No channel type given */
    MFW_SAT_CHN_VOICE,                   /* Channel type Voice */
    MFW_SAT_CHN_CSD,                      /* Channel type CSD */
    MFW_SAT_CHN_GPRS                     /* Channel type GPRS */
    
} T_MFW_SAT_CHN;

typedef struct
{
    T_MFW_SAT_CHN channel_type;
    T_MFW_SAT_EST establishment_type;
    SatTxt alpha_id;                                   /* Alpha identifier as in GSM 11.14 section 12.22 */
    U8 dur1Unit;                         /* redial time unit duration 1        */
    U8 dur1Value;                        /* redial max time duration 1         */
    U8 dur2Unit;                         /* redial time unit duration 2        */
    U8 dur2Value;                        /* redial max time duration 2         */
} SatOpenChannel;

typedef struct
{
    SatTxt alpha_id;                                   /* Alpha identifier as in GSM 11.14 section 12.22 */
} SatCloseChannel;

typedef struct
{
    SatTxt alpha_id;                                   /* Alpha identifier as in GSM 11.14 section 12.22 */
} SatSendData;

typedef struct
{
    SatTxt alpha_id;                                   /* Alpha identifier as in GSM 11.14 section 12.22 */
} SatReceiveData;

#ifdef FF_MMI_R99_SAT_LANG
/*OMAPS00070661(SAT-Lang Notification) a0393213(R.Prabakar)*/
typedef struct
{
    U8 lang[2];  /*stores language according to ISO639*/
} SatLanguageNotification;
#endif

typedef struct SatCmdTag                /* SAT COMMAND BUFFER       */
{
    U8 number;                          /* command number           */
    U8 type;                            /* command type             */
    U8 qual;                            /* command qualifier        */
    U8 source;                          /* source device            */
    U8 dest;                            /* destination device       */
    U8 pad1;
    U8 pad2;
    U8 pad3;
    union                               /* command data             */
    {
        SatTxt text;                    /* display text             */
        SatInput inp;                   /* input user string        */
        SatTone tone;                   /* play tone                */
        SatMenu menu;                   /* setup menu               */
        SatSMS sms;                     /* send SMS                 */
        SatSS ss;                       /* send SS                  */
        SatCall call;                   /* setup call               */
        SatList events;                 /* setup event list         */
        SatExec cmd;                    /* execute AT/DTMF command  */
        SatCcRes ccres;                 /* call control information */
		SatRefreshFiles refFiles;		/* files to be refreshed    */
        SatData files;                  /* old code for REFRESH*/
#if defined(FF_WAP)
	    SatLaunchBrowser browser;      /* launch browser command */
#endif
        /* SPR#1786 - DS - Added the following SAT Class E commands */
        SatOpenChannel open;            /* open channel command */
        SatCloseChannel close;          /* close channel command */
        SatSendData send;               /* send data command */
        SatReceiveData receive;        /* receive data command */
#ifdef FF_MMI_R99_SAT_LANG		
        SatLanguageNotification language;   /*language notification OMAPS00070661 a0393213(R.Prabakar)*/
#endif
// Sep 12, 2006 DR: OMAPS00083156 - x0039928 
// Fix : The fill array size is increased from 448 to 750 if sat icon is enabled
#ifdef FF_MMI_SAT_ICON		
        U8 fill [750];                  /* maximal union size       */
#else
        U8 fill [448];                  /* maximal union size       */
#endif
    } c;
} SatCmd;

typedef struct MfwSatTag                /* SAT CONTROL BLOCK        */
{
    MfwEvt map;                         /* selection of events      */
    MfwEvt evt;                         /* current event            */
    MfwCb  handler;                     /* event handler            */
    SatCmd *cmd;                        /* event related data       */
    MfwHnd parentWindow;                /* mother of all SAT windows */
    MfwHnd currentWindow;               /* SAT window in use         */
} MfwSat; 

                                        /* SAT COMMAND TYPES        */
#define SatCmdRefresh       0x01        /* refresh command          */
#define SatCmdMoreTime      0x02        /* more time command        */
#define SatCmdPollTime      0x03        /* poll interval command    */
#define SatCmdPollOff       0x04        /* polling off command      */
#define SatCmdEvtList       0x05        /* set event list command   */
#define SatCmdCall          0x10        /* set up call command      */
#define SatCmdSndSS         0x11        /* send SS command          */
#define SatCmdSndUSSD       0x12        /* send USSD command        */
#define SatCmdSndSMS        0x13        /* send SMS command         */
#define SatCmdSndDTMF       0x14        /* send DTMF command        */
#define SatCmdLaunchBrowser 0x15        /*Launch Browser */
#define SatCmdPlayTone      0x20        /* play tone command        */
#define SatCmdDispText      0x21        /* display text command     */
#define SatCmdGetKey        0x22        /* get inkey command        */
#define SatCmdGetString     0x23        /* get input command        */
#define SatCmdSelItem       0x24        /* select item command      */
#define SatCmdSetMenu       0x25        /* set up menu command      */
#define SatCmdLocalInf      0x26        /* provide local info       */
#define SatCmdIdleText      0x28        /* setup idle mode text     */
#define SatCmdExecAT        0x34        /* run AT command           */
#ifdef FF_MMI_R99_SAT_LANG
#define SatCmdLangNotify   0x35       /* Language Notification OMAPS00070661 a0393213(R.Prabakar)*/
#endif

/* SPR#1786 - DS - Command types for SAT Class E */
#define SatCmdOpenChannel        0x40        /* Open Channel command           */
#define SatCmdCloseChannel        0x41        /* Close Channel command           */
#define SatCmdReceiveData        0x42        /* Receive Data command           SPR#2508 - DS - Swapped Send and Receive values */
#define SatCmdSendData             0x43        /* Send Data command           */

#define SatCmdEOS           0x81        /* end of proactive session */

                                        /* SAT EVENT FLAGS          */
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
#define MfwSatCmdNone       0x00000000  /* SAT No command			*/
#endif
#define MfwSatRefresh       0x00000001  /* refresh SIM fields       */
#define MfwSatTextOut       0x00000002  /* display string           */
#define MfwSatGetKey        0x00000004  /* get user keystroke       */
#define MfwSatGetString     0x00000008  /* get user input           */
#define MfwSatPlayTone      0x00000010  /* play audio tone          */
#define MfwSatSetupMenu     0x00000020  /* setup toolkit menu       */
#define MfwSatSelectItem    0x00000040  /* select menu item         */
#define MfwSatSendSMS       0x00000080  /* send short message       */
#define MfwSatSendSS        0x00000100  /* send service command     */
#define MfwSatCall          0x00000200  /* setup a call             */
#define MfwSatSendUSSD      0x00000400  /* send USSD                */
#define MfwSatSetEvents     0x00000800  /* setup event list         */
#define MfwSatCcRes         0x00001000  /* call control result      */
#define MfwSatCcAlert       0x00002000  /* call control alerting    */
#define MfwSatIdleText      0x00004000  /* setup idle mode text     */
#define MfwSatExecAT        0x00008000  /* run AT command           */
#define MfwSatSendDTMF      0x00010000  /* send DTMF tones          */
#define MfwSatDataRefreshed 0x00020000  /* Sent when data has already been refreshed */
#if defined(FF_WAP)
#define MfwSatLaunchBrowser 0x00040000 /* Launch Browser command */
#endif
/* SPR#1786 - DS - SAT Class E events */
#define MfwSatOpenChannel       0x00080000 /* Open Channel command */
#define MfwSatCloseChannel      0x00100000 /* Close Channel command */
#define MfwSatSendData           0x00200000 /* Send Data command */
#define MfwSatReceiveData       0x00400000 /* Receive Data command */

#define MfwSatErrBusy		0x00800000 /* SPR#2321 - DS - SAT Busy Error */
#ifdef FF_MMI_R99_SAT_LANG
#define MfwSatLangNotify        0x01000000 /* Language Notification OMAPS00070661 a0393213(R.Prabakar)*/
#define MfwSatLocalInfo          0x02000000 /* Local Information OMAPS00070661 a0393213(R.Prabakar)*/
#endif

#define MfwSatSessionEnd    0x80000000  /* session terminated       */

                                        /* SAT DEVICE IDENTITIES    */
#define SatDevKeypad        0x01        /* keypad device            */
#define SatDevDisplay       0x02        /* display device           */
#define SatDevSpeaker       0x03        /* earpiece device          */
#define SatDevSIM           0x81        /* SIM device               */
#define SatDevME            0x82        /* mobile equipment device  */
#define SatDevNet           0x83        /* network device           */

/* SPR#2321 - DS - SAT Class E Device Identities */
#define SatDevCh1	0x21		/* Channel one */
#define SatDevCh2	0x22		/* Channel two */
#define SatDevCh3	0x23		/* Channel three */
#define SatDevCh4	0x24		/* Channel four */
#define SatDevCh5	0x25		/* Channel five */
#define SatDevCh6	0x26		/* Channel six */
#define SatDevCh7	0x27		/* Channel seven */

                                        /* SAT EVENT TYPES FOR MMI  */
#define SatEvtUserAction             0x04        /* user activity            */
#define SatEvtIdleScreen             0x05        /* idle screen available    */
#ifdef FF_MMI_R99_SAT_LANG
#define SatEvtLangSelection        0x07        /* Language Selection Event OMAPS00070661 a0393213(R.Prabakar)*/
#endif
#if defined(FF_WAP)
#define SatEvtBrowserTermination     0x08        /* Sat Browser termination   */
#endif
                                        /* SAT ERROR CODES          */
#define SatResSuccess       0x00        /* performed successsfully  */
#define SatResPartial       0x01        /* ok,partial comprehension */
#define SatResMissing       0x02        /* ok,missing information   */
#define SatResAdditional    0x03        /* ok,additional EFs read   */
#define SatResNoIcon        0x04        /* ok,no icon displayed     */
#define SatResModified      0x05        /* ok,modified by SIM       */
#define SatResUserAbort     0x10        /* terminated by user       */
#define SatResUserBack      0x11        /* backward move by user    */
#define SatResUserNoResp    0x12        /* no response from user    */
#define SatResUserHelp      0x13        /* help request from user   */
#define SatResUserAbortUSSD 0x14        /* USSD terminated by user  */
#define SatResBusyME        0x20        /* ME temporary failure     */
#define SatResBusyNet       0x21        /* Net temporary failure    */
#define SatResReject        0x22        /* user reject call setup   */
#define SatResCleared       0x23        /* call cleared (user/net)  */
#define SatResTimerConflict 0x24        /* conflict on timer state  */
#define SatResCCTemp        0x25        /* CC by SIM: temp failure  */
#define SatResImpossible    0x30        /* ME cannot do it          */
#define SatResUnknownType   0x31        /* type not understood      */
#define SatResUnknownData   0x32        /* data not understood      */
#define SatResUnknownNumber 0x33        /* number not understood    */
#define SatResSSerror       0x34        /* SS return error          */
#define SatResSMSerror      0x35        /* SMS-RP error             */
#define SatResNoValues      0x36        /* missing required values  */
#define SatResUSSDerror     0x37        /* USSD return error        */
#define SatResMultiCard     0x38        /* Multiple card error      */
#define SatResCCPermanent   0x39        /* CC by SIM: perm. failure */

                                        /* SAT ADDITIONAL INFO      */
#define SatResAiNoCause     0x00        /* no specific cause        */
#define SatResAiBusyScreen  0x01        /* screen is busy           */
#define SatResAiBusyCall    0x02        /* ME busy on call          */
#define SatResAiBusySS      0x03        /* ME busy on SS            */
#define SatResAiNoService   0x04        /* no service available     */
#define SatResAiAccess      0x05        /* access control class bar */
#define SatResAiNoRadio     0x06        /* no radio resource        */
#define SatResAiNoSpeech    0x07        /* not in speech call       */
#define SatResAiBusyUSSD    0x08        /* ME busy on USSD          */
#define SatResAiBusyDTMF    0x09        /* ME busy on send DTMF     */

                                        /* SAT MULTICARD INFO       */
#define SatResAiNoReader    0x01        /* no card reader           */
#define SatResAiNoCard      0x02        /* no card                  */
#define SatResAiBusyReader  0x03        /* card reader busy         */
#define SatResAiCardOff     0x04        /* no card power            */
#define SatResAiPDUError    0x05        /* PDU format error         */
#define SatResAiMuteCard    0x06        /* mute card                */
#define SatResAiXmitError   0x07        /* transmission error       */
#define SatResAiProtocol    0x08        /* unsuppoerted protocol    */
#define SatResAiIllReader   0x09        /* invalid card reader      */

                                        /* SAT LAUNCH BROWSER ADDITIONAL INFO      */
#define SatResAiBearerUnavailable  0x01  /* bearer unavailable           */
#define SatResAiBrowserUnavailable 0x02  /* browser unavailable           */
#define SatResAiUnableToReadData  0x03  /* ME unable to read the provisioning data  */

										 /* SAT BROWSER TERMINATION CAUSE CODE   */

#if defined(FF_WAP)
#define SatBrowserTerminCauseUser 0x00
#define SatBrowserTerminCauseError 0x01
#endif
										/* SAT SETUP EVENT LIST FLAGS  */
#define SatEvtUserActionActivated           0x01        /* user activity            */
#define SatEvtIdleScreenActivated           0x02        /* idle screen available    */
#if defined(FF_WAP)
#define SatEvtBrowserTerminateActivated    0x04        /* Browser terminated    */
#endif
#ifdef FF_MMI_R99_SAT_LANG
#define SatEvtLangSelectActivated           0x08        /* Event language Activated OMAPS00070661 a0393213(R.Prabakar)*/
#endif
                                        /* PROTOTYPES               */

MfwRes satInit (U8 *profile, U8 plen);
MfwRes satExit (void);
MfwHnd satCreate (MfwHnd w, MfwEvt e, MfwCb f, SatCmd *c);
MfwRes satDelete (MfwHnd h);

MfwRes satString (SatCmd *cmd, SatTxt *txt, U8 *res, int rlen);
MfwRes satItem (SatCmd *cmd, U8 itemId, U8 *res, int rlen);
MfwRes satMenuItem (U8 itemId, U8 help);
MfwRes satEvtDownload (U8 evt);
#ifdef FF_MMI_R99_SAT_LANG
MfwRes satLanguageSelected (U8 language); /* Language Selection Event Generation OMAPS00070661 a0393213(R.Prabakar)*/
#endif
MfwRes satDone (SatCmd *cmd, U8 *res, int rlen);
int satEvtRegistered (int evt); /*SPR#2121 - DS - Check if evt has been registered by SAT Setup Event List command*/

void satAccept (void);
void satReject (void);
void satDisc (void);
void Mfw_SAT_DTMF_EndSession(void); // x0021334 : To test SAT session end - CQ33597

void satSignal (U32 evt, void *data);

void rAT_PercentSATI (S16 len, U8 *sc);

/* BEGIN : : Neptune */
#ifndef NEPTUNE_BOARD
void rAT_PercentSATN (S16 len, U8 *sc, T_ACI_SATN_CNTRL_TYPE  cntrl_type);
#else
void rAT_PercentSATN  ( SHORT           len,UBYTE          *satCmd);
#endif
/* END : Neptune*/

#if defined (TI_PS_FF_SATE) || defined (FF_SAT_E) 
void rAT_PercentSATA (S16 cId, S32 rdlTimeout_ms,T_ACI_SATA_ADD *addParm);
#else 
void rAT_PercentSATA (S16 cId, S32 rdlTimeout_ms);
#endif /* TI_PS_FF_SATE */

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
void simReadIconData (U8 record_number);
void sim_img_cnf (USHORT error, UBYTE record_number, UBYTE *efimg);
void sim_img_instance_data_cnf (USHORT error, UBYTE *data_instance);
void iconDataUpdate(SatTxt *t, UBYTE width, UBYTE height, char  *dst);
void convert_21_image(UBYTE width, UBYTE height, UBYTE depth, UBYTE *src, UBYTE *palette, UBYTE *dst);
void convert_11_image(UBYTE width, UBYTE height, char *src, char *dst);
#endif

void rAT_PercentSATE (S16 len, U8 *sc);
BOOL satChangeNotify (int ref, T_SIM_FILE_UPDATE_IND *updateInd);
void satUpdateFiles  ( U8 updated, USHORT dataId );
BOOL satRefreshNotification ( int ref, T_SIM_FILE_UPDATE_IND *updateInd);
void satRefreshAbort(void);
#if defined(FF_WAP)
MfwRes satBrowserTerminated(U8 cause);
#endif
void sate_error(T_ACI_CME_ERR error); /* SPR#2321 - DS - SAT error handling */

// Jun 16, 2005    REF: CRR 31543   x0021334
// Issue: SATK: cannot get TR for send ussd command
// Prototype for new function
T_MFW_SS_RETURN satHangup (T_ACI_CMD_SRC sId);
#endif
