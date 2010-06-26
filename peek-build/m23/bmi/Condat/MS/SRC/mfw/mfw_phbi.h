/*
+--------------------------------------------------------------------+
| PROJECT:  GSM-MFW (8417)              $Workfile:: mfw_phbi.h       $|
| $Author:: Vo                          $Revision::  1              $|
| CREATED:  7.1.99                      $Modtime:: 12.01.00 11:26   $|
| STATE  :  code                                                     |
+--------------------------------------------------------------------+

	PURPOSE :  Internal types defination for phonebook management
    . 
*/ 

#ifndef DEF_MFW_PHBI
#define DEF_MFW_PHBI

/*
 *  PHB Control Block
 */
typedef struct
{
    T_MFW_EVENT     emask;          /* events of interest */
    T_MFW_EVENT     event;          /* current event */
    T_MFW_CB        handler;
    T_MFW_PHB_PARA  para;
} T_MFW_PHB;

EXTERN BOOL     phb_sign_exec            (T_MFW_HDR * hdr,
                                          T_MFW_EVENT event, 
                                          T_MFW_PHB_PARA *para);
EXTERN void     phb_ok_clck              (void);
EXTERN void     phb_error_clck           (void);
EXTERN T_MFW_PHB_TON phb_cvtTon          (T_ACI_TOA_TON ton);
EXTERN T_MFW_PHB_NPI phb_cvtNpi          (T_ACI_TOA_NPI npi);/*a0393213 warnings removal-return type changed from T_MFW_PHB_TON*/
EXTERN T_ACI_TOA_TON phb_ncvtTon         (T_MFW_PHB_TON ton);
EXTERN T_ACI_TOA_NPI phb_ncvtNpi         (T_MFW_PHB_NPI npi);/*a0393213 warnings removal-argument type changed from T_MFW_PHB_TON*/
EXTERN void     phb_read_upn_eeprom      (void);

#endif
