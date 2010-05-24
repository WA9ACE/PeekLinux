/**
 * @file	gbi_message_i.h
 *
 * Internal messsage definition
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	03/12/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __GBI_MESSAGE_I_H_
#define __GBI_MESSAGE_I_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "gbi/gbi_message.h"

#define GBI_I_MSG_OFFSET 	(LAST_USED_GBI_EXTERNAL_MSG_OFFSET + 1)

/**
 * @name GBI_I_SUBSCRIBE_EVENT_REQ_MSG
 *
 * Detailled description
 * This message is used to subscribe the client to an event
 */
/*@{*/

/** Subscribe event request. */
#define GBI_I_SUBSCRIBE_EVENT_REQ_MSG (GBI_MESSAGE_OFFSET | GBI_I_MSG_OFFSET)

typedef struct {
    T_RV_HDR 		    hdr;
		T_GBI_EVENTS		event;
		T_RV_RETURN 		return_path;
} T_GBI_I_SUBSCRIBE_EVENT_REQ_MSG;

/** Subscribe event response. */
#define GBI_I_SUBSCRIBE_EVENT_RSP_MSG (GBI_MESSAGE_OFFSET | (GBI_I_MSG_OFFSET + 1)) ) )

typedef struct {
    T_RV_HDR 		    hdr;
    T_GBI_RESULT	  result;
} T_GBI_I_SUBSCRIBE_EVENT_RSP_MSG;

/*@}*/



/**
 * @name GBI_I_STARTUP_EVENT_REQ_MSG
 *
 * Detailled description
 * This message is send to the GBI mailbox at startup
 */
/*@{*/

/** Subscribe event request. */
#define GBI_I_STARTUP_EVENT_REQ_MSG (GBI_MESSAGE_OFFSET | GBI_I_MSG_OFFSET + 2)

typedef struct {
    T_RV_HDR 		    hdr;
} T_GBI_I_STARTUP_EVENT_REQ_MSG;


/**
 * @name GBI_SINGLE_REQ_MSG
 *
 * Detailled description
 * This is single request message is to obtain the media and partition information 
 * form a set of GBI messages containing this information
 */
/*@{*/

#define GBI_SINGLE_REQ_MSG (GBI_MESSAGE_OFFSET | (GBI_I_MSG_OFFSET + 3))

typedef struct {
    T_RV_HDR 		  hdr;
    UINT8         media_nmb;
    UINT8         partition_nmb;
} T_GBI_SINGLE_REQ_MSG;


/**
 * @name GBI_RESULT_RSP_MSG
 *
 * Detailled description
 * This is dummy message to obtain the result of a response message
 */
/*@{*/

#define GBI_RESULT_RSP_MSG (GBI_MESSAGE_OFFSET | (GBI_I_MSG_OFFSET + 4))

typedef struct {
    T_RV_HDR 	        hdr;
		T_RV_RET		      result;
} T_GBI_RESULT_RSP_MSG;

/* Union for combining multiple request messages */
typedef union  {
    T_GBI_I_STARTUP_EVENT_REQ_MSG 	startup_msg;
		T_GBI_MEDIA_INFO_REQ_MSG	      media_info_msg;
		T_GBI_PARTITION_INFO_REQ_MSG		partition_info_msg;
} T_MULTIPLE_REQ_MSG;


#ifdef __cplusplus
}
#endif

#endif /* __GBI_MESSAGE_I_H_ */

