/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_str.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 1               $|
| CREATED: 03.08.00                     $Modtime:: 03.08.00 18:23   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_STR

   PURPOSE : String types & constants

   EXPORT  :

   TO DO   :

   $History:: mfw_str.h                                             $
*/

#ifndef _DEF_MFW_STR_H_
#define _DEF_MFW_STR_H_


/* #define MFW_UNICODE_STRINGS   */       /* 16 bit character codes   */
/* #define MFW_COUNTED_STRINGS   */       /* strings with prefix len  */


#ifdef MFW_UNICODE_STRINGS
#define MfwChar short
#else
#define MfwChar char
#endif

#ifdef MFW_COUNTED_STRINGS
typedef struct
{
    short len;
    MfwChar s [];
} MfwStr;
#else
typedef MfwChar MfwStr;
#endif


                                        /* PROTOTYPES               */
MfwChar* mfwStrChr (MfwChar* p, int c);
int mfwStrLen (MfwChar *s);
int mfwStrSiz (MfwStr *s);
MfwChar* mfwStrAt (MfwStr *s, int off);
MfwChar* mfwStrCut (MfwStr* s, int n);
MfwChar* mfwStrMov (MfwStr* d, int o, MfwChar* s);
MfwChar* mfwStrIns (MfwStr* d, int o, MfwChar* s);
MfwChar* mfwStrApp (MfwStr* s, MfwChar* c);
MfwChar* mfwStrRChr (MfwChar* p, int c);

#endif
