/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_str.c       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 1               $|
| CREATED: 03.08.00                     $Modtime:: 03.08.00 18:57   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_STR

   PURPOSE : string handling functions

   EXPORT  :

   TO DO   :

   $History:: mfw_str.c                                             $
*/

#define ENTITY_MFW

#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "mfw_str.h"



/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_STR            |
| STATE   : code                        ROUTINE : mfwStrChr          |
+--------------------------------------------------------------------+

  PURPOSE : find first occurence of char in string

*/

MfwChar* mfwStrChr (MfwChar* p, int c)
{
    while (*p && *p != c)
        p++;

    return (*p) ? p : 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_STR            |
| STATE   : code                        ROUTINE : mfwStrLen          |
+--------------------------------------------------------------------+

  PURPOSE : compute length of string

*/

int mfwStrLen (MfwChar *s)
{
    int i = 0;

    while (*s++)
        i++;
        
    return i;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_STR            |
| STATE   : code                        ROUTINE : mfwStrSiz          |
+--------------------------------------------------------------------+

  PURPOSE : get length of counted string

*/

int mfwStrSiz (MfwStr *s)
{
#ifdef MFW_COUNTED_STRINGS
    return s->len;
#else
    return mfwStrLen(s);
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_STR            |
| STATE   : code                        ROUTINE : mfwStrAt           |
+--------------------------------------------------------------------+

  PURPOSE : get substring of counted string

*/

MfwChar* mfwStrAt (MfwStr *s, int off)
{
#ifdef MFW_COUNTED_STRINGS
    return s->s+off;
#else
    return s+off;
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_STR            |
| STATE   : code                        ROUTINE : mfwStrCut          |
+--------------------------------------------------------------------+

  PURPOSE : cut off (shorten) counted string

*/

MfwChar* mfwStrCut (MfwStr* s, int n)
{
#ifdef MFW_COUNTED_STRINGS
    s->len-=n;
    s->s[s->len]=0;
    return s->s;
#else
    s[mfwStrLen(s)-n] = 0;
    return s;
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_STR            |
| STATE   : code                        ROUTINE : mfwStrMov          |
+--------------------------------------------------------------------+

  PURPOSE : move (copy) a string into a counted string (override)

*/

MfwChar* mfwStrMov (MfwStr* d, int o, MfwChar* s)
{
#ifdef MFW_COUNTED_STRINGS
    int i = mfwStrLen(s);
    memcpy(d->s+o,s,i*sizeof(MfwChar));
    d->len = i + o;
    d->s[o+i] = 0;
    return d->s;
#else
    int i = mfwStrLen(s);
    memcpy(d+o,s,i*sizeof(MfwChar));
    d[o+i] = 0;
    return d;
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_STR            |
| STATE   : code                        ROUTINE : mfwStrIns          |
+--------------------------------------------------------------------+

  PURPOSE : insert (copy) a string into a counted string (expand)

*/

MfwChar* mfwStrIns (MfwStr* d, int o, MfwChar* s)
{
#ifdef MFW_COUNTED_STRINGS
    int i = mfwStrLen(s);
    memmove(d->s+o+i,d->s+o,(mfwStrSiz(d)-o)*sizeof(MfwChar));
    memmove(d->s+o,s,i*sizeof(MfwChar));
    d->len += i;
    return d->s;
#else
    int i = mfwStrLen(s);
    memmove(d+o+i,d+o,(mfwStrSiz(d)-o)*sizeof(MfwChar));
    memmove(d+o,s,i*sizeof(MfwChar));
    return d;
#endif
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_STR            |
| STATE   : code                        ROUTINE : mfwStrApp          |
+--------------------------------------------------------------------+

  PURPOSE : append string to counted string

*/

MfwChar* mfwStrApp (MfwStr* s, MfwChar* c)
{
    return mfwStrMov(s,mfwStrSiz(s),c);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_STR            |
| STATE   : code                        ROUTINE : mfwStrRChr         |
+--------------------------------------------------------------------+

  PURPOSE : find last occurence of char in string

*/

MfwChar* mfwStrRChr (MfwChar* p, int c)
{
    int i = mfwStrLen(p);
    
    while (i && p[i-1] != c)
        i--;

    return (i) ? p + i - 1 : 0;
}
