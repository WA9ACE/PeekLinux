/**
 * @file   mks_messages_i.h
 *
 * Data structures that MKS SWE can receive.
 *
 * These messages are send by the bridge function. There are not available
 * out of the SWE.
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

#ifndef _MKS_MESSAGES_I_H_
#define _MKS_MESSAGES_I_H_



/**
 * @name MKS_INFOS_KEY_SEQUENCE_MSG
 *
 * Internal message.
 *
 * Message issued by MKS to MKS task.
 * This message is used to add a magic key sequence.
 */
/*@{*/
/** Message ID. */
#define MKS_INFOS_KEY_SEQUENCE_MSG (MKS_MESSAGES_OFFSET | 0x002)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR         hdr;

   /** Magic key sequence informations. */
   T_MKS_INFOS_KEY_SEQUENCE  key_sequence_infos;

}  T_MKS_INFOS_KEY_SEQUENCE_MSG;
/*@}*/

/**
 * @name MKS_REMOVE_KEY_SEQUENCE_MSG
 *
 * Internal message.
 *
 * Message issued by MKS to MKS task.
 * This message is used to remove a magic key sequence.
 */
/*@{*/
/** Message ID. */
#define MKS_REMOVE_KEY_SEQUENCE_MSG (MKS_MESSAGES_OFFSET | 0x003)

/** Message structure. */
typedef struct 
{
   /** Message header. */
   T_RV_HDR    hdr;

   /** Magic key sequence name. */
   char        name[KPD_MAX_CHAR_NAME+1];

}  T_MKS_REMOVE_KEY_SEQUENCE_MSG;
/*@}*/


#endif /* _MKS_MESSAGES_I_H_ */
