/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (?)                  $Workfile:: mfw_sim.h       $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 14.10.98                     $Modtime:: 20.03.00 14:09   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+
 History:

	Apr 03, 2007    ER: OMAPS00122561  x0pleela
	Description:   [ACI] Phone lock feature has to be supported by ACI
	Solution: Phone Lock ER implementation
    
   	Sep 26, 2006 DR: OMAPS00095524 x0pleela
 	Description:Implement 2 missing SIM Lock types
 	Solution: Made the following changes
 			- Added new macros MFW_SIM_PBLOCK_REQ, MFW_SIM_PBLOCK for
 				Blocked Network category
 				
	xashmic 9 Sep 2006, OMAPS00092732
	USBMS ER
  	Sep 08, 2006 DR: OMAPS00091250 x0pleela
 	Description:The phone resets if no MEPD data is presented
 	Solution: Added a new event MFW_MEPD_INVALID which will be returned to BMI
 			to display MEPD not present to the user

	July 21, 2006 ER: OMAPS00087586, OMAPS00087587 x0pleela
	Description: 	OMAPS00087586: MasterKey funtion in TCS3.2 SW
				OMAPS00087587: Time Penaly feature extend handset security
	Solution:Made the following changes
	 	Defined new macros MFW_SIM_MKEY( Master Unlocking during bootup)
	 					  MFW_SIM_MKEYM (Master Unlocking through menu)
 	
	May 26, 2006 ER: OMAPS00079607 x0012849 :Jagannatha M
	Description: CPHS: Display 'charge/Timers' menu based on AOC ( Advice of charge )
	Solution:Made the following changes
		       1. Defined MFW_SIM_NO_AOC with value 11
			
	May 15, 2006 DR: OMAPS00067919 x0pleela
	Description: SIMP:On repeat Depersonalisation with invalid password MEPD unblock key should 
				be asked not Puk1.
	Solution:Made the following changes
		       1. Added new macro BOOTUP_OK
			2. Added new enumeration T_MFW_SIMP_CLCK_FLAG
			
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
  			  
***********************************************************************

  PURPOSE: Types definitions for SIM management of MMI framework

*/

#ifndef DEF_MFW_SIM
#define DEF_MFW_SIM 

/*#ifndef  BMI_TEST_MC_SIM_EVENT
#define BMI_TEST_MC_SIM_EVENT
#endif*/

/*
 * events are organised as bitmaps
 */
#define E_SIM_STATUS         	0x0001
#define E_SIM_INSERTED       	0x0002
#define E_SIM_RESET          	0x0004
#define E_SIM_READ_CALLBACK		0x0008/*MC CONQ5862, 13/06/02*, added new event*/

#define E_SIM_ALL_SERVICES  0x000f

#define SIM_ACTIVE                0
#define SIM_NOT_ACTIVE            1
#define SIM_STK_RESET             2 // bugfix for SIM refresh
#define IMEI_NOT_VALID            3 // Nov 24, 2005, a0876501, DR: OMAPS00045909

typedef struct {
  UBYTE sim_procedure;
  UBYTE sim_status;
  UBYTE sim_operation_mode;
  UBYTE sim_pin_retries;
  UBYTE sim_status_type;
} T_MFW_SIM_STATUS; /* type definition for E_SIM_STATUS event */

typedef struct {
  UBYTE type;           /* MFW_SIM_PIN1/MFW_SIM_PIN2                       */
  UBYTE set;            /* MFW_SIM_ENABLE/MFW_SIM_DISABLE                  */
  UBYTE stat  ;         /* MFW_SIM_NO_PIN/MFW_SIM_PIN_REQ/MFW_SIM_PIN2_REQ */
} T_MFW_SIM_PIN_STATUS;

/*MC CONQ5862, 13/06/02*, added new data struct for E_SIM_READ_CALLBACK event*/
typedef struct {
	SHORT error_code;
	UBYTE* read_buffer;
} T_MFW_READ_CALLBACK;

/* SIM configuration information */
typedef struct
{
    UBYTE access_acm;
    UBYTE access_acmmax;
    UBYTE access_puct;
} T_MFW_AOC_ACCESS;

//x0pleela 07 Mar, 2007  ER: OMAPS00122561
#ifdef FF_PHONE_LOCK
#define MFW_PH_LOCK_ENABLE     	1
#define MFW_PH_LOCK_DISABLE	2
#define MFW_PH_LOCK_FAILURE 	3

#define MFW_PH_LOCK 			1 	/* Phone lock type 		*/
#define MFW_AUTO_PH_LOCK		2 	/* Auto Phone lock type	*/
#endif /* FF_PHONE_LOCK */

#ifdef SIM_PERS
#define UNBLOCK_SUCESS 1
#define UNBLOCK_FAILURE 2
#define BOOTUP_LOCK_SUCCESS 3
#define BOOTUP_LOCK_FAILURE 4
#define BOOTUP_OK 5		//x0pleela 21 Apr, 2006  DR: OMAPS00067919
#define MFW_SIM_NOT_BLOCKED  10
#define MFW_SIM_PERM_BLOCKED 11
#endif

/* definition of SIM procedure in E_SIM_STATUS event */

#define MFW_SIM_ACTIVATION 1
#define MFW_SIM_VERIFY     2
#define MFW_SIM_UNBLOCK    3
#define MFW_SIM_CHANGE     4
#define MFW_SIM_ENABLE     5
#define MFW_SIM_DISABLE    6
#define MFW_SIM_LOCKED     7
#define MFW_SIM_BLOCKED    8
#define MFW_SIM_REMOVED    9



/* definition of SIM operation mode in E_SIM_STATUS event */

#define MFW_SIM_NO_OPERATION    0
#define MFW_SIM_ADN_ENABLED     1
#define MFW_SIM_FDN_ENABLED     2
#define MFW_SIM_ADN_BDN_ENABLED 5
#define MFW_SIM_FDN_BDN_ENABLED 6

/* definition of SIM status in E_SIM_STATUS event */

#define MFW_SIM_PIN_REQ      1
#define MFW_SIM_PUK_REQ      2
#define MFW_SIM_PIN2_REQ     3
#define MFW_SIM_PUK2_REQ     4
#define MFW_SIM_INVALID_CARD 5
#define MFW_SIM_NO_SIM_CARD  6
#define MFW_SIM_NO_PIN       7
#define MFW_SIM_SUCCESS      8
#define MFW_SIM_FAILURE      9
#ifdef SIM_PERS
	#define MFW_SIM_NLOCK_REQ		10
	#define MFW_SIM_NSLOCK_REQ	11
	#define MFW_SIM_SPLOCK_REQ	12
	#define MFW_SIM_CLOCK_REQ		13
	#define MFW_SIM_PLOCK_REQ		14
	#define MFW_SIM_PLOCK_ERR		15
	#define MFW_SIM_PLOCK_OK		16
	#define MFW_MEPD_INVALID		17 //x0pleela 23 Aug, 2006 DR: OMAPS00091250
	#define MFW_SIM_PBLOCK_REQ	18 //x0pleela 25 Sep, 2006 ER: OMAPS00095524
#endif
#define MFW_SIM_NO_DISABLE	10  /*MC CONQ-5578, PIN disabling forbidden*/
//x0012849: Jagannatha  26 May, 2006  ER: OMAPS00079607 -Support AOC in CPHS
 #ifdef FF_CPHS
#define MFW_SIM_NO_AOC		11 
#endif
/* Shashi Shekar B.S. DR: OMAPS00045909, Improve IMEI control mechanism */
#define MFW_IMEI_NOT_VALID	20  


#define MFW_SIM_PIN1         1
#define MFW_SIM_PUK1         2
#define MFW_SIM_PIN2         3
#define MFW_SIM_PUK2         4
#define MFW_SIM_UNKNOWN      0

#define MFW_SIM_NLOCK        1
#define MFW_SIM_SPLOCK       2
#define MFW_SIM_NSLOCK       3
#define MFW_SIM_CLOCK        4
#define MFW_SIM_PLOCK        5
#define MFW_SIM_UNLOCK_ERR   6
#ifdef SIM_PERS
#define MFW_SIM_FC 7
#define MFW_SIM_FCM 8

//x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587

#define MFW_SIM_MKEY	9				//Master Unlocking during bootup
#define MFW_SIM_MKEYM	10				//Master Unlocking through menu
#define MFW_MASTER_UNLOCK_ACTIVE 1 	//Master unlock enabled
#define MFW_MASTER_UNLOCK_DEACTIVE 0	//Master unlock disabled


 //x0pleela 25 Sep, 2006 ER: OMAPS00095524
 #define MFW_SIM_PBLOCK        11			//Blocked Network Type

#endif

//x0pleela 27 Apr, 2006  DR: OMAPS00067919

#ifdef SIM_PERS
typedef enum
{
  SIMP_NONE,
  SIMP_MENU,
  SIMP_BOOTUP  
}T_MFW_SIMP_CLCK_FLAG;

#endif

/*
 * SIM Parameter
 */
typedef union
{
  T_MFW_SIM_STATUS status;
  T_MFW_READ_CALLBACK callback;
} T_MFW_SIM_PARA;

#ifdef SIM_PERS
extern int C_KEY_REQ;
#endif

int       sim_init                 (void); 
MfwHnd    sim_create               (MfwHnd hWin, MfwEvt event, MfwCb cbfunc);
MfwRes    sim_delete               (MfwHnd h);
void      sim_verify_pin           (UBYTE pin_id, char *pin);
void      sim_unblock_pin          (UBYTE pin_id, char *puk, char *newpin);
void      sim_change_pin           (UBYTE pin_id, UBYTE *alt_pin, UBYTE *new_pin);
T_MFW      sim_disable_pin          (UBYTE *pin);/*SPR 2145, now returns status*/
T_MFW      sim_enable_pin           (UBYTE *pin);/*SPR 2145, now returns status*/
void      sim_exit                 (void);
int       sim_pin_count            (UBYTE pin_id);
int 	SimHasPin				(U8 pin_id);
void      sim_pin_status           (T_MFW_SIM_PIN_STATUS *status);
T_MFW     sim_serv_table_check     (UBYTE serv_num);
int       sim_check_sim_Plock      (void);
int       sim_check_sim_Nlock      (void);
int       sim_check_sim_SPlock     (void);
int       sim_check_sim_NSlock     (void);
int       sim_check_sim_Clock      (void);
MfwRes    sim_configuration        (UBYTE *phase, UBYTE *serv, UBYTE *slen,
                                    UBYTE *lang, UBYTE *llen,
                                    T_MFW_AOC_ACCESS *access);
void      sim_save_pref_lang       (UBYTE *lang, UBYTE len);
T_MFW     sim_unlock_sim_lock      (UBYTE type, UBYTE *passwd);
void      sim_activate             (void);
UBYTE   * sim_get_imsi             (void);
UBYTE     sim_simlock_ok           (void);

void mfwSimRestartPinEntry(void);
//xashmic 9 Sep 2006, OMAPS00092732
void sim_enable(void);
void sim_disable(void);

/* 
	Ciphering
*/

// CPRImode :
#define MFW_SIM_CPRI_NO_SHOW      0
#define MFW_SIM_CPRI_SHOW        	1

//Callback function
typedef void (* CallbackCPRI)(UBYTE, UBYTE);  

//Initialize the ciphering indication
EXTERN UBYTE sim_init_CPRS (CallbackCPRI func, UBYTE CPRImode );
//TISH, patch for ASTec32515, modified by Jinshu Wang, 2007-06-15
//start
EXTERN void sim_invalid_ind(void);
//end
//x0pleela 03 Aug, 2006 ER: OMAPS00087586, OMAPS00087587
#ifdef SIM_PERS
//Set Simp_Busy_State to either TRUE or FALSE based on the ACI's busy error
void mfw_simlock_set_busy_state( UBYTE state );
//Get the current value of Simp_Busy_State
UBYTE mfw_simlock_get_busy_state( void );
#endif

#endif
