/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_nm.h        $|
| $Author:: Es                          $Revision::  1              $|
| CREATED: 07.10.98                     $Modtime:: 20.01.00 18:48   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

  PURPOSE: Type definitions for network management

  HISTORY:
  
	May 31, 2007 DR: OMAPS00133201 x0pleela
	Description: MS keeps searching networks and doesn't list available PLMNs after 
				performing manual selection
	Solution : Added new event E_NM_ABORT_NETWORK whihc wil be sent to BMI to 
				destroy the n/w searching dialog

	Sep 18,2006    DRT: OMAPS0090268 x0061088(Prachi)
	Description : To check for Forbidden network (for deneid services),it takes long time.
	Solution : Function rAT_PercentCREG() has now be implemented to handle CREG_STAT_Denied,
	which sends nm_signal(NETWORK_FORBIDDEN_NETWORK) to Bmi . 

	Jun 23, 2006    DR: OMAPS00082374 x0pleela
   	Description: EONS Failures - unexpected output from the MMI: 2 carriers; on the top always T-Mobile 
   	Solution: a) Removed the function prototypes of set_opn_sel_done and get_opn_sel_done
   	
	June 02, 2006    DR: OMAPS00078005 x0021334
	Descripton: Test for NITZ (Network Identity and Timezone) support - No New time was displayed
	Solution: New Time/Date received from network is sent to MFW by lower layer by invoking 'rAT_PercentCTZV'
	function. This function has now been implemented to pass on the received values to MMI
	for user confirmation and RTC updation.
   
	Feb 15, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: Incorporated review comments
   			- Implemented prototypes of 2 new functions set_opn_sel_done() and 
   				get_opn_sel_done() to set and get the current status of the operator selection
			- Added T_MFW_NM_OPN opn_read_from as part of T_MFW_NETWORK_STRUCT
				structure to avoid use of global variable as per review comments    

	Feb 09, 2006    ER: OMAPS00065203 x0pleela
   	Description: Cingular Operator Name Prioritization implementation required
   	Solution: Added new enumeration T_MFW_NM_OPN
   			Added new event E_NM_OPN
   			Added prototype of newly incorporated functions
   	
  	July 14, 2005  REF: CRR 31916  x0021334
    	Description: rCI_PercentCOPS() and rAT_PercentCOPS() is not implemented
    	Solution: rAT_PercentCOPS() is implemented. 
*/

#ifndef DEF_MFW_NM
#define DEF_MFW_NM



/* #include "phb.h" RSA 20/12/01 - Removed */
#include"mfw_phb.h" //RSA 20/12/01 - Added
#ifdef FF_2TO1_PS
#include "prim.h"
#else
#include "p_mmreg.val"
#endif
/*
 * global constants
 */
#define NM_AUTOMATIC    0       /* automatic mode set */
#define NM_MANUAL       1       /* manual mode is set */


/* permit maximal value */

#define MAX_PLMN          12     /* maximal PLMN number */

#define LONG_NAME         26
#define SHORT_NAME        10
#define SP_NAME           17
#define NUMERIC_LENGTH    7     /* 3 digits MCC + 3 digits MNC + '\0' */

#define MFW_PLMN_AVAILABLE  0   /* a permitted PLMN */
#define MFW_PLMN_FORBIDDEN  1   /* a forbidden PLMN */

#define MFW_FULL_SERVICE      1      /* definition for services */
#define MFW_LIMITED_SERVICE   2
#define MFW_SEARCH_NETWORK    3
#define MFW_NO_SERVICE        0

typedef enum
{
  NEW_PREF_PLMN = 1,    /* Add a new entry to the end of the list        */
  CHANGE_PREF_PLMN,     /* Change the place of two entries               */
  INSERT_PREF_PLMN,     /* Add a new entry to the list at a given index  */
  REPLACE_PREF_PLMN,    /* Overwrite an existing entry  at a given index */
  DELETE_PREF_PLMN      /* Delete an entry from the list                 */
} T_MFW_PPLMN_MODE;

typedef enum
{
  NM_ERR,
  NM_OK
} T_MFW_NM_RETURN;

//x0pleela 08 Feb, 2006 ER: OMAPS00065203
typedef enum
{
  NM_ROM,			/* operator name read from ROM   */
  NM_EONS,			/* operator name read from EONS */
  NM_CPHS,			/* operator name read from ONS   */
  NM_NITZ,			/* operator name read from NITZ  */
  NM_NONE = 255 	/* no operator name                     */
}T_MFW_NM_OPN;

//x0pleel 8 Feb, 2006 ER: OMAPS00065203
#define E_NM_OPN 0x4000  /* Event sent by MFW when operator name is obtained */

/*
 * events are organised as bitmaps
 */
#define E_NM_FULL_SERVICE      0x0001
#define E_NM_NO_SERVICE        0x0002
#define E_NM_LIMITED_SERVICE   0x0004
#define E_NM_SEARCH_NETWORK    0x0008
#define E_NM_PLMN_LIST         0x0010
#define E_NM_NO_NETWORK        0x0020
#define E_NM_DEREGISTRATION    0x0040
#define E_NM_PREF_PLMN_LIST    0x0080
#define E_NM_CHANGE_PPLMN_OK   0x0100
#define E_NM_CHANGE_PPLMN_ERR  0x0200
#define E_NM_PPLMN_MEM         0x0400
#define E_MFW_NET_IND          0x0800
#define E_MFW_TIME_IND         0x1000
#define E_NM_CELL_SELECT		 0x1001
#define E_NM_BAND				 0x2000	/*SPR919 - SH - added */

//x0pleela 30 May, 2007  DR: OMAPS00133201
#define E_NM_ABORT_NETWORK	0x3000 //Aborted by network during manual PLMN search
#define E_NM_FORBIDDEN_NETWORK 0x9000 //Sep 18,2006    DRT: OMAPS0090268 x0061088(Prachi)
#define E_NM_ALL_SERVICES      /*0x0FFF*/ 0xFFFF /* SPR919 - SH - changed */
//Geetha OMAPS00137726
#define E_NM_PLMN_LIST_BOOTUP 0x8000
/* Structures */

/* Cell Reselection Structure API 16-04-02 */
typedef struct
{
	USHORT ci;		/* cell identification */
	USHORT lid;		/* location identification */
}T_MFW_CELL_STRUCT;
	
typedef struct  /* LONG NAME */
{
  UBYTE dcs;
  UBYTE len;                  /* length of string data        */
  UBYTE data[LONG_NAME];      /* string data                  */
} T_MFW_LNAME;

typedef struct  /* SHORT NAME */
{
  UBYTE dcs;
  UBYTE len;                  /* length of string data        */
  UBYTE data[SHORT_NAME];     /* string data                  */
} T_MFW_SNAME;

typedef struct  /* SERVICE PROVIDER NAME */
{
  UBYTE dcs;
  UBYTE len;               /* length of string data        */
  UBYTE data[SP_NAME];     /* string data                  */
  UBYTE dummy;
} T_MFW_SP_NAME;

typedef struct
{
    SHORT index;
#ifdef NO_ASCIIZ
    T_MFW_SP_NAME service_provider; /* service provider name */
	T_MFW_LNAME network_long;  /* operator name in long format */
    T_MFW_SNAME network_short; /* operator name in short format */
#else
    UBYTE service_provider[LONG_NAME]; /* service provider name */
    UBYTE network_long [LONG_NAME];  /* operator name in long format */
    UBYTE network_short[SHORT_NAME]; /* operator name in short format */
#endif
    UBYTE network_numeric[NUMERIC_LENGTH];  /* operator name in numeric format */
    UBYTE display_condition;
    UBYTE roaming_indicator;    /* PLMN in HPLMN or not */
    UBYTE forbidden_indicator;  /* PLMN is member of the forbidden PLMN list */
    UBYTE *fieldstrength;       /* fieldstrength of PLMN */
    //x0pleela 15 Feb, 2006   ER: OMAPS00065203
    //Added as part of this structure to avoid use of global variable as per review comments
    T_MFW_NM_OPN opn_read_from; /* Holds the source id from where operator name was read */
} T_MFW_NETWORK_STRUCT;

typedef struct {
    UBYTE count;                /* number of entries in perferred PLMN list */
    T_MFW_NETWORK_STRUCT *plmn;      /* plmn identifications */
} T_MFW_PREF_PLMN_LIST; /* prefered PLMN list*/

typedef struct
{
    UBYTE count;                /* number of available PLMNs */
    T_MFW_NETWORK_STRUCT plmn[MAX_PLMN];  /* plmn identifications */
} T_MFW_PLMN_LIST;

typedef struct
{
    UBYTE maxRcd;               /* maximum preferred PLMN records in SIM card */
    UBYTE usedRcd;              /* used preferred PLMN records in SIM card */
} T_MFW_PPLMN_MEM;

typedef struct /* data for network name */
{
  UBYTE dcs;
  UBYTE add_ci;
  UBYTE num_spare;
  UBYTE len;
#ifdef FF_2TO1_PS
  UBYTE data[MMREG_MAX_TEXT_LEN];
#else
  UBYTE data[MMR_MAX_TEXT_LEN];
#endif
} T_MFW_NET_NAME;

typedef struct /* data buffer for network name */
{
#ifdef FF_2TO1_PS
  UBYTE mcc[PS_SIZE_MCC];       /* mobile country code */
  UBYTE mnc[PS_SIZE_MNC_MAX];       /* mobile network code */
#else
  UBYTE mcc[SIZE_MCC];       /* mobile country code */
  UBYTE mnc[SIZE_MNC];       /* mobile network code */
#endif
  T_MFW_NET_NAME long_name;  /* Network name, long format */
  T_MFW_NET_NAME short_name; /* Network name, short format */
} T_MFW_NET_IND;

typedef struct /* data for timer zone */
{
  UBYTE year;
  UBYTE month;
  UBYTE day;
  UBYTE hour;
  UBYTE minute;
  UBYTE second;
  UBYTE timezone;
} T_MFW_TIME_IND;

/*
 * Network Parameter
 */
typedef union
{
  T_MFW_NETWORK_STRUCT ident;
  T_MFW_PLMN_LIST  plmns;
  T_MFW_PREF_PLMN_LIST pplmns;
  T_MFW_PPLMN_MEM pplmn_mem;
  T_MFW_NET_IND net_info;
  T_MFW_TIME_IND tim_info;
  T_MFW_CELL_STRUCT cell_info;	/* sbh 29-04-02 - Cell reselection */
} T_MFW_NM_PARA;



/*
 * Prototypes
 */

int nm_init (void);
MfwHnd nm_create (MfwHnd hWin, MfwEvt event, MfwCb cbfunc);
MfwRes nm_delete (MfwHnd hWin);
void nm_set_mode (U8 mode);
T_MFW nm_get_mode (void);
T_MFW nm_reg_status (void);
void nm_select_plmn (U8 *network);
void nm_avail_plmn_list (void);
void nm_home_plmn (T_MFW_NETWORK_STRUCT *hplmn);
int nm_plnm_list(T_MFW_NETWORK_STRUCT *plmn_list, int start, int count);

int nm_plnm_list_longnames(char**plmn_ln_list, int start, int count); /*SPR 2635.*/

void nm_pref_plmn_list(T_MFW_NETWORK_STRUCT *pplmn, UBYTE count, SHORT start);
T_MFW_NM_RETURN nm_change_pref_plmn(T_MFW_PPLMN_MODE mode,
                                    SHORT index, SHORT index2,
                                    UBYTE *network_numeric);

T_MFW_NM_RETURN nm_change_pref_plmn_longname(T_MFW_PPLMN_MODE mode,/*SPR 2635.*/
                                    SHORT index, SHORT index2,
                                    char* network_long);

T_MFW nm_registration (U8 mode, U8 freq_bands, U8 limited);
void nm_deregistration (void);
void nm_abort_avail_plmn(void);
void nm_exit (void);
T_MFW_NM_RETURN nm_pplmn_mem_req(void);

//x0pleela 09 Feb, 2006  ER: OMAPS00065203
//Conversion from ACI souce Id type to MFW souceid type
#ifndef NEPTUNE_BOARD
int nm_decodeSourceId( UBYTE sourceId);
#endif
//To update plmn data after querying COPN
void nm_update_plmn_data( T_MFW_NETWORK_STRUCT *plmn_ident, 
							    T_ACI_OPER_NTRY oper_ntry);  
//To query COPN
void nm_get_COPN( T_MFW_NETWORK_STRUCT *plmn_ident );
//Get operator name from other sources (CPHS or NITZ or ROM)
void nm_get_opername(T_MFW_NETWORK_STRUCT  * plmn_ident);
//gets operator name from network
void nm_get_network_data(T_MFW_NETWORK_STRUCT *plmn_netw_data);
// June 02, 2006    DR: OMAPS00078005 x0021334
// Function to put the PCTZV mode on
#ifdef FF_TIMEZONE
void nm_get_network_time(void);

// June 02, 2006    DR: OMAPS00078005 x0021334
// Function to set network time
void nm_set_network_time(T_MFW_TIME_IND *currclock);
#endif 

#ifdef NO_ASCIIZ
void nm_get_spn (T_MFW_SP_NAME *spn);
#else
/* PATCH PMC 000804 */
void nm_get_spn (char *);
/* END PATCH PMC */
#endif
void nm_abort_registraion(void);

// July 14, 2005  REF: CRR 31916  x0021334
// Prototype for new common function 'nm_Plus_Percent_COPS()'
void nm_Plus_Percent_COPS (S16 last_ind, T_ACI_COPS_OPDESC *operLst);
/* Added to remove warning */
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);

#endif
/* Aug 07, 2007 DR:OMAPS00137726 x0066814(Geetha)*/
void nm_get_plmn_list_during_bootup(void);

