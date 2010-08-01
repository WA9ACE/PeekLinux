/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_kbd.c       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 18              $|
| CREATED: 21.09.98                     $Modtime:: 23.03.00 8:24    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_KBD

   PURPOSE : keyboard handling functions

   EXPORT  :

   TO DO   : adjust PEI/driver interface

   HISTORY :

   Sep 14, 2007	REF: OMAPS00145862  Adrian Salido
	Description:	FT - MMI: Wrong trace class 
	Solution:		changed some event traces to function traces because the information content of 
				this is low for non-MMI people and against TI coding convention.
				
   Oct 13, 2006 REF: OMAPS00097121 - x0039928
   Description: Keyboard pressed in a loop. KBP_R after suspend-resume
   Solution : If the Auto press timer expires mmi checks the bsp kpd status and accordingly
   sets auto key in the key map.
   
   Sep 18, 2006 REF: OMAPS00094426 - x0039928
   Description: Locosto - KPD Release event would NOT be generated if a key is pressed 
   several times in a short period of time
   Solution : If the long press timer expires mmi checks the bsp kpd status and accordingly
   sets long press bit in the key map.

    xrashmic 22 Aug, 2004 MMI-SPR-32798
    Adding the support for screen capture using a dynamically assigned key.

   Apr 03, 2005 REF: CRR 29988 - xpradipg
   Description: Optimisation 2: Reduce the keyboard buffer and the dtmf buffer
   size
   Solution : The keyboard buffer is reduced to 20 from 50

*/

#define ENTITY_MFW

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#include <stdio.h>
#include <string.h>

#include "mfw_mfw.h"
#include "mfw_sys.h"
#include "mfw_tim.h"
#include "drv_key.h"
#include "mfw_win.h"
#include "mfw_kbd.h"

/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#ifndef NEPTUNE_BOARD
/* END ADD: Req ID: : Sumit : 14-Mar-05 */
#include "kpd/kpd_api.h"
/* BEGIN ADD: Req ID: : Sumit : 14-Mar-05 */
#else
#include "ti1_key.h"
#endif /* NEPTUNE_BOARD*/
/* END ADD: Req ID: : Sumit : 14-Mar-05 */

#include "mfw_ffs.h"
#include "mfw_utils.h"

#include "mfw_mme.h"


static int kbdCommand (U32 cmd, void *h);
static int toLong (U32 t, void *h);
static int toAuto (U32 t, void *h);

static MfwCb doAlways;                  /* all cases handler        */
static MfwHdr timLongH;                 /* the long press timer     */
static MfwTim timLong;
static MfwHdr timAutoH;                 /* the auto repeat timer    */
static MfwTim timAuto;
static U32 valAuto;                     /* auto start intervall     */
static U32 valRepeat;                   /* auto repeat intervall    */
static U32 curMap;                      /* current key map          */
static U8 curKey;                       /* current key code         */

static int still_processing_flag;

// PATCH LE 06.06.00
// current MFW element needed for multiple instances
EXTERN MfwHdr * current_mfw_elem;
// END PATCH LE 06.06.00

/*
** KeyPress Buffer Macros and Limits
*/
//   Apr 03, 2005 REF: CRR 29988 - xpradipg
//	Keyboard buffer reduced from 50 to 20
#ifdef FF_MMI_REDUCED_KBD_BUFFER
#define MAX_KPRESS_BFR 20
#else
#define MAX_KPRESS_BFR 50
#endif
typedef struct keyPressDetailsTag {
	char make;
	char key;
} keyPressDetails;

// Sep 18, 2006 REF: OMAPS00094426 - x0039928
#if(BOARD == 71)
UBYTE kpd_key;
#endif

static keyPressDetails keyPressBfr[MAX_KPRESS_BFR];
static SHORT mfw_kbd_kpress_buf_id;

extern void sendKeyInd( T_KPD_VIRTUAL_KEY_ID virtual_key_id,
						T_KPD_KEY_STATE key_state,
						T_KPD_PRESS_STATE press_state);
extern char drvGetKeyIndex( char key);

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbdInit            |
+--------------------------------------------------------------------+

  PURPOSE : initialize keyboard handler

*/

MfwRes kbdInit (void)
{
    timLong.time = 0;                   /* setup long press timer   */
    timLong.left = 0;
    timLong.handler = toLong;
    timLong.next = 0;
    timLongH.data = &timLong;
    timLongH.type = MfwTypTim;

    timAuto.time = 0;                   /* setup auto repeat timer  */
    timAuto.left = 0;
    timAuto.handler = toAuto;
    timAuto.next = 0;
    timAutoH.data = &timAuto;
    timAutoH.type = MfwTypTim;
    valAuto = valRepeat = 0;

    keyInit(kbdSignal);                 /* init keyboard driver     */

    mfwCommand[MfwTypKbd] = (MfwCb) kbdCommand;
    doAlways = 0;

    mfw_kbd_kpress_buf_id = mfw_cbuf_create(MAX_KPRESS_BFR,
    										  sizeof(keyPressDetails),
    										  FALSE,
    										  0xFF,
    										  TRUE,
    										  keyPressBfr);

	if (mfw_kbd_kpress_buf_id < 0)
		TRACE_EVENT_P1("ERROR : mfw_cbuf_create failed with error value %d", mfw_kbd_kpress_buf_id);

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbdExit            |
+--------------------------------------------------------------------+

  PURPOSE : finalize keyboard handler

*/

MfwRes kbdExit (void)
{
    mfwCommand[MfwTypKbd] = 0;
    keyExit();

    return MfwResOk;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbdAlways          |
+--------------------------------------------------------------------+

  PURPOSE : create keyboard control for all events

*/

MfwCb kbdAlways (MfwCb f)
{
    MfwCb always = doAlways;

    doAlways = f;
	if  (!f)
	{
        timStop(&timLongH);
        timStop(&timAutoH);
	}

    return always;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbdTime            |
+--------------------------------------------------------------------+

  PURPOSE : define keyboard timeouts

*/

void kbdTime (long tLong, long tAuto, long tRepeat)
{
    timLong.time = tLong;               /* setup long press timer   */
    timAuto.time = tAuto;               /* setup auto repeat timer  */
    valAuto = tAuto;                    /* save auto start timeout  */
    valRepeat = tRepeat;                /* save repeat intervall    */
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbdCreate          |
+--------------------------------------------------------------------+

  PURPOSE : create keyboard control

*/

MfwHnd kbdCreate (MfwHnd w, MfwEvt e, MfwCb f)
{
    MfwHdr *hdr = (MfwHdr *) mfwAlloc(sizeof(MfwHdr));
    MfwKbd *kbd = (MfwKbd *) mfwAlloc(sizeof(MfwKbd));
	MfwHdr *insert_status =0;
	
    if (!hdr || !kbd)
    {
    	TRACE_ERROR("ERROR: kbdCreate() Mem Alloc Failed.");
			
	   	if(hdr)
   			mfwFree((U8*)hdr,sizeof(MfwHdr));
   		if(kbd)
   			mfwFree((U8*)kbd,sizeof(MfwKbd));	
	   	return 0;
  	}

    kbd->map = e;
    kbd->key = 0;
    kbd->handler = f;

    hdr->data = kbd;
    hdr->type = MfwTypKbd;

    insert_status = mfwInsert(w,hdr);
    
    if(!insert_status)
	{
  		TRACE_ERROR("ERROR: kbdCreate() Failed to Install Handler. ");
   		mfwFree((U8*)hdr,sizeof(MfwHdr));
   		mfwFree((U8*)kbd ,sizeof(MfwKbd));
		return 0;
  	}
    return insert_status;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbdDelete          |
+--------------------------------------------------------------------+

  PURPOSE : delete keyboard control

*/

MfwRes kbdDelete (MfwHnd h)
{
    MfwRes res;

    if (!h)
        return MfwResIllHnd;

    res = (mfwRemove(h)) ? MfwResOk : MfwResIllHnd;

    mfwFree(((MfwHdr *) h)->data,sizeof(MfwKbd));
    mfwFree(h,sizeof(MfwHdr));

    return res;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : sigExec            |
+--------------------------------------------------------------------+

  PURPOSE : execute keyboard signal
            if the registered set of keys match the currently
            pressed key, the event handler is called, if the
            registered flags match the current flags according
            to the following table:
            M: Make/Break (key release events wanted)
            L: Long press event (timeout set via kbdTime())
            A: Auto repeat event (timing defined via kbdTime())

                                            M 0 0 0 0 1 1 1 1
            current event flags ---------   L 0 0 0 0 0 0 1 1
                                            A 0 0 0 0 0 1 0 1
                                      M L A .................
                                      0 0 0 . 0 0 0 0 1 0 0 0
                                      0 0 1 . 0 0 0 0 1 1 0 1
            registered                0 1 0 . 0 0 0 0 0 0 1 1
            event       -----------   0 1 1 . 0 0 0 0 0 1 1 1
            flags                     1 0 0 . 1 1 1 1 1 0 0 0
                                      1 0 1 . 1 1 1 1 1 1 0 1
                                      1 1 0 . 1 1 1 1 0 0 1 1
                                      1 1 1 . 1 1 1 1 0 1 1 1

*/

static int sigExec (MfwHdr *curElem, U32 map, U8 key)
{
    MfwKbd *kc;

    while (curElem)
    {
        if (curElem->type == MfwTypKbd)
        {
            kc = curElem->data;
            if (kc->map & map & ~KEY_FLAGS)
            {                           /* keys match               */
                kc->code = key;         /* set current key code     */
                if (map & KEY_MAKE)
                {
                    kc->key |= map;     /* set key in control block */
                }
                else
                {
                    kc->key &= ~map;    /* del key in control block */
                    kc->key &= ~KEY_MAKE; /* del make/break flag    */
                }
                if ((KEY_MAKE & ~map & kc->map) ||
                    (KEY_MAKE & map &&
                        ((KEY_LONG & map & kc->map)        ||
                         (!(KEY_LONG & kc->map)
                            && (KEY_AUTO & kc->map & map)) ||
                         (!(KEY_LONG & map)
                            && !(KEY_AUTO & map)
                            && !(KEY_LONG & kc->map))      ||
                         (!(KEY_LONG & map)
                            && (KEY_AUTO & map)
                            && (KEY_AUTO & kc->map)))))
                {
                    if (kc->handler)    /* handler valid            */
                    {
                      // PATCH LE 06.06.00
                      // store current mfw elem
                      current_mfw_elem = curElem;
                      // END PATCH LE 06.06.00
                      if ((*(kc->handler))(map,kc))
                          return 1;   /* event consumed           */
                    }
                }
            }
        }
        curElem = curElem->next;
    }

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : sigDistribute      |
+--------------------------------------------------------------------+

  PURPOSE : distribute keyboard event

*/

static void sigDistribute (U32 map, U8 key)
{
    int res = 0;

    TRACE_EVENT("sigDistribute");

    if (doAlways)
        if (doAlways(map,(void*) -1))
            return;                     /* event consumed           */

    if (mfwSignallingMethod == 0)
    {
        if (mfwFocus)
            res = sigExec(mfwFocus,map,key);
        if (!res && mfwRoot)
            res = sigExec(mfwRoot,map,key);
    }
    else
    {
        MfwHdr *h = mfwFocus;
        if (!h)
            h = mfwRoot;
        while (h)
        {
            res = sigExec(h,map,key); /* Warning correction - x0020906 - 14-08-2006*/
            if (res)   
                break;
            if (h == mfwRoot)
                break;
            h = mfwParent(mfwParent(h));
            if (h)
                h = ((MfwWin *)(h->data))->elems;
        }
        if (!res && mfwRoot && h != mfwRoot)
            res = sigExec(mfwRoot,map,key);
    }

    if (doAlways)
        (void)(doAlways(map,(void*) res));/*a0393213 lint warnings removal - void cast is done to avoid lint warning though the function returns int*/

    return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbdSignal          |
+--------------------------------------------------------------------+

  PURPOSE : keyboard event (called by driver/PEI)

*/

#define MAX_CONSEC_KEYS 6   // Maximum number of keys which can be processed before
						        // allowing the protocol stack processing time
//xrashmic 22 Aug, 2004 MMI-SPR-32798					        
#ifdef MMI_EM_ENABLED
#ifndef NEPTUNE_BOARD
/* This is not valid for Neptune Engineering Mode, hence*/
extern U8 screenCaptureKey;
void screen_capture(void);
#endif /* ifndef NEPTUNE_BOARD */
#endif

void kbdSignal (char make, char key)
{
	U32 map;
	UBYTE temp=0 ;
	USHORT numElem;

	int loopNo;
	int keyNo;


	if ((kbd_processKeyInput()== QUEUE_EVERY_KEY) ||
	     (kbd_processKeyInput()== QUEUE_N_KEYS))		
		temp = dspl_Enable(0);

	mme_backlightEvent(BL_KEY_PRESS);

	loopNo = 0;
	keyNo = 0;

	while (kbd_getMakeAndKey(&make,&key) != -1)
	{
#ifdef MMI_EM_ENABLED
#ifndef NEPTUNE_BOARD
/* This is not valid for Neptune Engineering Mode, hence*/

              //xrashmic 22 Aug, 2004 MMI-SPR-32798
              //If screen capture is enabled the key pressed is the screen capture key, 
              //we write the LCD buffer into the FFS and and consume the key 
              //event here itself without sending it to BMI
              // Also the screen capture key will be disabled here
	      screenCaptureKey=41;
              if(key== screenCaptureKey && make==1)
              {
                    TRACE_EVENT("*****Capturing the screen");
                    //screenCaptureKey=KCD_NONE;
                    screen_capture();
                    dspl_Enable(temp);
                    return;
              }
#endif /* ifndef NEPTUNE_BOARD*/
#endif
		/* temp hack until we can add support for > 32 keys) */
		switch(key) {
			case 49: // power
				key = 23;
				break;
			case 46: // wheel button
				key = 16;
				break;
			case 47: // back button
				key = 17;
				break;
			case 41: // lock button
				key = 12;
				break;
		}
		still_processing_flag = 1;
		TRACE_EVENT_P2("NDH : KbdSignal - key %d, state %02x", key, make);

		if (key <= KCD_MAX) /*a0393213 compiler warnings removal - comparison of key with 0 removed*/
		{
			loopNo++;
			keyNo++;
			
			map = 1L << key;

			if (make)
			{
				map |= KEY_MAKE;
				curMap = map;
				curKey = key;
				timStart(&timLongH);
				timAuto.time = valAuto;

				/*NM, p007a*/
				if (valAuto)
					timStart(&timAutoH);
				/*NM, p007a end*/

			}
			else
			{
				map &= ~KEY_MAKE;
				curMap = map; //ES!!
				curKey = key; //ES!!
				timStop(&timLongH);

				if (valAuto)
					timStop(&timAutoH);
			}

			//Select when we update the display
			switch (kbd_processKeyInput())
			{
				case QUEUE_EVERY_KEY:
					sigDistribute(map,key);
					break;
					
				case PROCESS_EVERY_KEY:
					temp = dspl_Enable(0);
					sigDistribute(map,key);
					dspl_Enable(temp);
					break;
					
				case QUEUE_N_KEYS:
					if ((loopNo %(NUM_QUEUE_KEYS*2))==0) 
					{
						kbd_setDisplayUpdateNeeded(1);
						sigDistribute(map,key); 
						dspl_Enable(temp); 
						temp = dspl_Enable(0);
						kbd_setDisplayUpdateNeeded(0);
					}
					else
						sigDistribute(map,key);
					break;
			}
		}

		if (keyNo == MAX_CONSEC_KEYS)
		{
			still_processing_flag = FALSE;
			break;
		}

		still_processing_flag = FALSE;
	}

	numElem = mfw_cbuf_num_elements(mfw_kbd_kpress_buf_id);

	/* x0083025 on Sep 14, 2007 for OMAPS00145862 (adrian) */
	MMI_TRACE_EVENT_P2("NDH >>> Kbd :- There are %d elements in the buffer (id : %d)" ,
					 numElem, mfw_kbd_kpress_buf_id);
	
	if ((keyNo == MAX_CONSEC_KEYS) && (numElem > 0))
	{
		sendKeyInd(0, 0, 0); // dummy values to trigger another keypress_ind
		/*
		** This delay is required to slow down the BMI when no trace is being output in order
		** to permit the Protocol Stack & other tasks to function correctly
		*/
		vsi_t_sleep (VSI_CALLER  30); 
	}

	if ((kbd_processKeyInput()== QUEUE_EVERY_KEY) ||
	     (kbd_processKeyInput()== QUEUE_N_KEYS))		
		dspl_Enable(temp);

	return;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : toLong             |
+--------------------------------------------------------------------+

  PURPOSE : long press timeout handler

*/
static int toLong (U32 t, void *h)
{
// Sep 18, 2006 REF: OMAPS00094426 - x0039928
// Fix: On long key press timer expiry bsp kpd status is checked to see if the state is 
// in in Kpd pressed state and sets the long key bit.
   UBYTE state;

   kpd_retrieve_key_status(kpd_key, KPD_DEFAULT_MODE, &state);
   TRACE_EVENT_P2("toLong() state = %d CurKey = %d", state, curKey);
   if(curKey == 15 || curKey == 14)
	return 0;
   if(!state)
       curMap |= KEY_LONG;   
    sigDistribute(curMap,curKey);

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : toAuto             |
+--------------------------------------------------------------------+

  PURPOSE : auto repeat timeout handler

*/

static int toAuto (U32 t, void *h)
{
#if(BOARD == 71)
   UBYTE state;
   TRACE_EVENT("toAuto()");

   kpd_retrieve_key_status(kpd_key, KPD_DEFAULT_MODE, &state);
   if(!state)
   { 
#endif   
      curMap |= KEY_AUTO;
      sigDistribute(curMap,curKey);
 
	timAuto.time = valRepeat;
    /* NM p007c*/
    if (valRepeat)
   		timStart(&timAutoH);
	/* NM p007c end*/
#if(BOARD == 71)	  
   }
#endif  
    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbdCommand         |
+--------------------------------------------------------------------+

  PURPOSE : handle mfw windows command

*/

static int kbdCommand (U32 cmd, void *h)
{
    switch (cmd)
    {
        case MfwCmdDelete:              /* delete me                */
            if (!h)
                return 0;
            kbdDelete(h);
            return 1;
        default:
            break;
    }

    return 0;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbd_putMakeAndKey    |
+--------------------------------------------------------------------+

  PURPOSE : places 'make' (key up/down) and key index into a queue

*/

int kbd_putMakeAndKey( char make, char key)
{
	keyPressDetails localKP;
	SHORT retVal;

	localKP.make = make;
	localKP.key = key;

	retVal = mfw_cbuf_put(mfw_kbd_kpress_buf_id, &localKP);

	if (retVal < 0)
		TRACE_EVENT_P1("ERROR : mfw_cbuf_put failed with error value %d", retVal);

	return (retVal);
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbd_getMakeAndKey    |
+--------------------------------------------------------------------+

  PURPOSE : reads 'make' (key up/down) and key index into a queue
		Return	+ve number - keys left in buffer
				 0 - no keys left - last key press returned
				-1 - no keys and none in buffer
*/

int kbd_getMakeAndKey( char* make, char* key)
{
	keyPressDetails localKP;
	SHORT retVal;

	retVal = mfw_cbuf_get(mfw_kbd_kpress_buf_id, &localKP);

	if (retVal < 0)
	{
		*key = 0x7F;
		*make = 0;
		return (-1);
	}

	*make = !(localKP.make);
	*key =   drvGetKeyIndex(localKP.key);
	TRACE_FUNCTION_P1("mfw_kbd: drvGetKeyIndex %d", drvGetKeyIndex(localKP.key));
	return (mfw_cbuf_num_elements(mfw_kbd_kpress_buf_id));
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbd_getNumElements    |
+--------------------------------------------------------------------+

  PURPOSE : returns number of elements in queue

*/
int kbd_getNumElements(void)
{
	return ((int)mfw_cbuf_num_elements(mfw_kbd_kpress_buf_id));
}

int kbd_stillProcessingKeys(void)
{
	return (still_processing_flag);
}

int mfwKey_skipDisplay( void )
{
	if ((mfw_cbuf_num_elements(mfw_kbd_kpress_buf_id) > 2) && (still_processing_flag == 1))
		return (TRUE);
	else
		return (FALSE);
}

/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbd_displayUpdateNeeded   |
+--------------------------------------------------------------------+

  PURPOSE : returns TRUE if we need to update the display

*/
int displayUpdateNeeded; //used for output every 'n' key presses
int kbd_displayUpdateNeeded(void)
{	
	if (kbd_processKeyInput()==PROCESS_EVERY_KEY)
		return (TRUE);//Processing each key press - always update screen
	else if (displayUpdateNeeded==0)
		return (TRUE);//need to update the display (1 in 6 output) 
	else if (mfw_cbuf_num_elements(mfw_kbd_kpress_buf_id) > 1) 
		return (FALSE);//keys in queue - do not update
	else
		return (TRUE);//only 1 key up/down in queue - update display
}
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbd_setDisplayUpdateNeeded   |
+--------------------------------------------------------------------+

  PURPOSE : sets/clears the flag indicating we need to update the display

*/
void kbd_setDisplayUpdateNeeded(int set)
{
	displayUpdateNeeded = set;
}
	
/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_KBD            |
| STATE   : code                        ROUTINE : kbd_processEveryKeyInput    |
+--------------------------------------------------------------------+

  PURPOSE : indicates if the software should process each keypad input individually 
  			(TRUE) or if it should process keypad inputs one at a time (FALSE) 

*/
int kbd_processKeyInput(void)
{
	return (QUEUE_EVERY_KEY);//We buffer multiple key inputs

}







