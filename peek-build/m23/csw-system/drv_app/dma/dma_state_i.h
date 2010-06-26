/**
 * @file	dma_state_i.h
 * 
 * Internal definitions for DMA state machine, 
 * i.e. the states and the functions corresponding to the states.
 *
 * The messages are declared in dma_message.h, since they can 
 * be seen from outside.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/2/2003	 ()		Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __DMA_STATE_I_H_
#define __DMA_STATE_I_H_

#ifdef __cplusplus
extern "C"
{
#endif



/**
 * DMA states.
 *
 * DMA instance is a state machine with several possible states.
 * Based on the current state, DMA handle different kind of messages/events.
 * States are listed in the order they should normally be sequenced.
 *
 * See the Readme file for a description of the state machine.
 */
typedef enum
{
	DMA_STATE_1,
	DMA_STATE_2
} T_DMA_INTERNAL_STATE;


/*
 * Optional but	recommanded, define a function for the processing
 * in each state, like:
 *   T_DMA_RETURN dma_state_1					(T_RV_HDR * message_p);
 *   T_DMA_RETURN dma_state_2					(T_RV_HDR * message_p);
 */

#ifdef __cplusplus
}
#endif

#endif /* __DMA_STATE_I_H_ */
