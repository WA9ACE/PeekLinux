/**
 * @file	cry_message.h
 *
 * Data structures:
 * 1) used to send messages to the CRY SWE,
 * 2) CRY can receive.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/14/2003	 ()		Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __CRY_MESSAGE_H_
#define __CRY_MESSAGE_H_

#include "rv/rv_general.h"
#include "cry/cry_api.h"
#include "cry/cry_i.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "cry/cry_cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif


/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
#define CRYPTO_MESSAGE_OFFSET	 BUILD_MESSAGE_OFFSET(CRY_USE_ID)

/**
 * @name CRY_ENCRYPT_REQ_MSG
 *
 * Short description. Message used to request an encryption
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define CRYPTO_ENCRYPT_REQ_MSG (CRYPTO_MESSAGE_OFFSET | 0x001)
/** Message structure. */
typedef struct{
              T_RV_HDR      os_hdr;
              void          *des_context_p;
              UINT32        length;
              UINT8         *plaintext_p;
              UINT8         *cipherText_p;
} T_CRYPTO_ENCRYPT_REQ_MSG;
/*@}*/

/**
 * @name CRY_ENCRYPT_RSP_MSG
 *
 * Short description. Message used when encryption is handled
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define CRYPTO_ENCRYPT_RSP_MSG (CRYPTO_MESSAGE_OFFSET | 0x002)
/** Message structure. */
typedef struct{
              T_RV_HDR      os_hdr;
              T_RV_RET      result; /* <RV_OK | RV_INVALID_PARAMETER |
                                        RV_NOT_READY | RV_INTERNAL_ERROR>; */
              UINT32        length;
              UINT8         *plaintext_p;
              UINT8         *cipherText_p;
} T_CRYPTO_ENCRYPT_RSP_MSG;
/*@}*/

/**
 * @name CRY_DECRYPT_REQ_MSG
 *
 * Short description. Message used to request an decryption
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define CRYPTO_DECRYPT_REQ_MSG (CRYPTO_MESSAGE_OFFSET | 0x003)
/* Message structure. */
typedef struct{
              T_RV_HDR      os_hdr;
              void          *des_context_p;
              UINT32        length;
              UINT8         *plaintext_p;
              UINT8         *cipherText_p;
} T_CRYPTO_DECRYPT_REQ_MSG;
/*@}*/

/**
 * @name CRY_DECRYPT_RSP_MSG
 *
 * Short description. Message used when decryption is handled
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define CRYPTO_DECRYPT_RSP_MSG (CRYPTO_MESSAGE_OFFSET | 0x004)
/** Message structure. */
typedef struct{
              T_RV_HDR      os_hdr;
              T_RV_RET      result; /* <RV_OK | RV_INVALID_PARAMETER |
                                        RV_NOT_READY | RV_INTERNAL_ERROR>; */
              UINT32        length;
              UINT8         *plaintext_p;
              UINT8         *cipherText_p;
} T_CRYPTO_DECRYPT_RSP_MSG;
/*@}*/


#ifdef __cplusplus
}
#endif

#endif /* __CRY_MESSAGE_H_ */
