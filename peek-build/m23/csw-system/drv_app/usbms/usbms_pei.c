/*
+-----------------------------------------------------------------------------
|  Project :  Riv2Gpf
|  Module  :  USBMS
+-----------------------------------------------------------------------------
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
|  Purpose :  This module implements the process body interface
|             for the entity USBMS
|
|             Exported functions:
|
|          pei_create    - Create the Protocol Stack Entity
|          pei_init      - Initialize Protocol Stack Entity
|          pei_exit      - Close resources and terminate
|          pei_run       - Process Messages
|          pei_primitive - Process Primitive
|          pei_signal    - Process Signals
|          pei_timeout   - Process Timeout
|          pei_config    - Dynamic Configuration
|          pei_monitor   - Monitoring of physical Parameters
|+-----------------------------------------------------------------------------
*/

/*============================ Includes ======================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"

#include "pei.h"
#include "tok.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_ext_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "usbms/usbms_i.h"
#include "usbms/usbms_env.h"
#include "gbi/gbi_i.h"

/*============================ Macros ========================================*/
#define VSI_CALLER USBMS_handle,
#define pei_create usbms_pei_create
#define RVM_TRACE_DEBUG_HIGH(string) \
    rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_HANDLE USBMS_handle;
T_HANDLE usbms_hCommUSBMS = -1;

static BOOL first_access  = TRUE;

T_RV_HDR       *Tab[NB_ELEMENT];
UINT8       index;


T_USBMS_XFER_BUFFER usbMsXferBuffer;
T_USBMS_XFER_BUFFER  *xfer_buffer;

T_USBMS_ENV_CTRL_BLK *usbms_env_ctrl_blk_p = NULL;

/*============================ Extern Variables ==============================*/
extern T_GBI_ENV_CTRL_BLK *gbi_env_ctrl_blk_p;

/*============================ Function Definition============================*/
/*
+------------------------------------------------------------------------------
| Function    : pei_monitor
+------------------------------------------------------------------------------
| Description : This function is called by the frame in case sudden entity
|               specific data is requested (e.g. entity Version).
|
| Parameters  : out_monitor       - return the address of the data to be
|                                   monitoredCommunication handle
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/

/*
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  RVM_TRACE_DEBUG_HIGH ("USBMS: pei_monitor");

  return PEI_OK;

} // End pei_monitor(..) 

*/


/*
+------------------------------------------------------------------------------
| Function    : pei_config
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a primitive is
|               received indicating dynamic configuration.
|
|               This function is not used in this entity.
|
|Parameters   :  in_string   - configuration string
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/

/*
LOCAL SHORT pei_config (char *inString)
{
  RVM_TRACE_DEBUG_HIGH ("USBMS: pei_config");

  return PEI_OK;

}// End of pei_config(..) 

*/


/*
+------------------------------------------------------------------------------
|  Function     :  pei_timeout
+------------------------------------------------------------------------------
|  Description  :  Process timeout.
|
|  Parameters   :  index     - timer index
|
|  Return       :  PEI_OK    - timeout processed
|                  PEI_ERROR - timeout not processed
+------------------------------------------------------------------------------
*/

/*
LOCAL SHORT pei_timeout (unsigned short index)
{
  RVM_TRACE_DEBUG_HIGH ("USBMS: pei_timeout");

  return PEI_OK;

}// End of pei_timeout(..) 

*/


/*
+------------------------------------------------------------------------------
| Function    : pei_signal
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a signal has been
|               received.
|
| Parameters  : opc               - signal operation code
|               *data             - pointer to primitive
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/

/*
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  RVM_TRACE_DEBUG_HIGH ("USBMS: pei_signal");

  return PEI_OK;

}// End of pei_signal(..) 

*/


/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
  RVM_TRACE_DEBUG_HIGH ("USBMS: pei_exit");

  /*
   * Close communication channels
   */
  vsi_c_close (VSI_CALLER usbms_hCommUSBMS);
  usbms_hCommUSBMS = VSI_ERROR;

  usbms_stop (NULL);
  usbms_kill ();

  return RVM_OK;

}/* End of pei_exit(..) */

/*
+------------------------------------------------------------------------------
| Function    : pei_primitive
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when a primitive is
|                received and needs to be processed.
|
| Parameters  : prim      - Pointer to the received primitive
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/

/*
LOCAL SHORT pei_primitive (void * primptr)
{
   T_RV_HDR* msg_p = (T_RV_HDR *)primptr;
   T_RV_RET ret = RVM_OK;


   msg_p = (T_RV_HDR *)primptr;

   ret = usbms_handle_message(msg_p);

   return ret;
}

*/

/*
+------------------------------------------------------------------------------
| Function    : pei_run
+------------------------------------------------------------------------------
| Description : This function is called by the frame when entering the main
|               loop. This fucntion is only required in the active variant.
|
|               This function is not used.
|
| Parameters  : handle            - Communication handle
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{
   BOOLEAN error_occured = FALSE;
   T_RV_HDR* msg_p = NULL;
   UINT16 received_event = 0;
   char buf[64] = "";

   RVM_TRACE_DEBUG_HIGH("USBMS: pei_run");

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */
      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
         usbms_handle_message(msg_p);
      }

   }
   return PEI_OK;

}/* End of pei_run(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_init
+------------------------------------------------------------------------------
| Description : This function is called by the frame. It is used to initialise
|               the entitiy.
|
| Parameters  : handle            - task handle
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
    UINT8 i=0;

    RVM_TRACE_DEBUG_HIGH("USBMS: pei_init");

    /*
     * Initialize task handle
     */
    USBMS_handle = handle;
    usbms_hCommUSBMS = -1;

    if(USBMS_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("USBMS_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( USBMS_handle, "USBMS" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (usbms_hCommUSBMS < VSI_OK)
    {
      if ((usbms_hCommUSBMS = vsi_c_open (VSI_CALLER "USBMS" )) < VSI_OK)
        return PEI_ERROR;
    }

    /* Create instance gathering all the variable used by EXPL instance */

    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_USBMS_ENV_CTRL_BLK),
                   (T_RVF_BUFFER**)&usbms_env_ctrl_blk_p) != RVF_GREEN)
    {
            /* The environemnt will cancel the EXPL instance creation. */
            RVM_TRACE_DEBUG_HIGH ("USBMS: Error to get memory ");
            return RVM_MEMORY_ERR;
    }

    /* Store the address ID. */
    usbms_env_ctrl_blk_p->addr_id = handle;

    /* Store the pointer to the error function. */
    usbms_env_ctrl_blk_p->error_ft = rvm_error;

    /* Store the mem bank id. */
    usbms_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;
    usbms_env_ctrl_blk_p->sec_mb_id = EXT_MEM_POOL;

    xfer_buffer = &usbMsXferBuffer;

    /* Reference temporary buffer in SRAM */
    usbms_env_ctrl_blk_p->usbms_specific.buffer_rx_temp.buff = xfer_buffer->temp_buff;

    for(i = 0; i < MAX_LUN_NUM; i++)
    {
    T_USBMS_XFER    *request    = NULL;

        request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[i].read_request;
        request->data_buff      = xfer_buffer->data_xfer_buff.buffer[0];

/*----->>Initialise new data structure------>>*/
        request->data_read_buff                     = &(xfer_buffer->data_xfer_buff);
        request->data_read_buff->buffer_status[0]    = AVAILABLE;
        request->data_read_buff->buffer_status[1]    = AVAILABLE;
        request->data_read_buff->usb_op_buf_index    = 0;
        request->data_read_buff->gbi_op_buf_index    = 0;
/*-----<<Initialise new data structure------<<*/

        request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[i].write_request;
        request->data_read_buff = NULL;
		/* Overusing The read Buffer , here it is used for writing. */
        request->data_buff = xfer_buffer->data_xfer_buff.buffer[0];
    }

    /*
     * return_path of linked SWE:
     * This is an efficient way to get the return path of the linked SWE in order
     * to immediately communicate with them. However since the address id is not
     * needed for SWE providing bridge functions as API, this is not always usefull.
     */
    /* return_path... */
    usbms_env_ctrl_blk_p->return_path.callback_func = NULL;
    usbms_env_ctrl_blk_p->return_path.addr_id       = handle;

#ifdef RVM_MC_SWE
    rvf_delay(RVF_SECS_TO_TICKS(3));
#endif

    usbms_init();

    usbms_start();

    RVM_TRACE_DEBUG_HIGH("USBMS: pei_init Complete");

    return (PEI_OK);
}


/*
+------------------------------------------------------------------------------
| Function    : pei_create
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when the process is
|                created.
|
| Parameters  : out_name          - Pointer to the buffer in which to locate
|                                   the name of this entity
|
| Return      : PEI_OK            - entity created successfuly
|               PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{
static const T_PEI_INFO pei_info =
              {
               "USBMS",
               {
                 pei_init,
                 pei_exit,
                 NULL, // pei_primitive, /* pei_primitive */
                 NULL,          /* pei_timeout */
                 NULL,          /* pei_signal */
                 pei_run,       /* pei_run */
                 NULL,          /* pei_config */
                 NULL           /* pei_monitor */
               },
               USBMS_STACK_SIZE,    /* stack size */
               20,                  /* queue entries */
   //            10,                  /* queue entries */
               (255 - USBMS_TASK_PRIORITY),  /* priority (1->low, 255->high) */
               0,        /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND // | PASSIVE_BODY
               /* Flags Settings */
              };

  RVM_TRACE_DEBUG_HIGH("USBMS: pei_create");
  /*
   * Close Resources if open
   */
  if (first_access)
    first_access = FALSE;
  else
    pei_exit();

  /*
   * Export startup configuration data
   */
  *info = (T_PEI_INFO *) &pei_info;

  return PEI_OK;
}/* End pei_create(..) */


/*
+------------------------------------------------------------------------------
| Function    : usbms_init
+------------------------------------------------------------------------------
| Description : Called to initialize the USBMS SWE before creating the task
|               and calling usbms_start.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbms_init (void)
{
    /** Here the instance (usbms_env_ctrl_blk_p) could be initialised.*/
    USBMS_SEND_TRACE("USBMS: Entering in usbms_init() ", RV_TRACE_LEVEL_WARNING);
    usbms_env_ctrl_blk_p->interface_id = USBMS_INTERFACE_ID;

    /** Init standard Inquiry data  */
    usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.device                    = 0x00;
    usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.removable                 = 0x80;
    usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.version                   = 0x05;//0x02
    usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.rdf_hisup_naca            = 0x02;
    usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.additionnal_length        = 0x1f/*sizeof(T_USBMS_INQUIRY_DATA)-5*/;
    usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.protect_tpc_alua_acc_sccs = 0x00;
    usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.mchngr_multip_vs_es_bq    = 0x00;//0x02
    usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.vs_cmdque_linked          = 0x00;
    strcpy((char*)usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.vendor_id,"TI      ");
    strcpy((char*)usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.product_id,"Mobile Phone    ");
    strcpy((char*)usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.product_revision_level,"1000");


    /** Init vital product data for serial number   */
    /*
    usbms_env_ctrl_blk_p->usbms_specific.serial_number_page.device_type     = 0x0E;
    usbms_env_ctrl_blk_p->usbms_specific.serial_number_page.page_code       = 0x80;
    usbms_env_ctrl_blk_p->usbms_specific.serial_number_page.reserved        = 0x00;
    usbms_env_ctrl_blk_p->usbms_specific.serial_number_page.page_length     = sizeof(T_USBMS_SERIAL_PAGE)-4;
    strcpy((char*)usbms_env_ctrl_blk_p->usbms_specific.serial_number_page.serial_number,"xxxx-xxxx-xxxx");
    */

    /** Init vital product data for device ID   */
    /*
    usbms_env_ctrl_blk_p->usbms_specific.device_id_page.device_type = 0x0E;
    usbms_env_ctrl_blk_p->usbms_specific.device_id_page.page_code       = 0x83;
    usbms_env_ctrl_blk_p->usbms_specific.device_id_page.page_length_1   = 0x00;
    usbms_env_ctrl_blk_p->usbms_specific.device_id_page.page_length_0   = sizeof(T_USBMS_DEVICE_ID_PAGE)-4;
    usbms_env_ctrl_blk_p->usbms_specific.device_id_page.ascii_id_descriptor[0].code_protocol    = 0x02;
    usbms_env_ctrl_blk_p->usbms_specific.device_id_page.ascii_id_descriptor[0].IDtype_assoc_piv = 0x00;
    usbms_env_ctrl_blk_p->usbms_specific.device_id_page.ascii_id_descriptor[0].reserved_1           = 0x00;
    usbms_env_ctrl_blk_p->usbms_specific.device_id_page.ascii_id_descriptor[0].identifier_length    = sizeof(T_USBMS_ID_DESCRIPTOR)-4;
    strcpy((char*)usbms_env_ctrl_blk_p->usbms_specific.device_id_page.ascii_id_descriptor[0].ascii_id,"xxxx-xxxx-xxxx");
    */

    /** Init the T_USBMS_MODE_PARAMETER_HEAD    */
    usbms_env_ctrl_blk_p->usbms_specific.mode_parameter_head.mode_data_length           = 2*sizeof(T_USBMS_MODE_PARAMETER_HEAD)-1;
    usbms_env_ctrl_blk_p->usbms_specific.mode_parameter_head.block_descriptor_length    = 0;
    usbms_env_ctrl_blk_p->usbms_specific.mode_parameter_head.device_specific_parameter  = 0;
    usbms_env_ctrl_blk_p->usbms_specific.mode_parameter_head.medium_type                = 0;
    usbms_env_ctrl_blk_p->usbms_specific.media_number                                   = 0;
    usbms_env_ctrl_blk_p->media_change_info                                             = 0;


    memset(Tab,0,NB_ELEMENT*sizeof(T_RV_HDR*));
    index = 0;

    return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : usbms_start
+------------------------------------------------------------------------------
| Description : Called to start the USBMS SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbms_start(void)
{
    T_RV_RET                    status      = RVM_OK;
    T_GBI_MEDIA_INFO_RSP_MSG    *media_info = NULL;
    UINT32                      message;

    USBMS_SEND_TRACE("USBMS: start called", RV_TRACE_LEVEL_DEBUG_LOW);

/*Added to ensure the GBI is configured*/
/*
    while(gbi_env_ctrl_blk_p->state != GBI_OPERATIONAL)
    {
        rvf_wait (0, 40);
    }
*/
    /** the first internal state    */
    usbms_change_internal_state(INIT);
    usbms_change_internal_state(INIT);

    /** Let the system start    */
    //rvf_delay(RVF_MS_TO_TICKS(1000));

    /** ask for the gbi media info  */
    //status = gbi_get_media_info(  usbms_env_ctrl_blk_p->prim_mb_id,
    //                              usbms_env_ctrl_blk_p->return_path   );
    status = usb_fm_subscribe(  usbms_env_ctrl_blk_p->interface_id,
                                usbms_env_ctrl_blk_p->return_path   );

    USBMS_SEND_TRACE("USBMS: Entity started ", RV_TRACE_LEVEL_WARNING);

    return status;
}


/*
+------------------------------------------------------------------------------
| Function    : usbms_stop
+------------------------------------------------------------------------------
| Description : Called to stop the USBMS SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbms_stop (T_RV_HDR * msg)
{
    /*
     * Here we should stop the activities of the SWE
     * It is still possible to send messages to other SWE, to unregister for example.
     */
    USBMS_SEND_TRACE("USBMS: stop called", RV_TRACE_LEVEL_DEBUG_LOW);

    /*
     * After everything has been stopped, we inform RVM that the SWE is ready to be
     * killed. Next step is RVM calling usbms_kill().
     */
//  rvm_swe_stopped(msg);

    return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : usbms_kill
+------------------------------------------------------------------------------
| Description : Called to stop the USBMS SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbms_kill (void)
{
    /*
     * Here we cannot send messages anymore. We only free the last
     * used resources, like the control block buffer.
     */
    USBMS_SEND_TRACE("USBMS: kill called", RV_TRACE_LEVEL_DEBUG_LOW);

    /** release the internal structure  */
    rvf_free_buf(usbms_env_ctrl_blk_p);

    return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : usbms_change_internal_state
+------------------------------------------------------------------------------
| Description : Called to change the USB driver state
|
| Parameters  : T_USB_INTERNAL_STATE :  new state
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
void usbms_change_internal_state(T_USBMS_INTERNAL_STATE state)
{
    USBMS_SEND_TRACE("USBMS: usbms_change_internal_state", RV_TRACE_LEVEL_DEBUG_LOW);
    /** Store the previous internal_state   */
    usbms_env_ctrl_blk_p->previous_state    = usbms_env_ctrl_blk_p->state;
    /** sets the new internal state */
    usbms_env_ctrl_blk_p->state             = state;
    if ((state == IDLE)||(state == UNPLUGGED))
    {
        process_stored_message();
    }
}


/*
+------------------------------------------------------------------------------
| Function    : usbms_store_message
+------------------------------------------------------------------------------
| Description : Called to store the message received
|
| Parameters  : message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
void usbms_store_message (T_RV_HDR *msg_p)
{
    /*
    if (    (msg_p->msg_id != USB_BUS_SUSPEND_MSG)
            &&(msg_p->msg_id != USB_BUS_RESUME_MSG)
            &&(msg_p->msg_id != USB_STATUS_READY_MSG)
            &&(msg_p->msg_id != USB_HOST_RESET_MSG) )
    {
        USBMS_SEND_TRACE("USBMS: usbms_store_message", RV_TRACE_LEVEL_DEBUG_LOW);
        Tab[index] = msg_p;
        index++;
    }
    */
}


/*
+------------------------------------------------------------------------------
| Function    : process_stored_message
+------------------------------------------------------------------------------
| Description : Called to process the stored message
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
void process_stored_message(void)
{
    /*
    UINT8       counter;
    T_RV_HDR*   msg_p;

    while (Tab[0] != NULL)
    {
        msg_p = Tab[0];
        for (counter=0 ; counter < NB_ELEMENT-1 ; counter++)
        {
            Tab[counter]=Tab[counter+1];
        }
        index--;
        USBMS_SEND_TRACE("USBMS: process_stored_message", RV_TRACE_LEVEL_DEBUG_LOW);
        usbms_handle_message(msg_p);
    }
    */
}

