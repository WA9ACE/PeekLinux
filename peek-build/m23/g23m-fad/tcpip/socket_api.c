/* 
+------------------------------------------------------------------------------
|  File:       socket.c
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
|  Purpose :  This file implements the socket API functionality.
|             For a description of this file read g23m\condat\doc\8462_601.doc
+----------------------------------------------------------------------------- 
*/ 

#ifndef SAP_DCM
#define SAP_DCM
#endif /* !SAP_DCM */

#define ENTITY_TCPIP

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "socket_api.h"

#include "custom.h"
#include "pei.h"   
#include "socket_int.h"
#include "dcm.h"
#include "aci_all.h"


LOCAL const int UNUSED = 0;

/* this function maps the DCM result codes to the Socket-API result codes */
LOCAL T_SOCK_RESULT map_dcm_result_code(S32 result)
{
  switch(result)
  {
    case DCM_OK:                return SOCK_RESULT_OK;
    case DCM_NOT_READY:         return SOCK_RESULT_NOT_READY;
    case DCM_ALREADY_ACTIVATED: return SOCK_RESULT_BEARER_ACTIVE;
    case DCM_UNKNOWN_EVENT:     return SOCK_RESULT_INTERNAL_ERROR;
    case DCM_INVALID_PARAMETER: return SOCK_RESULT_INVALID_PARAMETER;
    case DCM_BUSY:              return SOCK_RESULT_IN_PROGRESS;
    case DCM_PS_CONN_BROKEN:    return SOCK_RESULT_NETWORK_LOST;
    case DCM_NO_NETWORK:        return SOCK_RESULT_BEARER_NOT_READY;
    default:                    return SOCK_RESULT_INTERNAL_ERROR;
  }
}


/*******************************************************************************
** Function  :   FindSocketData
** Parameter :   T_SOCK_API_INSTANCE_DATA*
**               T_SOCK_SOCKET
** Description : The function returns a pointer to the data structure of a socket.
**               If the socket does not exist or api_data is NULL,
**               then it returns NULL.
*******************************************************************************/
LOCAL T_SOCK_SOCKET_DATA* FindSocketData(T_SOCK_API_INSTANCE_DATA* api_data,
                                         T_SOCK_SOCKET tcpip_socket)
{
  T_SOCK_SOCKET_DATA* socket_data;

  TRACE_FUNCTION("FindSocketData()");

  /* must be checked whether api_data is NULL or not.*/
  if(api_data != NULL)       
     socket_data = api_data->socket_data;
  else
  {
    TRACE_ERROR("FindSocketData: api_data = NULL!");
     return NULL;
  }
  
  while((socket_data != NULL) && (tcpip_socket != socket_data->tcpip_socket))
  {
    socket_data = socket_data->next_socket_data;
  }

  TRACE_EVENT_P2("FindSocketData: tcpip_data=%x, ret=%x",tcpip_socket,socket_data );
  return socket_data;
} /* FindSocketData */



/*******************************************************************************
** Function  : ReleaseSocketData
** Parameter : T_SOCK_API_INSTANCE_DATA*
**             T_SOCK_SOCKET_DATA*
** Description : The function releases the specified socket data.
*******************************************************************************/
static void ReleaseSocketData(T_SOCK_API_INSTANCE_DATA* api_data,
                              T_SOCK_SOCKET_DATA* socket_data)
{
  T_SOCK_SOCKET_DATA* temp_socket_data;

  TRACE_FUNCTION("ReleaseSocketData()");

  /* take socket data from linked list */
  if(api_data->socket_data == socket_data)
  {
    api_data->socket_data = socket_data->next_socket_data;
  }
  else
  {
    temp_socket_data = api_data->socket_data;
    while(temp_socket_data->next_socket_data != socket_data)
    {
      temp_socket_data = temp_socket_data->next_socket_data;
    }
    temp_socket_data->next_socket_data = socket_data->next_socket_data;
  }
  
  /* release socket data  */
  MFREE(socket_data);
} /* ReleaseSocketData */


/******************************************************************************/
BOOL sock_api_initialize(T_SOCK_API_INSTANCE *api_instance,
                         T_HANDLE app_handle,
                         char* app_name)
{
  T_SOCK_API_INSTANCE_DATA* api_data_p;
  T_HANDLE                  hCommAPP;
  T_HANDLE                  hCommTCPIP;
  T_HANDLE                  hCommDCM;

  TRACE_FUNCTION("[Socket API] sock_api_initialize()");

  /* open communication handles */
  if((hCommTCPIP = vsi_c_open (app_handle, TCPIP_NAME)) < VSI_OK)
  {
    TRACE_ERROR( "sock_api_initialize: vsi_c_open(TCP) failed");
    return FALSE;
  }

  if((hCommAPP = vsi_c_open (app_handle, app_name)) < VSI_OK)
  {
    TRACE_ERROR( "sock_api_initialize: vsi_c_open(APP) failed");
    return FALSE;
  }
  
  if((hCommDCM = vsi_c_open (app_handle, DCM_NAME)) < VSI_OK)
  {
    TRACE_ERROR( "sock_api_initialize: vsi_c_open(DCM) failed");
    return FALSE;
  }
  /* initialization successful */
  
  /* allocate API Data structure */
  MALLOC(api_data_p, sizeof(T_SOCK_API_INSTANCE_DATA));
  
  /*initialize API Data */
  api_data_p->app_handle  = app_handle;
  api_data_p->hCommAPP    = hCommAPP;
  api_data_p->hCommTCPIP  = hCommTCPIP;
  api_data_p->hCommDCM    = hCommDCM;
  api_data_p->socket_data = NULL;
 
  /* derefernce 'api_instance' and save address of 'api_data' */
  *api_instance = (T_SOCK_API_INSTANCE)api_data_p;
  return TRUE;
} /* sock_api_initialize */


/******************************************************************************/
void sock_api_deinitialize(T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_API_INSTANCE_DATA* api_data_p;
  T_SOCK_SOCKET_DATA*       socket_data1;
  T_SOCK_SOCKET_DATA*       socket_data2;

  TRACE_FUNCTION("[Socket API] sock_api_deinitialize()");

  /* get the address of 'api_data' */
  api_data_p = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  
  /*close communication channels */
  vsi_c_close (api_data_p->app_handle, api_data_p->hCommAPP);
  vsi_c_close (api_data_p->app_handle, api_data_p->hCommTCPIP);
  vsi_c_close (api_data_p->app_handle, api_data_p->hCommDCM);

  /* release all socket data */
  socket_data1 = api_data_p->socket_data;

  while(socket_data1 != NULL)
  {
    socket_data2 = socket_data1;
    socket_data1 = socket_data1->next_socket_data;
    MFREE(socket_data2);
  }

  /* release API data */
  MFREE(api_data_p);
} 


/******************************************************************************/
T_SOCK_RESULT sock_open_bearer(T_SOCK_API_INSTANCE api_instance,
                               T_SOCK_BEARER_TYPE bearer_select,
                               int profile_number,
                               T_SOCK_BEARER_INFO *params,
                               T_SOCK_CALLBACK sock_cb,
                               void *context)
{
  T_SOCK_API_INSTANCE_DATA* api_data_p;

  TRACE_FUNCTION("[Socket API] sock_open_bearer()");
  
if(sock_cb EQ NULL)
  return SOCK_RESULT_INVALID_PARAMETER;

  /* set API data pointer */
  api_data_p = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  api_data_p->context = context;
  api_data_p->callback = sock_cb;
  
  /* send primitive to DCM */
  {
    PALLOC(dcm_open_conn_req, DCM_OPEN_CONN_REQ);
    dcm_open_conn_req->api_instance = api_instance;
    dcm_open_conn_req->bearer_select = bearer_select;
    dcm_open_conn_req->profile_number = profile_number;

    dcm_open_conn_req->dcm_info_conn.bearer_handle = UNUSED;
    dcm_open_conn_req->dcm_info_conn.app_handle = params->app_handle;
    dcm_open_conn_req->dcm_info_conn.bearer_type = params->bearer_type;
    dcm_open_conn_req->dcm_info_conn.apn_valid = params->apn_valid;
    memcpy(dcm_open_conn_req->dcm_info_conn.apn, params->apn, SOCK_MAX_APN_LEN+1);
    dcm_open_conn_req->dcm_info_conn.phone_number_valid = params->phone_nr_valid;
    memcpy(dcm_open_conn_req->dcm_info_conn.phone_number, params->phone_nr,
           SOCK_MAX_PHONENUM_LEN+1);
    dcm_open_conn_req->dcm_info_conn.user_id_valid = params->user_id_valid;
    memcpy(dcm_open_conn_req->dcm_info_conn.user_id, params->user_id,
           SOCK_MAX_USERID_LEN+1);
    dcm_open_conn_req->dcm_info_conn.password_valid = params->password_valid;
    memcpy(dcm_open_conn_req->dcm_info_conn.password, params->password,
           SOCK_MAX_PASSWORD_LEN+1);
    dcm_open_conn_req->dcm_info_conn.cid = params->cid;
    dcm_open_conn_req->dcm_info_conn.ip_address = params->ip_address;
    dcm_open_conn_req->dcm_info_conn.dns1 = params->dns1;
    dcm_open_conn_req->dcm_info_conn.dns2 = params->dns2;
    dcm_open_conn_req->dcm_info_conn.gateway = params->gateway;
    dcm_open_conn_req->dcm_info_conn.auth_type = params->authtype;
    dcm_open_conn_req->dcm_info_conn.data_compr = params->data_compr;
    dcm_open_conn_req->dcm_info_conn.header_compr = params->header_comp;
    dcm_open_conn_req->dcm_info_conn.precedence = params->precedence;
    dcm_open_conn_req->dcm_info_conn.delay = params->precedence;
    dcm_open_conn_req->dcm_info_conn.reliability = params->reliability;
    dcm_open_conn_req->dcm_info_conn.peak_throughput = params->peak_throughput;
    dcm_open_conn_req->dcm_info_conn.mean_throughput = params->mean_througput;
    dcm_open_conn_req->dcm_info_conn.shareable = params->shareable;

    PSEND(api_data_p->hCommDCM, dcm_open_conn_req);
  }
  return SOCK_RESULT_OK;
}

/******************************************************************************/
LOCAL void sock_open_bearer_cnf(T_DCM_OPEN_CONN_CNF *dcm_open_conn_cnf,
                                      T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_OPEN_BEARER_CNF sock_open_bearer_cnf;
  T_SOCK_API_INSTANCE_DATA *api_data_p;

  TRACE_FUNCTION("[Socket API] sock_open_bearer_cnf()");

  api_data_p = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  
  /* fill signal struct */
  sock_open_bearer_cnf.result = map_dcm_result_code(dcm_open_conn_cnf->result);
  sock_open_bearer_cnf.socket = UNUSED;
  sock_open_bearer_cnf.event_type = SOCK_OPEN_BEARER_CNF;
  sock_open_bearer_cnf.bearer_handle = dcm_open_conn_cnf->bearer_handle;

  /* release primitive */
  PFREE(dcm_open_conn_cnf);
  
  api_data_p->callback((T_SOCK_EVENTSTRUCT *)&sock_open_bearer_cnf,
                       api_data_p->context);
}

/******************************************************************************/
T_SOCK_RESULT sock_close_bearer(T_SOCK_API_INSTANCE api_instance,
                                T_SOCK_BEARER_HANDLE bearer_handle,
                                T_SOCK_CALLBACK sock_cb,
                                void *context)
{
  T_SOCK_API_INSTANCE_DATA* api_data_p;

  TRACE_FUNCTION("[Socket API] sock_close_bearer()");

  if(sock_cb EQ NULL) {
   return SOCK_RESULT_INVALID_PARAMETER;
  }

  /* set API data pointer */
  api_data_p = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  api_data_p->callback = sock_cb;
  api_data_p->context = context;

  /* send primitive to DCM */
  {
    PALLOC(dcm_close_conn_req, DCM_CLOSE_CONN_REQ);
    dcm_close_conn_req->api_instance = api_instance;
    dcm_close_conn_req->bearer_handle = bearer_handle;
    PSEND(api_data_p->hCommDCM, dcm_close_conn_req);
  }
  
  return SOCK_RESULT_OK;
}

/******************************************************************************/
LOCAL void sock_close_bearer_cnf(T_DCM_CLOSE_CONN_CNF *dcm_close_conn_cnf,
                                        T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_CLOSE_BEARER_CNF sock_close_bearer_cnf;
  T_SOCK_API_INSTANCE_DATA *api_data_p;

  TRACE_FUNCTION("[Socket API] dcm_close_conn_cnf()");

  api_data_p = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  sock_close_bearer_cnf.result = map_dcm_result_code(dcm_close_conn_cnf->result);
  sock_close_bearer_cnf.socket = UNUSED;
  sock_close_bearer_cnf.event_type = SOCK_CLOSE_BEARER_CNF;

  /* release primitive */
  PFREE(dcm_close_conn_cnf);

  /* call callback function */
  api_data_p->callback((T_SOCK_EVENTSTRUCT *)&sock_close_bearer_cnf,
                       api_data_p->context);
}

/******************************************************************************/
T_SOCK_RESULT sock_bearer_info(T_SOCK_API_INSTANCE api_instance,
                               T_SOCK_BEARER_HANDLE bearer_handle,
                               T_SOCK_CALLBACK sock_cb,
                               void *context)
{
  T_SOCK_API_INSTANCE_DATA* api_data_p;

  TRACE_FUNCTION("[Socket API] sock_bearer_info()");

  if(sock_cb EQ NULL)
   return SOCK_RESULT_INVALID_PARAMETER;

  /* set API data pointer */
  api_data_p = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  api_data_p->callback = sock_cb;
  api_data_p->context = context;

  /* send primitive to DCM */
  {
    PALLOC(dcm_get_current_conn_req, DCM_GET_CURRENT_CONN_REQ);
    dcm_get_current_conn_req->api_instance = api_instance;
    dcm_get_current_conn_req->bearer_handle = bearer_handle;
    PSEND(api_data_p->hCommDCM, dcm_get_current_conn_req);
  }
  return SOCK_RESULT_OK;
}


/******************************************************************************/
LOCAL void sock_bearer_info_cnf(T_DCM_GET_CURRENT_CONN_CNF *current_conn_cnf,
                                      T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_BEARER_INFO_CNF bearer_info;
  T_SOCK_API_INSTANCE_DATA *api_data_p;

  TRACE_FUNCTION("[Socket API] sock_bearer_info_cnf()");

  api_data_p = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  bearer_info.result = map_dcm_result_code(current_conn_cnf->result);
  bearer_info.socket = UNUSED;
  bearer_info.event_type = SOCK_BEARER_INFO_CNF;

  bearer_info.bearer_params.bearer_handle = current_conn_cnf->dcm_info_conn.bearer_handle;
  bearer_info.bearer_params.app_handle = current_conn_cnf->dcm_info_conn.app_handle;
  bearer_info.bearer_params.bearer_type =(T_SOCK_BEARER_TYPE)current_conn_cnf->dcm_info_conn.bearer_type;
  bearer_info.bearer_params.apn_valid = current_conn_cnf->dcm_info_conn.apn_valid;
  memcpy(bearer_info.bearer_params.apn,
         current_conn_cnf->dcm_info_conn.apn, SOCK_MAX_APN_LEN);
  bearer_info.bearer_params.phone_nr_valid = current_conn_cnf->dcm_info_conn.phone_number_valid;
  memcpy(bearer_info.bearer_params.phone_nr,current_conn_cnf->dcm_info_conn.phone_number,
         SOCK_MAX_PHONENUM_LEN);
  bearer_info.bearer_params.user_id_valid = current_conn_cnf->dcm_info_conn.user_id_valid;
  memcpy(bearer_info.bearer_params.user_id, current_conn_cnf->dcm_info_conn.user_id,
         SOCK_MAX_USERID_LEN);
  bearer_info.bearer_params.password_valid = current_conn_cnf->dcm_info_conn.password_valid;
  memcpy(bearer_info.bearer_params.password, current_conn_cnf->dcm_info_conn.password,
         SOCK_MAX_PASSWORD_LEN);
  bearer_info.bearer_params.cid = current_conn_cnf->dcm_info_conn.cid;
  bearer_info.bearer_params.ip_address = current_conn_cnf->dcm_info_conn.ip_address;
  bearer_info.bearer_params.dns1 = current_conn_cnf->dcm_info_conn.dns1;
  bearer_info.bearer_params.dns2 = current_conn_cnf->dcm_info_conn.dns2;
  bearer_info.bearer_params.gateway = current_conn_cnf->dcm_info_conn.gateway;
  bearer_info.bearer_params.authtype = (T_SOCK_AUTHTYPE)current_conn_cnf->dcm_info_conn.auth_type;
  bearer_info.bearer_params.data_compr = current_conn_cnf->dcm_info_conn.data_compr;
  bearer_info.bearer_params.header_comp = current_conn_cnf->dcm_info_conn.header_compr;
  bearer_info.bearer_params.precedence = current_conn_cnf->dcm_info_conn.precedence;
  bearer_info.bearer_params.delay = current_conn_cnf->dcm_info_conn.delay;
  bearer_info.bearer_params.reliability = current_conn_cnf->dcm_info_conn.reliability;
  bearer_info.bearer_params.peak_throughput = current_conn_cnf->dcm_info_conn.peak_throughput;
  bearer_info.bearer_params.mean_througput = current_conn_cnf->dcm_info_conn.mean_throughput;
  bearer_info.bearer_params.shareable = current_conn_cnf->dcm_info_conn.shareable;

  /* release primitive */
  PFREE(current_conn_cnf);

  /* call callback function */
  api_data_p->callback((T_SOCK_EVENTSTRUCT *)&bearer_info, api_data_p->context);
}


/******************************************************************************/
LOCAL void sock_dcm_error_ind(T_DCM_ERROR_IND *dcm_error_ind,
                                    T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_BAERER_CLOSED_IND sock_bearer_closed_ind;
  T_SOCK_API_INSTANCE_DATA *api_data_p;

  TRACE_FUNCTION("[Socket API] sock_dcm_error_ind()");

  api_data_p = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  sock_bearer_closed_ind.event_type = SOCK_BAERER_CLOSED_IND;
  sock_bearer_closed_ind.result = map_dcm_result_code(dcm_error_ind->result);
  sock_bearer_closed_ind.socket = UNUSED;
  sock_bearer_closed_ind.dcm_error = dcm_error_ind->dcm_err;

  PFREE(dcm_error_ind);
  
  if(api_data_p->callback NEQ NULL)
  {
    api_data_p->callback((T_SOCK_EVENTSTRUCT *)&sock_bearer_closed_ind,
                         api_data_p->context);
  }
}


/* ******************* Socket related functions ***************************** */

/******************************************************************************/
T_SOCK_RESULT sock_create(T_SOCK_API_INSTANCE api_instance,
                          T_SOCK_IPPROTO ipproto,
                          T_SOCK_CALLBACK callback,
                          void* context)
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA*       socket_data;

  TRACE_FUNCTION("[Socket API] sock_create()");
  TRACE_EVENT_P1("IP-Protocol=%d",(U32)ipproto);
  
  /* set api_data  */
  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  
 if(callback EQ NULL)
  return SOCK_RESULT_INVALID_PARAMETER;

 /* check for correct data */
  if((ipproto != SOCK_IPPROTO_UDP) && (ipproto != SOCK_IPPROTO_TCP))
  {
    TRACE_ERROR("[Socket API] Error: Invalid IP-Protocol");
    return SOCK_RESULT_INVALID_PARAMETER;
  } 
  if( api_data == NULL )
  {
    TRACE_ERROR("[Socket API] Error: api_instance == NULL!");
    return SOCK_RESULT_INVALID_PARAMETER;
  }

  /* allocate socket data */
  MALLOC(socket_data, sizeof(T_SOCK_SOCKET_DATA));
  
  /* put socket data in linked list */
  socket_data->next_socket_data = api_data->socket_data;
  api_data->socket_data         = socket_data;
  
  /* initialize socket data */
  socket_data->api_data        = api_data;
  socket_data->tcpip_socket    = 0;
  socket_data->callback        = callback;
  socket_data->context         = context;
  socket_data->rx_flow_control = SOCK_FLOW_XON;
  socket_data->tx_flow_control = SOCK_FLOW_XON;
  socket_data->rx_window       = 1;
  socket_data->tx_window       = 1;
  
  /* send primitive to TCPIP */
  {
    PALLOC(tcpip_create_req_prim, TCPIP_CREATE_REQ);

    tcpip_create_req_prim->app_handle = api_data->hCommAPP;
    tcpip_create_req_prim->ipproto    = ipproto;
    tcpip_create_req_prim->request_id = (T_SOCK_SOCKET)socket_data;

    PSEND(api_data->hCommTCPIP, tcpip_create_req_prim);
  }

  return SOCK_RESULT_OK;
}/* sock_create */


/*****************************************************************************************
** Function  : sock_create_cnf
** Parameter : T_TCPIP_CREATE_CNF *
**             T_SOCK_API_INSTANCE ' this value will be ignored in this function
**             because you can find socket_data only using request_id
*****************************************************************************************/
LOCAL void sock_create_cnf(T_TCPIP_CREATE_CNF  *tcpip_create_cnf,
                                  T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_SOCKET_DATA* socket_data;
  T_SOCK_CREATE_CNF   sock_create_cnf;

  TRACE_FUNCTION("[Socket API] sock_create_cnf()");

  /* set socket data */
  socket_data = (T_SOCK_SOCKET_DATA*)tcpip_create_cnf->request_id;
  
  /* set received values */
  socket_data->tcpip_socket = tcpip_create_cnf->socket;
  
  /* fill confirm struct */
  sock_create_cnf.event_type = (T_SOCK_EVENTTYPE)tcpip_create_cnf->event_type;
  sock_create_cnf.result     = (T_SOCK_RESULT)tcpip_create_cnf->result;
  sock_create_cnf.socket     = (T_SOCK_SOCKET)socket_data;
  
  socket_data->callback((T_SOCK_EVENTSTRUCT *)&sock_create_cnf, socket_data->context);

  /* release socket data if context creation was not successful */
  if(tcpip_create_cnf->result != TCPIP_RESULT_OK)
  {
    TRACE_ERROR("[Socket API] Error: sock_create() failed");
    ReleaseSocketData(socket_data->api_data, socket_data);
  }
  
  PFREE(tcpip_create_cnf);
}/* sock_create_cnf */


/******************************************************************************/
T_SOCK_RESULT sock_close(T_SOCK_SOCKET socket)
{
  T_SOCK_API_INSTANCE_DATA*  api_data;
  T_SOCK_SOCKET_DATA* sock_data;

  TRACE_FUNCTION("[Socket API] sock_close()");

  sock_data = (T_SOCK_SOCKET_DATA*)socket;
  if(sock_data == NULL)
  {
    return SOCK_RESULT_INVALID_PARAMETER;
  }

  api_data = sock_data->api_data;
   
  /* send primitive to TCPIP */
  {
    PALLOC(tcpip_close_req_prim, TCPIP_CLOSE_REQ);
    tcpip_close_req_prim->app_handle = api_data->hCommAPP;
    tcpip_close_req_prim->socket = sock_data->tcpip_socket;
    PSEND(api_data->hCommTCPIP, tcpip_close_req_prim);
  }
  return SOCK_RESULT_OK;
}/* sock_close */


/*******************************************************************************
** Function  : sock_close_cnf
** Parameter : T_SOCK_CLOSE_CNF *
*******************************************************************************/
LOCAL void sock_close_cnf(T_TCPIP_CLOSE_CNF *tcpip_close_cnf,
                                 T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_SOCKET_DATA* sock_data;
  T_SOCK_CLOSE_CNF   sock_close_cnf;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("[Socket API] sock_close_cnf()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  sock_data =  FindSocketData(api_data ,tcpip_close_cnf->socket);
  if(sock_data == NULL)
  {
    PFREE(tcpip_close_cnf);
    return;
  }

  /* fill confirm struct */
  sock_close_cnf.event_type  = (T_SOCK_EVENTTYPE)tcpip_close_cnf->event_type;
  sock_close_cnf.result      = (T_SOCK_RESULT)tcpip_close_cnf->result;
  sock_close_cnf.socket      = (T_SOCK_SOCKET)sock_data;
  
  sock_data->callback(&sock_close_cnf, sock_data->context);

  /* release socket data if sock closing was successful */
  if(tcpip_close_cnf->result == TCPIP_RESULT_OK)
  {
    ReleaseSocketData(sock_data->api_data, sock_data);
  }
  
  PFREE(tcpip_close_cnf);
}/* sock_close_cnf */


/******************************************************************************/
T_SOCK_RESULT sock_bind(T_SOCK_SOCKET socket, T_SOCK_PORT  port)
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA *sock_data;

  TRACE_FUNCTION("[Socket API] sock_bind()");

  sock_data = (T_SOCK_SOCKET_DATA*)socket;
  if(sock_data == NULL)
  {
    return SOCK_RESULT_INVALID_PARAMETER;
  }

  api_data = sock_data->api_data;
  {
    PALLOC (tcpip_bind_req_prim, TCPIP_BIND_REQ);
    tcpip_bind_req_prim->app_handle = api_data->hCommAPP;
    tcpip_bind_req_prim->socket     = sock_data->tcpip_socket;
    tcpip_bind_req_prim->port       = port;
    PSEND(api_data->hCommTCPIP, tcpip_bind_req_prim);
  }
  return SOCK_RESULT_OK;
}/* sock_bind */


/*******************************************************************************
** Function  : sock_bind_cnf
** Parameter : T_SOCK_BIND_CNF *
**                   T_SOCK_API_INSTANCE
*******************************************************************************/
LOCAL void sock_bind_cnf(T_TCPIP_BIND_CNF  *tcpip_bind_cnf,
                                T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_SOCKET_DATA *sock_data;
  T_SOCK_BIND_CNF sock_bind_cnf;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("[Socket API] sock_bind_cnf()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  sock_data =  FindSocketData(api_data ,tcpip_bind_cnf->socket);
  if(sock_data == NULL)
  {
    PFREE(tcpip_bind_cnf);
    return;
  }

  /* fill confirm struct */
  sock_bind_cnf.event_type = (T_SOCK_EVENTTYPE)tcpip_bind_cnf->event_type;
  sock_bind_cnf.result     = (T_SOCK_RESULT)tcpip_bind_cnf->result; 
  sock_bind_cnf.socket     = (T_SOCK_SOCKET)sock_data;

  sock_data->callback(&sock_bind_cnf,sock_data->context);

  PFREE(tcpip_bind_cnf);
}/* sock_bind_cnf */


/******************************************************************************/
T_SOCK_RESULT sock_listen(T_SOCK_SOCKET socket )
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA* sock_data;
    
  TRACE_FUNCTION("[Socket API] sock_listen()");
    
  sock_data = (T_SOCK_SOCKET_DATA*)socket;
  if(sock_data == NULL)
  {
    TRACE_ERROR("[Socket API] listen() error: Invalid socket data");
    return SOCK_RESULT_INVALID_PARAMETER;
  }

  api_data   = sock_data->api_data;
  {
    PALLOC (tcpip_listen_req_prim, TCPIP_LISTEN_REQ);
    tcpip_listen_req_prim->app_handle = api_data->hCommAPP;
    tcpip_listen_req_prim->socket = sock_data->tcpip_socket;
    PSEND(api_data->hCommTCPIP,tcpip_listen_req_prim );
  }
	return SOCK_RESULT_OK;
}/* sock_listen */


/*******************************************************************************
** Function  : sock_listen_cnf
** Parameter : T_SOCK_LISTEN_CNF *
**             T_SOCK_API_INSTANCE
*******************************************************************************/
LOCAL void sock_listen_cnf(T_TCPIP_LISTEN_CNF *tcpip_listen_cnf,
                                 T_SOCK_API_INSTANCE api_instance )
{
  T_SOCK_SOCKET_DATA *sock_data;
  T_SOCK_LISTEN_CNF sock_listen_cnf;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("[Socket API] sock_listen_cnf()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  sock_data = FindSocketData(api_data ,tcpip_listen_cnf->socket);
  if(sock_data == NULL)
  {
    PFREE(tcpip_listen_cnf);
    return;
  }

  /* fill confirm struct */
  sock_listen_cnf.event_type = (T_SOCK_EVENTTYPE)tcpip_listen_cnf->event_type;
  sock_listen_cnf.result     = (T_SOCK_RESULT)tcpip_listen_cnf->result; 
  sock_listen_cnf.socket     = (T_SOCK_SOCKET)sock_data;

  sock_data->callback(&sock_listen_cnf,sock_data->context);

  PFREE(tcpip_listen_cnf);
} /* sock_listen_cnf */


/******************************************************************************/
T_SOCK_RESULT sock_connect(T_SOCK_SOCKET socket,
                           T_SOCK_IPADDR ipaddr,
                           T_SOCK_PORT port)
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA* sock_data;

  TRACE_FUNCTION("sock_connect()");

  sock_data = (T_SOCK_SOCKET_DATA*)socket;
  if(sock_data == NULL)
  {
      TRACE_ERROR("[Socket API] connect error: Invalid socket data");
      return SOCK_RESULT_INVALID_PARAMETER;
  }

  api_data = sock_data->api_data;
  {
    PALLOC (tcpip_connect_req_prim, TCPIP_CONNECT_REQ);
    tcpip_connect_req_prim->app_handle = api_data->hCommAPP;
    tcpip_connect_req_prim->socket = sock_data->tcpip_socket;
    tcpip_connect_req_prim->ipaddr = ipaddr;
    tcpip_connect_req_prim->port   = port;
    PSEND(api_data->hCommTCPIP, tcpip_connect_req_prim);
  }
  return SOCK_RESULT_OK;
} /* sock_connect */


/*******************************************************************************
** Function  : sock_connect_cnf
** Parameter : T_SOCK_CONNECT_CNF *
**             T_SOCK_API_INSTANCE
*******************************************************************************/
LOCAL void sock_connect_cnf(T_TCPIP_CONNECT_CNF *tcpip_connect_cnf,
                                   T_SOCK_API_INSTANCE api_instance )
{
  T_SOCK_SOCKET_DATA *sock_data;
  T_SOCK_CONNECT_CNF sock_connect_cnf;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("sock_connect_cnf()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  TRACE_EVENT_P3("api_data %d, socket %d, reslut %d",api_data,
                 tcpip_connect_cnf->socket,tcpip_connect_cnf->result);

  sock_data =  FindSocketData(api_data ,tcpip_connect_cnf->socket);
  if(sock_data == NULL)
  {
    TRACE_EVENT("sock data NULL !!!");
    PFREE(tcpip_connect_cnf);
    return;
  }

  /* fill confirm struct */
  sock_connect_cnf.event_type = (T_SOCK_EVENTTYPE)tcpip_connect_cnf->event_type;
  sock_connect_cnf.result     = (T_SOCK_RESULT)tcpip_connect_cnf->result; 
  sock_connect_cnf.socket     = (T_SOCK_SOCKET)sock_data;

  sock_data->callback(&sock_connect_cnf,sock_data->context);

  PFREE(tcpip_connect_cnf);
} /* sock_connect_cnf */


/******************************************************************************/
T_SOCK_RESULT sock_getsockname(T_SOCK_SOCKET socket)
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA* sock_data;

  TRACE_FUNCTION("sock_getsockname()");

  sock_data = (T_SOCK_SOCKET_DATA*)socket;
  if(sock_data == NULL)
  {
    TRACE_ERROR("[Socket API] getsockname() error: Invalid socket data");
    return SOCK_RESULT_INVALID_PARAMETER;
  }
    api_data   = sock_data->api_data;

  {
    PALLOC(tcpip_sockname_req_prim, TCPIP_SOCKNAME_REQ);
    tcpip_sockname_req_prim->app_handle = api_data->hCommAPP;
    tcpip_sockname_req_prim->socket = sock_data->tcpip_socket;
    PSEND(api_data->hCommTCPIP, tcpip_sockname_req_prim);
  }
  return SOCK_RESULT_OK;
} /* sock_getsockname */



/*******************************************************************************
** Function  : sock_connect_cnf
** Parameter : T_SOCK_SOCKNAME_CNF *
**             T_SOCK_API_INSTANCE
*******************************************************************************/
LOCAL void sock_sockname_cnf(T_TCPIP_SOCKNAME_CNF *tcpip_sockname_cnf,
                                    T_SOCK_API_INSTANCE api_instance )
{
  T_SOCK_SOCKET_DATA *sock_data;
  T_SOCK_SOCKNAME_CNF sock_sockname_cnf;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("sock_getsockname_cnf()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  sock_data =  FindSocketData(api_data , tcpip_sockname_cnf->socket);
  if(sock_data == NULL)
  {
    PFREE(tcpip_sockname_cnf);
    return;
  }

  /* fill confirm struct */
  sock_sockname_cnf.event_type = (T_SOCK_EVENTTYPE)tcpip_sockname_cnf->event_type;
  sock_sockname_cnf.result     = (T_SOCK_RESULT)tcpip_sockname_cnf->result; 
  sock_sockname_cnf.socket     = (T_SOCK_SOCKET)sock_data;
  sock_sockname_cnf.ipaddr     = tcpip_sockname_cnf->ipaddr;
  sock_sockname_cnf.port       = tcpip_sockname_cnf->port;

  sock_data->callback((T_SOCK_EVENTSTRUCT*)&sock_sockname_cnf,sock_data->context);

  PFREE(tcpip_sockname_cnf);
} /* sock_sockname_cnf */


/******************************************************************************/
T_SOCK_RESULT sock_getpeername(T_SOCK_SOCKET socket)
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA* sock_data;

  TRACE_FUNCTION("sock_getpeername()");

  sock_data = (T_SOCK_SOCKET_DATA*)socket;
  api_data   = sock_data->api_data;
  
  if((sock_data == NULL) OR (api_data == NULL))
  {
    TRACE_ERROR("[Socket API] getpeername() error: Invalid socket data");
    return SOCK_RESULT_INVALID_PARAMETER;
  }

  {
    PALLOC (tcpip_peername_req_prim, TCPIP_PEERNAME_REQ);
    tcpip_peername_req_prim->app_handle = api_data->hCommAPP;
    tcpip_peername_req_prim->socket = sock_data->tcpip_socket;
    PSEND(api_data->hCommTCPIP, tcpip_peername_req_prim);
  }
  return SOCK_RESULT_OK;
} /* sock_getpeername */


/*******************************************************************************
** Function  : sock_peername_cnf
** Parameter : T_SOCK_PEERNAME_CNF *
**                   T_SOCK_API_INSTANCE
*******************************************************************************/
LOCAL void sock_peername_cnf(T_TCPIP_PEERNAME_CNF *tcpip_peername_cnf,
                                   T_SOCK_API_INSTANCE api_instance )
{
  T_SOCK_SOCKET_DATA *sock_data;
  T_SOCK_PEERNAME_CNF sock_peername_cnf;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("sock_getpeername_cnf()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  sock_data =  FindSocketData(api_data ,tcpip_peername_cnf->socket);
  if(sock_data == NULL)
  {
    PFREE(tcpip_peername_cnf);
    return;
  }

  sock_peername_cnf.event_type = (T_SOCK_EVENTTYPE)tcpip_peername_cnf->event_type;
  sock_peername_cnf.result     = (T_SOCK_RESULT)tcpip_peername_cnf->result; 
  sock_peername_cnf.socket     = (T_SOCK_SOCKET)sock_data;
  sock_peername_cnf.ipaddr     = tcpip_peername_cnf->ipaddr;
  sock_peername_cnf.port       = tcpip_peername_cnf->port;
   
  sock_data->callback((T_SOCK_EVENTSTRUCT*)&sock_peername_cnf,sock_data->context);

  PFREE(tcpip_peername_cnf);
} /* sock_peername_cnf */


/******************************************************************************/
T_SOCK_RESULT sock_gethostbyname(T_SOCK_API_INSTANCE api_instance,
                                 char* hostname, 
                                 T_SOCK_CALLBACK callback,
                                 void* context)
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA*       socket_data;

  TRACE_FUNCTION("sock_gethostbyname()");
  TRACE_EVENT_P1("hostname: %s",hostname);

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  if(callback EQ NULL)
   return SOCK_RESULT_INVALID_PARAMETER;

  if(api_data == NULL)
  {
    TRACE_ERROR("[Socket API] gethostbyname() error: Invalid socket data");
    return SOCK_RESULT_INVALID_PARAMETER;
  }
  
  MALLOC(socket_data, sizeof(T_SOCK_SOCKET_DATA));
  
  /* put socket data in linked list */
  socket_data->next_socket_data = api_data->socket_data;
  api_data->socket_data         = socket_data;
  
  /* initialize socket data */
  socket_data->api_data        = api_data;
  socket_data->tcpip_socket    = 0;
  socket_data->callback        = callback;
  socket_data->context         = context;
  socket_data->rx_flow_control = SOCK_FLOW_XON; 
  socket_data->tx_flow_control = SOCK_FLOW_XON;
  socket_data->rx_window       = 1;
  socket_data->tx_window       = 1;

  {
    PALLOC (tcpip_hostinfo_req_prim , TCPIP_HOSTINFO_REQ);
    tcpip_hostinfo_req_prim->app_handle = api_data->hCommAPP;
    tcpip_hostinfo_req_prim->ipaddr = 0;
    strcpy((char*)tcpip_hostinfo_req_prim->hostname, hostname);
    tcpip_hostinfo_req_prim->request_id = (T_SOCK_SOCKET)socket_data;     
    PSEND(api_data->hCommTCPIP,tcpip_hostinfo_req_prim);
  }
  return SOCK_RESULT_OK;
} /* sock_gethostbyname */


/*******************************************************************************
** Function  : sock_hostinfo_cnf
** Parameter : T_SOCK_HOSTINFO_CNF *
**             T_SOCK_API_INSTANCE    this value will be ignored ...
**                                    because you can find sock_data using request_id.
*******************************************************************************/
LOCAL void sock_hostinfo_cnf(T_TCPIP_HOSTINFO_CNF* tcpip_hostinfo_cnf , 
                                       T_SOCK_API_INSTANCE api_instance )
{
  T_SOCK_SOCKET_DATA* socket_data;
  T_SOCK_HOSTINFO_CNF  sock_hostinfo_cnf;

  socket_data = (T_SOCK_SOCKET_DATA*)tcpip_hostinfo_cnf->request_id;
  
  /* set received values */
  socket_data->tcpip_socket = tcpip_hostinfo_cnf->socket;
  
  /* fill confirm struct */
  sock_hostinfo_cnf.event_type = (T_SOCK_EVENTTYPE)tcpip_hostinfo_cnf->event_type;
  sock_hostinfo_cnf.result     = (T_SOCK_RESULT)tcpip_hostinfo_cnf->result;
  sock_hostinfo_cnf.socket     = (T_SOCK_SOCKET)socket_data;
  sock_hostinfo_cnf.ipaddr     =  tcpip_hostinfo_cnf->ipaddr;
  strcpy(sock_hostinfo_cnf.hostname,(char*)tcpip_hostinfo_cnf->hostname);

  socket_data->callback((T_SOCK_EVENTSTRUCT*)&sock_hostinfo_cnf, socket_data->context);

  ReleaseSocketData(socket_data->api_data, socket_data);
  
  PFREE(tcpip_hostinfo_cnf);
} /* sock_hostinfo_cnf */


/******************************************************************************/
T_SOCK_RESULT sock_send(T_SOCK_SOCKET socket, char* buffer, U16 buffer_length)
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA* sock_data;

  TRACE_FUNCTION("[Socket API] sock_send()");

  sock_data = (T_SOCK_SOCKET_DATA*)socket;
  if(sock_data == NULL)
  {
    return SOCK_RESULT_INVALID_PARAMETER;
  }

  api_data   = sock_data->api_data;

  if(sock_data->tx_window != 0)
  {
    PALLOC(tcpip_data_req_prim,TCPIP_DATA_REQ);
  	tcpip_data_req_prim->app_handle = api_data->hCommAPP;
  	tcpip_data_req_prim->socket     = sock_data->tcpip_socket;
  	tcpip_data_req_prim->ipaddr     = 0;
  	tcpip_data_req_prim->port       = 0;
  	tcpip_data_req_prim->buflen     =  buffer_length;
  	tcpip_data_req_prim->data       = (U32) M_ALLOC(buffer_length);

  	if(tcpip_data_req_prim->data)
  	{
      /* copy the user data buffer, 
         the prim-data is freed by tcpip_clear_send_buffer() */
      memcpy((char *)tcpip_data_req_prim->data,buffer,buffer_length);
    }
  	else
  	{
      PFREE(tcpip_data_req_prim);
      return SOCK_RESULT_OUT_OF_MEMORY;
    }
    sock_data->tx_window--;
  	PSEND(api_data->hCommTCPIP,tcpip_data_req_prim); 
  	return SOCK_RESULT_OK;
  }
  else /* tx_window = 0 */
  {
    sock_data->tx_flow_control = SOCK_FLOW_XOFF;
    return SOCK_RESULT_NO_BUFSPACE;
  }
} /* sock_send */


/******************************************************************************/
T_SOCK_RESULT sock_sendto(T_SOCK_SOCKET socket, char* buffer,U16 buffer_length,
                          T_SOCK_IPADDR ipaddr,T_SOCK_PORT port)
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA* sock_data;

  TRACE_FUNCTION("[Socket API] sock_sendto()");

  sock_data = (T_SOCK_SOCKET_DATA*)socket;
  if(sock_data == NULL)
      return SOCK_RESULT_INVALID_PARAMETER;
  api_data   = sock_data->api_data;

  if(sock_data->tx_window != 0)
  {
    PALLOC(tcpip_data_req_prim,TCPIP_DATA_REQ);
  	tcpip_data_req_prim->app_handle = api_data->hCommAPP;
  	tcpip_data_req_prim->socket        = sock_data->tcpip_socket;
  	tcpip_data_req_prim->ipaddr        = ipaddr;
  	tcpip_data_req_prim->port          = port;
  	tcpip_data_req_prim->buflen        =  buffer_length;
	  tcpip_data_req_prim->data=(U32) M_ALLOC(buffer_length);
  	if(tcpip_data_req_prim->data )
  	{
      /* copy the user data buffer, 
         the prim-data is freed by tcpip_clear_send_buffer() */
      memcpy((char *)tcpip_data_req_prim->data,buffer,buffer_length);
  	}
  	else
  	{
  	  PFREE(tcpip_data_req_prim);
  	  return SOCK_RESULT_OUT_OF_MEMORY;
  	}

 	  sock_data->tx_window--;
 	  PSEND(api_data->hCommTCPIP, tcpip_data_req_prim);
    return SOCK_RESULT_OK;
  }
  else /* tx_window = 0 */
  {
    sock_data->tx_flow_control = SOCK_FLOW_XOFF;
    return SOCK_RESULT_NO_BUFSPACE;
  }
}

  
/*******************************************************************************
** Function  : sock_mtu_size_cnf
** Parameter : T_SOCK_MTU_SIZE_CNF *
**             T_SOCK_API_INSTANCE
*******************************************************************************/
LOCAL void sock_mtu_size_cnf(T_TCPIP_MTU_SIZE_CNF *tcpip_mtu_size_cnf ,
                                   T_SOCK_API_INSTANCE api_instance )
{
  T_SOCK_SOCKET_DATA *sock_data;
  T_SOCK_MTU_SIZE_CNF sock_mtu_size_cnf;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("[Socket API] sock_mtu_size_cnf()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  sock_data =  FindSocketData(api_data ,tcpip_mtu_size_cnf->socket);
  if(sock_data == NULL)
  {
    PFREE(tcpip_mtu_size_cnf);
    return;
  }

  sock_mtu_size_cnf.event_type = (T_SOCK_EVENTTYPE)tcpip_mtu_size_cnf->event_type;
  sock_mtu_size_cnf.result     = (T_SOCK_RESULT)tcpip_mtu_size_cnf->result; 
  sock_mtu_size_cnf.socket     = (T_SOCK_SOCKET)sock_data;
  sock_mtu_size_cnf.mtu_size   =  tcpip_mtu_size_cnf->mtu_size;

  sock_data->callback((T_SOCK_EVENTSTRUCT*)&sock_mtu_size_cnf,sock_data->context);

  PFREE(tcpip_mtu_size_cnf);
} /* sock_mtu_size_cnf */



/*******************************************************************************
** Function  : sock_connect_ind
** Parameter : T_SOCK_CONNECT_IND *
**                   T_SOCK_API_INSTANCE
** Description : new socket_data will be linked to current socket_data(listening socke_data)
*******************************************************************************/
LOCAL void sock_connect_ind(T_TCPIP_CONNECT_IND *tcpip_connect_ind , 
                                  T_SOCK_API_INSTANCE api_instance )
{
  T_SOCK_SOCKET_DATA *cur_sock_data;   /* listening socket_data */
  T_SOCK_SOCKET_DATA *new_sock_data;   /* connected socket_data */
  T_SOCK_CONNECT_IND sock_connect_ind;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("[Socket API] sock_connect_ind()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  /* find listening socket_data
   * tcpip_connent_ind->socket is listening socket descrpitor
   */
  cur_sock_data =  FindSocketData(api_data ,tcpip_connect_ind->socket);
  if(cur_sock_data == NULL)
  {
    PFREE(tcpip_connect_ind);
    return;
  }

  /* allocate socket data */
  MALLOC(new_sock_data, sizeof(T_SOCK_SOCKET_DATA));

  /* put socket data in linked list */
  new_sock_data->next_socket_data = NULL;     
  cur_sock_data->next_socket_data = new_sock_data;

  /* initialize socket data */
  new_sock_data->api_data        = api_data;
  new_sock_data->tcpip_socket    = tcpip_connect_ind->new_socket;
  /* server must register callback using socket_get_callback() after receiving */
  /* connect ind event.                                                        */
  new_sock_data->callback        = NULL; 
  new_sock_data->context         = NULL;  
  new_sock_data->rx_flow_control = SOCK_FLOW_XON;
  new_sock_data->tx_flow_control = SOCK_FLOW_XON;
  new_sock_data->rx_window       = 1;
  new_sock_data->tx_window       = 1;

  /* fill confirm struct */
  sock_connect_ind.event_type   = (T_SOCK_EVENTTYPE)tcpip_connect_ind->event_type;
  sock_connect_ind.result       = (T_SOCK_RESULT)tcpip_connect_ind->result; 
  /* Check !!! why cur_sock data become new_sock_data */
  /* sock_connect_ind.socket       = (T_SOCK_SOCKET)cur_sock_data;*/
  sock_connect_ind.socket          = (T_SOCK_SOCKET)new_sock_data;   
  sock_connect_ind.new_socket   =  tcpip_connect_ind->new_socket;
  sock_connect_ind.peer_ipaddr  =  tcpip_connect_ind->ipaddr;
  sock_connect_ind.peer_port    =  tcpip_connect_ind->port;

  cur_sock_data->callback((T_SOCK_EVENTSTRUCT*)&sock_connect_ind,cur_sock_data->context);

  PFREE(tcpip_connect_ind);
} /* sock_connect_ind */


/*******************************************************************************
** Function  : sock_conn_closed_ind
** Parameter : T_SOCK_CONN_CLOSED_IND *
**             T_SOCK_API_INSTANCE
**             Release closed socket data
*******************************************************************************/
LOCAL void sock_conn_closed_ind(T_TCPIP_CONN_CLOSED_IND *tcpip_conn_closed_ind,
                                      T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_SOCKET_DATA *sock_data;
  T_SOCK_CONN_CLOSED_IND sock_conn_closed_ind;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("[Socket API] sock_closed_ind()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;

  sock_data =  FindSocketData(api_data, tcpip_conn_closed_ind->socket);
  if(sock_data != NULL)
  {
    sock_conn_closed_ind.event_type = (T_SOCK_EVENTTYPE)tcpip_conn_closed_ind->event_type; 
    sock_conn_closed_ind.result     = (T_SOCK_RESULT)tcpip_conn_closed_ind->result;  
    sock_conn_closed_ind.socket     = (T_SOCK_SOCKET)sock_data;

    sock_data->callback(&sock_conn_closed_ind,sock_data->context);

    ReleaseSocketData(sock_data->api_data,sock_data);
  }
  else {;}

  PFREE(tcpip_conn_closed_ind);
} /* sock_conn_closed_ind */


/*******************************************************************************
** Function  : sock_error_ind
** Parameter : T_SOCK_ERROR_IND *
**             T_SOCK_API_INSTANCE
*******************************************************************************/
LOCAL void sock_error_ind(T_TCPIP_ERROR_IND* tcpip_error_ind , 
                                T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_SOCKET_DATA*   sock_data;
  T_SOCK_ERROR_IND sock_error_ind;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("[Socket API] sock_error_ind()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  
  sock_data =  FindSocketData(api_data , tcpip_error_ind->socket);
  if(sock_data != NULL)
  {
    /* fill confirm struct */
    sock_error_ind.event_type   = (T_SOCK_EVENTTYPE)tcpip_error_ind->event_type;
    sock_error_ind.result       = (T_SOCK_RESULT)tcpip_error_ind->result; 
    sock_error_ind.socket       = (T_SOCK_SOCKET)sock_data;

    sock_data->callback(&sock_error_ind,sock_data->context);
  }
  else {;}
  
  PFREE(tcpip_error_ind);
} /* sock_error_ind */


/******************************************************************************/
T_SOCK_RESULT sock_gethostbyaddr(T_SOCK_API_INSTANCE api_instance,
                                 T_SOCK_IPADDR ipaddr, 
                                 T_SOCK_CALLBACK callback,
                                 void* context )
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA*     socket_data;

  TRACE_FUNCTION("[Socket API] sock_gethostbyaddr()");

  if(callback EQ NULL)
   return SOCK_RESULT_INVALID_PARAMETER;

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  
  MALLOC(socket_data, sizeof(T_SOCK_SOCKET_DATA));
   
  /* put socket data in linked list */
  socket_data->next_socket_data = api_data->socket_data;
  api_data->socket_data         = socket_data;
  
  /* initialize socket data */
  socket_data->api_data        = api_data;
  socket_data->tcpip_socket    = 0;
  socket_data->callback        = callback;
  socket_data->context         = context;
  socket_data->rx_flow_control = SOCK_FLOW_XON;
  socket_data->tx_flow_control = SOCK_FLOW_XON;
  socket_data->rx_window       = 1;
  socket_data->tx_window       = 1;
  
  {
    PALLOC (tcpip_hostinfo_req_prim ,TCPIP_HOSTINFO_REQ);
    tcpip_hostinfo_req_prim->app_handle = api_data->hCommAPP;
    tcpip_hostinfo_req_prim->ipaddr = ipaddr;
    *tcpip_hostinfo_req_prim->hostname = 0;
    tcpip_hostinfo_req_prim->request_id = (T_SOCK_SOCKET)socket_data;     
    PSEND(api_data->hCommTCPIP, tcpip_hostinfo_req_prim);
 }
 return SOCK_RESULT_OK;
} /* sock_gethostbyaddr */


/*******************************************************************************
   Function     :  sock_recv_ind
   Parameter    :  T_TCPIP_DATA_IND*
                   T_SOCK_API_INSTANCE  
   Return       : VOID
   Return Event : None
*******************************************************************************/
static void sock_recv_ind(T_TCPIP_DATA_IND* tcpip_data_ind,
                                T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_SOCKET_DATA*   sock_data;
  T_SOCK_RECV_IND sock_receive_ind;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("[Socket API] sock_recv_ind()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  
  sock_data =  FindSocketData(api_data , tcpip_data_ind->socket);
  if(sock_data == NULL)
  {
    MFREE(tcpip_data_ind->data);
    PFREE(tcpip_data_ind);
    return;
  }

  sock_data->rx_window--;  /* when data received, window size will be descreased..*/

  if(sock_data->rx_flow_control == SOCK_FLOW_XOFF )
  {
    /* case sock flow xoff, sock_data will be stored and send to the application
       with sock_flow_xon() */
    sock_data->recv_ind.socket  = tcpip_data_ind->socket;
    sock_data->recv_ind.result = (T_SOCK_RESULT)tcpip_data_ind->result;
    sock_data->recv_ind.event_type = (T_SOCK_EVENTTYPE)tcpip_data_ind->event_type;
    sock_data->recv_ind.data_length = tcpip_data_ind->buflen;
    sock_data->recv_ind.data_buffer = (char*)tcpip_data_ind->data;
  }
  else 
  {
    /* case sock flow xon , sock_data will be sent to application...*/
    sock_receive_ind.event_type   =  (T_SOCK_EVENTTYPE)tcpip_data_ind->event_type;
    sock_receive_ind.result       =  (T_SOCK_RESULT)tcpip_data_ind->result;
    sock_receive_ind.socket       =  (T_SOCK_SOCKET)sock_data;
    sock_receive_ind.data_length  =  tcpip_data_ind->buflen;
    sock_receive_ind.data_buffer  =  (char *)tcpip_data_ind->data;

    sock_data->callback((T_SOCK_EVENTSTRUCT*)&sock_receive_ind,sock_data->context);

    /* send primitive */
    if( (sock_data->rx_flow_control != SOCK_FLOW_XOFF) &&
        (sock_data->rx_window == 0) )
    {
      PALLOC(tcpip_data_res_prim,TCPIP_DATA_RES);
      tcpip_data_res_prim->app_handle = api_data->hCommAPP;
      ACI_ASSERT(sock_data->tcpip_socket == tcpip_data_ind->socket);
      tcpip_data_res_prim->socket       =  tcpip_data_ind->socket ;
      tcpip_data_res_prim->window       = 1;  
      sock_data->rx_window   = 1;
      PSEND(api_data->hCommTCPIP, tcpip_data_res_prim);	
    }
  }
  PFREE(tcpip_data_ind);
} /* sock_recv_ind */


/*******************************************************************************
** Function  : sock_send_cnf
** Parameter : T_TCPIP_DATA_CNF *
**                   T_SOCK_API_INSTANCE
*******************************************************************************/
static void sock_send_cnf(T_TCPIP_DATA_CNF* tcpip_data_cnf,
                                T_SOCK_API_INSTANCE api_instance)
{
  T_SOCK_SOCKET_DATA *sock_data;
  T_SOCK_API_INSTANCE_DATA *api_data;
  T_SOCK_FLOW_READY_IND sock_flow_ready_ind;

  TRACE_FUNCTION("[Socket API] sock_send_cnf()");

  api_data = (T_SOCK_API_INSTANCE_DATA*)api_instance;
  sock_data =  FindSocketData(api_data , tcpip_data_cnf->socket);

  if(sock_data == NULL)
  {
    PFREE(tcpip_data_cnf);
    return;
  }
  /* if tcpip_data_cnf->window is 0,*/
  /* this value will be 1 because 0 and 1 is the same meaning in the Rnet TCPIP.*/
  if( tcpip_data_cnf->window  == 0 )
  {
    sock_data->tx_window  = 1;
  }
  else
  {
    sock_data->tx_window = tcpip_data_cnf->window;
  }

  sock_flow_ready_ind.event_type = (T_SOCK_EVENTTYPE)tcpip_data_cnf->event_type;
  sock_flow_ready_ind.result     = (T_SOCK_RESULT)tcpip_data_cnf->result; 
  sock_flow_ready_ind.socket     = (T_SOCK_SOCKET)sock_data;
  /* permit next packet to be send */
  sock_data->tx_flow_control     = SOCK_FLOW_XON;

  sock_data->callback(&sock_flow_ready_ind,sock_data->context);

  PFREE(tcpip_data_cnf);
}


/******************************************************************************/
T_SOCK_RESULT sock_set_callback(T_SOCK_SOCKET socket ,T_SOCK_CALLBACK new_callback, 
                                void* new_context)
{
  T_SOCK_SOCKET_DATA* sock_data;

  sock_data = (T_SOCK_SOCKET_DATA*)socket;

  if(new_callback EQ NULL)
   return SOCK_RESULT_INVALID_PARAMETER;

  if(sock_data == NULL)
  {
    return SOCK_RESULT_INVALID_PARAMETER;
  }

  sock_data->callback = new_callback;
  sock_data->context = new_context;

  return SOCK_RESULT_OK;
}


/******************************************************************************/
T_SOCK_RESULT sock_get_callback(T_SOCK_SOCKET socket, T_SOCK_CALLBACK *callback_p,
                                void **context_p)
{
  T_SOCK_SOCKET_DATA* sock_data;

  TRACE_FUNCTION("[Socket API] sock_get_callback()");
  
  /* get current callback,context */    
  sock_data = (T_SOCK_SOCKET_DATA*)socket;
  if(sock_data == NULL)
  {
    return SOCK_RESULT_INVALID_PARAMETER;
  }
  if(callback_p != NULL)
  {
    callback_p = &sock_data->callback;
  }
  if(context_p != NULL)
  {
    context_p  = &sock_data->context;
  }
  return SOCK_RESULT_OK;
} /* socket_get_callback */


/******************************************************************************/
T_SOCK_RESULT sock_flow_xoff(T_SOCK_SOCKET socket)
{
  T_SOCK_SOCKET_DATA* sock_data;

  TRACE_FUNCTION("[Socket API] sock_flow_xoff()");

  sock_data = (T_SOCK_SOCKET_DATA*)socket;
  if(sock_data == NULL)
  {
    return SOCK_RESULT_INVALID_PARAMETER;
  }

  sock_data->rx_flow_control = SOCK_FLOW_XOFF;

  return SOCK_RESULT_OK;
 
} /* sock_flow_xoff */


/******************************************************************************/
T_SOCK_RESULT sock_flow_xon(T_SOCK_SOCKET socket)
{
  T_SOCK_SOCKET_DATA* sock_data;
  T_SOCK_RECV_IND sock_receive_ind;
  T_SOCK_API_INSTANCE_DATA *api_data;

  TRACE_FUNCTION("[Socket API] sock_flow_xon()");

  sock_data = (T_SOCK_SOCKET_DATA*)socket;

  if(sock_data == NULL)
  {
    return SOCK_RESULT_INVALID_PARAMETER;
  }

  api_data = sock_data->api_data;
  sock_data->rx_flow_control = SOCK_FLOW_XON;
  
  if(sock_data->recv_ind.data_length != 0)
  {     
    sock_receive_ind.event_type  = sock_data->recv_ind.event_type;
    sock_receive_ind.result      = sock_data->recv_ind.result;
    sock_receive_ind.socket      = (T_SOCK_SOCKET)sock_data;
    sock_receive_ind.data_length  = sock_data->recv_ind.data_length;
    sock_receive_ind.data_buffer  = sock_data->recv_ind.data_buffer;

    /* clear recv_ind struct, buffer is freed by application */
    memset(&sock_data->recv_ind, 0x00 , sizeof(T_SOCK_RECV_IND)); 

    sock_data->callback((T_SOCK_EVENTSTRUCT*)&sock_receive_ind,sock_data->context);
  }

  if((sock_data->rx_flow_control != SOCK_FLOW_XOFF) && (sock_data->rx_window == 0))
  {
    PALLOC(tcpip_data_res_prim,TCPIP_DATA_RES);
    tcpip_data_res_prim->app_handle = api_data->hCommAPP;
    tcpip_data_res_prim->socket   =  sock_data->tcpip_socket;
    tcpip_data_res_prim->window  = 1;
    sock_data->rx_window = 1;

    PSEND(api_data->hCommTCPIP, tcpip_data_res_prim);
  }
  return SOCK_RESULT_OK;
} /* sock_flow_xon */


/******************************************************************************/
T_SOCK_RESULT sock_get_mtu_size(T_SOCK_SOCKET socket)
{
  T_SOCK_API_INSTANCE_DATA* api_data;
  T_SOCK_SOCKET_DATA* sock_data;

  TRACE_FUNCTION("[Socket API] sock_get_mtu_size()");

  sock_data = (T_SOCK_SOCKET_DATA*)socket;

  /* check whether sock_data is NULL or not */
  if(sock_data == NULL)
  {
    return SOCK_RESULT_INVALID_PARAMETER;
  }

  api_data   = sock_data->api_data;

  /* send primitive */
  {
    PALLOC (tcpip_mtu_size_req_prim, TCPIP_MTU_SIZE_REQ);
    tcpip_mtu_size_req_prim->app_handle = api_data->hCommAPP;
    tcpip_mtu_size_req_prim->socket     = sock_data->tcpip_socket;
    PSEND(api_data->hCommTCPIP, tcpip_mtu_size_req_prim);
  }
  return SOCK_RESULT_OK;
} /* sock_get_mtu_size */


/******************************************************************************/
LOCAL void pei_not_supported (void *data)
{
  TRACE_FUNCTION ("pei_not_supported()");

  PFREE (data);
}


/******************************************************************************/
BOOL sock_api_handles_primitive( T_SOCK_API_INSTANCE api_instance, T_PRIM* prim)
{
  LOCAL const T_FUNC tcpip_table[] = 
  {
    MAK_FUNC_N( pei_not_supported,     0 /*TCPIP_INITIALIZE_CNF*/ ),  /* 0x00 */
    MAK_FUNC_N( pei_not_supported,     0 /*TCPIP_SHUTDOWN_CNF*/   ),  /* 0x01 */
    MAK_FUNC_N( pei_not_supported,     0 /*TCPIP_IFCONFIG_CNF */  ),  /* 0x02 */
    MAK_FUNC_N( pei_not_supported,     0 /*TCPIP_DTI_CNF*/        ),  /* 0x03 */
    MAK_FUNC_0( sock_create_cnf,       TCPIP_CREATE_CNF           ),  /* 0x04 */
    MAK_FUNC_0( sock_close_cnf,        TCPIP_CLOSE_CNF            ),  /* 0x05 */
    MAK_FUNC_0( sock_bind_cnf,         TCPIP_BIND_CNF             ),  /* 0x06 */
    MAK_FUNC_0( sock_listen_cnf,       TCPIP_LISTEN_CNF           ),  /* 0x07 */
    MAK_FUNC_0( sock_connect_cnf,      TCPIP_CONNECT_CNF          ),  /* 0x08 */
    MAK_FUNC_0( sock_send_cnf,         TCPIP_DATA_CNF             ),  /* 0x09 */
    MAK_FUNC_0( sock_recv_ind,         TCPIP_DATA_IND             ),  /* 0x0a */
    MAK_FUNC_0( sock_sockname_cnf,     TCPIP_SOCKNAME_CNF         ),  /* 0x0b */
    MAK_FUNC_0( sock_peername_cnf,     TCPIP_PEERNAME_CNF         ),  /* 0x0c */
    MAK_FUNC_0( sock_hostinfo_cnf,     TCPIP_HOSTINFO_CNF         ),  /* 0x0d */
    MAK_FUNC_0( sock_mtu_size_cnf,     TCPIP_MTU_SIZE_CNF         ),  /* 0x0e */
    MAK_FUNC_0( sock_connect_ind,      TCPIP_CONNECT_IND          ),  /* 0x0f */
    MAK_FUNC_0( sock_conn_closed_ind,  TCPIP_CONN_CLOSED_IND      ),  /* 0x10 */
    MAK_FUNC_0( sock_error_ind,        TCPIP_ERROR_IND            ),  /* 0x11 */
  };

  LOCAL const T_FUNC dcm_table[] =
  {
    MAK_FUNC_0( sock_open_bearer_cnf,     DCM_OPEN_CONN_CNF       ),
    MAK_FUNC_0( sock_close_bearer_cnf,    DCM_CLOSE_CONN_CNF      ),
    MAK_FUNC_0( sock_bearer_info_cnf,     DCM_GET_CURRENT_CONN_CNF),
    MAK_FUNC_0( sock_dcm_error_ind,       DCM_ERROR_IND           ),
  };

  TRACE_FUNCTION("sock_api_handles_primitive()");

  if(prim NEQ NULL)
  {
    ULONG        opc = prim->custom.opc;
    USHORT       tabsize;
    const T_FUNC *table = 0;

    switch( SAP_NR(opc) )
    {
      case SAP_NR(TCPIP_DL):
        table = tcpip_table;
        tabsize = TAB_SIZE (tcpip_table);
        break;
      case SAP_NR(DCM_UL):
        table = dcm_table;
        tabsize = TAB_SIZE(dcm_table);
        break;       
      default:
        /* Primitive is not for these SAP's */
        return FALSE;
    }
    
    if (PRIM_NR(opc) < tabsize)
    {
      table += PRIM_NR(opc);
      { 
        TRACE_EVENT_P1( "sock_api_handles_primitive: %d", PRIM_NR(opc) );
      }
      JUMP (table->func) (P2D(prim),api_instance);
    }
    else
    {
      TRACE_ERROR("PRIM_NR(opc) >= tabsize");
      return FALSE;
    }
  }
  return TRUE;
} /* sock_api_handles_primitive */
