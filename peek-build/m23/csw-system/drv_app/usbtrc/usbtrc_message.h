/**
 * @file
 *
 * @brief Declaration of messages exchanged with USBTRC.
 *
 * This file declares all data structures :
 * -# used to send messages to the USBTRC SWE,
 * -# the USBTRC SWE can receive.
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	4/14/2004	Charles-Hubert BESSON		Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */

#ifndef __USBTRC_MESSAGE_H_
#define __USBTRC_MESSAGE_H_


#include "rv/rv_general.h"

#include "usbtrc/usbtrc_cfg.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief The message offset of the USBTRC SWE.
 *
 * The message offset must differ for each SWE so that each message
 * can be uniquely identified in the system.
 */
#define USBTRC_MESSAGE_OFFSET	 BUILD_MESSAGE_OFFSET(USBTRC_USE_ID)



/**
 * @name USBTRC_SAMPLE_MESSAGE
 *
 * @brief This is the brief description of the sample message.
 *
 * And here is the detailled description.
 */
/*@{*/

/// Sample message identifier.
#define USBTRC_SAMPLE_MESSAGE (USBTRC_MESSAGE_OFFSET | 0x001)

/// Sample message structure.
typedef struct 
{
	/// Message header.
	T_RV_HDR	hdr;

	/// First field of the sample message.
	//T_USBTRC_ANY	first_field;

	// ...
} T_USBTRC_SAMPLE_MESSAGE;

/*@}*/


#ifdef __cplusplus
}
#endif

#endif // __USBTRC_MESSAGE_H_
