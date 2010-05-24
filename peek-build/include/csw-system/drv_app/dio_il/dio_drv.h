/*
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
|  Purpose : General driver definitions of DIO driver interface
+-----------------------------------------------------------------------------
*/

#ifndef __DIO_DRV_H__
#define __DIO_DRV_H__

/*==== INCLUDES =============================================================*/

#include "dio.h"

/*==== DEFINITIONS ==========================================================*/

/*
 *  Device Number Mask
 */
#define DIO_DRV_MASK     0xFF000000 /* bit 24-31: Driver number               */
#define DIO_TYPE_MASK    0x00FFFF00 /* bit 08-23: Type of the device          */
#define DIO_DEVICE_MASK  0x000000FF /* bit 00-07: Actual device number        */

/*
 *  Driver Numbers
 */
#define DIO_DRV_UART     0x00000000 /* driver number - UART driver            */
#define DIO_DRV_USB      0x01000000 /* driver number - USB  driver            */
#define DIO_DRV_MUX      0x02000000 /* driver number - 27.010 multiplexer     */
#define DIO_DRV_PKT      0x03000000 /* driver number - Packet driver          */
#define DIO_DRV_MCBSP    0x04000000 /* driver number - McBSP driver           */
#define DIO_DRV_APP      0x05000000 /* driver number - Application adapter    */
#define DIO_DRV_BAT      0x06000000 /* driver number - BAT adapter            */
#define DIO_DRV_CSMI     0x07000000 /* driver number - CSMI driver            */
#define DIO_DRV_BT       0x0D000000 /* driver number - BT driver            */
#define DIO_DRV_RVT      0x0E000000 /* driver number - BAT adapter  */

/*
 *  mux driver mode
 */
#define DIO_MUX_DYNAMIC 0x00 /* maybe started dynamically on runtime */
#define DIO_MUX_START   0x01 /* starts the 27.010 MUX permanently */

/*==== PROTOTYPES ===========================================================*/

extern void dio_user_ready_drv (U32 device_range_start, U32 device_range_end, U16 drv_handle, T_DRV_CB_FUNC signal_callback);
extern U16 dio_user_not_ready_drv (U32 device_range_start, U32 device_range_end);

/*==== TYPES ================================================================*/

typedef struct
{
  U32   device_range_start;
  U32   device_range_end;
  U16   drv_handle;
  T_DRV_CB_FUNC  signal_callback;
} T_DIO_CB_ROUTE;

typedef void (*T_DIO_USER_READY)
                    (U32 device_range_start,
                     U32 device_range_end,
                     U16 drv_handle,
                     T_DRV_CB_FUNC signal_callback);

typedef U16 (*T_DIO_USER_NOT_READY)
                    (U32 device_range_start,
                     U32 device_range_end);

typedef U16 (*T_DIO_GET_CAPABILITIES)
                    (U32 device,
                     T_DIO_CAP **cap);

typedef U16 (*T_DIO_GET_CONFIG)
                    (U32 device,
                     T_DIO_DCB *dcb);

typedef U16 (*T_DIO_SET_CONFIG)
                    (U32 device,
                     T_DIO_DCB *dcb);

typedef U16 (*T_DIO_SET_RX_BUFFER)
                    (U32 device,
                     T_dio_buffer *buffer);

typedef U16 (*T_DIO_READ)
                    (U32 device,
                     T_DIO_CTRL *control_info,
                     T_dio_buffer **buffer);

typedef void(*T_DIO_EXIT) (void);

typedef U16 (*T_DIO_WRITE)
                     (U32 device,
                      T_DIO_CTRL *control_info,
                      T_dio_buffer *buffer);

typedef U16 (*T_DIO_GET_TX_BUFFER)
                      (U32 device,
                       T_dio_buffer **buffer);

typedef U16 (*T_DIO_CLEAR) (U32 device);

typedef U16 (*T_DIO_FLUSH) (U32 device);

typedef U16 (*T_DIO_CLOSE_DEVICE) (U32 device);

typedef  struct
{
   T_DIO_USER_READY       dio_user_ready_drv;
   T_DIO_USER_NOT_READY   dio_user_not_ready_drv;
   T_DIO_EXIT             dio_exit;
   T_DIO_SET_RX_BUFFER    dio_set_rx_buffer;
   T_DIO_READ             dio_read;
   T_DIO_WRITE            dio_write;
   T_DIO_GET_TX_BUFFER    dio_get_tx_buffer;
   T_DIO_CLEAR            dio_clear;
   T_DIO_FLUSH            dio_flush;
   T_DIO_GET_CAPABILITIES dio_get_capabilities;
   T_DIO_SET_CONFIG       dio_set_config;
   T_DIO_GET_CONFIG       dio_get_config;
   T_DIO_CLOSE_DEVICE     dio_close_device;
} T_DIO_FUNC;

typedef struct
{
  U16 max_devices; /* holds the maximum number of serial USB device the driver will open */
} T_DIO_DRV;

/* mux driver init structure*/
typedef struct
{
  U16             max_devices;
  void            (*dio_export_ser)();
  U16             (*dio_init_ser)();
  T_DIO_DRV*      drv_init_ser;
  U32             device_ser;
  U16             cb_route_count;
  T_DIO_CB_ROUTE  *cb_route_table;
  U8              start_mux;
  U32             baudrate;
  U32             flow_control;
  U8              xon;
  U8              xoff;
  U32             mux_mode;
  U16             n1;
  U8              n2;
  U8              t1;
  U8              t2;
  U8              t3;
  U8              k;
}T_DIO_DRV_MUX;

typedef struct
{
	U16		max_devices;	/* holds the maximum number of serial USB device the driver will open */
	U16		max_ser_devices;
	U16		max_pkt_devices;
} T_DIO_DRV_USB;

typedef struct
{
  U16   max_devices;
  U16   max_ser_devices;
  U16   max_pkt_devices;
} T_DIO_DRV_APP;

typedef struct
{
  U16   max_devices;
  U16   max_ser_devices;
  U16   max_pkt_devices;
} T_DIO_DRV_BAT;

typedef T_DIO_DRV T_DIO_DRV_RVT;

/*==== PROTOTYPES ===========================================================*/

extern U16 dio_init_drv(T_DIO_DRV *drv_init);
extern void dio_export_drv(T_DIO_FUNC **dio_func);



#endif /* __DIO_DRV_H__ */
