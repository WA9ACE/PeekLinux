/**
 * @file  dmg_api.h
 *
 * API Definition for DMG SWE.
 *
 * @author  J.A. Renia ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  1/22/2004 J.A. Renia ()   Create.
 *
 * (C) Copyright 2004 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __DMG_API_H_
#define __DMG_API_H_


#include "dma/dma_api.h"
#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @name DMG Return type and return values.
 *
 * Currently they are the standard RV return types, but they may
 * be customized in the future.
 */
/*@{*/
typedef T_RV_RET T_DMG_RETURN;

#define DMG_OK              RV_OK       
#define DMG_NOT_SUPPORTED   RV_NOT_SUPPORTED    
#define DMG_INTERNAL_ERR    RV_INTERNAL_ERR   
/*@}*/

/* Defines illegal info */
#define DMG_ILLEGAL 0xFF
/**
  Defines the channel number.
  Channel range is from DMG_MIN_CHANNEL to DMG_MAX_CHANNEL or DMG_ANY_CHANNEL.
  */
typedef UINT8  T_DMG_CHANNEL;


/**
  Defines the minimum channel number that can be requested at a specific reservation.
  */
#define DMG_MIN_CHANNEL   DMA_MIN_CHANNEL

/**
  Defines the maximum channel number that can be requested at a specific reservation.
  */
#define DMG_MAX_CHANNEL   DMA_MAX_CHANNEL

/**
  This define is to be used when the client wants to reserve any of 
  the available DMA channels.
  */
#define DMG_ANY_CHANNEL   (0xFF)

/**
  Defines is a channel is locked or not.
  */
typedef UINT8 T_DMG_LOCK;
#define DMG_CHANNEL_NOT_LOCK (0)
#define DMG_CHANNEL_LOCKED   (1)

/**
  Defines the information required to reserve a channel.

  The channel number has to be in the range from DMG_MIN_CHANNEL to DMG_MAX_CHANNEL 
  if the client requested an specific channel. 
  If the request is not for a specific channel, but for any of the available 
  channels the user has to provide DMG_ANY_CHANNEL. 

  lock can be set by the client to keep a channel locked after a transfer has been completed.
  */
typedef struct  { T_DMG_CHANNEL channel;
                  T_DMG_LOCK    lock;
                } T_DMG_CHANNEL_DATA;

/**
  T_DMG_PERIPHERAL defines the source or destination for starting a DMA transfer. 
  Depending on the dmg_peripheral the priority of a queued reservation might be changed. 
  */
#ifdef _WINDOWS
typedef T_DMA_SYNC_DEVICE  T_DMG_PERIPHERAL;
typedef T_DMA_SYNC_DEVICE  T_DMG_SYNC_DEVICE;
#else
typedef T_DMA_TYPE_CHANNEL_HW_SYNCH  T_DMG_PERIPHERAL;
typedef T_DMA_TYPE_CHANNEL_HW_SYNCH  T_DMG_SYNC_DEVICE;
#endif
#define DMG_PERIPHERAL_NONE         DMA_SYNC_DEVICE_NONE        
#define DMG_PERIPHERAL_HW_RIF_TX    DMA_SYNC_DEVICE_HW_RIF_TX   
#define DMG_PERIPHERAL_HW_RIF_RX    DMA_SYNC_DEVICE_HW_RIF_RX   
#define DMG_PERIPHERAL_HW_LCD       DMA_SYNC_DEVICE_HW_LCD      
#define DMG_PERIPHERAL_UART1_TX     DMA_SYNC_DEVICE_UART1_TX    
#define DMG_PERIPHERAL_UART1_RX     DMA_SYNC_DEVICE_UART1_RX    
#define DMG_PERIPHERAL_UART2_TX     DMA_SYNC_DEVICE_UART2_TX    
#define DMG_PERIPHERAL_UART2_RX     DMA_SYNC_DEVICE_UART2_RX    
#define DMG_PERIPHERAL_UART_IRDA_TX DMA_SYNC_DEVICE_UART_IRDA_TX
#define DMG_PERIPHERAL_UART_IRDA_RX DMA_SYNC_DEVICE_UART_IRDA_RX
#define DMG_PERIPHERAL_USB_RX1      DMA_SYNC_DEVICE_USB_RX1     
#define DMG_PERIPHERAL_USB_TX1      DMA_SYNC_DEVICE_USB_TX1     
#define DMG_PERIPHERAL_USB_RX2      DMA_SYNC_DEVICE_USB_RX2     
#define DMG_PERIPHERAL_USB_TX2      DMA_SYNC_DEVICE_USB_TX2     
#define DMG_PERIPHERAL_USB_RX3      DMA_SYNC_DEVICE_USB_RX3     
#define DMG_PERIPHERAL_USB_TX3      DMA_SYNC_DEVICE_USB_TX3     
#define DMG_PERIPHERAL_MC_SD_RX     DMA_SYNC_DEVICE_MC_SD_RX   
#define DMG_PERIPHERAL_MC_SD_TX     DMA_SYNC_DEVICE_MC_SD_TX   
#define DMG_PERIPHERAL_MS_RX_TX     DMA_SYNC_DEVICE_MS_RX_TX    
#define DMG_PERIPHERAL_USIM_RX      DMA_SYNC_DEVICE_USIM_RX     
#define DMG_PERIPHERAL_USIM_TX      DMA_SYNC_DEVICE_USIM_TX     
#define DMG_PERIPHERAL_UWIRE_RX_TX  DMA_SYNC_DEVICE_UWIRE_RX_TX 
#define DMG_PERIPHERAL_NAND_FLASH   DMA_SYNC_DEVICE_NAND_FLASH  
#define DMG_PERIPHERAL_I2C_RX       DMA_SYNC_DEVICE_I2C_RX      
#define DMG_PERIPHERAL_I2C_TX       DMA_SYNC_DEVICE_I2C_TX      
#define DMG_PERIPHERAL_SHA1_TX      DMA_SYNC_DEVICE_SHA1_TX     
#define DMG_PERIPHERAL_DES_RX       DMA_SYNC_DEVICE_DES_RX      
#define DMG_PERIPHERAL_DES_TX       DMA_SYNC_DEVICE_DES_TX      
#define DMG_PERIPHERAL_CPORT_RX     DMA_SYNC_DEVICE_CPORT_RX    
#define DMG_PERIPHERAL_CPORT_TX     DMA_SYNC_DEVICE_CPORT_TX    

#define DMG_SYNC_DEVICE_MAX         DMA_SYNC_DEVICE_MAX         

/* Do not use these define, as they a specially used for the DMG tests */
#define DMG_TEST_PERIPHERAL_1       250
#define DMG_TEST_PERIPHERAL_2       DMG_TEST_PERIPHERAL_1+1 
#define DMG_TEST_PERIPHERAL_3       DMG_TEST_PERIPHERAL_1+2
#define DMG_TEST_PERIPHERAL_4       DMG_TEST_PERIPHERAL_1+3
#define DMG_TEST_PERIPHERAL_5       DMG_TEST_PERIPHERAL_1+4

/**
  T_DMG_QUEUE indicates if the request is to be queued when it can not be granted immediately.
  */
typedef UINT8 T_DMG_QUEUE;
#define DMG_QUEUE_DISABLE   DMA_QUEUE_DISABLE
#define DMG_QUEUE_ENABLE    DMA_QUEUE_ENABLE

/**
  Defines the information required to queue the reservation of a channel in 
  case at the time of reservation no channel is available. 
  */
typedef struct  { T_DMG_PERIPHERAL dmg_peripheral;
                  T_DMG_QUEUE      dmg_queued;
                } T_DMG_REQUEST_DATA;


typedef UINT16  T_DMG_RET;
/**
  Type definition T_DMG_RET holds the reply status of a channel reservation request.
  */
#define DMG_MEMORY_ERROR    DMA_MEMORY_ERROR

/* The channel reservation request is granted, The status 
 * message structure member result.channel holds the granted 
 * channel number. 
 */
#define DMG_RESERVE_OK DMA_RESERVE_OK

/*
 * One ore more of the parameters are illegal.
 */
#define DMG_INVALID_PARAMETER DMA_INVALID_PARAMETER

/* The channel could not be released now. An asynchronous 
 * message is send as soon the channel could be released.
 */
#define DMG_CHANNEL_BUSY  DMA_CHANNEL_BUSY 

/* Requested process is supported but cannot be processed 
 * now (the driver initialization is not done correct).
 */ 
#define DMG_NOT_READY     DMA_NOT_READY 

/* Request could not be granted now and is queued. The 
 * status message structure member result.channel holds 
 * a channel queue identi-fier. This identifier can be 
 * used to remove the reservation request from the queue. 
 */
#define DMG_QUEUED      DMA_QUEUED

/* Request denied because there is no free channel available. */
#define DMG_NO_CHANNEL  DMA_NO_CHANNEL   

/* Request could not be queued because queue is full. */
#define DMG_TOO_MANY_REQUESTS     DMA_TOO_MANY_REQUESTS 

/* The action is not allowed now (sequence of API calls incorrect). */
#define DMG_ACTION_NOT_ALLOWED    DMA_ACTION_NOT_ALLOWED 

/* The DMA source device has created did not provide data in time. */
#define DMG_TIMEOUT_SOURCE        DMA_TIMEOUT_SOURCE

/* The DMA destination device has created did not 
   provide data in time. */
#define DMG_TIMEOUT_DESTINATION   DMA_TIMEOUT_DESTINATION  

/* The DMA transfer was still busy while a new transfer 
   start event occurred. */
#define DMG_MISS_EVENT            DMA_MISS_EVENT

/* The DMA transfer is completed. The client may release 
   the channel now or prepare a new one. */
#define DMG_COMPLETED             DMA_COMPLETED

/* The reserved channel is now released. */
#define DMG_CHANNEL_RELEASED      DMA_CHANNEL_RELEASED 

/* Client tried to enable a channel but the channel is
    already enabled.    */
#define DMG_CHANNEL_ENABLED       DMA_CHANNEL_ENABLED

/* Request has been processed from the queue */
#define DMG_QUEUE_PROC            DMA_QUEUE_PROC     

/* A reservation has been removed from the queue */
#define DMG_REMOVED_FROM_QUEUE    DMA_REMOVED_FROM_QUEUE

/* Parameters for the channel provided have been set */
#define DMG_PARAM_SET             DMA_PARAM_SET

/* Defines for starting a transfer automatically or not */
#define DMG_MODE_TRANSFER_DISABLE DMA_MODE_TRANSFER_DISABLE
#define DMG_MODE_TRANSFER_ENABLE  DMA_MODE_TRANSFER_ENABLE 

#define DMG_BUF1_READY          DMA_BUF1_READY
#define DMG_BUF2_READY          DMA_BUF2_READY

/* Requested channel is already locked but has been queued. */
#define DMG_ALREADY_LOCKED_QUEUDED (DMG_BUF2_READY+1) 

/** Specifies status and return information. */ 
typedef struct 
{
  T_DMG_RET     status;
  T_DMG_CHANNEL channel;
} T_DMG_RESULT;


/**
  T_DMG_DATA_WIDTH defines the data width for the channel transfer. 

  The chosen definition here must match the alignment of the start address 
  in source and destination device. This means that for 32 bit data width 
  the address in source and destination device must be 32bits aligned.  
  For 8 bit data width, no alignment requirement exists.

  Data width is part of the definition of the number of bytes to transfer 
  (like frames and elements, see for more information T_DMG_FRAMES).
  */
typedef UINT8 T_DMG_DATA_WIDTH;

#define DMG_DATA_S8   DMA_DATA_S8  /*  8 bits */
#define DMG_DATA_S16  DMA_DATA_S16 /* 16 bits */
#define DMG_DATA_S32  DMA_DATA_S32 /* 32 bits */

/**
  T_DMG_PACKING defines if data must be packed or not. The packing of a source 
  or destination device can be set separate. Packing can be set if the data size 
  of the port is less then the DMA data width of the port. Packing increases the 
  transfer rate.
  */
typedef UINT8 T_DMG_PACKING;
#define DMG_NOT_PACKED  DMA_NOT_PACKED
#define DMG_PACKED      DMA_PACKED

/**
  T_DMG_BURST defines if burst access should be used. The burst mode of a source 
  or destination device can be set separate. Bust allows consecutive bytes to be 
  to be packed is a way that it will result in a single transfer of 4 words of 
  32 bits. Burst is only available for IMIF access. 
  */
typedef UINT8 T_DMG_BURST;
#define DMG_NO_BURST      DMA_NO_BURST 
#define DMG_BURST_ENABLED DMA_BURST_ENABLED 

/**
  T_DMG_HW_PRIORITY defines the hardware priority of the channel when it has to 
  compete with other internal channels. High priority channels are served before 
  low priority channels. Competing channels of the same prior-ity are served 
  on a round robin manner.
  */
typedef UINT8 T_DMG_HW_PRIORITY;
#define DMG_HW_PRIORITY_LOW   DMA_HW_PRIORITY_LOW 
#define DMG_HW_PRIORITY_HIGH  DMA_HW_PRIORITY_HIGH


/**
  T_DMG_REPEAT definition controls whether the DMA process is a single transfer 
  operation or a continuous one. In a continuous DMA process the DMA controller 
  re-initializes after a completed transfer. 

  Note that in a continiuous mode the client has to release the channel it self. 
  */
typedef UINT8 T_DMG_REPEAT;
#define DMG_SINGLE      DMA_SINGLE
#define DMG_CONTINIOUS  DMA_CONTINIOUS
/**
  T_DMG_FLUSH is only valid when a source or destination device has its burst 
  mode enabled (see T_DMG_BURST). In the last burst cycle the source may fail 
  to supply the remaining bytes to complete the burst. By ena-bling the FIFO 
  flush, an incomplete last burst cycle is forced to complete.
  */
typedef UINT8 T_DMG_FLUSH;
#define DMG_FLUSH_DISABLED  DMA_FLUSH_DISABLED
#define DMG_FLUSH_ENABLED   DMA_FLUSH_ENABLED

/**
  T_DMG_ADDRESS_MODE 
  This controls the addressing mode towards the DMA device. The addressing mode 
  of both, the source and destination device can be set individually.

  About addressing modes:
  DMG_ADDR_MODE_CONSTANT :read/write always from the same (start)address.
  DMG_ADDR_MODE_POST_INC  : read/write sequential. (next address always one higher)
  DMG_ADDR_MODE_FRAME_INDEX : read/write sequential only within a frame. 
  New frames start again from the start address. (See T_DMG_FRAME about what a frame is).
  */
typedef UINT8 T_DMG_ADDRESS_MODE;
#define DMG_ADDR_MODE_CONSTANT    DMA_ADDR_MODE_CONSTANT
#define DMG_ADDR_MODE_POST_INC    DMA_ADDR_MODE_POST_INC
#define DMG_ADDR_MODE_FRAME_INDEX DMA_ADDR_MODE_FRAME_INDEX

/**
  T_DMG_FRAMES
  Frames are part of the definition of the number of bytes to transfer (like 
  elements and data width).

  TotalNumberOfBytes = NumberOfFrames x NumberOfElementsperFrame x NumberOfBytesPerElement
  i.e.
  TotalNumberOfBytes = T_DMG_FRAMES x T_DMG_ELEMENTS x T_DMG_DATA_WIDTH

  By carefully selecting the number of frames, elements per frame and bytes per element 
  the client has the possibility of using the DMA controller in the best possible way.
  */
typedef UINT16 T_DMG_FRAMES;

/**
  T_DMG_ELEMENTS
  Elements are part of the definition of the number of bytes to transfer 
  (like frames and data width, see for more information T_DMG_FRAMES).
  */
typedef UINT16 T_DMG_ELEMENTS;


/** 
  T_DMG_END_NOTIFICATION

  Defines if the client is notified through the return path about 
  the completion of the DMA transfer. 
  */
typedef UINT8   T_DMG_END_NOTIFICATION;
#define DMG_NO_NOTIFICATION DMA_NO_NOTIFICATION
#define DMG_NOTIFICATION    DMA_NOTIFICATION


/** 
  T_DMG_SECURE
  DMA channels can be restricted for secured DMA transfer. Access to those 
  secured channels is re-stricted to the ARM running an secure applications. 
  Ones a channel is secured, writing of channel configuration data is having 
  no effect and reading from the DMA controller always reads zero.
  */
typedef UINT8 T_DMG_SECURE;
#define DMG_NOT_SECURED   DMA_NOT_SECURED 
#define DMG_SECURED       DMA_SECURED 


/**
  T_DMG_CHANNEL_PARAMETERS contains all the information to setup a DMA transfer. 

  Note that for double buffering the same address mode, packing and burst mode 
  are used as set for the base address. 

  It is allowed that the address of the source or destination point to external
  memory. Double buffer addresses have to point to the same type of memory as 
  the base address. 
  */
typedef struct 
{
  T_DMG_DATA_WIDTH    data_width;
  T_DMG_SYNC_DEVICE   sync;
  T_DMG_HW_PRIORITY   hw_priority;
  T_DMG_REPEAT        repeat;
  T_DMG_FLUSH         flush;
  T_DMG_FRAMES        nmb_frames;
  T_DMG_ELEMENTS      nmb_elements;
  T_DMG_END_NOTIFICATION  dmg_end_notification;
  T_DMG_SECURE        secure;

  UINT32              source_address;
  T_DMG_ADDRESS_MODE  source_address_mode;
  T_DMG_PACKING       source_packet;
  T_DMG_BURST         source_burst;

  UINT32              destination_address;
  T_DMG_ADDRESS_MODE  destination_address_mode;
  T_DMG_PACKING       destination_packet;
  T_DMG_BURST         destination_burst;

  UINT32      double_buf_source_address;
  UINT32      double_buf_destination_address;
} T_DMG_CHANNEL_PARAMETERS;


/**
  Description
  This function allows the reservation of a free DMA channel or of a specific 
  channel. If the request is honoured, a channel number shall be returned as a 
  result. This channel number is required for other function calls like 
  programming DMA transfer information and to enable the transfer. 

  By default the DMG will release a channel after a DMA transfer but certain 
  clients might need a (spe-cific) channel which remains locked for a certain 
  amount of time. Therefore it is also possible to lock a channel which means 
  that it will not released automatically after a DMA transfer. 

  The T_DMG_REQUEST_DATA structure does contain peripheral device and a queue 
  option. The peripheral device is used for two things:
  1. To indicate the sync device for starting the DMA transfer
  2. To determine the priority of how a queued option is served. 

  The queue option is used because a limited number of channels are available. 
  If the request can not be granted at the time, it can be queued as an option. 
  When a channel comes available later, waiting reservation requests are handled
  with respect to the given peripheral ID in the T_DMG_QUEUE_INFO structure. 

  The function returns immediately and handles the request asynchronously. The 
  message is then vali-dated and handled. The return path is used to inform the
  client about the result of the message proc-essing and to inform the client 
  of any asynchronous events.
  An example of an asynchronous event is a status message informing the client
  that the reservation request is granted at a later time.

  Parameters

  @param dmg_channel_data_p
  dmg_channel_data_p must point to a structure containing the information if 
  any or a specific channel has to be reserved. If a specific channel is 
  required the channel number has to be provided. See the description of 
  T_DMG_CHANNEL_DATA

  @param dmg_request_data_p
  dmg_request_data_p must point to a structure containing the information 
  if a reservation has to be queued or not in the case that no DMA channels 
  are available. If a queue option is requested the pe-ripheral for the queue 
  has to provided as well. See the description of T_DMG_REQUEST_DATA 

  @return return_path
  Return path for notifications 

  Immediate Return

  - T_RV_RET

  The possible values are:
  id  Definition
  RV_OK The API function was successfully executed. Expect status message.
  RV_MEMORY_ERR Insufficient memory to create the buffer.
  RV_NOT_READY  The driver is not able to handle this request at this moment.
                (SWE initiali-zation is not done correctly).



  Event Return

  - DMG_STATUS_RSP_MSG
  This message is send to the client to return the status and result of the 
  requested action. Other API's also uses this message to return status or 
  to notify clients of asynchronous events.   

  structure member result.status 
  For this action, the value of the message structure member result.status can 
  have the following val-ues:
  id  Definition
  DMG_RESERVE_OK  Request is granted. The structure member result.channel holds
  the granted channel number.

  DMG_QUEUED  Request could not be granted now and is queued. The structure 
  member result.channel holds a channel queue identifier that can be used to 
  remove the reservation from the queue (with the function dmg_remove_from_queue()).

  DMG_NO_CHANNEL  Request denied because there is no free channel available.

  DMG_TOO_MANY_REQUESTS Request could not be granted and the queue is full.

  DMG_INVALID_PARAMETER One ore more of the parameters is incorrect.

  DMG_NOT_READY Requested process is supported but cannot be processed now. 
  (SWE initialization is not done correct ).

  DMG_MEMORY_ERR  The available memory within the DMA SWE is insufficient to
  process the command. 

  DMG_ALREADY_LOCKED_QUEUDED  Requested channel is already locked but has 
  been queued. 

  structure member result.channel 
  For this action, the value of the message structure member result.channel 
  holds only a valid value if the member result.status has the value 
  DMG_RESERVE_OK or DMG_QUEUED. For DMG_RESERVE_OK the member holds the 
  assigned channel number. For DMG_QUEUED or DMG_ALREADY_LOCKED_QUEUED the 
  member holds a channel queue identifier that can be used by the client if 
  he wants to remove the request from the queue (using the function 
  dmg_remove_from_queue()).

  Note that this API functionality can also be invoked with the message 
  DMG_RESERVE_CHANNEL_REQ_MSG.


  Current restriction of use
  None
*/

T_RV_RET dmg_reserve_channel( const T_DMG_CHANNEL_DATA *dmg_channel_data_p,
                              const T_DMG_REQUEST_DATA *dmg_request_data_p,
                              T_RV_RETURN        return_path);


/**
  Description

  This function allows the programming of the specific channel parameters.
  All operational settings required for executing a DMA transfer, have to 
  be provided within the chan-nel_info structure. Examples of parameters are:
  - The client can indicate if he wants to be notified when the DMA transfer 
    is completed or not.
  - Software or hardware start-source (which hardware source).
  - length, endian, hardware priority, data width, DMA mode etc. 
  - the internal or external RAM address from Riviera
  - optional double buffering addresses

  The function must be called after memory has been reserved (through the 
  riviera memory reservation function) and the channel reservation is granted.
  The DMA transfer is started automatically after call-ing this function. 
  The function returns immediately and handles the request asynchronously. 
  The message is then validated and handled. The return path is used to inform 
  the client about the result of the message processing and to inform the 
  client of any asynchronous events.

  When a client wants to use double buffering, the second address has to be 
  provided as well in the structure. When the value of the double buffering 
  address is NULL, double buffering is disabled.

  Parameters

  @param dmg_channel
  dmg_channel contains the number of the reserved channel. This number is 
  returned at the moment the reservation is granted.

  @param dmg_channel_info_p
  dmg_channel_info_p must point to a structure containing all the information 
  required to prepare the DMA transfer. See the description of 
  T_DMG_CHANNEL_PARAMETERS

  @return return_path
  Return path for notifications 

  Immediate Return

  - T_RV_RET

  The possible values are:
  id  Definition
  RV_OK The API function was successfully executed. Expect status message.
  RV_MEMORY_ERR Insufficient memory to execute the task requested.
  RV_NOT_READY  The driver is not able to handle this request at this moment. 
  (SWE initiali-zation is not done correctly).


  Event Return

  - DMG_STATUS_RSP_MSG
  This message is send to the client to return the status and result of the 
  requested action. Other API's also uses this message to return status or 
  to notify clients of asynchronous events.   
  
  structure member result.status 
  For this action, the value of the message structure member result.status can 
  have the following val-ues:
  id  Definition
  DMG_OK  The provided information is validated and accepted. Hardware settings 
  are updated accordingly.

  DMG_INVALID_PARAMETER One ore more of the parameters is incorrect.

  DMG_NOT_READY The driver is not able to handle this request at this moment. 
  (SWE initialization is not done correctly).

  DMG_ACTION_NOT_ALLOWED  There is an incorrect sequence of API invocations 
  (command ignored).

  DMG_MEMORY_ERR  The available memory within the DMG SWE is insufficient to
  process the command. 

  structure member result.channel 
  For this action, the value of the message structure member result.channel  
  is to be ignored.

  Note that this API functionality can also be invoked with the message 
  DMG_SET_CHANNEL_PARAMETERS_REQ_MSG.

  Current restriction of use
  None

*/
T_RV_RET dmg_set_channel_parameters (T_DMG_CHANNEL            dmg_channel,
                           const T_DMG_CHANNEL_PARAMETERS *dmg_channel_info_p,
                                     T_RV_RETURN      return_path );



/**
  Description
  This function allows the removal of the queued channel reservation request. 

  The function returns immediately and handles the request asynchronously. The 
  message is then vali-dated and handled. The return path is used to inform the 
  client about the result of the message proc-essing and to inform the client 
  of any asynchronous events.

  If the client is not queued, the message status DMG_ACTION_NOT_ALLOWED is 
  returned. If it was queued, the message status DMG_OK is returned.

  Parameters

  @param channel_queue_id
  This parameter is the returned queue identifier that has been returned by the 
  driver at the moment of reservation (see dmg_reserve_channel()).

  @return return_path
  Return path for notifications. 

  Immediate Return

  - T_RV_RET

  The possible values are:
  id  Definition
  RV_OK The API function was successfully executed. Expect status message.
  RV_MEMORY_ERR Insufficient memory to execute the task requested.
  RV_NOT_READY  The driver is not able to handle this request at this moment. 
  (SWE initiali-zation is not done correctly).


  Event Return

  - DMG_STATUS_RSP_MSG
  This message is send to the client to return the status and result of the 
  requested action. Other API's also uses this message to return status or to 
  notify clients of asynchronous events.   

  structure member result.status 
  For this action, the value of the message structure member result.status can 
  have the following val-ues:
  id  Definition
  DMG_OK  The provided information is validated and accepted. The channel 
  reservation request is removed from the queue.
  DMG_INVALID_PARAMETER The parameter is incorrect.
  DMG_NOT_READY The driver is not able to handle this request at this moment.
  (SWE initiali-zation is not done correctly or the channel is already de-queued).
  DMG_ACTION_NOT_ALLOWED  There is no queued channel reservation request of 
  this client available.
  DMG_MEMORY_ERR  The available memory within the DMG SWE is insufficient to 
  process the command. 
  
  structure member result.channel 
  For this action, the value of the message structure member result.channel is 
  to be ignored.

  Note that this API functionality can also be invoked with the message 
  DMG_REMOVE_FROM_QUEUE_REQ_MSG.

  Current restriction of use
  None.
*/
T_RV_RET dmg_remove_from_queue( T_DMG_CHANNEL channel_queue_id, 
                                T_RV_RETURN     return_path );

/**
  Description
  This function allows the release of the reserved DMA channel. Depending on 
  the state of transfer, the release may be immediately or postponed till the
  busy transfer is completed. IF the channel was locked, it is also unlocked.
  If the release must be postponed, the client shall be notified asynchronous 
  as soon as the release is possible (status message DMG_CHANNEL_RELEASED, 
  using the return path provided at reserving time). 

  Releasing a channel enables the DMG SWE to serve a queued reservation request.
  The channel shall be assigned to a queued client with the highest priority or 
  when several queued reservations have equal priority the channel will be 
  assigned on the FIFO principle.

  The function returns immediately and handles the request asynchronously. 
  The message is then vali-dated and handled. The return path is used to 
  inform the client about the result of the message proc-essing and to 
  inform the client of any asynchronous events.

  Parameters

  @param channel
  channel contains the number of the channel to be released. This identification 
  has been returned at the moment the reservation was granted.

  @param return_path
  Return path for notifications. 
  Immediate Return

  @return T_RV_RET

  The possible values are:
  id  Definition
  RV_OK The API function was successfully executed. Expect status message.
  RV_MEMORY_ERR Insufficient memory to execute the task requested.
  RV_NOT_READY  The driver is not able to handle this request at this moment. 
  (SWE initiali-zation is not done correctly).


  Event Return

  - DMG_STATUS_RSP_MSG
  This message is send to the client to return the status and result of the 
  requested action. Other API's also uses this message to return status or 
  to notify clients of asynchronous events.   

  structure member result.status 
  For this action, the value of the message structure member result.status can 
  have the following val-ues:
  id  Definition
  DMG_OK  The provided information is validated and accepted. The channel is 
  released. If the channel was locked, it is still locked.
  DMG_INVALID_PARAMETER The parameter is incorrect.
  DMG_NOT_READY The driver is not able to handle this request at this moment. 
  (SWE initiali-zation is not done correctly).
  DMG_ACTION_NOT_ALLOWED  There is an incorrect sequence of API invocations 
  (command ignored).
  DMG_MEMORY_ERR  The available memory within the DMG SWE is insufficient 
  to process the command. 
  DMG_CHANNEL_BUSY  The channel could not be released now. An asynchronous 
  message is send as soon the channel could be released (status DMG_CHANNEL_RELEASED).

  structure member result.channel 
  For this action, the value of the message structure member result.channel  
  is to be ignored.

  Note that this API functionality can also be invoked with the message 
  DMG_RELEASE_CHANNEL_REQ_MSG.

  Current restriction of use
  None.
*/

T_RV_RET dmg_release_channel ( T_DMG_CHANNEL  channel,           
                               T_RV_RETURN  return_path);


/**
  Description
  This function returns the driver version.

  Parameters

  None.


  Immediate Return

  @return UINT32

  Bit Name  Function
  [0-15]  BUILD Build number
  [16-23] MINOR Minor version number
  [24-31] MAJOR Major version number


  Event Return

  None.

  Current restriction of use
  None.
*/
UINT32 dmg_get_sw_version(void);



/*@}*/


#ifdef __cplusplus
}
#endif


#endif /*__DMG_API_H_*/

