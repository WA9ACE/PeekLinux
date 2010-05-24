/**
 * @file	hash_message.h
 *
 * Data structures:
 * 1) used to send messages to the SHA SWE,
 * 2) SHA can receive.
 *
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/23/2003	raymond zandbergen ()		Create.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */

#ifndef __HASH_MESSAGE_H_
#define __HASH_MESSAGE_H_


#include "rv/rv_general.h"

#include "hash/hash_cfg.h"


#ifdef __cplusplus
extern "C"
{
#endif


/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
#define HASH_MESSAGE_OFFSET	 BUILD_MESSAGE_OFFSET(HASH_USE_ID)



/**
 * @name HASH_SAMPLE_MESSAGE
 *
 * Short description.
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define HASH_SAMPLE_MESSAGE (HASH_MESSAGE_OFFSET | 0x001)

/** Message structure. */
  typedef struct
  {
        /** Message header. */
    T_RV_HDR hdr;

        /** Some parameters. */
    /* ... */

  }
  T_HASH_SAMPLE_MESSAGE;
/*@}*/


#ifdef __cplusplus
}
#endif

#endif                          /* __HASH_MESSAGE_H_ */
