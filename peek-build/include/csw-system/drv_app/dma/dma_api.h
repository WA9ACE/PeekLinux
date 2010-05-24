/**
 * @file  dma_api.h
 *
 * API Definition for DMA SWE.
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

#ifndef __DMA_API_H_
#define __DMA_API_H_


#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */
#include "dma/sys_dma.h"
#include "chipset.cfg" 

#ifdef __cplusplus
extern "C"
{
#endif


/** Defines the channel number. */
typedef UINT8  T_DMA_CHANNEL;

/*  Minimum channel number */
/* !!!!! */
/* It seems that Layer1 use DMA channels 0 and 1. Please beaware !! */ 
/* !!!!! 
 */
#define DMA_MIN_CHANNEL   (2)

/*  Maximum channel number */
#define DMA_MAX_CHANNEL   (5)



/** This type definition holds the reply status 
    of a channel reservation request. 
  */
typedef UINT16 T_DMA_RET;


/** Specifies status and return information. */ 
typedef struct 
{
  T_DMA_RET     status;
  T_DMA_CHANNEL channel;
} T_DMA_RESULT;
/** Possible values for T_DMA_RET */
#define DMA_OK                  (0)
#define DMA_RESERVE_OK          (1)
#define DMA_INVALID_PARAMETER   (2)
#define DMA_MEMORY_ERROR        (3)
#define DMA_CHANNEL_BUSY        (4)
#define DMA_NOT_READY           (5)
#define DMA_QUEUED              (6)
#define DMA_NO_CHANNEL          (7) 
#define DMA_TOO_MANY_REQUESTS   (8)
#define DMA_ACTION_NOT_ALLOWED  (9)
#define DMA_TIMEOUT_SOURCE      (10)
#define DMA_TIMEOUT_DESTINATION (11)
#define DMA_MISS_EVENT          (12)
#define DMA_COMPLETED           (13)
#define DMA_CHANNEL_RELEASED    (14)
#define DMA_CHANNEL_ENABLED     (15)
#define DMA_QUEUE_PROC          (16)
#define DMA_REMOVED_FROM_QUEUE  (17)
#define DMA_PARAM_SET           (18)
#define DMA_BUF1_READY          (19)
#define DMA_BUF2_READY          (20)
#define DMA_HALF_BLOCK          (21)


/** Defines if the channel reservation is for a 
    specific channel number or not. 
  */
typedef UINT8   T_DMA_SPECIFIC;
#define DMA_CHAN_ANY      (0)
#define DMA_CHAN_SPECIFIC (1)


/** Defines if the channel reservation has to 
    be queued when it can not be granted immediately. 
  */
typedef UINT8   T_DMA_QUEUE;
#define DMA_QUEUE_DISABLE (0)
#define DMA_QUEUE_ENABLE  (1)

/** Defines with which priority a reservation has to be served 
    when it is queued and a dma channel becomes free. The lower
    the value of the priority, the higher the priority will be.
  */
typedef UINT8 T_DMA_SW_PRIORITY;

/** ! When someone changes one of the next two defines then also ! 
    ! ENABLE the parameter range checking in the file: dma_i.c,  ! 
    ! function: dma_queue_spec_prio_check                        ! 
  */
#define DMA_SW_PRIORITY_HIGHEST (0)
#define DMA_SW_PRIORITY_LOWEST (255)


/** This defines the source to be used for starting the DMA transfer. 
    The transfer can be started by source device, destination device 
    or by software (enable). 
  */
#ifdef _WINDOWS
typedef UINT16  T_DMA_SYNC_DEVICE;
#define DMA_SYNC_DEVICE_NONE         (0)
#define DMA_SYNC_DEVICE_HW_RIF_TX    (1)
#define DMA_SYNC_DEVICE_HW_RIF_RX    (2)
#define DMA_SYNC_DEVICE_HW_LCD       (3)
#define DMA_SYNC_DEVICE_UART1_TX     (4)
#define DMA_SYNC_DEVICE_UART1_RX     (5)
#define DMA_SYNC_DEVICE_UART2_TX     (6)
#define DMA_SYNC_DEVICE_UART2_RX     (7)
#define DMA_SYNC_DEVICE_UART_IRDA_TX (8)
#define DMA_SYNC_DEVICE_UART_IRDA_RX (9)
#define DMA_SYNC_DEVICE_USB_RX1      (10)
#define DMA_SYNC_DEVICE_USB_TX1      (11)
#define DMA_SYNC_DEVICE_USB_RX2      (12)
#define DMA_SYNC_DEVICE_USB_TX2      (13)
#define DMA_SYNC_DEVICE_USB_RX3      (14)
#define DMA_SYNC_DEVICE_USB_TX3      (15)
#define DMA_SYNC_DEVICE_MC_SD_RX     (16)
#define DMA_SYNC_DEVICE_MC_SD_TX     (17)
#define DMA_SYNC_DEVICE_MS_RX_TX     (18)
#define DMA_SYNC_DEVICE_USIM_RX      (19)
#define DMA_SYNC_DEVICE_USIM_TX      (20)
#define DMA_SYNC_DEVICE_UWIRE_RX_TX  (23)
#define DMA_SYNC_DEVICE_NAND_FLASH   (24)
#define DMA_SYNC_DEVICE_I2C_RX       (25)
#define DMA_SYNC_DEVICE_I2C_TX       (26)
#define DMA_SYNC_DEVICE_SHA1_TX      (27)
#define DMA_SYNC_DEVICE_DES_RX       (28) 
#define DMA_SYNC_DEVICE_DES_TX       (29)
#define DMA_SYNC_DEVICE_CPORT_RX     (30)
#define DMA_SYNC_DEVICE_CPORT_TX     (31)
#else
#if (CHIPSET == 15)
typedef T_DMA_TYPE_CHANNEL_HW_SYNCH  T_DMA_SYNC_DEVICE;
    #define   DMA_SYNC_DEVICE_NONE C_DMA_CHANNEL_NO_SYNCH 
    #define   DMA_SYNC_DEVICE_DRP_TX C_DMA_CHANNEL_DRP_TX              
    #define   DMA_SYNC_DEVICE_DRP_RX  C_DMA_CHANNEL_DRP_RX             
    #define   DMA_SYNC_DEVICE_HW_LCD C_DMA_CHANNEL_LCD                 
    #define   DMA_SYNC_DEVICE_UART_IRDA_MODEM_TX C_DMA_CHANNEL_UART_IRDA_MODEM_TX  
    #define   DMA_SYNC_DEVICE_UART_IRDA_MODEM_RX C_DMA_CHANNEL_UART_IRDA_MODEM_RX  
    #define   DMA_SYNC_DEVICE_UART_MSSPI_TX C_DMA_CHANNEL_UART_MSSPI_TX       
    #define   DMA_SYNC_DEVICE_UART_MSSPI_RX C_DMA_CHANNEL_UART_MSSPI_RX       
    #define   DMA_SYNC_DEVICE_USB_RX1 C_DMA_CHANNEL_USB_RX1			    
    #define   DMA_SYNC_DEVICE_USB_TX1 C_DMA_CHANNEL_USB_TX1             
    #define   DMA_SYNC_DEVICE_USB_RX2 C_DMA_CHANNEL_USB_RX2             
    #define   DMA_SYNC_DEVICE_USB_TX2 C_DMA_CHANNEL_USB_TX2             
    #define   DMA_SYNC_DEVICE_USB_RX3 C_DMA_CHANNEL_USB_RX3             
    #define   DMA_SYNC_DEVICE_USB_TX3 C_DMA_CHANNEL_USB_TX3             
    #define   DMA_SYNC_DEVICE_I2C_TRITON_TX C_DMA_CHANNEL_I2C_TRITON_RX       
    #define   DMA_SYNC_DEVICE_I2C_TRITON_RX C_DMA_CHANNEL_I2C_TRITON_TX       
    #define   DMA_SYNC_DEVICE_USIM_RX C_DMA_CHANNEL_USIM_RX             
    #define   DMA_SYNC_DEVICE_USIM_TX C_DMA_CHANNEL_USIM_TX             
    #define  DMA_SYNC_DEVICE_CAM_THRESHOLD  C_DMA_CHANNEL_CAM_THRESHOLD       
    #define  DMA_SYNC_DEVICE_NAND_FLASH C_DMA_CHANNEL_NAND_FLASH          
    #define   DMA_SYNC_DEVICE_I2C_RX C_DMA_CHANNEL_I2C_RX              
    #define   DMA_SYNC_DEVICE_I2C_TX C_DMA_CHANNEL_I2C_TX              
    #define   DMA_SYNC_DEVICE_SHA1_TX C_DMA_CHANNEL_SHA1_TX                // channel must be set as secured channel
    #define   DMA_SYNC_DEVICE_DES_RX C_DMA_CHANNEL_DES_RX                  // channel must be set as secured channel
    #define   DMA_SYNC_DEVICE_DES_TX C_DMA_CHANNEL_DES_TX                  // channel must be set as secured channel
    #define   DMA_SYNC_DEVICE_CPORT_RX C_DMA_CHANNEL_CPORT_RX            
    #define   DMA_SYNC_DEVICE_CPORT_TX C_DMA_CHANNEL_CPORT_TX             
#else
typedef T_DMA_TYPE_CHANNEL_HW_SYNCH  T_DMA_SYNC_DEVICE;
#define DMA_SYNC_DEVICE_NONE         C_DMA_CHANNEL_NO_SYNCH      
#define DMA_SYNC_DEVICE_HW_RIF_TX    C_DMA_CHANNEL_RIF_TX        
#define DMA_SYNC_DEVICE_HW_RIF_RX    C_DMA_CHANNEL_RIF_RX        
#define DMA_SYNC_DEVICE_HW_LCD       C_DMA_CHANNEL_LCD           
#define DMA_SYNC_DEVICE_UART1_TX     C_DMA_CHANNEL_UART_MODEM1_TX
#define DMA_SYNC_DEVICE_UART1_RX     C_DMA_CHANNEL_UART_MODEM1_RX
#define DMA_SYNC_DEVICE_UART2_TX     C_DMA_CHANNEL_UART_MODEM2_TX
#define DMA_SYNC_DEVICE_UART2_RX     C_DMA_CHANNEL_UART_MODEM2_RX
#define DMA_SYNC_DEVICE_UART_IRDA_TX C_DMA_CHANNEL_UART_IRDA_TX  
#define DMA_SYNC_DEVICE_UART_IRDA_RX C_DMA_CHANNEL_UART_IRDA_RX  
#define DMA_SYNC_DEVICE_USB_RX1      C_DMA_CHANNEL_USB_RX1       
#define DMA_SYNC_DEVICE_USB_TX1      C_DMA_CHANNEL_USB_TX1       
#define DMA_SYNC_DEVICE_USB_RX2      C_DMA_CHANNEL_USB_RX2       
#define DMA_SYNC_DEVICE_USB_TX2      C_DMA_CHANNEL_USB_TX2       
#define DMA_SYNC_DEVICE_USB_RX3      C_DMA_CHANNEL_USB_RX3       
#define DMA_SYNC_DEVICE_USB_TX3      C_DMA_CHANNEL_USB_TX3       
#define DMA_SYNC_DEVICE_MC_SD_RX     C_DMA_CHANNEL_MMC_SD_RX     
#define DMA_SYNC_DEVICE_MC_SD_TX     C_DMA_CHANNEL_MMC_SD_TX     
#define DMA_SYNC_DEVICE_MS_RX_TX     C_DMA_CHANNEL_MS_RX_TX      
#define DMA_SYNC_DEVICE_USIM_RX      C_DMA_CHANNEL_USIM_RX       
#define DMA_SYNC_DEVICE_USIM_TX      C_DMA_CHANNEL_USIM_TX       
#define DMA_SYNC_DEVICE_UWIRE_RX_TX  C_DMA_CHANNEL_UWIRE_RX_TX   
#define DMA_SYNC_DEVICE_NAND_FLASH   C_DMA_CHANNEL_NAND_FLASH    
#define DMA_SYNC_DEVICE_I2C_RX       C_DMA_CHANNEL_I2C_RX        
#define DMA_SYNC_DEVICE_I2C_TX       C_DMA_CHANNEL_I2C_TX        
#define DMA_SYNC_DEVICE_SHA1_TX      C_DMA_CHANNEL_SHA1_TX       
#define DMA_SYNC_DEVICE_DES_RX       C_DMA_CHANNEL_DES_RX         
#define DMA_SYNC_DEVICE_DES_TX       C_DMA_CHANNEL_DES_TX        
#define DMA_SYNC_DEVICE_CPORT_RX     C_DMA_CHANNEL_CPORT_RX      
#define DMA_SYNC_DEVICE_CPORT_TX     C_DMA_CHANNEL_CPORT_TX      
#endif
#endif
/* 
  This define is used for parameter checking. 
  Please update accoringly if needed 
 */
#define DMA_SYNC_DEVICE_MAX  DMA_SYNC_DEVICE_CPORT_TX     


/** Defines if the client is notified through the return path about 
    the completion of the DMA transfer. 
  */
typedef UINT8   T_DMA_END_NOTIFICATION;
#define DMA_NO_NOTIFICATION (0)
#define DMA_NOTIFICATION    (1)


/** This defines the data width for the channel transfer. */
typedef UINT8   T_DMA_DATA_WIDTH;
#define DMA_DATA_S8   (0) /* 8 bits  */
#define DMA_DATA_S16  (1) /* 16 bits */
#define DMA_DATA_S32  (2) /* 32 bits */


/** This defines if data must be packed or not.  The packing of a source or 
    destination device can be set separate. Packing can be set if the data 
    size of the port is less then the DMA data width of the port. Packing 
    increases the transfer rate. 
  */
typedef UINT8   T_DMA_PACKING;
#define DMA_NOT_PACKED (0) 
#define DMA_PACKED     (1) 


/** This defines if burst access should be used. The burst mode of a source 
    or destination device can be set separate. Bust allows consecutive bytes 
    to be to be packed is a way that it will result in a single transfer of 
    4 words of 32 bits. Burst is only available for IMIF access. 
  */
typedef UINT8   T_DMA_BURST;
#define DMA_NO_BURST      (0) 
#define DMA_BURST_ENABLED (2) 


/** This defines the hardware priority of the channel when it has to compete 
    with other internal channels. High priority channels are served before low 
    priority channels. Competing channels of the same priority are served on 
    a round robin manner.
  */
typedef UINT8   T_DMA_HW_PRIORITY;
#define DMA_HW_PRIORITY_LOW  (0) 
#define DMA_HW_PRIORITY_HIGH (1) 


/** This definition controls whether the DMA process is a single transfer 
    operation or a continuous one. In a continuos DMA process the DMA 
    controller re-initializes after a completed transfer.
  */
typedef UINT8   T_DMA_REPEAT;
#define DMA_SINGLE     (0) 
#define DMA_CONTINIOUS (1) 


/** This definition is only valid when a source or destination device has its 
    burst mode enabled (see T_DMA_BURST). 
    In the last burst cycle the source may fail to supply the remaining bytes 
    to complete the burst. By enabling the FIFO flush, an incomplete last 
    burst cycle is forced to complete. 
  */
typedef UINT8   T_DMA_FLUSH;
#define DMA_FLUSH_DISABLED  (0) 
#define DMA_FLUSH_ENABLED   (1) 


/** This controls the addressing mode towards the DMA device. The addressing 
    mode of both, the source and destination device can be set individually.
  */
typedef UINT8   T_DMA_ADDRESS_MODE;
#define DMA_ADDR_MODE_CONSTANT    (0)
#define DMA_ADDR_MODE_POST_INC    (1)
#define DMA_ADDR_MODE_FRAME_INDEX (2)


/** Frames are part of the definition of the number of bytes to transfer 
    (like elements and data width).

    TotalNumberOfBytes = NumberOfFrames x NumberOfElementsperFrame x 
                         NumberOfBytesPerElement
    i.e.
    TotalNumberOfBytes = T_DMA_FRAMES x T_DMA_ELEMENTS x T_DMA_DATA_WIDTH

    By carefully selecting the number of frames, elements per frame and bytes 
    per element the client has the possibility of using the DMA controller in
    the best possible way.
  */
typedef UINT16  T_DMA_FRAMES;


/** Elements are part of the definition of the number of bytes to transfer 
    (like frames and data width, see for more information T_DMA_FRAMES).
  */
typedef UINT16  T_DMA_ELEMENTS;


/** DMA channels can be restricted for secured DMA transfer. Access to those 
    secured channels is restricted to the ARM running an secure applications.
    Ones a channel is secured, writing of channel configuration data is having
    no effect and reading from the DMA controller always reads zero.
  */
typedef UINT8   T_DMA_SECURE;
#define DMA_NOT_SECURED (0) 
#define DMA_SECURED     (1) 


/** When a DMA is completed, there are two possibilities:
    The channel gets internal disabled. The client must enable a new transfer. 
    The mode is called 'single transfer mode'.
    The channel remains internal enabled. A new DMA request shall start a new
    transfer. The mode is called 'auto-initialization mode'.
  */
typedef UINT8   T_DMA_MODE;
#define DMA_MODE_SINGLE     (0)
#define DMA_MODE_AUTO_INIT  (1)


/** This define enables the DMA process to start when all the channel 
    parameters are handled. If software synchronization was selected, 
    the transfer starts immediately. If hardware synchronization was selected, 
    the controller is armed and starts as soon as the appropriate DMA 
    request arrives.
  */
typedef UINT8   T_DMA_TRANSFER;
#define DMA_MODE_TRANSFER_DISABLE  (0)
#define DMA_MODE_TRANSFER_ENABLE   (1)


/** This struct does contain all the parameters to set up an transfer
  */
typedef struct 
{
  T_DMA_DATA_WIDTH  data_width;
  T_DMA_SYNC_DEVICE sync;
  T_DMA_HW_PRIORITY hw_priority;
  T_DMA_FLUSH       flush;
  T_DMA_FRAMES      nmb_frames;
  T_DMA_ELEMENTS    nmb_elements;
  T_DMA_END_NOTIFICATION dma_end_notification;
  T_DMA_SECURE      secure;
  T_DMA_MODE        dma_mode;
  T_DMA_TRANSFER    transfer;

  UINT32            source_address;
  T_DMA_ADDRESS_MODE source_address_mode;
  T_DMA_PACKING     source_packet;
  T_DMA_BURST       source_burst;

  UINT32            destination_address;
  T_DMA_ADDRESS_MODE destination_address_mode;
  T_DMA_PACKING     destination_packet;
  T_DMA_BURST       destination_burst;


} T_DMA_CHANNEL_PARAMETERS;


/**
 * function: dma_reserve_channel
 *
 * This function allows the reservation of a free DMA channel or of a specific 
 * channel. If the request is honoured, a channel number shall be returned as a 
 * result. This channel number is required for other function calls like 
 * programming DMA transfer information and to enable the transfer. 
 *
 * A limited number of channels are available. If the request can not be 
 * granted at the time, it can be queued as an option. When a channel comes 
 * available later, waiting reservation requests are handled with respect 
 * to the given priority. 
 *
 * The function returns immediately and handles the request asynchronously. 
 * The message is then validated and handled. The return path is used to 
 * inform the client about the result of the message processing and to 
 * inform the client of any asynchronous events.
 * An example of an asynchronous event is a status message informing 
 * the client that the reservation request is granted at a later time.
 *
 * @param specific
 * specific indicates whether the client request a specific channel 
 * or the first available. 
 *
 * @param channel
 * The channel number if the client request an specific channel (see 
 * parameter specific). If the request is not for a specific channel, 
 * this parameter is ignored.
 * Channel range is from DMA_MIN_CHANNEL  to DMA_MAX_CHANNEL.
 *
 * @param queue
 * queue indicates if the request is to be queued when it can 
 * not be granted immediately.
 *
 * @sw_priority
 * The sw_priority number is used when a reservation is handled from 
 * the queue. The reservation with the highest sw_priority is handled 
 * first. When several reservations are made with the same sw_priority, 
 * the reservations are handled on a first-in/first-out basis. 
 * If the request is not queued, this parameter is ignored.
 * sw_priority range is from DMA_SW_PRIORITY_MIN  to DMA_SW_PRIORITY_MAX.
 * 
 * @param return_path
 * Return path for notifications 
 *
 * @return
 * RV_OK     The API function was successfully executed. Expect status message.
 * RV_MEMORY_ERR  Insufficient memory to create the context.
 * RV_NOT_READY   The driver is not able to handle this request at this moment.
 *                (SWE initialization is not done correctly).
 *
 * Message returned: DMA_STATUS_RSP_MSG
 * This message is send to the client to return the status and result of the 
 * requested action. Other API's also uses this message to return status or 
 * to notify clients of asynchronous events.   
 *
 * structure member result.status 
 * For this action, the value of the message structure member result.status 
 * can have the following values:
 *
 * DMA_RESERVE_OK   Request is granted. The structure member result.channel 
 *                  holds the granted channel number.
 *
 * DMA_QUEUED       Request could not be granted now and is queued. The 
 *                  structure member result..channel holds a channel queue 
 *                  identifier that can be used to remove the reservation 
 *                  from the queue (with the function dma_remove_from_queue()).
 * 
 * DMA_NO_CHANNEL   Request denied because there is no free channel available.
 * 
 * DMA_TOO_MANY_REQUESTS  Request could not be granted and the queue is full.
 * 
 * DMA_INVALID_PARAMETER  One ore more of the parameters is incorrect.
 * 
 * DMA_NOT_READY    Requested process is supported but cannot be processed 
 *                  now. (SWE initialization is not done correct).
 *
 * DMA_MEMORY_ERR   The available memory within the DMA SWE is insufficient 
 *                  to process the command. 
 */
T_RV_RET dma_reserve_channel (T_DMA_SPECIFIC specific,
                              T_DMA_CHANNEL  channel,
                              T_DMA_QUEUE    queue,
                              T_DMA_SW_PRIORITY sw_priority,                              
                              T_RV_RETURN    return_path);


/**
 * function: dma_remove_from_queue
 *
 * This function allows the removal of the queued channel reservation request. 
 *
 * The function returns immediately and handles the request asynchronously. 
 * The message is then validated and handled. The return path is used to inform
 * the client about the result of the message processing and to inform the 
 * client of any asynchronous events.
 *
 * If the client is not queued, the message status DMA_ACTION_NOT_ALLOWED 
 * is returned. If it was queued, the message status DMA_OK is returned.
 *
 *
 * Parameters
 *
 * @param channel_queue_id
 * This parameter is the returned queue identifier that has been returned by 
 * the driver at the moment of reservation (see dma_reserve_channel()).
 *
 * @Return
 *
 * The possible values are:
 * RV_OK         The API function was successfully executed. Expect status 
 *               message.
 * RV_MEMORY_ERR Insufficient memory to create the context.
 * RV_NOT_READY  The driver is not able to handle this request at this 
 *               moment. (SWE initialization is not done correctly).
 *
 *
 * Event Return
 *
 * DMA_STATUS_RSP_MSG
 * This message is send to the client to return the status and result of the 
 * requested action. Other API's also uses this message to return status or 
 * to notify clients of asynchronous events.   
 * For this action, the value of the message structure member result.status 
 * can have the following values:
 *
 * DMA_OK The provided information is validated and accepted. The channel 
 *        reservation request is removed from the queue.
 *
 * DMA_INVALID_PARAMETER  The parameter is incorrect.
 *
 * DMA_NOT_READY  The driver is not able to handle this request at this 
 *                moment. (SWE initialization is not done correctly or the 
 *                channel is already dequeued).
 *
 * DMA_ACTION_NOT_ALLOWED There is no queued channel reservation request of 
 *                        this client available.
 *
 * DMA_MEMORY_ERR The available memory within the DMA SWE is insufficient 
 *                to process the command. 
 *
 * @Note that this API functionality can also be invoked with the message 
 * DMA_REMOVE_FROM_QUEUE_REQ_MSG.
 *
 */
T_RV_RET dma_remove_from_queue (T_DMA_CHANNEL channel_queue_id);


/*
 * @function: dma_set_channel_parameters 
 *
 * This function allows the programming of the specific channel parameters.
 * All operational settings required for executing a DMA transfer, has to be 
 * provided within the channel_info structure. Examples of parameters are:
 * - The client can indicate if he wants to be notified when the DMA transfer 
 *   is completed or not.
 * - The client can indicate if he wants to enable the transfer immediately 
 *   or later.
 * - Software or hardware start-source (which hardware source.
 * - length, endian, priority, data width, DMA mode etc. 
 *
 * The function must be called after the channel reservation is granted and 
 * before the DMA transfer is enabled. The function returns immediately and 
 * handles the request asynchronously. The message is then validated and 
 * handled. The return path is used to inform the client about the result 
 * of the message processing and to inform the client of any asynchronous 
 * events.
 * 
 * Parameters
 * 
 * @param channel
 * channel contains the number of the reserved channel. This number is 
 *         returned at the moment the reservation is granted.
 *  
 * @param channel_info_p
 * channel_info_p must point to a structure containing all the information 
 *                required to prepare the DMA transfer. 
 * 
 * @return
 * 
 * T_RV_RET
 * 
 * The possible values are:
 * RV_OK The API function was successfully executed. Expect status message.
 * RV_MEMORY_ERR Insufficient memory to create the context.
 * RV_NOT_READY  The driver is not able to handle this request at this moment. 
 *               (SWE initialization is not done correctly).
 * 
 * 
 * Event Return
 * 
 * DMA_STATUS_RSP_MSG
 * This message is send to the client to return the status and result of the 
 * requested action. Other API's also uses this message to return status or to 
 * notify clients of asynchronous events.   
 * 
 * structure member result.status 
 * For this action, the value of the message structure member result.status can
 * have the following values:
 *
 * DMA_OK  The provided information is validated and accepted. Hardware 
 *         settings are updated accordingly.
 *
 * DMA_INVALID_PARAMETER One ore more of the parameters is incorrect.
 *
 * DMA_NOT_READY The driver is not able to handle this request at this moment.
 *               (SWE initialization is not done correctly).
 *
 * DMA_ACTION_NOT_ALLOWED There is an incorrect sequence of API invocations 
 *                        (command ignored).
 *
 * DMA_MEMORY_ERR  The available memory within the DMA SWE is insufficient to 
 *                 process the command. 
 * 
 * structure member result.channel 
 * For this action, the value of the message structure member result.channel 
 * is to be ignored.
 * 
 * Note that this API functionality can also be invoked with the message 
 * DMA_SET_CHANNEL_PARAMETERS_REQ_MSG.
 *
**/
T_RV_RET dma_set_channel_parameters (T_DMA_CHANNEL channel,
                                      T_DMA_CHANNEL_PARAMETERS *channel_info_p);

/*
 * @function: dma_enable_transfer
 * Description
 * 
 * This function allows the start of the prepared DMA transfer. 
 * If the DMA parameters have been set-up to use the hardware synchronisation,
 * this is armed. The DMA device can initiate the transfer at any time.
 * If the DMA parameters have been set-up to use software synchronisation, 
 * the DMA transfer is started immediately.
 * The function can be called again, after a transfer is completed (if the 
 * channel is not released). Called more then once before the transfer is 
 * completed shall result in the error DMA_CHANNEL_ENABLED.
 * @note 
 * enabling a channel is also possible with the dma_set_channel_parameters()
 * function.
 * 
 * The function must be called after the channel reservation is granted and 
 * channel parameters has been set. The function returns immediately and 
 * handles the request asynchronously. The message is then validated and 
 * handled. The return path is used to inform the client about the result 
 * of the message processing and to inform the client of any asynchronous 
 * events.
 * 
 * Parameters
 * 
 * @param channel
 * channel contains the number of the reserved channel. This number has been 
 * returned at the moment the reservation was granted.
 * 
 * @return
 * 
 * T_RV_RET
 * 
 * The possible values are:
 * id  Definition
 * RV_OK The API function was successfully executed. Expect status message.
 * RV_MEMORY_ERR Insufficient memory to create the context.
 * RV_NOT_READY  The driver is not able to handle this request at this moment. 
 * (SWE initialization is not done correctly).
 * 
 * 
 * Event Return
 * 
 * DMA_STATUS_RSP_MSG
 * This message is send to the client to return the status and result of the 
 * requested action. Other API's also uses this message to return status or 
 * to notify clients of asynchronous events.   
 * 
 * structure member result.status 
 * For this action, the value of the message structure member result.status 
 * can have the following values:
 * DMA_OK  The provided information is validated and accepted. The DMA is 
 *         started or armed.
 *
 * DMA_INVALID_PARAMETER The parameter is incorrect.
 *
 * DMA_NOT_READY The driver is not able to handle this request at this moment.
 *               (SWE initialization is not done correctly).
 *
 * DMA_ACTION_NOT_ALLOWED There is an incorrect sequence of API invocations 
 *                        (command ignored).
 *
 * DMA_MEMORY_ERR  The available memory within the DMA SWE is insufficient 
 *                 to process the command. 
 *
 * DMA_CHANNEL_ENABLED The channel is already enabled.
 * 
 * structure member result.channel 
 * For this action, the value of the message structure member result.channel 
 * is to be ignored.
 * 
 * 
 * */
T_RV_RET dma_enable_transfer (T_DMA_CHANNEL channel);
 

/* @function dma_release_channel
 * 
 * Description
 * This function allows the release of the reserved DMA channel. Depending on 
 * the state of transfer, the release may be immediately or postponed till 
 * the busy transfer is completed.
 * If the release must be postponed, the client shall be notified asynchronous
 * as soon as the release is possible (status message DMA_CHANNEL_RELEASED, 
 * using the return path provided at reserving time). 
 * 
 * Releasing a channel enables the DMA SWE to serve any queued reservation 
 * request. The channel shall be assigned to a queued client immediately.
 * Receiving an asynchronous error event does not free the client of the 
 * obligation to release the channel.
 * 
 * The function returns immediately and handles the request asynchronously.
 * The message is then validated and handled. The return path is used to 
 * inform the client about the result of the message processing and to 
 * inform the client of any asynchronous events.
 * 
 * Parameters
 * 
 * @param channel
 * channel contains the number of the channel to be released. This 
 * identification has been returned at the moment the reservation was granted.
 * 
 * @return
 * 
 * T_RV_RET
 * 
 * The possible values are:
 * RV_OK The API function was successfully executed. Expect status message.
 * RV_MEMORY_ERR Insufficient memory to create the context.
 * RV_NOT_READY  The driver is not able to handle this request at this moment.
 *               (SWE initialization is not done correctly).
 * 
 * 
 * Event Return
 * 
 * DMA_STATUS_RSP_MSG
 * This message is send to the client to return the status and result of the 
 * requested action. Other API's also uses this message to return status or 
 * to notify clients of asynchronous events.   
 * 
 * structure member result.status 
 * For this action, the value of the message structure member result.status 
 * can have the following values:
 *
 * DMA_OK The provided information is validated and accepted. The channel 
 *        is released.
 *
 * DMA_INVALID_PARAMETER The parameter is incorrect.
 *
 * DMA_NOT_READY The driver is not able to handle this request at this moment.
 *               (SWE initialization is not done correctly).
 *
 * DMA_ACTION_NOT_ALLOWED There is an incorrect sequence of API invocations 
 *                        (command ignored).
 *
 * DMA_MEMORY_ERR  The available memory within the DMA SWE is insufficient to 
 *                 process the command. 
 *
 * DMA_CHANNEL_BUSY  The channel could not be released now. An asynchronous 
 *                   message is send as soon the channel could be released 
 *                   (status DMA_CHANNEL_RELEASED).
 * 
 * structure member result.channel 
 * For this action, the value of the message structure member result.channel 
 * is to be ignored.
 * 
 * 
**/
T_RV_RET dma_release_channel (T_DMA_CHANNEL channel);


/* @function dma_get_sw_version
 *
 * Description
 * This function returns the driver version.
 * 
 * @param
 * 
 * None.
 * 
 * 
 * @return
 * 
 * UINT32
 * 
 * Bit Name  Function
 * [0-15]  BUILD Build number
 * [16-23] MINOR Minor version number
 * [24-31] MAJOR Major version number
 * 
 * 
 * 
 **/
UINT32 dma_get_sw_version(void);


/*@}*/


#ifdef __cplusplus
}
#endif

#endif /*__DMA_API_H_*/

