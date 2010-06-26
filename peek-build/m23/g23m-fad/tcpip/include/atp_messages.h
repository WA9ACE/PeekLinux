/*******************************************************************************
 *
 * File Name : atp_messages.h
 *
 * External definition for ATP SW entity
 *
 * (C) Texas Instruments, all rights reserved
 *
 * Version number  : 0.1      Date : 28-Feb-2000
 *
 * History      : 0.1  - Created by E. Baissus
 *
 *
 * Author           : Eric Baissus : e-baissus@ti.com
 *
 *   (C) Copyright 2000 by Texas Instruments Incorporated, All Rights Reserved
 ******************************************************************************/
#ifndef ATP_MESSAGES_H
#define ATP_MESSAGES_H

#include "rv_general.h"
#include "rvf_api.h"
#include "atp_config.h"
#include "atp_api.h"

#define ATP_MESSAGES_OFFSET (0x32 << 10)

/* Generic structure for an ATP message */
typedef T_RV_HDR T_ATP_MESSAGE;

/* Header used by nearly all the ATP events */
typedef struct
{
  T_RV_HDR rv_hdr;
  T_ATP_PORT_NB port_nb;
} T_ATP_GENERIC_HEADER;

/* Open port Indication */
#define ATP_OPEN_PORT_IND (ATP_MESSAGES_OFFSET | 0x0001)

typedef struct
{
  T_RV_HDR rv_hdr;
  T_ATP_PORT_NB initiator_port_nb;
  T_ATP_SW_ENTITY_ID initiator_id;
  T_ATP_CUSTOM_INFO *custom_info_p;
} T_ATP_OPEN_PORT_IND;

/* Open port Confimation */
#define ATP_OPEN_PORT_CFM (ATP_MESSAGES_OFFSET | 0x0002)

typedef struct
{
  T_RV_HDR rv_hdr;
  T_ATP_PORT_NB initiator_port_nb;
  T_ATP_CUSTOM_INFO *custom_info_p;
  T_ATP_OPEN_PORT_RESULT result;
} T_ATP_OPEN_PORT_CFM;

/* Port Closed event */
#define ATP_PORT_CLOSED (ATP_MESSAGES_OFFSET | 0x0003)

typedef struct
{
  T_RV_HDR rv_hdr;
  T_ATP_PORT_NB port_nb;
  T_ATP_SW_ENTITY_ID closer_sw_id;
  T_ATP_PORT_NB closer_port_nb;

} T_ATP_PORT_CLOSED;

/* Command Ready Event */
#define ATP_CMD_RDY (ATP_MESSAGES_OFFSET | 0x0010)

typedef struct
{
  T_RV_HDR rv_hdr;
  T_ATP_PORT_NB port_nb;
  T_ATP_CMD_TYPE cmd_type;
  T_ATP_CMD_NB cmd_nb;
  T_ATP_CMD *cmd_info_p;
}T_ATP_CMD_RDY;

/* Text Command Ready Event */
#define ATP_TXT_CMD_RDY (ATP_MESSAGES_OFFSET | 0x0011)

typedef struct
{
  T_RV_HDR rv_hdr;
  T_ATP_PORT_NB port_nb;
  T_ATP_CMD_TYPE cmd_type;
  T_ATP_TXT_CMD txt_cmd_p;
}T_ATP_TXT_CMD_RDY;

/* No copy data ready event */
#define ATP_NO_COPY_DATA_RDY (ATP_MESSAGES_OFFSET | 0x0020)

typedef struct
{
  T_RV_HDR rv_hdr;
  T_ATP_PORT_NB port_nb;
  UINT32 buffer_size;
  T_ATP_BUFFER atp_buffer_p;
} T_ATP_NO_COPY_DATA_RDY;

/* Copy data ready event */
#define ATP_DATA_RDY (ATP_MESSAGES_OFFSET | 0x0021)

typedef struct
{
  T_RV_HDR rv_hdr;
  T_ATP_PORT_NB port_nb;
  UINT32 nb_bytes;

}T_ATP_DATA_RDY;

/* Signal changed event*/
#define ATP_SIGNAL_CHANGED (ATP_MESSAGES_OFFSET | 0x0030)

typedef struct
{
  T_RV_HDR rv_hdr;
  T_ATP_PORT_NB port_nb;
  T_ATP_PORT_SIGNAL signal;
  T_ATP_SIGNAL_CHANGE_MASK mask;
  T_RVF_MB_ID mb;
}T_ATP_SIGNAL_CHANGED;

/* Mode changed event*/
#define ATP_PORT_MODE_CHANGED (ATP_MESSAGES_OFFSET | 0x0031)

typedef struct
{
  T_RV_HDR rv_hdr;
  T_ATP_PORT_NB port_nb;
  T_ATP_PORT_MODE mode;
} T_ATP_PORT_MODE_CHANGED;

/* Mode changed event*/
#define ATP_ERROR (ATP_MESSAGES_OFFSET | 0x0041)

typedef enum
{
  ATP_MEMORY_ERROR,
  ATP_PARAM_ERROR,
  ATP_ISSUED_IN_A_WRONG_STATE_ERROR,
  ATP_WAITING_FOR_RESULT,
  ATP_CANNOT_TRANSLATE_CMD,
  ATP_OTHER_SWE_NOT_IN_PROPER_MODE,
  ATP_SAME_ACTION_ALREADY_DONE, // Example : SWE already registerd
  ATP_NO_MORE_RESSOURCE // Example : no more place for any new SWE registration
} T_ATP_ERROR_TYPE;

typedef enum
{
  ATP_ERROR_FAILED_TO_OPEN_A_PORT,
  ATP_ERROR_FAILED_TO_ACCEPT_A_PORT,
  ATP_ERROR_FAILED_TO_SEND_CMD,
  ATP_ERROR_FAILED_TO_CLOSE_A_PORT,
  ATP_ERROR_FAILED_TO_SEND_DATA,
  ATP_ERROR_FAILED_TO_GET_DATA,
  ATP_ERROR_FAILED_TO_HANDLE_MODE,
  ATP_ERROR_FAILED_TO_HANDLE_SIGNAL,
  ATP_ERROR_FAILED_TO_HANDLE_FLOW_REDIRECTION,
  ATP_ERROR_FAILED_TO_HANDLE_REGISTRATION,
  ATP_ERROR_FAILED_TO_GET_MEMORY
} T_ATP_ERROR_MAIN_REASON;

typedef struct // TO UPDATE
{
  T_RV_HDR rv_hdr;
  T_ATP_ERROR_MAIN_REASON main_reason;
  T_ATP_ERROR_TYPE error_type;
} T_ATP_ERROR;

#endif

