#ifndef _DEF_MMI_SMSBROADCAST_H_
#define _DEF_MMI_SMSBROADCAST_H_
/*
   MODULE  : MMI
   PURPOSE : header of template for MMI smscb dynamic event handling

   EXPORT  :

   TO DO   :
 * *****************  Version 1  *****************
 * User: RSA           Date: 04.08.00   Time: 19:50
 * Created in $/GSM/Condat/SND-MMI/MMI
 * Initial
*/

#include "mfw_mfw.h"
#include "MmiBookShared.h" //GW-SPR#762 For definition of NUMBER_LENGTH

#define SMSCB_SWITCH_ON  0
#define SMSCB_SWITCH_OFF 1

//GW-SPR#762 - removed #define NUMBER_LENGTH    20       /* count of digits to be transferred to phonebook */
#define NO_MID 0xFFFF         /* Invalid MID number.      */

#ifdef FF_2TO1_PS
#define MAX_STORED_PAGES CBM_MAX_RANGE_NUMBER
#else
#define MAX_STORED_PAGES MAX_IDENTS
#endif
#define CB_PAGE_LENGTH 93
#define CB_TAG_LENGTH 10
#define CB_MID_LENGTH 4

#define MAX_MIDS MAX_STORED_PAGES
#define CB_STORE_LENGTH ((MAX_STORED_PAGES * CB_PAGE_LENGTH) + 1)            /* 25 pages of 93 characters + 1 */

#define IMMEDIATE_CELL_WIDE  0x0000
#define NORMAL_PLMN_WIDE     0x4000
#define NORMAL_LOCATION_WIDE 0x8000
#define NORMAL_CELL_WIDE     0xC000

typedef enum
{
    NO_MESSAGE,
    UNREAD_MESSAGE,
    READ_MESSAGE,
    ALL_MESSAGE,
    CORRUPT_MESSAGE
}T_SMSCB_STATUS;


//T_MFW_HND smscb_M_CB_start(T_MFW_HND parent_window, void *dummy);
void smscb_InitData (void);
SHORT smscb_get_new_cb_messages_count(void);
T_MFW smscb_getCBsettings(void);
/*SPR 1920, removed cellbroadcast_start() header*/
int temp_smscb_M_CB_ExeRead (MfwMnu* m, MfwMnuItem* i);

/*MC SPR 1920 moved from .c file */
int smscb_M_CB_TOP_ExeConsult(MfwMnu* m, MfwMnuItem* i);
int smscb_M_CB_TOP_ExeAddNew(MfwMnu* m, MfwMnuItem* i);
int smscb_M_CB_ACT_ExeNo(MfwMnu* m, MfwMnuItem* i);
int smscb_M_CB_ACT_ExeYes(MfwMnu* m, MfwMnuItem* i);
int sms_cb_select_read (MfwMnu* m, MfwMnuItem* i);

void sms_cb_init (void);
void sms_cb_exit (void); /*MC SPR 1920, added new function*/
void sms_new_incoming_cb (T_MFW_SMS_CB *Message);
int sms_cb_read_msg (T_SMSCB_STATUS status);

void info_screen (T_MFW_HND win, USHORT TextId, USHORT TextId2,T_VOID_FUNC  Callback);
#define THREE_SECS  0x0BB8             /* 3000 milliseconds.                  */
#define FIVE_SECS   0x1388             /* 5000 milliseconds.                  */
#define TEN_SECS    0x2710             /* 10000 milliseconds.                 */
#define TWO_MIN     0x1D4C0            /* 2 min                               */
#define FOREVER     0xFFFF             /* Infinite time period.               */

/* SPR759 - SH - moved from .c file */

/*********************************************************************
**********************************************************************

			cell broadcast fifo & lists declaration

*********************************************************************
**********************************************************************/



/* Stored Cell Broadcast message attributes. */
typedef struct
{
    SHORT           msg_id;              /* message identification          */
    T_SMSCB_STATUS status;              /* Read status of this message.    */
    CHAR            name[CB_TAG_LENGTH]; /* Alphanumeric message tag.       */
    CHAR          * text;	         /* Pointer to cell broadcast msg   */
} T_SMS_CB_ATT;

/* Cell Broadcast message list. */
typedef struct 
{
    int length; /* actual number of entries */
    T_SMS_CB_ATT entry[MAX_MIDS];
} T_SMSCB_LIST;

/* Cell Broadcast topics attributes. */
typedef struct 
{
    USHORT       msg_id;              /* Message identifier.             */
    SHORT        dcs;                 /* data coding scheme              */
    char         name [CB_TAG_LENGTH];/* Alphanumeric message tag.       */
} T_SMSCB_TOPICS_ENTRY;

/* Cell Broadcast topics list. */
typedef struct 
{
    int length; /* actual number of entries */
    T_SMSCB_TOPICS_ENTRY entry[MAX_MIDS];
} T_SMSCB_TOPICS;

/*SPR 1920, removed defn of T_SMSCB_CLASS_0_MSG, not used*/
/* class 0 Cell Broadcast Message */
/* storage for Cell Broadcast lists */
typedef struct 
{
    T_SMSCB_TOPICS      CBTopics;                         /* list of topics to be received */
    /*SPR 1920 removed SmsCbInfo, smsCBList, and Class0CBMessage from structure*/
} T_SMSCB_DATASTORE;

//end; will be delete later


////////////////////////////////////////////////////////////////////////////////////////

	// The CB fifo is organized over the
	// link list. The "used_pointer" is giving the
	// starting point of the CB-fifo list. The "next_xxx"
	// is giving the link to the next pointer and so on.
	// used_pointer = FF means ->no cb messages stored 
	// next_used   = FF means ->end of the cb messages list.
	// free_pointer = FF means ->no free spaces !
	// next_used   = FF means ->end of the free spaces list

/* Stored Cell Broadcast message attributes. */
typedef struct
{
    SHORT           sn;                  /* serial number in integer format */
    USHORT          mid;              /* message identification          */
    UBYTE           dcs;                 /* data coding scheme              */
    UBYTE           page;                /* number of this page             */
    UBYTE           pages;              	 /* number of total pages           */
    UBYTE           msg_len;        	    /* length of short message         */
    T_SMSCB_STATUS status;       	       /* Read status of this message.    */
  	/*SPR 1920, removed name field*/
	UBYTE 	    	 next_link;
	/*SPR 1920, memory for header string now dynamically allocated as needed*/
	char*			 header/*[25]*/; //header information for the menu list
	UBYTE 	    	 start_page; //indicate the first page of the multipage; need for the menu list
} T_SMSCB_ATT;

T_MFW smscb_setCBsettings(UBYTE State);

#endif /* _DEF_MMI_SMSBROADCAST_H_ */
