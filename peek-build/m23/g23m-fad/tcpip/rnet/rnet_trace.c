/**
 * @file  rnet_trace.c
 *
 * Riviera NET.
 *
 * PC trace functions. Only under Riviera Tool.
 *
 * @author  Vincent Oberle (v-oberle@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date         Author        Modification
 *  --------------------------------------------------
 *  01/30/2002  Vincent Oberle    Create
 *
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "rnet_trace_i.h"

#ifdef RNET_TRACE_RET

/**
 * Traces the return value of RNET API functions.
 */
void rnet_trace_ret (T_RNET_RET ret)
{
  switch (ret) {
  case RNET_OK:
    //RNET_TRACE_LOW("RNET: OK");
    break;
  case RNET_MEMORY_ERR:
    RNET_TRACE_LOW("RNET: MEMORY_ERR");
    break;
  case RNET_INVALID_PARAMETER:
    RNET_TRACE_LOW("RNET: INVALID_PARAMETER");
    break;
  case RNET_NOT_SUPPORTED:
    RNET_TRACE_LOW("RNET: NOT_SUPPORTED");
    break;
  case RNET_NOT_READY:
    RNET_TRACE_LOW("RNET: NOT_READY");
    break;
  case RNET_INTERNAL_ERR:
    RNET_TRACE_LOW("RNET: INTERNAL_ERR");
    break;
  case RNET_IN_USE:
    RNET_TRACE_LOW("RNET: IN_USE");
    break;
  case RNET_NOT_INITIALIZED:
    RNET_TRACE_LOW("RNET: NOT_INITIALIZED");
    break;
  case RNET_NET_UNREACHABLE:
    RNET_TRACE_LOW("RNET: NET_UNREACHABLE");
    break;
  case RNET_TIMEOUT:
    RNET_TRACE_LOW("RNET: TIMEOUT");
    break;
  case RNET_CONN_REFUSED:
    RNET_TRACE_LOW("RNET: CONN_REFUSED");
    break;
  case RNET_CONN_RESET:
    RNET_TRACE_LOW("RNET: CONN_RESET");
    break;
  case RNET_CONN_ABORTED:
    RNET_TRACE_LOW("RNET: CONN_ABORTED");
    break;
  case RNET_CONN_CLOSED:
    RNET_TRACE_LOW("RNET: CONN_CLOSED");
    break;
  case RNET_MSG_SIZE:
    RNET_TRACE_LOW("RNET: MSG_SIZE");
    break;
  case RNET_PARTIAL_SENT:
    RNET_TRACE_LOW("RNET: PARTIAL_SENT");
    break;
  case RNET_HOST_NOT_FOUND:
    RNET_TRACE_LOW("RNET: HOST_NOT_FOUND");
    break;

  default:
    RNET_TRACE_MEDIUM_PARAM("RNET: Unknown return value: ", ret);
  }
}

#endif

