/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_lng.c       $|
| $Author:: Kk  $ CONDAT GmbH           $Revision:: 3               $|
| CREATED: 27.08.99                     $Modtime:: 14.03.00 13:59   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_LNG

   PURPOSE : language handling functions

   EXPORT  :

   TO DO   :

   $History:: mfw_lng.c                                             $
 * 
 * *****************  Version 3  *****************
 * User: Kk           Date: 14.03.00   Time: 15:48
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * added handling of various texts
 *
 * *****************  Version 2  *****************
 * User: Kk           Date: 10.01.00   Time: 14:03
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 3  *****************
 * User: Rm           Date: 12/22/99   Time: 4:34p
 * Updated in $/GSM/Condat/SND-MMI/MFW
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 22.11.99   Time: 10:29
 * Updated in $/GSM/Condat/SND-MMI/MFW
 *
 * *****************  Version 1  *****************
 * User: Es           Date: 18.11.99   Time: 16:35
 * Created in $/GSM/Condat/SND-MMI/MFW
 * Initial
*/

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

#include <stdio.h>

#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "drv_key.h"
#include "mfw_kbd.h"
#include "mfw_lng.h"


static int lngCommand (U32 cmd, void *h);

EXTERN MfwHdr * current_mfw_elem;


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_LNG            |
| STATE   : code                        ROUTINE : lngInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize language handler

*/

MfwRes lngInit (void)
{
    mfwCommand[MfwTypLng] = (MfwCb) lngCommand;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_LNG            |
| STATE   : code                        ROUTINE : lngExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize language handler

*/

MfwRes lngExit (void)
{
    mfwCommand[MfwTypLng] = 0;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_LNG            |
| STATE   : code                        ROUTINE : lngCreate          |
+--------------------------------------------------------------------+

  PURPOSE : create language control

*/

MfwHnd lngCreate (MfwHnd w, MfwEvt e, LngInfo *l, MfwCb f)
{
    MfwHdr *hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    MfwLng *lng = (MfwLng *) mfwAlloc(sizeof(MfwLng));
	MfwHdr *insert_status =0;
	
    if (!hdr || !lng)
    {
    	TRACE_ERROR("ERROR: lngCreate() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));
   		if(lng)
   			mfwFree((U8*)lng,sizeof(MfwLng));	
   		
	   	return 0;
  	}

    lng->map = e;
    lng->evt = 0;
    lng->handler = f;
    lng->lng = l;

    hdr->data = lng;
    hdr->type = MfwTypLng;

    insert_status = mfwInsert(w,hdr);
    if(!insert_status)
	{
  		TRACE_ERROR("ERROR: lngCreate() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)lng ,sizeof(MfwLng));
		return 0;
  	}
    return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_LNG            |
| STATE   : code                        ROUTINE : lngDelete          |
+--------------------------------------------------------------------+

  PURPOSE : delete language control

*/

MfwRes lngDelete (MfwHnd h)
{
    MfwRes res;

    if (!h)
        return MfwResIllHnd;

    res = (mfwRemove(h)) ? MfwResOk : MfwResIllHnd;

    mfwFree(((MfwHdr *) h)->data,sizeof(MfwLng));
    mfwFree(h,sizeof(MfwHdr));

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_LNG            |
| STATE   : code                        ROUTINE : lngText            |
+--------------------------------------------------------------------+

  PURPOSE : get prmpt text in current language

*/

char* lngText (MfwHnd h, unsigned int t)
{
    char *err = "lngText: Fatal";
    MfwHdr *hp = h;
    LngInfo *lc;
    const char* const* lt;

    if (!hp || hp->type != MfwTypLng)
        return err;

    lc = ((MfwLng *) (hp->data))->lng;

    if (!lc || !lc->prompts)
        return err;

    if (t >= lc->nPrompts)
        return ((char *) t);

    lt = lc->prompts[lc->language];

    return ((char *) (lt[t]));
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_LNG            |
| STATE   : code                        ROUTINE : sigExec            |
+--------------------------------------------------------------------+

  PURPOSE : execute language control related signal

*/

static int sigExec (MfwHdr *curElem, U32 event/*, U8 key*/)
{
    MfwLng *lc;

    while (curElem)
    {
        if (curElem->type == MfwTypLng)
        {
            lc = curElem->data;
            if (lc->map & event)
            {
                if (lc->handler)        /* handler valid            */
                {
                  // PATCH LE 06.06.00
                  // store current mfw elem
                  current_mfw_elem = curElem;
                  // END PATCH LE 06.06.00
                  if ((*(lc->handler))(event,lc))
                    return 1;       /* event consumed           */
                }
            }
        }
        curElem = curElem->next;
    }

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_LNG            |
| STATE   : code                        ROUTINE : lngSignal          |
+--------------------------------------------------------------------+

  PURPOSE : language control related event

*/

void lngSignal (void /*char make, char key*/)
{/*MC, SPR 1389, we have to enable the display whenever 
	we send an event up to the MMI*/
  	UBYTE temp = dspl_Enable(0);
    U32 event = 0;  // RAVI

    if (mfwFocus)
        if (sigExec(mfwFocus,event/*,(U8) key)*/))
        { dspl_Enable(temp);/*MC, SPR 1389*/
	        return;
	 	}
    if (mfwRoot)
        sigExec(mfwRoot,event/*,(U8) key*/);
    
   dspl_Enable(temp);/*MC, SPR 1389*/
   return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_LNG            |
| STATE   : code                        ROUTINE : lngCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int lngCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            lngDelete(h);
            return 1;
        default:
            break;
    }

    return 0;
}

