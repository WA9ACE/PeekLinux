/**
 * @file  mmc_state_i.h
 * 
 * Internal definitions for MMC state machine, 
 * i.e. the states and the functions corresponding to the states.
 *
 * The messages are declared in mc_message.h, since they can 
 * be seen from outside.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/1/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __MC_STATE_I_H_
#define __MC_STATE_I_H_


#include "rv/rv_general.h"


/**
 * MC states.
 *
 * MMC instance is a state machine with several possible states.
 * Based on the current state, MMC handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */
typedef enum
{
  MC_STATE_UNINITIALISED,  
  MC_STATE_INITIALISED,
  MC_STATE_INIT_DRIVER,
  MC_STATE_IDLE_DETACHED,
  MC_STATE_IDLE_ATTACHED,
  MC_STATE_HANDLE_REQ,
  MC_STATE_STOPPED,  
  MC_STATE_KILLED
} T_MC_INTERNAL_STATE;

#endif /* __MC_STATE_I_H_ */
