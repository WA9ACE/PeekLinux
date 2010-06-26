#ifndef __OBEXUTL_H
#define __OBEXUTL_H
/***************************************************************************
 *
 * File:        obex_utl.h
 *
 * Description: This file contains the function prototypes and structures
 *              that comprise the MD5 algorithm used for OBEX Authentication
 *              and OBEX Session support.
 *
 * Created:     October 10, 2001
 *
 * Version:     MTObex 3.5
 *
 * Copyright 2001-2006 Extended Systems, Inc.
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
#include "obex.h"

#if XA_DEBUG == XA_ENABLED
extern const char *ObParserEventStr[];
#endif

#if OBEX_SESSION_SUPPORT == XA_ENABLED || OBEX_AUTHENTICATION == XA_ENABLED

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * ObSessionError type
 *
 * Indicates the error code from the Session Sequence Number 
 * verification routines.
 */
typedef U8      ObSessionError;

#define ERR_NONE                    0x00    /* No Error */
#define ERR_RETRANSMIT              0x01    /* Retransmit last packet */
#define ERR_RETRANSMIT_NO_HEADERS   0x02    /* Retransmit last packet w/o headers */
#define ERR_RETRANSMIT_AND_IGNORE   0x03    /* Retransmit occurring, ignore 
                                             * duplicate data */
#define ERR_DISCONNECT              0x04    /* Disconnect transport */
#define ERR_ABORT                   0x05    /* Abort operation */
#define ERR_NO_SEQ_NUM_HEADER       0xFF    /* No sequence number header received */
/* End of ObSessionError */

/*--------------------------------------------------------------------------
 * OBEX Session Parameter header tag values
 */
#define SESSION_TAG_DEV_ADDR        0
#define SESSION_TAG_NONCE           1
#define SESSION_TAG_ID              2
#define SESSION_TAG_SEQ_NUM         3
#define SESSION_TAG_TIMEOUT         4
#define SESSION_TAG_OPCODE          5

/* Undefine session opcode values that are only used by the application.
 * These DO NOT equate to the actual OBEX Session opcode values, which
 * start with OB_SESSION_OPCODE_Xxx.
 */
#undef OB_OPCODE_CREATE_SESSION
#undef OB_OPCODE_CLOSE_SESSION
#undef OB_OPCODE_SUSPEND_SESSION
#undef OB_OPCODE_RESUME_SESSION
#undef OB_OPCODE_SET_SESSION_TIMEOUT
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * AUTH_NONCE_LEN constant
 *
 *     Defines the fixed size of an OBEX authentication nonce 
 *     (and digest).
 */
#define AUTH_NONCE_LEN              16

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/****************************************************************************
 *
 * Internal Function Prototypes
 *
 ****************************************************************************/
typedef struct _xMD5Context {
    U32     buf[4];
    U32     bytes[2];
    U32     in[16];
} xMD5Context;

/****************************************************************************
 *
 * Internal Functions used by OBEX
 *
 ****************************************************************************/

/* Functions for calculating MD5 digest */
void MD5(void *dest, void *orig, U16 len);
void xMD5Init(xMD5Context *context);
void xMD5Update(xMD5Context *context, const U8 *buf, U16 len);
void xMD5Final(U8 digest[AUTH_NONCE_LEN], xMD5Context *context);

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED || OBEX_AUTHENTICATION == XA_ENABLED */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/* Function for calculating Nonce and Session Id */
void ObCreateNonce(U8 *dest);
#if OBEX_ROLE_CLIENT == XA_ENABLED
ObSessionError VerifyClientSequenceNum(ObexClientApp *ObexApp, U8 seqNum);
extern BOOL ProcessResumeSequenceNum(ObexClientApp *ObexApp, U8 seqNum);
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

BOOL ObexParseSessionParms(ObexAppHandle *AppHndl, ObexSessionParms *sp);

/****************************************************************************
 *
 * Internal Macros used by OBEX
 *
 ****************************************************************************/

/* Determine Device Address length based on the transport type */
U8   GetDevAddrLen(ObexTpType *trans);
#define GetDevAddrLen(_TRANS)           ((U8)((_TRANS == OBEX_TP_BLUETOOTH)? 6: 4))

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

#endif  /* __OBEXUTL_H */
