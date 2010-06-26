/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (?)                  $Workfile:: mfw_nmi.h       $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED: 07.10.98                     $Modtime:: 21.10.99 12:18   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

	PURPOSE :  Internal definitions for network management
	
    History

   	May 31, 2007 DR: OMAPS00133201 x0pleela
	Description: MS keeps searching networks and doesn't list available PLMNs after 
			performing manual selection
	Solution : Modified the prototype of function  nm_error_cops()
*/ 

#ifndef DEF_MFW_NMI_HEADER
#define DEF_MFW_NMI_HEADER

#define MAX_PREF_PLNM     8

#define MAX_PIN_COUNT     3      /* permit maximal entering for user */
#define MAX_PUK_COUNT     5
#define FIRST_SIM_INVAILD 1

#define SERVICE_OK        1
#define SERVICE_FAILURE   0

#define CMD_NOT_PRESENT   0


/*
 *  Network Control Block
 */
typedef struct
{
    T_MFW_EVENT   emask;          /* events of interest */
    T_MFW_EVENT   event;          /* current event */
    T_MFW_CB      handler;
    T_MFW_NM_PARA para;
} T_MFW_NM;
 

EXTERN T_MFW_HND nm_install              (T_MFW_HDR              *w, 
                                          T_MFW_HDR              *h, 
                                          T_MFW_NM               *nm);
EXTERN T_MFW_RES nm_remove               (T_MFW_HDR              *h);


EXTERN void     nm_activate              (UBYTE result);
#ifdef FF_2TO1_PS
EXTERN void     nm_mmi_parameters        (T_SIM_imsi_field *imsi_field, 
                                          T_SIM_pref_plmn  *pref_plmn);
#else
EXTERN void     nm_mmi_parameters        (T_imsi_field *imsi_field);
#endif
EXTERN void     nm_spn_cnf               (USHORT error, T_EF_SPN *efspn);
EXTERN void     nm_ok_cops               (void);
//x0pleela 30 May, 2007  DR: OMAPS00133201
//Passing T_ACI_CME_ERR as parameter
EXTERN void     nm_error_cops            ( T_ACI_CME_ERR err );
EXTERN void     nm_req_imsi              (UBYTE *imsi_id);
EXTERN T_MFW    nm_reg_flag_req          (void);
EXTERN void     nm_ok_deregistration     (void);
EXTERN void     nm_ok_pref_plmn          (void);
EXTERN void     nm_error_pref_plmn       (void);
EXTERN void     nm_sat_file_update       (USHORT dataId);
EXTERN void		nm_ok_band						(void);		/*SPR919 - SH - added */

#endif

