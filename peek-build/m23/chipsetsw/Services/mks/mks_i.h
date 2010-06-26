/**
 * @file   mks_i.h
 *
 * Declaration of internal function for MKS service.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  11/19/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _MKS_I_H_
#define _MKS_I_H_

#include "mks/mks_api.h"
#include "mks/mks_messages_i.h"

/* Macro definition for sending trace */
#define MKS_SEND_TRACE(text,level) rvf_send_trace(text, sizeof(text)-1, NULL_PARAM, level, MKS_USE_ID )
#define MKS_SEND_TRACE_PARAM(text,param,level) rvf_send_trace(text, sizeof(text)-1, param, level, MKS_USE_ID )


/**
 * @name Internal functions
 *
 */
/*@{*/

/**
 * function: mks_add_key_sequence_i
 *
 * This function add a magic key sequence.
 *
 * @param   infos_key_sequence_p  Key sequence informations.
 *
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INTERNAL_ERR if operation cannot be completed
 *    - RV_MEMORY_ERR if memory reach its size limit.
 */
T_RV_RET mks_add_key_sequence_i(T_MKS_INFOS_KEY_SEQUENCE_MSG* infos_key_sequence_p);


/**
 * function: mks_remove_key_sequence_i
 *
 * This function remove a magic key sequence.
 *
 * @param   remove_key_sequence_p  Key sequence name.
 *
 * @return
 *    - RV_OK
 */
T_RV_RET mks_remove_key_sequence_i(T_MKS_REMOVE_KEY_SEQUENCE_MSG* remove_key_sequence_p);


/**
 * function: mks_check_key_sequence
 *
 * This function is called when a key is pressed and check if a magic
 * key sequence is completed.
 *
 * @param   key_event_p  Key event message.
 *
 */
void mks_check_key_sequence(T_KPD_KEY_EVENT_MSG* key_event_p);


/**
 * function: mks_send_key_sequence_completed_msg
 *
 * Send a message that magic key sequence is completed.
 *
 * @param   completion_level  - MKS_SEQUENCE_COMPLETED:
 *                              Call subscriber when magic key sequence is completed,
 *                            - MKS_POST_SEQUENCE:
 *                              Call subscriber after magic key sequence is completed
 * @param   key_id            Virtual key Id pressed.
 * @param   return_path       Return path used to notify the subscriber.
 * @param   name              Name of the key sequence.
 */
void mks_send_key_sequence_completed_msg(UINT8 completion_level,
                                         T_KPD_VIRTUAL_KEY_ID key_id,
                                         T_RV_RETURN return_path,
                                         char* name);


/**
 * function: mks_reset_sequence
 *
 * This function is called when timer 0 expire, to reset all
 * key sequence.
 *
 */
void mks_reset_sequence(void);


/**
 * function: mks_initialize_swe
 *
 * This function is called at the initialization of the SWE.
 * It only subscribe to the keypad.
 *
 */
void mks_initialize_swe(void);


/**
 * function: mks_stop_swe
 *
 * This function is called when SWE is stopped, 
 * It unsubscribe from the keypad
 *
 */
void mks_stop_swe(void);



/*@}*/
#endif /* #ifndef _MKS_I_H_ */




