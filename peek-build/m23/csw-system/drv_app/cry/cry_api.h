/**
 * @file	cry_api.h
 *
 * API Definition for CRY SWE.
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

#ifndef __CRY_API_H_
#define __CRY_API_H_

#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @name CRY type definitions and definitionss.
 *
 */
/*@{*/
typedef T_RV_RET T_CRY_RETURN;

#define CRY_DES_BLOCK_SIZE    8 /* Used block size in bytes */
#define CRYPTO_MAX_KEY_LEN		8 /* maximum key size in units of bytes */
#define CRYPTO_BLOCK_SIZE		  8	/* block size in units of bytes       */
#define CRYPTO_CONTEXT_SIZE  28 /* Size of context structure          */
#define CRYPTO_KEY_SIZE      32 /* Size of key structure              */

typedef enum
  { CRYPTO_FORCE_CPU, CRYPTO_FORCE_DMA, CRYPTO_DMA_AUTO }
  T_CRYPTO_DMA_MODE;

typedef enum{
            CRYPTO_ECB,
            CRYPTO_CBC
} T_CRYPTO_MODE;

typedef enum{
            CRYPTO_DES,
            CRYPTO_3DES
} T_CRYPTO_ALGORITM;


/*@}*/

/**
 * @name API functions
 *
 * API functions declarations.
 */
/*@{*/

/**
 * @name crypto_des_key_create
 *
 * Detailed description.
 * This function creates a DES-key. The client has to provide
 * an address of a structure where the values will be copied to.
 *
 * @param	algorithm: DES algorithm (DES or 3DES)
 * @param	key1_len: Length of key1
 * @param	key1_p: DES key 1.
 * @param	key2_len: Length of key2
 * @param	key2_p: DES key 2.
 * @param	key3_len: Length of key3
 * @param	key3_p: DES key 3.
 * @param	des_key_p: The DES-key, which is allocated by the client and filled 
 *                   by the driver.
 * @return	RV_OK:  The API function was successfully executed.
 * @return	RV_INVALID_PARAMETER: invalid parameter (e.g. key_length > 8)
 * @return	RV_NOT_READY: Driver is not ready
 */
T_RV_RET crypto_des_key_create(T_CRYPTO_ALGORITM algorithm,
                               UINT8 key1_len, UINT8 *key1_p,
                               UINT8 key2_len, UINT8 *key2_p,
                               UINT8 key3_len, UINT8 *key3_p,
                               void *des_key_p);

/**
 * @name crypto_des_key_get
 *
 * Detailed description.
 * This function creates a DES-key. The client has to provide
 * an address of a structure where the values will be copied to.
 *
 * @param	algorithm: DES algorithm (DES or 3DES)
 * @param	key1_len: Length of key1
 * @param	key1_p: DES key 1.
 * @param	key2_len: Length of key2
 * @param	key2_p: DES key 2.
 * @param	key3_len: Length of key3
 * @param	key3_p: DES key 3.
 * @param	des_key_p: The DES-key, which is allocated by the client and filled 
 *                   by the driver.
 * @return	RV_OK:  The API function was successfully executed.
 * @return	RV_INVALID_PARAMETER: invalid parameter (e.g. key_length > 8)
 * @return	RV_NOT_READY: Driver is not ready
 */
T_RV_RET crypto_des_key_get(void* des_key_p,
                            T_CRYPTO_ALGORITM *algorithm_p,
                            UINT8 *key1_len_p, UINT8 *key1_p,
                            UINT8 *key2_len_p, UINT8 *key2_p,
                            UINT8 *key3_len_p, UINT8 *key3_p);
/**
 * @name crypto_des_begin
 *
 * Detailed description.
 * This function initialises a DES-context. The client has to provide
 * an address of a structure where the values will be copied to.
 *
 * @param	mode: DES mode (ECB or CBC)
 * @param	des_key: DES-key which is generated with crypto_des_key_create
 * @param	des_context_p: The DES-context, which is allocated by the client and 
 *                       filled by the driver.
 * @param	return_path: return path of the function
 * @param	iv_len: Length of Initialisation Vector.
 * @param	iv_p: DES Initialisation Vector.
 * @return	RV_OK:  The API function was successfully executed.
 * @return	RV_INVALID_PARAMETER: invalid parameter (e.g. des_key = NULL)
 * @return	RV_NOT_READY: Driver is not ready
 */
T_RV_RET crypto_des_begin(T_CRYPTO_MODE mode, void *des_key_p,
                          void *des_context_p,
                          T_RV_RETURN return_path,
                          UINT16 ivlen, UINT8 *iv_p);

/**
 * @name crypto_des_encrypt
 *
 * Detailed description.
 * This function encrypts data. It sends an encrypt request
 * message with all the necessary parameters.
 *
 * @param	des_context_p: DES context which is created with crypto_des_begin
 * @param	length: Length of the data to encrypt
 * @param	plaintext_p: Data to be encrypted.
 * @param	ciphertext_p: Data that has been encrypted.
 * @return	RV_OK:  The API function was successfully executed.
 * @return	RV_MEMORY_ERR: Memory error (out of memory).
 * @return	RV_NOT_READY: Driver is not ready
 */
T_RV_RET crypto_des_encrypt(void *des_context_p, UINT32 length,
                            UINT8 *plaintext_p, UINT8 *ciphertext_p);

/**
 * @name crypto_des_decrypt
 *
 * Detailed description.
 * This function decrypts data. It sends a decrypt request
 * message with all the necessary parameters.
 *
 * @param	des_context_p: DES context which is created with crypto_des_begin
 * @param	length: Length of the data to decrypt
 * @param	ciphertext_p: Data to be decrypted.
 * @param	plaintext_p: Data that has been decrypted.
 * @return	RV_OK:  The API function was successfully executed.
 * @return	RV_MEMORY_ERR: Memory error (out of memory).
 * @return	RV_NOT_READY: Driver is not ready
 */
T_RV_RET crypto_des_decrypt(void *des_context_p, UINT32 length,
                            UINT8 *ciphertext_p, UINT8 *plaintext_p);

/**
 * @name crypto_get_sw_version
 *
 * Detailed description.
 * This function gets the software version of the driver.
 *
 * @return	SW_VERSION: software version of the driver.
 */
UINT32 crypto_get_sw_version(void);

/**
 * @name crypto_get_hw_version
 *
 * Detailed description.
 * This function gets the hardware version of the driver.
 *
 * @return  hardware version
 */
UINT32 crypto_get_hw_version(void);

/**
 * @name crypto_set_dma_mode
 *
 * Detailed description.
 * This function sets the DMA mode
 *
 * @return  RV_OK or RV_INVALID_PARAMETER
 */
T_RV_RET crypto_set_dma_mode(T_CRYPTO_DMA_MODE mode);
/*@}*/

#ifdef __cplusplus
}
#endif


#endif /*__CRY_API_H_*/

