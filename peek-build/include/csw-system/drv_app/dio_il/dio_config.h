/*
+-----------------------------------------------------------------------------
|  Project :
|  Modul   :
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
|  Purpose :  DIO board dependent configuration
+-----------------------------------------------------------------------------
*/

#ifndef __DIOCONFIG_H__
#define __DIOCONFIG_H__

/*==== COMMON INCLUDES ======================================================*/

#include "dio_drv.h"
#include "swconfig.cfg"

/*==== COMMON DEFINITIONS ===================================================*/

/*==== COMMON TYPES =========================================================*/

typedef struct
{
  U16        (*dio_init_drv)(T_DIO_DRV *drv_init);
  T_DIO_DRV  *t_dio_drv;
  void       (*dio_export_drv)(T_DIO_FUNC **dio_func);
  T_DIO_FUNC *dio_func;
} T_DIO_IL_ROUTING_TABLE;

typedef struct
{
  char const * user_name;
  U32  device_range_start;
  U32  device_range_end;
} T_DIO_IL_USER_INFO;

/*==== BOARD DEPENDENT CONFIGURATION ========================================*/

/******************************************************************************
 * DIOIL_CONFIG == 1
 * Drivers : MUX
 *           SAM
 * Users   : UART entity
 * Comments: support of 1 physical serial connection
 *             multiplexer can be used (serial channels only)
 */
#if (DIOIL_CONFIG == 1)
  /*
   * Include driver prototypes
   */
#include "sam/dio_sam.h"
#include "mux/mux.h"
  /*
   * UART driver configuration
   */
#define DIOIL_UART_MAX_DEVICES         1
static T_DIO_DRV drv_init_uart =
                   {
                     DIOIL_UART_MAX_DEVICES /* max_devices */
                   };
  /*
   * MUX driver configuration
   */
#define DIOIL_MUX_USER_COUNT           2
#define DIOIL_MUX_MAX_DEVICES          6
static T_DIO_CB_ROUTE dio_il_mux_route[DIOIL_MUX_USER_COUNT]; /* routing table for mux */
static T_DIO_DRV_MUX drv_init_mux =
                       {
                         DIOIL_MUX_MAX_DEVICES,           /* max_devices */
                         dio_export_uart,                 /* dio_export_ser */
                         dio_init_uart,                   /* dio_init_ser */
                         &drv_init_uart,                  /* drv_init_ser */
                         DIO_DRV_UART | DIO_TYPE_SER | 0, /* device_ser */
                         DIOIL_MUX_USER_COUNT,            /* cb_route_count */
                         dio_il_mux_route,                /* cb_route_table */
                         DIO_MUX_DYNAMIC,                 /* start_mux */
                         DIO_BAUD_115200,                 /* baudrate */
                         DIO_FLOW_RTS_CTS,                /* flow_control */
                         0,                               /* xon */
                         0,                               /* xoff */
                         DIO_MUX_UIH,                     /* mux_mode */
                         64,                              /* n1 */
                         3,                               /* n2 */
                         100,                             /* t1 */
                         300,                             /* t2 */
                         10,                              /* t3 */
                         2                                /* k */
                       };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  3
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv, t_dio_drv,                 dio_export_drv, dio_func */
{
  {NULL,         NULL,                      dio_export_mux, NULL }, /* UART driver */
  {NULL,         NULL,                      NULL,           NULL }, /* USB driver */
  {dio_init_mux, (T_DIO_DRV*)&drv_init_mux, dio_export_mux, NULL }  /* MUX driver */
};
  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  2
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,               device_range_end */
{
  {"UART", DIO_DRV_UART | DIO_TYPE_SER_MUX | 0, DIO_DRV_UART | DIO_TYPE_SER_MUX | 0},
  {"UART", DIO_DRV_MUX  | DIO_TYPE_SER     | 1, DIO_DRV_MUX  | DIO_TYPE_SER     | 63}
};
#endif /* (DIOIL_CONFIG == 1) */



/******************************************************************************
 * DIOIL_CONFIG == 2
 * Drivers : USB
 * Users   : PSI entity
 * Comments: support of USB for serial data transfer
 */
#if (DIOIL_CONFIG == 2)
  /*
   * Include driver prototypes
   */
#include "usbfax/dio_usb.h"
  /*
   * USB driver configuration
   */
#define DIOIL_USB_MAX_DEVICES          1
#define DIOIL_USB_MAX_SER_DEVICES      1
#define DIOIL_USB_MAX_PKT_DEVICES      0
static T_DIO_DRV_USB drv_init_usb =
                       {
                         DIOIL_USB_MAX_DEVICES,     /* max_devices */
                         DIOIL_USB_MAX_SER_DEVICES, /* max_ser_devices */
                         DIOIL_USB_MAX_PKT_DEVICES  /* max_pkt_devices */
                       };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  2
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv, t_dio_drv,                 dio_export_drv, dio_func */
{
  {NULL,         NULL,                      NULL,           NULL }, /* UART driver */
  {dio_init_usb, (T_DIO_DRV*)&drv_init_usb, dio_export_usb, NULL }, /* USB driver */
};
  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  1
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,         device_range_end */
{
  {"PSI", DIO_DRV_USB | DIO_TYPE_SER | 0, DIO_DRV_USB | DIO_TYPE_SER | 255}
};
#endif /* (DIOIL_CONFIG == 2) */



/******************************************************************************
 * DIOIL_CONFIG == 3
 * Drivers : USB
 *           MUX
 *           SAM
 * Users   : PSI entity
 *           UART entity
 * Comments: support of USB for serial data transfer;
 *           support of 1 physical serial connection
 *             multiplexer can be used (serial channels only)
 */
#if (DIOIL_CONFIG == 3)
  /*
   * Include driver prototypes
   */
#include "usbfax/dio_usb.h"
#include "sam/dio_sam.h"
#include "mux/mux.h"
  /*
   * USB driver configuration
   */
#define DIOIL_USB_MAX_DEVICES          1
#define DIOIL_USB_MAX_SER_DEVICES      1
#define DIOIL_USB_MAX_PKT_DEVICES      0
static T_DIO_DRV_USB drv_init_usb =
                       {
                         DIOIL_USB_MAX_DEVICES,     /* max_devices */
                         DIOIL_USB_MAX_SER_DEVICES, /* max_ser_devices */
                         DIOIL_USB_MAX_PKT_DEVICES  /* max_pkt_devices */
                       };
  /*
   * UART driver configuration
   */
#define DIOIL_UART_MAX_DEVICES         1
static T_DIO_DRV drv_init_uart =
                   {
                     DIOIL_UART_MAX_DEVICES /* max_devices */
                   };
  /*
   * MUX driver configuration
   */
#define DIOIL_MUX_USER_COUNT           2
#define DIOIL_MUX_MAX_DEVICES          6
static T_DIO_CB_ROUTE dio_il_mux_route[DIOIL_MUX_USER_COUNT]; /* routing table for mux */
static T_DIO_DRV_MUX drv_init_mux =
                       {
                         DIOIL_MUX_MAX_DEVICES,           /* max_devices */
                         dio_export_uart,                 /* dio_export_ser */
                         dio_init_uart,                   /* dio_init_ser */
                         &drv_init_uart,                  /* drv_init_ser */
                         DIO_DRV_UART | DIO_TYPE_SER | 0, /* device_ser */
                         DIOIL_MUX_USER_COUNT,            /* cb_route_count */
                         dio_il_mux_route,                /* cb_route_table */
                         DIO_MUX_DYNAMIC,                 /* start_mux */
                         DIO_BAUD_115200,                 /* baudrate */
                         DIO_FLOW_RTS_CTS,                /* flow_control */
                         0,                               /* xon */
                         0,                               /* xoff */
                         DIO_MUX_UIH,                     /* mux_mode */
                         64,                              /* n1 */
                         3,                               /* n2 */
                         100,                             /* t1 */
                         300,                             /* t2 */
                         10,                              /* t3 */
                         2                                /* k */
                       };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  3
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv, t_dio_drv,                 dio_export_drv, dio_func */
{
  {NULL,         NULL,                      dio_export_mux, NULL }, /* UART driver */
  {dio_init_usb, (T_DIO_DRV*)&drv_init_usb, dio_export_usb, NULL }, /* USB driver */
  {dio_init_mux, (T_DIO_DRV*)&drv_init_mux, dio_export_mux, NULL }  /* MUX driver */
};
  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  3
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,               device_range_end */
{
  {"PSI",  DIO_DRV_USB  | DIO_TYPE_SER     | 0, DIO_DRV_USB  | DIO_TYPE_SER     | 255},
  {"UART", DIO_DRV_UART | DIO_TYPE_SER_MUX | 0, DIO_DRV_UART | DIO_TYPE_SER_MUX | 0},
  {"UART", DIO_DRV_MUX  | DIO_TYPE_SER     | 1, DIO_DRV_MUX  | DIO_TYPE_SER     | 63}
};
#endif /* (DIOIL_CONFIG == 3) */



/******************************************************************************
 * DIOIL_CONFIG == 4
 * Drivers : USB
 *           MUX
 *           SAM
 * Users   : PSI entity
 *           UART entity
 *           PKTIO entity
 * Comments: support of USB for serial data transfer;
 *           support of 1 physical serial connection
 *             multiplexer can be used (including multiple PDP context)
 */
#if (DIOIL_CONFIG == 4)
  /*
   * Include driver prototypes
   */
#include "usbfax/dio_usb.h"
#include "sam/dio_sam.h"
#include "mux/mux.h"
  /*
   * USB driver configuration
   */
#define DIOIL_USB_MAX_DEVICES          1
#define DIOIL_USB_MAX_SER_DEVICES      1
#define DIOIL_USB_MAX_PKT_DEVICES      0
static T_DIO_DRV_USB drv_init_usb =
                       {
                         DIOIL_USB_MAX_DEVICES,     /* max_devices */
                         DIOIL_USB_MAX_SER_DEVICES, /* max_ser_devices */
                         DIOIL_USB_MAX_PKT_DEVICES  /* max_pkt_devices */
                       };
  /*
   * UART driver configuration
   */
#define DIOIL_UART_MAX_DEVICES         1
static T_DIO_DRV drv_init_uart =
                   {
                     DIOIL_UART_MAX_DEVICES /* max_devices */
                   };
  /*
   * MUX driver configuration
   */
#define DIOIL_MUX_USER_COUNT           3
#define DIOIL_MUX_MAX_DEVICES          6
static T_DIO_CB_ROUTE dio_il_mux_route[DIOIL_MUX_USER_COUNT]; /* routing table for mux */
static T_DIO_DRV_MUX drv_init_mux =
                       {
                         DIOIL_MUX_MAX_DEVICES,           /* max_devices */
                         dio_export_uart,                 /* dio_export_ser */
                         dio_init_uart,                   /* dio_init_ser */
                         &drv_init_uart,                  /* drv_init_ser */
                         DIO_DRV_UART | DIO_TYPE_SER | 0, /* device_ser */
                         DIOIL_MUX_USER_COUNT,            /* cb_route_count */
                         dio_il_mux_route,                /* cb_route_table */
                         DIO_MUX_DYNAMIC,                 /* start_mux */
                         DIO_BAUD_115200,                 /* baudrate */
                         DIO_FLOW_RTS_CTS,                /* flow_control */
                         0,                               /* xon */
                         0,                               /* xoff */
                         DIO_MUX_UIH,                     /* mux_mode */
                         64,                              /* n1 */
                         3,                               /* n2 */
                         100,                             /* t1 */
                         300,                             /* t2 */
                         10,                              /* t3 */
                         2                                /* k */
                       };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  3
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv, t_dio_drv,                 dio_export_drv, dio_func */
{
  {NULL,         NULL,                      dio_export_mux, NULL }, /* UART driver */
  {dio_init_usb, (T_DIO_DRV*)&drv_init_usb, dio_export_usb, NULL }, /* USB driver */
  {dio_init_mux, (T_DIO_DRV*)&drv_init_mux, dio_export_mux, NULL }  /* MUX driver */
};
  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  4
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,               device_range_end */
{
  {"PSI",  DIO_DRV_USB  | DIO_TYPE_SER     | 0, DIO_DRV_USB  | DIO_TYPE_SER     | 255},
  {"UART", DIO_DRV_UART | DIO_TYPE_SER_MUX | 0, DIO_DRV_UART | DIO_TYPE_SER_MUX | 0},
  {"UART", DIO_DRV_MUX  | DIO_TYPE_SER     | 1, DIO_DRV_MUX  | DIO_TYPE_SER     | 63},
  {"PKT",  DIO_DRV_MUX  | DIO_TYPE_PKT     | 1, DIO_DRV_MUX  | DIO_TYPE_PKT     | 63}
};
#endif /* (DIOIL_CONFIG == 4) */



/******************************************************************************
 * DIOIL_CONFIG == 5
 * Drivers : MUX
 *           SAM
 * Users   : UART entity
 *           PKTIO entity
 * Comments: support of 1 physical serial connection
 *             multiplexer can be used (including multiple PDP context)
 */
#if (DIOIL_CONFIG == 5)
  /*
   * Include driver prototypes
   */
#include "sam/dio_sam.h"
#include "mux/mux.h"
  /*
   * UART driver configuration
   */
#define DIOIL_UART_MAX_DEVICES         1
static T_DIO_DRV drv_init_uart =
                   {
                     DIOIL_UART_MAX_DEVICES /* max_devices */
                   };
  /*
   * MUX driver configuration
   */
#define DIOIL_MUX_USER_COUNT           3
#define DIOIL_MUX_MAX_DEVICES          6
static T_DIO_CB_ROUTE dio_il_mux_route[DIOIL_MUX_USER_COUNT]; /* routing table for mux */
static T_DIO_DRV_MUX drv_init_mux =
                       {
                         DIOIL_MUX_MAX_DEVICES,           /* max_devices */
                         dio_export_uart,                 /* dio_export_ser */
                         dio_init_uart,                   /* dio_init_ser */
                         &drv_init_uart,                  /* drv_init_ser */
                         DIO_DRV_UART | DIO_TYPE_SER | 0, /* device_ser */
                         DIOIL_MUX_USER_COUNT,            /* cb_route_count */
                         dio_il_mux_route,                /* cb_route_table */
                         DIO_MUX_DYNAMIC,                 /* start_mux */
                         DIO_BAUD_115200,                 /* baudrate */
                         DIO_FLOW_RTS_CTS,                /* flow_control */
                         0,                               /* xon */
                         0,                               /* xoff */
                         DIO_MUX_UIH,                     /* mux_mode */
                         64,                              /* n1 */
                         3,                               /* n2 */
                         100,                             /* t1 */
                         300,                             /* t2 */
                         10,                              /* t3 */
                         2                                /* k */
                       };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  3
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv, t_dio_drv,                 dio_export_drv, dio_func */
{
  {NULL,         NULL,                      dio_export_mux, NULL }, /* UART driver */
  {NULL,         NULL,                      NULL,           NULL }, /* USB driver */
  {dio_init_mux, (T_DIO_DRV*)&drv_init_mux, dio_export_mux, NULL }  /* MUX driver */
};
  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  3
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,               device_range_end */
{
  {"UART", DIO_DRV_UART | DIO_TYPE_SER_MUX | 0, DIO_DRV_UART | DIO_TYPE_SER_MUX | 0},
  {"UART", DIO_DRV_MUX  | DIO_TYPE_SER     | 1, DIO_DRV_MUX  | DIO_TYPE_SER     | 63},
  {"PKT",  DIO_DRV_MUX  | DIO_TYPE_PKT     | 1, DIO_DRV_MUX  | DIO_TYPE_PKT     | 63}
};
#endif /* (DIOIL_CONFIG == 5) */



/******************************************************************************
 * DIOIL_CONFIG == 6
 * Drivers : SAM
 *           PKT
 * Users   : UART entity
 *           PKTIO entity
 * Comments: support of 2 physical serial connection;
 *           multiple PDP context via Packet driver (PKT)
 */
#if (DIOIL_CONFIG == 6)
  /*
   * Include driver prototypes
   */
#include "sam/dio_sam.h"
#include "pkt/pkt.h"
  /*
   * UART driver configuration
   */
#define DIOIL_UART_MAX_DEVICES         2
static T_DIO_DRV drv_init_uart =
                   {
                     DIOIL_UART_MAX_DEVICES /* max_devices */
                   };
  /*
   * Packet driver configuration
   */
#define DIOIL_PKT_MAX_DEVICES          6
static T_DIO_DRV drv_init_pkt =
                   {
                     DIOIL_PKT_MAX_DEVICES /* max_devices */
                   };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  4
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv,  t_dio_drv,                  dio_export_drv,  dio_func */
{
  {dio_init_uart, (T_DIO_DRV*)&drv_init_uart, dio_export_uart, NULL }, /* UART driver */
  {NULL,          NULL,                       NULL,            NULL }, /* USB driver */
  {NULL,          NULL,                       NULL,            NULL }, /* MUX driver */
  {dio_init_pkt,  (T_DIO_DRV*)&drv_init_pkt,  dio_export_pkt,  NULL }  /* Packet driver */
};
  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  2
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,           device_range_end */
{
  {"UART", DIO_DRV_UART | DIO_TYPE_SER | 0, DIO_DRV_UART | DIO_TYPE_SER | 255},
  {"PKT",  DIO_DRV_PKT  | DIO_TYPE_PKT | 0, DIO_DRV_PKT  | DIO_TYPE_PKT | 255}
};
#endif /* (DIOIL_CONFIG == 6) */



/******************************************************************************
 * DIOIL_CONFIG == 7
 * Drivers : MUX
 *           SAM
 *           PKT
 * Users   : UART entity
 *           PKTIO entity
 * Comments: support of 1 physical serial connection;
 *             multiplexer can be used (excluding multiple PDP context)
 *             multiple PDP context via Packet driver (PKT)
 */
#if (DIOIL_CONFIG == 7)
  /*
   * Include driver prototypes
   */
#include "sam/dio_sam.h"
#include "pkt/pkt.h"
#include "mux/mux.h"
  /*
   * UART driver configuration
   */
#define DIOIL_UART_MAX_DEVICES         1
static T_DIO_DRV drv_init_uart =
                   {
                     DIOIL_UART_MAX_DEVICES /* max_devices */
                   };
  /*
   * Packet driver configuration
   */
#define DIOIL_PKT_MAX_DEVICES          6
static T_DIO_DRV drv_init_pkt =
                   {
                     DIOIL_PKT_MAX_DEVICES /* max_devices */
                   };
  /*
   * MUX driver configuration
   */
#define DIOIL_MUX_USER_COUNT           3
#define DIOIL_MUX_MAX_DEVICES          6
static T_DIO_CB_ROUTE dio_il_mux_route[DIOIL_MUX_USER_COUNT]; /* routing table for mux */
static T_DIO_DRV_MUX drv_init_mux =
                       {
                         DIOIL_MUX_MAX_DEVICES,           /* max_devices */
                         dio_export_uart,                 /* dio_export_ser */
                         dio_init_uart,                   /* dio_init_ser */
                         &drv_init_uart,                  /* drv_init_ser */
                         DIO_DRV_UART | DIO_TYPE_SER | 0, /* device_ser */
                         DIOIL_MUX_USER_COUNT,            /* cb_route_count */
                         dio_il_mux_route,                /* cb_route_table */
                         DIO_MUX_DYNAMIC,                 /* start_mux */
                         DIO_BAUD_115200,                 /* baudrate */
                         DIO_FLOW_RTS_CTS,                /* flow_control */
                         0,                               /* xon */
                         0,                               /* xoff */
                         DIO_MUX_UIH,                     /* mux_mode */
                         64,                              /* n1 */
                         3,                               /* n2 */
                         100,                             /* t1 */
                         300,                             /* t2 */
                         10,                              /* t3 */
                         2                                /* k */
                       };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  4
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv,  t_dio_drv,                  dio_export_drv,  dio_func */
{
  {NULL,          NULL,                       dio_export_mux,  NULL }, /* UART driver */
  {NULL,          NULL,                       NULL,            NULL }, /* USB driver */
  {dio_init_mux,  (T_DIO_DRV*)&drv_init_mux,  dio_export_mux,  NULL }, /* MUX driver */
  {dio_init_pkt,  (T_DIO_DRV*)&drv_init_pkt,  dio_export_pkt,  NULL }  /* Packet driver */
};
  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  4
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,               device_range_end */
{
  {"UART", DIO_DRV_UART | DIO_TYPE_SER_MUX | 0, DIO_DRV_UART | DIO_TYPE_SER_MUX | 0},
  {"UART", DIO_DRV_MUX  | DIO_TYPE_SER     | 1, DIO_DRV_MUX  | DIO_TYPE_SER     | 63},
  {"PKT",  DIO_DRV_MUX  | DIO_TYPE_PKT     | 1, DIO_DRV_MUX  | DIO_TYPE_PKT     | 63},
  {"PKT",  DIO_DRV_PKT  | DIO_TYPE_PKT     | 0, DIO_DRV_PKT  | DIO_TYPE_PKT     | 255}
};
#endif /* (DIOIL_CONFIG == 7) */



/******************************************************************************
 * DIOIL_CONFIG == 8
 * Drivers : PKT
 * Users   : PKTIO entity
 * Comments: multiple PDP context via Packet driver (PKT)
 */
#if (DIOIL_CONFIG == 8)
  /*
   * Include driver prototypes
   */
#include "pkt/pkt.h"
  /*
   * Packet driver configuration
   */
#define DIOIL_PKT_MAX_DEVICES          6
static T_DIO_DRV drv_init_pkt =
                   {
                     DIOIL_PKT_MAX_DEVICES /* max_devices */
                   };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  4
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv,  t_dio_drv,                  dio_export_drv,  dio_func */
{
  {NULL,          NULL,                       NULL,            NULL }, /* UART driver */
  {NULL,          NULL,                       NULL,            NULL }, /* USB driver */
  {NULL,          NULL,                       NULL,            NULL }, /* MUX driver */
  {dio_init_pkt,  (T_DIO_DRV*)&drv_init_pkt,  dio_export_pkt,  NULL }  /* Packet driver */
};
  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  1
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,               device_range_end */
{
  {"PKT",  DIO_DRV_PKT  | DIO_TYPE_PKT     | 0, DIO_DRV_PKT  | DIO_TYPE_PKT     | 255}
};
#endif /* (DIOIL_CONFIG == 8) */



/******************************************************************************
 * DIOIL_CONFIG == 9
 * Drivers : USB
 *           BAT
 * Users   : PSI entity
 * Comments: support of USB for serial data transfer
 *           support of BAT for packet data transfer
 */
#if (DIOIL_CONFIG == 9)
  /*
   * Include driver prototypes
   */
#include "usbfax/dio_usb.h"

  /*
   * Declared the init/export functions for the BAT and APP driver.
   * Since these are software drivers and do not have a subdirectory within 
   * csw-system/drv_app, no header files ("dio_bat.h"/"dio_app.h") are available.
   */
U16	dio_init_bat(T_DIO_DRV* drv_init);
void	dio_export_bat(T_DIO_FUNC** dio_func);
U16	dio_init_app(T_DIO_DRV* drv_init);
void	dio_export_app(T_DIO_FUNC** dio_func);

  /*
   * USB driver configuration
   */
#define DIOIL_USB_MAX_DEVICES          1
#define DIOIL_USB_MAX_SER_DEVICES      1
#define DIOIL_USB_MAX_PKT_DEVICES      0
static T_DIO_DRV_USB drv_init_usb =
                       {
                         DIOIL_USB_MAX_DEVICES,     /* max_devices */
                         DIOIL_USB_MAX_SER_DEVICES, /* max_ser_devices */
                         DIOIL_USB_MAX_PKT_DEVICES  /* max_pkt_devices */
                       };
  /*
   * Application adapter configuration
   */
#define DIOIL_APP_MAX_DEVICES          4
#define DIOIL_APP_MAX_SER_DEVICES      1
#define DIOIL_APP_MAX_PKT_DEVICES      3
static T_DIO_DRV_APP drv_init_app =
                       {
                         DIOIL_APP_MAX_DEVICES,     /* max_devices */
                         DIOIL_APP_MAX_SER_DEVICES, /* max_ser_devices */
                         DIOIL_APP_MAX_PKT_DEVICES  /* max_pkt_devices */
                       };
  /*
   * binary AT command adapter configuration
   */
#define DIOIL_BAT_MAX_DEVICES          4
#define DIOIL_BAT_MAX_SER_DEVICES      0
#define DIOIL_BAT_MAX_PKT_DEVICES      4
static T_DIO_DRV_BAT drv_init_bat =
                       {
                         DIOIL_BAT_MAX_DEVICES,     /* max_devices */
                         DIOIL_BAT_MAX_SER_DEVICES, /* max_ser_devices */
                         DIOIL_BAT_MAX_PKT_DEVICES  /* max_pkt_devices */
                       };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  7
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv, t_dio_drv,                 dio_export_drv, dio_func */
{
  {NULL,         NULL,                      NULL,           NULL }, /* UART driver */
  {dio_init_usb, (T_DIO_DRV*)&drv_init_usb, dio_export_usb, NULL }, /* USB driver */
  {NULL,         NULL,                      NULL,           NULL }, /* MUX driver */
  {NULL,         NULL,                      NULL,           NULL }, /* Packet driver */
  {NULL,         NULL,                      NULL,           NULL }, /* McBSP driver */
  {NULL,         NULL,                      NULL,           NULL }, /* Application adapter */
  {dio_init_bat, (T_DIO_DRV*)&drv_init_bat, dio_export_bat, NULL }  /* BAT adapter */
};
  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  2
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,         device_range_end */
{
  {"PSI", DIO_DRV_USB | DIO_TYPE_SER | 0, DIO_DRV_USB | DIO_TYPE_SER | 255},
/** RITTER, 11 March 2005 - deactivated APP driver for now.
    For re-activation, uncomment the two linew below, set DIOIL_UT_SIZE to 4 and
    set the corresponding functions in the dio_il_rt router table above */
/*  {"PSI", DIO_DRV_APP | DIO_TYPE_SER | 0, DIO_DRV_APP | DIO_TYPE_SER | 255}, */
/*  {"PSI", DIO_DRV_APP | DIO_TYPE_PKT | 0, DIO_DRV_APP | DIO_TYPE_PKT | 255}, */
  {"PSI", DIO_DRV_BAT | DIO_TYPE_PKT | 0, DIO_DRV_BAT | DIO_TYPE_PKT | 255}
};
#endif /* (DIOIL_CONFIG == 9) */

/*Added by x0035470 - for Bluetooth DUN*/

/******************************************************************************
 * DIOIL_CONFIG == 10
 * Drivers : BAT
 *           BT
 * Users   : PSI entity
 * Comments: support of BAT for packet data transfer
 */
#if (DIOIL_CONFIG == 10)
  /*
   * Include driver prototypes
   */
#include "../../../../../../EBTIPS/bthal/inc/int/dio_bt.h"
 #include "usbfax/dio_usb.h"
 /*
   * Declared the init/export functions for the BAT and APP adapter.
   * Since these are software drivers and do not have a subdirectory within
   * csw-system/drv_app, no header files ("dio_bat.h"/"dio_app.h") are available.
   */
U16 dio_init_bat(T_DIO_DRV* drv_init);
void  dio_export_bat(T_DIO_FUNC** dio_func);
U16 dio_init_app(T_DIO_DRV* drv_init);
void  dio_export_app(T_DIO_FUNC** dio_func);

  /*
   * USB driver configuration
   */
#define DIOIL_USB_MAX_DEVICES          1
#define DIOIL_USB_MAX_SER_DEVICES      1
#define DIOIL_USB_MAX_PKT_DEVICES      0
static T_DIO_DRV_USB drv_init_usb =
                       {
                         DIOIL_USB_MAX_DEVICES,     /* max_devices */
                         DIOIL_USB_MAX_SER_DEVICES, /* max_ser_devices */
                         DIOIL_USB_MAX_PKT_DEVICES  /* max_pkt_devices */
                       };
  /*
   * Application adapter configuration
   */
#define DIOIL_APP_MAX_DEVICES          4
#define DIOIL_APP_MAX_SER_DEVICES      1
#define DIOIL_APP_MAX_PKT_DEVICES      3
static T_DIO_DRV_APP drv_init_app =
                       {
                         DIOIL_APP_MAX_DEVICES,     /* max_devices */
                         DIOIL_APP_MAX_SER_DEVICES, /* max_ser_devices */
                         DIOIL_APP_MAX_PKT_DEVICES  /* max_pkt_devices */
                       };
  /*
   * binary AT command adapter configuration
   */
#define DIOIL_BAT_MAX_DEVICES          4
#define DIOIL_BAT_MAX_SER_DEVICES      0
#define DIOIL_BAT_MAX_PKT_DEVICES      4
static T_DIO_DRV_BAT drv_init_bat =
                       {
                         DIOIL_BAT_MAX_DEVICES,     /* max_devices */
                         DIOIL_BAT_MAX_SER_DEVICES, /* max_ser_devices */
                         DIOIL_BAT_MAX_PKT_DEVICES  /* max_pkt_devices */
                       };
  /*
   * BT driver configuration
   */
#define DIOIL_BT_MAX_DEVICES           1
static T_DIO_DRV drv_init_bt =
                   {
                     DIOIL_BT_MAX_DEVICES /* max_devices */
                   };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  14
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv, t_dio_drv,                 dio_export_drv, dio_func */
{
  {NULL,         NULL,                      NULL,           NULL }, /* UART driver */
  {dio_init_usb, (T_DIO_DRV*)&drv_init_usb, dio_export_usb, NULL }, /* USB driver */
  {NULL,         NULL,                      NULL,           NULL }, /* MUX driver */
  {NULL,         NULL,                      NULL,           NULL }, /* Packet driver */
  {NULL,         NULL,                      NULL,           NULL }, /* McBSP driver */
  {NULL,         NULL,                      NULL,           NULL }, /* Application adapter */
  {dio_init_bat, (T_DIO_DRV*)&drv_init_bat, dio_export_bat, NULL }, /* BAT adapter */
  {NULL,         NULL,                      NULL,           NULL }, /* Serial CSMI driver */
  {NULL,         NULL,                      NULL,           NULL }, /* Serial ICX adapter */
  {NULL,         NULL,                      NULL,           NULL }, /* ICX packet multiplexer adapter */
  {NULL,         NULL,                      NULL,           NULL }, /* TCP/IP adapter */
  {NULL,         NULL,                      NULL,           NULL }, /* Socket adapter */
  {NULL,         NULL,                      NULL,           NULL }, /* Socket configuration adapter */
  {dio_init_bt,  (T_DIO_DRV*)&drv_init_bt,  dio_export_bt,  NULL }  /* Bluetooth driver */
};
  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  3
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,         device_range_end */
{
  {"PSI", DIO_DRV_USB | DIO_TYPE_SER | 0, DIO_DRV_USB | DIO_TYPE_SER | 255},
/** RITTER, 11 March 2005 - deactivated APP driver for now.
    For re-activation, uncomment the two linew below, set DIOIL_UT_SIZE to 4 and
    set the corresponding functions in the dio_il_rt router table above */
/*  {"PSI", DIO_DRV_APP | DIO_TYPE_SER | 0, DIO_DRV_APP | DIO_TYPE_SER | 255}, */
/*  {"PSI", DIO_DRV_APP | DIO_TYPE_PKT | 0, DIO_DRV_APP | DIO_TYPE_PKT | 255}, */
  {"PSI", DIO_DRV_BAT | DIO_TYPE_PKT | 0, DIO_DRV_BAT | DIO_TYPE_PKT | 255},
  {"PSI", DIO_DRV_BT  | DIO_TYPE_SER | 0, DIO_DRV_BT  | DIO_TYPE_SER | 255}
};
#endif /* (DIOIL_CONFIG == 10) */


/******************************************************************************
 * DIOIL_CONFIG == 11
 * Drivers : RVT
 * Users   : PSI entity
 * Comments: support of RVT for serial data transfer
 */
#if (DIOIL_CONFIG == 11)

  /*
   * Include driver prototypes
   */
#include "rvt_dio.h"

  /*
   * RVT driver configuration
   */
#define DIOIL_RVT_MAX_DEVICES          1
static T_DIO_DRV_RVT drv_init_rvt =
                       {
                         DIOIL_RVT_MAX_DEVICES,     /* max_devices */
                       };
  /*
   * routing table
   */
#define DIOIL_RT_SIZE                  15
static T_DIO_IL_ROUTING_TABLE dio_il_rt[DIOIL_RT_SIZE] =
/* dio_init_drv, t_dio_drv,                 dio_export_drv, dio_func */
{
  {NULL,         NULL,                      NULL,           NULL }, /* UART driver */
  {NULL,         NULL,                      NULL,           NULL }, /* USB driver */
  {NULL,         NULL,                      NULL,           NULL }, /* MUX driver */
  {NULL,         NULL,                      NULL,           NULL }, /* Packet driver */
  {NULL,         NULL,                      NULL,           NULL }, /* McBSP driver */
  {NULL,         NULL,                      NULL,           NULL }, /* Application adapter */
  {NULL,         NULL,                      NULL,           NULL },  /* BAT adapter */
  {NULL,         NULL,                      NULL,           NULL }, /* Serial CSMI driver */
  {NULL,         NULL,                      NULL,           NULL }, /* Serial ICX adapter */
  {NULL,         NULL,                      NULL,           NULL }, /* ICX packet multiplexer adapter */
  {NULL,         NULL,                      NULL,           NULL }, /* TCP/IP adapter */
  {NULL,         NULL,                      NULL,           NULL }, /* Socket adapter */
  {NULL,         NULL,                      NULL,           NULL }, /* Socket configuration adapter */
  {NULL,  	 NULL,  		    NULL,           NULL }, /* Bluetooth driver */
  {dio_init_rvt, (T_DIO_DRV*)&drv_init_rvt, dio_export_rvt, NULL }  /* RVT driver */
};

  /*
   * user info table
   */
#define DIOIL_UT_SIZE                  1
static T_DIO_IL_USER_INFO dio_il_ut[DIOIL_UT_SIZE] =
/* user_name, device_range_start,         device_range_end */
{
  {"PSI", DIO_DRV_RVT | DIO_TYPE_SER | 0, DIO_DRV_RVT | DIO_TYPE_SER | 0}
};
#endif /* (DIOIL_CONFIG == 11) */




#endif /* __DIOCONFIG_H__ */
