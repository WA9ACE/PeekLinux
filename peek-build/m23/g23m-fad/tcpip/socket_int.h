/* 
+------------------------------------------------------------------------------
|  File:       socket_int.h
+------------------------------------------------------------------------------
|  Copyright 2002 Texas Instruments Berlin, AG 
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
|  Purpose :      Socket specific definitions
+----------------------------------------------------------------------------- 
*/ 

#ifndef __SOCKET_INT_H__
#define __SOCKET_INT_H__


/*
 * Flow control enum type
 */
typedef enum {
  SOCK_FLOW_XON = 0,
  SOCK_FLOW_XOFF
} T_XONOFF_VAL;

/*
 * struct prototypes for internal data
 */
struct T_SOCK_API_INSTANCE_DATA_TAG;
struct T_SOCK_SOCKET_DATA_TAG;

/*
 * Socket API instance data structure
 */
typedef struct T_SOCK_API_INSTANCE_DATA_TAG
{
  T_HANDLE app_handle;     /* Application Handle */
  T_HANDLE hCommAPP;       /* Communivation Handle of the Application */
  T_HANDLE hCommTCPIP;     /* Communication Handle of the TCPIP entity */
  T_HANDLE hCommDCM;       /* Communication Handle of the DCM entity */
  void*         context;   /* application context pointer */
  T_SOCK_CALLBACK callback;  /* applicatin callback function */
  struct T_SOCK_SOCKET_DATA_TAG*  socket_data; /* pointer to linked list of Socket Data */
} T_SOCK_API_INSTANCE_DATA;


/*
 * Socket data structure
 */
typedef struct T_SOCK_SOCKET_DATA_TAG
{
  struct T_SOCK_SOCKET_DATA_TAG*        next_socket_data; /* pointer to next Socket Data structure */
  struct T_SOCK_API_INSTANCE_DATA_TAG*  api_data;         /* pointer to Instance Data structure */
  U32                                   tcpip_socket;     /* Socket value from TCPIP entity */
  T_SOCK_CALLBACK                       callback;         /* application callback function */
  void*                                 context;          /* application context pointer */
  T_XONOFF_VAL                          rx_flow_control;  /* status of RX flow control */
  T_XONOFF_VAL                          tx_flow_control;  /* status of TX flow control */
  U32                                   rx_window;        /* window size for data reception */
  U32                                   tx_window;        /* window size for data transmission */
  T_SOCK_RECV_IND                       recv_ind;         /* buffer for received data */
} T_SOCK_SOCKET_DATA;

#endif  /* __SOCKET_INT_H__ */

