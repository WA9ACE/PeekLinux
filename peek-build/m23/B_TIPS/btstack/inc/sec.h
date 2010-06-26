#ifndef __SEC_H
#define __SEC_H

/****************************************************************************
 *
 * File:
 *     $Workfile:sec.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:57$
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     security manager functionality of the Management Entity.
 * 
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
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

#include "me.h"

#if TI_CHANGES == XA_ENABLED
#include "sdp.h"
#endif /* TI_CHANGES == XA_ENABLED */

/*---------------------------------------------------------------------------
 * Security Manager (SEC) layer
 *
 *     The Security Manager provides access to the security features
 *     offered by Bluetooth radio hardware. This includes encryption,
 *     authentication, and pairing services.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtPairingType type
 *
 *     Bluetooth Pairing type. Indicates the action that should be taken during
 *     pairing. A link key is generated as a result of pairing. What happens to
 *     the link key is determined by the setting of BtPairingType.
 */
typedef U8 BtPairingType;

#define BPT_NOT_SAVED        0x00 /* Do not save the link key */
#define BPT_SAVE_NOT_TRUSTED 0x01 /* Save the link key but make not trusted */
#define BPT_SAVE_TRUSTED     0x02 /* Save the link key and make trusted */
#define BPT_NOT_SAVE_TRUSTED 0x03 /* Do not save the link key and make trusted */

/* End of BtPairingType */


/*---------------------------------------------------------------------------
 * BtSecurityLevel type
 *
 *     Authorization and encryption require authentication. So if either of
 *     these two are set then authentication must also be set.
 */
typedef U8 BtSecurityLevel;

#define BSL_AUTHENTICATION_IN  0x01 /* Authentication on incoming required */
#define BSL_AUTHORIZATION_IN   0x02 /* Authorization on incoming required */
#define BSL_ENCRYPTION_IN      0x04 /* Encryption on incoming required */

#define BSL_AUTHENTICATION_OUT 0x08 /* Authentication on outgoing required */
#define BSL_AUTHORIZATION_OUT  0x10 /* Authorization on outgoing required */
#define BSL_ENCRYPTION_OUT     0x20 /* Encryption on outgoing required */

#define BSL_BROADCAST_IN       0x40 /* Reception of broadcast packets allowed */
#define BSL_NO_SECURITY        0x00

#define BSL_DEFAULT  (BSL_AUTHENTICATION_IN | BSL_AUTHORIZATION_IN |\
                      BSL_AUTHENTICATION_OUT)

/* End of BtSecurityLevel */


/*---------------------------------------------------------------------------
 * BtEncryptMode type
 */
typedef U8 BtEncryptMode;

#define BECM_ENCRYPT_DISABLE 0x00
#define BECM_ENCRYPT_ENABLE  0x01

/* End of BtEncryptMode */


/*---------------------------------------------------------------------------
 * BtSecurityMode type
 */
typedef U8 BtSecurityMode;

#define BSM_SEC_DISABLED    0x01
#define BSM_SEC_LEVEL_1     0x02
#define BSM_SEC_LEVEL_2     0x03
#define BSM_SEC_LEVEL_3     0x04
#define BSM_SEC_ENCRYPT     0x05

/* End of BtSecurityMode */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtSecurityRecord structure
 *
 *     Security records are not stored in non-volatile memory.
 */
typedef struct _BtSecurityRecord
{
    ListEntry  node; /* Used internally by the stack */

    /* Group: The user must fill in the three fields below */
    BtProtocolId    id;
    U32             channel;
    BtSecurityLevel level;

#if TI_CHANGES == XA_ENABLED
	SdpServicesMask	service;	/* Associated service only one bit is '1' */
#endif /* TI_CHANGES == XA_ENABLED */

} BtSecurityRecord;


/*---------------------------------------------------------------------------
 * BtSecurityToken structure
 *
 *     Security tokens are used to request access by protocol
 *     layers and multiplexors.
 */
typedef struct _BtSecurityToken
{
    ListEntry  node;   /* Used internally by the stack */

    /* Group: The following parameters must be set before calling
     * SEC_AccessRequest.
     */
    BtProtocolId    id;      /* Protocol ID of caller */

    /* Pointer to remote device for which the access is being requested */
    BtRemoteDevice *remDev;  

    /* The multiplexing channel number. For example for L2CAP it is the PSM 
     * and for RFCOMM it is the Server ID (incoming only) or a pointer to an 
     * RfChannel structure (outgoing only).
     */
    U32             channel;

    /* Incoming must be set to TRUE if access is desired for a connection 
     * request from a remote (incoming). It must be set to FALSE if the 
     * request is for a connection to remote device (outgoing).
     */
    BOOL            incoming; 

    /* Group: The stack uses the following fields are internally. */
    BtEventType         result;
    BtSecurityRecord   *record;
    BtSecurityLevel     level;
    EvmTimer            timer;
} BtSecurityToken;

#if BT_SECURITY == XA_ENABLED
/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * SEC_RegisterPairingHandler()
 *
 *     Register the handler that will handle all pairing.  This 
 *     handler is called when pairing is required. Pairing occurs 
 *     for two reasons. The first is when a device initiates the 
 *     bonding procedure. The second is when authentication is
 *     required and a link does not exist. 
 *
 *     If no handler is set (handler == 0) then the pairing
 *     procedure will fail. This function is used to set pairable
 *     mode. The old handler is returned when the function is 
 *     called.
 *
 *     The pairing handler is called with the following events:
 *
 *     BTEVENT_PIN_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates a PIN code is needed. The handler provides a PIN
 *     code by calling SEC_SetPin. If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous authentication request has
 *     been cancelled.
 *
 *     BTEVENT_PAIRING_COMPLETE - This event indicates that pairing
 *     has completed successfully and the link is authenticated.
 *     The "errCode" field of the BtEvent indicates the status of
 *     storing the link key. If pairing fails no event will be 
 *     sent (there is no event sent by the radio module indicating 
 *     failure).
 *
 * Parameters:
 *     handler - pointer to the handler for dealing with pairing.
 *         This handler can be a registered global handler but does
 *         not have to be.
 *
 * Returns:
 *     Pointer to the old handler.
 */
BtHandler *SEC_RegisterPairingHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * SEC_RegisterAuthorizeHandler()
 *
 *     Register the handler that will handle all authorization.  
 *     This handler is called when authorization is needed as
 *     part of security level 2.  
 *
 *     If no handler is set (handler == 0) then the authorization
 *     procedure will fail. 
 *
 *     The authorization handler is called with the following events:
 *
 *     BTEVENT_AUTHORIZATION_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates a request for authorization. The link needing authorization
 *     is passed in the "remDev" field of BtEvent. During the callback, the
 *     handler can call SEC_GetSecurityRecord to obtain more information on
 *     what service or channel is being authorized. If "errCode" is NOT
 *     BEC_NO_ERROR, this event indicates a previous authorization request
 *     has been cancelled.
 *
 * Parameters:
 *     handler - pointer to the handler for dealing with 
 *         authorization.  This handler can be a registered global 
 *         handler or even the same handler that does pairing but it
 *         does not have to be.
 *
 * Returns:
 *     Pointer to the old handler.
 */
BtHandler *SEC_RegisterAuthorizeHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * SEC_AccessRequest()
 *
 *     Determine if access is allowed for the device specified
 *     by rm at the protocol specified by id for the given
 *     channel. The incoming parameter indicates if the request
 *     is incoming or outgoing.
 *
 * Parameters:
 *     token - pointer to a Security token. The fields of the
 *         token must be filled in appropriately. If BT_STATUS_PENDING
 *         is returned the token cannot be modified until the protocol's
 *         callback is called.
 *     
 * Returns:
 *     BT_STATUS_SUCCESS - Access is approved
 *
 *     BT_STATUS_PENDING - The operation has started. The results
 *         will be returned via the protocol's callback identified by
 *         id. The callback is called with "eventType"
 *         BTEVENT_ACCESS_APPROVED or BTEVENT_ACCESS_DENIED.
 *
 *     BT_STATUS_FAILED - Access is denied.
 *
 *     BT_STATUS_INVALID_PARM - Access is denied. Token is not
 *         properly initialized, token is 0 (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_AccessRequest(BtSecurityToken *token);

/*---------------------------------------------------------------------------
 * SEC_CancelAccessRequest()
 *
 *     SEC_CancelAccessRequest cancels the security procedure associated with
 *     the "token" BtSecurityToken.
 *
 * Parameters:
 *     token - pointer to a Security token with active access request.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - cancel operation successful
 *
 *     BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to cancel access request.
 */
BtStatus SEC_CancelAccessRequest(BtSecurityToken *token);

/*---------------------------------------------------------------------------
 * SEC_Register()
 *
 *     Register the security record with the security manager.
 *
 * Parameters:
 *     sr - A pointer to the security record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - security record is registered.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the
 *         security record pointer is 0, or the security level is
 *         not properly formed. (error check only).
 *
 *     BT_STATUS_IN_USE - operation failed because the protocol ID and
 *         channel in security record was already registered. 
 *
 *     BT_STATUS_FAILED - operation failed because record is
 *         already on the list (XA_ERROR_CHECK only).
 */
BtStatus SEC_Register(BtSecurityRecord *sr);


/*---------------------------------------------------------------------------
 * SEC_Unregister()
 *
 *     Unregister the security record with the security manager.
 *
 * Parameters:
 *     sr - A pointer to the security record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - security record is unregistered.
 *
 *     BT_STATUS_FAILED - operation failed because record is not
 *         on the list. 
 */
BtStatus SEC_Unregister(BtSecurityRecord *sr);

/*---------------------------------------------------------------------------
 * SEC_Authorize()
 *
 *     If "isAuthorized" is set to TRUE then temporary authorization 
 *     for the remote device is granted otherwise it is denied. If
 *     the "futureTrust" parameter is set to TRUE then the device
 *     is marked as a trusted device in the device database. The 
 *     trust mark holds beyond this connection. If "futureTrust" is
 *     FALSE then the device database record is not modified.
 *     The authorization handler calls this function after 
 *     receiving a BTEVENT_AUTHORIZATION_REQ when "errCode" is
 *     BEC_NO_ERROR.
 *
 * Parameters:
 *     rm - Pointer to remote device structure.
 *
 *     isAuthorized - TRUE indications authorization is granted.
 *         FALSE indicates authorization is denied.
 *
 *     futureTrust - If set to TRUE the device is marked as trusted
 *         in the device database. If set to FALSE the device database 
 *         record is not modified.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to accept authorization.
 *
 *     BT_STATUS_DEVICE_NOT_FOUND - the authorization was successful
 *         but the device data base write failed because the device was
 *         not found in the database or the write operation to the 
 *         database failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device pointer is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_Authorize(BtRemoteDevice *rm, BOOL isAuthorized,
                       BOOL futureTrust);
#define SEC_Authorize(rm, au, tr) (SEC_AuthorizeServiceB(rm, au, tr, FALSE))

/*---------------------------------------------------------------------------
 * SEC_AuthorizeService()
 *
 *     SEC_AuthorizeService is similar to SEC_Authorize, but temporarily
 *     authorizes a device for a specific service or channel. If
 *     "isAuthorized" is set to TRUE then temporary authorization 
 *     for a service is granted to the remote device, otherwise it
 *     is denied.  There is no "futureTrust" parameter as this authorization
 *     is always temporary for a one time only access of the service. 
 *     The authorization handler calls this function after receiving
 *     a BTEVENT_AUTHORIZATION_REQ event when "errCode" is BEC_NO_ERROR.
 *     The event handler can access the security record registered for the
 *     service with the SEC_GetSecurityRecord API.
 *
 * Parameters:
 *     rm - Pointer to remote device structure.
 *
 *     isAuthorized - TRUE indications authorization is granted.
 *         FALSE indicates authorization is denied.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to accept authorization.
 *
 *     BT_STATUS_DEVICE_NOT_FOUND - the authorization was successful
 *         but the device data base write failed because the device was
 *         not found in the database or the write operation to the 
 *         database failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device pointer is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_AuthorizeService(BtRemoteDevice *rm, BOOL isAuthorized);
#define SEC_AuthorizeService(rm, au) (SEC_AuthorizeServiceB(rm, au, FALSE, TRUE))

/* Prototype for actual SEC_Authorize and SEC_AuthorizeService implementation */
BtStatus SEC_AuthorizeServiceB(BtRemoteDevice *rm, BOOL isAuthorized,
                               BOOL futureTrust, BOOL authService);

/*---------------------------------------------------------------------------
 * SEC_GetSecurityRecord()
 *
 *     SEC_GetSecurityRecord retrieves the security record associated with
 *     a BTEVENT_AUTHORIZATION_REQ event. The API must be used only during
 *     the BTEVENT_AUTHORIZATION_REQ event. An authorization handler might
 *     use this call to determine what service or channel is being authorized.
 *
 * Parameters:
 *     rm - Pointer to remote device structure.
 *
 * Returns:
 *     pointer to a BtSecurityRecord structure
 */
BtSecurityRecord *SEC_GetSecurityRecord(BtRemoteDevice *rm);
#define SEC_GetSecurityRecord(rm) ((BtSecurityRecord*)(rm)->secRec)

/*---------------------------------------------------------------------------
 * SEC_SetPin()
 *
 *     Set the pin value for the device specified by rm. This
 *     function is called as a response to the BTEVENT_PIN_REQ
 *     event when "errCode" is BEC_NO_ERROR. Setting pin to 0
 *     rejects the pairing operation.
 *     
 * Parameters:
 *     rm - Pointer to remote device structure
 *
 *     pin - pointer to pin. Pin set to 0 indicates that
 *         the pairing operation should be rejected. 
 *
 *     len - number of bytes in the pin.
 *
 *     type - indicates how the link key should be handled.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation is started successful. The
 *         pairing handler will be called with the BTEVENT_PAIRING_COMPLETE
 *         event when pairing is complete.
 *
 *     BT_STATUS_FAILED - operation failed because the remote
 *         device structure is not in a state where a pin is needed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid or the pin is not set to 0 but
 *         the len is 0 or greater than 16.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_SetPin(BtRemoteDevice *rm, const U8* pin, U8 len, BtPairingType type);

/*---------------------------------------------------------------------------
 * SEC_AuthenticateLink()
 *
 *     Authenticate an existing link with a remote device. If
 *     a link key does not exist then pairing will be attempted.
 *     Pairing events will be sent to the Pairing handler. If
 *     a pairing handler does not exist then pairing will fail 
 *     which in turn will cause authentication to fail. 
 *     BTEVENT_AUTHENTICATE_CNF will be sent to the handler provided.
 *     The "errCode" field of the BtEvent specifies if the operation 
 *     was successful or not.
 *
 *     If the operation is successful then BTEVENT_AUTHENTICATED will
 *     be sent to all registered global handlers and all handlers
 *     bound to the remote device structure. Note that this event does not
 *     occur if the remote device initiated an authentication operation
 *     because authentication is a one-way operation.
 *
 *     The link must be active (not in hold, sniff, park or in the
 *     process of being disconnected). Also it must not already
 *     be authenticated.
 *
 * Parameters:
 *     handler - handler to receive authenticate results. This can
 *         be the registered global handler or a handler bound to
 *         a remote device or 0. Failures are sent to this handler
 *         only. The handler can be 0 if desired, though no handler
 *         will receive failure notifications in this case.
 *
 *     remDev - pointer to remote device structure. 
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *
 *     BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform authentication or is already
 *         authenticated.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the "remDev" is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_AuthenticateLink(BtHandler *handler, BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * SEC_SetLinkEncryption()
 *
 *     Enable or disable link level encryption for the give link.
 *     If the function returns BT_STATUS_PENDING then 
 *     BTEVENT_ENCRYPT_COMPLETE will be sent to the handler. The "errCode"
 *     field will indicate the result and the "p.remDev" field will 
 *     indication the remote device.
 *
 *     The operation is successful then all handlers bound to the
 *     remote device and all registered global handlers will receive 
 *     the BTEVENT_ENCRYPTION_CHANGE event. The "p.encrypt.remDev" field 
 *     indications the link and the "p.encrypt.mode" field indicates 
 *     the new encryption mode.
 *      
 *     The link must be active (not in hold, sniff, park or in the
 *     process of being disconnected). Also it must not already
 *     be in the process of performing an encryption operation.
 *
 * Parameters:
 *     handler - handler to receive encryption results. This can
 *         be the registered global handler or a handler bound to
 *         a remote device or 0. Failures are sent to this handler
 *         only. The handler can be 0 if desired, though no handler
 *         will receive failure notifications in this case.
 *
 *     remDev - pointer to remote device structure. 
 *
 *     mode - desired encryption mode (enabled or disabled)
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         When the operation is complete the handler will be
 *         called with the BTEVENT_ENCRYPT_COMPLETE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - the link is already in the desired mode.
 *         No events will be generated.
 *
 *     BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform encryption. The link must
 *         be authenticated before encryption is allowed.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the handler, "remDev" or mode are invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_SetLinkEncryption(BtHandler *handler, BtRemoteDevice *remDev,
                               BtEncryptMode mode);

/*---------------------------------------------------------------------------
 * SEC_EnableSecurityMode3()
 *
 *     Enable security mode 3. That means all connections
 *     (incoming and outgoing) must be authenticated and if
 *     the encryption parameter is set to TRUE then encryption
 *     is required. If any client sets security mode 3 then
 *     security mode 3 is in force for all. The security Mode 2
 *     security records are still checked and authorization
 *     may still be required. Existing connections are not affected.
 *
 *     If the function returns BT_STATUS_PENDING then the handler
 *     is called with BTEVENT_SECURITY3_COMPLETE when the operation
 *     is finished. The "errCode" field indicates the status of the
 *     operation and the "p.secMode.mode" field indicates the new 
 *     security mode. Success of the operation is based on whether
 *     authentication is set or not. If setting the authentication
 *     enable is successful but setting encryption mode fails the 
 *     operation is still successful. The "p.secMode.encrypt" field
 *     shows the status of the encryption mode setting. TRUE means
 *     that encryption mode is active FALSE means it is not active.
 *
 *     If the operation is successful (as defined above) then all 
 *     registered global handlers will receive BTEVENT_SECURITY_CHANGE 
 *     event. The same fields as used above provide the results of
 *     the change.
 *
 * Parameters:
 *     handler - A pointer to a handler. It can be a registered
 *         handler, the pairing handler, or the authorization handler
 *         but doesn't have to be. Failures are sent only to this
 *         handler.
 *
 *     encryption - If set to TRUE then encryption is required.
 *         Otherwise encryption is not required.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has started successfully.
 *         When the operation is complete the handler will be
 *         called with the BTEVENT_SECURITY3_COMPLETE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - the operation is successful. The
 *         security mode was already set to 3. No events will be
 *         generated.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the handler is 0 (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_FAILED - operations failed because change of
 *         security mode is already in progress.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_EnableSecurityMode3(BtHandler *handler, BOOL encryption);


/*---------------------------------------------------------------------------
 * SEC_DisableSecurityMode3()
 *
 *     Disable security mode. The Security Manager now operates
 *     in security mode 2.
 *
 *     If the function returns BT_STATUS_PENDING then the handler
 *     is called with BTEVENT_SECURITY3_COMPLETE when the operation
 *     is finished. The "errCode" field indicates the status of the
 *     operation and the "p.secMode.mode" field indicates the new 
 *     security mode. The "p.secMode.encrypt" field indicates the
 *     state of encryption mode. If the operation is successful 
 *     then all registered global handlers will receive 
 *     BTEVENT_SECURITY_CHANGE event. 
 *
 * Parameters:
 *     handler - A pointer to a handler.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has started successfully.
 *         When the operation is complete the handler will be
 *         called with the BTEVENT_SECURITY3_COMPLETE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - the operation is successful. The
 *         security mode was already set to 3. No events will be
 *         generated.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the handler is 0 (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_FAILED - operations failed because change of
 *         security mode is already in progress.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_DisableSecurityMode3(BtHandler *handler);


/*---------------------------------------------------------------------------
 * SEC_DeleteDeviceRecord()
 *
 *     Delete a record with the given "bdAddr" from the device
 *     database. 
 *
 * Parameters:
 *     bdAddr - pointer to 48-bit address of device record to delete.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *
 *     BT_STATUS_FAILED - operation failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because bdAddr
 *         is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_DeleteDeviceRecord(const BD_ADDR *bdAddr);


#if TI_CHANGES == XA_ENABLED
/*---------------------------------------------------------------------------
 * SEC_DeleteAllDeviceRecords()
 *
 *     Delete all records from the device database.
 *
 * Parameters:
 *     void.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *
 *     BT_STATUS_FAILED - operation failed.
 */
BtStatus SEC_DeleteAllDeviceRecords(void);
#endif /* TI_CHANGES == XA_ENABLED */


/*---------------------------------------------------------------------------
 * SEC_AddDeviceRecord()
 *
 *     Add a record to the device database. A pointer to a
 *     record is passed to the function . The contents of the
 *     record are copied into the device database. If a record
 *     with the same BD_ADDR already exists then the existing
 *     record is changed. This function can be used to
 *     preload the device database.
 *
 * Parameters:
 *     record - pointer to record which is to be copied into the
 *     database.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record written successfully.
 *
 *     BT_STATUS_FAILED - record was not written.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because record
 *         is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_AddDeviceRecord(const BtDeviceRecord *record);


/*---------------------------------------------------------------------------
 * SEC_FindDeviceRecord()
 *
 *     Find the device record with the given bdAddr. The record
 *     is copied to the location pointed to by the record parameter.
 *
 * Parameters:
 *     bdAddr - pointer to 48-bit address of device record to find.
 *
 *     record - pointer to location to receive record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *
 *     BT_STATUS_FAILED - operation failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because bdAddr
 *         or record is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_FindDeviceRecord(const BD_ADDR *bdAddr, BtDeviceRecord *record);

#if TI_CHANGES == XA_ENABLED
/* Gili - this function should be merged with SEC_FindDeviceRecord. there should be one function that
returns whether key exists or not and another that returns the record see DDB_FindRecord.*/
BtStatus SEC_FindDeviceRecordRegardlessKeyInfo(const BD_ADDR *bdAddr, BtDeviceRecord *record);
#endif



/*---------------------------------------------------------------------------
 * SEC_EnumDeviceRecords()
 *
 *     Enumerate the device database. The first record is at index 0. The
 *     intent of this function is to enumerate all records in the device
 *     database in a loop. If records are added or deleted in between calls
 *     to this function then all records may not be returned or duplicate
 *     records could be returned.
 *
 * Parameters:
 *     i - index of desired record. The first record is at index 0.
 *
 *     record - pointer to location to receive record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful. 
 *         Record is returned
 *
 *     BT_STATUS_FAILED - operation failed. No record is
 *         returned. This occurs if the index is greater than
 *         the number of records or an error occurred in reading
 *         the database.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because record
 *         is invalid (XA_ERROR_CHECK only).
 */
BtStatus SEC_EnumDeviceRecords(I16 i, BtDeviceRecord *record);
#endif /* BT_SECURITY */

#endif /* __SEC_H */
