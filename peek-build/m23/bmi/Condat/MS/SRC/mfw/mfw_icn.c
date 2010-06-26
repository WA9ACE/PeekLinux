/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_icn.c       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 6               $|
| CREATED: 24.11.98                     $Modtime:: 23.03.00 8:36    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_ICN

   PURPOSE : icon handling functions

   EXPORT  :

   TO DO   :

   $History:: mfw_icn.c                                             $
 * 
 * *****************  Version 6  *****************
 * User: Es           Date: 23.03.00   Time: 14:41
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added icnUnhide(); removed 'update()' from 'hide()' and 'unhide()'
| 
| *****************  Version 5  *****************
| User: Le           Date: 6.01.00    Time: 9:23
| Updated in $/GSM/Condat/MS/SRC/MFW
| Alignment of MFW versions
 * 
 * *****************  Version 4  *****************
 * User: Rm           Date: 12/03/99   Time: 10:15a
 * Updated in $/GSM/Condat/SND-MMI/MFW
 * new  parameter by icnCycle
 * 
 * *****************  Version 3  *****************
 * User: Es           Date: 24.11.99   Time: 11:54
 * Updated in $/GSM/Condat/SND-MMI/MFW
 * improved 'icnHide()' (really hides the icon now).
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

#include "mfw_sys.h"
#include "mfw_mfw.h"
#include "gdi.h"
#include "dspl.h"
#include "mfw_icn.h"


static int icnCommand (U32 cmd, void *h);


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_ICN            |
| STATE   : code                        ROUTINE : icnInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize icon handler

*/

MfwRes icnInit (void)
{
    mfwCommand[MfwTypIcn] = (MfwCb) icnCommand;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_ICN            |
| STATE   : code                        ROUTINE : icnExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize icon handler

*/

MfwRes icnExit (void)
{
    mfwCommand[MfwTypIcn] = 0;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_ICN            |
| STATE   : code                        ROUTINE : icnCreate          |
+--------------------------------------------------------------------+

  PURPOSE : create icon control

*/

MfwHnd icnCreate (MfwHnd w, MfwIcnAttr * a, MfwEvt e, MfwCb f)
{
    MfwHdr *hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    MfwIcn *icn = (MfwIcn *) mfwAlloc(sizeof(MfwIcn));
	MfwHdr *insert_status =0;
	
    if (!hdr || !icn)
	{
    	TRACE_ERROR("ERROR: icnCreate() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));
   		if(icn)
   			mfwFree((U8*)icn,sizeof(MfwIcn));	
	   	return 0;
  	}
    
    icn->mask = e;
    icn->flags = 0;
    icn->handler = f;
    icn->attr = a;
    icn->index = 0;

    hdr->data = icn;
    hdr->type = MfwTypIcn;

    insert_status = mfwInsert(w, hdr);
    
  	if(!insert_status)
	{
  		TRACE_ERROR("ERROR: icnCreate() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)icn,sizeof(MfwIcn));
		return 0;
  	}
    return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_ICN            |
| STATE   : code                        ROUTINE : icnDelete          |
+--------------------------------------------------------------------+

  PURPOSE : delete icon control

*/

MfwRes icnDelete (MfwHnd i)
{
    MfwRes res;

    if (!i)
        return MfwResIllHnd;

    icnHide(i);
    res = (mfwRemove(i)) ? MfwResOk : MfwResIllHnd;

    mfwFree(((MfwHdr *) i)->data,sizeof(MfwIcn));
    mfwFree(i,sizeof(MfwHdr));

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_ICN            |
| STATE   : code                        ROUTINE : icnShow            |
+--------------------------------------------------------------------+

  PURPOSE : show icon

*/

MfwRes icnShow (MfwHnd i)
{
    MfwIcn *icn;

    if (!i)
        return MfwResIllHnd;

    icn = ((MfwHdr *) i)->data;
    icn->flags |= E_ICN_VISIBLE;

    return icnUpdate(icn);
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_ICN            |
| STATE   : code                        ROUTINE : icnHide            |
+--------------------------------------------------------------------+

  PURPOSE : hide icon (without drawing)

*/

MfwRes icnHide (MfwHnd i)
{
    MfwIcn *icn;

    if (!i)
        return MfwResIllHnd;            /* icon does not exist      */

    icn = ((MfwHdr *) i)->data;         /* get control block        */
    icn->flags &= ~E_ICN_VISIBLE;       /* icon is not visible      */
    icn->index = 0;
    if (icn->handler)
        if (icn->mask & E_ICN_VISIBLE)
            (void)(icn->handler(E_ICN_VISIBLE,icn));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_ICN            |
| STATE   : code                        ROUTINE : icnUnhide          |
+--------------------------------------------------------------------+

  PURPOSE : unhide icon (without drawing)

*/

MfwRes icnUnhide (MfwHnd i)
{
    MfwIcn *icn;

    if (!i)
        return MfwResIllHnd;            /* icon does not exist      */

    icn = ((MfwHdr *) i)->data;         /* get control block        */
    icn->flags |= E_ICN_VISIBLE;        /* icon is visible          */
    icn->index = 0;
    if (icn->handler)
        if (icn->mask & E_ICN_VISIBLE)
            (void)(icn->handler(E_ICN_VISIBLE,icn));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : icnUpdate          |
+--------------------------------------------------------------------+

  PURPOSE : draw icon

*/

MfwRes icnUpdate (MfwIcn *i)
{
    if (!i)
        return MfwResIllHnd;

    if (!(i->flags & E_ICN_VISIBLE))
    {
        dspl_Clear(i->attr->area.px,i->attr->area.py,
                   (U16)(i->attr->area.px+i->attr->area.sx-1),
                   (U16)(i->attr->area.py+i->attr->area.sy-1));
    }
    else
    {
        dspl_BitBlt2(i->attr->area.px,i->attr->area.py,
                    i->attr->area.sx,i->attr->area.sy,
                    i->attr->icons,i->index,i->attr->icnType);
    }

    if (i->handler)
        if (i->mask & E_ICN_VISIBLE)
            (void)(i->handler(E_ICN_VISIBLE, i));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/

    return MfwResOk;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_MNU            |
| STATE   : code                        ROUTINE : icnCycle           |
+--------------------------------------------------------------------+

  PURPOSE : one-step-icon-animation

*/

MfwRes icnCycle (MfwHnd i,int offset)
{
    MfwIcn *icn;

    if (!i) return MfwResIllHnd;
    if (((MfwHdr *) i)->type != MfwTypIcn) return MfwResIllHnd;

    icn = ((MfwHdr *) i)->data;

    icn->index = (U8) ((icn->index + offset) % icn->attr->nIcons);

    dspl_BitBlt(icn->attr->area.px,icn->attr->area.py,
                icn->attr->area.sx,icn->attr->area.sy,
                icn->index,icn->attr->icons,0);

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_ICN            |
| STATE   : code                        ROUTINE : icnCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int icnCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            icnDelete(h);
            return 1;
        case MfwCmdUpdate:              /* repaint                  */
            if (!h || ((MfwHdr *) h)->type != MfwTypIcn)
                return 0;
            icnUpdate(((MfwHdr *) h)->data);
            return 1;
        default:
            break;
    }

    return 0;
}

