/**
 * @file   mks_api.h
 *
 * API Definition for Magic Key Sequence Service.
 *
 * This file gathers all the constants, structure and functions declaration
 * useful for a MKS service user.
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

#ifndef _MKS_API_H_
#define _MKS_API_H_

#include "mks/mks_cfg.h"

#include "kpd/kpd_api.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

/**
 * @name External types
 *
 * Types used in API.
 *
 */
/*@{*/

/** Definition of completion type used.
 * Subscriber is notified when key sequence is completed.
 */
#define MKS_SEQUENCE_COMPLETED 0x01

/** Definition of completion type used.
 * Subscriber is notified after key sequence is completed.
 */
#define MKS_POST_SEQUENCE 0x02


/** Definition of one magic key sequence. */
typedef struct {  char name[KPD_MAX_CHAR_NAME+1]; /* It's not ensured that this name is unique */
                  UINT8 nb_key_of_sequence;
                  T_KPD_VIRTUAL_KEY_ID key_id[MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE];
                  UINT8 completion_type; /* can equal to MKS_SEQUENCE_COMPLETED or MKS_POST_SEQUENCE */
                  UINT8 nb_key_for_post_sequence; /* used only if completion_type == MKS_POST_SEQUENCE */
                  T_RV_RETURN return_path;
               } T_MKS_INFOS_KEY_SEQUENCE;

/*@}*/



/**
 * @name API functions
 *
 * API functions declarations.
 */
/*@{*/

/**
 * function: mks_add_key_sequence
 *
 * This function initialize a magic key sequence.
 *
 * @param   infos_key_sequence_p    Magic key sequence.
 * @return
 *    - RV_OK if operation is successful,
 *    - RV_INVALID_PARAMETER if one parameter is incorrect,
 *    - RV_MEMORY_ERR if memory reach its size limit.
 *
 */
T_RV_RET mks_add_key_sequence(T_MKS_INFOS_KEY_SEQUENCE* infos_key_sequence_p);

/**
 * function: mks_remove_key_sequence
 *
 * This function remove an existing magic key sequence.
 *
 * @param   infos_key_sequence_p    Magic key sequence.
 * @return
 *    - RV_OK
 *    - RV_MEMORY_ERR if memory reach its size limit.
 *
 * @note
 *       - If the name of the key sequence doesn't exist, no return status is
 *         sent to inform client that process has failed.
 *
 */
T_RV_RET mks_remove_key_sequence(char name[KPD_MAX_CHAR_NAME+1]);


/*@}*/


/*************************************************************************/
/************************** MESSAGES DEFINITION **************************/
/*************************************************************************/

/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
#define MKS_MESSAGES_OFFSET      (0x37 << 10)



/**
 * @name MKS_SEQUENCE_COMPLETED
 *
 * This message is sent to a subscriber when a key sequence is completed.
 *
 * Message issued by MKS to a subscriber.
 */
/*@{*/


/** Message ID. */
#define MKS_SEQUENCE_COMPLETED_MSG (MKS_MESSAGES_OFFSET | 0x001)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR    hdr;

   /** Name of key sequence. */
   char name[KPD_MAX_CHAR_NAME];

   /** Kind of completion (sequence completed or post_sequence). */
   UINT8 completion_type;

   /** Virtual key Id pressed (used when completion_type == MKS_POST_SEQUENCE). */
   T_KPD_VIRTUAL_KEY_ID key_pressed;

}  T_MKS_SEQUENCE_COMPLETED_MSG;
/*@}*/


#endif /* #ifndef _MKS_API_H_ */


