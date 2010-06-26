/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_acie.h      $|
| $Author:: Es                          $Revision::  1              $|
| CREATED: 26.10.98                     $Modtime:: 2.03.00 15:24    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_ACIE

   PURPOSE : This modul contains the definition for ACI external functions.


   $History:: mfw_acie.h                                              $
 * 
 * *****************  Version 7  *****************
 * User: Es           Date: 2.03.00    Time: 16:04
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * some cleanups
 * 
 * *****************  Version 6  *****************
 * User: Es           Date: 8.12.98    Time: 16:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 5  *****************
 * User: Vo           Date: 24.11.98   Time: 21:40
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * first successful compilation of MFW
 *
 * *****************  Version 4  *****************
 * User: Le           Date: 11.11.98   Time: 13:35
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 3  *****************
 * User: Le           Date: 10.11.98   Time: 16:07
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 2  *****************
 * User: Le           Date: 10.11.98   Time: 11:08
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 1  *****************
 * User: Le           Date: 27.10.98   Time: 15:58
 * Created in $/GSM/DEV/MS/SRC/MFW
 * first implementation

*/

#define MAX_ACI_HANDLER 10

typedef  BOOL (*T_PRIM_HANDLER)(ULONG, void *);
typedef  BOOL (*T_CMD_HANDLER)(char *);

typedef struct
{
  T_PRIM_HANDLER prim_handler;
  T_CMD_HANDLER  cmd_handler;
} T_MFW_ACI_EXT;

EXTERN void aci_ext_init(void);
EXTERN int  aci_create(T_PRIM_HANDLER prim_func, T_CMD_HANDLER cmd_func);
EXTERN void aci_delete(int handle);
EXTERN BOOL aci_check_primitive(ULONG opc, void * data);
EXTERN BOOL aci_check_command(char * cmd);
#ifdef BT_INTERFACE
EXTERN int pSlotBt;               /* primitive handler slot   */
#endif
