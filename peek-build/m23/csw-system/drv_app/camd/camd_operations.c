#define AGILENT 0
#define MICRON 1

#if (CAM_SENSOR==AGILENT)
/**
 * @file  camd_operations.c
 *
 * CORE for CAMD SWE.
 *
 * @author  raymond zandbergen
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  6/12/2003 raymond zandbergen (ICT)    Create.
 *  27/05/2005  Venugopa Naik
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "camd/camd_i.h"
#include "camd/camd_operations.h"
#include "camd/camd_commands.h"
#include "camd/camd_env.h"

#if (CHIPSET ==15)
#include "dma/dma_api.h"
#include "dma/dma_message.h"
#include "camd/camcore/camcore_api.h"
#include "camd/camcore/camcore_hwapi.h"
#include "pin_config.h"
#if (WCP_PROF==1)
#include "../chipsetsw/Services/prf/prf_api.h"
#endif
#include "Armio.h"

//extern void camcore_readReg(void );

/*External declarations*/
extern T_CAMCORE_RET CameraCore_Clear_FIFO(void);
extern void read_commands_camera(void);
extern void camd_get_frame(void);

/*for debug information*/
extern UINT8 Overflow_flag;
extern UINT8 Underflow_flag;
extern T_CAMD_ENCODING camd_encoding;

//void camd_change_fps(UINT8);


static BOOL camd_dma_wait_status_OK (T_DMA_CHANNEL *channel);



#define FPS 1


/* FIFO Threshold size */

#define CAMCORE_FIFO_THRESHOLD 64
//#define CAM_DMA_CHANNEL C_DMA_CHANNEL_4



#define VIDEO 1
#define SNAPSHOT 0

extern UINT8 I2C_complete;

T_CAMCORE_CONFIGPARAM camcore_viewfinder_params = {
   CAMCORE_VIEWFINDER,
   CAMCORE_FIFO_THRESHOLD,
   0,
   CAMCORE_CCP_PARNOBT_8,
   4
};

T_CAMCORE_CONFIGPARAM camcore_snapshot_params  = {
   CAMCORE_SNAPSHOT,
   CAMCORE_FIFO_THRESHOLD,
   0,
   CAMCORE_CCP_PARNOBT_8,
   4
};


T_DMA_CHANNEL camd_available_dma_channel=0xff;

CAM_SENSOR_CAPABILITIES cam_current_sensor_capabilities=
{

#if (CAM_SENSOR==AGILENT)
       CAM_FEATURE_NOT_SUPPORTED,
        CAM_FEATURE_SUPPORTED,
        CAM_SENSOR_AGILENT,
        CAMD_VGA,
        640,
        480,
        T_CAM_NO_EFFECT
          

#else
        CAM_ZOOM_SUPPORTED,
        CAM_MIRRORING_SUPPORTED,
        CAM_SENSOR_MICRON,  
        CAMD_SXGA,
        1280,
        1024
#endif    



};

#undef _FPS_


#endif


static BOOL viewfinder_parameters_OK (T_CAMD_PARAMETERS * p);
static BOOL snapshot_parameters_OK (T_CAMD_PARAMETERS * p);

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_enable_sensor                                   */
/*                                                                              */
/*    Purpose:                                                                  */
/*                            If sensor is to be enabled,                       */
/*                                1. Resets camera core,                        */
/*                                2. Enables clock to sensor                    */
/*                            If sensor is to be disabled,                      */
/*                                1. Disables camera core,                      */
/*                                2. Disables clock to sensor                   */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p : A CAMD message.                                   */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*      None                                                                    */
/********************************************************************************/
    T_RV_RET
camd_int_enable_sensor (T_CAMD_MSG * camd_msg_p)
{

    T_RVF_RET ret_val;
    CAMD_SEND_TRACE ("CAMD CORE entering camd_int_enable_sensor",
            RV_TRACE_LEVEL_DEBUG_HIGH);
    camd_msg_p->status = CAMD_OK;

    switch (camd_msg_p->body.enable_sensor)
    {
        case TRUE:
            if(camd_env_ctrl_blk_p->sensor_enabled == FALSE)
            {
#if (CHIPSET==15)
#define  I_sample 27
#if I_sample==27
                CONF_GPIO_10	 = MUX_CFG(1, PULLOFF);
                AI_ConfigBitAsOutput(10);
                AI_ResetBit(10);
#else

                CONF_GPIO_17	 = MUX_CFG(0, PULLOFF);
                AI_ConfigBitAsOutput(17);
                AI_ResetBit(17);



#endif

                F_INTH_ENABLE_ONE_IT(10) ; 

                if (camcore_reset(CAMCORE_RESET_ALL)!=CAMCORE_OK)
                {
                    camd_msg_p->status = CAMD_INTERNAL_ERR;
                    break;
                }

                if(camcore_config(&camcore_snapshot_params) != CAMCORE_OK)
                {
                    camd_msg_p->status = CAMD_INVALID_PARAMETER;
                    break;
                }
                camcore_disable();

#endif

                camd_init_sensor_clock ();
                /* allow time for sensor startup */
                rvf_delay (20);


                camd_init_sensor ();

#if CHIPSET == 15
                if(!I2C_complete)
                {
                    CAMD_SEND_TRACE ("Camera Sensor is not responding: ", RV_TRACE_LEVEL_ERROR);
                    camd_msg_p->status = CAMD_INTERNAL_ERR;
                    break;
                }
#endif
                camd_start_sensor_clock();

#if CHIPSET == 15
                camd_return_queue_flush ();
                if (RV_OK != dma_reserve_channel (DMA_CHAN_ANY,
                            0,
                            DMA_QUEUE_DISABLE,
                            0,
                            camd_env_ctrl_blk_p->path_to_return_queue)
                        || !camd_dma_wait_status_OK (&camd_available_dma_channel))
                {
                    CAMD_SEND_TRACE ("camd_int_enable_sensor: "
                            "dma_reserve_channel failed", RV_TRACE_LEVEL_ERROR);
                    camd_msg_p->status = CAMD_INTERNAL_ERR;
                    break;
                }
#endif

                camd_env_ctrl_blk_p->sensor_enabled = TRUE;
                camd_env_ctrl_blk_p->state = CAMD_CAMERA_ENABLED;

                CAMD_SEND_TRACE ("CAMD CORE entering camd_int_enable_sensor DONE",
                        RV_TRACE_LEVEL_DEBUG_HIGH);
            }
            break;

        case FALSE:

            if(camd_env_ctrl_blk_p->sensor_enabled == TRUE)
            {
                camd_env_ctrl_blk_p->sensor_enabled = FALSE;
                camd_env_ctrl_blk_p->state = CAMD_CAMERA_DISABLED;


#if I_sample==27
                AI_ConfigBitAsOutput(10);
                AI_SetBit(10);
#else
                AI_ConfigBitAsOutput(17);
                AI_SetBit(17); 
#endif
                camd_stop_sensor_clock();
#if CHIPSET == 15
                /* release DMA channel */
                if (RV_OK != dma_release_channel (camd_available_dma_channel)
                        || !camd_dma_wait_status_OK (NULL))
                {

                    CAMD_SEND_TRACE ("camd_int_enable_sensor: "
                            "dma_release_channel failed", RV_TRACE_LEVEL_ERROR);
                    camd_msg_p->status = CAMD_INTERNAL_ERR;
                }
                camd_available_dma_channel=0xff;
                camd_return_queue_flush ();
#endif
                CAMD_SEND_TRACE ("CAMD sensor disabled", RV_TRACE_LEVEL_DEBUG_HIGH);
            }
            break;
        default:
            camd_msg_p->status = CAMD_INVALID_PARAMETER;
    }

    return camd_send_response_to_client (CAMD_REGISTERCLIENT_REQ_MSG, camd_msg_p);
}
/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_set_snapshot_parameters                         */
/*                                                                              */
/*    Purpose:     Sets snapshot parameters.                                    */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    T_RV_RET
camd_int_set_snapshot_parameters (T_CAMD_MSG * msg_p)
{
    CAMD_SEND_TRACE ("snapshot_parameters",
            RV_TRACE_LEVEL_ERROR);
    msg_p->status = CAMD_OK;
    if (!camd_env_ctrl_blk_p->sensor_enabled)
    {
        msg_p->status = CAMD_NOT_READY;
    }
    else
    {
        camd_env_ctrl_blk_p->snapshot_parameters =
            msg_p->body.configparams;

        camd_write_snapshot_gamma_correction_to_camera(
                camd_env_ctrl_blk_p->snapshot_parameters.gamma_correction);
        /*Change dimensions and set format RGB,YUv etc*/
        camd_change_snapshot_imagedim(msg_p->body.configparams.imagewidth, msg_p->body.configparams.imageheight, msg_p->body.configparams.encoding,msg_p->body.configparams.flip_x,msg_p->body.configparams.flip_y,msg_p->body.configparams.zoom);


        camd_change_fps(VIDEO); 

        camd_env_ctrl_blk_p->snapshot_parameters_valid = TRUE;


        if(camcore_config(&camcore_snapshot_params) != CAMCORE_OK)
            msg_p->status = CAMD_INVALID_PARAMETER;

    }
    camd_env_ctrl_blk_p->state = CAMD_CAMERA_ENABLED;

    return camd_send_response_to_client (CAMD_SET_CONFIGPARAMS_REQ_MSG, msg_p);
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_get_snapshot_parameters                         */
/*                                                                              */
/*    Purpose:     Gets snapshot parameters.                                    */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    T_RV_RET
camd_int_get_snapshot_parameters (T_CAMD_MSG * msg_p)
{
    CAMD_SEND_TRACE ("CAMD CORE entering camd_int_get_snapshot_parameters",
            RV_TRACE_LEVEL_DEBUG_HIGH);
    msg_p->status = CAMD_OK;
    if (!camd_env_ctrl_blk_p->sensor_enabled
            || !camd_env_ctrl_blk_p->snapshot_parameters_valid)
    {
        msg_p->status = CAMD_NOT_READY;
    }
    else
    {
        msg_p->body.configparams =
            camd_env_ctrl_blk_p->snapshot_parameters;
    }
    camd_env_ctrl_blk_p->state = CAMD_CAMERA_ENABLED;

    return camd_send_response_to_client (CAMD_GET_CONFIGPARAMS_RSP_MSG,
            msg_p);
}




/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_get_snapshot                                    */
/*                                                                              */
/*    Purpose:     Takes a snapshot.                                            */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/


    T_RV_RET
camd_int_get_snapshot (T_CAMD_MSG * camd_msg_p)
{
    T_RV_RET ret_val = CAMD_TRANSFER_COMPLETE;
    const UINT16 snapshot_width =
        camd_env_ctrl_blk_p->snapshot_parameters.imagewidth;


    const UINT16 snapshot_height =
        camd_env_ctrl_blk_p->snapshot_parameters.imageheight;


    const UINT16 snapshot_pixel_size =2;
     /*   (2 * (camd_camera_capabilities.resolution[CAMD_VGA].Y_bits +
              camd_camera_capabilities.resolution[CAMD_VGA].U_bits +
              camd_camera_capabilities.resolution[CAMD_VGA].V_bits)) >> 4;
*/
    T_DMA_CHANNEL_PARAMETERS dma_channel_parameters;

    T_DMA_STATUS_RSP_MSG *dma_status_rsp_msg;

    UINT32 bytes_expected = snapshot_width * snapshot_height * snapshot_pixel_size;

    T_RV_RETURN_PATH return_path;
#if(FPS ==1)
    UINT32 t1,t2;
    t1 = rvf_get_tick_count();
#endif

#if(WCP_PROF==1)
    prf_LogPointOfInterest("SnapShot start");
#endif
    camd_env_ctrl_blk_p->state = CAMD_SNAPSHOT_MODE;

    CAMD_SEND_TRACE ("get_snapshot",
            RV_TRACE_LEVEL_ERROR);

    if (!camd_env_ctrl_blk_p->sensor_enabled
            || !camd_env_ctrl_blk_p->snapshot_parameters_valid)
    {
        camd_msg_p->status = CAMD_NOT_READY;
        return camd_send_response_to_client (CAMD_SNAPSHOT_DATA_RSP_MSG,
                camd_msg_p);
    }

    do
    {
        camd_return_queue_flush();

        /* Set DMA parameters */
        dma_channel_parameters.data_width = DMA_DATA_S32;
        dma_channel_parameters.sync = DMA_SYNC_DEVICE_CAM_THRESHOLD;
        dma_channel_parameters.hw_priority = DMA_HW_PRIORITY_HIGH;

        dma_channel_parameters.flush = DMA_FLUSH_DISABLED;

        dma_channel_parameters.nmb_frames =
            (bytes_expected/(camcore_snapshot_params.fifothreshold*4));

        dma_channel_parameters.nmb_elements =
            (camcore_snapshot_params.fifothreshold);

        dma_channel_parameters.dma_end_notification = DMA_NOTIFICATION;
        dma_channel_parameters.secure = DMA_NOT_SECURED;
        dma_channel_parameters.transfer = DMA_MODE_TRANSFER_ENABLE;
        dma_channel_parameters.dma_mode = DMA_MODE_SINGLE;
        dma_channel_parameters.source_address = (UINT32) CC_FIFODATA;;
        dma_channel_parameters.source_address_mode = DMA_ADDR_MODE_CONSTANT;
        dma_channel_parameters.source_packet = DMA_NOT_PACKED;
        dma_channel_parameters.source_burst = DMA_NO_BURST;

        dma_channel_parameters.destination_address =     (UINT32) (camd_msg_p->body.buff);
        dma_channel_parameters.destination_address_mode = DMA_ADDR_MODE_POST_INC;
        dma_channel_parameters.destination_packet = DMA_NOT_PACKED;
        dma_channel_parameters.destination_burst = DMA_NO_BURST;


        CameraCore_Clear_FIFO();

        if (RV_OK != dma_set_channel_parameters (camd_available_dma_channel,
                    &dma_channel_parameters) ||
                !camd_dma_wait_status_OK (NULL))
        {
            CAMD_SEND_TRACE ("camd_int_get_viewfinder_frames: "
                    "dma_set_channel_parameters failed",
                    RV_TRACE_LEVEL_ERROR);
            ret_val = CAMD_INTERNAL_ERR;
            break;
        }

        camcore_setmode(CAMCORE_SNAPSHOT);

#if(WCP_PROF==1)
        prf_LogPointOfInterest("I2C START start");
#endif
        camd_switch_snapshot ();

        dma_status_rsp_msg = (T_DMA_STATUS_RSP_MSG *)camd_return_queue_get_msg ();
        if(dma_status_rsp_msg == NULL)
        {
            CAMD_SEND_TRACE ("Null message recived WAITING AGAIN",
                    RV_TRACE_LEVEL_ERROR);
            dma_status_rsp_msg = (T_DMA_STATUS_RSP_MSG *)camd_return_queue_get_msg ();
        }

        /* complain if not a DMA_COMPLETED message */
        if (dma_status_rsp_msg->hdr.msg_id != DMA_STATUS_RSP_MSG
                || dma_status_rsp_msg->result.status != DMA_COMPLETED  )
        {
            CAMD_SEND_TRACE
                ("camd_int_get_viewfinder: from dma received unexpected message",
                 RV_TRACE_LEVEL_ERROR);

            if (RVF_OK != rvf_free_buf (dma_status_rsp_msg))
            {
                CAMD_SEND_TRACE ("CAMD DMA could not free DMA status msg",
                        RV_TRACE_LEVEL_ERROR);
            }
            ret_val = CAMD_INTERNAL_ERR;
            break;
        }

        if (RVF_OK != rvf_free_buf (dma_status_rsp_msg))
        {
            CAMD_SEND_TRACE ("CAMD DMA could not free DMA status msg",
                    RV_TRACE_LEVEL_ERROR);
            ret_val = CAMD_INTERNAL_ERR;
            break;
        }

    }
    while (0);
    camcore_disable();
    camcore_reset(CAMCORE_RESET_FSM);

#if(FPS ==1)
    t2 = rvf_get_tick_count();
    CAMD_SEND_TRACE_PARAM("Snapshot Time = ",RVF_TICKS_TO_MS((t2-t1)), RV_TRACE_LEVEL_DEBUG_HIGH);
#endif

    camd_msg_p->status = ret_val;

    CAMD_SEND_TRACE ("CAMD CORE leaving camd_int_get_snapshot",
            RV_TRACE_LEVEL_DEBUG_HIGH);


#if(WCP_PROF==1)
    prf_LogPointOfInterest("SnapShot end");
#endif


    CAMD_SEND_TRACE_PARAM("snap ret buf",camd_msg_p->body.buff,RV_TRACE_LEVEL_ERROR);
    return camd_send_response_to_client (CAMD_SNAPSHOT_DATA_RSP_MSG,
            camd_msg_p);


}




/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_set_viewfinder_parameters                       */
/*                                                                              */
/*    Purpose:     Sets viewfinder parameters.                                  */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    T_RV_RET
camd_int_set_viewfinder_parameters (T_CAMD_MSG * msg_p)
{

    CAMD_SEND_TRACE ("viewfinder_parameters",
            RV_TRACE_LEVEL_ERROR);
    msg_p->status = CAMD_OK;
    camd_encoding = msg_p->body.configparams.encoding;

    if (!camd_env_ctrl_blk_p->sensor_enabled)
    {
        msg_p->status = CAMD_NOT_READY;
    }
    else
    {
        camd_env_ctrl_blk_p->viewfinder_parameters =
            msg_p->body.configparams;

        camd_write_viewfinder_gamma_correction_to_camera (camd_env_ctrl_blk_p->viewfinder_parameters.gamma_correction);



        camd_env_ctrl_blk_p->viewfinder_parameters_valid = TRUE;

        /*Changes image dimensions and selects image format RGB.*/
        camd_change_viewfinder_imagedim(msg_p->body.configparams.imagewidth,
                msg_p->body.configparams.imageheight,
                msg_p->body.configparams.encoding,
                msg_p->body.configparams.flip_x,
                msg_p->body.configparams.flip_y,
                msg_p->body.configparams.zoom);

        camd_change_fps(VIDEO);


        /*Just set the config bit to update detailed registers
        */
#if (CHIPSET==15)
        if(camcore_config(&camcore_viewfinder_params) != CAMCORE_OK)
            msg_p->status = CAMD_INVALID_PARAMETER;
#endif

    }

    camd_env_ctrl_blk_p->state = CAMD_CAMERA_ENABLED;

    return camd_send_response_to_client (CAMD_SET_CONFIGPARAMS_REQ_MSG, msg_p);
}
/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_get_viewfinder_parameters                       */
/*                                                                              */
/*    Purpose:     gets viewfinder parameters.                                  */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    T_RV_RET
camd_int_get_viewfinder_parameters (T_CAMD_MSG * msg_p)
{
    CAMD_SEND_TRACE ("CAMD CORE entering camd_int_get_viewfinder_parameters",
            RV_TRACE_LEVEL_DEBUG_HIGH);
    msg_p->status = CAMD_OK;
    if (!camd_env_ctrl_blk_p->sensor_enabled
            || !camd_env_ctrl_blk_p->viewfinder_parameters_valid)
    {
        msg_p->status = CAMD_NOT_READY;
    }
    else
    {
        msg_p->body.configparams =
            camd_env_ctrl_blk_p->viewfinder_parameters;
    }

    camd_env_ctrl_blk_p->state = CAMD_CAMERA_ENABLED;

    return camd_send_response_to_client (CAMD_GET_VIEWFINDER_PARAMETERS_RSP_MSG,
            msg_p);
}




/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_get_viewfinder_frames                           */
/*                                                                              */
/*    Purpose:     Sets camera in viewfinder mode.                              */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    T_RV_RET
camd_int_get_viewfinder_frames (T_CAMD_MSG * camd_msg_p)
{
    const UINT16 viewfinder_width =
        camd_env_ctrl_blk_p->viewfinder_parameters.imagewidth;


    const UINT16 viewfinder_height =
        camd_env_ctrl_blk_p->viewfinder_parameters.imageheight;


    const UINT16 viewfinder_pixel_size =2;
       /* (camd_camera_capabilities.resolution[CAMD_QCIF].R_bits +
         camd_camera_capabilities.resolution[CAMD_QCIF].G_bits +
         camd_camera_capabilities.resolution[CAMD_QCIF].B_bits) >> 3;*/


    T_RV_HDR *dma_rsp_msg;
    T_DMA_STATUS_RSP_MSG *dma_status_rsp_msg;
    T_CAMD_MSG *camd_bitmap_rsp_msg_p;
    T_RV_RETURN_PATH return_path;

    T_RV_RET ret_val = RV_OK;
    T_CAMD_MSG *camd_new_request_p = NULL;
    T_DMA_CHANNEL_PARAMETERS dma_channel_parameters;

    UINT16 event;

    UINT32 bytes_expected = viewfinder_width * viewfinder_height
        * viewfinder_pixel_size;
#if(FPS ==1)
    UINT32 t1,t2;
    t1 = rvf_get_tick_count();
#endif

    camd_env_ctrl_blk_p->state = CAMD_VIEWFINDER_MODE;
    CAMD_SEND_TRACE ("get_viewfinder_frames",
            RV_TRACE_LEVEL_ERROR);

    if (!camd_env_ctrl_blk_p->sensor_enabled)
    {
        camd_msg_p->status = CAMD_NOT_READY;
        return camd_send_response_to_client (CAMD_VIEWFINDER_DATA_RSP_MSG,
                camd_msg_p);
    }


    do{


       camd_return_queue_flush();

       /* Set DMA parameters */
       dma_channel_parameters.data_width = DMA_DATA_S32;
       dma_channel_parameters.sync = DMA_SYNC_DEVICE_CAM_THRESHOLD;
       dma_channel_parameters.hw_priority = DMA_HW_PRIORITY_HIGH;

       dma_channel_parameters.flush = DMA_FLUSH_DISABLED;
       dma_channel_parameters.nmb_frames =
           (bytes_expected/(camcore_viewfinder_params.fifothreshold*4));
       dma_channel_parameters.nmb_elements =
           (camcore_viewfinder_params.fifothreshold);

       dma_channel_parameters.dma_end_notification = DMA_NOTIFICATION;
       dma_channel_parameters.secure = DMA_NOT_SECURED;
       dma_channel_parameters.transfer = DMA_MODE_TRANSFER_ENABLE;
       dma_channel_parameters.dma_mode = DMA_MODE_SINGLE;
       dma_channel_parameters.source_address = (UINT32) CC_FIFODATA;;
       dma_channel_parameters.source_address_mode = DMA_ADDR_MODE_CONSTANT;
       dma_channel_parameters.source_packet = DMA_NOT_PACKED;
       dma_channel_parameters.source_burst = DMA_NO_BURST;

       dma_channel_parameters.destination_address = (UINT32) (camd_msg_p->body.buff);

       dma_channel_parameters.destination_address_mode = DMA_ADDR_MODE_POST_INC;
       dma_channel_parameters.destination_packet = DMA_NOT_PACKED;
       dma_channel_parameters.destination_burst = DMA_NO_BURST;


       camcore_setmode(CAMCORE_VIEWFINDER);
       CameraCore_Clear_FIFO();

       rvf_lock_mutex (&camd_env_ctrl_blk_p->framebuf_mutex);

       /* allocate camd response message */

       camd_msg_p->status = CAMD_TRANSFER_COMPLETE;
       if (RV_OK != dma_set_channel_parameters (camd_available_dma_channel,
                   &dma_channel_parameters) ||
               !camd_dma_wait_status_OK (NULL))
       {
           CAMD_SEND_TRACE ("camd_int_get_viewfinder_frames: "
                   "dma_set_channel_parameters failed",
                   RV_TRACE_LEVEL_ERROR);
           ret_val = CAMD_INTERNAL_ERR;
           break;
       }

       if(Underflow_flag)
       {
           CAMD_SEND_TRACE_PARAM("Underflow!!",Underflow_flag,RV_TRACE_LEVEL_ERROR);
           Underflow_flag = 0;
       }
       if(Overflow_flag)
       {
           CAMD_SEND_TRACE_PARAM("Overflow!!",Overflow_flag,RV_TRACE_LEVEL_ERROR);
           Overflow_flag = 0;

       }

       camcore_enable();
       /*Get the response message*/
       camd_switch_preview ();

    }while(0);




    return RV_OK;
}
/* END camd_int_get_viewfinder_frames */


/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_dma_wait_status_OK                                  */
/*                                                                              */
/*    Purpose:      Blocks camd waiting for a dma return queue message.         */
/*                                                                              */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - channel : dma channel number.                                  */
/*    Return :                                                                  */
/*            TRUE/FALSE                                                        */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/

    static BOOL
camd_dma_wait_status_OK (T_DMA_CHANNEL *channel)
{
    T_DMA_STATUS_RSP_MSG *dma_status_rsp_msg = camd_return_queue_get_msg ();
    BOOL ret_val = TRUE;

    switch (dma_status_rsp_msg->result.status)
    {
        case DMA_RESERVE_OK:
            if (channel)
                *channel=dma_status_rsp_msg->result.channel;
            /* fall through */
        case DMA_OK:
        case DMA_PARAM_SET:
        case DMA_COMPLETED:
        case DMA_CHANNEL_RELEASED:
        case DMA_CHANNEL_ENABLED:
            /* status OK: do nothing */
            break;
        default:
            CAMD_SEND_TRACE_PARAM ("CAMD DMA error status received: ",
                    dma_status_rsp_msg->result.status,
                    RV_TRACE_LEVEL_ERROR);
            ret_val = FALSE;
    }                            /* end switch */

    if (RVF_OK != rvf_free_buf (dma_status_rsp_msg))
    {
        CAMD_SEND_TRACE ("CAMD DMA could not free DMA status msg",
                RV_TRACE_LEVEL_ERROR);
    }
    return ret_val;
}




#else
/**
 * @file  camd_operations.c
 *
 * CORE for CAMD SWE.
 *
 * @author  raymond zandbergen
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  6/12/2003 raymond zandbergen (ICT)    Create.
 *  27/05/2005  Venugopa Naik
 *  11-Sept-2006 Magnus Aman
 *				Added Micron Sensor MT9x112 for Locosto only
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "camd/camd_i.h"
#include "camd/camd_operations.h"
#include "camd/camd_commands.h"
#include "camd/camd_env.h"
#if (WCP_PROF==1)
#include "../chipsetsw/Services/prf/prf_api.h"
#endif

#include "dma/dma_api.h"
#include "dma/dma_message.h"
#include "camd/camcore/camcore_api.h"
#include "camd/camcore/camcore_hwapi.h"

#include "Armio.h"
#include "Pin_config.h"

#define CAMD_MICRON_MT9x112
extern void camcore_readReg(void );
extern T_CAMCORE_RET CameraCore_Clear_FIFO(void);
extern void read_commands_camera(void);
extern void camd_get_frame(void);

extern UINT8 Overflow_flag;
extern UINT8 Underflow_flag;

void camd_change_fps(UINT8);



static BOOL camd_dma_wait_status_OK (T_DMA_CHANNEL *channel);


#define FPS 1




/* FIFO Threshold size */
#define CAMCORE_FIFO_THRESHOLD 16
//#define CAM_DMA_CHANNEL C_DMA_CHANNEL_4


extern UINT8 I2C_complete;

T_DMA_CHANNEL camd_available_dma_channel=0xFF;
const T_CAMCORE_CONFIGPARAM camcore_viewfinder_params = {
   CAMCORE_VIEWFINDER,
   CAMCORE_FIFO_THRESHOLD,
   0,
   CAMCORE_CCP_PARNOBT_8,
   31  
};

const T_CAMCORE_CONFIGPARAM camcore_snapshot_params  = {
   CAMCORE_SNAPSHOT,
   CAMCORE_FIFO_THRESHOLD,
   0,
   CAMCORE_CCP_PARNOBT_8,
   2
};


CAM_SENSOR_CAPABILITIES cam_current_sensor_capabilities= 
{

#if (CAM_SENSOR==AGILENT)
        CAM_FEATURE_NOT_SUPPROTED,
        CAM_FEATURE_SUPPORTED,
        CAM_SENSOR_AGILENT,
        CAMD_VGA,
        640,
        480,
        T_CAM_NO_EFFECT


#else
        CAM_FEATURE_SUPPORTED,
        CAM_FEATURE_SUPPORTED,
        CAM_SENSOR_MICRON,  
        CAMD_SXGA,
        1280,
        1024,
        T_CAM_SEPIA_EFFECT|T_CAM_MONOCHROME_EFFECT|T_CAM_NEGATIVE_EFFECT
        
#endif    
};




static BOOL viewfinder_parameters_OK (T_CAMD_PARAMETERS * p);
static BOOL snapshot_parameters_OK (T_CAMD_PARAMETERS * p);

void camd_init_fps(void)
{

    camd_write_sensor(0x005, 0x0296);     /* Context B (full-res); Horizontal Blank*/
    camd_write_sensor(0x006, 0x012D);     /* Context B (full-res); Vertical Blank*/
    camd_write_sensor(0x007, 0x0188);     /* Context A (preview); Horizontal Blank*/
    camd_write_sensor(0x008, 0x005D);     /* Context A (preview); Vertical Blank*/
    camd_write_sensor(0x020, 0x0100);     /* Read Mode Context B*/
    camd_write_sensor(0x021, 0x8400);     /* Read Mode Context A*/
    camd_write_sensor(0x022, 0x0D0F);     /* Dark col / rows*/
    camd_write_sensor(0x024, 0x8000);     /* Extra Reset*/
    camd_write_sensor(0x059, 0x00FF);     /* Black Rows*/
    camd_write_sensor(0x239, 0x07A0);     /* AE Line size Context A*/
    camd_write_sensor(0x23A, 0x079E);     /* AE Line size Context B*/
    camd_write_sensor(0x23B, 0x026E);     /* AE shutter delay limit Context A*/
    camd_write_sensor(0x23C, 0x03C2);     /* AE shutter delay limit Context B*/
    camd_write_sensor(0x257, 0x01BB);     /* Context A Flicker full frame time (60Hz);*/
    camd_write_sensor(0x258, 0x0214);     /* Context A Flicker full frame time (50Hz);*/
    camd_write_sensor(0x259, 0x01BC);     /* Context B Flicker full frame time (60Hz);*/
    camd_write_sensor(0x25A, 0x0215);     /* Context B Flicker full frame time (50Hz);*/
    camd_write_sensor(0x25C, 0x1F19);     /* 60Hz Flicker Search Range*/
    camd_write_sensor(0x25D, 0x241E);     /* 50Hz Flicker Search Range*/
    camd_write_sensor(0x264, 0x1E1C);     /* Flicker parameter*/
    camd_write_sensor(0x237, 0x8080);	  /*Upper zone = 4 to fix frame rat*/



}



/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_enable_sensor                                   */
/*                                                                              */
/*    Purpose:                                                                  */
/*                            If sensor is to be enabled,                       */
/*                                1. Resets camera core,                        */
/*                                2. Enables clock to sensor                    */
/*                            If sensor is to be disabled,                      */
/*                                1. Disables camera core,                      */
/*                                2. Disables clock to sensor                   */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p : A CAMD message.                                   */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*      None                                                                    */
/********************************************************************************/
    T_RV_RET
camd_int_enable_sensor (T_CAMD_MSG * camd_msg_p)
{

    T_RVF_RET ret_val;
    CAMD_SEND_TRACE ("CAMD CORE entering camd_int_enable_sensor",
            RV_TRACE_LEVEL_DEBUG_HIGH);
    camd_msg_p->status = CAMD_OK;

    switch (camd_msg_p->body.enable_sensor)
    {
        case TRUE:
            if(camd_env_ctrl_blk_p->sensor_enabled == FALSE)
            {

                {
                    camd_env_ctrl_blk_p->state = CAMD_CAMERA_ENABLED;

                    CAMD_SEND_TRACE ("Camera Init: ", RV_TRACE_LEVEL_ERROR);


#define I_sample 27

#if I_sample==27
                    CONF_GPIO_10	 = MUX_CFG(1, PULLOFF);
                    AI_ConfigBitAsOutput(10);
                    AI_ResetBit(10);
#else

                    CONF_GPIO_17	 = MUX_CFG(0, PULLOFF);
                    AI_ConfigBitAsOutput(17);
                    AI_ResetBit(17);

#endif
                    /*Assert nCAM_RST to sensor, pin already setup in Init.c*/
                    GPIO_CLEAR_OUTPUT(4); 




                    F_INTH_ENABLE_ONE_IT(10) ;

                    if (camcore_reset(CAMCORE_RESET_ALL)!=CAMCORE_OK)
                    {
                        camd_msg_p->status = CAMD_INTERNAL_ERR;
                        break;
                    }

                    if(camcore_config(&camcore_viewfinder_params) != CAMCORE_OK)
                    {
                        camd_msg_p->status = CAMD_INVALID_PARAMETER;
                        break;
                    }


                    camd_init_sensor_clock ();

                    rvf_delay (1);
                    GPIO_SET_OUTPUT(4);  /* Release camera reset*/

                    /* allow time for sensor startup */

                    rvf_delay (10);

                    CameraCore_Clear_FIFO();
                    camcore_enable();

                    camd_init_sensor ();


                    camd_init_fps();




                    if(I2C_complete==0)
                    {
                        CAMD_SEND_TRACE ("Camera Sensor is not responding: ", RV_TRACE_LEVEL_ERROR);
                        camd_msg_p->status = CAMD_INTERNAL_ERR;
                        break;
                    }
                    camd_start_sensor_clock();

                }

                if (RV_OK != dma_reserve_channel (DMA_CHAN_ANY,
                            0,
                            DMA_QUEUE_DISABLE,
                            0,
                            camd_env_ctrl_blk_p->path_to_return_queue)
                        || !camd_dma_wait_status_OK (&camd_available_dma_channel))
                {
                    CAMD_SEND_TRACE ("camd_int_enable_sensor: "
                            "dma_reserve_channel failed", RV_TRACE_LEVEL_ERROR);
                    camd_msg_p->status = CAMD_INTERNAL_ERR;
                    break;
                }

                camd_env_ctrl_blk_p->sensor_enabled = TRUE;
                camd_env_ctrl_blk_p->state = CAMD_CAMERA_ENABLED;

                CAMD_SEND_TRACE ("CAMD CORE entering camd_int_enable_sensor DONE",
                        RV_TRACE_LEVEL_DEBUG_HIGH);
            }
            break;

        case FALSE:

            if(camd_env_ctrl_blk_p->sensor_enabled == TRUE)
            {
                camd_env_ctrl_blk_p->sensor_enabled = FALSE;
                camd_env_ctrl_blk_p->state = CAMD_CAMERA_DISABLED;

                camcore_disable();

                camd_stop_sensor_clock();

                /* release DMA channel */
                if (RV_OK != dma_release_channel (camd_available_dma_channel)
                        || !camd_dma_wait_status_OK (NULL))
                {
                    CAMD_SEND_TRACE ("camd_int_enable_sensor: "
                            "dma_release_channel failed", RV_TRACE_LEVEL_ERROR);
                    camd_msg_p->status = CAMD_INTERNAL_ERR;
                }
                camd_return_queue_flush ();

                CAMD_SEND_TRACE ("CAMD sensor disabled", RV_TRACE_LEVEL_DEBUG_HIGH);
            }
            break;
        default:
            camd_msg_p->status = CAMD_INVALID_PARAMETER;
    }

    return camd_send_response_to_client (CAMD_REGISTERCLIENT_REQ_MSG, camd_msg_p);
}


T_CAMD_PARAMETERS camd_current_snapshot_parameters;
/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_set_snapshot_parameters                         */
/*                                                                              */
/*    Purpose:     Sets snapshot parameters.                                    */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    T_RV_RET
camd_int_set_snapshot_parameters (T_CAMD_MSG * msg_p)
{
    CAMD_SEND_TRACE ("CAMD CORE entering camd_int_set_snapshot_parameters",
            RV_TRACE_LEVEL_DEBUG_HIGH);
    msg_p->status = CAMD_OK;
    if (!camd_env_ctrl_blk_p->sensor_enabled)
    {
        msg_p->status = CAMD_NOT_READY;
    }
    else
    {

		camd_env_ctrl_blk_p->snapshot_parameters =msg_p->body.configparams;

		 camd_current_snapshot_parameters=msg_p->body.configparams;
/*
        camd_current_snapshot_parameters.black_and_white=camd_current_snapshot_parameters.black_and_white;
camd_current_snapshot_parameters.capturemode=camd_current_snapshot_parameters.capturemode;
camd_current_snapshot_parameters.encoding=camd_current_snapshot_parameters.encoding;
camd_current_snapshot_parameters.flip_x=camd_current_snapshot_parameters.flip_x;
camd_current_snapshot_parameters.flip_y=camd_current_snapshot_parameters.flip_y;
camd_current_snapshot_parameters.gamma_correction=camd_current_snapshot_parameters.gamma_correction
camd_current_snapshot_parameters.imageheight=camd_current_snapshot_parameters.imageheight
camd_current_snapshot_parameters.imagewidth=camd_current_snapshot_parameters.imagewidth;
camd_current_snapshot_parameters.mode=camd_current_snapshot_parameters.mode;
camd_current_snapshot_parameters.resolution=camd_current_snapshot_parameters.resolution;
camd_current_snapshot_parameters.rotate=camd_current_snapshot_parameters.rotate;
camd_current_snapshot_parameters.start_transfer_cb=camd_current_snapshot_parameters.start_transfer_cb;
camd_current_snapshot_parameters.zoom=camd_current_snapshot_parameters.zoom;


*/


        camd_change_snapshot_imagedim(msg_p->body.configparams.imagewidth,
                msg_p->body.configparams.imageheight,
                msg_p->body.configparams.encoding,
                msg_p->body.configparams.flip_x,
                msg_p->body.configparams.flip_y,
                msg_p->body.configparams.zoom);
{  
	UINT16 reg_val=0x7000;
 
		reg_val=((msg_p->body.configparams.effect==T_CAM_SEPIA_EFFECT)?0x7002:7000);
		reg_val=((msg_p->body.configparams.effect==T_CAM_NEGATIVE_EFFECT)?0x7003:7000);
		reg_val=((msg_p->body.configparams.effect==T_CAM_MONOCHROME_EFFECT)?0x7001:7000);

             camd_write_sensor(0x01E2,reg_val);
		camd_write_sensor(0x01E3,0xB423);	 
}


        camd_env_ctrl_blk_p->snapshot_parameters_valid = TRUE;

        if(camcore_config(&camcore_snapshot_params) != CAMCORE_OK)
            msg_p->status = CAMD_INVALID_PARAMETER;


        camd_write_sensor(0x02D2,0x007F);
        camd_write_sensor(0x02CB,0x0001);

        rvf_delay(10);

    }
    camd_env_ctrl_blk_p->state = CAMD_CAMERA_ENABLED;

    return camd_send_response_to_client (CAMD_SET_CONFIGPARAMS_REQ_MSG, msg_p);
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_get_snapshot_parameters                         */
/*                                                                              */
/*    Purpose:     Gets snapshot parameters.                                    */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    T_RV_RET
camd_int_get_snapshot_parameters (T_CAMD_MSG * msg_p)
{
    CAMD_SEND_TRACE ("CAMD CORE entering camd_int_get_snapshot_parameters",
            RV_TRACE_LEVEL_DEBUG_HIGH);
    msg_p->status = CAMD_OK;
    if (!camd_env_ctrl_blk_p->sensor_enabled
            || !camd_env_ctrl_blk_p->snapshot_parameters_valid)
    {
        msg_p->status = CAMD_NOT_READY;
    }
    else
    {
        msg_p->body.configparams =camd_current_snapshot_parameters;
            //camd_env_ctrl_blk_p->snapshot_parameters;
    }
    camd_env_ctrl_blk_p->state = CAMD_CAMERA_ENABLED;

    return camd_send_response_to_client (CAMD_GET_CONFIGPARAMS_RSP_MSG,
            msg_p);
}


/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_get_snapshot                                    */
/*                                                                              */
/*    Purpose:     Takes a snapshot.                                            */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/

extern void camd_disable_sensor_output();
extern void camd_enable_sensor_output();

//#pragma DATA_SECTION(snap, ".camd_data_section")
//volatile int snap=0;



//#pragma CODE_SECTION(cam_assert, ".camd_code_section")
/*
cam_assert()
{

    snap=1;
    while(snap==1);

}
*/

    T_RV_RET
camd_int_get_snapshot (T_CAMD_MSG * camd_msg_p)
{
    T_RV_RET ret_val = CAMD_TRANSFER_COMPLETE;
    const UINT16 snapshot_width =camd_current_snapshot_parameters.imagewidth;
//        camd_env_ctrl_blk_p->snapshot_parameters.imagewidth;


    const UINT16 snapshot_height =camd_current_snapshot_parameters.imageheight;
    //    camd_env_ctrl_blk_p->snapshot_parameters.imageheight;

    int i=0;
    UINT32 t1,t2;

    volatile UINT16 blanking_samples;
    volatile UINT16 blanking_loops;

    const UINT16 snapshot_pixel_size =2;
        /*(2 *( camd_camera_capabilities.resolution[camd_current_snapshot_parameters.resolution].Y_bits +
              camd_camera_capabilities.resolution[camd_current_snapshot_parameters.resolution].U_bits +
              camd_camera_capabilities.resolution[camd_current_snapshot_parameters.resolution].V_bits)) >> 4;
*/
    T_DMA_CHANNEL_PARAMETERS dma_channel_parameters;

    T_DMA_STATUS_RSP_MSG *dma_status_rsp_msg;

    UINT32 bytes_expected = snapshot_width * snapshot_height * snapshot_pixel_size;

    T_RV_RETURN_PATH return_path;


    t1 = rvf_get_tick_count();


    camd_env_ctrl_blk_p->state = CAMD_SNAPSHOT_MODE;

    CAMD_SEND_TRACE ("CAMD CORE entering camd_int_get_snapshot",
            RV_TRACE_LEVEL_DEBUG_HIGH);

    if (!camd_env_ctrl_blk_p->sensor_enabled
            || !camd_env_ctrl_blk_p->snapshot_parameters_valid)
    {
        camd_msg_p->status = CAMD_NOT_READY;
        return camd_send_response_to_client (CAMD_SNAPSHOT_DATA_RSP_MSG,
                camd_msg_p);
    }


    for(i=0;i<1;i++)	{

       CameraCore_Clear_FIFO();

       rvf_delay(2);
       camcore_disable();


#if(WCP_PROF==1)
       prf_LogPointOfInterest("SnapShot start");
#endif

       CameraCore_Clear_FIFO();

       camd_return_queue_flush();

       /* Set DMA parameters */
       dma_channel_parameters.data_width = DMA_DATA_S32;
       dma_channel_parameters.sync = DMA_SYNC_DEVICE_CAM_THRESHOLD;
       dma_channel_parameters.hw_priority = DMA_HW_PRIORITY_HIGH;
       dma_channel_parameters.flush = DMA_FLUSH_ENABLED;
       dma_channel_parameters.nmb_frames = (bytes_expected/((camcore_snapshot_params.fifothreshold)*4));
       dma_channel_parameters.nmb_elements = (camcore_snapshot_params.fifothreshold);
       dma_channel_parameters.dma_end_notification = DMA_NOTIFICATION;
       dma_channel_parameters.secure = DMA_NOT_SECURED;
       dma_channel_parameters.transfer = DMA_MODE_TRANSFER_ENABLE;
       dma_channel_parameters.dma_mode = DMA_MODE_SINGLE;
       dma_channel_parameters.source_address = (UINT32) CC_FIFODATA;
       dma_channel_parameters.source_address_mode = DMA_ADDR_MODE_CONSTANT;
       dma_channel_parameters.source_packet = DMA_NOT_PACKED;
       dma_channel_parameters.source_burst = DMA_NO_BURST;
       dma_channel_parameters.destination_address = (UINT32) (camd_msg_p->body.buff);
       dma_channel_parameters.destination_address_mode = DMA_ADDR_MODE_POST_INC;
       dma_channel_parameters.destination_packet = DMA_NOT_PACKED;
       dma_channel_parameters.destination_burst = DMA_BURST_ENABLED;


       if (RV_OK != dma_set_channel_parameters (camd_available_dma_channel,
                   &dma_channel_parameters) ||
               !camd_dma_wait_status_OK (NULL))
       {
           CAMD_SEND_TRACE ("camd_int_get_viewfinder_frames: "
                   "dma_set_channel_parameters failed",
                   RV_TRACE_LEVEL_ERROR);
           ret_val = CAMD_INTERNAL_ERR;
           break;
       }


#if(WCP_PROF==1)
       prf_LogPointOfInterest("I2C start");
#endif

       camd_write_sensor(0x000D,0x020A);

       camcore_setmode(CAMCORE_SNAPSHOT);

#if(WCP_PROF==1)
       prf_LogPointOfInterest("I2C end");
#endif

       //cam_assert();

       dma_status_rsp_msg = (T_DMA_STATUS_RSP_MSG *)camd_return_queue_get_msg ();
       if(dma_status_rsp_msg == NULL)
       {
           CAMD_SEND_TRACE ("Null message recived WAITING AGAIN",
                   RV_TRACE_LEVEL_DEBUG_HIGH);
           dma_status_rsp_msg = (T_DMA_STATUS_RSP_MSG *)camd_return_queue_get_msg ();
       }

       /* complain if not a DMA_COMPLETED message */
       if (dma_status_rsp_msg->hdr.msg_id != DMA_STATUS_RSP_MSG
               || dma_status_rsp_msg->result.status != DMA_COMPLETED  )
       {
           CAMD_SEND_TRACE
               ("camd_int_get_viewfinder: from dma received unexpected message",
                RV_TRACE_LEVEL_ERROR);

           if (RVF_OK != rvf_free_buf (dma_status_rsp_msg))
           {
               CAMD_SEND_TRACE ("CAMD DMA could not free DMA status msg",
                       RV_TRACE_LEVEL_ERROR);
           }
           ret_val = CAMD_INTERNAL_ERR;
           break;
       }

       if (RVF_OK != rvf_free_buf (dma_status_rsp_msg))
       {
           CAMD_SEND_TRACE ("CAMD DMA could not free DMA status msg",
                   RV_TRACE_LEVEL_ERROR);
           ret_val = CAMD_INTERNAL_ERR;
           break;
       }

       camcore_disable();

    }
    camcore_reset(CAMCORE_RESET_FSM);

#if(FPS ==1)
    t2 = rvf_get_tick_count();
    CAMD_SEND_TRACE_PARAM("Snapshot Time = ",
            RVF_TICKS_TO_MS((t2-t1)), RV_TRACE_LEVEL_DEBUG_HIGH);
#endif
    camd_msg_p->status = ret_val;
    CAMD_SEND_TRACE ("CAMD CORE leaving camd_int_get_snapshot",
            RV_TRACE_LEVEL_DEBUG_HIGH);



#if(WCP_PROF==1)
    prf_LogPointOfInterest("SnapShot end");
#endif

    return camd_send_response_to_client (CAMD_SNAPSHOT_DATA_RSP_MSG,
            camd_msg_p);

}

T_CAMD_PARAMETERS camd_current_viewfinder_parameters;

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_set_viewfinder_parameters                       */
/*                                                                              */
/*    Purpose:     Sets viewfinder parameters.                                  */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    T_RV_RET
camd_int_set_viewfinder_parameters (T_CAMD_MSG * msg_p)
{

    CAMD_SEND_TRACE ("CAMD CORE entering camd_int_set_viewfinder_parameters",
            RV_TRACE_LEVEL_DEBUG_HIGH);
    msg_p->status = CAMD_OK;

    if (!camd_env_ctrl_blk_p->sensor_enabled)
    {
        msg_p->status = CAMD_NOT_READY;
    }
    else
    {
        camd_env_ctrl_blk_p->viewfinder_parameters =
            msg_p->body.configparams;

camd_current_viewfinder_parameters=msg_p->body.configparams;

/*
       camd_current_viewfinder_parameters.black_and_white=msg_p->body.configparams.black_and_white;
	camd_current_viewfinder_parameters.capturemode=msg_p->body.configparams.capturemode;
		camd_current_viewfinder_parameters.encoding=msg_p->body.configparams.encoding;
		camd_current_viewfinder_parameters.flip_x=msg_p->body.configparams.flip_x;
		camd_current_viewfinder_parameters.flip_y=msg_p->body.configparams.flip_y;
		camd_current_viewfinder_parameters.gamma_correction=msg_p->body.configparams.gamma_correction;
		camd_current_viewfinder_parameters.imageheight=msg_p->body.configparams.mageheight;
		camd_current_viewfinder_parameters.imagewidth=msg_p->body.configparams.imagewidth;
		camd_current_viewfinder_parameters.mode=msg_p->body.configparams.mode;
		camd_current_viewfinder_parameters.resolution=msg_p->body.configparams.resolution;
		camd_current_viewfinder_parameters.start_transfer_cb=msg_p->body.configparams.start_transfer_cb;
		camd_current_viewfinder_parameters.zoom=msg_p->body.configparams.zoom;
		
	*/	
		
        
       // camd_write_viewfinder_gamma_correction_to_camera (camd_env_ctrl_blk_p->viewfinder_parameters.gamma_correction);

        camd_env_ctrl_blk_p->viewfinder_parameters_valid = TRUE;
        camd_change_viewfinder_imagedim(msg_p->body.configparams.imagewidth,
                msg_p->body.configparams.imageheight,
                msg_p->body.configparams.encoding,
                msg_p->body.configparams.flip_x,
                msg_p->body.configparams.flip_y,
                msg_p->body.configparams.zoom);
{  

	UINT16 reg_val=0x7000;
 
		switch(msg_p->body.configparams.effect){
			case T_CAM_SEPIA_EFFECT:
				{
					reg_val=0x7002;
				break;
				}
			case T_CAM_NEGATIVE_EFFECT:
				{
				reg_val=0x7003;
				break;
 
			   }
			case T_CAM_MONOCHROME_EFFECT:
				{
					 reg_val=0x7001;
                                  break;
				}
			
					 
             }
            camd_write_sensor(0x01E2,reg_val);
}

		
        if(camcore_config(&camcore_viewfinder_params) != CAMCORE_OK)
            msg_p->status = CAMD_INVALID_PARAMETER;

    }

    camd_env_ctrl_blk_p->state = CAMD_VIEWFINDER_MODE;

    camd_write_sensor(0x02D2,0x0000);
    camd_write_sensor(0x02CB,0x0001);	

    return camd_send_response_to_client (CAMD_SET_CONFIGPARAMS_REQ_MSG, msg_p);
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_get_viewfinder_parameters                       */
/*                                                                              */
/*    Purpose:     gets viewfinder parameters.                                  */ 
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    T_RV_RET
camd_int_get_viewfinder_parameters (T_CAMD_MSG * msg_p)
{
    CAMD_SEND_TRACE ("CAMD CORE entering camd_int_get_viewfinder_parameters",
            RV_TRACE_LEVEL_DEBUG_HIGH);
    msg_p->status = CAMD_OK;
    if (!camd_env_ctrl_blk_p->sensor_enabled
            || !camd_env_ctrl_blk_p->viewfinder_parameters_valid)
    {
        msg_p->status = CAMD_NOT_READY;
    }
    else
    {
        msg_p->body.configparams =
            camd_env_ctrl_blk_p->viewfinder_parameters;
    }

    camd_env_ctrl_blk_p->state = CAMD_CAMERA_ENABLED;

    return camd_send_response_to_client (CAMD_GET_VIEWFINDER_PARAMETERS_RSP_MSG,
            msg_p);
}

/**
 *
 * Puts sensor in preview mode and sets up parallel port
 *
 * @param none                
 *
 * @return none
 *
 */

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_int_get_viewfinder_frames                           */
/*                                                                              */
/*    Purpose:     Sets camera in viewfinder mode.                              */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - cama_msg_p:A CAMD message                                      */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    T_RV_RET
camd_int_get_viewfinder_frames (T_CAMD_MSG * camd_msg_p)
{	
    const UINT16 viewfinder_width =camd_current_viewfinder_parameters.imagewidth;
//        camd_env_ctrl_blk_p->viewfinder_parameters.imagewidth;


    const UINT16 viewfinder_height =camd_current_viewfinder_parameters.imageheight;
  //      camd_env_ctrl_blk_p->viewfinder_parameters.imageheight;


    const UINT16 viewfinder_pixel_size =2;
      /*  (camd_camera_capabilities.resolution[camd_current_viewfinder_parameters.resolution].R_bits +
         camd_camera_capabilities.resolution[camd_current_viewfinder_parameters.resolution].G_bits +
         camd_camera_capabilities.resolution[camd_current_viewfinder_parameters.resolution].B_bits) >> 3;
*/

    T_RV_HDR *dma_rsp_msg;
    T_DMA_STATUS_RSP_MSG *dma_status_rsp_msg;
    T_CAMD_MSG *camd_bitmap_rsp_msg_p;
    T_RV_RETURN_PATH return_path;

    T_RV_RET ret_val = RV_OK;
    T_CAMD_MSG *camd_new_request_p = NULL;
    T_DMA_CHANNEL_PARAMETERS dma_channel_parameters;

    UINT16 event;

    UINT32 bytes_expected = viewfinder_width * viewfinder_height
        * viewfinder_pixel_size;


#if(FPS ==1)
    UINT32 t1,t2;
    t1 = rvf_get_tick_count();
#endif


    camd_env_ctrl_blk_p->state = CAMD_VIEWFINDER_MODE;
    CAMD_SEND_TRACE ("CAMD entering camd_int_get_viewfinder_frames",
            RV_TRACE_LEVEL_DEBUG_HIGH);

    if (!camd_env_ctrl_blk_p->sensor_enabled)
    {
        camd_msg_p->status = CAMD_NOT_READY;
        return camd_send_response_to_client (CAMD_VIEWFINDER_DATA_RSP_MSG,
                camd_msg_p);
    }

    camcore_disable();

    CameraCore_Clear_FIFO(); 
    camd_return_queue_flush();

    /* Set DMA parameters */     
    dma_channel_parameters.data_width = DMA_DATA_S32;
    dma_channel_parameters.sync = DMA_SYNC_DEVICE_CAM_THRESHOLD;
    dma_channel_parameters.hw_priority = DMA_HW_PRIORITY_HIGH;
    dma_channel_parameters.flush = DMA_FLUSH_DISABLED;
    dma_channel_parameters.nmb_frames =  
        (bytes_expected/(camcore_viewfinder_params.fifothreshold*4));
    dma_channel_parameters.nmb_elements = 
        (camcore_viewfinder_params.fifothreshold);
    dma_channel_parameters.dma_end_notification = DMA_NOTIFICATION;
    dma_channel_parameters.secure = DMA_NOT_SECURED;
    dma_channel_parameters.transfer = DMA_MODE_TRANSFER_ENABLE;
    dma_channel_parameters.dma_mode = DMA_MODE_SINGLE;
    dma_channel_parameters.source_address = (UINT32) CC_FIFODATA;;
    dma_channel_parameters.source_address_mode = DMA_ADDR_MODE_CONSTANT;
    dma_channel_parameters.source_packet = DMA_NOT_PACKED;
    dma_channel_parameters.source_burst = DMA_NO_BURST;
    dma_channel_parameters.destination_address = (UINT32) (camd_msg_p->body.buff);
    dma_channel_parameters.destination_address_mode = DMA_ADDR_MODE_POST_INC;
    dma_channel_parameters.destination_packet = DMA_NOT_PACKED;
    dma_channel_parameters.destination_burst = DMA_BURST_ENABLED;

    /* allocate camd response message */

    camd_msg_p->status = CAMD_TRANSFER_COMPLETE;
    if (RV_OK != dma_set_channel_parameters (camd_available_dma_channel,
                &dma_channel_parameters) ||
            !camd_dma_wait_status_OK (NULL))
    {
        CAMD_SEND_TRACE ("camd_int_get_viewfinder_frames: "
                "dma_set_channel_parameters failed",
                RV_TRACE_LEVEL_ERROR);
        ret_val = CAMD_INTERNAL_ERR;

    }

    if(Underflow_flag)
    {
        CAMD_SEND_TRACE_PARAM("Underflow!!",Underflow_flag,RV_TRACE_LEVEL_ERROR);
        Underflow_flag = 0;
    }
    if(Overflow_flag)
    {
        CAMD_SEND_TRACE_PARAM("Overflow!!",Overflow_flag,RV_TRACE_LEVEL_ERROR);
        Overflow_flag = 0;
    }

    camd_write_sensor(0x000D,0x020A);
    camcore_enable();
    //4 March 2008, OMAPS00165017 - x0083029 (Jessica) - Start
    /* 01/24/2008, Herman OMAP151386 Start */
    /*
 if (rvf_free_buf (camd_msg_p) != RVF_OK)
                {
                    CAMD_SEND_TRACE ("CAMD CORE unable to free message",
                            RV_TRACE_LEVEL_ERROR);
                    ret_val = RVM_MEMORY_ERR;
                }
    */
     /* 01/24/2008, Herman OMAP151386 End */
    //4 March 2008, OMAPS00165017 - x0083029 (Jessica) - End

    return RV_OK;
}
/* END camd_int_get_viewfinder_frames */



/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_dma_wait_status_OK                                  */
/*                                                                              */
/*    Purpose:      Blocks camd waiting for a dma return queue message.         */
/*                                                                              */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - channel : dma channel number.                                  */
/*    Return :                                                                  */
/*            TRUE/FALSE                                                        */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/

    static BOOL
camd_dma_wait_status_OK (T_DMA_CHANNEL *channel)
{
    T_DMA_STATUS_RSP_MSG *dma_status_rsp_msg = camd_return_queue_get_msg ();
    BOOL ret_val = TRUE;

    switch (dma_status_rsp_msg->result.status)
    {
        case DMA_RESERVE_OK:
            if (channel)
                *channel=dma_status_rsp_msg->result.channel;
            /* fall through */
        case DMA_OK:
        case DMA_PARAM_SET:
        case DMA_COMPLETED:
        case DMA_CHANNEL_RELEASED:
        case DMA_CHANNEL_ENABLED:
            /* status OK: do nothing */
            break;
        default:
            CAMD_SEND_TRACE_PARAM ("CAMD DMA error status received: ",
                    dma_status_rsp_msg->result.status,
                    RV_TRACE_LEVEL_ERROR);
            ret_val = FALSE;
    }                            /* end switch */

    if (RVF_OK != rvf_free_buf (dma_status_rsp_msg))
    {
        CAMD_SEND_TRACE ("CAMD DMA could not free DMA status msg",
                RV_TRACE_LEVEL_ERROR);
    }
    return ret_val;
}

/**REMOVE THIS****/

//This is needed in case of RTEST build


#if (TEST==1 && CAMD_STATE==2)
cam_test_buffer camd_test_buf;
#endif


T_RV_RET camd_set_zoom(T_CAMD_ZOOM zoom)

{
    UINT16 zoom_reg=0x0000;
    T_RV_RET ret_val=RV_OK;	
    CAMD_SEND_TRACE_PARAM ("CAMD ZOOM",zoom,
            RV_TRACE_LEVEL_ERROR);

    switch(zoom) {

       case ZOOM_IN: {
                        zoom_reg=0x8100;
                     }	
                     break;

       case ZOOM_OUT: {
	   	           
                         zoom_reg=0x8200;
                      }
                      break;	

       case ZOOM_STOP :	{ 
			  zoom_reg=0x8000;
			break;		   
                    }
	   default:  {
	   	       ret_val=RV_INTERNAL_ERR;
			}
			break;	
    }

    if(ret_val==RV_OK) {

       camd_write_sensor(0x000D,0x8008);
       camd_write_sensor(0x1AE,0x0B09);
       camd_write_sensor(0x01AF,zoom_reg);
       camd_write_sensor(0x000D,0x0008);
    }


    return ret_val;






}

#endif
