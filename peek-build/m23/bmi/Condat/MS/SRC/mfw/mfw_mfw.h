/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_mfw.h       $|
| $Author:: Kk  $ CONDAT GmbH           $Revision:: 13              $|
| CREATED: 21.09.98                     $Modtime:: 23.03.00 16:37   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_MFW

   PURPOSE : Framework common types & constants

   EXPORT  :

   TO DO   :

   $History:: mfw_mfw.h                                             $

	Sep 14, 2007	REF: OMAPS00145866  Adrian Salido
	Description:	FT - MMI: Wrong trace class 
	Solution:		Added some event trace function so that internal event traces for events that 
					are too low for non-MMI people and against TI coding convention.
    
 * *****************  Version 14  *****************
	Dec 22, 2005    REF: ENH  xdeepadh
	Description: Image Viewer Application
	Solution: Implemeted the Image Viewer  to view the jpeg images
	
* Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
* Description: MFW memory size variable changed from U16 -> U32
* Solution: The use of variable U16 has been replaced with U32

 * *****************  Version 13  *****************
 * User: Kk           Date: 24.03.00   Time: 11:02
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 12  *****************
 * User: Es           Date: 23.03.00   Time: 14:43
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Added 'mfwParent(elem)'
 *
 * *****************  Version 11  *****************
 * User: Es           Date: 18.02.00   Time: 15:45
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * fixed alloc/free bug (shrink)
 * changed MfwMemLarge macro
 * added constants for MMI event handler return values
 *
 * *****************  Version 10  *****************
 * User: Kk           Date: 10.01.00   Time: 14:03
 * Updated in $/GSM/Condat/MS/SRC/MFW
 *
 * *****************  Version 9  *****************
 * User: Le           Date: 6.01.00    Time: 9:23
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Alignment of MFW versions
 *
 * *****************  Version 3  *****************
 * User: Xsp          Date: 8/12/99    Time: 15:34
 * Updated in $/GSM/Condat/SND-MMI/MFW
 * Patch for CHAR botch...
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 22.11.99   Time: 10:29
 * Updated in $/GSM/Condat/SND-MMI/MFW
 *
 * *****************  Version 1  *****************
 * User: Es           Date: 18.11.99   Time: 16:35
 * Created in $/GSM/Condat/SND-MMI/MFW
 * Initial
 *
 * *****************  Version 8  *****************
 * User: Es           Date: 14.06.99   Time: 12:14
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 6  *****************
 * User: Es           Date: 25.02.99   Time: 9:53
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 5  *****************
 * User: Es           Date: 20.02.99   Time: 11:54
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 4  *****************
 * User: Es           Date: 17.02.99   Time: 20:00
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 3  *****************
 * User: Es           Date: 14.01.99   Time: 17:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
 *
 * *****************  Version 2  *****************
 * User: Es           Date: 23.12.98   Time: 16:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
*/

#ifndef _DEF_MFW_MFW_H_
#define _DEF_MFW_MFW_H_

/* 
	x0083025 on Sep 14, 2007 for OMAPS00145866 (adrian) 
	MMI Internal Development trace events replacement
*/
#include "vsi.h"		/* for TRACE_FUNCTION dependency */
#define MMI_TRACE_EVENT(a) 					TRACE_FUNCTION(a)
#define MMI_TRACE_EVENT_P1(f, a1) 			TRACE_FUNCTION_P1(f, a1)
#define MMI_TRACE_EVENT_P2(f, a1, a2) 		TRACE_FUNCTION_P2(f, a1, a2)
#define MMI_TRACE_EVENT_P3(f, a1, a2, a3) 	TRACE_FUNCTION_P2(f, a1, a2, a3)
#define ENTITY_MFW

#ifndef NO_ASCIIZ
#define NO_ASCIIZ
#endif

/* Ring tone definition */
#ifdef FF_MMI_RINGTONE
#define CALLTONE_SELECT      41
#define SMSTONE_SELECT       42
#define ALARMTONE_SELECT   43
#endif

/**************************************//* PORTABILITY TYPES        */

#define U8  unsigned char
#define S8  signed char
#define U16 unsigned short
#define S16 signed short
#define U32 unsigned long
#define S32 signed long

/**************************************//* MMI EVENT HANDLER RETURN */

#define MFW_EVENT_CONSUMED	1			/* return, if event handled */
#define MFW_EVENT_REJECTED	0			/* return, if event unknown */


#ifdef TI_PS_OP_VSI_NO_CALL_ID
#define TIMER_START(C,I,T)         vsi_t_start_nc(I,T)
#define TIMER_PSTART(C,I,T,R)      vsi_t_pstart_nc(I,T,R)
#define TIMER_STOP(C,I)            vsi_t_stop_nc(I)
#define TIMER_STATUS(C,I,T)        vsi_t_status_nc(I,T)
#define SUSPEND_SELF(C,T)          vsi_t_sleep_nc(T)
#define SYSTEM_TIME(C,T)           vsi_t_time_nc(T)               
#else /* TI_PS_OP_VSI_NO_CALL_ID */
#define TIMER_START(C,I,T)         vsi_t_start(C,I,T)
#define TIMER_PSTART(C,I,T,R)      vsi_t_pstart(C,I,T,R)
#define TIMER_STOP(C,I)            vsi_t_stop(C,I)
#define TIMER_STATUS(C,I,T)        vsi_t_status(C,I,T)
#define SUSPEND_SELF(C,T)          vsi_t_sleep(C,T)
#define SYSTEM_TIME(C,T)           vsi_t_time(C,T)               
#endif /* TI_PS_OP_VSI_NO_CALL_ID */


/**************************************//* COMMON MFW TYPES         */

typedef void * MfwHnd;                  /* MFW element handle       */
typedef U32 MfwEvt;                     /* MFW event mask           */
typedef int (*MfwCb) (MfwEvt, MfwHnd);  /* MFW event handler        */

typedef struct MfwRectTag               /* RECTANGLE SPEC           */
{
    U16 px;                             /* top left pos x           */
    U16 py;                             /* top left pos y           */
    U16 sx;                             /* horizontal size          */
    U16 sy;                             /* vertical size            */
} MfwRect;

typedef enum                            /* Error codes              */
{
    MfwResOk = 0,                       /* no error                 */
    MfwResDone,                         /* OK, handler ready        */
    MfwResErr,                          /* unspecific error         */
    MfwResNoMem,                        /* insufficient memory      */
    MfwResIllHnd,                       /* invalid handle           */
    MfwResDriver                        /* driver error             */
} MfwRes;

typedef enum                            /* MFW commands to modules  */
{
    MfwCmdNone = 0,                     /* no command               */
    MfwCmdDelete,                       /* delete element           */
    MfwCmdUpdate                        /* repaint visibles         */
} MfwCmd;

typedef enum                            /* MFW element types        */
{
    MfwTypNone = 0,                     /* no element (first !!)    */
    MfwTypWin,                          /* window                   */
    MfwTypEdt,                          /* editor                   */
    MfwTypMnu,                          /* menu handler             */
    MfwTypIcn,                          /* icon handler             */
    MfwTypKbd,                          /* keyboard                 */
    MfwTypTim,                          /* timer                    */
    MfwTypMme,                          /* misc. mobile equipment   */
    MfwTypVmm,                          /* MDL virtual machine      */
    MfwTypNm,                           /* network                  */
    MfwTypSim,                          /* SIM card                 */
    MfwTypCm,                           /* call handling            */
    MfwTypPhb,                          /* phonebook handling       */
    MfwTypSms,                          /* short message handling   */
    MfwTypSs,                           /* supl. services handling  */
    MfwTypSat,                          /* SIM Toolkit handling     */
    MfwTypLng,                          /* language handling        */
    MfwTypInp,                          /* input handling           */
    MfwTypNode,                         /* node element             */
    MfwTypCphs,                         /* CPHS element             */
    MfwTypGprs,                         /* GPRS handling            */
    MfwTypEm,							/*MC, SPR 1209 Eng Mode handling*/
    MfwTypTd, 							/*MC, SPr 1725, Time/date handling*/
#ifdef BT_INTERFACE
  MfwTypBt,                           /* bluetooth handling       */
#endif
#ifdef BTE_MOBILE
  MfwTypBte,                           /* bluetooth handling       */
#endif
   MfwTypCt,                           /* camera test handling*/

#ifdef FF_MMI_FILEMANAGER	
 MfwTypfm,
#endif
    MfwTypMax                           /* end of types (last !!)   */
} MfwTyp;

typedef struct MfwHdrTag                /* header structure         */
{
    struct MfwHdrTag *next;             /* next header              */
    MfwTyp type;                        /* elem type code           */
    void *data;                         /* elem control structure   */

} MfwHdr;

// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32
typedef struct                          /* memory management header */
{
    U32 next;                           /* index of next free block */
    U32 len;                            /* length of this block     */
} MfwMemHdr;
								/* sizeof(MfwMemHdr) must be 2^n !! */
                                        /* align with header length */
#define mfwMemLarge(x) (((x)+sizeof(MfwMemHdr)-1)&(~(sizeof(MfwMemHdr)-1)))
/* not used:     #define mfwMemShrink(x) ((x)&(~(sizeof(MfwMemHdr)-1))) */

/**************************************//* MFW EXPORTED VARIABLES   */

extern MfwHdr *mfwFocus;                /* the focus node           */
extern MfwHdr *mfwRoot;                 /* root of MFW elements     */
extern MfwCb mfwCommand [];             /* MFW commands to modules  */


/**************************************//* MFW FUNCTION PROTOTYPES  */

// Dec 22, 2004 REF: CRR MFW-SPR-27847 xnkulkar
// Description: MFW memory size variable changed from U16 -> U32
// Solution: The use of variable U16 has been replaced with U32

MfwRes mfwInit (void *externalRoot, U32 dynMemSize); 
void mfwExit (void);
MfwHdr *mfwAppend (MfwHdr **h, MfwHdr *e);
MfwHdr **mfwRemove (MfwHdr *e);
MfwHdr *mfwInsert (MfwHdr *h, MfwHdr *e);
MfwHnd mfwParent (MfwHdr *h);
MfwHnd mfwControl (MfwHdr *h);
U8 *mfwAlloc (U32 size);
void mfwFree (U8 *mem, U32 size);
MfwHnd mfwHeader (void);
MfwRes mfwSetSignallingMethod (U8 method);
U32 mfwCheckMemoryLeft(void);
void mfwSetFocus(MfwHdr *w);
void mfwSetSatIdle(MfwHdr *w);
char *mfwStrncpy(char *cpyto, const char *cpyfrm, size_t len);

extern U8 mfwSignallingMethod;
/**************************************//* PRELIMINARIES ES!!       */

#define T_MFW int
#define T_MFW_HND MfwHnd
#define T_MFW_EVENT MfwEvt
#define T_MFW_HDR MfwHdr
#define T_MFW_RES MfwRes
#define T_MFW_CB MfwCb
#define MFW_TYP_SIM MfwTypSim
#define MFW_TYP_NM MfwTypNm
#define MFW_TYP_CM MfwTypCm
#define MFW_TYP_PHB MfwTypPhb
#define MFW_TYP_SMS MfwTypSms
#define MFW_TYP_SS MfwTypSs
#define MFW_RES_ILL_HND MfwResIllHnd
#define MFW_RES_OK MfwResOk
#define MFW_RES_ERR MfwResErr
#define T_MFW_PREF_PLMN_LIST MfwNmPrefPlmnList

#endif

