/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_mfw.c       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 11              $|
| CREATED: 21.09.98                     $Modtime:: 23.03.00 10:47   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_MFW

   PURPOSE : element chaining functions

   EXPORT  :

   TO DO   :

   $History:: mfw_mfw.c                                             $
	Sep 14, 2007	REF: OMAPS00145866  Adrian Salido
	Description:	FT - MMI: Wrong trace class 
	Solution:		changed event traces to function traces because the information content of 
				this is low for non-MMI people and against TI coding convention.

      Nov 05, 2005	REF: OMAPS00049571   Sumanth Kumar. C
      Description:	eZi Text function is not working
      Solution:		current_mfw_elem variable is updated when any of the elements 
                            such as window, keyboard, timer, etc., are deleted so that the 
                            current_mfw_element does not point to element that has already been freed.

  	Feb 28, 2004    REF: CRR 25608     Deepa M.D
	The mfwEorc is made const.The changes will be present both in Color and Golite build .

      Jan 27, 2005 REF: CRR MMI-SPR-28300 xnkulkar
      Description: BMI: memory leak in mfwFree/mfwAlloc ??
      Solution: Trace output to print memory address and size freed  
                    modified in function mfwFree()
      
      Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
      Description: MFW memory size variable changed from U16 -> U32
      Solution: The use of variable U16 has been replaced with U32

      Oct 06, 2004    REF: CRR 25608     Deepa M.D
      Bug:Change "static MfwHdr mfwEorc =" to const
      Fix: MfwHdr mfwEorc is made const to save memory.
 *
 * *****************  Version 11  *****************
 * User: Es           Date: 23.03.00   Time: 14:43
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added 'mfwParent(elem)'
 *
 * *****************  Version 10  *****************
 * User: Es           Date: 18.02.00   Time: 15:45
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * fixed alloc/free bug (shrink)
 * changed MfwMemLarge macro
 * added constants for MMI event handler return values
 *
 * *****************  Version 9  *****************
 * User: Kk           Date: 10.01.00   Time: 14:03
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 4  *****************
 * User: Rm           Date: 12/22/99   Time: 4:34p
 * Updated in $/GSM/Condat/SND-MMI/MFW
 *
 * *****************  Version 3  *****************
 * User: Be           Date: 29.11.99   Time: 14:31
 * Updated in $/GSM/Condat/SND-MMI/MFW
 * target integration
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 22.11.99   Time: 10:24
 * Updated in $/GSM/Condat/SND-MMI/MFW
 *
 * *****************  Version 1  *****************
 * User: Es           Date: 18.11.99   Time: 16:35
 * Created in $/GSM/Condat/SND-MMI/MFW
 * Initial
 *
 * *****************  Version 6  *****************
 * User: Es           Date: 1.04.99    Time: 17:07
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * removed lots of traces
 *
 * *****************  Version 5  *****************
 * User: Es           Date: 18.02.99   Time: 17:01
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 4  *****************
 * User: Es           Date: 17.02.99   Time: 19:11
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 3  *****************
 * User: Es           Date: 27.01.99   Time: 15:06
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 23.12.98   Time: 16:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
*/
#define ENTITY_MFW

//#define TRACE_MEMORY_CHECK

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

//#include <stdio.h>
//#include <string.h>

#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "mfw_win.h"
#include "mfw_sat.h"	/* CQ16435 : Header Required */

// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32
U32 mfwMemDynTotal = 0;                 /* sample memory usage      */
U32 mfwMemLowWater = 0xffffffff;            /* sample memory usage      */

MfwHdr *mfwFocus = 0;                   /* the focus node           */
MfwHdr *mfwRoot = 0;                    /* root of MFW elements     */
MfwCb mfwCommand [MfwTypMax];           /* MFW commands to modules  */

static MfwHdr *mfwSatIdleWin = 0;	/* CQ16435 : When this window gets focus, SAT will be informed */

static MfwMemHdr mfwMem;                /* root of free memory list */
static U8 *mfwMemBase;                  /* mfw dynamic memory pool  */
static U8 *mfwMemEnd;                   /* end of mfw memory        */

// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32
static U32 mfwMemLeft;                  /* space left in pool       */

//Feb 28, 2005    REF: CRR 25608     Deepa M.D
//The mfwEorc is made const.The changes will be present both in Color and Golite build
static const  MfwHdr mfwEorc = {0, MfwTypMax, 0}; /* end of root chain     */
 

/***************************Go-lite Optimization changes end***********************/

U8     mfwSignallingMethod = 0;           /* default is focus+root    */

EXTERN MfwHdr * current_mfw_elem;

#define SENTINAL_CHECK 1

// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: define SENTINEL_SIZE to a value of 4
#define SENTINEL_SIZE  4   //Will, Dec/07/2004-->Ti is 4 bytes
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize framework

*/

// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32
MfwRes mfwInit (void *externalRoot, U32 dynMemSize)
{
    int i;

    mfwSetFocus(0);                       /* initially no focus       */
    dynMemSize &= ~(sizeof(MfwMemHdr)-1); /* align to header size   */
    mfwMemDynTotal = dynMemSize;
    if (dynMemSize)
    {
		
/***************************Go-lite Optimization changes start***********************/
        mfwRoot = (MfwHdr *)&mfwEorc;             /* points to end of chain   */
		
/***************************Go-lite Optimization changes end***********************/
        mfwMemBase = (U8 *) externalRoot; /* setup memory handler   */
        mfwMemEnd = (U8 *) externalRoot + dynMemSize;
        mfwMem.next = 0;
        mfwMem.len = 0;
        ((MfwMemHdr *) mfwMemBase)->next = dynMemSize;
        ((MfwMemHdr *) mfwMemBase)->len = dynMemSize;
        mfwMemLeft = dynMemSize;
        memset(mfwMemBase+sizeof(MfwMemHdr),0,
                                    dynMemSize-sizeof(MfwMemHdr));
    }
    else
    {
        mfwRoot = (MfwHdr *) externalRoot; /* setup root pointer    */
		
/***************************Go-lite Optimization changes start***********************/
        if (!mfwRoot)
            mfwRoot = (MfwHdr *)&mfwEorc;         /* points to end of chain   */
		
/***************************Go-lite Optimization changes end***********************/
        mfwMemBase = 0;
        mfwMemEnd = 0;
        mfwMem.next = 0;
        mfwMem.len = 0;
    }
    for (i = 0; i < MfwTypMax; i++)     /* init command table       */
        mfwCommand[i] = 0;

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize framework

*/

void mfwExit (void)
{
    TRACE_FUNCTION("mfwExit()");
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwSignallingMethod|
+--------------------------------------------------------------------+

  PURPOSE : Defines the signalling method

*/

MfwRes mfwSetSignallingMethod (U8 method)
{
  mfwSignallingMethod = method;
  return MfwResOk; /*return statement added for warning removal*/
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwSetFocus          |
+--------------------------------------------------------------------+
  PURPOSE : Assign Focus to the passed window

*/

void mfwSetFocus(MfwHdr *w)
{
       if (satEvtRegistered(SatEvtIdleScreenActivated) == TRUE)
       {
		if ((mfwSatIdleWin != 0) && (mfwSatIdleWin == w))
		{
			/* x0083025 on Sep 14, 2007 for OMAPS00145866 (adrian) */
			MMI_TRACE_EVENT("NDH >>> CQ16435 : Send the Idle Screen Available Event to the SIM");
			satEvtDownload(SatEvtIdleScreen);
		}
       }

	mfwFocus = w;
	return;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwSetSatIdle          |
+--------------------------------------------------------------------+

  PURPOSE : Set the window which will trigger SAT Event Idle Activated if required

*/
void mfwSetSatIdle(MfwHdr *w)
{
	MfwWin *win;

	if (!w)
	{
		mfwSatIdleWin = 0;
		return;
	}

	win = w->data;

	mfwSatIdleWin = win->elems;
	return;
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwInsert          |
+--------------------------------------------------------------------+

  PURPOSE : chain up framework element

*/

MfwHdr *mfwInsert (MfwHdr *h, MfwHdr *e)
{
    if (!e)                             /* nothing to insert        */
        return 0;

    if (!h)                             /* use root chain           */
    {
        e->next = mfwRoot;
        mfwRoot = e;
    }
    else if (h->type == MfwTypWin)      /* use window chain         */
    {
        e->next = ((MfwWin *) (h->data))->elems;
        ((MfwWin *) h->data)->elems = e;
        if (mfwFocus == e->next)        /* was focus window ?       */
            mfwSetFocus(e);               /* yes: reassign focus      */
    }
    else                                /* insert into any chain    */
    {
      return 0;
    }

    return e;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwAppend          |
+--------------------------------------------------------------------+

  PURPOSE : chain up framework element at end of chain

*/

MfwHdr *mfwAppend (MfwHdr **h, MfwHdr *e)
{
    if (!e)                             /* nothing to append        */
        return 0;

    if (!h)                             /* append to header chain   */
        h = &mfwRoot;

    if ((*h)->type == MfwTypMax)        /* empty chain              */
    {
        if (mfwFocus == *h)
            mfwSetFocus(e);
        e->next = *h;
        *h = e;
        if(e->type == MfwTypWin || e->type == MfwTypKbd ||e->type == MfwTypTim ||
	         e->type == MfwTypEdt || e->type == MfwTypMnu || e->type == MfwTypIcn)
	             current_mfw_elem = e;
        return e;
    }

    while ((*h)->next && (*h)->next->type != MfwTypMax)
        h = &((*h)->next);

    e->next = (*h)->next;
    (*h)->next = e;
    if(e->type == MfwTypWin || e->type == MfwTypKbd ||e->type == MfwTypTim ||
	         e->type == MfwTypEdt || e->type == MfwTypMnu || e->type == MfwTypIcn)
	             current_mfw_elem = e;

    return e;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwRemove          |
+--------------------------------------------------------------------+

  PURPOSE : unchain element and return its root

*/

MfwHdr **mfwRemove (MfwHdr *e)
{
    MfwHdr **rp, **rpr, *h;

    if (!e)                             /* nothing to remove        */
        return 0;

    h = e;                              /* find elements root       */
    while (h->next && h->type != MfwTypMax)
        h = h->next;
    if (h->type != MfwTypMax)
        return 0;
    h = h->next;                        /* h points to root element */
    if (!h)                             /* remove from header chain */
        rp = &mfwRoot;
    else if (h->type == MfwTypWin)      /* remove from window chain */
        rp = &(((MfwWin*)(h->data))->elems);
    else                                /* don't remove from normal */
        return 0;

    rpr = rp;
    while (*rp && (*rp)->type != MfwTypMax)
    {
        if (*rp == e)
        {
            *rp = e->next;
            if (mfwFocus == e)          /* first element of window  */
                mfwSetFocus(e->next);
/*    Nov 05, 2005	REF: OMAPS00049571   Sumanth Kumar. C
  *    Solution: 	current_mfw_elem variable is updated when any of the elements 
  *                         such as window, keyboard, timer, etc., are deleted so that the 
  *                         current_mfw_element does not point to element that has already been freed.
  */			
            if(e->type == MfwTypWin || e->type == MfwTypKbd ||e->type == MfwTypTim ||
	         e->type == MfwTypEdt || e->type == MfwTypMnu || e->type == MfwTypIcn)
	             current_mfw_elem = e->next;
            return rpr;
        }
        rp = &((*rp)->next);
    }

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwParent          |
+--------------------------------------------------------------------+

  PURPOSE : get parent of given element

*/

MfwHnd mfwParent (MfwHdr *h)
{
    if (!h)                             /* no element               */
        return 0;

    while (h->next && h->type != MfwTypMax)
    {
      h = h->next;
      if(!h)
      {
        return 0;
      }
    }
    if (h->type != MfwTypMax)
        return 0;                       /* ERROR !                  */
    h = h->next;                        /* h points to root element */
    if (h && h->type == MfwTypWin)      /* parent should be window  */
        return h;

    return 0;                           /* root element             */
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwHeader          |
+--------------------------------------------------------------------+

  PURPOSE : get header of a given element

*/

MfwHnd mfwHeader (void)
{
  return current_mfw_elem;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwControl         |
+--------------------------------------------------------------------+

  PURPOSE : retrieve element control block

*/

MfwHnd mfwControl (MfwHdr *h)
{
    if (!h)
        return 0;
    if (h->type <= MfwTypNone || h->type >= MfwTypMax)
        return 0;

    return h->data;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwAlloc           |
+--------------------------------------------------------------------+

  PURPOSE : alloc MFW memory

*/

// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32
U8 *mfwAlloc (U32 size)
{
    MfwMemHdr *p, *n, *l;
    U8 *data_ptr;

    if (!size)
    {
#ifdef MFW_MEM
        TRACE_ERROR("mfwAlloc(): invalid memory request");
#endif
        return 0;
    }
    
#ifdef SENTINAL_CHECK
    if (!mfwMemBase)
    {
    	size += (SENTINEL_SIZE*2);
    	data_ptr = (U8 *) sysAlloc(size);
    	memset(data_ptr, 0x55, SENTINEL_SIZE);
    	memset(data_ptr + (size-SENTINEL_SIZE), 0x55, SENTINEL_SIZE);
    	data_ptr += SENTINEL_SIZE;
        return data_ptr;
    }

   	size = (U32) mfwMemLarge(size) + (SENTINEL_SIZE*2);

    if (size >= mfwMemLeft)
    {
        TRACE_ERROR("mfwAlloc(): no memory");
        return 0;
    }
    
    if (mfwMemBase + mfwMem.next > mfwMemEnd
              || mfwMemBase + mfwMem.next < mfwMemBase)
    {
        TRACE_ERROR("mfwAlloc(): corrupted memory");
        return 0;
    }

    p = &mfwMem;                        /* root of dyn memory       */
    n = (MfwMemHdr *) (mfwMemBase + p->next); /* first free block   */
    while ((U8 *) n < mfwMemEnd)
    {
        if (n->len == size)             /* fits exactly:            */
        {
            p->next = n->next;          /* just remove from chain   */

            mfwMemLeft -= size;
            if (mfwMemLeft < mfwMemLowWater)
                mfwMemLowWater = mfwMemLeft;

			data_ptr = (U8 *)n;

            memset(data_ptr, 0x55, SENTINEL_SIZE);
            memset(data_ptr + size-SENTINEL_SIZE, 0x55, SENTINEL_SIZE);
            memset(data_ptr + SENTINEL_SIZE, 0, size-(SENTINEL_SIZE*2));

            data_ptr += SENTINEL_SIZE;

#ifdef TRACE_MEMORY_CHECK
			/* x0083025 on Sep 14, 2007 for OMAPS00145866 (adrian) */
            MMI_TRACE_EVENT_P2("MFWAlloc %d bytes in address %p",size,n);
            mfwCheckMemoryLeft();
#endif
            return data_ptr;
        }

        if (n->len > size)              /* take it from a big one   */
        {
            l = (MfwMemHdr *) ((U8 *) n + size); /* new header    */
            l->next = n->next;          /* setup chain to next    */
            l->len = (U32) (n->len - size); /* remaining memory   */
            p->next += size;            /* link with previous block */

            mfwMemLeft -= size;
            if (mfwMemLeft < mfwMemLowWater)
                mfwMemLowWater = mfwMemLeft;

			data_ptr = (U8 *)n;

            memset(data_ptr, 0x55, SENTINEL_SIZE);
            memset(data_ptr + size-SENTINEL_SIZE, 0x55, SENTINEL_SIZE);
            memset(data_ptr + SENTINEL_SIZE, 0, size-(SENTINEL_SIZE*2));

            data_ptr += SENTINEL_SIZE;

#ifdef TRACE_MEMORY_CHECK
			/* x0083025 on Sep 14, 2007 for OMAPS00145866 (adrian) */
            MMI_TRACE_EVENT_P2("MFWAlloc %d bytes in address %p",size,n);
            mfwCheckMemoryLeft();
#endif
            return data_ptr;            /* allocated me address     */
        }
        p = n;
        n = (MfwMemHdr *) (mfwMemBase + n->next);
    }
#else
    if (!mfwMemBase)
        return (U8 *) sysAlloc(size);

    size = (U32) mfwMemLarge(size);
    if (size >= mfwMemLeft)
    {
        TRACE_ERROR("mfwAlloc(): no memory");
        return 0;
    }
    
    if (mfwMemBase + mfwMem.next > mfwMemEnd
              || mfwMemBase + mfwMem.next < mfwMemBase)
    {
        TRACE_ERROR("mfwAlloc(): corrupted memory");
        return 0;
    }

    p = &mfwMem;                        /* root of dyn memory       */
    n = (MfwMemHdr *) (mfwMemBase + p->next); /* first free block   */
    while ((U8 *) n < mfwMemEnd)
    {
        if (n->len == size)             /* fits exactly:            */
        {
            p->next = n->next;          /* just remove from chain   */
            mfwMemLeft -= size;
            if (mfwMemLeft < mfwMemLowWater)
                mfwMemLowWater = mfwMemLeft;
            memset(n,0,size);
#ifdef TRACE_MEMORY_CHECK
			/* x0083025 on Sep 14, 2007 for OMAPS00145866 (adrian) */
            MMI_TRACE_EVENT_P2("MFWAlloc %d bytes in address %p",size,n);
            mfwCheckMemoryLeft();
#endif
            return (U8 *) n;
        }
        if (n->len > size)              /* take it from a big one   */
        {
            l = (MfwMemHdr *) ((U8 *) n + size); /* new header    */
            l->next = n->next;          /* setup chain to next    */
            l->len = (U32) (n->len - size); /* remaining memory   */
            p->next += size;            /* link with previous block */
            mfwMemLeft -= size;
            if (mfwMemLeft < mfwMemLowWater)
                mfwMemLowWater = mfwMemLeft;
            memset(n,0,size);
#ifdef TRACE_MEMORY_CHECK
			/* x0083025 on Sep 14, 2007 for OMAPS00145866 (adrian) */
            MMI_TRACE_EVENT_P2("MFWAlloc %d bytes in address %p",size,n);
            mfwCheckMemoryLeft();
#endif
            return (U8 *) n;            /* allocated me address     */
        }
        p = n;
        n = (MfwMemHdr *) (mfwMemBase + n->next);
    }
#endif

    if ((U8 *)n == mfwMemEnd)
	{
    	TRACE_ERROR("mfwAlloc(): no memory block big enough to allocate size requested");
	}
    else
	{
    	TRACE_ERROR("mfwAlloc(): Error: free list corruption is likely");
	}

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW                |
| STATE   : code                        ROUTINE : mfwFree            |
+--------------------------------------------------------------------+

  PURPOSE : free allocated MFW memory

*/

// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32
void mfwFree (U8 *mem, U32 size)
{
    MfwMemHdr *p, *n;
    U8 check_val[SENTINEL_SIZE];
//    U32 check_val = 0x55555555;

	memset(&check_val, 0x55, SENTINEL_SIZE);

#ifdef SENTINAL_CHECK
	mem -= SENTINEL_SIZE;
#endif

    if (!size || !mem )
    {
        TRACE_ERROR("mfwFree(): invalid");
        return;                         /* nothing to free          */
    }

#ifdef SENTINAL_CHECK
	/*
	** Check the 4 preceeding bytes to ensure they haven't been changed
	*/
	if (memcmp(mem, &check_val, SENTINEL_SIZE) != 0)
	{
    	TRACE_ERROR("mfwFree(): Error: Preceeding Sentinal Bytes have been corrupted");
	}
#endif

#ifdef TRACE_MEMORY_CHECK
// Jan 27, 2005 REF: CRR MMI-SPR-28300 xnkulkar
// Description: BMI: memory leak in mfwFree/mfwAlloc ??
// Solution: Trace output to print memory address and size freed modified 
	/* x0083025 on Sep 14, 2007 for OMAPS00145866 (adrian) */
	MMI_TRACE_EVENT_P2("MFWFree address = %p, size : %d bytes",mem,size);
#endif

    if (!mfwMemBase)
    {
        void *m = mem;
        
#ifdef SENTINAL_CHECK
		/*
		** Check the SENTINEL_SIZE last bytes to ensure they haven't been changed
		*/
		if (memcmp(mem + size + SENTINEL_SIZE, &check_val, SENTINEL_SIZE) != 0)
		{
	    	TRACE_ERROR("mfwFree(): Error: Following Sentinal Bytes have been corrupted");
		}
#endif
        sysFree(m);
        return;
    }

#ifdef SENTINAL_CHECK

	size = (U32) mfwMemLarge(size) + (SENTINEL_SIZE*2);
#else
    size = (U32) mfwMemLarge(size);
#endif

    if ( mem < mfwMemBase || mem >= mfwMemEnd ||
       mem + size > mfwMemEnd ||
       mem + size <= mfwMemBase)
    {
        TRACE_ERROR("mfwFree(): invalid");
        return;                         /* nothing to free          */
    }

    p = &mfwMem;                        /* root of dyn memory       */
    n = (MfwMemHdr *) (mfwMemBase + p->next); /* first free block   */

	/*
	** Skip through the Free Link List until we get to where the current pointer
	** should be added
	*/
    while ((U8 *) n < mfwMemEnd && (U8 *) n < mem)
    {                                   /* search allocated area    */
        p = n;
        n = (MfwMemHdr *) (mfwMemBase + n->next);
    }

    /*
    ** Check that the select memory isn't already free
    */
    if (mem == (U8 *) p || mem == (U8 *) n)
    {
        TRACE_ERROR("mfwFree(): already free");
        return;                         /* already free             */
    }

	/*
	** Memory not already free
	*/
        
#ifdef SENTINAL_CHECK
		/*
		** Check the 4 last bytes to ensure they haven't been changed
		*/
		if (memcmp(mem + size - SENTINEL_SIZE, &check_val, SENTINEL_SIZE) != 0)
		{
	    	TRACE_ERROR("mfwFree(): Error: Following Sentinal Bytes have been corrupted");
		}
#endif

	/*
	** This memset should only be performed after we are sure that the memory should be freed
	*/
    memset(mem, 0, size);

    if (p != &mfwMem && (U8 *) p + p->len == mem)
    {                                   /* adjacent to left free:   */
        p->len += size;                 /* just add it              */
    }
    else
    {
        p->next = (U32) (mem - mfwMemBase); /* new free link        */
        p = (MfwMemHdr *) mem;          /* to new header            */
        p->next = (U32) ((U8 *) n - mfwMemBase); /* link to next    */
        p->len = size;
    }

    if ((U8 *) n < mfwMemEnd && ((mem + size) == (U8 *) n))
    {                                   /* adjacent to right free:  */
        p->next = n->next;              /* eliminate link and       */
        p->len += n->len;               /* eat up the space         */
    }

    mfwMemLeft += size;
#ifdef TRACE_MEMORY_CHECK
            mfwCheckMemoryLeft();
#endif
}

// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32
U32 mfwCheckMemoryLeft(void)
{
  /* x0083025 on Sep 14, 2007 for OMAPS00145866 (adrian) */
  MMI_TRACE_EVENT_P1("memory left %ld",mfwMemLeft);

  return mfwMemLeft;
}

/*
** This function gives the bounds safety of strncpy, but NULL terminates the string like strcpy
** 
** parameters : cpyto : the pointer to the destination string
**                   : cpyfrm : the pointer to the source string
**                   : len : the maximum length of the destination string including the NULL terminator
**
** returns        : A character pointer to the destination string if successful, otherwise NULL
**
** CQ16507      : strcpy call for string without null termination causing hardware reset
*/

char *mfwStrncpy(char *cpyto, const char *cpyfrm, size_t len)
{

	if (len <= 0)
		return (void *)0;

	if (cpyto == (char *)0)
		return (void *)0;

	strncpy(cpyto, cpyfrm, len-1);

	*(cpyto + (len -1)) = 0x00;

	return cpyto;
}

