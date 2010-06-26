/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI                                                            
 $Module:		SMS
 $File:		    MmiSmsRead.c
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:
 
    Shared definitions of data elements are made in here
    and accessed via the primitive operations supplied
                        
********************************************************************************

 $History: MmiSmsRead.c

      Apr 09, 2007 OMAPS00124879 x0039928(sumanth)
      Description : I sample crashes while reading the received SMS from the Agilent
      Solution     : Memory is allocated for temp_text.data
      
      Feb 28, 2007 OMAPS00109953 a0393213(R.Prabakar)
      Description : Need to be able to store SMS in SIM/ME
      Solution     : Earlier, index alone was sufficient to identify messages. Now memory should also be taken care of.

      	Oct 18, 2006    DR: OMAPS00099411  x0039928
   	Description: TC2.1.10(BMI/MFW) - English Editor - Reading Chinese message fails
   	Solution: 'Not Supported' string is displayed as Message editor title for a chinese message
   	received when english editor is set.
   	
      	May 05, 2006    DR: OMAPS00071374 x0039928
   	Description: CT-GCF[34.2.5.3]- MS failed to indicate ErrorCause "Protocol error, unspecified" 
   	Solution: Display Info structure is initialized properly to display dialog "Message Deleted".

       Feb 07, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: smsread_r_exec_cb(): avoiding display of sender's address in header for status reports during listing
   			smsread_getstatustext():renamed txtreport to txtreport
   			Replaced MT_STATUS_RPT with MFW_SMS_STATRPT as MT_STATUS_RPT is removed

      	Feb 01, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: Added a check for Status report to display a text "Report:Pending" during listing of this message
			Made changes as per review comments
   	
      	Jan 30, 2006    DR: OMAPS00059261 x0pleela
   	Description: SMS is not forwarding STATUS reports that are read from SIM storage to ACI 
   	Solution: Added code to read and display MT Status report
 	
	Mar 28, 2006    MMI-FIX-47813 x0035544
   	Description: Not Available transient screen is displayed when Extract number option is selected 
   	in messages option screen.
   	Solution: The necessary functionality is provided to extract the numbers in a message and display 
   	them appropriately.
   	
 	May 27, 2005    MMI-FIX-29869 x0018858
   	Description: There is a limitation on the number of messages that can be downloaded from 
   	server even though there is space on the sim.
   	Solution: The limitation has been removed and the number of messages that can be dowloaded
   	has been modified to the masimum number that can be stored.

       March 2, 2005    REF: CRR 11536 x0018858
       Description: Reply path not supported while sending a reply.
       Solution: Added menu option for the reply path.

       Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
  	Issue description:  BMI: using the tool nowSMS the voicemail indication via sms does not work
  	Solution: Instead of identifying voice using addr[0] and pid, we use dcs. Also, messages belonging
  	              to group "1100" are deleted when voicemail icon is displayed.

 	Feb 16, 2005 REF: CRR 28966 x0012851
 	Description: Editing or forwarding of large MT messages (>160 characters) 
 				shouldn't be allowed when Concatenation is set to 'OFF'
 	Solution: If the 'ConcatenateStatus' flag is FALSE, the 'if' check condition
 				which allows the message to be edited or forwarded has been 
 				modified for the same.

	Dec 15,2004 REF: CRR 24396 xkundadu
	Description: Phone crashes when a message is received while the 
			    'List Empty' dialog is displayed.
	Solution: NULL check is added for the list_win handle before calling 
			listsDestroy() when an MT sms is receieved.
 
	 Dec 01, 2004   REF: CRR 24225 xnkulkar
	 Description: Reset when forwarding SMS
	 Solution: The message content is sent excluding the date/time info from the 
	               original message, which was causing the reset as the max limit (161) was crossed.
 	
	 Sep 08, 2004    REF: CRR 24393  Deepa M. D
	 Bug:Phone crashes when user tries to reply to a saved message in the Inbox
	 Fix:The reply and Callnumber Option has been removed for the saved message.
	25/10/00			Original Condat(UK) BMI version.	
	14/10/00			SPR 1257 - merged in issue 888 from b-sample line (extract
						number from SMS improvements) and made changes to accommodate
						Unicode SMSs containing numbers. 
	May 03, 2004    REF: CRR 13845  Deepa M
	The  Forward menuitem added.
 $End

*******************************************************************************/

#define MMI_SMSREAD_C

#define ENTITY_MFW

/* includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "audio.h"

#include "prim.h"


#include "mfw_mfw.h"
#include "mfw_win.h"
#include "mfw_kbd.h"
/* SPR#1428 - SH - New Editor changes */
#ifndef NEW_EDITOR
#include "mfw_edt.h"
#endif
#include "mfw_tim.h"
#include "mfw_phb.h"
#include "ksd.h"
#include "psa.h"
#include "mfw_sms.h"
#include "mfw_smsi.h"
#include "mfw_icn.h"
#include "mfw_mnu.h"
#include "mfw_lng.h"
#include "mfw_sat.h"
#include "mfw_kbd.h"
#include "mfw_nm.h"
#include "mfw_mme.h"

#include "psa_util.h"

#include "dspl.h"

#include "MmiMain.h"
#include "MmiDummy.h"
#include "MmiMmi.h"
#include "mmiCall.h"//GW 29/11/01 for 'callNumber'

#include "MmiDialogs.h"
#include "MmiLists.h"
#include "MmiMenu.h"
#include "MmiSoftKeys.h"
/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
#include "ATBCommon.h"
#include "ATBDisplay.h"
#include "ATBEditor.h"
#include "AUIEditor.h"
#else
#include "MmiEditor.h"
#endif
#include "MmiBookShared.h" // MZ 
#include "mmiSmsMenu.h"
#include "mmismssend.h"
#include "mmiSmsRead.h"
#include "mmiSmsIdle.h"
#include "MmiResources.h"/*MC*/
#include "mfw_ffs.h"

#include "cus_aci.h"

#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


#include "message.h"
#include "prim.h"
#include "aci_cmh.h"	//GW 29/11/01 added for types in aci_fd.h
#include "aci_fd.h"		//GW 29/11/01 added for types in 'cmh.h'
#include "cmh.h" 		//GW 29/11/01 added for types in 'cmh_phb.h'
#include "cmh_phb.h"	//GW 29/11/01 added for cmhPHB_getAdrStr

#include "mmiColours.h"

//GW 29/11/01 - added header file - removed extern void callNumber(UBYTE* number);
MfwWin* getWinData( MfwHnd win);

/*SPR 1453*/
#ifdef LSCREEN
#define UCS2_MENU_TEXT_LEN ((MFW_TAG_LEN +10)*2 +4)/*SPR 2686 only need name+status text*/
#else
#define UCS2_MENU_TEXT_LEN 36 /*mc SPR 1242, no of bytes in latin unicode string which fills one whole line*/ 
#endif

#define MAX_EDITOR_HEADER_LENGTH 25

// Dec 01, 2004   REF: CRR 24225 xnkulkar
// Description: Reset when forwarding SMS
// Added to exclude the original date/time info from forwarded message
#define ORIGINAL_DATE_TIME_INFO 17
//Mar 28, 2006 DR:OMAPS00047813 x0035544
#define MAX_NUMBER_SIZE 10 //Max Size of the number array to hold the extracted numbers
#define MIN_NUM_LENGTH 3 //minimum length of the number to be extracted.

// Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
extern BOOL invalidVm;

extern UBYTE reply_path_bit;
/*********************************************************************

				 DYNAMIC MENU WINDOW. DECLARATION

**********************************************************************/

typedef enum
{
    E_DELETE,
    E_REPLY,
    E_CALL_SENDER,
    E_SAVE_NUMBER,
    E_DATE_TIME,
    E_FORWARD
} E_SMS_OPERATIONS;

// Those information are passed to the sms editor
typedef struct
{
	SHORT          sms_index;
	T_MMI_SMS_STAT sms_type;
	T_MFW_SMS_PARA *sms_info;
	char *assoc_name;
	T_MFW_SMS_MEM sms_mem; /*a0393213 OMAPS00109953 - this stores memory(ME/SIM) where the sms exists*/
}T_SMS_EDITOR_DATA;


/* This variable is used to know in which state we are when MFW callback function is called */
typedef struct
{
	SHORT nb_sms_stored;
	/*SPR 2530, need more info than just indexes*/
	
//    May 27, 2005    MMI-FIX-29869 x0018858
//   	Description: There is a limitation on the number of messages that can be downloaded from 
//   	server even though there is space on the sim.
//   	Solution: The limitation has been removed and the number of messages that can be dowloaded
//   	has been modified to the maximum number that can be stored.
#ifdef FF_MMI_SMS_DYNAMIC
	T_MFW_SMS_MSG* sms_messages; /* store info about the SMS: index... */

//    May 27, 2005    MMI-FIX-29869 x0018858
//   	Changed from static to dynamic
	wstring_t** sms_header; /* header displayed in the dynamic menu */
#else  
	T_MFW_SMS_MSG sms_messages[MAX_MESSAGES]; /* store info about the SMS: index... */
	wstring_t* sms_header[MAX_MESSAGES]; /* header displayed in the dynamic menu */
#endif
}T_SMS_LIST_DATA;

typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    SHORT           id;
    T_MFW_HND       parent;

    /* associated handlers */

    T_MFW_HND       sms_handle;         /* MFW sms handler      */

    /* internal data */
	T_SMS_LIST_DATA sms_list_data;
	T_SMS_EDITOR_DATA sms_editor_data;
	ListMenuData *menu_list_data;
	/*SPR 2686, removed unused data field*/

} T_SMSREAD_R;
T_SMSREAD_R* getUserData_T_SMSREAD_R( T_MFW_WIN *win)
{
	if (win == NULL)
		return (NULL);
	return((T_SMSREAD_R *)win->user);
}


typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    SHORT           id;
    T_MFW_HND       parent;
    T_MFW_HND       editor_win;

    /* associated handlers */


    /* internal data */
	T_SMS_EDITOR_DATA *sms_editor_data;
/*SPR 2686, removed unused data field*/
} T_SMSREAD_R_TEXTEDT2;

T_SMSREAD_R_TEXTEDT2* getUserData_T_SMSREAD_R_TEXTEDT2( T_MFW_WIN *win)
{
	if (win == NULL)
		return (NULL);
	return((T_SMSREAD_R_TEXTEDT2 *)win->user);
}



typedef struct
{
    /* administrative data */

    T_MMI_CONTROL   mmi_control;
    T_MFW_HND       win;
    SHORT           id;
    T_MFW_HND       parent;

    /* associated handlers */

    /* !!! **************************************** modify here **************************************** !!!*/
    T_MFW_HND       kbd;
    T_MFW_HND       kbd_long;
    T_MFW_HND       menu;
    /* !!! **************************************** end modify here **************************************** !!! */

    /* internal data */

    /* !!! **************************************** modify here **************************************** !!!*/
/*SPR 2686, removed unused data field*/
	T_MFW_HND		sms;
    T_MFW_HND       dialog_win;

    /* !!! **************************************** end modify here **************************************** !!! */
	T_SMS_EDITOR_DATA *sms_editor_data;    //NM 25.8
} T_SMSREAD_R_OPT;
T_SMSREAD_R_OPT* getUserData_T_SMSREAD_R_OPT( T_MFW_WIN *win)
{
	if (win == NULL)
		return (NULL);
	return((T_SMSREAD_R_OPT *)win->user);
}





static enum
{
	READ_SMS_LIST,
	READ_SELECTED_SMS
} g_sms_state;	

//MZ 7/02/01 tmp enums.

enum
{
	MFW_MB_LINE1,
	MFW_MB_DATA
};

/*MC SPR2175, removed alphabet conversion tables which are now in ATBCommon.c*/

static const MfwMnuAttr readSMS_menuAttrib =
{
    &readSMS_menuArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    NULL,                               /* with these items         */
    0,                                   /* number of items     */
	COLOUR_LIST_SMS,
	TxtMessages,
	NULL, 
	MNUATTRSPARE

};

//Mar 28, 2006 DR:OMAPS00047813 x0035544
/*default static menu*/
static const MfwMnuAttr menuSearchNumListAttr =
{
	&menuArea,
	MNU_LEFT | MNU_LIST | MNU_CUR_LINE,		/* centered page menu */
	(U8)-1,										/* use default font  */
	NULL,									/* with these items (filled in later) */
	0,										/* number of items (filled in leter)  

*/
	COLOUR_LIST_XX,							/* Colour */
	TxtExtractNumber,								/* Hdr Id */
	NULL,									/* Background */
	MNUATTRSPARE							/* unused */
};


/* x0039928 - Lint warning fix
static SHORT g_sms_list_counter; 
static SHORT g_nb_line_sms; */

static int search_index;	/* SPR888 - SH - keeps track of position within SMS for extract number */

static  T_MFW_HND       info_win=0;
static T_MFW_HND        deleting_win = 0;
static  T_MFW_HND       list_win=0;
static T_MFW_HND		sms_read_win = 0;/*MC 1991 main SMS read window*/
static T_MFW_HND 		message_win = 0; /*MC 2132, individual message display win*/
static T_MFW_HND        option_mnu_win = 0;/*MC 2132, options menu window*/

static char sms_text_ascii[MAX_EDITOR_LEN];

/* we need this variable "read_status" to know which sms message we are going
   to edit (user pressed in the sms menu-> EDIT)
   because SMS_MT & SMS_MO have different size of character 
   (current length saved in "current_sms_len") !!
   SMS_MT includes additional TIME, DATE and NUMBER and we dont want
   to show it when we are editing.
*/
static UBYTE read_status;
static USHORT current_sms_len= 0;

//	Dec 15,2004 REF: CRR 24396 xkundadu
//	Description: Phone crashes when a message is received while the 
//			    'List Empty' dialog is displayed.
//	Solution: NULL check is added for the list_win handle before calling 
//			listsDestroy() function  when an MT sms is receieved.

// This checks whether the 'List Empty' dialog is present or not.
static BOOL SmsListEmptyDlgPresent = 0;

static T_SmsUserData   SmsData;
/*a0393213 OMAPS00109953 - memory passed as parameter*/
int SmsRead_GetStatusText( char* op_text_str, int status, T_MFW_SMS_MEM mem); // RAVI
static T_MFW_HND smsRead_NumberName_info_screen(T_MFW_HND parent_window, T_SmsUserData *UserData,char* name);
UBYTE check_assoc_name(char *name, T_MFW_SMS_ADDR sms_addr);


static T_MFW_HND SmsRead_R_OPT_create (T_MFW_HND parent);
static void SmsRead_R_OPT_destroy (T_MFW_HND window);
static void SmsRead_R_OPT_StandardDialog(T_MFW_HND win, char *text, ULONG duration);
static void SmsRead_R_OPT_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int SmsRead_R_OPT_win_cb (MfwEvt e, MfwWin *w);
static int SmsRead_R_OPT_mfw_cb (MfwEvt event, void *parameter);
static int SmsRead_R_OPT_dialog_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason);
static int SmsRead_R_OPT_kbd_cb (MfwEvt e, MfwKbd *k);
static int SmsRead_R_OPT_mnu_cb (MfwEvt e, MfwMnu *m);
static int SmsRead_R_OPTExeDelete(MfwMnu* m, MfwMnuItem* i);
static int SmsRead_R_OPTExeReply(MfwMnu* m, MfwMnuItem* i);
static int SmsRead_R_OPTExeStoreNumber(MfwMnu* m, MfwMnuItem* i);
void SmsRead_StoreNumber(void);
static int SmsRead_R_OPTExeEdit(MfwMnu* m, MfwMnuItem* i);

/*SPR 2530,, old code which displays the contents of the SMS messages in the SMS list
has been compiled out using the macro ORIGINAL_SMS_LIST.  
If ORGINAL_SMS_LIST is defined, then the old list is shown, otherwise the new kind of SMS
list is shown*/


/*SPR 2530,compiled out unused global vars*/
#ifdef ORIGINAL_SMS_LIST
static SHORT g_sms_list_counter;

static SHORT g_nb_line_sms;
#endif
T_MFW_HND retry_handle;/*handle for the timer to retry reading an SMS. */
int retry_index=0;/*index of message we're trying to read again.*/
/*a0393213 OMAPS00109953 - memory where the message, which we're trying to read again, resides*/
static T_MFW_SMS_MEM retry_mem=MFW_MEM_NOTPRESENT;
/*a0393213 OMAPS00109953 - memory added as parameter*/
void retry_sms_read(int index, T_MFW_SMS_MEM sms_memory);		
int retry_sms_read_tim_cb(MfwEvt event, MfwTim *timer_info);

/* R is the identifier used for the function of the Read menu window */
static T_MFW_HND SmsRead_R_create (T_MFW_HND parent);
static void SmsRead_R_destroy (T_MFW_HND window);
static void SmsRead_R_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int SmsRead_R_win_cb (MfwEvt e, MfwWin *w);
static int SmsRead_R_mfw_cb(MfwEvt event, void *parameter);
static int SmsRead_R_dialog_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason);
static void SmsRead_R_smslist_cb(T_MFW_HND * Parent, ListMenuData * ListData);
static void SmsRead_R_BuildList(T_MFW_HND win,int inUnicode);

// Fill an unicode structure for display
void string_CopyW2W(wstring_t *strDestination, charw_t *strSource, USHORT count );


static T_MFW_HND SmsRead_R_TEXTEDT2_create (T_MFW_HND parent);
static void SmsRead_R_TEXTEDT2_destroy (T_MFW_HND window);
static void SmsRead_R_TEXTEDT2_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter);
static int SmsRead_R_TEXTEDT2_win_cb (MfwEvt e, MfwWin *w);
static void SmsRead_R_TEXTEDT2_editor_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason);
static void SmsRead_R_TEXTEDT2_smslist_cb(T_MFW_HND * Parent, ListMenuData * ListData);



static BOOL Search_forNextNumber(T_SmsUserData *UserData);
//Mar 27, 2006 DR:OMAPS00047813 x0035544
static void SmsRead_numlist_destroy(MfwHnd own_window);
static void SmsRead_numlist_cb(T_MFW_HND Parent, ListMenuData * ListData);
void smsread_numlist_exec_cb (T_MFW_HND win, USHORT event);
static T_MFW_HND SmsRead_BuildNumList_Menu(T_MFW_HND win);
static int smsread_numlist_win_cb (MfwEvt e, MfwWin *w);
void Search_forNumberInSMS(T_SmsUserData *UserData);


//Mar 24, 2006 REF: SPR 47813 x0035544
static int smsread_num_ctr;
static char **num_buffer=NULL;

static  const MfwMnuItem SmsRead_R_OPTItems [] =
{
    {0,0,0,(char *)TxtDelete,0,(MenuFunc)SmsRead_R_OPTExeDelete,item_flag_none},
    {0,0,0,(char *)TxtReply,0,(MenuFunc)SmsRead_R_OPTExeReply,item_flag_none},
    {0,0,0,(char *)TxtEdit,0,(MenuFunc)SmsRead_R_OPTExeEdit,item_flag_none},
    {0,0,0,(char *)TxtForward,0,(MenuFunc)SmsRead_R_OPTExeEdit,item_flag_none},//May 03, 2004    REF: CRR 13845  Deepa M -Menu item to forward SMS
    {0,0,0,(char *)TxtExtractNumber,0,(MenuFunc)SmsRead_R_OPTExeStoreNumber,item_flag_none},
    {0,0,0,(char *)TxtCallNumber,0,(MenuFunc)SmsRead_StoreNumber,item_flag_none}
};

static  const MfwMnuAttr SmsRead_R_OPTAttrib =
{
    &SmsRead_R_OPTArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    (MfwMnuItem*) SmsRead_R_OPTItems,                      /* with these items         */
    sizeof(SmsRead_R_OPTItems)/sizeof(MfwMnuItem) /* number of items     */
};

//Sep 08, 2004    REF: CRR 24393  Deepa M. D
//New menu item for the saved/unsent messages.
static  const MfwMnuItem SmsRead_R_Unsent_OPTItems [] =
{
    {0,0,0,(char *)TxtDelete,0,(MenuFunc)SmsRead_R_OPTExeDelete,item_flag_none},
    {0,0,0,(char *)TxtEdit,0,(MenuFunc)SmsRead_R_OPTExeEdit,item_flag_none},
	{0,0,0,(char *)TxtForward,0,(MenuFunc)SmsRead_R_OPTExeEdit,item_flag_none},
    {0,0,0,(char *)TxtExtractNumber,0,(MenuFunc)SmsRead_R_OPTExeStoreNumber,item_flag_none},
    
};

//Sep 08, 2004    REF: CRR 24393  Deepa M. D
//Menu attributes for the saved/unsent messages.
static  const MfwMnuAttr SmsRead_R_Unsent_OPTAttrib =
{
    &SmsRead_R_OPTArea,
    MNU_LEFT | MNU_LIST | MNU_CUR_LINE, /* centered page menu       */
    (U8)-1,                                 /* use default font         */
    (MfwMnuItem*) SmsRead_R_Unsent_OPTItems,                      /* with these items         */
    sizeof(SmsRead_R_Unsent_OPTItems)/sizeof(MfwMnuItem) /* number of items     */
};



/*******************************************************************************

 $Function:    	charPtrToUBYTEPtr

 $Description:	Converts a char ptr to a UBYTE ptr. The procedures can be replaced by macro 
 				once all conversions are confirmed as valid.
 				
 $Returns:		data set to new type

 $Arguments:	data -data item of input type.
 
*******************************************************************************/
#ifndef MMI_USE_MACROS
UBYTE 	charToUBYTE				( char chr )	{	return((UBYTE)chr);		}
UBYTE* 	charPtrToUBYTEPtr		( char *ptr )	{	return((UBYTE*)ptr);	}
UBYTE 	SHORTToUBYTE			( SHORT data )	{	return((UBYTE)data);	}

char	UBYTEToChar				( UBYTE data)	{	return((char)data);		}
char*	UBYTEPtrToCharPtr		( UBYTE* data)	{	return((char*)data);	}
char*	USHORTPtrToCharPtr	( USHORT* data)	{	return((char*)data);		}
/*a0393213 extra casting done to avoid lint warning*/
USHORT 	charToUSHORT			( char chr )	{	return((USHORT)(int)chr);	}
USHORT	UBYTEToUSHORT			( UBYTE data)	{	return((USHORT)data);	}
USHORT*	charPtrToUSHORTPtr	(char* data)		{	return((USHORT*)data);	}

#else
#define 	charToUBYTE			(UBYTE)
#define 	charPtrToUBYTEPtr	(UBYTE*)
#define 	SHORTToUBYTE		(UBYTE)	

#define		UBYTEPtrToCharPtr	(char*)
#define		UBYTEToChar			(char)
#define		USHORTPtrToCharPtr	(char*)

#define 	charToUSHORT		(USHORT)	
#define 	UBYTEToUSHORT		(USHORT)
#define		charPtrToUSHORTPtr	(USHORT*)
#endif
/*MC SPR2175, removed GSM<->ASCII<->UCS2 conversion functions, moved to ATBCommon*/

/**/

/*******************************************************************************

 $Function:    	SmsRead_R_start

 $Description:	Start the creation of the main window for SMS reading
 
 $Returns:		mfw window handler 

 $Arguments:	parent_window - Parent window handler
 				menuAttr	  - Menu attributes.
 
*******************************************************************************/
T_MFW_HND SmsRead_R_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr)
{
   TRACE_EVENT_P1("sms_read_win:%d", sms_read_win);
    /*SPR2132, if the list window already exists, destroy it before creating a new one*/
	if (sms_read_win != NULL)
		SmsRead_R_destroy(sms_read_win);
	
    TRACE_FUNCTION ("SmsRead_R_start()");
	/*SPR 1991 use ssms_read_win rather than a local var*/
    sms_read_win = SmsRead_R_create (parent_window);

    if (sms_read_win NEQ NULL)
	{
		/* We transmit to the SMS editor the address of the data buffer */
	    SEND_EVENT (sms_read_win, E_INIT, SmsRead_R_ID, (void *)menuAttr);
	}
    return sms_read_win;
}

/*******************************************************************************

 $Function:    	SmsRead_R_create

 $Description:	create the SMS read window (dynamic list of all the SMS)
 
 $Returns:		mfw window handler 

 $Arguments:	parent_window - Parent window handler
 
*******************************************************************************/
static T_MFW_HND SmsRead_R_create(MfwHnd parent_window)
{
    T_SMSREAD_R      * data = (T_SMSREAD_R *)ALLOC_MEMORY (sizeof (T_SMSREAD_R));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("SmsRead_R_create()");

    /*
     * Create window handler
     */

    data->win = 
	win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)SmsRead_R_win_cb);
    if (data->win EQ NULL)
	{
	    return NULL;
	}
    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)SmsRead_R_exec_cb;
    data->mmi_control.data   = data;
    win                      = getWinData(data->win);
    win->user                = (void *)data;
    data->parent             = parent_window;
	winShow(data->win);

    /*
     * return window handle
     */

    return data->win;
}

/*******************************************************************************

 $Function:    	SmsRead_R_destroy

 $Description:	Destroy the SMS read window (dynamic list of all the SMS)
 
 $Returns:		none

 $Arguments:	own_window - window handler
 
*******************************************************************************/

static void SmsRead_R_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_SMSREAD_R     * data;
	USHORT i;

	
    TRACE_FUNCTION ("SmsRead_R_destroy()");

    if (own_window)
	{
	    win_data = getWinData(own_window); //((T_MFW_HDR *))->data;
	    data = getUserData_T_SMSREAD_R(win_data);
	    if (data)
		{
		    /*
		     * Delete WIN handler
		     */ 
			sms_delete(data->sms_handle);

			
			/*SPR 2132, if the message options menu or the messaeg display screen exist, 
			destroy them */
			if (option_mnu_win)
			{	
				SmsRead_R_OPT_destroy(option_mnu_win);
				
			}
			if (message_win)
			{
   	           SmsRead_R_TEXTEDT2_destroy(message_win);
   	           message_win = 0;
			}	
			/*SPR 2132 end*/
			
			if (info_win)
			{
   	           SEND_EVENT(info_win,DIALOG_DESTROY,0,0 );
   	           info_win = 0;
   	        }
			if (list_win){
				
   	           SEND_EVENT(list_win/*SPR1991*/,E_LIST_DESTROY/*SPR 2132*/,0,0 );
   	           list_win = 0;
			}	
		    win_delete (data->win);
		   	{

		    }

		    /*     
		     * Free Memory
		     */

			/* Free the data allocated for the name associated to the phone number */
			if (data->sms_editor_data.assoc_name != NULL)
				FREE_MEMORY( (void *)data->sms_editor_data.assoc_name, strlen(data->sms_editor_data.assoc_name) + 1);
			
			/* Free the data allocated for the dynamic list */
			if (data->menu_list_data != NULL)
			{
				for (i = 0; i < ((data->sms_list_data).nb_sms_stored); i++)
					{
					FREE_MEMORY( (void *)data->sms_list_data.sms_header[i], UCS2_MENU_TEXT_LEN+ 1/*SPR1242*/);
					}
				/*SPR 2686, only de-allocate memory for the number of messages displayed*/
				FREE_MEMORY ((void *)data->menu_list_data->List,((data->sms_list_data).nb_sms_stored) * sizeof(T_MFW_MNU_ITEM));
				FREE_MEMORY ((UBYTE *)data->menu_list_data, sizeof(ListMenuData));
			}
#ifdef FF_MMI_SMS_DYNAMIC
			FREE_MEMORY((void*)data->sms_list_data.sms_messages,g_max_messages * sizeof(T_MFW_SMS_MSG) );
			FREE_MEMORY((void*)data->sms_list_data.sms_header, g_max_messages * sizeof(wstring_t));
#endif
			FREE_MEMORY ((void *)data, sizeof (T_SMSREAD_R));
			sms_read_win = 0;
		}
	    else
		{
		    TRACE_EVENT ("SmsRead_R_destroy() called twice");
		}
	}
}

/*******************************************************************************

 $Function:    	SmsRead_R_exec_cb

 $Description:	Exec callback function of the SMS read window (dynamic list of 
 				all the SMS)
 
 $Returns:		none

 $Arguments:	win - window handler
 				event - mfw event
 				parameter - optional data.
 
*******************************************************************************/
void SmsRead_R_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
    /* callback handler for events sent to to trigger execution */
{
    T_MFW_WIN      * win_data;
    T_SMSREAD_R    * data;
    T_SMS_LIST_DATA * list_data;
	T_MFW_EVENT		MfwEvents;

	T_MFW_SMS_STAT sms_list_type = *(T_MFW_SMS_STAT *)parameter;

	T_MFW_HND l_parent;
	T_DISPLAY_DATA DisplayInfo;
	// Status of SMS reading

	/* x0045876, 14-Aug-2006 (WR - "status" & "i" was declared but never referenced)*/
	/* T_MFW status; */
	  /* int i;*/

	win_data = getWinData(win); //((T_MFW_HDR *))->data;
	data = getUserData_T_SMSREAD_R(win_data);
    list_data = &data->sms_list_data;
	
    TRACE_FUNCTION ("SmsRead_R_exec_cb()");

    switch (event)
	{
	case E_INIT:
		// May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
	       // Memory allocation done here
		if(list_data->sms_messages == NULL)
			list_data->sms_messages = (T_MFW_SMS_MSG*)ALLOC_MEMORY (g_max_messages * sizeof(T_MFW_SMS_MSG)  );
#endif
	    /* initialization of administrative data */
		info_win=0;
	    data->id = value;
		data->menu_list_data = NULL;


		//NM 24.8
		/* Verify that SMS handling has been correctly initialised */
		if (!smsidle_get_ready_state())
		{
			SmsMenu_loadDialogDefault(&DisplayInfo);

            DisplayInfo.TextId	= TxtPleaseWait;
            //DisplayInfo.TextId2  = TxtReady;
			// Generic function for info messages
			DisplayInfo.Callback = (T_VOID_FUNC)SmsRead_R_dialog_cb; 

            info_dialog(win, &DisplayInfo);
	
			break;
		}		
		/* We have to handle E_SMS_MO and E_SMS_MT MFW events, in order to retrieve the SMS */
		MfwEvents = E_SMS_MT  | E_SMS_MO | E_SMS_MT_RECEIVED | E_SMS_MO_AVAIL| 
		            E_SMS_ERR | E_SMS_OK | E_SMS_MEM_FREE    | E_SMS_MEM_FULL; 
		
  		data->sms_handle = sms_create(win,MfwEvents,(MfwCb)SmsRead_R_mfw_cb);


  		/* Get info on the stored messages 
		   According to sms_list_type filter some specific type (new, read...) */
  		/*MC SPR 2530, change sms_index_list to sms_message_list, we need additional info*/
#ifdef FF_MMI_SMS_DYNAMIC
  		list_data->nb_sms_stored = sms_message_list(sms_list_type, list_data->sms_messages, g_max_messages); 
#else
  		list_data->nb_sms_stored = sms_message_list(sms_list_type, list_data->sms_messages, MAX_MESSAGES); 
#endif
  		/* we want to get the whole SMS list */
  		if (list_data->nb_sms_stored > 0)
  		{
  		#ifdef ORIGINAL_SMS_LIST
			if (!info_win)
				info_win = mmi_dialog_information_screen_forever(win,TxtPleaseWait, NULL, COLOUR_STATUS_WAIT);

  			// We want to read all the SMS
  			g_sms_state = READ_SMS_LIST;
			/* global counter to decrement each time a message is read */
			g_sms_list_counter = list_data->nb_sms_stored - 1;

			/* Initialise counter for number of entries in sms list. used to call the correct 
			   no of FreeAllocs MZ 8/03/01  */
			g_nb_line_sms =0;
	
			/* x0045876, 14-Aug-2006 (WR - "status" was declared but never referenced)*/
			/*a0393213 OMAPS00109953 - memory passed as parameter*/
 			sms_read(MFW_SMS_MESSAGE, (unsigned char)(list_data->sms_messages[g_sms_list_counter]).index,(list_data->sms_messages[g_sms_list_counter]).mem);
			
  		#else
  		/*SPR 2530, instead of starting to raed the SMS list in a chain , just copy SMS status info
  		and sender names to list*/
  			int i, txt_len;
  			/*we're assuming the default language is a latin one*/
  			T_MFW_PHB_ENTRY phonebook_entry;
  			BOOL unicode_flag= FALSE;
  			UBYTE output_alphabet= MFW_ASCII;
  			char* name_pointer = NULL;
				
			// May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
		      //  Memory allocation done here
			list_data->sms_header = (wstring_t **) ALLOC_MEMORY (g_max_messages * sizeof(wstring_t)  );
#endif	

			/*for each entry in the message list*/
			for (i=0;i<list_data->nb_sms_stored; i++)
			{	/*allocate memory in the list*/

				if(list_data->sms_header[i] == NULL)
				list_data->sms_header[i] = (wstring_t *) ALLOC_MEMORY (UCS2_MENU_TEXT_LEN + 1);
				/*SPR2530 clear buffer*/
				memset(sms_text_ascii, 0, MAX_EDITOR_LEN);
				memset(list_data->sms_header[i], 0, UCS2_MENU_TEXT_LEN + 1);

				//Use common procedure to display status info.]
				//x0pleela 07 Feb, 2006 DR: OMAPS00059261
				//check for Status report type and send MFW_SMS_STATRPT as status
				if( list_data->sms_messages[i].msg_type  EQ MFW_SMS_STATRPT) 
				{
				  strcpy(sms_text_ascii, MmiRsrcGetText(TxtReport));
				  txt_len = strlen( sms_text_ascii);
				}
				else
				{
				  /*a0393213 OMAPS00109953 - memory passed as parameter*/
				  txt_len = SmsRead_GetStatusText( sms_text_ascii, list_data->sms_messages[i].stat, list_data->sms_messages[i].mem);
				}

				/*if we're in chinese, then all string handling is in Unicode*/
				if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				{	unicode_flag = TRUE;
					output_alphabet= MFW_DCS_UCS2;
				}
				/*if incoming SMS, copy name or number to list*/
				if (list_data->sms_messages[i].stat == MFW_SMS_UNREAD || list_data->sms_messages[i].stat == MFW_SMS_READ)
				{	/*if phonebook entry found*/
					if (bookFindNameInPhonebook( list_data->sms_messages[i].addr.number, &phonebook_entry ))
					{	
						/*if name isn't blank*/
						if (phonebook_entry.name.len >0)
						{	/*if unicode tag we have to make some adjustments*/
							if (phonebook_entry.name.data[0] == 0x80)
							{	name_pointer = (char*)&phonebook_entry.name.data[2];
								phonebook_entry.name.dcs = MFW_DCS_UCS2;
								phonebook_entry.name.len-=2;
								/*SPR2530 moved conversion here*/
								//x0pleela 07 Feb, 2006 DR: OMAPS00059261
								//Donot display sender's address for status report
								if( list_data->sms_messages[i].msg_type  NEQ MFW_SMS_STATRPT) 
								  ATB_convert_String(name_pointer, phonebook_entry.name.dcs&0x0c,  phonebook_entry.name.len,
												  &sms_text_ascii[txt_len],output_alphabet , UCS2_MENU_TEXT_LEN-txt_len, TRUE);
							}
							else
							{	name_pointer = (char*)phonebook_entry.name.data;
							/*copy (and convert) name to buffer*/
							/*SPR2530 explicitly convert from ASCII to current alphabet*/
							//x0pleela 07 Feb, 2006 DR: OMAPS00059261
							//Donot display sender's address for status report
							if( list_data->sms_messages[i].msg_type  NEQ MFW_SMS_STATRPT)
							  ATB_convert_String(name_pointer, MFW_ASCII,  phonebook_entry.name.len,
								&sms_text_ascii[txt_len],output_alphabet , UCS2_MENU_TEXT_LEN-txt_len, TRUE);
						
							}
						}
					}
					else
					{
						//x0pleela 07 Feb, 2006 DR: OMAPS00059261
						//Donot display sender's address for status report
					     if( list_data->sms_messages[i].msg_type  NEQ MFW_SMS_STATRPT)
					       /*copy (and convert) number to buffer*/
						ATB_convert_String(list_data->sms_messages[i].addr.number, MFW_ASCII,  MFW_NUM_LEN,
							&sms_text_ascii[txt_len],output_alphabet ,UCS2_MENU_TEXT_LEN-txt_len, TRUE);
					}
						
				}
				/* use truncation function to copy message header to list*/
				resources_truncate_to_screen_width((char*)sms_text_ascii,0,(char*) list_data->sms_header[i], UCS2_MENU_TEXT_LEN, SCREEN_SIZE_X, unicode_flag);
			}
			//*All the messages have been retrieved --> display the list of SMS*/
				if (Mmi_getCurrentLanguage() != CHINESE_LANGUAGE)
					SmsRead_R_BuildList(win,0);
				else
					SmsRead_R_BuildList(win,1);
		#endif
		}
		else
			SmsRead_DisplayStandardDialog(win, TxtNoMessages, THREE_SECS);
		break;

	case E_RETURN:
		/* Call the recreate function that will create and display a dynamic list from the list of SMS */
	    listDisplayListMenu(win, data->menu_list_data, (ListCbFunc_t)SmsRead_R_smslist_cb,0);
		break;
		
	case E_BACK:
		/* Received an E_BACK event from the dynamic list --> suicide and E_RETURN to parent */
	    l_parent = data->parent;
		SmsRead_R_destroy(win);
		SEND_EVENT(l_parent, E_RETURN, data->id, NULL);		
	
	case E_EXIT: // Terminate exit from sms send...
	// May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
     {
     		int i;
		for (i=0;i<list_data->nb_sms_stored; i++)
			{
				FREE_MEMORY((U8*)list_data->sms_header[i], UCS2_MENU_TEXT_LEN + 1 );
			}
		FREE_MEMORY((U8*)list_data->sms_header, g_max_messages * sizeof(wstring_t) );		
		FREE_MEMORY((U8*)list_data->sms_messages, g_max_messages * sizeof(T_MFW_SMS_MSG));		
	}
#endif
	    listDisplayListMenu(win, data->menu_list_data, (ListCbFunc_t)SmsRead_R_smslist_cb,0);
// 	FREE_MEMORY((U8*)list_data->sms_messages, g_max_messages * sizeof(T_MFW_SMS_MSG));

	    break;
	default:
	    break;
	}
}

/*******************************************************************************

 $Function:    	SmsRead_R_win_cb

 $Description:	Window callback function of the SMS read window (dynamic list of 
 				all the SMS)
 
 $Returns:		none

 $Arguments:	w - mfw window handler
 				e - mfw event
 				
*******************************************************************************/

static int SmsRead_R_win_cb (MfwEvt e, MfwWin *w)    /* yyy window event handler */
{
	TRACE_FUNCTION ("SmsRead_R_win_cb()");
	switch (e)
	{
	case MfwWinVisible:  /* window is visible  */	    
		break;
	case MfwWinFocussed: /* input focus / selected   */	
	case MfwWinDelete:   /* window will be deleted   */        
	default:	    
		return MFW_EVENT_REJECTED;
	}
	return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	SmsRead_GetStatusText

 $Description:	Function to copy status string into array provided. 
 
 $Returns:		number of characters used in the output array.

 $Arguments:	op_text_str - pointer to array to store the string
 				status - read status of text message.

 $History
  SPR#780-GW-Created to allow consistent display of status in text message list 
*******************************************************************************/
int SmsRead_GetStatusText( char* op_text_str, int status, T_MFW_SMS_MEM mem)
{
	int txt_len, txtId;

	TRACE_FUNCTION("SmsRead_GetStatusText()");

	if (status == MFW_SMS_READ)
		txtId = TxtOld;
	else if(status == MFW_SMS_UNREAD)
		txtId = TxtNew;
	else if(status == MFW_SMS_STOR_UNSENT)
		txtId = TxtSaved;
	else if(status == MFW_SMS_STOR_SENT)
		txtId = TxtSend;
	else if(status == MFW_SMS_INVALID)/*SPR 2530*/
		txtId = TxtInvalidMessage;
	else //assume if it is not marked as new/old/sent/unsent
		txtId = TxtNull;
	
	// Check for the READ/UNREAD status of the MT message.
	if (Mmi_getCurrentLanguage() != CHINESE_LANGUAGE)
	{
		strcpy(op_text_str,MmiRsrcGetText(txtId));
		/* Only add colon if a non-empty string and valid message*/
		/*SPR2530 only out in a colon if the message is MT*/
		if (txtId == TxtOld || txtId == TxtNew)
			strcat(op_text_str,":");
		/*a0393213 OMAPS00109953 - '[S]' indicates the message is stored in SIM
		                                        -  '[P]' indicates the message is stored in PHONE*/
		if(mem==MFW_MEM_SIM)
			{
			strcat(op_text_str,"[S]");
			}
		else if(mem==MFW_MEM_ME)
			{
			strcat(op_text_str,"[P]");
			}
			
		txt_len = strlen(op_text_str);
	}
	else //language = Chinese, copy unicode string
	{
		USHORT* unicodeStr; 
		USHORT* txtStr;
		int uIndex;

		unicodeStr = charPtrToUSHORTPtr(op_text_str);

		txtStr = charPtrToUSHORTPtr(MmiRsrcGetText(txtId));

		uIndex = 0;
		txt_len = 0;
		while ((*txtStr!= 0)  && (uIndex*2 <MENU_TEXT_LENGTH))
		{
			unicodeStr[uIndex] = *txtStr;
			txtStr++;
			uIndex++;
			txt_len = txt_len+2; // 2bytes per chinese char
		}
		/*SPR2530 only out in a colon if the message is MT*/
		if ((uIndex*2 <MENU_TEXT_LENGTH) &&(txtId == TxtOld || txtId == TxtNew))
		{
			unicodeStr[uIndex] = ((charToUSHORT)(':'))<<8;
			uIndex++;
			txt_len = txt_len+2; // 2bytes per chinese char
		}
	}
	/* Return number of characters copied into the array  */
	return (txt_len);
}

/*******************************************************************************

 $Function:    	SmsRead_R_mfw_cb

 $Description:	MFW callback function of the SMS read window (dynamic list of all the SMS).
 				Only interesting events for reading are handled here
 
 $Returns:		event status

 $Arguments:	parameter - optional data
 				event - mfw event

*******************************************************************************/
int SmsRead_R_mfw_cb(MfwEvt event, void *parameter)
{
    T_MFW_HND    win  = mfwParent((MfwHdr *)mfw_header());
    T_MFW_WIN  * win_data;
    T_SMSREAD_R      * data;

/* x0045876, 14-Aug-2006 (WR - "list_data", " input_type" & "input_length" was set but never used)*/
#ifdef  ORIGINAL_SMS_LIST
    	T_SMS_LIST_DATA * list_data;
	UBYTE input_type;/*SPR1242, UCS2 or GSM string*/
	int input_length = 0; /*MC, SPR 1453*/
	BOOL unicode = FALSE;/*MC, SPR 1442*/
	int txt_len;
#endif

	T_MFW_SMS_MO * MoMessage;
	T_MFW_SMS_MT * MtMessage;
	
	T_MFW_SMS_IDX msg_info;

	

  	T_DISPLAY_DATA DisplayInfo;
	
	/* x0045876, 14-Aug-2006 (WR - "sms_len" & "sms_text" was declared but never referenced)*/
	/* USHORT sms_len; */
	/* char *sms_text; */
//	int i;                      // RAVI
	

	win_data = getWinData(win); 
	data = getUserData_T_SMSREAD_R(win_data);

/* x0045876, 14-Aug-2006 (WR - "list_data" was set but never used)*/
#ifdef  ORIGINAL_SMS_LIST
	list_data = &(data->sms_list_data);
#endif
	
	TRACE_FUNCTION("SmsRead_R_mfw_cb");

	switch(event)
	{
 		case E_SMS_OK: 
			TRACE_EVENT("SmsRead_R_mfw_cb case:E_SMS_OK ");
			if(deleting_win)
			{
				T_MFW_SMS_STAT  sms_list_type = MFW_SMS_ALL;
				
				listsDestroy(list_win);
				list_win=0;
			    SmsRead_R_destroy(data->win);
				SmsRead_R_start(NULL, (MfwMnuAttr*)&sms_list_type);
   	            SEND_EVENT(deleting_win,DIALOG_DESTROY,0,0 );

				//API - Added line to show if the message was deleted!
				dlg_initDisplayData_TextId( &DisplayInfo, TxtNull, TxtNull, TxtDeleted,  TxtNull, COLOUR_STATUS_WAIT);
				dlg_initDisplayData_events( &DisplayInfo, NULL, TWO_SECS, KEY_HUP );
				info_dialog (0, &DisplayInfo);
    
				deleting_win = 0;
			}

		break;
	case E_SMS_MO:
		TRACE_EVENT("SmsRead_R_mfw_cb case:E_SMS_MO ");
		MoMessage = (T_MFW_SMS_MO *)parameter;
	/*SPR 2530, don't need to craete SMS list here anymore*/
#ifdef ORIGINAL_SMS_LIST
		// We are reading all the SMS on the SIM
		if (g_sms_state EQ READ_SMS_LIST)
		{
			// If no message text, display the address: TBD international format and phonebook
			if (MoMessage->msg_len == 0)
			{

				/* x0045876, 14-Aug-2006 (WR - "sms_len" & "sms_text" was declared but never referenced)*/
				// If there is an associated name, it is displayed (but not stored here, we will have to retrieve it again if detailed view)
				/* sms_text = (MoMessage->dest_addr).number; */
				/* sms_len = strlen(sms_text); */
			}
			else
			{
			
				/* x0045876, 14-Aug-2006 (WR - "sms_len" & "sms_text" was declared but never referenced)*/
				/* sms_text = MoMessage->sms_msg; */
				/* sms_len = MoMessage->msg_len; */
			}

			memset((char*)sms_text_ascii,'\0',UCS2_MENU_TEXT_LEN + 1);

			TRACE_EVENT_P1("MO Message Status = %d", msg_info.stat);
			/*a0393213 OMAPS00109953- memory passed as parameter*/
			sms_copy_idx(MoMessage->msg_ref, &msg_info,MoMessage->memory);

			//GW-SPR#780-Use common procedure to display status info.
			/*a0393213 OMAPS00109953 - memory passed as parameter*/
			txt_len = SmsRead_GetStatusText( sms_text_ascii, msg_info.stat, msg_info.memory);

			/*MC, SPR1242 don't trust the DCS, check for Unicode tag*/
			if (MoMessage->sms_msg[0] == 0x80 ||MoMessage->dcs == MFW_DCS_UCS2 )
			{	input_type = MFW_DCS_UCS2;
				input_length =  MoMessage->msg_len-1;/*MC,SPR 1453, prevent @ symbol on end of string*/
			}
			else
			{	input_type = MFW_DCS_8bits;
				input_length =  MoMessage->msg_len;/*MC, SPR 1453*/
			}

			/*mc end*/
			
			if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
			{	unicode = TRUE;/*MC, SPR 1442, we'll be displaying messages as unicode*/
				/*SPR2175, use new function for conversion*/
				ATB_convert_String(	MoMessage->sms_msg, input_type /*MC SPR1242*/, MoMessage->msg_len,
										&sms_text_ascii[txt_len], MFW_DCS_UCS2,  UCS2_MENU_TEXT_LEN-(txt_len+1), TRUE);

			}
			else
			{	unicode = FALSE;/*MC, SPR 1442, we'll be displaying messages as ASCII*/
				/*SPR2175, use new function for conversion*/
				ATB_convert_String(	MoMessage->sms_msg, input_type /*MC SPR1242*/, input_length/*SPR 1453*/,
										&sms_text_ascii[txt_len] , MFW_ASCII,  MENU_TEXT_LENGTH-(txt_len+1), TRUE);
				
			}


		/* just in case  */
	  if (g_sms_list_counter >= 0)
	  {
				list_data->sms_header[g_sms_list_counter] = (wstring_t *) ALLOC_MEMORY (UCS2_MENU_TEXT_LEN + 1/*MC SPR1242*/);
				/*MC, SPR 1442, use truncation function to copy message header to list*/
				resources_truncate_to_screen_width(sms_text_ascii,0,(char*) list_data->sms_header[g_sms_list_counter], UCS2_MENU_TEXT_LEN, SCREEN_SIZE_X, unicode);
		}
			TRACE_EVENT_P1("g_sms_list_counter = %d", g_sms_list_counter);
			
			g_sms_list_counter--;

	        if (g_sms_list_counter >= 0)
	        {
	        	// There are still messages to read.
	        	/*SPR2260, if reading SMS fails, retry*/
			/*a0393213 OMAPS00109953 - memory passed as parameter*/
	            if (sms_read(MFW_SMS_MESSAGE, SHORTToUBYTE(list_data->sms_messages[g_sms_list_counter].index),(list_data->sms_messages[g_sms_list_counter].mem))== MFW_SMS_FAIL)		  
	            {
					retry_sms_read(SHORTToUBYTE(list_data->sms_messages[g_sms_list_counter].index));
	            }
	            else
	      		g_nb_line_sms++;
	        }
	        else
	        {
	        	// All the messages have been retrieved --> display the list of SMS
				if (Mmi_getCurrentLanguage() != CHINESE_LANGUAGE)
					SmsRead_R_BuildList(win,0);
				else
					SmsRead_R_BuildList(win,1);
			}
		}
		// A particular SMS has been selected. We need to get all the info: full text, time stamp...
		else 
#endif
			if (g_sms_state EQ READ_SELECTED_SMS)

		{
			data->sms_editor_data.sms_info = (T_MFW_SMS_PARA *)ALLOC_MEMORY (sizeof (T_MFW_SMS_PARA));
			data->sms_editor_data.sms_type = MMI_SMS_MO;
		
    		      memcpy(data->sms_editor_data.sms_info, (T_MFW_SMS_PARA *)MoMessage, sizeof(T_MFW_SMS_PARA));
		      SmsRead_R_TEXTEDT2_start(win, (MfwMnuAttr *)&(data->sms_editor_data));
		}
	
		break;				
		case E_SMS_MT:

		MtMessage = (T_MFW_SMS_MT *)parameter;

	/*SPR 2530, don't need to craete SMS list here anymore*/
#ifdef  ORIGINAL_SMS_LIST

		if (g_sms_state EQ READ_SMS_LIST)
		{	
			/*SPR 2176, if message is the last one requested from MFW*/
			if (list_data->sms_messages[g_sms_list_counter].index == MtMessage->index)
			{

			// If no message text, display the address: TBD international format and phonebook
			if (MtMessage->msg_len == 0)
			{
				// If there is an associated name, it is displayed (but not stored here, we will have to retrieve it again if detailed view)
				// MZ check for assoc_name !!! 14/2/01
				/* x0045876, 14-Aug-2006 (WR - "sms_len" & "sms_msg" was declared but never referenced)*/
				/* sms_text = (MtMessage->orig_addr).number; */
				/* sms_len = strlen(sms_text); */
			}
			else
			{
				/* x0045876, 14-Aug-2006 (WR - "sms_len" & "sms_msg" was declared but never referenced)*/
				/* sms_text = MtMessage->sms_msg; */
				/* sms_len = MtMessage->msg_len; */
			}

   			//MZ 24/05/01 clear the temp sms text buffer.
   			memset((char*)sms_text_ascii,'\0',UCS2_MENU_TEXT_LEN + 1);
			/*a0393213 OMAPS00109953 - memory passed as parameter*/
			sms_copy_idx(MtMessage->index, &msg_info,MtMessage->memory);
			//x0pleela 07 Feb, 2006 DR: OMAPS00059261
			//Display "Report" for status report
			//GW-SPR#780-Use common procedure to display status info.
			TRACE_EVENT_P1("Status of message = %d", msg_info.stat);
			if( list_data->sms_messages[i].msg_type  EQ MFW_SMS_STATRPT)
			{
			  strcpy(sms_text_ascii, MmiRsrcGetText(TxtReport));
			  txt_len = strlen( sms_text_ascii);
			}
			else
			  txt_len = SmsRead_GetStatusText( sms_text_ascii, msg_info.stat,msg_info.memory);
	
			/*MCSPR1242 don't trust the DCS, check for Unicode tag*/
			if (MtMessage->sms_msg[0] == 0x80||MtMessage->dcs == MFW_DCS_UCS2 )
			{	input_type = MFW_DCS_UCS2;/*MC*/
				input_length =  MtMessage->msg_len-1;/*1498, use MT length*/
			}
			else
			{	input_type = MFW_DCS_8bits;/*MC*/
				input_length =  MtMessage->msg_len;/*1498, use MT length*/
			}
			
			/*mc end*/
			if (Mmi_getCurrentLanguage() != CHINESE_LANGUAGE)
			{	unicode = FALSE; /*MC, SPR 1442, we'll be displaying messages as ASCII*/
				/*SPR2175, use new function for conversion*/
				//x0pleela 07 Feb, 2006 DR: OMAPS00059261
				//Donot display sender's address for status report
				if( list_data->sms_messages[i].msg_type  NEQ MFW_SMS_STATRPT)
				ATB_convert_String(	MtMessage->sms_msg, input_type/*MC SPR1242*/, input_length/*SPR 1453*/,
										&sms_text_ascii[txt_len], MFW_ASCII /*MC SPR1242*/,  MENU_TEXT_LENGTH-(txt_len+1)/*MC SPR1242*/, TRUE);
			}
			else //language = Chinese
			{	unicode = TRUE;/*MC, SPR 1442, we'll be displaying messages as unicode*/
				/*SPR2175, use new function for conversion*/
				//x0pleela 07 Feb, 2006 DR: OMAPS00059261
				//Donot display sender's address for status report
				if( list_data->sms_messages[i].msg_type  NEQ MFW_SMS_STATRPT)
				ATB_convert_String(	MtMessage->sms_msg, input_type /*SH*/, MtMessage->msg_len,
											&sms_text_ascii[txt_len], MFW_DCS_UCS2,  UCS2_MENU_TEXT_LEN-txt_len/*MC SPR1242*/, TRUE);
			}
			list_data->sms_header[g_sms_list_counter] = (wstring_t *) ALLOC_MEMORY (UCS2_MENU_TEXT_LEN + 1/*MC SPR1242*/);

			//Check for allocation fail before using memory
		    if(list_data->sms_header[g_sms_list_counter]  != NULL)
		    {	/*MC, SPR 1442, use truncation function to copy message header to list*/
		    	resources_truncate_to_screen_width(sms_text_ascii,0,(char*) list_data->sms_header[g_sms_list_counter], UCS2_MENU_TEXT_LEN, SCREEN_SIZE_X, unicode);
				
			}
			else
				TRACE_EVENT(" Memory allocation failed ");
				
			g_sms_list_counter--;

	        if (g_sms_list_counter >= 0)
	        {
	        	// There are  still messages to read.
	        	/*SPR2260, if reading SMS fails, retry*/
	        	if (sms_read(MFW_SMS_MESSAGE, (UBYTE)list_data->sms_messages[g_sms_list_counter].index) == MFW_SMS_FAIL)
	        	{
					retry_sms_read(SHORTToUBYTE(list_data->sms_messages[g_sms_list_counter].index));
	            }
	        	g_nb_line_sms++;
	         }
	        else
	        {
	        	// All the messages have been retrieved --> display the list of SMS
				if (Mmi_getCurrentLanguage() != CHINESE_LANGUAGE)
					SmsRead_R_BuildList(win,0);
				else
					SmsRead_R_BuildList(win,1);
	        }
		  }
		  else 
		  {	/*if index is not the one expected, pass event on to SmsIdle module*/
		  	return MFW_EVENT_PASSED;
		  }
		  
		}
		else 
#endif
		if (g_sms_state EQ READ_SELECTED_SMS)
		{
			TRACE_EVENT_P1("MtMessage len: %d", MtMessage->msg_len);
			/*SPR 2176, if this was the message we requested*/
			/*a0393213 OMAPS00109953 - mem is also taken into consideration*/
			if (MtMessage->index == data->sms_editor_data.sms_index
			    && MtMessage->memory == data->sms_editor_data.sms_mem 
				)
			{
				data->sms_editor_data.sms_info = (T_MFW_SMS_PARA *)ALLOC_MEMORY (sizeof (T_MFW_SMS_PARA));
				
				data->sms_editor_data.sms_type = MMI_SMS_MT;
			    memcpy(data->sms_editor_data.sms_info, (T_MFW_SMS_PARA *)MtMessage, sizeof(T_MFW_SMS_PARA));
				/*a0393213 OMAPS00109953 - memory passed as parameter*/
				sms_copy_idx(MtMessage->index, &msg_info,MtMessage->memory);
			
				
				SmsRead_R_TEXTEDT2_start(win, (MfwMnuAttr *)&(data->sms_editor_data));
			}
			else
			{	/*if index is not the one expected, pass event on to SmsIdle module*/
				return MFW_EVENT_PASSED;
			}
		}
		break;
    case E_SMS_ERR:
      TRACE_EVENT("SMS Error");
      if (info_win){
        SEND_EVENT(info_win,DIALOG_DESTROY,0,0 );
        info_win = 0;
      }
#ifndef    ORIGINAL_SMS_LIST
      /*SPR 2530 Delete and recreate SMS list window to show updates*/
		{ T_MFW_SMS_STAT  sms_list_type = MFW_SMS_ALL;

			listsDestroy(list_win);
			list_win=0;
			SmsRead_R_destroy(sms_read_win);
			SmsRead_R_start(NULL, (MfwMnuAttr*)&sms_list_type);
      	}
#endif
	break;
		/*SPR 2530 handle new message event*/
	case E_SMS_MT_RECEIVED:
		/*if we are not currently displaying a message*/
		// Feb 16, 2005 REF: CRR MMI-SPR-27900 xnkulkar 
		// Added condition ' invalidVm == FALSE' before updating sms list
		// to ensure that no blank voice mail sms entry is shown. 
		if (message_win == NULL &&  invalidVm == FALSE) 
 		/*SPR 2530 Delete and recreate SMS list window to show updates*/
		{ T_MFW_SMS_STAT  sms_list_type = MFW_SMS_ALL;

//	Dec 15,2004 REF: CRR 24396 xkundadu
//	Description: Phone crashes when a message is received while the 
//			    'List Empty' dialog is displayed.
//	Solution: NULL check is added for the list_win handle before calling 
//			listsDestroy() function  when an MT sms is receieved.

			// Check whether the sms list window is already destroyed or not.
			TRACE_EVENT(" MT SMS RECEIVED ");
			if(list_win != NULL)
			{
				listsDestroy(list_win);
				list_win=0;
			}

			// If the 'List empty' dialog is being displayed,
			// wait for 3 seconds and continue.
			if(SmsListEmptyDlgPresent)
			{
				vsi_t_sleep (VSI_CALLER 3000);
			}
			SmsRead_R_destroy(sms_read_win);
			SmsRead_R_start(NULL, (MfwMnuAttr*)&sms_list_type);
      	}
		/*pass event to next handler*/
		return MFW_EVENT_REJECTED;
//	break;    // RAVI
    	/*SPR 2530 end*/
    default:
    		TRACE_EVENT(" SmsRead_R_mfw_cb case: Default");
    	return MFW_EVENT_REJECTED;
		}
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	retry_sms_read.  Added for SPR2260

 $Description:	set up a timer to re-read sms at passed index in 100ms
 
 $Returns:		none

 $Arguments:	index of sms to be re-read
 				
*******************************************************************************/
/*a0393213 OMAPS00109953 - sms_memory parameter added - It indicates where (SIM/ME) the 
message which is to be read resides*/
void retry_sms_read(int index, T_MFW_SMS_MEM sms_memory)
{	TRACE_EVENT("retry_sms_read()");
	retry_handle = tim_create(NULL, 1000, (MfwCb)retry_sms_read_tim_cb);
	tim_start(retry_handle);
	retry_index = index;
	retry_mem = sms_memory;/*a0393213 OMAPS00109953 - update the global variable*/
}
/*******************************************************************************

 $Function:    	retry_sms_read_tim_cb.  Added for SPR2260

 $Description:	Callback function for retry SMS read timer
 
 $Returns:		status int

 $Arguments:	event, timer data structure
 				
*******************************************************************************/
int retry_sms_read_tim_cb(MfwEvt event, MfwTim *timer_info)
{	TRACE_EVENT("retry_sms_read_tim_cb()");
	/*a0393213 OMAPS00109953 - retry to read the message*/
	if (sms_read(MFW_SMS_MESSAGE, retry_index, retry_mem) == MFW_SMS_FAIL)
	{	tim_start(retry_handle);}
	else
	{	tim_delete(retry_handle);
		retry_handle = NULL;
	}
	return MFW_EVENT_CONSUMED;

}
/*******************************************************************************

 $Function:    	SmsRead_R_smslist_cb

 $Description:	Callback function for the dynamic list of the SMS read window.
 
 $Returns:		none

 $Arguments:	Parent - window handler
 				ListData - Menu list data.
 				
*******************************************************************************/

void SmsRead_R_smslist_cb(T_MFW_HND * Parent, ListMenuData * ListData)
{



    T_MFW_WIN       * win_data;
    T_SMSREAD_R * data;    
	  
//	T_MFW_HND      * l_parent;   // RAVI
 //   SHORT            l_id;   // RAVI


	TRACE_FUNCTION("SmsRead_R_smslist_cb ");
	
	win_data = getWinData(Parent); 
	data = getUserData_T_SMSREAD_R(win_data);

	// In all the case, we need the index of the selected message
	data->sms_editor_data.sms_index = data->sms_list_data.sms_messages[ListData->ListPosition].index;
	/*a0393213 OMAPS00109953 - memory is also taken into account*/
	data->sms_editor_data.sms_mem = data->sms_list_data.sms_messages[ListData->ListPosition].mem;
	
	if (ListData->Reason EQ LISTS_REASON_SELECT/* && ListData->KeyReason EQ KCD_LEFT*/)
	{
		TRACE_EVENT(">>> Reason:LISTS_REASON_SELECT ");
		// We want only to retrieve the selected SMS
		g_sms_state = READ_SELECTED_SMS;

		if (!info_win)
			info_win = mmi_dialog_information_screen_forever(Parent,TxtPleaseWait, NULL, COLOUR_STATUS_WAIT);

		/*NM p030 just in case not to using the '0' index for reading */
		if (data->sms_editor_data.sms_index NEQ 0)
   		{	/*SPR 2530, if read fails, retry later*/
	   		/*a0393213 OMAPS00109953 - memory passed as parameter*/
   			if (sms_read(MFW_SMS_MESSAGE, SHORTToUBYTE(data->sms_editor_data.sms_index ),data->sms_editor_data.sms_mem)== MFW_SMS_FAIL)
	            {
		            /*a0393213 OMAPS00109953 - memory passed as parameter*/
				retry_sms_read(SHORTToUBYTE(data->sms_editor_data.sms_index ),data->sms_editor_data.sms_mem);						
	            }		
		}
	}

	/*NM p032 check if the TEST SIM is inserted.
	  During testing FTA 34.2.3 we would like to delete
	  the sms from the list without changing the sms-status
	  from unread to read
	*/
	if (test_sim EQ TRUE)
	{

	 if (ListData->Reason EQ LISTS_REASON_HANGUP)
	{
		TRACE_EVENT(">>> Reason: LISTS_REASON_HANGUP ");
		listsDestroy(ListData->win);
		list_win=0;
	    SmsRead_R_destroy(data->win);
	}
	/*
	the right-soft-key  deletes
	the selected sms in the list-menu without changing
	the read-status of the sms.

	go-back key is now on the "Hang_up"
	*/
	if ((ListData->Reason EQ LISTS_REASON_BACK )
    || (ListData->Reason EQ LISTS_REASON_CLEAR ))
	{
		T_DISPLAY_DATA DisplayInfo;
		
		TRACE_EVENT(">>> Reason: LISTS_REASON_BACK/CLEAR ");
		
		/* show for a short period  "Message deleted" and go
		   back to the previous menu-screen
		*/
		/*a0393213 OMAPS00109953*/
		if((sms_msg_delete((UBYTE)data->sms_editor_data.sms_index,data->sms_editor_data.sms_mem) EQ MFW_SMS_OK)	
		   && (data->sms_editor_data.sms_index NEQ 0))
		{

			// May 05, 2006    DR: OMAPS00071374 x0039928
			// Fix : DisplayInfo structure is initialized properly.
			dlg_initDisplayData_TextId( &DisplayInfo, TxtNull, TxtNull, TxtMessage, TxtDeleted, COLOUR_STATUS);
			dlg_initDisplayData_events( &DisplayInfo, (T_VOID_FUNC)NULL, THREE_SECS, KEY_CLEAR|KEY_LEFT|KEY_RIGHT);
			info_dialog(data->win, &DisplayInfo);

		}
	
		/* destroy the read-sms-menu and go back to 
		the previous screen
		*/
		listsDestroy(ListData->win);
		list_win=0;
	    SmsRead_R_destroy(data->win);

		}

	}
	else
	{
	 if ((ListData->Reason EQ LISTS_REASON_BACK /*&& ListData->KeyReason EQ KCD_RIGHT*/)
    || (ListData->Reason EQ LISTS_REASON_CLEAR /*&& ListData->KeyReason EQ KCD_HUP */)
    || (ListData->Reason EQ LISTS_REASON_HANGUP))	// sbh - added hangup key
	{
		TRACE_EVENT(">>> Reason: LISTS_REASON_BACK/CLEAR ");
		listsDestroy(ListData->win);
		list_win=0;
	    SmsRead_R_destroy(data->win);
	}

	
	}
	/*NM p032 end*/
	
}

/*******************************************************************************

 $Function:    	SmsRead_R_dialog_cb

 $Description:	Dialog callback function of the SMS read window (dynamic list of all the SMS)
 				Case where SMS has not been initialized yet
 
 $Returns:		status

 $Arguments:	win - current window handler
 				Identifier - window id
 				Reason - Event cause.
 				
*******************************************************************************/

int SmsRead_R_dialog_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason)
{
    T_MFW_WIN       * win_data;
    T_SMSREAD_R * data;    
	T_MFW_HND *lparent;

	win_data = getWinData(win); 
	data = getUserData_T_SMSREAD_R(win_data);
	lparent = data->parent;
	
    TRACE_FUNCTION ("SmsRead_dialog_cb()");
	
	switch (Reason)
	{
	case INFO_TIMEOUT:
	case INFO_KCD_HUP:
		// Commit suicide
		SmsRead_R_destroy(win);
		// Send E_RETURN event to parent window
		SEND_EVENT(lparent, E_RETURN, data->id, NULL);

//	Dec 15,2004 REF: CRR 24396 xkundadu
//	Description: Phone crashes when a message is received while the 
//			    'List Empty' dialog is displayed.

	// set the variable to FALSE, since  'List empty' dialog has been destroyed 		.
		SmsListEmptyDlgPresent = FALSE;
		return MFW_EVENT_CONSUMED;

	default:
		return MFW_EVENT_REJECTED;
	}
}

/*******************************************************************************

 $Function:    	SmsRead_R_BuildList

 $Description:	Build the list of SMS for the dynamic list.
 
 $Returns:		status

 $Arguments:	win - current window handler
 
*******************************************************************************/

void SmsRead_R_BuildList(T_MFW_HND win, int inUnicode)
{

    T_MFW_WIN       * win_data;
    T_SMSREAD_R * data;    

    /* List is complete so present menu. */
    USHORT i;

    TRACE_FUNCTION("SmsRead_R_BuildList");
	
	win_data = getWinData(win); 
	data = getUserData_T_SMSREAD_R(win_data);
	
	data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));

	if(data->menu_list_data == 0)
	{
		TRACE_EVENT("Failed memory alloc 1 ");
		return;
	}
		/*SPR 2686, only allocate memory for the number of messages displayed*/	
	data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( ((data->sms_list_data).nb_sms_stored) * sizeof(T_MFW_MNU_ITEM) );//JVJE

	if(data->menu_list_data->List == 0)
	{
		TRACE_EVENT("Failed memory alloc 2");
		return;
	}
		
	
	for (i = 0; i < ((data->sms_list_data).nb_sms_stored); i++)
	{
		mnuInitDataItem(&data->menu_list_data->List[i]);
		data->menu_list_data->List[i].str  = (char *)(data->sms_list_data).sms_header[i];
		data->menu_list_data->List[i].flagFunc = item_flag_none; 
	}
	
   	data->menu_list_data->ListLength =(data->sms_list_data).nb_sms_stored;
	data->menu_list_data->ListPosition = 1;
	data->menu_list_data->CursorPosition = 1;
	data->menu_list_data->SnapshotSize =(data->sms_list_data).nb_sms_stored;
	data->menu_list_data->Font = 0;
	data->menu_list_data->LeftSoftKey = TxtSoftSelect;
	/*NM p032*/
	if(test_sim EQ TRUE)
		data->menu_list_data->RightSoftKey = TxtDelete;
	else
		data->menu_list_data->RightSoftKey = TxtSoftBack;
		
	data->menu_list_data->KeyEvents = KEY_ALL;
	data->menu_list_data->Reason = 0;
	data->menu_list_data->Strings = TRUE;
 	data->menu_list_data->Attr   = (MfwMnuAttr*)&readSMS_menuAttrib;
	data->menu_list_data->autoDestroy    = FALSE;
 	
    /* Create the dynamic menu window */
listDisplayListMenu(win, data->menu_list_data, (ListCbFunc_t)SmsRead_R_smslist_cb,inUnicode);
list_win = data->menu_list_data->win;
	if (info_win){
          SEND_EVENT(info_win,DIALOG_DESTROY,0,0 );
          info_win = 0;
	}
}



/*******************************************************************************

 $Function:    	SmsRead_R_OPTExeDelete

 $Description:	Called when the user press Delete in the Option menu
 			
 $Returns:		event status.

 $Arguments:	m - pointer to menu.
 				i - index if selected menu item.
*******************************************************************************/

static int SmsRead_R_OPTExeDelete(MfwMnu* m, MfwMnuItem* item)
{
    T_MFW_HND     	   win      = mfwParent(mfw_header());
    T_MFW_WIN        * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSREAD_R_OPT  * data     = (T_SMSREAD_R_OPT *)win_data->user;

	/* access to data from parent   */
    T_MFW_HDR    * parent      =  data->parent;
    T_MFW_WIN    * win_data2   =  ((T_MFW_HDR *)parent)->data;
    
    T_SMSREAD_R_TEXTEDT2  * parent_data =  (T_SMSREAD_R_TEXTEDT2 *)win_data2->user;
//    May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
//   	Changed from static to dynamic
	T_MFW_SMS_IDX* temp_message_list;/*SPR2530*/
#else
	T_MFW_SMS_IDX temp_message_list[MAX_MESSAGES];/*SPR2530*/
#endif
	int no_of_messages, i;/*SPR2530*/
	BOOL message_currently_in_list = FALSE;/*SPR2530*/
	UBYTE deleting_index = parent_data->sms_editor_data->sms_index;/*SPR2686*/
	/*a0393213 OMAPS00109953*/
	T_MFW_SMS_MEM deleting_mem = parent_data->sms_editor_data->sms_mem;
#ifdef FF_MMI_SMS_DYNAMIC
	temp_message_list = (T_MFW_SMS_IDX*)ALLOC_MEMORY(g_max_messages*sizeof(T_MFW_SMS_IDX));	  
#endif
	TRACE_FUNCTION("SmsRead_R_OPTExeDelete");
#ifdef MMI_LITE
	/*SPR 2686 destroy option window to reduce memory usage*/
		if (mfwCheckMemoryLeft() < 400)
			SmsRead_R_OPT_destroy(option_mnu_win);
#endif	
	/*SPR2530, check if message index in current list of messages*/
#ifdef FF_MMI_SMS_DYNAMIC
	no_of_messages = sms_index_list(MFW_SMS_ALL, temp_message_list, g_max_messages);
#else
	no_of_messages = sms_index_list(MFW_SMS_ALL, temp_message_list, MAX_MESSAGES);
#endif
	for (i= 0; i< no_of_messages;i++)
	{
		/*a0393213 OMAPS00109953*/
		if (temp_message_list[i].index == deleting_index && temp_message_list[i].memory== deleting_mem)
			message_currently_in_list =  TRUE;
	}

	if (message_currently_in_list == TRUE)
	{
		/*a0393213 OMAPS00109953*/
		if(sms_msg_delete((UBYTE)deleting_index/*SPR 2686*/,deleting_mem)== MFW_SMS_OK)
		{
	       /* set the status that the user selected the delete item */
			if(deleting_win == 0)
				deleting_win= mmi_dialog_information_screen_forever(0,TxtPleaseWait, NULL, COLOUR_STATUS_WAIT);

			{
		   		// Destroy the text editor and free memory.
		   		SmsRead_R_TEXTEDT2_destroy(parent_data->win);
		   		/*SPR 2686, only destroy option menu window if it hasn't already been destroyed*/
		   		if (option_mnu_win)
		   				SmsRead_R_OPT_destroy(option_mnu_win);
		   		
			}

		}
		else
		{		   		
			
			/*SPR2530, don't delete option and display windows, just show failure*/
			mmi_dialog_information_screen(0,TxtFailed, NULL, NULL, COLOUR_STATUS_WAIT);
		}
	}
	else
	{
		/*SPR2530, don't delete option and display windows, just show failure*/
		mmi_dialog_information_screen(0,TxtFailed, NULL, NULL, COLOUR_STATUS_WAIT);
	}
   
//    May 27, 2005    MMI-FIX-29869 x0018858
#ifdef FF_MMI_SMS_DYNAMIC
//   Free the allocated memory
   FREE_MEMORY((U8*)temp_message_list, g_max_messages*sizeof(T_MFW_SMS_IDX));
#endif
   return 1;   // ADDED BY RAVI - 28-11-2005
}

/*******************************************************************************

 $Function:    	SmsRead_R_OPTExeReply

 $Description:	Called when the user press Reply in the Option menu
 			
 $Returns:		event status.

 $Arguments:	m - pointer to menu.
 				i - index if selected menu item.
*******************************************************************************/

static int SmsRead_R_OPTExeReply(MfwMnu* m, MfwMnuItem* i)
{

    T_MFW_HND       win  = mfwParent(mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSREAD_R_OPT         * data = (T_SMSREAD_R_OPT *)win_data->user;	
	
	T_MFW_HDR    * parent      =  data->parent;
    T_MFW_WIN    * win_data2   =  ((T_MFW_HDR *)parent)->data;
    
    T_SMSREAD_R_TEXTEDT2  * parent_data =  (T_SMSREAD_R_TEXTEDT2 *)win_data2->user;
    // March 2, 2005    REF: CRR 11536 x0018858
   //  T_MFW_SMS_INFO	sms_parameter;//Added for retreiving the Reply path. // RAVI
	

	TRACE_FUNCTION("SmsRead_R_OPTExeReply");
		
	    /* Copy the number and name information to the SMS buffer
	    */
	 /*MC SPR 1257, clear whole buffer in case of Unicode entry*/
    memset(SmsData.TextBuffer, '\0', MAX_MSG_LEN_ARRAY);
		//Copy the calling number for pasting into the number buffer;
	/* Marcus: Issue 1804: 12/03/2003: Start */
	if(parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.ton == MFW_TON_INTERNATIONAL)
	{
		if(parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.number[0] == '+')
		{
			strcpy((char *)SmsData.NumberBuffer,(char *)parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.number); 
		}
		else
		{
			strcpy((char *)SmsData.NumberBuffer,"+");				
			strncat((char *)SmsData.NumberBuffer,(char *)parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.number,sizeof(SmsData.NumberBuffer)-2); 
		}
	}
	else
		strcpy((char *)SmsData.NumberBuffer,(char *)parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.number); 
	/* Marcus: Issue 1804: 12/03/2003: End */

              // March 2, 2005    REF: CRR 11536 x0018858
	      // Retreive and check the reply bit from the structure.
	      // see if we should use our own service centre number
		if ( reply_path_bit && parent_data->sms_editor_data->sms_info->sms_mt.rp &&
			strlen(parent_data->sms_editor_data->sms_info->sms_mt.sc_addr) != 0 )
		{
			TRACE_EVENT_P1("The receivers SCA is %s",parent_data->sms_editor_data->sms_info->sms_mt.sc_addr);
			SmsData.called_from_reply_item = TRUE;
	  	  	/* copy the service center number as well */
			strcpy((char *)SmsData.CentreBuffer, (char *)parent_data->sms_editor_data->sms_info->sms_mt.sc_addr);
		}
		else
		{
			TRACE_EVENT_P1("The senders SCA is %s",SmsData.NumberBuffer);
			/* use the own Service Center Number if the caller doesnt send one*/
			SmsData.called_from_reply_item = FALSE;
		}

	/* p304 SH save window pointer */
		win = parent_data->win;
		// Destroy the text editor, free memory
	    SmsRead_R_TEXTEDT2_destroy(parent_data->win);
		/* Destroy the Options Menu and text editor.before starting the Send Editor.*/
		SmsRead_R_OPT_destroy(data->win);

		/*SPR 2686 destroy the SMS read window to free memory*/
		/*destroy the built list window*/
		listsDestroy(list_win);
		list_win=0;
		/*destroy the main control window*/
		if (sms_read_win)/*SPR 2132, check window exists before destroying it*/
		{
			SmsRead_R_destroy(sms_read_win);
			sms_read_win = NULL;
		}
		/* NM,  FTA 34.2.8  */
		
		/* p304 SH - call this after destroying editor & windows. parent_data is lost,
		so use pointer saved from above */
		SmsSend_SEND_start(win, (void *) &SmsData);
    

    
	
    return 1;
}

/*******************************************************************************

 $Function:    	SmsRead_R_OPTExeStoreNumber

 $Description:	Called when the user press Store Number in the Option menu
 			
 $Returns:		event status.

 $Arguments:	m - pointer to menu.
 				i - index if selected menu item.
*******************************************************************************/
static int SmsRead_R_OPTExeStoreNumber(MfwMnu* m, MfwMnuItem* i)
{
    T_MFW_HND       win  = mfwParent(mfw_header());
    T_MFW_HND       win2; //Mar 27, 2006 DR:OMAPS00047813 x0035544
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSREAD_R_OPT         * data = (T_SMSREAD_R_OPT *)win_data->user;
	T_MFW_HDR    * parent      =  data->parent;
    T_MFW_WIN    * win_data2   =  ((T_MFW_HDR *)parent)->data;
    T_SMSREAD_R_TEXTEDT2  * parent_data =  (T_SMSREAD_R_TEXTEDT2 *)win_data2->user;
	// UBYTE found_name;			/*SPR888 -SH*/   // RAVI
	//Mar 27, 2006 DR:OMAPS00047813 x0035544
	 CHAR  number[MFW_NUM_LEN];
	T_MFW_PHB_TON ton;
	
	TRACE_FUNCTION("SmsRead_R_OPTExeStoreNumber");

	 /* Copy the number and name information to the SMS buffer
    */
    //Mar 27, 2006 DR:OMAPS00047813 x0035544
	ton = parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.ton;
   	strcpy(number, parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.number);
    	
	/*SPR 2686 destroy windows before attempting to extract numbers.*/
	// Destroy the text editor, rel memory. 
    SmsRead_R_TEXTEDT2_destroy(parent_data->win);
	/* Destroy the Options Menu and text editor. */
	SmsRead_R_OPT_destroy(data->win);
	
	//Mar 27, 2006 DR:OMAPS00047813 x0035544	
	if(ton == MFW_TON_INTERNATIONAL &&
		number[0]!='+' && strlen((char *)number)>0 )
		/* SPR888 - SH - do not add + if already there */
	{
		strcpy((char *)SmsData.NumberBuffer,"+");
		strcat((char *)SmsData.NumberBuffer,(char *)number); 		
	}
	else
	{
		strcpy((char *)SmsData.NumberBuffer,(char *)number); 
	}	
	
	if (read_status EQ MMI_SMS_MT)
	{
		/* MT -SMS has the additional information (Date, Time & Number)
		   we dont want to show these when we are editing       */
    	memcpy(SmsData.TextBuffer,&sms_text_ascii[current_sms_len],MAX_MSG_LEN_ARRAY);
	}
	else
	{
		//when we were reading the saved-SMS with the length of MAX_MSG_LEN
  		memcpy(SmsData.TextBuffer,sms_text_ascii,MAX_MSG_LEN_ARRAY);/* assumption: sms_text_ascii is still filled from reading the SMS*/
	}
	
	/* SPR888 - SH - Start looking for numbers from beginning of SMS text */
#if 0 //Mar 24, 2006 DR:OMAPS00047813 x0035544
	search_index = 0;
	if (strlen((char *)SmsData.NumberBuffer)==0)
	{
		Search_forNextNumber(&SmsData);
	}

	if (strlen((char *)SmsData.NumberBuffer)==0)
	{
		mmi_dialog_information_screen(0, TxtNotAvailable, NULL, NULL, NULL);
	}
	else
	{
		//Search the phonebook for the sender number entry and display the tag and number.
		if (bookFindNameInPhonebook((const char*)&SmsData.NumberBuffer[0], &pb_data) !=0)
		{
#ifdef NO_ASCIIZ			
			//Display the tag and number dialog.
			/* SPR888 - SH - parent_data->parent changed to parent_data->win */
			smsRead_NumberName_info_screen(0,(void *)&SmsData, (char*)pb_data.name.data);
#else
			//Display the tag and number dialog.
			/* SPR888 - SH - parent_data->parent changed to parent_data->win */
			smsRead_NumberName_info_screen(0,(void *)&SmsData, (char*)pb_data.name);
#endif

		}
		else
		{
			/* SPR888 - SH - parent_data->parent changed to parent_data->win */
			smsRead_NumberName_info_screen(0,(void *)&SmsData,NULL);
		}
	}
#endif //Mar 24, 2006 DR:OMAPS00047813 x0035544

//Mar 24, 2006 DR:OMAPS00047813 x0035544
Search_forNumberInSMS(&SmsData);
win2 = SmsRead_BuildNumList_Menu( win);
if (win2 NEQ NULL)
{
  SEND_EVENT (win2, E_INIT, 0, 0);
}

   	
    return 1;
}

/*******************************************************************************

 $Function:    	SmsRead_R_OPTExeEdit

 $Description:	Called when the user press Forward in the Option menu
 			
 $Returns:		event status.

 $Arguments:	m - pointer to menu.
 				i - index if selected menu item.
*******************************************************************************/

static int SmsRead_R_OPTExeEdit(MfwMnu* m, MfwMnuItem* i)
{
	T_MFW_HND       win  = mfwParent(mfw_header());
    	T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    	T_SMSREAD_R_OPT         * data = (T_SMSREAD_R_OPT *)win_data->user;

	T_MFW_HDR    * parent      =  data->parent;
    	T_MFW_WIN    * win_data2   =  ((T_MFW_HDR *)parent)->data;
    	T_SMSREAD_R_TEXTEDT2  * parent_data =  (T_SMSREAD_R_TEXTEDT2 *)win_data2->user;
	T_DISPLAY_DATA Dialog;

	TRACE_FUNCTION("SmsRead_R_OPTExeEdit");
#ifdef TI_PS_FF_CONC_SMS	
	if (FFS_flashData.ConcatenateStatus == TRUE )
	{
		 /* Copy the number and name information to the SMS buffer
	    	*/
	    	SmsData.NumberBuffer[0] = '\0';
		if (read_status EQ MMI_SMS_MT)
		{
			/* MT -SMS has the additional information (Date, Time & Number)
			   we dont want to show these when we are editing       */
			   
//	       	Dec 01, 2004   REF: CRR 24225 xnkulkar
//	 		Description: Reset when forwarding SMS
//	 		Solution: The message content is sent excluding the date/time info from the 
//	               	original message, which was causing the reset as the max limit was crossed.
	    		memcpy(SmsData.TextBuffer,&sms_text_ascii[current_sms_len + ORIGINAL_DATE_TIME_INFO],MAX_MSG_LEN_ARRAY);
		}
		else
		{
			//when we were reading the saved-SMS with the length of MAX_MSG_LEN
	  		memcpy(SmsData.TextBuffer,sms_text_ascii,MAX_MSG_LEN_ARRAY);/* assumption: sms_text_ascii is still filled from reading the SMS*/
		}
		SmsData.called_from_reply_item = FALSE;
		
		/* p304 SH save window pointer */
		win = parent_data->win;
		// Destroy the text editor, free memory
	    	SmsRead_R_TEXTEDT2_destroy(parent_data->win);
		/* Destroy the Options Menu and text editor.before starting the Send Editor.*/
		SmsRead_R_OPT_destroy(data->win);
		/*SPR 1991 destroy the SMS read window to free memory*/
		/*destroy the built list window*/
		listsDestroy(list_win);
		list_win=0;
		/*destroy the main control window*/
		if (sms_read_win)/*SPR 2132, check window exists before destroying it*/
		{
			SmsRead_R_destroy(sms_read_win);
			sms_read_win = NULL;
		}
				
   	     	/*SPR 1991 end*/
		/* NM,  FTA 34.2.8  */
		
		/* p304 SH - call this after destroying editor & windows. parent_data is lost,
		so use pointer saved from above */
		SmsSend_SEND_start(win, (void *) &SmsData);
	}
	/* API - 28-01-03 - 1628 - Allow the editing of a single SMS if ConcatenateStatus is FALSE */
	/* API - 19-03-03 - 1825 - Allow the editing of a single saved SMS if ConcatenateStatus is FLASE*/

//	Feb 16, 2005 REF: CRR 28966 x0012851
//	Description: Editing or forwarding of large MT messages (>160 characters) 
// 				shouldn't be allowed when Concatenation is set to 'OFF'
// 	Solution: If the 'ConcatenateStatus' flag is FALSE, the 'if' check condition
//				which allows the message to be edited or forwarded has been 
// 				modified for the same.
	else if (FALSE==FFS_flashData.ConcatenateStatus)
#endif /* TI_PS_FF_CONC_SMS*/

	{
		if (((read_status EQ MMI_SMS_MT) && (parent_data->sms_editor_data->sms_info->sms_mt.msg_len <= BASIC_MAX_MSG_LEN))
			||((read_status EQ MMI_SMS_MO) && (parent_data->sms_editor_data->sms_info->sms_mo.msg_len <= BASIC_MAX_MSG_LEN)))
		{
			TRACE_EVENT_P1("msg_len in edit is = %d", parent_data->sms_editor_data->sms_info->sms_mt.msg_len);
			/* Copy the number and name information to the SMS buffer
	    		*/
	    		SmsData.NumberBuffer[0] = '\0';
			if (read_status EQ MMI_SMS_MT)
			{
			/* MT -SMS has the additional information (Date, Time & Number)
			   we dont want to show these when we are editing       */
//	       	Dec 01, 2004   REF: CRR 24225 xnkulkar
//	 		Description: Reset when forwarding SMS
//	 		Solution: The message content is sent excluding the date/time info from the 
//	               		    original message, which was causing the reset as the max limit was crossed.
	    		memcpy(SmsData.TextBuffer,&sms_text_ascii[current_sms_len + ORIGINAL_DATE_TIME_INFO],MAX_MSG_LEN_SGL);
		}
		else
		{
			//when we were reading the saved-SMS with the length of MAX_MSG_LEN
	  		memcpy(SmsData.TextBuffer,sms_text_ascii,MAX_MSG_LEN_SGL);/* assumption: sms_text_ascii is still filled from reading the SMS*/
		}
		SmsData.called_from_reply_item = FALSE;

		win = parent_data->win;
		// Destroy the text editor, free memory
	    	SmsRead_R_TEXTEDT2_destroy(parent_data->win);
		/* Destroy the Options Menu and text editor.before starting the Send Editor.*/
		SmsRead_R_OPT_destroy(data->win);
		/*SPR 1991 destroy the SMS read window to free memory*/
		/*destroy the built list window*/
		listsDestroy(list_win);
		list_win=0;
		/*destroy the main control window*/
		if (sms_read_win)/*SPR 2132, check window exists before destroying it*/
		{
			SmsRead_R_destroy(sms_read_win);
			sms_read_win = NULL;
		}
		/*SPR 1991 end*/		
		SmsSend_SEND_start(win, (void *) &SmsData);
	}
	/* API - 28-01-03 - 1628 - END */
	else
	{
	/* Initialise the dialog control block with default information
	*/
	dlg_initDisplayData_TextId( &Dialog, TxtNull, TxtNull, TxtNotAllowed, TxtNull , COLOUR_STATUS_WAIT);
	dlg_initDisplayData_events( &Dialog, (T_VOID_FUNC)NULL, 3000, KEY_CLEAR );

	/* Show the dialog
	*/
	info_dialog( win, &Dialog );
	
	}
    }
    return 1;
}

/*******************************************************************************

 $Function:    	SmsRead_R_READSMS_OPT_start

 $Description:	Start the creation of the Option Window from the Read SMS
  				Text window.
 			
 $Returns:		mfw window handler

 $Arguments:	parent_window- pointer to menu.
 				menuAttr - menu attributes.
*******************************************************************************/

T_MFW_HND SmsRead_R_READSMS_OPT_start(T_MFW_HND parent_window, MfwMnuAttr *menuAttr)
{
   /*SPR 2132, setting global var to keep track of options window*/

    TRACE_FUNCTION ("SmsRead_R_OPT_start()");

    option_mnu_win = SmsRead_R_OPT_create (parent_window);

    if (option_mnu_win  NEQ NULL)
	{
	    SEND_EVENT (option_mnu_win , E_INIT, SmsRead_R_READSMS_OPT_ID, (void *)menuAttr);
	}
    return option_mnu_win;
}

/*SPR 2132, removed obsolete function*/
/*******************************************************************************

 $Function:    	SmsRead_R_OPT_create

 $Description:	Create the Options window
 			
 $Returns:		window handler

 $Arguments:	parent_window- pointer to parent window
*******************************************************************************/

static T_MFW_HND SmsRead_R_OPT_create(MfwHnd parent_window)
{
    T_SMSREAD_R_OPT      * data = (T_SMSREAD_R_OPT *)ALLOC_MEMORY (sizeof (T_SMSREAD_R_OPT));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("SmsRead_R_OPT_create()");

    /*
     * Create window handler
     */

    data->win = 
	win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)SmsRead_R_OPT_win_cb);
    if (data->win EQ NULL)
	{
	    return NULL;
	}
    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)SmsRead_R_OPT_exec_cb;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (void *)data;
    data->parent             = parent_window;

    /*
     * return window handle
     */
	
    return data->win;
}

/*******************************************************************************

 $Function:    	SmsRead_R_OPT_destroy

 $Description:	Destroy the Option window
 			
 $Returns:		none

 $Arguments:	own_window- current window
*******************************************************************************/

static void SmsRead_R_OPT_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_SMSREAD_R_OPT     * data;

    TRACE_FUNCTION ("SmsRead_R_OPT_destroy()");

    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data = (T_SMSREAD_R_OPT *)win_data->user;

	    if (data)
		{	
		    /*
		     * Delete WIN handler
		     */ 
		     win_delete (data->win);

		    /*     
		     * Free Memory
		     */
		    FREE_MEMORY ((void *)data, sizeof (T_SMSREAD_R_OPT));
		    option_mnu_win =0;/*SPR2132, make sure window pointer is reset*/
		}
	    else
		{
		    TRACE_EVENT ("SmsRead_R_OPT_destroy() called twice");
		}
	}
}

/*******************************************************************************

 $Function:    	SmsRead_R_OPT_exec_cb

 $Description:	Exec callback function for the Option window
 			
 $Returns:		none

 $Arguments:	win- current window
 				event - window event Id
 				value - unique Id
 				parameter- optional data.
*******************************************************************************/

void SmsRead_R_OPT_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
    /* callback handler for events sent to to trigger execution */
{
    T_MFW_WIN        * win_data = ((T_MFW_HDR *) win)->data;
    T_SMSREAD_R_OPT  * data     = (T_SMSREAD_R_OPT *)win_data->user;
    T_MFW_HND      * l_parent = NULL;  // RAVI
    SHORT            l_id = 0;  // RAVI

	T_MFW_EVENT         MfwEvents;
	
    TRACE_FUNCTION ("SmsRead_R_OPT_exec_cb()");


    switch (event)
	{
	case E_INIT:
		TRACE_EVENT(">> SmsRead_R_OPT_exec_cb(), E_INIT ");
		/* initialization of administrative data */
		data->id = value;
		
		if(value == SmsRead_R_READSMS_OPT_ID)
		{
			data->kbd =      kbdCreate(data->win,KEY_ALL,         (MfwCb)SmsRead_R_OPT_kbd_cb);
			data->kbd_long = kbdCreate(data->win,KEY_ALL|KEY_LONG,(MfwCb)SmsRead_R_OPT_kbd_cb);
			//Sep 08, 2004    REF: CRR 24393  Deepa M. D
			//If the message is a saved/unsent message, then provide a different menu with the Reply option removed.
			if(read_status EQ MMI_SMS_MO)
			{
				data->menu =     mnuCreate(data->win,(MfwMnuAttr *)&SmsRead_R_Unsent_OPTAttrib, E_MNU_ESCAPE, (MfwCb)SmsRead_R_OPT_mnu_cb);
			}
			else
			{
				data->menu =     mnuCreate(data->win,(MfwMnuAttr *)&SmsRead_R_OPTAttrib, E_MNU_ESCAPE, (MfwCb)SmsRead_R_OPT_mnu_cb);
			}
			mnuLang(data->menu,mainMmiLng);
			
			/* put the (new) dialog window on top of the window stack */
			mnuUnhide(data->menu);
			winShow(win);
			break;
		}	
		else
		{
			// Catch the events concerning delete
			MfwEvents = E_SMS_ERR | E_SMS_OK;
			
			data->sms = sms_create(data->win,MfwEvents,(MfwCb)SmsRead_R_OPT_mfw_cb);
			data->sms_editor_data = (T_SMS_EDITOR_DATA *)parameter;
		}
	case E_RETURN:
		/* create the dialog handler */
		data->kbd =      kbdCreate(data->win,KEY_ALL,         (MfwCb)SmsRead_R_OPT_kbd_cb);
		data->kbd_long = kbdCreate(data->win,KEY_ALL|KEY_LONG,(MfwCb)SmsRead_R_OPT_kbd_cb);
		//Sep 08, 2004    REF: CRR 24393  Deepa M. D
		//If the message is a saved/unsent message, then provide a different menu with the Reply option removed.
		if(read_status EQ MMI_SMS_MO)
		{
			data->menu =     mnuCreate(data->win,(MfwMnuAttr *)&SmsRead_R_Unsent_OPTAttrib, E_MNU_ESCAPE, (MfwCb)SmsRead_R_OPT_mnu_cb);
		}
		else
		{
			data->menu =     mnuCreate(data->win,(MfwMnuAttr *)&SmsRead_R_OPTAttrib, E_MNU_ESCAPE, (MfwCb)SmsRead_R_OPT_mnu_cb);
		}
		mnuLang(data->menu,mainMmiLng);
		
		/* put the (new) dialog window on top of the window stack */
		mnuUnhide(data->menu);
   	    winShow(win);
		break;
	    
	case E_BACK:
	    l_parent = data->parent;
		l_id = data->id;
	    SmsRead_R_OPT_destroy(data->win);
	    SEND_EVENT (l_parent, E_RETURN, l_id, NULL); /* forward event to parent */
	
	case E_ABORT:
	case E_EXIT:
	    SmsRead_R_OPT_destroy(data->win);
	    SEND_EVENT (l_parent, event, l_id, NULL); /* forward event to parent */
	    break;
	default:
	    return;
	}
}

/*******************************************************************************

 $Function:    	SmsRead_R_OPT_mfw_cb

 $Description:	Exec callback function for the DEL_ALL_ACK window
 			
 $Returns:		status

 $Arguments:	event - window event Id
 				parameter- optional data.
*******************************************************************************/

int SmsRead_R_OPT_mfw_cb (MfwEvt event, void *parameter)
{
    T_MFW_HND          win       = mfwParent((MfwHdr *)mfw_header());
    T_MFW_WIN        * win_data  = (T_MFW_WIN *)((T_MFW_HDR *)win)->data;
    T_SMSREAD_R_OPT  * data      = (T_SMSREAD_R_OPT *)win_data->user;
//	T_MFW_EVENT        MfwEvents;   // RAVI


    TRACE_FUNCTION ("SmsRead_R_OPT_mfw_cb()");

    switch (event)
	{

	case E_SMS_ERR:

		winDelete(data->dialog_win);

    	/* Display "All SMS deleted" and after timeout go back to parent */
		SmsRead_R_OPT_StandardDialog(win, "SMS not deleted", THREE_SECS); // TBD remplacer par ID

		break;

	case E_SMS_OK: 

		/* go now back to the parent             */

     	/* destroy the dialog at first           */
		winDelete(data->dialog_win);

    	/* Display "All SMS deleted" and after timeout go back to parent */
		SmsRead_R_OPT_StandardDialog(win, "SMS deleted", THREE_SECS); // TBD remplacer par ID
		break;
	}
	return 1; // ADDED BY RAVI - 28-11-2005
}	

/*******************************************************************************

 $Function:    	SmsRead_R_OPT_mnu_cb

 $Description:	Menu callback function for the Option window
 			
 $Returns:		status

 $Arguments:	e - mfw event Id
 				m - menu handle
*******************************************************************************/

static int SmsRead_R_OPT_mnu_cb (MfwEvt e, MfwMnu *m)
    /* SmsRead_R_OPT menu event handler */
{
    T_MFW_HND    win  = mfwParent(mfw_header());
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSREAD_R_OPT      * data = (T_SMSREAD_R_OPT *)win_data->user;
    T_MFW_HND  * l_parent;

    /* x0045876, 14-Aug-2006 (WR - "l_id" was set but never used)*/
    /* SHORT        l_id; */

    TRACE_FUNCTION ("SmsRead_R_OPT_mnu_cb()");

    switch (e)
        {
	case E_MNU_ESCAPE: /* back to previous menu */
	    l_parent = data->parent;

	    /* x0045876, 14-Aug-2006 (WR - "l_id" was set but never used)*/
	    /* l_id = data->id; */
	    SmsRead_R_OPT_destroy(data->win);
	    SEND_EVENT (l_parent, E_RETURN, data->id, NULL);
	    break;
	default: /* in mnuCreate() only E_MNU_ESCAPE has been enabled! */
	    return MFW_EVENT_REJECTED;
        }
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	SmsRead_R_OPT_win_cb

 $Description:	Window callback function for the Option window
 			
 $Returns:		status

 $Arguments:	e - mfw event Id
 				w - window handler
*******************************************************************************/

static int SmsRead_R_OPT_win_cb (MfwEvt e, MfwWin *w)
    /* SmsRead_R_OPT window event handler */
{
    TRACE_FUNCTION ("SmsRead_R_OPT_win_cb()");

    switch (e)
	{
        case MfwWinVisible:  /* window is visible  */
	    displaySoftKeys(TxtSoftSelect,TxtSoftBack);
	    break;
	case MfwWinFocussed: /* input focus / selected   */
	case MfwWinDelete:   /* window will be deleted   */
        default:
	    return MFW_EVENT_REJECTED;
	}
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	SmsRead_R_OPT_kbd_cb

 $Description:	Exec callback function for the Option window
 			
 $Returns:		status

 $Arguments:	e - keyboard event Id
 				k - keyboard data 
*******************************************************************************/

static int SmsRead_R_OPT_kbd_cb (MfwEvt e, MfwKbd *k)
    /* SmsRead_R_OPT keyboard event handler */
{
    T_MFW_HND    win  = mfwParent(mfw_header());
    T_MFW_WIN  * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSREAD_R_OPT      * data = (T_SMSREAD_R_OPT *)win_data->user;

    TRACE_FUNCTION ("SmsRead_R_OPT_kbd_cb()");

    if (e & KEY_LONG)
	{
	    switch (k->code)
		{
		case KCD_HUP: /* back to previous menu */
		    mnuEscape(data->menu);
		    break;
		case KCD_RIGHT: /* Power Down */
		    return MFW_EVENT_REJECTED; /* handled by idle */
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
    else
	{
	    switch (k->code)
		{
		case KCD_MNUUP: /* highlight previous entry */
		    mnuUp(data->menu);
		    break;
		case KCD_MNUDOWN: /* highlight next entry */
		    mnuDown(data->menu);
		    break;
		case KCD_MNUSELECT:
		case KCD_LEFT: /* activate this entry */
		    mnuSelect(data->menu);
		    break;
		case KCD_HUP: /* back to previous menu */
		case KCD_RIGHT: /* back to previous menu */
		    mnuEscape(data->menu);
		    break;
		default: /* no response to all other keys */
		    return MFW_EVENT_CONSUMED;
		}
	}
    return MFW_EVENT_CONSUMED;
}

/*******************************************************************************

 $Function:    	SmsRead_R_OPT_dialog_cb

 $Description:	Dialog callback function of the SMS read window (dynamic list 
 				of all the SMS). Case where SMS has not been initialized yet.
 			
 $Returns:		status

 $Arguments:	win - window handler.
 				Identifier -  unique identifier.
 				Reason - cause of event.
*******************************************************************************/

int SmsRead_R_OPT_dialog_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason)
{
	T_MFW_WIN *win_data = ((T_MFW_HDR *)win)->data;
	T_SMSREAD_R_OPT *data = (T_SMSREAD_R_OPT *)win_data->user;
	T_MFW_HND *lparent = data->parent;
	
    TRACE_FUNCTION ("SmsRead_dialog_cb()");
	
	switch (Reason)
	{
	case INFO_TIMEOUT:
	case INFO_KCD_HUP:
		// Commit suicide
		SmsRead_R_OPT_destroy(win);
		// Send E_RETURN event to parent window
		SEND_EVENT(lparent, E_RETURN, data->id, NULL);
		return MFW_EVENT_CONSUMED;

	default:
		return MFW_EVENT_REJECTED;
	}
}

/*******************************************************************************

 $Function:    	SmsRead_DisplayStandardDialog

 $Description:	Display a standard dialog with no soft key and 3 seconds of display
 			
 $Returns:		none

 $Arguments:	win - window handler.
 				text -  text string.
 				duration - seconds
*******************************************************************************/

void SmsRead_DisplayStandardDialog(T_MFW_HND win, int TextId, ULONG duration)
{
	T_DISPLAY_DATA DisplayInfo;
	
	SmsMenu_loadDialogDefault(&DisplayInfo);

       DisplayInfo.TextId = TextId;
	DisplayInfo.Time = duration;
	DisplayInfo.Callback = (T_VOID_FUNC)SmsRead_R_dialog_cb; 

//	Dec 15,2004 REF: CRR 24396 xkundadu
//	Description: Phone crashes when a message is received while the 
//			    'List Empty' dialog is displayed.

	// set the variable to TRUE since  'List empty' dialog is going to display. 
	SmsListEmptyDlgPresent = TRUE;
   	 info_dialog(win, &DisplayInfo);
}

/*******************************************************************************

 $Function:    	SmsRead_R_OPT_StandardDialog

 $Description:	Display a standard dialog with no soft key and 3 seconds of display
 			
 $Returns:		none

 $Arguments:	win - window handler.
 				text -  text string.
 				duration - seconds
*******************************************************************************/

void SmsRead_R_OPT_StandardDialog(T_MFW_HND win, char *text, ULONG duration)
{
	T_MFW_WIN *win_data = ((T_MFW_HDR *)win)->data;
	T_SMSREAD_R_OPT *data = (T_SMSREAD_R_OPT *)win_data->user;

	T_DISPLAY_DATA DisplayInfo;
	
	SmsMenu_loadDialogDefault(&DisplayInfo);

    DisplayInfo.TextId = 0;
	DisplayInfo.Time = duration;
    DisplayInfo.TextString = text;
	DisplayInfo.Callback = (T_VOID_FUNC)SmsRead_R_OPT_dialog_cb; //TBD

    data->dialog_win = info_dialog(win, &DisplayInfo);
}


/*SPR2175, removed unused function*/


/*******************************************************************************

 $Function:    	string_CopyW2W

 $Description:	
 			
 $Returns:		none

 $Arguments:	strDestination - destination string
 				strSource -  source string
 				count -number of character.
*******************************************************************************/

void string_CopyW2W(wstring_t *strDestination, charw_t *strSource, USHORT count )
	// Copy characters of an ascii string to a wide counted string
{
	charw_t *pout = strDestination;
	while(count!=0)
	{
		*pout++ = *strSource++;
		count--;
	}
}

/*******************************************************************************

 $Function:    	SmsRead_R_TEXTEDT2_start

 $Description:	Start the creation of the SMS editor window (display of the text 
 				of one SMS)
 
 $Returns:		win - window handler.

 $Arguments:	parent_window - current window handler
 				data - Menu attributes.
 
*******************************************************************************/

T_MFW_HND SmsRead_R_TEXTEDT2_start(T_MFW_HND parent_window, MfwMnuAttr *data)
{
   /*SPR 2132, use global var*/
    TRACE_FUNCTION ("SmsRead_R_TEXTEDT2_start()");

    message_win = SmsRead_R_TEXTEDT2_create (parent_window);

    if (message_win NEQ NULL)
	{
	    SEND_EVENT (message_win, E_INIT, SmsRead_R_TEXTEDT2_ID, (void *)data);
	}
    return message_win;
}

/*******************************************************************************

 $Function:    	SmsRead_R_TEXTEDT2_create

 $Description:	Create the SMS editor window (display of the text of one SMS)
 
 $Returns:		window handler.

 $Arguments:	parent_window - current window handler
 				
*******************************************************************************/

static T_MFW_HND SmsRead_R_TEXTEDT2_create(MfwHnd parent_window)
{
    T_SMSREAD_R_TEXTEDT2      * data = (T_SMSREAD_R_TEXTEDT2 *)ALLOC_MEMORY (sizeof (T_SMSREAD_R_TEXTEDT2));
    T_MFW_WIN  * win;

    TRACE_FUNCTION ("SmsRead_R_TEXTEDT2_create()");

    /*
     * Create window handler
     */
	
    data->win = 
	win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)SmsRead_R_TEXTEDT2_win_cb);
    if (data->win EQ NULL)
	{
	    return NULL;
	}
    /* 
     * connect the dialog data to the MFW-window
     */

    data->mmi_control.dialog = (T_DIALOG_FUNC)SmsRead_R_TEXTEDT2_exec_cb;
    data->mmi_control.data   = data;
    win                      = ((T_MFW_HDR *)data->win)->data;
    win->user                = (void *)data;
    data->parent             = parent_window;

    /*
     * return window handle
     */
	winShow(data->win);
    return data->win;
}

/*******************************************************************************

 $Function:    	SmsRead_R_TEXTEDT2_destroy

 $Description:	Destroy the SMS editor window (display of the text of one SMS)
 
 $Returns:		none

 $Arguments:	own_window - current window handler
 				
*******************************************************************************/

static void SmsRead_R_TEXTEDT2_destroy(MfwHnd own_window)
{
    T_MFW_WIN * win_data;
    T_SMSREAD_R_TEXTEDT2     * data;

//	USHORT i;   // RAVI

    TRACE_FUNCTION ("SmsRead_R_TEXTEDT2_destroy()");

    if (own_window)
	{
	    win_data = ((T_MFW_HDR *)own_window)->data;
	    data = (T_SMSREAD_R_TEXTEDT2 *)win_data->user;

	    if (data)
		{
		    /*
		     * Delete WIN handler
		     */ 

			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_Destroy(data->editor_win);
#else /* NEW_EDITOR */
			editor_destroy(data->editor_win);
#endif /* NEW_EDITOR */

		    win_delete (data->win);

			
	        
			FREE_MEMORY ((void *)data->sms_editor_data->sms_info, sizeof (T_MFW_SMS_PARA));

		    FREE_MEMORY ((void *)data, sizeof (T_SMSREAD_R_TEXTEDT2));
		
		message_win = 0; /*SPR2132, make sure window pointer is reset*/
		}
	    else
		{
		    TRACE_EVENT ("SmsRead_R_TEXTEDT2_destroy() called twice");
		}
	}
}

/*******************************************************************************

 $Function:    	SmsRead_R_TEXT_editor_cb

 $Description:	Callback function for the text editor
 
 $Returns:		none

 $Arguments:	win - current window handler
 				Identifier -Unique identifier.
 				reason - event 
*******************************************************************************/

static void SmsRead_R_TEXT_editor_cb (T_MFW_HND win, USHORT Identifier, SHORT reason)
{
	T_MFW_PHB_ENTRY  pb_data;

	T_MFW_WIN   * win_data = ((T_MFW_HDR *) win)->data;
	T_SMSREAD_R_TEXTEDT2      * data     = (T_SMSREAD_R_TEXTEDT2 *)win_data->user;

	/* x0045876, 14-Aug-2006 (WR - "parent" was declared but never referenced)*/
	/* T_MFW_HDR    * parent      =  data->parent; */

	/* x0045876, 14-Aug-2006 (WR - "win_data2" was declared but never referenced)*/
    	/* T_MFW_WIN    * win_data2   =  ((T_MFW_HDR *)parent)->data; */
	
//    T_SMSREAD_R_TEXTEDT2  * parent_data =  (T_SMSREAD_R_TEXTEDT2 *)win_data2->user;  // RAVI

 //   T_MFW_SMS_STAT  sms_list_type = MFW_SMS_ALL;  // RAVI


	TRACE_FUNCTION ("SmsRead_R_TEXT_editor_cb()");

	switch (reason)
	{
	case INFO_KCD_LEFT:
		//x0pleela 08 Feb, 2006 DR: OMAPS00059261
		//Replacing MT_STATUS_RPT with MFW_SMS_STATRPT as MT_STATUS_RPT is removed
		//x0pleela 30 Jan, 2006 DR: OMAPS00059261
		//Displaying different options screen for normal MT text message. 
		//whereas, for staus report message, the message is deleted, since the option is "Delete" in LSK
		if( data->sms_editor_data->sms_info->sms_mt.sms_msg_type EQ MFW_SMS_STATRPT)
		{
			if(sms_msg_delete((UBYTE)data->sms_editor_data->sms_info->sms_mt.index/*SPR 2686*/
								, data->sms_editor_data->sms_mem)== MFW_SMS_OK)/*a0393213 OMAPS00109953*/

			{
		       /* set the status that the user selected the delete item */
				if(deleting_win == 0)
					deleting_win= mmi_dialog_information_screen_forever(0,TxtPleaseWait, NULL, COLOUR_STATUS_WAIT);

				SmsRead_R_TEXTEDT2_destroy(data->win);
			/*SPR 2530, Delete and recreate SMS list window to show updates*/
			 {	T_MFW_SMS_STAT  sms_list_type = MFW_SMS_ALL;

				listsDestroy(list_win);
				list_win=0;
			    SmsRead_R_destroy(sms_read_win);
				SmsRead_R_start(NULL, (MfwMnuAttr*)&sms_list_type);
			}

			}
			else
			{		   		
				
				/*SPR2530, don't delete option and display windows, just show failure*/
				mmi_dialog_information_screen(0,TxtFailed, NULL, NULL, COLOUR_STATUS_WAIT);
			}
		}
		//x0pleela 01 Feb, 2006 DR: OMAPS00059261
		//Made changes as per review comments
		else //MT_TEXT
		{
		// Present the user with an Options menu from the Read SMS text screen.:16/2/01
			SmsRead_R_READSMS_OPT_start(win, (MfwMnuAttr*)&data->sms_editor_data);
		}
		
		break;
	          
		case INFO_KCD_RIGHT:
		case INFO_KCD_HUP:
			SmsRead_R_TEXTEDT2_destroy(data->win);
			/*SPR 2530, Delete and recreate SMS list window to show updates*/
			{ T_MFW_SMS_STAT  sms_list_type = MFW_SMS_ALL;

				listsDestroy(list_win);
				list_win=0;
			    SmsRead_R_destroy(sms_read_win);
				SmsRead_R_start(NULL, (MfwMnuAttr*)&sms_list_type);
	    	}
			break;
		case INFO_KCD_DOWN:
			break;
		case INFO_KCD_CALL:
            /* Marcus: Issue 1041: 05/09/2002: Start */
            strcpy((char *)pb_data.number, (char *)SmsData.NumberBuffer);
            /* Marcus: Issue 1041: 05/09/2002: End */
        	TRACE_EVENT_P1("Number is = %d",(int)pb_data.number);
        	callNumber(pb_data.number);
		break;


		default:
			TRACE_EVENT("Err: Default No match found");
			break;
	}
}

/*******************************************************************************

 $Function:    	SmsRead_R_TEXTEDT2_exec_cb

 $Description:	Exec callback function of the SMS editor window (display of the 
 				text of one SMS)
 
 $Returns:		none

 $Arguments:	win - current window handler
 				event - event id
 				value - unique id
 				parameter - optional data. 
*******************************************************************************/

void SmsRead_R_TEXTEDT2_exec_cb (T_MFW_HND win, USHORT event, SHORT value, void * parameter)
    /* callback handler for events sent to to trigger execution */
{	
    T_MFW_WIN      * win_data = ((T_MFW_HDR *) win)->data;
    T_SMSREAD_R_TEXTEDT2          * data = (T_SMSREAD_R_TEXTEDT2 *)win_data->user;
    T_MFW_HND      * l_parent;
    SHORT            l_id;
    T_MFW_SMS_MO *SmsMo;
    T_MFW_SMS_MT *SmsMt = NULL;

	/* x0045876, 14-Aug-2006 (WR - "SmsCb" was set but never used)*/
	/* T_MFW_SMS_CB *SmsCb; */

	/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
	T_AUI_EDITOR_DATA sms_editor_info;
	USHORT size;
/*SPR 2530*/
	T_ATB_TEXT main_text;
	T_ATB_TEXT temp_text;
//	T_ATB_DCS atb_dcs;  // RAVI
#else /* NEW_EDITOR */
	T_EDITOR_DATA sms_editor_info;
#endif /* NEW_EDITOR */

//	USHORT i, line_ret, line_nb;   // RAVI

/* x0045876, 14-Aug-2006 (WR - "msg_len" was declared but never referenced)*/
#ifndef NEW_EDITOR
	USHORT msg_len;
#endif

//	div_t result;     // RAVI
    /* JVJ SPR 1298 - Memory optimization. Converted to dynamic and size decreased*/
    char* sms_text_buffer = (char*)ALLOC_MEMORY(MAX_EDITOR_HEADER_LENGTH);
	T_MFW_PHB_ENTRY  pb_entry;
	USHORT index=0;
	T_MFW_SMS_IDX msg_info;/*SPR 2530*/
	
    TRACE_FUNCTION ("SmsRead_R_TEXTEDT2_exec_cb()");

    switch (event)
	{
	case E_INIT:
	{  

	    /* x0045876, 14-Aug-2006 (WR - "input_type" was declared but never referenced)*/
#ifndef NEW_EDITOR
	    UBYTE input_type;/*MC SPR1242*/
#endif

	    /* initialization of administrative data */
	    data->id = value;

		data->sms_editor_data = (T_SMS_EDITOR_DATA *)parameter;
		
		switch (data->sms_editor_data->sms_type)
		{
		case MMI_SMS_MO:
			
			/* where does we need that:
			   For editor when the user wants to editing the sms-message
			*/
			read_status = MMI_SMS_MO;

			SmsMo = (T_MFW_SMS_MO *)data->sms_editor_data->sms_info;
			/*a0393213 OMAPS00109953*/
			sms_copy_idx(SmsMo->msg_ref, &msg_info,data->sms_editor_data->sms_mem );/*SPR 1830*/
			// API Clear the buffers before showing the MO message.
      		memset (sms_text_ascii, '\0', MAX_EDITOR_LEN);
      		memset (sms_text_buffer, '\0', MAX_EDITOR_HEADER_LENGTH);
/*SPR 2530, if new ediotr use nice string-handling functions*/
#ifdef NEW_EDITOR
			main_text.data = (UBYTE*)sms_text_ascii;
			/*if we are in chinese, then we need to display in Unicode, otherwise ASCII*/
			if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				main_text.dcs = ATB_DCS_UNICODE;
			else
				main_text.dcs = ATB_DCS_ASCII;
		       ATB_string_Length(&main_text);  
			/*if this isn't an invalid message*/
			if (msg_info.stat != MFW_SMS_INVALID)
			{	/*set text string dcs, according to alphabet used for message*/
				if ( (SmsMo->dcs & 0x0c) == MFW_DCS_UCS2 || SmsMo->sms_msg[0] == (char)0x80)
				{
					temp_text.dcs = ATB_DCS_UNICODE;
					/* CQ 16780  15-01-04 MZ */
					/*copy message text to string*/
					temp_text.data = (UBYTE*)SmsMo->sms_msg;
				}
				else
				{
					temp_text.dcs = ATB_DCS_ASCII;
					/* CQ: 16780 Convert to Ascii format 15-01-04 MZ */
					ATB_convert_String(	SmsMo->sms_msg, MFW_DCS_8bits , SmsMo->msg_len,
										sms_text_ascii, MFW_ASCII,  MAX_MSG_LEN, TRUE);
					/*copy message text to string*/
					temp_text.data = (UBYTE*)sms_text_ascii;
				}
				
				ATB_string_Length(&temp_text);
				/*concat message text into display string*/
				ATB_string_Concat(&main_text, &temp_text);
#ifdef TI_PS_FF_CONC_SMS					
				/*if this is an incomplete concat message, add "Incomplete Message" to end of display text*/
				if (msg_info.concat_status == MFW_SMS_CONC_PART)
				{	
				       ATB_string_SetChar(&main_text, ATB_string_Length(&main_text),(USHORT)':');
					if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
						temp_text.dcs = ATB_DCS_UNICODE;
					else
						temp_text.dcs = ATB_DCS_ASCII;
					temp_text.data = (UBYTE*)MmiRsrcGetText(TxtIncompleteMessage);
					ATB_string_Length(&temp_text);
					ATB_string_Concat(&main_text, &temp_text);
				}
#endif /*TI_PS_FF_CONC_SMS*/				

			}
			else
			{/*set display text to "Invalid Message"*/
				if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
					temp_text.dcs = ATB_DCS_UNICODE;
				else
					temp_text.dcs = ATB_DCS_ASCII;
				temp_text.data = (UBYTE*)MmiRsrcGetText(TxtInvalidMessage);
				ATB_string_Length(&temp_text);
				ATB_string_Concat(&main_text, &temp_text);
			}
#else

					/* SH - convert from original dcs once more */

			/*MC,SPR1242 don't trust the DCS, check for Unicode tag*/
			if (SmsMo->sms_msg[0] == 0x80||SmsMo->dcs == MFW_DCS_UCS2)
			{	input_type = MFW_DCS_UCS2;}
			else
			{	input_type = MFW_DCS_8bits;}

			/*mc end*/
			if (input_type == MFW_DCS_UCS2/*MC SPR1242*/)
			{/*SPR2175, use new function for conversion*/

				ATB_convert_String(	SmsMo->sms_msg, /*SmsMo->dcs*/input_type/*MC SPR1242*/, SmsMo->msg_len,
										&sms_text_ascii[2], MFW_DCS_UCS2,  MAX_MSG_LEN-2, TRUE);
				sms_text_ascii[0] = 0x80;
				sms_text_ascii[1] = 0x7F;
			}
			else
			{/*SPR2175, use new function for conversion*/
			  
				ATB_convert_String(	SmsMo->sms_msg, /*SmsMo->dcs*/input_type/*MC SPR1242*/ , SmsMo->msg_len,
										sms_text_ascii, MFW_ASCII,  MAX_MSG_LEN, TRUE);
				
			}

#endif			
			break;

		case MMI_SMS_MT:
			/* where does we need that:
				For editor when the user wants to editing the sms-message
			 */
			read_status = MMI_SMS_MT;
			
			SmsMt = (T_MFW_SMS_MT *)data->sms_editor_data->sms_info;
			/*a0393213 OMAPS00109953*/
			sms_copy_idx(SmsMt->index, &msg_info	,SmsMt->memory);/*SPR 2530*/
      		memset (sms_text_ascii, '\0', MAX_EDITOR_LEN);
      		memset (sms_text_buffer, '\0',MAX_EDITOR_HEADER_LENGTH);
/*SPR 2530, if new editor use nice string-handling functions*/
#ifdef NEW_EDITOR
			main_text.data = (UBYTE*)sms_text_ascii;
/*set up text data stucture for main display string*/
			if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
				main_text.dcs = ATB_DCS_UNICODE;
			else
				main_text.dcs = ATB_DCS_ASCII;
			ATB_string_Length(&main_text);  		
#endif
			if (msg_info.stat != MFW_SMS_INVALID)/*SPR 1830*/
			{	

            /* Marcus: Issue 1041: 05/09/2002: Start */
            // API - Copy number into buffer so when Call key is pressed in editor call is generated
        	strcpy((char *)SmsData.NumberBuffer,(char *)SmsMt->orig_addr.number);
            /* Marcus: Issue 1041: 05/09/2002: End */

            /* Marcus: Issue 1489: 13/12/2002: Start */
            /*API - 1053 - 09-09-02 Copy the number into this buffer so that when 
									a call is generated from the editor it is connected.
			*/
            if(SmsMt->orig_addr.ton == MFW_TON_INTERNATIONAL)
            {
     			if(SmsMt->orig_addr.number[0] == '+')
	    		{
		    		strcpy((char *)SmsData.NumberBuffer,(char *)SmsMt->orig_addr.number);
     			}
	    		else
    			{
	    			strcpy((char *)SmsData.NumberBuffer, "+");
    				strncat((char *)SmsData.NumberBuffer,(char *)SmsMt->orig_addr.number,sizeof(SmsData.NumberBuffer)-2);
    			}
            }
#ifndef NEW_EDITOR
							/* add the number or the name if he can find it in the phonebook*/
			if (bookFindNameInPhonebook((const char*)SmsMt->orig_addr.number, &pb_entry) !=0)
			{
#ifdef NO_ASCIIZ
				/*mc SPR 1257, unicode support*/
				memcpy(sms_text_ascii, (char*)pb_entry.name.data,pb_entry.name.len );
#else
				strcpy(sms_text_ascii, (char*)pb_entry.name);
#endif
			}
			else
			{
				
				strcpy(sms_text_ascii, (char *)SmsData.NumberBuffer);
                /* Marcus: Issue 1489: 13/12/2002: End */
			}
#else
/*SPR 2530, if new editor use nice string-handling functions*/
		/*see if the sender is in the phonebook*/
		if (bookFindNameInPhonebook((const char*)SmsMt->orig_addr.number, &pb_entry) !=0)
		{	char temp_string[PHB_MAX_LEN];

				memset(temp_string, '\0', PHB_MAX_LEN);
				/*copy name into a text data structure*/
				
				/* CQ: 16780 Check the phonebook string format by testing the dcs. NOT the dcs values are Enum types and not the true dcs 
				     values of the ADN store. 15-01-04 MZ */
				if (pb_entry.name.dcs == MFW_DCS_UCS2  ||pb_entry.name.data[0] == 0x80 )
					    temp_text.dcs = ATB_DCS_UNICODE;
				else
					temp_text.dcs = ATB_DCS_ASCII;
				
				if (temp_text.dcs != ATB_DCS_UNICODE)
				{	
					temp_text.data = (UBYTE*)pb_entry.name.data;	
				}
				
				else
				{
					/*  CQ:16780 Check for unicode string type 15-01-04 MZ*/ 
					if(pb_entry.name.data[0] == 0x80)
					{
					 	pb_entry.name.len-=2;
						memcpy(temp_string, &pb_entry.name.data[2], pb_entry.name.len);
					}
					else
					{
						memcpy(temp_string, &pb_entry.name.data[0], pb_entry.name.len);
					}
					
					temp_text.data = (UBYTE*)temp_string;
					
					
				}
				ATB_string_Length(&temp_text);
				/*add name text into main display string*/
				ATB_string_Concat(&main_text, &temp_text);

		}
			ATB_string_Length(&main_text);

#endif
#ifndef NEW_EDITOR
	/* SH - convert from original dcs once more */
				/*MC,SPR1242 don't trust the DCS, check for Unicode tag*/
			if (SmsMt->sms_msg[0] == 0x80||SmsMt->dcs == MFW_DCS_UCS2)
			{	input_type = MFW_DCS_UCS2;}
			else
			{	input_type = MFW_DCS_8bits;}

			/*mc end*/
			if (/*SmsMt->dcs*/input_type == MFW_DCS_UCS2)
			{	/*SPR2175, use new function for conversion*/
				ATB_convert_String(	SmsMt->sms_msg, input_type /*MC SPR1242*/, SmsMt->msg_len,
										&sms_text_ascii[2], MFW_DCS_UCS2,  MAX_MSG_LEN-2, TRUE);
				sms_text_ascii[0] = 0x80;
				sms_text_ascii[1] = 0x7F;
			}
			else
			{
		
#endif
					if(strlen(sms_text_ascii) != 14 ) // number of character that can be displayed on one line.
						sms_text_buffer[index++]='\n';
				
					sms_text_buffer[index++]='0'+SmsMt->sctp.day[0];
					sms_text_buffer[index++]='0'+SmsMt->sctp.day[1];
					sms_text_buffer[index++]='/';
					sms_text_buffer[index++]='0'+SmsMt->sctp.month[0];
					sms_text_buffer[index++]='0'+SmsMt->sctp.month[1];
					sms_text_buffer[index++]='/';
					sms_text_buffer[index++]='0'+SmsMt->sctp.year[0];
					sms_text_buffer[index++]='0'+SmsMt->sctp.year[1];
					sms_text_buffer[index++]=',';
					sms_text_buffer[index++]='\n';
					
					sms_text_buffer[index++] ='0'+SmsMt->sctp.hour[0];
					sms_text_buffer[index++]='0'+SmsMt->sctp.hour[1];
					sms_text_buffer[index++]=':';
					sms_text_buffer[index++]='0'+SmsMt->sctp.minute[0];
					sms_text_buffer[index++]='0'+SmsMt->sctp.minute[1];
					sms_text_buffer[index++]='\n';	
#ifndef NEW_EDITOR
		strcat(sms_text_ascii, sms_text_buffer);
				msg_len = strlen(sms_text_ascii);
				
				/*
					keep the length of the additional information (Date,Time & Number);
					need it later for editing the sms messages (user pressed-> EDIT)
				*/
				current_sms_len = msg_len;
				
				/* SH - convert from original dcs once more */
				/*SPR2175, use new function for conversion*/
				ATB_convert_String(	SmsMt->sms_msg, /*SmsMt->dcs*/input_type /*MC SPR1242*/, SmsMt->msg_len,
            		&sms_text_ascii[msg_len], MFW_ASCII /*SH*/,  MAX_EDITOR_LEN-(msg_len+1), TRUE);

			}

#else
/*SPR 2530, if new editor use nice string-handling functions*/
				/*Add date and time to main display string*/
				temp_text.dcs = ATB_DCS_ASCII;
				temp_text.data = (UBYTE*)sms_text_buffer;
				ATB_string_Length(&temp_text);
				ATB_string_Concat(&main_text, &temp_text);

				
				if ((SmsMt->dcs&0x0c) == MFW_DCS_UCS2)
				{
					temp_text.dcs = ATB_DCS_UNICODE;
					temp_text.data = (UBYTE*)SmsMt->sms_msg;

					ATB_string_Length(&temp_text);
					ATB_string_Concat(&main_text, &temp_text);
				}
				else
				{
					temp_text.dcs = ATB_DCS_ASCII;
					
					/* CQ 16780 Convert incoming SMS text to ASCIII 15-01-04 MZ */ 

					/* Apr 09, 2007 OMAPS00124879 x0039928(sumanth) */
					/* Fix : Memory is allocated for temporary data */
					temp_text.data = (UBYTE*)ALLOC_MEMORY(MAX_MSG_LEN);
					ATB_convert_String(	SmsMt->sms_msg, MFW_DCS_8bits , SmsMt->msg_len,
										(char*)temp_text.data, MFW_ASCII,  MAX_MSG_LEN, TRUE);

					ATB_string_Length(&temp_text);
					ATB_string_Concat(&main_text, &temp_text);
					FREE_MEMORY((void*)temp_text.data,MAX_MSG_LEN);
				}
				
#ifdef TI_PS_FF_CONC_SMS					
				/*if the message is a fragment of a concat message*/
				if (msg_info.concat_status == MFW_SMS_CONC_PART)
				{	ATB_string_SetChar(&main_text, ATB_string_Length(&main_text),(USHORT)':');
					if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
						temp_text.dcs = ATB_DCS_UNICODE;
					else
						temp_text.dcs = ATB_DCS_ASCII;
					/*append "Incomplete Message" onto display string*/
					temp_text.data = (UBYTE*)MmiRsrcGetText(TxtIncompleteMessage);
					ATB_string_Length(&temp_text);
					ATB_string_Concat(&main_text, &temp_text);
				}
#endif /*#ifdef TI_PS_FF_CONC_SMS*/				
#endif
			}
			else
#ifdef NEW_EDITOR
			/*SPR 2530, if new editor use nice string-handling functions*/
			{	/*if invalid message just copy "Invalid message" into main display string*/
				if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
					temp_text.dcs = ATB_DCS_UNICODE;
				else
					temp_text.dcs = ATB_DCS_ASCII;
				temp_text.data = (UBYTE*)MmiRsrcGetText(TxtInvalidMessage);
				ATB_string_Length(&temp_text);
				ATB_string_Concat(&main_text, &temp_text);
			}
#endif
			sms_change_read_status(SmsMt->index
									,data->sms_editor_data->sms_mem /*a0393213 - the memory was saved whenever a message was selected in a list of messages*/
									);
			break;

		case MMI_SMS_CB:

			/* x0045876, 14-Aug-2006 (WR - "SmsCb" was set but never used)*/
			/* SmsCb = (T_MFW_SMS_CB *)data->sms_editor_data->sms_info; */
			
			break;

		}

			/*
				Actually we have 2 different size of messages:
				1) SMS_MT with MAX_EDITOR_LEN because of the DATE,TIME,NUMBER
				2) SMS_MO with MAX_MSG_LEN, just the message itself
			*/

			/* SPR#1428 - SH - New Editor changes */
#ifdef NEW_EDITOR
			AUI_edit_SetDefault(&sms_editor_info);
			
			/* Include title for colour display */
#ifdef COLOURDISPLAY
			AUI_edit_SetDisplay(&sms_editor_info, ZONE_FULL_SK_TITLE_SCROLL, COLOUR_EDITOR, EDITOR_FONT);
#else /* COLOURDISPLAY */
			AUI_edit_SetDisplay(&sms_editor_info, ZONE_FULL_SK_SCROLL, COLOUR_EDITOR, EDITOR_FONT);
#endif /* COLOURDISPLAY */
			AUI_edit_SetMode(&sms_editor_info, ED_MODE_ALPHA | ED_MODE_READONLY, ED_CURSOR_NONE);
			AUI_edit_SetEvents(&sms_editor_info, 0, FALSE, FOREVER, (T_AUI_EDIT_CB)SmsRead_R_TEXT_editor_cb);

			//x0pleela 08 Feb, 2006 DR: OMAPS00059261
			//Replacing MT_STATUS_RPT with MFW_SMS_STATRPT as MT_STATUS_RPT is removed

			//x0pleela 01 Feb, 2006 DR: OMAPS00059261
			//Made changes as per review comments
			
			//x0pleela 30 Jan, 2006 DR: OMAPS00059261
			//Displaying the LSK as "Delete" for MT Staus report and "Options" for normal MT Text message 
			if (SmsMt!=NULL && SmsMt->sms_msg_type EQ MFW_SMS_STATRPT) 
				AUI_edit_SetTextStr(&sms_editor_info, TxtDelete, TxtSoftBack, TxtMessage, NULL);
			else 
			{
			// Oct 18, 2006    DR: OMAPS00099411  x0039928
			// Fix: Message title is displayed as 'Not supported' 
				if (Mmi_getCurrentLanguage() != CHINESE_LANGUAGE)
				{	if(SmsMt!=NULL && ((SmsMt->dcs&0x0c) == MFW_DCS_UCS2))
						AUI_edit_SetTextStr(&sms_editor_info, TxtSoftOptions, TxtSoftBack, TxtNotSupported, NULL);
					else
						AUI_edit_SetTextStr(&sms_editor_info, TxtSoftOptions, TxtSoftBack, TxtMessage, NULL);
				}
				else
						AUI_edit_SetTextStr(&sms_editor_info, TxtSoftOptions, TxtSoftBack, TxtMessage, NULL);
			}
			
			/* Check if concatenated sms is activated */
			
			if (read_status EQ MMI_SMS_MO)
      			size	= MAX_MSG_LEN;
			else
				size	= MAX_EDITOR_LEN;
/*SPR 2530, copy display screen to editor, alphabet depends on current lanaguage*/	
			if (Mmi_getCurrentLanguage() == CHINESE_LANGUAGE)
			{
				/* Unicode SMS message  */
				AUI_edit_SetBuffer(&sms_editor_info, ATB_DCS_UNICODE, (UBYTE *)&sms_text_ascii[0],size/2);
			}
			else
			{
				/* ASCII SMS message */
				AUI_edit_SetBuffer(&sms_editor_info, ATB_DCS_ASCII, (UBYTE *)&sms_text_ascii[0],size);
			}

			data->editor_win = AUI_edit_Start(win,&sms_editor_info);  /* start the editor */
			
		#else /* NEW_EDITOR */

			SmsSend_loadEditDefault(&sms_editor_info);			
			sms_editor_info.editor_attr.text	    = (char *)sms_text_ascii;

			if (read_status EQ MMI_SMS_MO)
      			sms_editor_info.editor_attr.size	= MAX_MSG_LEN;
			else
				sms_editor_info.editor_attr.size	= MAX_EDITOR_LEN;
			sms_editor_info.min_enter				= 0; 

			//x0pleela 08 Feb, 2006 DR: OMAPS00059261
			//Replacing MT_STATUS_RPT with MFW_SMS_STATRPT as MT_STATUS_RPT is removed
			
			//x0pleela 30 Jan, 2006 DR: OMAPS00059261
			//Displaying the LSK as "Delete" for MT Staus report and "Options" for normal MT Text message 
			if (SmsMt->sms_msg_type EQ MFW_SMS_STATRPT )
			{
				sms_editor_info.LeftSoftKey				= TxtDelete;
				sms_editor_info.AlternateLeftSoftKey	= TxtDelete;
			}
			//x0pleela 01 Feb, 2006 DR: OMAPS00059261
			//Made changes as per review comments
			else //if( SmsMt->sms_msg_type EQ MT_TEXT )
			{
				sms_editor_info.LeftSoftKey				= TxtSoftOptions;
				sms_editor_info.AlternateLeftSoftKey	= TxtSoftOptions;
			}	
			sms_editor_info.RightSoftKey			= TxtSoftBack;
			sms_editor_info.Identifier				= 0 ; 
			sms_editor_info.destroyEditor			= FALSE ;
			sms_editor_info.Callback				= (T_EDIT_CB)SmsRead_R_TEXT_editor_cb;
			sms_editor_info.mode					= READ_ONLY_MODE;

			data->editor_win = editor_start(win,&sms_editor_info);  /* start the editor */
		#endif /* NEW_EDITOR */


		if (info_win){
    	      SEND_EVENT(info_win,DIALOG_DESTROY,0,0 );
	          info_win = 0;
		}


		break;
	}
	case E_BACK:
	    l_parent = data->parent;
	    l_id = data->id;
	    /* The editor has sent an E_BACK event, we return to the previous window (E_RETURN) */
	    SEND_EVENT (l_parent, E_RETURN, l_id, NULL);
	    SmsRead_R_TEXTEDT2_destroy(data->win);
		break;
		
	case E_ABORT:
	case E_EXIT:
	    l_parent = data->parent;
	    l_id = data->id;
	    SEND_EVENT (l_parent, event, l_id, NULL); /* forward event to parent */
	    SmsRead_R_TEXTEDT2_destroy(data->win);
	    break;

	default:
    break;
	}

    FREE_MEMORY((void*)sms_text_buffer,MAX_EDITOR_HEADER_LENGTH);
    return;
}

/*******************************************************************************

 $Function:    	SmsRead_R_TEXTEDT2_win_cb

 $Description:	Window callback function of the SMS editor window (display of 
 				the text of one SMS).

 $Returns:		execution status

 $Arguments:	w - mfw window handle
 				e - event id
 				value - unique id
 				parameter - optional data. 
*******************************************************************************/

static int SmsRead_R_TEXTEDT2_win_cb (MfwEvt e, MfwWin *w)
    /* window event handler */
{
    TRACE_FUNCTION ("SmsRead_R_TEXTEDT2_win_cb()");

    switch (e)
	{
        case MfwWinVisible:  /* window is visible  */
//JVJE	    displaySoftKeys(TxtSoftSelect,TxtSoftBack);
	    break;
	case MfwWinFocussed: /* input focus / selected   */
	case MfwWinDelete:   /* window will be deleted   */
        default:
	    return MFW_EVENT_REJECTED;
	}
    return MFW_EVENT_CONSUMED;
}
#if(0) /* x0039928 - Lint warning fix */
/*******************************************************************************

 $Function:    	SmsRead_R_TEXTEDT2_smslist_cb

 $Description:	Callback function of the SMS editor window (editor itself).
 
 $Returns:		none
 
 $Arguments:	Parent - parent window.
 				ListData - Menu item list
*******************************************************************************/

void SmsRead_R_TEXTEDT2_smslist_cb(T_MFW_HND * Parent, ListMenuData * ListData)
{
    T_MFW_WIN       * win_data = ((T_MFW_HDR *)Parent)->data;
    T_SMSREAD_R * data     = (T_SMSREAD_R *)win_data->user;    
	  
	T_MFW_HND      * l_parent;
    SHORT            l_id;

	if ((ListData->Reason EQ LISTS_REASON_BACK) || (ListData->Reason EQ LISTS_REASON_CLEAR)
		|| (ListData->Reason EQ LISTS_REASON_HANGUP))
	{
	    l_parent = data->parent;
	    l_id = data->id;
	    SmsRead_R_destroy(data->win);
	    //The editor has sent an back event, we return to the previous window (E_RETURN)
	    SEND_EVENT (l_parent, E_RETURN, l_id, NULL);
	}
}
#endif
/*******************************************************************************

 $Function:    	check_assoc_name

 $Description:	This function search if a phone number corresponds to any name in phonebook
			 	or to the voice mailbox or to the SMS center
			 	BE CAREFUL it allocates memory for the name, that has to be freed by the caller
			 	It returns the length of the associated name if found, else length is 0 and name is NULL
			 
 $Returns:		string length
 
 $Arguments:	name - string
 				sms_addr - phonenumber string.
*******************************************************************************/

UBYTE check_assoc_name(char *name, T_MFW_SMS_ADDR sms_addr)
{
	// MZ T_MFW_SMS_STAT pb_entry;
	T_MFW_SMS_ADDR pb_entry;
	USHORT i;



	// TBD: add international numbers management

	// Voice, fax... mailboxes data
	
//	USHORT max_rcd;  // RAVI
	char mail_number[PHB_MAX_LEN + 1];
	char mail_alpha[10]; // size hard coded in pcm.h
	UBYTE nlen;

	name = NULL;

	// First: look in phonebook
	
	if (bookFindNameInPhonebook((const char*)&sms_addr.number[0], (T_MFW_PHB_ENTRY*)&pb_entry))
	{
		// A name has been found
		if ((name = (char*)ALLOC_MEMORY(strlen(pb_entry.tag) + 1)) != NULL)
		{
		/*Mc , unicode support*/
		#ifdef NO_ASCIIZ
				memcpy(name, pb_entry.tag, pb_entry.len);
				return pb_entry.len;
		#else
			strcpy(name, pb_entry.tag);
			return strlen(name);
		#endif
		}
	}
	
	// Then the different mailboxes (voice, fax, data...)
	for (i = MFW_MB_LINE1; i <= MFW_MB_DATA; i++)
	{

		{
			cmhPHB_getAdrStr(mail_number, PHB_MAX_LEN - 1, FFS_flashData.mbn_Num, FFS_flashData.mbn_len);
			
			if (strcmp(sms_addr.number, mail_number) EQ 0)
			{
				// If the same number then we get the alpha identifier
				cmhPHB_getMfwTagNt ( FFS_flashData.mbn_AlphId, 10, (UBYTE*)mail_alpha, &nlen );
				mail_alpha[nlen] = '\0';    

				if ((name = (char*)ALLOC_MEMORY(strlen(mail_alpha) + 1)) != NULL)
				{
/*MC, yet more unicode support*/
#ifdef NO_ASCIIZ
					memcpy(name, mail_alpha, nlen);
					return nlen;
#else
					strcpy(name, mail_alpha);
					return strlen(name);
#endif
				}
			}
		}
	}
	return 1;  // RAVI
}

#if(0) /* x0039928 - Lint warning fix */
/*******************************************************************************

 $Function:    	SmsRead_R_TEXTEDT2_editor_cb

 $Description:  Callback function for the  text editor.
			 
 $Returns:		void
 
 $Arguments:	win - current window.
 				Identifier - unique id
 				Reason - event id
*******************************************************************************/

void SmsRead_R_TEXTEDT2_editor_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason)
{

    T_MFW_WIN     		* win_data = ((T_MFW_HDR *) win)->data;
    T_SMSREAD_R_TEXTEDT2 * data     = (T_SMSREAD_R_TEXTEDT2 *)win_data->user;

    TRACE_FUNCTION("SmsRead_R_TEXTEDT2_editor_cb");
	switch (Reason)
	{
	case INFO_KCD_LEFT:
	case INFO_KCD_RIGHT:
	case INFO_KCD_HUP:
	    SEND_EVENT (data->parent, E_RETURN, Identifier, NULL); /* forward event to parent */
	    break;
	default:
		break;
	}
}
#endif

/*******************************************************************************

 $Function:    	SmsRead_FindNextNumber_cb

 $Description:	Exec callback function for searching the SMS text message.
 
 $Returns:		none

 $Arguments:	win - window handler
 				event - mfw event
 				value -optional id
 				parameter - optional data.
 
*******************************************************************************/
void SmsRead_FindNextNumber_cb(T_MFW_HND win, USHORT Identifier, USHORT Reason)
{
    
	T_MFW_PHB_ENTRY  pb_data;

	switch (Reason)
	{
	case INFO_KCD_RIGHT:
	case INFO_KCD_CLEAR:
	
		break;
	case INFO_KCD_LEFT:
		
		// Check for numbers in SMS text, check for '+' .

		if(Search_forNextNumber(&SmsData))
		{
			
			if (bookFindNameInPhonebook((const char*)&SmsData.NumberBuffer[0], &pb_data) )
			{
				//Display the tag and number dialog.
#ifdef NO_ASCIIZ			
	/* SPR888 - SH - parent_data->parent changed to win */
				smsRead_NumberName_info_screen(win,(void *)&SmsData, (char*)pb_data.name.data);
#else
	/* SPR888 - SH - parent_data->parent changed to win */
				smsRead_NumberName_info_screen(win,(void *)&SmsData, (char*)pb_data.name);
#endif
			}
			else
			{
				/* SPR888 - SH - parent_data->parent changed to win */
				smsRead_NumberName_info_screen(win,(void *)&SmsData,NULL);
			}			
		}
		break;
	case INFO_KCD_OFFHOOK:
		callNumber(SmsData.NumberBuffer); 
		break;
		
	default:
			TRACE_EVENT(" >>> INFO Default   ");
		//return MFW_EVENT_REJECTED;
		break;
	}
	
}

/*******************************************************************************

 $Function:    	smsRead_NumberName_info_screen

 $Description:    Display the number and name of numbers found in the received SMS text.
			 
 $Returns:		window handle
 
 $Arguments:	win - current window.
 				UserData - Sms data info
*******************************************************************************/

static MfwHnd smsRead_NumberName_info_screen(MfwHnd win,T_SmsUserData *UserData, char* nameTag)
{

	T_DISPLAY_DATA DisplayInfo;
	TRACE_FUNCTION("smsRead_NumberName_info_screen() ");
	
	dlg_initDisplayData_TextStr( &DisplayInfo, TxtSoftNext, TxtSoftBack, nameTag,  (char*)UserData->NumberBuffer, COLOUR_STATUS);
	dlg_initDisplayData_events( &DisplayInfo, (T_VOID_FUNC)SmsRead_FindNextNumber_cb, FOREVER, KEY_ALL );

	TRACE_EVENT(">>>>> Display Information Screen ");
   return info_dialog(win, &DisplayInfo);

}

/*******************************************************************************

 $Function:    	Search_forNextNumber

 $Description:    Search through the SMS text and find a telephone number, the number 
 				may start with the '+' prefix.
			 
 $Returns:		Boolean
 
 $Arguments:	UserData - SMS data info
*******************************************************************************/
static BOOL Search_forNextNumber(T_SmsUserData *UserData)
{
		/* SPR888 - SH - Rewritten, old function below */
	/* Use search_index so we can start looking where we left off last time.
	Set to 0 in SMSRead_R_OPTExeStoreNumber */
	
	int sms_length;
	int number_index;
	char number_buffer[NUMBER_LENGTH*2+4];
	char *sms_buffer;
	char character;
	UBYTE found_status;

	TRACE_FUNCTION("Search_forNextNumber()");
	sms_buffer = (char*)UserData->TextBuffer;
	#ifdef NO_ASCIIZ
	if (sms_buffer[0] == (char)0x80)
	{
#ifdef NEW_EDITOR /* SPR#1428 - SH - New Editor changes - new function to calculate length of unicode string */
		sms_length = ATB_string_UCLength((U16*)sms_buffer)*sizeof(USHORT);
#else /* NEW_EDITOR */
		sms_length = strlenUnicode((U16*)sms_buffer);
#endif /* NEW_EDITOR */
	}
	else
	{
		sms_length = strlen(sms_buffer);
	}
	#else /* NO_ASCIIZ */
	sms_length = strlen(sms_buffer);
	#endif /* NO_ASCIIZ */
	
	number_index = 0;
	found_status = 0;

	memset (number_buffer, '\0', NUMBER_LENGTH*2+4);

	
	while (search_index<sms_length && found_status<2 && number_index<NUMBER_LENGTH*2+4)
	{
		character = sms_buffer[search_index];

		search_index++;
#ifdef NO_ASCIIZ
		if (character=='+' ||  ( character>='0' && character<='9' ) || (character==0))
#else
		if (character=='+' ||  ( character>='0' && character<='9' ) )
#endif
		{	if (character != 0) /*MC, only copy non-Null chars*/
				number_buffer[number_index++] = character;
			if (found_status==0)
			{
				found_status = 1;		/* Found start of number */
			}
		}
		else
		{
			if (found_status==1)
				found_status = 2;		/* Have reached end of number */
		}
		TRACE_EVENT_P1("Extract number string:%s", number_buffer);
	}

	if(strlen(number_buffer) > 10)

	{

	   memcpy((char*)UserData->NumberBuffer, number_buffer, sizeof(UserData->NumberBuffer));
	   return TRUE;
	}

	return FALSE;
}
	

/* api Store number for calling when reading SMS - 04/03/2002
*/

/*******************************************************************************

 $Function:    	Search_forNextNumber

 $Description:    Search through the SMS text and find a telephone number, the number 
 				may start with the '+' prefix.
			 
 $Returns:		Boolean
 
 $Arguments:	UserData - SMS data info
*******************************************************************************/
void SmsRead_StoreNumber(void)
{
    T_MFW_HND       win  = mfwParent(mfw_header());
    T_MFW_WIN     * win_data = ((T_MFW_HDR *)win)->data;
    T_SMSREAD_R_OPT         * data = (T_SMSREAD_R_OPT *)win_data->user;

	T_MFW_HDR    * parent      =  data->parent;
    T_MFW_WIN    * win_data2   =  ((T_MFW_HDR *)parent)->data;
    T_SMSREAD_R_TEXTEDT2  * parent_data =  (T_SMSREAD_R_TEXTEDT2 *)win_data2->user;
	T_MFW_PHB_ENTRY  pb_data;
	
	TRACE_FUNCTION("SmsRead_StoreNumber");

	TRACE_EVENT_P1("TON = %d",parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.ton);
	/* Marcus: Issue 1489: 13/12/2002: Start */
	if(parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.ton == MFW_TON_INTERNATIONAL)
	{
		if(parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.number[0] == '+')
		{
			strcpy((char *)SmsData.NumberBuffer,(char *)parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.number); 
		}
		else
		{
			strcpy((char *)SmsData.NumberBuffer,"+");
			strncat((char *)SmsData.NumberBuffer,(char *)parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.number,sizeof(SmsData.NumberBuffer)-2); 
		}
	}
	else
	{
	
		TRACE_EVENT_P1("Number = %s",parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.number);
		strcpy((char *)SmsData.NumberBuffer,(char *)parent_data->sms_editor_data->sms_info->sms_mt.orig_addr.number); 
	}
	/* Marcus: Issue 1489: 13/12/2002: End */
	/*MC, unicode support*/
	/* x0039928 - Lint warning fix */
	memcpy((char*)pb_data.number, (char*)SmsData.NumberBuffer, sizeof(SmsData.NumberBuffer)); 


	    SmsRead_R_TEXTEDT2_destroy(message_win);
		/* Destroy the Options Menu and text editor.before starting the Send Editor.*/
		SmsRead_R_OPT_destroy(option_mnu_win);

		/*SPR 2686 destroy the SMS read window to free memory*/
		/*destroy the built list window*/
		listsDestroy(list_win);
		list_win=0;
		/*destroy the main control window*/
		if (sms_read_win)/*check window exists before destroying it*/
		{
			SmsRead_R_destroy(sms_read_win);
			sms_read_win = NULL;
		}

	callNumber(pb_data.number);
	
}

//x0035544 Mar 23, 2006 DR:OMAPS00047813 
/*******************************************************************************

 $Function:    	Search_forNumberInSMS

 $Description:    Search through the SMS text and find the numbers
			 
 $Returns:		void
 
 $Arguments:	UserData - SMS data info
*******************************************************************************/
void Search_forNumberInSMS(T_SmsUserData *UserData)
{
  int num_row, num_col, i;
  int len, sms_length = 0;
  char character;
  char *sms_buffer, *sender_num;
  BOOL mesg_end=FALSE;  
    
  TRACE_FUNCTION("Search_forNumberInSMS()");

  sms_buffer = (char*)UserData->TextBuffer;
  sender_num = (char*)UserData->NumberBuffer;
  
#ifdef NO_ASCIIZ
  if (sms_buffer[0] == (char)0x80)
  {
  #ifdef NEW_EDITOR /* SPR#1428 - SH - New Editor changes - new function to calculate length of unicode string */
      sms_length = ATB_string_UCLength((U16*)sms_buffer)*sizeof(USHORT);
  #else /* NEW_EDITOR */
      sms_length = strlenUnicode((U16*)sms_buffer);
  #endif /* NEW_EDITOR */
  }
  else
  {
    sms_length = strlen(sms_buffer);
  }
#else /* NO_ASCIIZ */
  sms_length = strlen(sms_buffer);
#endif /* NO_ASCIIZ */
  sms_length--;
  len= num_row=num_col=0;
  smsread_num_ctr =0;  
  num_buffer = (char**)ALLOC_MEMORY(sizeof(char*)*MAX_NUMBER_SIZE);
  if (num_buffer == NULL)
	{	
		TRACE_EVENT("sms number list alloc failed");
		return;
	}
  
  for(i=0; i<MAX_NUMBER_SIZE; i++)
  	num_buffer[i] = (char*)ALLOC_MEMORY(sizeof(char)*NUMBER_LENGTH);
		if (num_buffer[i] == NULL)
		{	
			TRACE_EVENT(" sms number string alloc failed");
			return; 
		}
  
      
    while (len <= sms_length)
  {
    character = sms_buffer[len];
	
	if(len == sms_length)
	 {
	    mesg_end = TRUE;
  	 }
	
#ifdef NO_ASCIIZ
		if (character=='+' ||  ( character>='0' && character<='9' ) || (character==0))
#else
		if (character=='+' ||  ( character>='0' && character<='9' ) )
#endif
    {	
      if (character != 0) /*MC, only copy non-Null chars*/
      {      
      if(num_col <= NUMBER_LENGTH)
      	{
         	num_buffer[num_row][num_col] = character;		
        	num_col++;
      	}
		if(mesg_end && (strlen(num_buffer[num_row]) >= MIN_NUM_LENGTH))//copy if number is at end of the message
		 {			 	
			smsread_num_ctr++;
			num_row++;
		 }		
      }
    }
    else
    {
      if(num_col != 0 )
      {
        num_buffer[num_row][num_col] = '\0';		
        if(strlen(num_buffer[num_row]) < MIN_NUM_LENGTH )
        {
          num_col=0;
        }  
        else
        {
          smsread_num_ctr++;
          num_row++;
          if( num_row >= MAX_NUMBER_SIZE)
            break;
          num_col = 0;
        }
      }
    }
	len++;
  } 	
		
  if( num_row < 10 )//copy the sender number
   {
    	memcpy((char*)num_buffer[num_row], (char*)sender_num, NUMBER_LENGTH);
	smsread_num_ctr++;
   }
   
  return ;
}

//Mar 23, 2006 DR:OMAPS00047813 x0035544
/*******************************************************************************

 $Function:     smsread_numlist_win_cb

 $Description: Window event handler

 $Returns:	MFW_EVENT_REJECTED: 
 			MFW_EVENT_CONSUMED:

 $Arguments:	

*******************************************************************************/
static int smsread_numlist_win_cb (MfwEvt e, MfwWin *w)    
{
  	TRACE_FUNCTION ("smsread_numlist_win_cb()");
  	switch (e)
  	{
    		case MfwWinVisible:  /* window is visible  */
    		break;
    		case MfwWinFocussed: /* input focus / selected   */
    		case MfwWinDelete:   /* window will be deleted   */

    		default:
      		return MFW_EVENT_REJECTED;
  	}
  	return MFW_EVENT_CONSUMED;
}

//Mar 23, 2006 DR:OMAPS00047813 x0035544
/*******************************************************************************

 $Function:     SmsRead_BuildNumList_Menu

 $Description: creates dynamic menu

 $Returns:

 $Arguments:

*******************************************************************************/
static T_MFW_HND SmsRead_BuildNumList_Menu(T_MFW_HND parent_window)
{
  T_SMSREAD_R *  data = (T_SMSREAD_R *)mfwAlloc(sizeof (T_SMSREAD_R));
  T_MFW_WIN  * win;
  
  TRACE_FUNCTION ("SmsRead_BuildNumList_Menu");
  
  /*
  	* Create window handler
  	*/
  
  data->win = win_create (parent_window, 0, E_WIN_VISIBLE, (T_MFW_CB)smsread_numlist_win_cb);
  if (data->win EQ NULL)
  {
  FREE_MEMORY( (void *)data, sizeof( T_SMSREAD_R ) );
  		return NULL; 
  }
  
  TRACE_EVENT("list holder window created: " );
  /*
  	* connect the dialog data to the MFW-window
  	*/
  
  data->mmi_control.dialog = (T_DIALOG_FUNC)smsread_numlist_exec_cb;
  data->mmi_control.data   = data;
  win                      = ((T_MFW_HDR *)data->win)->data;
  win->user                = (void *)data;
  data->parent             = parent_window;
  winShow(data->win);
  /*
  	* return window handle
  	*/
  
  	return data->win;
}

//Mar 23, 2006 DR:OMAPS00047813 x0035544
/*******************************************************************************

 $Function:     smsread_numlist_exec_cb

 $Description: callback function

 $Returns:

 $Arguments:

*******************************************************************************/
void smsread_numlist_exec_cb (T_MFW_HND win, USHORT event)
{
  T_MFW_WIN       * win_data;
  T_SMSREAD_R * data;    

  /* List is complete so present menu. */
  USHORT i;
  
  TRACE_FUNCTION("smsread_numlist_exec_cb");
 
		
  win_data = ((T_MFW_HDR *) win)->data; 
  data = (T_SMSREAD_R *)win_data->user;

 switch (event)
	{
	case E_INIT:				
  data->menu_list_data = (ListMenuData *)ALLOC_MEMORY(sizeof(ListMenuData));
  
  if(data->menu_list_data == 0)
  {
    TRACE_EVENT("Failed memory alloc 1 ");
    return;
  }
  /*SPR 2686, only allocate memory for the number of messages displayed*/	
  data->menu_list_data->List = (T_MFW_MNU_ITEM *)ALLOC_MEMORY( smsread_num_ctr * sizeof(T_MFW_MNU_ITEM) );//JVJE

  if(data->menu_list_data->List == 0)
  {
    TRACE_EVENT("Failed memory alloc 2");
    return;
  }
  
  for (i = 0; i < smsread_num_ctr; i++)
  {
    mnuInitDataItem(&data->menu_list_data->List[i]);
    data->menu_list_data->List[i].str  = (char *)num_buffer[i];
    TRACE_EVENT_P1("Menu Tags:%s", data->menu_list_data->List[i].str);
    data->menu_list_data->List[i].flagFunc = item_flag_none; 
  } 	
	
  data->menu_list_data->ListLength =smsread_num_ctr;
  data->menu_list_data->ListPosition = 1;
  data->menu_list_data->CursorPosition = 1;
  data->menu_list_data->SnapshotSize =smsread_num_ctr;
  data->menu_list_data->Font = 0;
  data->menu_list_data->LeftSoftKey = TxtSoftSelect;
  /*NM p032*/
  if(test_sim EQ TRUE)
    data->menu_list_data->RightSoftKey = TxtNull;
  else
    data->menu_list_data->RightSoftKey = TxtSoftBack;
  	
  data->menu_list_data->KeyEvents = KEY_ALL;
  data->menu_list_data->Reason = 0;
  data->menu_list_data->Strings = TRUE;
  data->menu_list_data->Attr   = (MfwMnuAttr*)&menuSearchNumListAttr;
  data->menu_list_data->autoDestroy    = FALSE;  
 	
  /* Create the dynamic menu window */
  listDisplayListMenu(win, data->menu_list_data, (ListCbFunc_t)SmsRead_numlist_cb ,0);
  break;
  default:
  	break;
 	}
  
  }

//Mar 23, 2006 DR:OMAPS00047813 x0035544
/*******************************************************************************

 $Function:     SmsRead_numlist_cb

 $Description: destroy lists and menu in case of back softkey or menu exit

 $Returns:

 $Arguments:

*******************************************************************************/
static void SmsRead_numlist_cb(T_MFW_HND Parent, ListMenuData * ListData)
{
	T_MFW_WIN			*win_data	= ((T_MFW_HDR *)Parent)->data;
	T_SMSREAD_R	*data		= (T_SMSREAD_R *)win_data->user;
	char *pb_number;
	int i;

	TRACE_FUNCTION ("SmsRead_numlist_cb()");


	if ((ListData->Reason EQ LISTS_REASON_BACK) ||
	     (ListData->Reason EQ LISTS_REASON_CLEAR) ||
	     (ListData->Reason EQ LISTS_REASON_HANGUP))
	{
		/*
		** Destroy the window --- Not sure if this is required! ... leave it out for now
		*/
		listsDestroy(ListData->win);

		/*
		** Free the memory set aside for ALL the devices to be listed.
		*/
		for(i=0; i<MAX_NUMBER_SIZE;i++)
  		 {  
  			FREE_MEMORY((U8*)num_buffer[i], sizeof(char)*NUMBER_LENGTH);
			num_buffer[i] = NULL;  		 	
		 }
	
		/*deallocate memory for the array of pointer to numbers*/
  			FREE_MEMORY((U8*)num_buffer, sizeof(char*)*MAX_NUMBER_SIZE);
	
		if (data->menu_list_data->List != NULL)
			{
			FREE_MEMORY ((void *)data->menu_list_data->List, (U16)((data->menu_list_data->ListLength) * 
						sizeof(T_MFW_MNU_ITEM)));
			}
		if (data->menu_list_data != NULL)
			{
			FREE_MEMORY ((void *)data->menu_list_data, sizeof (ListMenuData));
			}
	 	
		SmsRead_numlist_destroy(data->win);
		
		

	}
	 
	else if(ListData->Reason EQ LISTS_REASON_SELECT)
	  {			
		pb_number = ListData->List[ListData->ListPosition].str;		
		callNumber((UBYTE*)pb_number);		
		
	  }

	return;
}

//Mar 23, 2006 DR:OMAPS00047813 x0035544
/*******************************************************************************

 $Function:    	SmsRead_numlist_destroy

 $Description:	Destroy the Extr Number window.
 
 $Returns:		none

 $Arguments:	own_window - window handler
 
*******************************************************************************/

static void SmsRead_numlist_destroy(MfwHnd own_window)
{
	T_MFW_WIN     * win_data  = ((T_MFW_HDR *)own_window)->data;
	T_SMSREAD_R   * data = (T_SMSREAD_R *)win_data->user;

	TRACE_FUNCTION ("SmsRead_numlist_destroy");

	if (own_window == NULL)
	{
		return;
	}

	if (data)
	{
		win_delete(data->win);
		
		/*
		** Free Memory
		*/
		mfwFree((void *)data, sizeof (T_SMSREAD_R));
	}
}
#undef MMI_SMSREAD_C
