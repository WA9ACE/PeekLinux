/**
* @file i2c_state_i.h
* 
* Internal definitions for I2C state machine, 
* i.e. the states and the functions corresponding to the states.
*
* The messages are declared in i2c_message.h, since they can 
* be seen from outside.
*
* @author Remco Hiemstra (remco.hiemstra@ict.nl)
* @version 0.1
*/

/*
* History:
*
* Date        Author          Modification
* -------------------------------------------------------------------
* 12/30/2003  Remco Hiemstra (remco.hiemstra@ict.nl)    Create.
*
* (C) Copyright 2003 by ICT Automatisering, All Rights Reserved
*/

#ifndef __I2C_STATE_I_H_
#define __I2C_STATE_I_H_


#include "rv/rv_general.h"


/**
* I2C states.
*
* I2C instance is a state machine with several possible states.
* Based on the current state, I2C handle different kind of messages/events.
* States are listed in the order they should normally be sequenced.
*
* See the Readme file for a description of the state machine.
*/
typedef enum
{
  I2C_STATE_INTERRUPT,
    I2C_STATE_POLLING,
    I2C_STATE_DMA
} T_I2C_INTERNAL_STATE;


/*
* Optional but  recommanded, define a function for the processing
* in each state, like:
*   T_I2C_RETURN i2c_state_1          (T_RV_HDR * message_p);
*   T_I2C_RETURN i2c_state_2          (T_RV_HDR * message_p);
*/

#endif /* __I2C_STATE_I_H_ */
