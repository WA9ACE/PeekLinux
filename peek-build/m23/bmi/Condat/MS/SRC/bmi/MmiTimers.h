#ifndef _DEF_MMI_TIMERS_H_
#define _DEF_MMI_TIMERS_H_
/*******************************************************************************

					CONDAT (UK)

********************************************************************************

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		MMI
 $File:		    MmiTimers.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    25/10/00                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This handles the storage of call times.
                        
********************************************************************************
 $History: MmiTimers.h


	   
 $End

*******************************************************************************/
#include "MmiBookShared.h"

typedef enum
{
	LAST_CALL,
	OUTGOING_CALLS,
	INCOMING_CALLS
} timer_types;

/*SPR 2142 removed timer window data structure*/


void setLastCallDuration(long call_time, T_MFW_CM_CALL_DIR call_direction);
int timerLastCall(MfwMnu* m, MfwMnuItem* i);
int timerIncomingCalls(MfwMnu* m, MfwMnuItem* i);
int timerOutgoingCalls(MfwMnu* m, MfwMnuItem* i);
void resetIncomingCallsDuration( void );
void resetOutgoingCallsDuration( void );

#endif
