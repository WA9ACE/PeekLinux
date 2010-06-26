/*******************************************************************************
 *
 * File Name : atp_api.h
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
#ifndef ATP_API_H
#define ATP_API_H

#include "rv_general.h"
#include "rvf_api.h"
#include "atp_config.h"
#include "atp_cmd.h"

typedef T_RV_RET T_ATP_RET;

typedef UINT16 T_ATP_SW_ENTITY_ID;
#define ATP_WRONG_SW_ID 0xFFFF

typedef UINT8 T_ATP_SW_ENTITY_NAME[ATP_SW_ENTITY_NAME_MAX_CARAC];
typedef UINT8 * T_ATP_BUFFER;

typedef T_RV_RETURN T_ATP_CALLBACK;

/* ---------------------------------------------------*/
/*   Definitions related to SW entity registration     */
/* ---------------------------------------------------*/

// Define the command modes : text or interpreted
typedef enum
{
  TXT_MODE,
  INTERPRETED_MODE
} T_ATP_CMD_MODE;

// Define the copy modes : copy is on  or off
typedef enum
{
  COPY_ON,
  COPY_OFF
} T_ATP_COPY_MODE;

// Define if the SWE support commands or not
typedef enum
{
  CMD_SUPPORT_ON,
  CMD_SUPPORT_OFF
} T_ATP_CMD_SUPPORT;

// Structure defining the different mode selection of a SW entity which register to the ATP
typedef struct
{
  T_ATP_CMD_SUPPORT cmd_support_mode;
  T_ATP_CMD_MODE cmd_mode;
  T_ATP_COPY_MODE cp_mode;
} T_ATP_ENTITY_MODE;

#define ATP_INVALID_SWE_ID (0xFF)

/* ---------------------------------------------------*/
/*   Definitions related to port registration     */
/* ---------------------------------------------------*/
typedef UINT16 T_ATP_PORT_NB; // Need to remain 16 bits in order to fit with SPP (MSB is FLAG SERVER/CLIENT)
typedef UINT8 T_ATP_SIGNAL_MASK;
typedef UINT8 T_ATP_SIGNAL_CHANGE_MASK;

typedef enum
{
  DCE_CONFIG, // in this configuration, if one of the SW entity is not supporting commands (CMD_SUPPORT_OFF), ATP will automatically emulates a DCE on the port .
  DTE_CONFIG, // in this configuration, if one of the SW entity is not supporting commands (CMD_SUPPORT_OFF), ATP will automatically emulates a DTE on the port .
  DATA_CONFIG, // in this configuration, only data can be exchanged on the port.
  NOT_DEFINED_CONFIG // Configuration defined by the other SWE
 } T_ATP_PORT_CONFIG;

typedef UINT8 T_ATP_RING_TYPE;

#define ATP_NO_RING_TYPE (0x00) // the ringing signal is meaningless.
#define ATP_VOICE_RING_TYPE (0x01) // the SW entity is only interested in voice calls (i.e Headset).
#define ATP_DATA_RING_TYPE (0x02) // the SW entity is only interested in data calls (i.e Dial-Up Networking).
#define ATP_FAX_RING_TYPE (0x04) // the SW entity is only interested in fax calls (i.e Fax).

#define ATP_INVALID_PORT_CONFIG_VALUE (0xFF)

typedef struct
{
  T_ATP_PORT_CONFIG port_config;
  T_ATP_RING_TYPE ring_type;
  T_ATP_SIGNAL_MASK signal_mask;
  T_ATP_DCE_MASK dce_mask;
} T_ATP_PORT_INFO;

// Structure used to store information when no copy option is selected
typedef enum
{
  RX_HEADER_ON,
  RX_HEADER_OFF
} T_ATP_RX_HEADER_MODE;

typedef enum
{
  TX_HEADER_ON,
  TX_HEADER_OFF
} T_ATP_TX_HEADER_MODE;

typedef enum
{
  SEGMENTED_PACKET,
  NORMAL_PACKET
} T_ATP_PACKET_MODE;

#define ATP_GET_ALL_EVENTS (0xFFFF)

typedef struct
{
  T_RVF_MB_ID rx_mb;
  T_RVF_MB_ID tx_mb;
  T_ATP_TX_HEADER_MODE tx_head_mode;
  UINT16 tx_head_size;
  UINT16 tx_trail_size;
  T_ATP_RX_HEADER_MODE rx_head_mode;
  UINT16 rx_head_size;
  UINT16 rx_trail_size;
  T_ATP_PACKET_MODE packet_mode;
} T_ATP_NO_COPY_INFO;

// Definition for custom parameters
typedef enum
{
  ATP_TO_SPP_INFO,
  ATP_FROM_SPP_INFO,
  ATP_TO_GSM_INFO,
  ATP_FROM_GSM_INFO,
  ATP_TO_RNET_INFO,
  ATP_FROM_RNET_INFO
} T_ATP_CUSTOM_TYPE;

/* Custom information used during a new open port response by gsm */

typedef struct
{
  T_ATP_CUSTOM_TYPE custom_type;
} T_ATP_CUSTOM_INFO;

// open port result definition
typedef enum
{
  OPEN_PORT_OK,
  OPEN_PORT_NOK
} T_ATP_OPEN_PORT_RESULT;

// definition related to command functions
typedef UINT8 T_ATP_CMD_TYPE;
#define AT_CMD (0)
#define RESULT_CODE (1)
#define UNSOLICITED_RESULT (2)
#define INFORMATION_TXT (3)
#define CUSTOM_CMD (4)
#define CMD_ABORT (5)
#define UNKNOWN (6)
#define PRELIMINARY_RESULT_CODE (7)

typedef UINT16 T_ATP_CMD_NB;

typedef void T_ATP_CMD;

typedef char * T_ATP_TXT_CMD;

// definition related to signals
typedef UINT8 T_ATP_PORT_SIGNAL;

// Signal is unmasked when mask is set to 1
#define ATP_RX_FLOW_ON (0x01)
#define ATP_RX_FLOW_OFF (0x00)
#define ATP_TX_FLOW_ON (0x02)
#define ATP_TX_FLOW_OFF (0x00)
#define ATP_DTR_DSR_0 (0x00)
#define ATP_DTR_DSR_1 (0x04)
#define ATP_RTS_CTS_0 (0x00)
#define ATP_RTS_CTS_1 (0x08)
#define ATP_DCD_1 (0x10)
#define ATP_DCD_0 (0x00)
#define ATP_RI_1 (0x020)
#define ATP_RI_0 (0x00)

#define ATP_DTR_DSR_UNMASK (ATP_DTR_DSR_1)
#define ATP_RTS_CTS_UNMASK (ATP_RTS_CTS_1)
#define ATP_DCD_UNMASK (ATP_DCD_1)
#define ATP_RI_UNMASK (ATP_RI_1)
#define ATP_RX_FLOW_UNMASK (ATP_RX_FLOW_ON)
#define ATP_TX_FLOW_UNMASK (ATP_TX_FLOW_ON)

/* Number of signal allowed is defined by */
#define ATP_NON_RX_TX_SIGNAL_UNMASK (ATP_DTR_DSR_UNMASK | ATP_RTS_CTS_UNMASK | ATP_DCD_UNMASK | \
    ATP_RI_UNMASK)
#define ATP_ALL_THE_SIGNAL_UNMASK (ATP_RX_FLOW_UNMASK | ATP_TX_FLOW_UNMASK | \
    ATP_NON_RX_TX_SIGNAL_UNMASK)

// definition related to port mode and data exchange
typedef enum
{
  ATP_PORT_DATA_MODE,
  ATP_PORT_CMD_MODE
} T_ATP_PORT_MODE;

typedef enum
{
  ATP_REDIRECT_ON,
  ATP_REDIRECT_OFF
} T_ATP_REDIRECT_MODE;

/******************************************************************************
 * Function name: atp_reg
 *
 * Description : This function is used to register a new SW entity to the ATP entity

 * Parameters :  - name = SW entity name
 *         - return_path => way to send event to the new registered entity,
 *         - mode => supported mode of the new registered entity
 *         - &sw_id_p => pointer on id to use later on to identify the SW entity
 *
 * Return     :   Standard error
 *          RV_OK or RV_MEMORY_ERR (no more entity can register, or prim MB not GREEN)
 *
 * History      : 0.1 (29-Feb-2000)
 *          : 0.9 (3-May-2000) : reviewed
 ******************************************************************************/
 T_ATP_RET atp_reg(T_ATP_SW_ENTITY_NAME name, T_ATP_CALLBACK return_path,
   T_ATP_ENTITY_MODE mode , T_ATP_SW_ENTITY_ID * sw_id_p);

/******************************************************************************
 * Function name: atp_dereg
 *
 * Description : This function is used to remove a SW entity to the list of
 * SW entities registered in ATP.
 *
 * Parameters :  - sw_id => id of the SWE
 *
 * Return     :   Standard error
 *          RV_OK  ,
 *          RV_NOT_READY if a port is still open involving the SW entity
 *          RV_NOT_SUPPORTED if sw_id is already de-registered -> ignore call
 *
 * History      : 0.1 (29-Feb-2000)
 *          : 0.9 (3-May-2000) : reviewed
 ******************************************************************************/
T_ATP_RET atp_dereg(T_ATP_SW_ENTITY_ID sw_id);

/******************************************************************************
 * Function name: atp_reg_info
 *
 * Description : This function is used to get info on a SWE
 *
 * Parameters :  - name => name of the SWE to get info on
 *         - return : id of the SWE
 *         - return : SWE mode information
 *
 * Return     :   Standard error
 *          RV_OK  ,
 *          RV_NOT_SUPPORTED if name has not been found
 *
 * History      : 0.1 (29-Feb-2000)
 *          : 0.9 (3-May-2000) : reviewed
 ******************************************************************************/
T_ATP_RET atp_reg_info(T_ATP_SW_ENTITY_NAME name, T_ATP_SW_ENTITY_ID * sw_id_p,
             T_ATP_ENTITY_MODE * mode_p);

/******************************************************************************
 * Function name: atp_free_message
 *
 * Description : Generic function service provided to SW entity to free an ATP message
 *
 * Parameters :  pointer on the buffer
 *
 * Return     :   return of the rvf_free_buf function
 *
 * History      : 0.1 (1-Marsh-2000) - Created
 *
 ******************************************************************************/
T_ATP_RET atp_free_message(void * buffer_p);

/******************************************************************************
* Function name: atp_open_port_rqst
*
* Description : Initialise a port creation
*
* Parameters : see BT9901
*
* Return :
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/

T_ATP_RET atp_open_port_rqst(T_ATP_SW_ENTITY_ID initiator_id, T_ATP_SW_ENTITY_ID target_id,
               T_ATP_PORT_NB port_nb, T_ATP_PORT_INFO port_info,
               T_ATP_NO_COPY_INFO no_copy_info, T_ATP_CUSTOM_INFO * cust_info_p);

/******************************************************************************
* Function name: atp_open_port_rsp
*
* Description : Response from the target to a open port request
*
* Parameters : see BT9901
*
* Return :
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/

T_ATP_RET atp_open_port_rsp(T_ATP_SW_ENTITY_ID initiator_id, T_ATP_PORT_NB initiator_port_nb,
              T_ATP_SW_ENTITY_ID target_id, T_ATP_PORT_NB target_port_nb,
              T_ATP_PORT_INFO port_info, T_ATP_NO_COPY_INFO no_copy_info,
              T_ATP_CUSTOM_INFO * custom_info_p, T_ATP_OPEN_PORT_RESULT result);

/******************************************************************************
* Function name: atp_close_port
*
* Description : Close a port
*
* Parameters : see BT9901
*
* Return :
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_close_port(T_ATP_SW_ENTITY_ID closer_sw_id, T_ATP_PORT_NB port_nb);

/******************************************************************************
* Function name: atp_get_buffer
*
* Description : Generic function service provided to SW entity to free an ATP buffer
*
* Parameters : pointer on the buffer
*
* Return : return of the rvf_free_buf function
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_get_buffer(UINT32 buffer_size,void ** buffer_pp);

/******************************************************************************
* Function name: atp_free_buffer
*
* Description : Generic function service provided to SW entity to free an ATP buffer
*
* Parameters : pointer on the buffer
*
* Return : return of the rvf_free_buf function
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_free_buffer(void * buffer_p);

/******************************************************************************
* Function name: atp_send_cmd
*
* Description : Send a command
*
* Parameters : see BT9901
*
* Return :
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_send_cmd(T_ATP_SW_ENTITY_ID sender_sw_id, T_ATP_PORT_NB sender_port_nb,
             T_ATP_CMD_TYPE cmd_type, T_ATP_CMD_NB cmd_nb, T_ATP_CMD * cmd_info_p);

/******************************************************************************
* Function name: atp_send_txt_cmd
*
* Description : Send a command in text format
*
* Parameters : see BT9901
*
* Return :
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_send_txt_cmd(T_ATP_SW_ENTITY_ID sender_sw_id, T_ATP_PORT_NB sender_port_nb,
               T_ATP_CMD_TYPE cmd_type, T_ATP_TXT_CMD text_p);

/******************************************************************************
* Function name: atp_no_copy_send_data
*
* Description : Send data on a port.
*
* Parameters : see BT9901
*
* Return :
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_no_copy_send_data(T_ATP_SW_ENTITY_ID sender_sw_id, T_ATP_PORT_NB sender_port_nb,
            T_ATP_BUFFER atp_buffer_p, UINT32 buffer_size);

/******************************************************************************
* Function name: atp_send_data
*
* Description : Send data on a port
*
* Parameters : see BT9901
*
* Return :
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_send_data(T_ATP_SW_ENTITY_ID sender_sw_id, T_ATP_PORT_NB sender_port_nb,
            void * data_buffer_p, UINT32 buffer_size, UINT32 *nb_bytes_left_p);

/******************************************************************************
* Function name: atp_get_data
*
* Description : Copy data in the SW entity buffer
*
* Parameters : see BT9901
*
* Return :
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_get_data(T_ATP_SW_ENTITY_ID receiver_sw_id, T_ATP_PORT_NB receiver_port_nb,
             UINT8 * data_buffer, UINT32 nb_to_read, UINT32 *nb_read_p, UINT32 *nb_left_p);

/******************************************************************************
* Function name: atp_set_signal
*
* Description : Set modem signals
*
* Parameters : see BT9901
*
* Return :
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_set_signal(T_ATP_SW_ENTITY_ID sender_sw_id, T_ATP_PORT_NB sender_port_nb,
             T_ATP_PORT_SIGNAL signal_value, T_ATP_SIGNAL_CHANGE_MASK signal_mask);

/******************************************************************************
* Function name: atp_get_signal
*
* Description : Get signal value of the port
*
* Parameters : see BT9901
*
* Return : RV_OK
* RV_INVALID_PARAMETER : one of the id or port_nb was wrong : ignore call
*
* atp_error can be called if MB is RED
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_get_signal(T_ATP_SW_ENTITY_ID sw_id, T_ATP_PORT_NB port_nb,
             T_ATP_PORT_SIGNAL * signal_p);

/******************************************************************************
* Function name: atp_set_mode
*
* Description : Change the mode of the port
*
* Parameters : see BT9901
*
* Return : RV_OK
* RV_INVALID_PARAMETER : one of the id or port_nb was wrong : ignore call
*
* atp_error can be called if MB is RED
*
* History : 0.1 (09-May-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_set_mode(T_ATP_SW_ENTITY_ID sender_sw_id, T_ATP_PORT_NB sender_port_nb,
             T_ATP_PORT_MODE mode);

/******************************************************************************
* Function name: atp_flow_redirect
*
* Description : Redirect the flow from one port to another one
*
* Parameters : see BT9901
*
* Return : RV_OK
* RV_INVALID_PARAMETER : one of the id or port_nb was wrong : ignore call
*
* atp_error can be called if MB is RED
*
* History : 0.1 (22-May-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_flow_redirect(T_ATP_SW_ENTITY_ID sw_id, T_ATP_PORT_NB port_nb_1,
             T_ATP_PORT_NB port_nb_2, T_ATP_REDIRECT_MODE redirect_mode);

/******************************************************************************
* Function name: atp_get_buffer_from_tx_mb
*
* Description : Function used to get memory from the tx memory bank of the SWE
*
* Parameters :
*
* Return : return of the rvf_free_buf function
*
* History : 0.1 (1-Marsh-2000) - Created
*
******************************************************************************/
T_ATP_RET atp_get_buffer_from_tx_mb(T_ATP_SW_ENTITY_ID sender_sw_id,T_ATP_PORT_NB sender_port_nb,
                  UINT32 buffer_size,void ** buffer_pp);

/******************************************************************************
* Function name: atp_get_info_on_port_end
*
* Description : Provide information on the other end of the port
* (for example, which format of data the other SW entity is expecting )
*
* Parameters : see BT9901
*
* Return : RV_OK
* RV_INVALID_PARAMETER : one of the id or port_nb was wrong : ignore call
* RV_NOT_SUPPORTED : command needed to be translated and was unknow by ATP
*
* atp_error can be called if MB is RED
*
* History : 0.1 19-Dec-2001
*
******************************************************************************/
// Structure gathering main information regardinmg NO_COPY mode
typedef struct
{
  UINT16 tx_head_size;
  UINT16 tx_trail_size;
  UINT16 rx_head_size;
  UINT16 rx_trail_size;
    T_ATP_PACKET_MODE packet_mode; // SEGMENTED means that data are stored in segmented buffer.
                  // l2cap_read_uint function must be called to fetch the data
                    // NORMAL means that data are in a single buffer
} T_ATP_NO_COPY_PORT_INFO; // Information regarding no copy data processing

typedef struct
{
  T_ATP_SW_ENTITY_NAME name;
  T_ATP_ENTITY_MODE mode;
  T_ATP_NO_COPY_PORT_INFO no_copy_info;
} T_ATP_OTHER_PORT_END_INFO;

T_ATP_RET atp_get_info_on_port_end (T_ATP_SW_ENTITY_ID requester_sw_id, T_ATP_PORT_NB requester_port_nb,
             T_ATP_OTHER_PORT_END_INFO * other_info_p);

#endif

