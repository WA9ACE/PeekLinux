#ifndef __OBEXERRH
#define __OBEXERRH

/****************************************************************************
 *
 * File:          obexerr.h
 *
 * Description:   This file specifies the OBEX Protocol error codes
 * 
 * Created:       August 14, 1997
 *
 * Version:       MTObex 3.5
 *
 * Copyright 1997-2006 Extended Systems, Inc.
 * Portions copyright 2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "xatypes.h"

/*---------------------------------------------------------------------------
 * OBEX Errors layer
 *
 *     This section defines error codes used by OBEX.
 */

/****************************************************************************
 *
 * Section: Types
 *
 ****************************************************************************/

/*--------------------------------------------------------------------------
 * ObexRespCode type
 *
 *     Response codes used to signify the status of an OBEX operation. They
 *     are not standard function return codes, but rather are returned by
 *     ObStore functions to then be included within OBEX Response packets,
 *     which are sent out to other devices.
 *
 *     The first two response codes listed below are success responses;
 *     the remainder are considered failures. The failure codes are
 *     frequently associated with aborted operations.
 */
typedef U8 ObexRespCode;

/* Group: Successful response codes */

#define OBRC_CONTINUE             0x10 /* Continue */
#define OBRC_SUCCESS              0x20 /* OK, Success */

/* Group: Failure response codes */

#define OBRC_CREATED              0x21 /* Created */
#define OBRC_ACCEPTED             0x22 /* Accepted */
#define OBRC_NON_AUTHOR_INFO      0x23 /* Non-Authoritative Information */
#define OBRC_NO_CONTENT           0x24 /* No Content */
#define OBRC_RESET_CONTENT        0x25 /* Reset Content */
#define OBRC_PARTIAL_CONTENT      0x26 /* Partial Content */

#define OBRC_MULTIPLE_CHOICES     0x30 /* Multiple Choices */
#define OBRC_MOVED_PERMANENT      0x31 /* Moved Permanently */
#define OBRC_MOVED_TEMPORARY      0x32 /* Moved Temporarily */
#define OBRC_SEE_OTHER            0x33 /* See Other */
#define OBRC_NOT_MODIFIED         0x34 /* Not Modified */
#define OBRC_USE_PROXY            0x35 /* Use Proxy */

#define OBRC_BAD_REQUEST          0x40 /* Bad Request */
#define OBRC_UNAUTHORIZED         0x41 /* Unauthorized */
#define OBRC_PAYMENT_REQUIRED     0x42 /* Payment Required */
#define OBRC_FORBIDDEN            0x43 /* Forbidden - operation is understood
                                        * but refused */
#define OBRC_NOT_FOUND            0x44 /* Not Found */
#define OBRC_METHOD_NOT_ALLOWED   0x45 /* Method Not Allowed */
#define OBRC_NOT_ACCEPTABLE       0x46 /* Not Acceptable */
#define OBRC_PROXY_AUTHEN_REQ     0x47 /* Proxy Authentication Required */
#define OBRC_REQUEST_TIME_OUT     0x48 /* Request Timed Out */
#define OBRC_CONFLICT             0x49 /* Conflict */

#define OBRC_GONE                 0x4a /* Gone */
#define OBRC_LENGTH_REQUIRED      0x4b /* Length Required */
#define OBRC_PRECONDITION_FAILED  0x4c /* Precondition Failed */
#define OBRC_REQ_ENTITY_TOO_LARGE 0x4d /* Requested entity is too large */
#define OBRC_REQ_URL_TOO_LARGE    0x4e /* Requested URL is too large */
#define OBRC_UNSUPPORT_MEDIA_TYPE 0x4f /* Unsupported Media Type */

#define OBRC_INTERNAL_SERVER_ERR  0x50 /* Internal Server Error */
#define OBRC_NOT_IMPLEMENTED      0x51 /* Not Implemented */
#define OBRC_BAD_GATEWAY          0x52 /* Bad Gateway */
#define OBRC_SERVICE_UNAVAILABLE  0x53 /* Service Unavailable */
#define OBRC_GATEWAY_TIMEOUT      0x54 /* Gateway Timeout */
#define OBRC_HTTP_VER_NO_SUPPORT  0x55 /* HTTP version not supported */

#define OBRC_DATABASE_FULL        0x60 /* Database Full */
#define OBRC_DATABASE_LOCKED      0x61 /* Database Locked */

/* End of ObexRespCode */

#endif /* __OBEXERRH */

