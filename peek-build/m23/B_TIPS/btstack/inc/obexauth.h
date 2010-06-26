#ifndef __OBEXAUTH_H
#define __OBEXAUTH_H
/***************************************************************************
 *
 * File:        obexauth.h
 *
 * Description: This file contains the definitions and function prototypes 
 *              that comprise the Authentication Extensions to the OBEX 
 *              Protocol API.
 *
 * Created:     September 4, 2000
 *
 * Version:     MTObex 3.5
 *
 * Copyright 2000-2006 Extended Systems, Inc.
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

#include "obconfig.h"
#include "sys/obex_utl.h"

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEX Authentication API layer
 *
 *     The OBEX Authentication API adds a robust set of functions that 
 *     simplify supporting OBEX authentication. These functions exist
 *     when the configuration option OBEX_AUTHENTICATION is enabled.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Section: Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ObexAuthOpt type
 *
 *     The ObexAuthOpt type is used with the authentication challenge to
 *     convey additional information about the challenge.
 */
typedef U8  ObexAuthOpt;

#define AUTH_OPT_DEFAULT        0x00    /* No options specified */
#define AUTH_OPT_SEND_USERID    0x01    /* UserId must be sent in response */
#define AUTH_OPT_READ_ONLY      0x02    /* For Read Only Access */

/* End of ObexAuthOpt */

/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ObexAuthChallenge structure
 *
 *     This structure encapsulates the information necessary to build an
 *     authentication challenge. It is filled in by the application and
 *     passed to the Client or Server BuildAuthChallenge() functions.
 */
typedef struct _ObexAuthChallenge {

    /* The options field contains the authentication options. The
     * default value for this field is zero.
     */
    ObexAuthOpt options;

    /* The challenge string. This value is used to build the challenge
     * nonce. It should be a non-repeating value, and is typically 
     * based on a timestamp and a private-key.
     */
    U8         *challenge;

    /* The length in bytes of the challenge string */
    U8          challengeLen;

    /* The realm is a displayable string indicating which userid and/or
     * password to use. The first byte of the string is the character set.
     * The ASCII character set is value 0, refer to the OBEX spec for the
     * remainder of the character set definitions. The realm is optional.
     */
    U8         *realm;

    /* The length in bytes of the realm string. */
    U8          realmLen;

} ObexAuthChallenge;

/*---------------------------------------------------------------------------
 * ObexAuthChallengeInfo structure
 *
 *     This structure contains the information received in an authentication
 *     challenge header. It is used by the application in conjunction with
 *     the Client or Server ParseAuthChallenge() function to parse and store
 *     received challenge information.
 */
typedef struct _ObexAuthChallengeInfo {

#if OBEX_MAX_REALM_LEN > 0
    /* If present in the challenge, the realm is copied here. If the 
     * received realm exceeds the array size, it will be copied and truncated.
     */
    U8          realm[OBEX_MAX_REALM_LEN];

    /* The length in bytes of the received realm. */
    U8          realmLen;
#endif /* OBEX_MAX_REALM_LEN > 0 */

    /* Received options based on the ObexAuthOpt type */
    ObexAuthOpt options;

    /* The nonce received in the request. This is fed back into the 
     * authentication process when a response is built.
     */
    U8          nonce[AUTH_NONCE_LEN];

} ObexAuthChallengeInfo;

/*---------------------------------------------------------------------------
 * ObexAuthResponse structure
 *
 *     This structure encapsulates the information necessary to build an
 *     authentication response. It is filled in by the application and
 *     passed to the Client or Server BuildAuthResponse() functions.
 */
typedef struct _ObexAuthResponse {
    /* The password string is combined with the nonce received in the 
     * challenge to build the digest. The password is NOT sent over OBEX.
     */
    U8          *password;

    /* The length of the password string. */
    U8           passwordLen;

    /* The userId string is optional. Unless the AUTH_OPT_SEND_USERID
     * flag was set in the challenge. The userId can be up to 20 bytes.
     */
    U8          *userId;

    /* The length of the userId string */
    U8           userIdLen;

} ObexAuthResponse;

/*---------------------------------------------------------------------------
 * ObexAuthResponseInfo structure
 *
 *     This structure is used to hold the information received in an 
 *     authentication response header. It is used by the application in 
 *     conjunction with the Client or Server ParseAuthChallenge() function to 
 *     parse and store received response information.
 */
typedef struct _ObexAuthResponseInfo {
#if OBEX_MAX_USERID_LEN > 0
    /* If present in the response, the userId is copied here. If the received
     * userId exceeds the array size, it will be copied and truncated.
     */
    U8          userId[OBEX_MAX_USERID_LEN];

    /* The length of the userId. */
    U8          userIdLen;
#endif /* OBEX_MAX_USERID_LEN > 0 */

    /* The challenge nonce for which this is a response to. This may not
     * be valid if only one challenge header was sent.
     */
    U8          nonce[AUTH_NONCE_LEN];

    /* The digest represents the challenge nonce combined with password.
     * It should be compared against a locally generated digest to verify
     * the remote user.
     */
    U8          digest[AUTH_NONCE_LEN];

} ObexAuthResponseInfo;

/*---------------------------------------------------------------------------
 * ObexAuthResponseVerify structure
 *
 *     This structure is used once an authentication response has been 
 *     received, to verify the received digest. The application builds
 *     this structure based on information from the challenge, the response
 *     and the user and passes it to OBEX_VerifyAuthResponse() for processing.
 */
typedef struct _ObexAuthResponseVerify {

    /* The nonce sent in the challenge. The challenge nonce was generated into
     * the NonceOut parameter to Client/ServerBuildAuthChallenge(). 
     */
    U8     *nonce;

    /* The digest received from the remote device. This value can be found 
     * in the ObexAuthResponseInfo.digest field.
     */
    U8     *digest;

    /* The user's password string. */
    U8     *password;

    /* The length of the password string. */
    U8      passwordLen;

} ObexAuthResponseVerify;


/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * OBEXH_BuildAuthChallenge()
 *
 *     OBEX applications use this function to build an Authentication
 *     Challenge header. The header is constructed from the field values
 *     in the ObexAuthChallenge structure.
 *
 * Requires:
 *     OBEX_AUTHENTICATION be XA_ENABLED
 *
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 *     Challenge - This structure provides the content for the OBEX challenge
 *          header.
 *
 *     NonceOut - This output parameter is the Nonce which was built in the
 *          challenge header. It should be saved by the app and presented
 *          to the verification function when the response is received.
 *          The NonceOut must point to AUTH_NONCE_LEN bytes.
 *
 * Returns:
 *     TRUE - If header was successfully built.
 *     FALSE - Header was not built because of insufficient header space.
 */
BOOL OBEXH_BuildAuthChallenge(ObexAppHandle *AppHndl, ObexAuthChallenge *Challenge, 
                              U8 *NonceOut);

/*---------------------------------------------------------------------------
 * OBEXH_BuildAuthResponse()
 *
 *     This function is used by server applications to build an Authentication
 *     response to a received Challenge. The header is constructed from the
 *     fields in the ObexAuthResponse structure.
 *
 * Requires:
 *     OBEX_AUTHENTICATION be XA_ENABLED
 *
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 *     Response - This structure provides the information built into the OBEX
 *          Authentication Response header.
 *
 *     NonceIn - The Nonce received in the Authentication Challenge.
 *
 * Returns:
 *     TRUE - If header was successfully built.
 *     FALSE - Header was not built because of insufficient header space.
 */
BOOL OBEXH_BuildAuthResponse(ObexAppHandle *AppHndl, ObexAuthResponse *Response, 
                             U8 *NonceIn);

/*---------------------------------------------------------------------------
 * OBEXH_ParseAuthChallenge()
 *
 *     Applications use this function to collect Authentication Challenge
 *     information as it is indicated in OBxx_HEADER_RX events. This
 *     function converts the raw header data into the provided structure.
 *     It must be called during every OBxx_HEADER_RX event when the header
 *     type is OBEX_AUTH_CHAL.
 *
 * Requires:
 *     OBEX_AUTHENTICATION be XA_ENABLED
 *     
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 *     Info - This structure receives the parsed authentication challenge.
 *         Its contents should be cleared prior to the first call to this 
 *         function for a particular OBEX_AUTH_CHAL header.
 *
 * Returns:
 *     TRUE - When the entire header has been received and parsed.
 *     FALSE - If there is more header information expected.
 */
BOOL OBEXH_ParseAuthChallenge(ObexAppHandle *AppHndl, ObexAuthChallengeInfo *Info);

/*---------------------------------------------------------------------------
 * OBEXH_ParseAuthResponse()
 *
 *     Applications use this function to collect Authentication Response
 *     information as it is indicated in OBxx_HEADER_RX events. This
 *     function converts the raw header data into the provided structure.
 *     It must be called during every OBxx_HEADER_RX event when the header
 *     type is OBEX_AUTH_RESP.
 *
 * Requires:
 *     OBEX_AUTHENTICATION be XA_ENABLED
 *     
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 *     Info - This structure receives the parsed authentication response.
 *         Its contents should be cleared prior to the first call to this 
 *         function for a particular OBEX_AUTH_RESP header.
 *
 * Returns:
 *     TRUE - When the entire header has been received and parsed.
 *     FALSE - If there is more header information expected.
 */
BOOL OBEXH_ParseAuthResponse(ObexAppHandle *AppHndl, ObexAuthResponseInfo *Info);

/*---------------------------------------------------------------------------
 * OBEXH_VerifyAuthResponse()
 *
 *     This function is used by both client & servers to verify the received
 *     authentication response digest against the expected value.
 *
 * Requires:
 *     OBEX_AUTHENTICATION be XA_ENABLED
 *
 * Parameters:
 *     Verify - The verify structure contains information from the 
 *          authentication request, response and the users password.
 *
 * Returns:
 *     TRUE - If the response is authenticated.
 *     FALSE - The response did not match the expected value.
 */
BOOL OBEXH_VerifyAuthResponse(ObexAuthResponseVerify *Verify);

#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
#endif  /* __OBEXAUTH_H */

