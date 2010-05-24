#ifndef _L1DSPAGE_H_
#define _L1DSPAGE_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Data
----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 Global Defines  Macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/
/*****************************************************************************

 FUNCTION NAME:     L1dSPageInit

 DESCRIPTION:       This function is called during power up. It does:
                        - set slotted paging sub-state to the idle state
                        - disable the 32KHz calibration
                          
 PARAMETERS:        None.

 RETURNED VALUES:   None.

*****************************************************************************/
extern	void L1dSPageInit(void);

/*****************************************************************************

 FUNCTION NAME:     L1dSPageEnable

 DESCRIPTION:       This function is called when l1dmtask receives the slotted 
                    paging enable messge from the PSW task. It changes the L1d
                    sub-state to slotted paging and changes the slotted paging 
                    sub-state from the idle state to the pending calibration 
                    state. The call back functions are set to the proper system
                    times to start the 32KHz calibration and read the long code
                    state for long code hopping. 
                     
 PARAMETERS:        Message data pointer inlcuding paging slot and slot cycle index.

 RETURNED VALUES:   None.

*****************************************************************************/
extern	void L1dSPageEnable(void *MsgDataPtr);

/*****************************************************************************

 FUNCTION NAME:     L1dSPageStateMachine

 DESCRIPTION:        
                     
 PARAMETERS:        

 RETURNED VALUES:   None.

*****************************************************************************/
extern	void L1dSPageStateMachine(L1dSPageEventT SpEvent, void *MsgDataPtr);

/*****************************************************************************

 FUNCTION NAME:   L1dSPageLisrReSync

 DESCRIPTION:     This is the 32KHz re-sync timer ISR. When the 32KHz re-sync 
                  timer is expired, it signals a normal l1d wake up process. This 
                  function initiates the l1d recovery process. It initiates:
                    - activating HISR
                    - the RF transition
                    - the mini-acquisition
                    - the system time re-adjustment
                    - long code hopping
                    - new long code state

 PARAMETERS:      None.

 RETURNED VALUES: None.

****************************************************************************/
extern	void L1dSPageLisrReSync(void);

/*****************************************************************************

 FUNCTION NAME:   L1dSPageHisr

 DESCRIPTION:     This is a call back function for slotted paging hisr to initiate
                  the RF transition.

 PARAMETERS:      None.

 RETURNED VALUES: None.

****************************************************************************/
extern	void L1dSPageHisrCreate(void);

/*****************************************************************************

 FUNCTION NAME:   L1dSPageStatusGet

 DESCRIPTION:     This function reports the status of the slotted paging state
                  machine.
 PARAMETERS:      None.

 RETURNED VALUES: TRUE  = ENABLED
                  FALSE = DISABLED

****************************************************************************/
extern  bool L1dSPageStatusGet(void);

/*****************************************************************************

 FUNCTION NAME:   L1dSPageGetWakeMode

 DESCRIPTION:     This function reports the current Wakeup slot mode
                  machine.
 PARAMETERS:      None.

 RETURNED VALUES: WakeModeT

****************************************************************************/
extern WakeModeT L1dSPageGetWakeMode(void);



#endif

