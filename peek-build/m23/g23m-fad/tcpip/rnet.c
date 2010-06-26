/*---------------------------------------------------------------------------
 * Adaption of Riviera Net (RNET) TCP/IP to GPF and DTI2
 *-------------------------------------------------------------------------*/

/* Here we define some dummy Riviera Frame functions
 * so that we can link an executable,
 * only to be sure that we've copied all we need. */

/*-------------------------------------------------------------------------*/

#include "rnet.h"

/*-------------------------------------------------------------------------*/


#include "atp_cmd.h"

/* from /ti/release/ti_5.4.0_p1/code/services/atp/atp_sw_ent.c */

T_ATP_RET atp_reg (
  T_ATP_SW_ENTITY_NAME name,
  T_ATP_CALLBACK return_path,
  T_ATP_ENTITY_MODE mode ,
  T_ATP_SW_ENTITY_ID * sw_id_p
) {
  return RV_OK;
}

T_ATP_RET atp_dereg (T_ATP_SW_ENTITY_ID sw_id)
{
  return RV_OK;
}

/*-------------------------------------------------------------------------*/

/* from /ti/release/ti_5.4.0_p1/code/services/atp/atp_services.c */

T_ATP_RET atp_send_data (
  T_ATP_SW_ENTITY_ID sender_sw_id,
  T_ATP_PORT_NB sender_port_nb,
  void * data_buffer_p,
  UINT32 buffer_size,
  UINT32 * nb_bytes_left_p
) {
  return RV_OK;
}

T_ATP_RET atp_get_data (
  T_ATP_SW_ENTITY_ID receiver_sw_id,
  T_ATP_PORT_NB receiver_port_nb,
  UINT8 * data_buffer,
  UINT32 nb_to_read,
  UINT32 * nb_read_p,
  UINT32 * nb_left_p
) {
  return RV_OK;
}

T_ATP_RET atp_open_port_rsp (
  T_ATP_SW_ENTITY_ID initiator_id,
  T_ATP_PORT_NB initiator_port_nb,
  T_ATP_SW_ENTITY_ID target_id,
  T_ATP_PORT_NB target_port_nb,
  T_ATP_PORT_INFO port_info,
  T_ATP_NO_COPY_INFO no_copy_info,
  T_ATP_CUSTOM_INFO * custom_info_p,
  T_ATP_OPEN_PORT_RESULT result
) {
  return RV_OK;
}

T_ATP_RET atp_free_buffer (void * buffer_p)
{
  return RV_OK;
}

