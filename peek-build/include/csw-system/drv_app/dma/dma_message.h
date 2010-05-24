/**
 * @file  dma_message.h
 *
 * Data structures:
 * 1) used to send messages to the DMA SWE,
 * 2) DMA can receive.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/2/2003   ()   Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __DMA_MESSAGE_H_
#define __DMA_MESSAGE_H_


#include "rv/rv_general.h"

#include "dma/dma_cfg.h"
#include "dma/dma_api.h"

#ifdef __cplusplus
extern "C"
{
#endif


/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */
#define DMA_MESSAGE_OFFSET   BUILD_MESSAGE_OFFSET(DMA_USE_ID)


/**
 * @name DMA_STATUS_MSG
 *
 * This message is sent to a client to provide the result of a 
 * command or to give notice of an asynchro-nous event occurs.
 *
 */
/*@{*/
/** Message ID. */
#define DMA_STATUS_RSP_MSG (DMA_MESSAGE_OFFSET | 0x001)

typedef struct {
  T_RV_HDR     hdr;
  T_DMA_RESULT result;
} T_DMA_STATUS_RSP_MSG;


/**
 * @name DMA_RESERVE_CHANNEL_MSG
 *
 * This message is send by the client to request a channel. 
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define DMA_RESERVE_CHANNEL_REQ_MSG (DMA_MESSAGE_OFFSET | 0x002)

typedef struct {
  T_RV_HDR       hdr;
  T_DMA_SPECIFIC specific;
  T_DMA_CHANNEL  channel;
  T_DMA_QUEUE    queue;
  T_DMA_SW_PRIORITY sw_priority;
  T_RV_RETURN    return_path;
} T_DMA_RESERVE_CHANNEL_REQ_MSG;


/**
 * @name DMA_REMOVE_FROM_QUEUE_REQ_MSG
 *
 * This message is send by the client to provide detailed 
 * channel information specifying the DMA transfer. 
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define DMA_REMOVE_FROM_QUEUE_REQ_MSG (DMA_MESSAGE_OFFSET | 0x003)



typedef struct {
  T_RV_HDR      hdr;
  T_DMA_CHANNEL channel_queue_id;
} T_DMA_REMOVE_FROM_QUEUE_REQ_MSG;

/**
 * @name DMA_SET_CHANNEL_PARAMETERS_MSG
 *
 * This message is send by the client to provide detailed 
 * channel information specifying the DMA transfer. 
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define DMA_SET_CHANNEL_PARAMETERS_REQ_MSG (DMA_MESSAGE_OFFSET | 0x004)



typedef struct {
  T_RV_HDR      hdr;
  T_DMA_CHANNEL channel;
  T_DMA_CHANNEL_PARAMETERS channel_info;
} T_DMA_SET_CHANNEL_PARAMETERS_REQ_MSG;


/**
 * @name DMA_ENABLE_TRANSFER_MSG
 *
 * This message is send by the client to enable the DMA transfer. 
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define DMA_ENABLE_TRANSFER_REQ_MSG (DMA_MESSAGE_OFFSET | 0x005)

typedef struct {
  T_RV_HDR      hdr;
  T_DMA_CHANNEL channel;
} T_DMA_ENABLE_TRANSFER_REQ_MSG;


/**
 * @name DMA_RELEASE_CHANNEL_REQ_MSG
 *
 * This message is send by the client to release a reserved channel. 
 *
 * Detailled description
 */
/*@{*/
/** Message ID. */
#define DMA_RELEASE_CHANNEL_REQ_MSG (DMA_MESSAGE_OFFSET | 0x006)

typedef struct {
  T_RV_HDR      hdr;
  T_DMA_CHANNEL channel;
} T_DMA_RELEASE_CHANNEL_REQ_MSG;


T_RVM_RETURN dma_handle_message (T_RV_HDR *msg_p);


#ifdef __cplusplus
}
#endif

#endif /* __DMA_MESSAGE_H_ */
