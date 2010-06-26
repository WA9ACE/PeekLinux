/**
 * @file  rnet_trace_i.h
 *
 * Riviera NET - Internal file.
 *
 * RNET trace macros.
 *
 * @author  Vincent Oberle (v-oberle@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date         Author        Modification
 *  --------------------------------------------------
 *  01/25/2002  Vincent Oberle    Create
 *
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __RNET_TRACE_I_H_
#define __RNET_TRACE_I_H_

#include "rv_general.h"
#include "rvf_api.h"
#include "rvm_use_id_list.h"

#include "rnet_api.h"

/** Traces the return value of RNET API functions or not. */
#define RNET_TRACE_RET

#define RNET_TRACE(string, level) \
  rvf_send_trace(string, sizeof(string) - 1, NULL_PARAM, level, RNET_USE_ID);

#define RNET_TRACE_PARAM(string, param, level) \
  rvf_send_trace(string, sizeof(string) - 1, param, level, RNET_USE_ID);

#define RNET_TRACE_ERROR(string) \
  rvf_send_trace(string, sizeof(string) - 1, NULL_PARAM, \
          RV_TRACE_LEVEL_ERROR, RNET_USE_ID);

#define RNET_TRACE_ERROR_PARAM(string, param) \
  rvf_send_trace(string, sizeof(string) - 1, param, \
          RV_TRACE_LEVEL_ERROR, RNET_USE_ID);

#define RNET_TRACE_WARNING(string) \
  rvf_send_trace(string, sizeof(string) - 1, NULL_PARAM, \
          RV_TRACE_LEVEL_WARNING, RNET_USE_ID);

#define RNET_TRACE_WARNING_PARAM(string, param) \
  rvf_send_trace(string, sizeof(string) - 1, param, \
          RV_TRACE_LEVEL_WARNING, RNET_USE_ID);

#define RNET_TRACE_HIGH(string) \
  rvf_send_trace(string, sizeof(string) - 1, NULL_PARAM, \
          RV_TRACE_LEVEL_DEBUG_HIGH, RNET_USE_ID);

#define RNET_TRACE_HIGH_PARAM(string, param) \
  rvf_send_trace(string, sizeof(string) - 1, param, \
          RV_TRACE_LEVEL_DEBUG_HIGH, RNET_USE_ID);

#define RNET_TRACE_MEDIUM(string) \
  rvf_send_trace(string, sizeof(string) - 1, NULL_PARAM, \
          RV_TRACE_LEVEL_DEBUG_MEDIUM, RNET_USE_ID);

#define RNET_TRACE_MEDIUM_PARAM(string, param) \
  rvf_send_trace(string, sizeof(string) - 1, param, \
          RV_TRACE_LEVEL_DEBUG_MEDIUM, RNET_USE_ID);

#define RNET_TRACE_LOW(string) \
  rvf_send_trace(string, sizeof(string) - 1, NULL_PARAM, \
          RV_TRACE_LEVEL_DEBUG_LOW, RNET_USE_ID);

#define RNET_TRACE_LOW_PARAM(string, param) \
  rvf_send_trace(string, sizeof(string) - 1, param, \
          RV_TRACE_LEVEL_DEBUG_LOW, RNET_USE_ID);

#ifdef RNET_TRACE_RET
void rnet_trace_ret (T_RNET_RET ret);
#else
#define rnet_trace_ret(ret)
#endif

#endif /* __RNET_TRACE_I_H_ */

