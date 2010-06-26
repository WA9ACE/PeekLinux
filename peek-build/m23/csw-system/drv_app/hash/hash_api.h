/**
 * @file	hash_api.h
 *
 * API Definition for SHA SWE.
 *
 * @author	raymond zandbergen ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					          Modification
 *	-------------------------------------------------------------------
 *	7/23/2003	raymond zandbergen (ICT)		Create.
 *	7/31/2003	Eddy Oude Middendorp(ICT)		Completion/board/pc.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */

#ifndef __HASH_API_H_
#define __HASH_API_H_


#include "rvm/rvm_gen.h"            /* Generic RVM types and functions. */


#ifdef __cplusplus
extern "C"
{
#endif

#define HASH_CONTEXT_SIZE	(128) //86

#define HASH_MESSAGE_OFFSET         BUILD_MESSAGE_OFFSET(HASH_USE_ID)
#define HASH_SHA1_HASH_REQ_MSG	    (HASH_MESSAGE_OFFSET+0)
#define HASH_MD5_HASH_REQ_MSG	      (HASH_MESSAGE_OFFSET+1)
#define HASH_SHA1_REQ_DATA_RSP_MSG	(HASH_MESSAGE_OFFSET+3)
#define HASH_SHA1_RESULT_RSP_MSG	  (HASH_MESSAGE_OFFSET+4)
#define HASH_MD5_REQ_DATA_RSP_MSG	  (HASH_MESSAGE_OFFSET+5)
#define HASH_MD5_RESULT_RSP_MSG	    (HASH_MESSAGE_OFFSET+6)

typedef enum {
  HASH_FORCE_CPU,
  HASH_FORCE_DMA,
  HASH_DMA_AUTO
}T_HASH_DMA_MODE;

typedef struct {
  T_RV_HDR    os_hdr;
  T_RV_RETURN return_path;
  void        *context_p;
  UINT32      length;
  UINT8       *data_begin_p;
  BOOL        last_block;
}T_HASH_SHA1_HASH_REQ_MSG;

typedef struct {
  T_RV_HDR    os_hdr;
  T_RV_RETURN return_path;
  void        *context_p;
  UINT32      length;
  UINT8       *data_begin_p;
  BOOL        last_block;
}T_HASH_MD5_HASH_REQ_MSG;

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  return_status;
  UINT32    length;
  UINT8     *data_begin_p;
}T_HASH_SHA1_REQ_DATA_RSP_MSG;

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  return_status;
  UINT32    length;
  UINT8     *data_begin_p;
}T_HASH_MD5_REQ_DATA_RSP_MSG;

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  return_status;
  UINT32    digest_a;
  UINT32    digest_b;
  UINT32    digest_c;
  UINT32    digest_d;
  UINT32    digest_e;
  UINT32    length;
  UINT8     *data_begin_p;
}T_HASH_SHA1_RESULT_RSP_MSG;

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  return_status;
  UINT32    digest_a;
  UINT32    digest_b;
  UINT32    digest_c;
  UINT32    digest_d;
  UINT32    length;
  UINT8     *data_begin_p;
}T_HASH_MD5_RESULT_RSP_MSG;

/**
 * @name API functions
 *
 * API functions declarations (bridge functions).
 */
/*@{*/

/**
 * function hash_sha1_begin
 *
 * Detailled description.
 * This function initialises an SHA-1 context. The client has to reserve memory for
 * this context at the size of HASH_CONTEXT_SIZE bytes and provide a pointer to it.
 * This pointer is to be used in the subse-quent call to hash_sha1_hash().
 * Also the return path must be given to provide the driver with information about
 * how to notify the client of asynchronous events.
 * No hash operation is performed with this function, only initialisation is done here.
 * 
 * @param	context_p: existing context which must be initialized
 * @param	return_path: The riviera structure for providing a message ID or call-back
 *                     pointer to allow client notification of asynchronous events.
 * @return  result
 */
  extern T_RV_RET hash_sha1_begin (void *context_p, T_RV_RETURN return_path);

/**
 * function hash_sha1_hash
 *
 * Detailled description.
 * This function supplies the data that has to be hashed. The function is to be
 * used as many times as required to make up the total hash length. The last
 * data block must be marked. Processing of the command is done asynchronously
 * and may be queued for execution. Queued commands are executed on a FIFO base
 * To prevent a single hash command for taken too much time, individual data
 * blocks should be limited (e.g. 64 Kbytes). Because of the queuing, the data
 * of the last invocation must be kept available till the hash-driver sends an
 * asynchronous message reply. With this message the driver may either be
 * requesting new data or is providing the hash result. 
 * 32-bit data alignment in blocks of a multiple of 32-bits is advisable.
 * 
 * @param	context_p: existing and initialized context
 * @param	length: Length of the data to hashed
 * @param	data_begin_p: Data to be hashed.
 * @param	last_block: indication of last data.
 * @return  result
 */
  extern T_RV_RET hash_sha1_hash (void *context_p, UINT32 length,
                                  UINT8 * data_begin_p, BOOL last_block);
/**
 * function hash_md5_begin
 *
 * Detailled description.
 * This function initialises an MD5 context. The client has to reserve memory for
 * this context at the size of HASH_CONTEXT_SIZE bytes and provide a pointer to it.
 * This pointer is to be used in the subse-quent call to hash_sha1_hash().
 * Also the return path must be given to provide the driver with information about
 * how to notify the client of asynchronous events.
 * No hash operation is performed with this function, only initialisation is done here.
 * 
 * @param	context_p: existing context which must be initialized
 * @param	return_path: The riviera structure for providing a message ID or call-back
 *                     pointer to allow client notification of asynchronous events.
 * @return  result
 */
  extern T_RV_RET hash_md5_begin (void *context_p, T_RV_RETURN return_path);

/**
 * function hash_md5_hash
 *
 * Detailled description.
 * This function supplies the data that has to be hashed. The function is to be
 * used as many times as required to make up the total hash length. The last
 * data block must be marked. Processing of the command is done asynchronously
 * and may be queued for execution. Queued commands are executed on a FIFO base
 * To prevent a single hash command for taken too much time, individual data
 * blocks should be limited (e.g. 64 Kbytes). Because of the queuing, the data
 * of the last invocation must be kept available till the hash-driver sends an
 * asynchronous message reply. With this message the driver may either be
 * requesting new data or is providing the hash result.
 * 32-bit data alignment in blocks of a multiple of 32-bits is advisable.
 * 
 * @param	context_p: existing and initialized context
 * @param	length: Length of the data to hashed
 * @param	data_begin_p: Data to be hashed.
 * @param	last_block: indication of last data.
 * @return  result
 */
  extern T_RV_RET hash_md5_hash (void *context_p, UINT32 length,
                                 UINT8 * data_begin_p, BOOL last_block);
/**
 * function hash_set_dma
 *
 * Detailled description.
 * With this function the client can set the mode for copying the hash data to
 * the hardware FIFO.
 * IMPORTANT: The function is not for regular use. It is available for test
 * purpose only. Changing the mode is only possible when no hashing is busy
 * and no hash request is queued.
 * Function has no effect in this version.
 * 
 * @param	dma_mode: · HASH_FORCE_CPU | · HASH_FORCE_DMA | HASH_DMA_AUTO 
 * @return  result
 */
  extern T_RV_RET hash_set_dma (T_HASH_DMA_MODE hash_dma_mode);

/**
 * function hash_get_sw_version
 *
 * Detailled description.
 * This function returns the software version of the driver.
 * The version is a 32-bit value, which is organ-ised as follows:
 * bit  0 - 15 : build number
 * bit 16 - 23 : minor version number
 * bit 24 - 31 : major version number
 * 
 * @return  Software version information
 */
  extern UINT32 hash_get_sw_version (void);

/**
 * function hash_get_hw_version
 *
 * Detailled description.
 * This function returns the hardware version number (module revision)
 * of the controller. This number is stored in the Module Revision Register
 * (SHA_REV) of the controller.
 * The version is a 32-bit value, which is organ-ised as follows:
 * bit  0 - 15 : minor version number
 * bit 16 - 23 : majo version number
 * bit 24 - 31 : reserved
 * 
 * @return  Hardware version information
 */
  extern UINT32 hash_get_hw_version (void);

/*@}*/


#ifdef __cplusplus
}
#endif


#endif /*__HASH_API_H_*/
