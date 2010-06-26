/**
* @file i2c_i.h
*
* Internal definitions for I2C.
*
* @author Remco Hiemstra (remco.hiemstra@ict.nl)
* @version 0.1
*/

/*
* History:
*
* Date        Author          Modification
* -------------------------------------------------------------------
* 12/30/2003  Remco Hiemstra (remco.hiemstra@ict.nl)    Create.
*
* (C) Copyright 2003 by ICT Automatisering, All Rights Reserved
*/

#ifndef __I2C_INST_I_H_
#define __I2C_INST_I_H_

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "i2c/i2c_cfg.h"
#include "i2c/i2c_api.h"
#include "i2c/i2c_message.h"

#include "i2c/i2c_state_i.h"
#include "i2c/i2c_hw_functions.h"

#ifndef _WINDOWS

#include "nucleus.h" /* used for HISR */

#endif




/** Macro used for tracing I2C messages. */
#if 0
#define I2C_SEND_TRACE(string, trace_level) \
 rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, I2C_USE_ID)  

#define I2C_SEND_TRACE_PARAM(string, param, trace_level) \
 rvf_send_trace (string, (sizeof(string) - 1), param, trace_level, I2C_USE_ID) 
#else

#define I2C_SEND_TRACE(string, trace_level)
#define I2C_SEND_TRACE_PARAM(string, param, trace_level)

#endif

#ifndef _WINDOWS

typedef struct 
{  
  NU_HISR  hisr;
#ifndef HISR_STACK_SHARING
  char     hisr_stack[512];
#endif
} T_HISR_INFOS;

#endif

/**
* The Control Block buffer of I2C, which gathers all 'Global variables'
* used by I2C instance.
*
* A structure should gathers all the 'global variables' of I2C instance.
* Indeed, global variable must not be defined in order to avoid using static memory.
* A T_I2C_ENV_CTRL_BLK buffer is allocated when creating I2C instance and is 
* then always refered by I2C instance when access to 'global variable' 
* is necessary.
*/
typedef struct
{
  /** Store the current state of the I2C instance */
  T_I2C_INTERNAL_STATE state;
  
  /** Pointer to the error function */
  T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name, 
    T_RVM_RETURN error_cause,
    T_RVM_ERROR_TYPE error_type,
    T_RVM_STRING error_msg);
  /** Mem bank id. */
  T_RVF_MB_ID prim_mb_id;
  
  T_RVF_ADDR_ID addr_id;
  
  BOOLEAN initialized;
  
  /** Holds configuration setting of I2C 
  use for parameter checking */
  T_I2C_CONFIG config;
  
  /* interrupt callback pointer */
  T_I2C_INT_CALLBCK  i2c_int_callbck_p;

#ifndef _WINDOWS

  T_HISR_INFOS hisr_infos;
#endif

  
} T_I2C_ENV_CTRL_BLK;


/** External ref "global variables" structure. */
extern T_I2C_ENV_CTRL_BLK *i2c_env_ctrl_blk_p;


#endif /* __I2C_INST_I_H_ */
