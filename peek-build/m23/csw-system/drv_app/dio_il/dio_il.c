/* 
+-----------------------------------------------------------------------------
|  Project :
|  Modul   : DIO_IL
+-----------------------------------------------------------------------------
|  Copyright 2004 Texas Instruments Berlin, AG
|                 All rights reserved.
|
|                 This file is confidential and a trade secret of Texas
|                 Instruments Berlin, AG
|                 The receipt of or possession of this file does not convey
|                 any rights to reproduce or disclose its contents or to
|                 manufacture, use, or sell anything it may describe, in
|                 whole, or in part, without the specific written consent of
|                 Texas Instruments Berlin, AG.
+-----------------------------------------------------------------------------
|  Purpose :  This module is the general part of the DIO interface layer
|             (DIO_IL). It contains the functional DIO interface to all DIO
|             users.
+-----------------------------------------------------------------------------
*/

/*==== INCLUDES =============================================================*/
#include <string.h>
#include "dio_config.h"

/*==== DEFINITIONS ==========================================================*/

/*
 * position of Driver number
 */
#define DIO_DRV_POS      24

/*==== TYPES ================================================================*/

/*==== PROTOTYPES ===========================================================*/

/*==== GLOBAL VARS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/
static BOOL is_dio_initialized = FALSE;
static U16 dioil_rt_size       = 0;
static U16 dioil_ut_size       = 0;

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/



/*
+------------------------------------------------------------------------------
|  Function : dio_init_config
+------------------------------------------------------------------------------
|  Description : This function implements the serialswitch configuration in the
|                DIO interface. It is a mechanisme on the ARM7 side which allows
|                specifying the data flows at boot time.
|
|  Parameter : int configuration index (0,1 or2)
|
|  Return : none
|
|  NOTE:  The compiler flag DIOIL_RT_SIZE is only used in static
|         configurations. So it can be used here to distinguish between static
|         and dynamic configurations.
+------------------------------------------------------------------------------
*/
void dio_init_config(int cfg)
{
#ifndef DIOIL_RT_SIZE
  dioil_rt_size = dio_il_rt_p[cfg].dio_routing_table_size;
  dioil_ut_size = dio_il_ut_p[cfg].dio_user_info_size;

  dio_il_rt     = dio_il_rt_p[cfg].dio_routing_table;
  dio_il_ut     = dio_il_ut_p[cfg].dio_user_info;
#endif /* ifndef DIOIL_RT_SIZE */
} /* dio_init_config() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_init
+------------------------------------------------------------------------------
|  Description : This function initializes the DIO interface layer and 
|                all DIO drivers.
|
|                The function return DRV_INITIALIZED if the interface has
|                already been initialized and is ready to be used or is
|                already in use.
|
|                In case of an initialization failure, which means the DIO
|                interface cannot be used, the function returns 
|                DRV_INITFAILURE.
|
|  Parameter : none 
|
|  Return : DRV_OK - Initialization successful
|           DRV_INITIALIZED - Interface already initialized
|           DRV_INITFAILURE - Initialization failed
|
|  NOTE:  The compiler flag DIOIL_RT_SIZE is only used in static
|         configurations. So it can be used here to distinguish between static
|         and dynamic configurations.
+------------------------------------------------------------------------------
*/
U16 dio_init( void )
{
  U8  i;
  U16 ret_val;

  if ( is_dio_initialized )
  {
    /* 
     * DIO IL is already initialzed 
     */
    return DRV_INITIALIZED;
  }

#ifdef DIOIL_RT_SIZE
  dioil_rt_size = DIOIL_RT_SIZE;
  dioil_ut_size = DIOIL_UT_SIZE;
#endif /* ifdef DIOIL_RT_SIZE */

  /* 
   * Process each entry in the DIO IL routing table.
   */
  for ( i = 0; i < dioil_rt_size; i++ )
  {
    /* 
     * Initiliaze each driver
     */
    ret_val = DRV_OK;
    if ( dio_il_rt[i].dio_init_drv != NULL )
    {
      ret_val = dio_il_rt[i].dio_init_drv( dio_il_rt[i].t_dio_drv );
    }
    if ( (ret_val == DRV_OK) || (ret_val == DRV_INITIALIZED) )
    {
      /* 
       * Call the export function for each driver
       */
      if ( dio_il_rt[i].dio_export_drv != NULL )
      {
        dio_il_rt[i].dio_export_drv( &dio_il_rt[i].dio_func );
      }
    }
    else
    {
      return DRV_INITFAILURE;
    }
  }

  /* 
   * Set the state of the DIO IL to initialized.
   */
  is_dio_initialized = TRUE;

  return DRV_OK; 

} /* dio_init() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_user_init
+------------------------------------------------------------------------------
|  Description : The function sets the signal_callback function of devices 
|                of the DIO user. It also informs the driver that the user is 
|                now ready to receive signals from the driver.
|
|                The driver(s) store the drv_handle and passes it in the 
|                T_DRV_SIGNAL structure of the Signal parameter to the 
|                calling process every time the callback function is called.
|
|                The signal_callback function must be reentrant and it must 
|                be possible to run the signal_callback function in 
|                interrupt context.
| 
|                The function returns DRV_NOTCONFIGURED if dio_init()
|                has not been called yet. In this case the user cannot use
|                the interface yet. The user should recall the function after
|                dio_init() has been called.
|
|                The function returns DRV_INVALID_PARAMS if the given 
|                user_name is not known to the DIO interface layer.
|               
|  Parameter : 
|    user_name       - user of the DIO interface
|    drv_handle      - handle for this callback function
|    signal_callback - points to the function that is called at the time
|                      an event occurs that is to be signaled
|
|  Return : 
|    DRV_OK             - Callback successfully set
|    DRV_INVALID_PARAMS - Unknown user
|    DRV_INTERNAL_ERROR - Internal error
|    DRV_NOTCONFIGURED  - DIO interface is not yet initialized
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_user_init( char const    *user_name, 
                   U16           drv_handle, 
                   T_DRV_CB_FUNC signal_callback )
{
  U8    id_ut;
  U8    id_rt;
  BOOL  user_in_list = FALSE;

  if ( !is_dio_initialized )
  {
    return DRV_NOTCONFIGURED;
  }

  /*
   * activate each device number range of the user
   */
  for ( id_ut = 0; id_ut < dioil_ut_size; id_ut++ )
  {
    if ( strcmp( dio_il_ut[id_ut].user_name, user_name ) == 0 )
    {
      user_in_list = TRUE;
      /*
       * get routing table index
       */
      id_rt = (U8)((dio_il_ut[id_ut].device_range_start & DIO_DRV_MASK)
                      >> DIO_DRV_POS);
      /*
       * deactivate the devices of that driver
       */
      dio_il_rt[id_rt].dio_func->dio_user_ready_drv ( 
        dio_il_ut[id_ut].device_range_start,
        dio_il_ut[id_ut].device_range_end,
        drv_handle,
        signal_callback );
    }
  }

  if ( !user_in_list )
  {
    return DRV_INVALID_PARAMS;
  }

  return DRV_OK; 

} /* dio_user_init() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_user_exit
+------------------------------------------------------------------------------
|  Description : The function is called when the DIO interface functionality 
|                is no longer required by a user. That means the 
|                signal_callback function of the user will not be called
|                any more. All devices of the user need to be closed before
|                the function can be called.
|  
|                The function returns DRV_OK if it was able to terminate
|                the user operation successfully. In case the specified 
|                user_name is not known to the DIO interface layer the 
|                function also returns DRV_OK.
|
|                If there is still a device of the user open then the 
|                function returns DRV_INVALID_PARAMS. In this case 
|                the signal_callback function of the user may still be
|                called. The user should call dio_close_device() in order 
|                to close the devices. After that the function may be 
|                called again.
|
|  Parameter : 
|    user_name  - This is a pointer to a zero terminated string which 
|                 represents the name of the user of the DIO interface. 
|                 User names usually have 3 to 6 characters and they 
|                 do not contain space characters.
|
|  Return : 
|    DRV_OK             - User operation successfully terminated
|    DRV_INVALID_PARAMS - User operation cannot be terminated yet
|    DRV_INTERNAL_ERROR - Internal driver error
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_user_exit( char const *user_name )
{
  U8    id_ut;
  U8    id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  /*
   * deactivate each device number range of the user
   */
  for ( id_ut = 0; id_ut < dioil_ut_size; id_ut++ )
  {
    if ( strcmp( dio_il_ut[id_ut].user_name, user_name ) == 0 )
    {
      /*
       * get routing table index
       */
      id_rt = (U8)((dio_il_ut[id_ut].device_range_start & DIO_DRV_MASK)
                      >> DIO_DRV_POS);
      /*
       * deactivate the devices of that driver
       */
      if ( dio_il_rt[id_rt].dio_func->dio_user_not_ready_drv (
             dio_il_ut[id_ut].device_range_start,
             dio_il_ut[id_ut].device_range_end) != DRV_OK )
      {
        /*
         * there are still some devices open in that device range
         */
        return DRV_INVALID_PARAMS;
      }
    }
  }

  return DRV_OK; 

} /* dio_user_exit() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_exit
+------------------------------------------------------------------------------
|  Description : The function is called when the DIO interface 
|                functionality is no longer required. The function
|                de-allocates the resources (interrupts, buffers, etc.). 
|                The DIO drivers terminate regardless of any outstanding 
|                data to be sent.
|
|  Parameter : none 
|
|  Return : none
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
void dio_exit( void )
{
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return;
  }

  /* 
   * Process each entry in the DIO IL routing table.
   */
  for ( id_rt = 0; id_rt < dioil_rt_size; id_rt++ )
  {
    /* 
     * Exit for each driver
     */
    if ( dio_il_rt[id_rt].dio_func != NULL )
    {
      dio_il_rt[id_rt].dio_func->dio_exit();
    }
  }

  /* 
   * Set the state of the DIO IL to uninitialized.
   */
  is_dio_initialized = FALSE;

} /* dio_exit() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_set_rx_buffer
+------------------------------------------------------------------------------
|  Description : This function provides a receive buffer to the driver. 
|                The driver should use this buffer to store the received
|                data of the specified device. The function should always
|                return immediately after overtaking the buffer, without
|                waiting for data.
|
|                To avoid reception gaps more than one receive buffer 
|                should be provided to the driver via several calls of 
|                this function. The provided buffers should be used in the
|                order they were provided. If the driver is not able to take
|                over the provided buffer (e.g. because its internal data
|                buffer queue is full) the function returns DRV_BUFFER_FULL.
|
|                The driver uses the DRV_SIGTYPE_READ signal when data is 
|                received.
|
|  Parameter : 
|    device      - Data device number
|    buffer      - Data buffer description
|
|
|  Return : 
|    DRV_OK             - Function successful.
|    DRV_BUFFER_FULL    - Buffer queue full.
|    DRV_INVALID_PARAMS - The specified device does not exist or 
|                             the data buffer is not big enough.
|    DRV_INTERNAL_ERROR - Internal driver error.
|    DRV_NOTCONFIGURED  - The device is not yet configured.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_set_rx_buffer( U32 device, T_dio_buffer *buffer )
{ 
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  id_rt = (U8)((device & DIO_DRV_MASK) >> DIO_DRV_POS);

  if ( ( id_rt >= dioil_rt_size ) || ( dio_il_rt[id_rt].dio_func == NULL ) )
  {
    /*
     * the device does not exist
     */
    return DRV_INVALID_PARAMS;
  }

  return ( dio_il_rt[id_rt].dio_func->dio_set_rx_buffer( device, buffer ) );

} /* dio_set_rx_buffer() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_read
+------------------------------------------------------------------------------
|  Description : This function returns a receive buffer and control 
|                information. It should always return immediately after
|                changing internal states, without waiting for any more 
|                data. The receive buffers should be returned in the 
|                same order as provided with the dio_set_rx_buffer() 
|                calls (First-In-First-Out). The returned buffer is not 
|                in control of the driver any more.
| 
|                The buffer should be returned even if it is empty. If 
|                there is no receive buffer in control of the driver any
|                more then buffer is set to NULL. In this case only 
|                control information is delivered.
|
|  Parameter : 
|    device       - Data device number
|    control_info - The driver copies control information into the 
|                   provided control buffer.
|    buffer       - Returned data buffer description
|
|  Return : 
|    DRV_OK             - Function successful.
|    DRV_INVALID_PARAMS - The specified device does not exist.
|    DRV_INTERNAL_ERROR - Internal driver error.
|    DRV_NOTCONFIGURED  - The device is not yet configured.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_read( U32 device, T_DIO_CTRL *control_info, T_dio_buffer **buffer )
{ 
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  id_rt = (U8)((device & DIO_DRV_MASK) >> DIO_DRV_POS);

  if ( ( id_rt >= dioil_rt_size ) || ( dio_il_rt[id_rt].dio_func == NULL ) )
  {
    /*
     * the device does not exist
     */
    return DRV_INVALID_PARAMS;
  }

  return ( dio_il_rt[id_rt].dio_func->dio_read( device,
                                                control_info,
                                                buffer ) );

} /* dio_read() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_write
+------------------------------------------------------------------------------
|  Description : This function provides a send buffer to the driver which
|                contains data to send. This function should return 
|                immediately after overtaking the buffer.
|
|                To avoid transmission gaps more than one send buffer
|                should be provided to the driver via several calls of 
|                this function. The provided send buffers should be sent
|                in the order they were provided. If the driver is not able
|                to take over the provided buffer (e.g. because its
|                internal buffer queue is full) the function returns
|                DRV_BUFFET_DIO_DRV *drv_initR_FULL.
|
|                If buffer is set to NULL then the driver only copies
|                the provided control information.
|
|                The driver uses the DRV_SIGTYPE_WRITE signal when the
|                data of the buffer is sent.
|
|  Parameter : 
|    device        - Data device number
|    control_info  - The driver copies control information from the
|                    provided control buffer.
|    buffer        - Data buffer description
|
|  Return : 
|    DRV_OK              - Function successful.
|    DRV_INVALID_PARAMS  - The specified device does not exist
|                              or the data buffer is to big.
|    DRV_INTERNAL_ERROR  - Internal driver error.
|    DRV_NOTCONFIGURED   - The device is not yet configured.
|    DRV_BUFFER_FULL     - Buffer queue full.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_write( U32 device, T_DIO_CTRL *control_info, T_dio_buffer *buffer )
{ 
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  id_rt = (U8)((device & DIO_DRV_MASK) >> DIO_DRV_POS);

  if ( ( id_rt >= dioil_rt_size) || ( dio_il_rt[id_rt].dio_func == NULL ) )
  {
    /*
     * the device does not exist
     */
    return DRV_INVALID_PARAMS;
  }

  return ( dio_il_rt[id_rt].dio_func->dio_write( device,
                                                 control_info,
                                                 buffer ) );

} /* dio_write() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_get_tx_buffer
+------------------------------------------------------------------------------
|  Description : This function returns a send buffer provided via 
|                dio_write(). 
|T_DIO_DRV *drv_init
|                It should always return immediately after
|                changing internal states, without waiting for any
|                outstanding events. The send buffers should be returned
|                in the same order as provided with the dio_write()
|                calls (First-In-First-Out). The returned send buffer is 
|                not in control of the driver any more.
| 
|                If there is no send buffer in control of the driver
|                any more then buffer is set to NULL.
|
|  Parameter : 
|    device      - Data device number
|    buffer      - Returned data buffer description
|
|  Return : 
|    DRV_OK             - Function successful.
|    DRV_INVALID_PARAMS - The specified device does not exist.
|    DRV_INTERNAL_ERROR - Internal driver error.
|    DRV_NOTCONFIGURED  - The device is not yet configured.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_get_tx_buffer( U32 device, T_dio_buffer **buffer )
{ 
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  id_rt = (U8)((device & DIO_DRV_MASK) >> DIO_DRV_POS);

  if ( ( id_rt >= dioil_rt_size ) || ( dio_il_rt[id_rt].dio_func == NULL ) )
  {
    /*
     * the device does not exist
     */
    return DRV_INVALID_PARAMS;
  }

  return ( dio_il_rt[id_rt].dio_func->dio_get_tx_buffer( device, buffer ) ); 

} /* dio_get_tx_buffer() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_clear 
+------------------------------------------------------------------------------
|  Description : This function is used to clear the hardware send buffer.
|                It should always return immediately after changing
|                internal states and resetting internal values, without
|                waiting for any outstanding events.
|
|                If the driver could not clear the hardware send buffer
|                at once the function returns DRV_INPROCESS. In this
|                case the driver will send the signal DRV_SIGTYPE_CLEAR 
|                to the protocol stack when the hardware send buffer
|                is cleared completely.
|
|                If the driver was able to clear the hardware send buffer
|                at once the function returns DRV_OK. In this case
|                the signal DRV_SIGTYPE_CLEAR is not sent to the
|                protocol stack.
|
|  Parameter : device - Data device number
|
|  Return : 
|    DRV_OK              - Function successful
|    DRV_INVALID_PARAMS  - The specified device does not exist
|    DRV_INTERNAL_ERROR  - Internal driver error
|    DRV_NOTCONFIGURED   - The device is not yet configured.
|    DRV_INPROCESS       - The driver is busy clearing the buffer.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_clear(U32 device)
{ 
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  id_rt = (U8)((device & DIO_DRV_MASK) >> DIO_DRV_POS);

  if ( ( id_rt >= dioil_rt_size ) || ( dio_il_rt[id_rt].dio_func == NULL ) )
  {
    /*
     * the device does not exist
     */
    return DRV_INVALID_PARAMS;
  }

  return ( dio_il_rt[id_rt].dio_func->dio_clear( device ) ); 

} /* dio_clear() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_flush
+------------------------------------------------------------------------------
|  Description : With this function the driver is requested to inform
|                the protocol stack, when data of the hardware send
|                buffer have been written successfully. That means the
|                hardware send buffer is empty. The function should 
|                always return immediately after changing internal
|                states without waiting for any outstanding events.
|
|                This function can be used by the protocol stack to
|                ensure that no more data is to be sent (e.g. before
|                switching between command mode and data mode or before
|                changing the settings of the driver).
|
|                If the driver could not complete flushing the buffer
|                at once the function returns DRV_INPROCESS. 
|                In this case the driver will send the signal 
|                DRV_SIGTYPE_FLUSH to the protocol stack when the 
|                buffer is flushed completely.
|
|                If the hardware send buffer is already empty then 
|                the function returns DRV_OK. In this case the signal 
|                DRV_SIGTYPE_FLUSH is not sent to the protocol stack.
|
|  Parameter : device - Data device number
|
|  Return : 
|    DRV_OK             - Function successful.
|    DRV_INVALID_PARAMS - The specified device does not exist.
|    DRV_INTERNAL_ERROR - Internal driver error.
|    DRV_NOTCONFIGURED  - The device is not yet configured.
|    DRV_INPROCESS      - The driver is busy flushing the buffer.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_flush( U32 device )
{ 
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  id_rt = (U8)((device & DIO_DRV_MASK) >> DIO_DRV_POS);

  if ( ( id_rt >= dioil_rt_size ) || ( dio_il_rt[id_rt].dio_func == NULL ) )
  {
    /*
     * the device does not exist
     */
    return DRV_INVALID_PARAMS;
  }

  return ( dio_il_rt[id_rt].dio_func->dio_flush( device ) ); 

} /* dio_flush() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_get_capabilities
+------------------------------------------------------------------------------
|  Description : This function is used to retrieve the capabilities 
|                of a device. The driver returns a pointer to a static
|                struct of constant values. It is not allowed to change
|                these values neither by the driver nor by the protocol
|                stack. 
|
|  Parameter : 
|     device       - Data device number
|     capabilities - Returned pointer to the device capabilities
|
|  Return : 
|    DRV_OK             - Function successfully completed.
|    DRV_INVALID_PARAMS - The specified device does not exist.
|    DRV_INTERNAL_ERROR - Internal driver error.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_get_capabilities( U32 device, T_DIO_CAP **capabilities)
{ 
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  id_rt = (U8)((device & DIO_DRV_MASK) >> DIO_DRV_POS);

  if ( ( id_rt >= dioil_rt_size ) || ( dio_il_rt[id_rt].dio_func == NULL ) )
  {
    /*
     * the device does not exist
     */
    return DRV_INVALID_PARAMS;
  }

  return ( dio_il_rt[id_rt].dio_func->dio_get_capabilities( device,
                                                            capabilities ) );

} /* dio_get_capabilities() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_set_config
+------------------------------------------------------------------------------
|  Description : This function is used to configure a device 
|                (transmission rate, flow control, etc). A device can
|                be configured at any time.
|
|                The dcb points to a Device Control Block. The parameters
|                that can be configured are included in the Device Control
|                Block. 
|
|                If any value of the configuration is out of range, 
|                not supported or invalid in combination with any other
|                value of the configuration, the function returns
|                DRV_INVALID_PARAMS.
|
|                Each device needs to be configured after the reception of
|                a DRV_SIGTYPE_CONNECT signal. Only dio_get_capabilities(), 
|                dio_set_config() and dio_close_device() can be called
|                while the device is not configured. All other device 
|                specific functions return DRV_NOTCONFIGURED.
|
|  Parameter : 
|    device - Data device number
|    dcb    - Pointer to a Device Control Block
|
|  Return : 
|    DRV_OK             - Function successfully completed
|    DRV_INVALID_PARAMS - One or more values are out of range or
|                             invalid in that combination
|    DRV_INTERNAL_ERROR - Internal driver error
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_set_config( U32 device, T_DIO_DCB *dcb )
{ 
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  id_rt = (U8)((device & DIO_DRV_MASK) >> DIO_DRV_POS);

  if ( ( id_rt >= dioil_rt_size ) || ( dio_il_rt[id_rt].dio_func == NULL ) )
  {
    /*
     * the device does not exist
     */
    return DRV_INVALID_PARAMS;
  }

  return ( dio_il_rt[id_rt].dio_func->dio_set_config( device, dcb ) ); 

} /* dio_set_config() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_get_config
+------------------------------------------------------------------------------
|  Description : This function is used to retrieve the configuration
|                of a device. The driver copies the configuration into
|                the Device Control Block provided with dcb.
|
|                To ensure that the configuration data will be copied
|                in an appropriate struct the parameter device_type of
|                the Device Control Block needs to be set properly
|                before the function is called.
|
|  Parameter : 
|    device - Data device number
|    dcb    - Pointer to a Device Control Block
|
|  Return : 
|    DRV_OK             - Function successfully completed.
|    DRV_INVALID_PARAMS - The specified device does not exist or
|                             wrong Device Control Block provided.
|    DRV_INTERNAL_ERROR - Internal driver error.
|    DRV_NOTCONFIGURED  - The device is not yet configured.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_get_config( U32 device, T_DIO_DCB *dcb )
{ 
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  id_rt = (U8)((device & DIO_DRV_MASK) >> DIO_DRV_POS);

  if ( ( id_rt >= dioil_rt_size ) || ( dio_il_rt[id_rt].dio_func == NULL ) )
  {
    /*
     * the device does not exist
     */
    return DRV_INVALID_PARAMS;
  }

  return ( dio_il_rt[id_rt].dio_func->dio_get_config( device, dcb ) ); 

} /* dio_get_config() */



/* 
+------------------------------------------------------------------------------
|  Function : dio_close_device
+------------------------------------------------------------------------------
|  Description : This function is used to close a device. The driver
|                returns DRV_OK if it was able to close the device
|                successfully. In case the specified device does not
|                exist the driver also returns DRV_OK.
|
|                If the driver still controls a protocol stack buffer
|                for this device then it returns DRV_INVALID_PARAMS. 
|                In this case the device is not closed. In order to get
|                the remaining buffers the protocol stack needs to call
|                the functions dio_read() and dio_get_tx_buffer().
|
|  Parameter : device - Data device number
|
|  Return : 
|    DRV_OK             - Device successfully closed.
|    DRV_INVALID_PARAMS - The specified device can not be closed yet.
|    DRV_INTERNAL_ERROR - Internal driver error.
|
|  NOTE:  none
+------------------------------------------------------------------------------
*/
U16 dio_close_device( U32 device )
{ 
  U8 id_rt;

  if ( !is_dio_initialized )
  {
    return DRV_INTERNAL_ERROR;
  }

  id_rt = (U8)((device & DIO_DRV_MASK) >> DIO_DRV_POS);

  if ( ( id_rt >= dioil_rt_size ) || ( dio_il_rt[id_rt].dio_func == NULL ) )
  {
    /*
     * the device does not exist, so it is already closed
     */
    return DRV_OK;
  }

  return ( dio_il_rt[id_rt].dio_func->dio_close_device( device ) ); 

} /* dio_close_device() */
