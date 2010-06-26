/**
 * @file    usbms_handle_message.c
 *
 * USBMS handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author  Your name here (your_email_here)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author                  Modification
 *  -------------------------------------------------------------------
 *  3/19/2004   Virgile COULANGE        Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */


#include <string.h>
//#include "usb/usb_api.h"
//#include "usb/usb_message.h"
#include "usbms/usbms_i.h"
#include "usbms/usbms_env.h"
#include "rfs/rfs_api.h"


#if (WCP_PROF == 1)
    #define TEMP_WCP_PROF 1
#endif


#if TEMP_WCP_PROF
    UINT8 dbg_buffer[20] = {'S','E','T','-','T','X','0','0','0','0','0','0','0','0','0','0','0','0','0','\0'};
#endif

/**
 * Called every time the SW entity is in WAITING state
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  RVM_OK or RVM_MEMORY_ERR.
 */
T_RVM_RETURN usbms_handle_message (T_RV_HDR *msg_p)
{
    T_RVM_RETURN    ret = RV_OK;


    USBMS_SEND_TRACE("USBMS: usbms_handle_message called",RV_TRACE_LEVEL_DEBUG_LOW);



    if (msg_p != NULL)
    {
        switch(usbms_env_ctrl_blk_p->state)
        {
        case INIT:              /** Processes the messages when the system is in the INIT state */
            USBMS_SEND_TRACE("USBMS: INIT STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_init_state(msg_p);
            break;
        case UNPLUGGED:         /** Processes the messages when the system is in the UNPLUGGED state    */
            USBMS_SEND_TRACE("USBMS: UNPLUGGED STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_unplugged(msg_p);
            break;
        case IDLE:              /** Processes the messages when the system is in the IDLE state */
            USBMS_SEND_TRACE("USBMS: IDLE STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_idle(msg_p);
            break;
        case USB_SEND_NO_GBI:   /** Processes the messages when the system is in the USB_SEND_NO_GBI state  */
            USBMS_SEND_TRACE("USBMS: USB_SEND_NO_GBI STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_usb_send_no_gbi(msg_p);
            break;
        case GET_FROM_USB:      /** Processes the messages when the system is in the GET_FROM_USB state */
            USBMS_SEND_TRACE("USBMS: GET_FROM_USB_STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_get_from_usb(msg_p);
            break;
        case GET_FROM_GBI:      /** Processes the messages when the system is in the GET_FROM_GBI state */
            USBMS_SEND_TRACE("USBMS: GET_FROM_GBI STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_get_from_gbi(msg_p);
            break;
        case SEND_TO_USB:       /** Processes the messages when the system is in the SEND_TO_USB state  */
            USBMS_SEND_TRACE("USBMS: SEND_TO_USB STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_send_to_usb(msg_p);
            break;
        case SEND_TO_GBI:       /** Processes the messages when the system is in the SEND_TO_GBI state  */
            USBMS_SEND_TRACE("USBMS: SEND_TO_GBI STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_send_to_gbi(msg_p);
            break;
        case SBW_PROCESS:       /** Processes the messages when the system is in the SBW_PROCESS state  */
            USBMS_SEND_TRACE("USBMS: SBW_PROCESS STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_sbw_process(msg_p);
            break;
        case RESET_IN_PROGRESS: /** Processes the messages when the system is in the RESET_IN_PROGRESS state    */
            USBMS_SEND_TRACE("USBMS: RESET_IN_PROGRESS STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_reset_in_progress(msg_p);
            break;
        case REINIT_USB:    /** Processes the messages when the system is in the REINIT_USB state   */
            USBMS_SEND_TRACE("USBMS: REINIT_USB STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_reinit_usb(msg_p);
            break;
        case REINIT_GBI:    /** Processes the messages when the system is in the REINIT_GBI state   */
            USBMS_SEND_TRACE("USBMS: REINIT_GBI STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_reinit_gbi(msg_p);
            break;
        case SEND_TO_USB_GET_FROM_GBI:    /** Processes the messages when the system is in the REINIT_GBI state   */
            USBMS_SEND_TRACE("USBMS: SEND_TO_USB_GET_FROM_GBI STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            ret = usbms_send_to_usb_get_from_gbi(msg_p);
            break;

        default:
            ret = RV_INVALID_PARAMETER;
            USBMS_SEND_TRACE("USBMS: default STATE",RV_TRACE_LEVEL_DEBUG_LOW);
            break;
        }

        if(usbms_env_ctrl_blk_p->media_change_info & USBMS_MEDIA_INSERTED)
        {
            switch (msg_p->msg_id)
            {
            case GBI_MEDIA_INFO_RSP_MSG:
            case GBI_PARTITION_INFO_RSP_MSG:
                USBMS_SEND_TRACE("USBMS: Media Insertion Info",RV_TRACE_LEVEL_ERROR);
                ret = usbms_reinit_gbi(msg_p);
            }
        }

        /* Free message */
        if (rvf_free_buf(msg_p) != RVF_OK)
        {
            USBMS_SEND_TRACE("USBMS: Unable to free message",RV_TRACE_LEVEL_ERROR);
            return RVM_MEMORY_ERR;
        }
    }

    return ret;
}



/**
 * Function for managing the INIT state
 *
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_init_state(T_RV_HDR *message_p)
{
    T_RV_RET                        status                  = RV_OK;
//  T_GBI_MEDIA_INFO_RSP_MSG        *gbi_media_info_rsp     = NULL;
//  T_GBI_PARTITION_INFO_RSP_MSG    *gbi_partition_info_rsp = NULL;
    T_USB_FM_RESULT_MSG             *usb_fm_subscribe_msg   = NULL;
    UINT8                           counter                 = 0;
//  UINT8                           counter2                = 0;
//  UINT8                           position                = 0;


    switch (message_p->msg_id)
    {
    case USB_FM_RESULT_MSG: /** Process the USB_FM_SUBSCRIBE_MSG message    */
        usb_fm_subscribe_msg = (T_USB_FM_RESULT_MSG *)message_p;
        if (usb_fm_subscribe_msg->result == succes)
        {
            /*### ===> VCO le 08/06/2004 VCO*/
            for (counter=0 ; counter<MAX_LUN_NUM ; counter++)
            {
                usbms_env_ctrl_blk_p->usbms_specific.usb_lun[counter].media_info.media_pressent = FALSE;
            }
            /*### <=== VCO*/

            /** Subscribe to the GBI event  */
            status = gbi_subscribe_events(  GBI_EVENT_MEDIA_INSERT|GBI_EVENT_MEDIA_REMOVEAL|GBI_EVENT_NAN_MEDIA_AVAILABLE,
                                            usbms_env_ctrl_blk_p->return_path );
            /** Init completed, change the state to UNPLUGGED   */
            usbms_change_internal_state(UNPLUGGED);
        }
        break;
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    default:
        usbms_store_message (message_p);
        break;
    }

    return status;
}



/**
 * Function for managing the REINIT_GBI state
 *
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */

UINT8 usbmsReadyCount = 0;

T_USBMS_RETURN usbms_reinit_gbi(T_RV_HDR *message_p)
{
    T_RV_RET                        status                  = RV_OK;
    T_GBI_MEDIA_INFO_RSP_MSG        *gbi_media_info_rsp     = NULL;
    T_GBI_PARTITION_INFO_RSP_MSG    *gbi_partition_info_rsp = NULL;
    UINT8                           counter                 = 0;
    UINT8                           counter2                = 0;
    UINT8                           position                = 0;


    switch (message_p->msg_id)
    {
    case GBI_MEDIA_INFO_RSP_MSG:        /** Process the GBI_MEDIA_INFO_RSP_MSG message  */
        USBMS_SEND_TRACE("USBMS: GBI_MEDIA_INFO_RSP_MSG",RV_TRACE_LEVEL_DEBUG_LOW);
        gbi_media_info_rsp = (T_GBI_MEDIA_INFO_RSP_MSG  *)message_p;

        /** Store the total number of media */
        //usbms_env_ctrl_blk_p->usbms_specific.media_number = gbi_media_info_rsp->nmb_of_media;

        USBMS_TRACE_PARAM("USBMS: gbi_media_info_rsp->nmb_of_media = ", gbi_media_info_rsp->nmb_of_media);

        /** Store each media description    */
        //position = 0;
        for (counter=0 ; counter<gbi_media_info_rsp->nmb_of_media ; counter++)
        {
            if ((gbi_media_info_rsp->info_p[counter].media_type == GBI_MMC)
                ||(gbi_media_info_rsp->info_p[counter].media_type == GBI_SD)
                ||(gbi_media_info_rsp->info_p[counter].media_type == GBI_INTERNAL_NAND)
                ||(gbi_media_info_rsp->info_p[counter].media_type == GBI_INTERNAL_NOR))
            {
                if(counter < usbms_env_ctrl_blk_p->usbms_specific.media_number)
                {
                    if(
                        (gbi_media_info_rsp->info_p[counter].media_type 
                        == usbms_env_ctrl_blk_p->usbms_specific.usb_lun[counter].media_info.media_type)
                        &&
                        (usbms_env_ctrl_blk_p->usbms_specific.usb_lun[counter].media_info.media_pressent == TRUE)
                       )
                    {
                        position++;
                        continue;
                    }
                }
                else
                {
                    usbms_env_ctrl_blk_p->usbms_specific.media_number++;
                }
                memcpy( &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[/*counter*/position].media_info,
                        &gbi_media_info_rsp->info_p[counter],
                        sizeof(T_GBI_MEDIA_INFO)    );
                usbms_env_ctrl_blk_p->usbms_specific.usb_lun[counter].media_info.media_pressent = FALSE;
                position++;
            }
        }
/*###===>ANO-USBMS 10/14/2004 YL*/
        if(position == 0) /** no valid media*/
        {
            USBMS_SEND_TRACE("USBMS: GBI_MEDIA_INFO_RSP_MSG : no valid media",RV_TRACE_LEVEL_ERROR);
            /*no valid media, return to default state*/
            if (usbms_env_ctrl_blk_p->previous_state == UNPLUGGED)
            {
                usbms_change_internal_state(UNPLUGGED);
            }
            else
            {
                usbms_change_internal_state(IDLE);
            }
            if (0 == usbmsReadyCount)
            {
                /* Send Ready to USB using interface number given in parameter */
                if (usb_fm_ready(usbms_env_ctrl_blk_p->interface_id, usbms_env_ctrl_blk_p->return_path) != RVM_OK)
                {
                    status = RV_INTERNAL_ERR;
                }
                else
                {
                    usbmsReadyCount = 1;
                }
            }
        }
        else /**valid media present, init data and retreive part_info*/
        {
            for (counter=usbms_env_ctrl_blk_p->usbms_specific.media_number ; counter<MAX_LUN_NUM ; counter++)
        {
            usbms_env_ctrl_blk_p->usbms_specific.usb_lun[counter].media_info.media_pressent = FALSE;
        }

        rvf_free_buf(gbi_media_info_rsp->info_p);


        /** Store the total number of media */
        //usbms_env_ctrl_blk_p->usbms_specific.media_number = position;

            USBMS_SEND_TRACE("USBMS: Querry gbi partition info",RV_TRACE_LEVEL_DEBUG_LOW);
        /** Ask for the partition description   */
        status = gbi_get_partition_info(    usbms_env_ctrl_blk_p->prim_mb_id,
                                            usbms_env_ctrl_blk_p->return_path   );
        }
/*###===>ANO-USBMS*/

        break;
    case GBI_PARTITION_INFO_RSP_MSG:    /** Process the GBI_PARTITION_INFO_RSP_MSG message  */
        USBMS_SEND_TRACE("USBMS: GBI_PARTITION_INFO_RSP_MSG",RV_TRACE_LEVEL_DEBUG_LOW);
        gbi_partition_info_rsp = (T_GBI_PARTITION_INFO_RSP_MSG  *)message_p;

        USBMS_TRACE_PARAM("USBMS: gbi_partition_info_rsp->nmb_of_partitions = ", gbi_partition_info_rsp->nmb_of_partitions);

        /** Store each partition description in the corresponding media description */
        //usbms_env_ctrl_blk_p->usbms_specific.media_number =0;
        for (counter=0; counter<gbi_partition_info_rsp->nmb_of_partitions;counter++)
        {
            if ((gbi_partition_info_rsp->info_p[counter].partition_nmb == PARTITION_NMB)
                &&( (gbi_partition_info_rsp->info_p[counter].filesystem_type == GBI_FAT12)
                ||(gbi_partition_info_rsp->info_p[counter].filesystem_type == GBI_FAT16_A)
                ||(gbi_partition_info_rsp->info_p[counter].filesystem_type == GBI_FAT16_B)
                ||(gbi_partition_info_rsp->info_p[counter].filesystem_type == GBI_FAT32)
                ||(gbi_partition_info_rsp->info_p[counter].filesystem_type == GBI_FAT32_LBA)
                ||(gbi_partition_info_rsp->info_p[counter].filesystem_type == GBI_FAT16_LBA)
                ||(gbi_partition_info_rsp->info_p[counter].filesystem_type == GBI_UNKNOWN)))
            {
                for (counter2=0 ; counter2<MAX_LUN_NUM ; counter2++)
                {
                    if (usbms_env_ctrl_blk_p->usbms_specific.usb_lun[counter2].media_info.media_nmb
                        == gbi_partition_info_rsp->info_p[counter].media_nmb)
                    {
                        memcpy( &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[counter2].partition_info,
                                &gbi_partition_info_rsp->info_p[counter],
                                sizeof(T_GBI_PARTITION_INFO)    );

                        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[counter2].media_info.media_pressent = TRUE;
                        /** Initialization of the data  */
                        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[counter2].fail_reason      = GOOD_STATUS;
                        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[counter2].fail_request     = FALSE;
                        //usbms_env_ctrl_blk_p->usbms_specific.media_number++;
                        break;
                    }
                }

            }
        }

        usbms_env_ctrl_blk_p->media_change_info &= (~USBMS_MEDIA_INSERTED);
        /** Subscribe to the GBI event  */
        //status = gbi_subscribe_events(    GBI_EVENT_MEDIA_INSERT|GBI_EVENT_MEDIA_REMOVEAL,
        //                              usbms_env_ctrl_blk_p->return_path );
        /** reInit gbi completed, return to the previous state  */
        //usbms_change_internal_state(usbms_env_ctrl_blk_p->previous_state);

        if (0 == usbmsReadyCount)
        {
            /* Send Ready to USB using interface number given in parameter */
            if (usb_fm_ready(usbms_env_ctrl_blk_p->interface_id, usbms_env_ctrl_blk_p->return_path) != RVM_OK)
            {
                status = RV_INTERNAL_ERR;
            }
            else
            {
                usbmsReadyCount = 1;
            }
        }

        rvf_free_buf(gbi_partition_info_rsp->info_p);

        break;
    case GBI_EVENT_IND_MSG:
        USBMS_SEND_TRACE("USBMS: GBI_EVENT_IND_MSG",RV_TRACE_LEVEL_DEBUG_LOW);
        status = usbms_restart(message_p);
        break;
    default:
        break;
    }

    return status;
}



/**
 * Function for managing the UNPLUGGED state
 *
 * The message in parameter is freed in this function.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_unplugged(T_RV_HDR *message_p)
{
    T_RV_RET                status          = RV_OK;


    switch(message_p->msg_id)
    {
    case USB_BUS_CONNECTED_MSG:     /** Process the USB_BUS_CONNECTED_MSG message   */
        /** Set the rx buffer for the class request */
        status  = usb_set_rx_buffer(    usbms_env_ctrl_blk_p->interface_id,
                                        EP_CTRL,
                                        (UINT8*)&usbms_env_ctrl_blk_p->usbms_specific.class_request,
                                        sizeof(T_USBMS_CLASS_REQUEST) );

        /** Set the rx buffer for the CBW request   */
        usbms_env_ctrl_blk_p->usbms_specific.buffer_rx_temp.size = MAX_COMMAND_SIZE;

        status  = usb_set_rx_buffer(    usbms_env_ctrl_blk_p->interface_id,
                                        EP_RX,
                                        (UINT8*)usbms_env_ctrl_blk_p->usbms_specific.buffer_rx_temp.buff,
                                        usbms_env_ctrl_blk_p->usbms_specific.buffer_rx_temp.size    );

        if (status != RV_OK)
        {
            //Stall EP_RX;
            usbms_change_internal_state(RESET_IN_PROGRESS);
        }
        else
        {
            usbms_change_internal_state(IDLE);

/* ********************************************************************* */
         rfs_unmount(RFS_FAT);  /* UnMount the file system */

/* ********************************************************************* */
			
        }
        break;
    case USB_BUS_DISCONNECTED_MSG:  /** Process the USB_BUS_DISCONNECTED_MSG message    */
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    case USB_FM_RESULT_MSG:
        break;
    default:
        usbms_store_message(message_p);
        break;
    }

    return status;
}




/**
 * Function for managing the RESET_IN_PROGRESS state
 *
 * The message in parameter is freed in this function.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_reset_in_progress(T_RV_HDR *message_p)
{
    T_RV_RET                    status                  = RV_OK;
    T_USB_RX_BUFFER_FULL_MSG    *usb_rx_buffer_full_msg = NULL;


    switch(message_p->msg_id)
    {
    case USB_RX_BUFFER_FULL_MSG:    /** Process the USB_RX_BUFFER_FULL_MSG message  */
        usb_rx_buffer_full_msg = (T_USB_RX_BUFFER_FULL_MSG  *)message_p;
        /** the only message processed is the class request "Mass Storage Reset */
        if ( usb_rx_buffer_full_msg->endpoint == EP_CTRL )
        {
            if (usbms_env_ctrl_blk_p->usbms_specific.class_request.bmRequestType == 0x21)
            {
                /** The system is reset so the system state returns to IDLE */
                usbms_change_internal_state(IDLE);

            }
/*###==> ANO-USBMS 08102004 YL*/
            /** need to repost a buffer for the control endpoint*/
            cr_mass_storage_reset();
/*<==### ANO-USBMS*/
        }
        break;
    case USB_BUS_DISCONNECTED_MSG:  /** Process the USB_BUS_DISCONNECTED_MSG message    */
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_CTRL );
//      /** the usb is disconnected so the internal state changes to UNPLUGGED  */
//      usbms_change_internal_state(UNPLUGGED);
//      break;
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    case USB_BUS_SUSPEND_MSG:
        USBMS_SEND_TRACE("USBMS: RESET_IN_PROGRESS: USB_BUS_SUSPEND_MSG",RV_TRACE_LEVEL_DEBUG_LOW);
        break;
    case USB_BUS_RESUME_MSG:
        USBMS_SEND_TRACE("USBMS: RESET_IN_PROGRESS: USB_BUS_RESUME_MSG",RV_TRACE_LEVEL_DEBUG_LOW);
        break;
    case USB_HOST_RESET_MSG:
        USBMS_SEND_TRACE("USBMS: RESET_IN_PROGRESS: USB_HOST_RESET_MSG",RV_TRACE_LEVEL_DEBUG_LOW);
        break;
    case USB_FM_RESULT_MSG:
        USBMS_SEND_TRACE("USBMS: RESET_IN_PROGRESS: USB_FM_RESULT_MSG",RV_TRACE_LEVEL_DEBUG_LOW);
        break;
    case USB_BUS_CONNECTED_MSG:
        USBMS_SEND_TRACE("USBMS: RESET_IN_PROGRESS: USB_BUS_CONNECTED_MSG",RV_TRACE_LEVEL_DEBUG_LOW);
        break;
    case USB_STATUS_READY_MSG:
        USBMS_SEND_TRACE("USBMS: RESET_IN_PROGRESS: USB_STATUS_READY_MSG",RV_TRACE_LEVEL_DEBUG_LOW);
        break;
    case USB_TX_BUFFER_EMPTY_MSG:
        USBMS_SEND_TRACE("USBMS: RESET_IN_PROGRESS: USB_TX_BUFFER_EMPTY_MSG",RV_TRACE_LEVEL_DEBUG_LOW);
        break;
    default:
        usbms_store_message (message_p);
        break;
    }

    return status;
}


/**
 * Function for managing the IDLE state
 *
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_idle(T_RV_HDR *message_p)
{
    T_RV_RET                    status              = RV_OK;
    T_USB_RX_BUFFER_FULL_MSG    *usb_rx_buffer_full = NULL;
    T_USB_TX_BUFFER_EMPTY_MSG   *usb_tx_buffer_empty= NULL;
    T_USBMS_CBW                 *cbw                = NULL;
    T_USBMS_BOT_CDB             *bot_cdb            = NULL;
    T_USBMS_XFER                *request            = NULL;
    UINT16                      temp16              = 0;
    UINT32                      temp32              = 0;



    switch (message_p->msg_id)
    {
    case USB_RX_BUFFER_FULL_MSG:        /** Process the USB_RX_BUFFER_FULL_MSG message  */
        usb_rx_buffer_full = (T_USB_RX_BUFFER_FULL_MSG  *)message_p;
        switch(usb_rx_buffer_full->endpoint)
        {
        case EP_CTRL:           /** the target endpoint is EP0 , so this is a class request */
            if (    usbms_env_ctrl_blk_p->usbms_specific.class_request.bmRequestType
                    == MAX_LUN_REQUEST_TYPE )   /** it is a "Get Max Lun" class request */
            {
                USBMS_SEND_TRACE("USBMS: Get Max Lun ",RV_TRACE_LEVEL_DEBUG_LOW);
                status = cr_get_max_lun();
            }
            else                                /** it is a "Mass Storage Reset" class request  */
            {
                USBMS_SEND_TRACE("USBMS: Mass Storage Reset",RV_TRACE_LEVEL_DEBUG_LOW);
                status = cr_mass_storage_reset();
            }

            if (status == RV_OK)
            {
                usbms_change_internal_state(IDLE);
            }
            else
            {
                usbms_change_internal_state(RESET_IN_PROGRESS);
            }
            break;
        case EP_RX: /** the target endpoint is EP2, so this is a RBC request    */
            /** Initialize the various temporary variables  */
            cbw     = (T_USBMS_CBW*)usbms_env_ctrl_blk_p->usbms_specific.buffer_rx_temp.buff;
            /*
            usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWTag =  (cbw->dCBWTag_3<<24)
                                                                |(cbw->dCBWTag_2<<16)
                                                                |(cbw->dCBWTag_1<<8)
                                                                |(cbw->dCBWTag_0);
            */
            usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWTag_0  = cbw->dCBWTag_0;
            usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWTag_1  = cbw->dCBWTag_1;
            usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWTag_2  = cbw->dCBWTag_2;
            usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWTag_3  = cbw->dCBWTag_3;
            bot_cdb = (T_USBMS_BOT_CDB*)&cbw->CBWCB;
            usbms_env_ctrl_blk_p->usbms_specific.current_lun = cbw->bCBWLUN;


            /** Process the RBC request */
            switch (bot_cdb->generic_rbc.operation_code)
            {
            case INQUIRY:
                /** Process the RBC INQUIRY request */
                USBMS_SEND_TRACE("USBMS: RBC INQUIRY",RV_TRACE_LEVEL_DEBUG_LOW);
                status = rbc_inquiry(&bot_cdb->inquiry);
                if (RV_OK == status)
                {
                    /** the process is in progress, the internal state changes to USB_SEND_NO_GBI   */
                    usbms_change_internal_state(USB_SEND_NO_GBI);
                }
                else
                {
                    // Stall EP_RX ???
                    // Stall EP_TX ???
                    /** an error occured, so the internal state changes to RESET_IN_PROGRESS    */
                    usbms_change_internal_state(RESET_IN_PROGRESS);
                }
                break;
            case MODE_SELECT:
                /** Process the RBC MODE_SELECT request */
                USBMS_SEND_TRACE("USBMS: RBC MODE_SELECT",RV_TRACE_LEVEL_DEBUG_LOW);
                status = rbc_mode_select(&bot_cdb->mode_select);
                if (RV_OK == status )
                {
                    /** the command has been fully processed    */
                    send_bot_status(GOOD_STATUS,0);
                }
                else
                {
                    /** an error occured during the processing  */
                    send_bot_status(PHASE_ERROR,0);
                }
                /** The internal state changes to the CBW status phase  */
                usbms_change_internal_state(SBW_PROCESS);
                break;
            case PREVENT_ALLOW_REMOVAL:
                /** Process the RBC PREVENT_ALLOW_REMOVAL request   */
                USBMS_SEND_TRACE("USBMS: RBC PREVENT_ALLOW_REMOVAL",RV_TRACE_LEVEL_DEBUG_LOW);
                status = rbc_prevent_allow_removal(&bot_cdb->media_removal);
                if (RV_OK == status )
                {
                    /** the command has been fully processed    */
                    send_bot_status(GOOD_STATUS,0);
                }
                else
                {
                    /** an error occured during the processing  */
                    send_bot_status(PHASE_ERROR,0);
                }
                /** The internal state changes to the CBW status phase  */
                usbms_change_internal_state(SBW_PROCESS);
                break;
            case START_STOP_UNIT:
                /** Process the RBC START_STOP_UNIT request */
                USBMS_SEND_TRACE("USBMS: RBC START_STOP_UNIT",RV_TRACE_LEVEL_DEBUG_LOW);
                status = rbc_start_stop_unit(&bot_cdb->start_stop);
                if (RV_OK == status )
                {
                    /** the command has been fully processed    */
                    send_bot_status(GOOD_STATUS,0);
                }
                else
                {
                    /** an error occured during the processing  */
                    send_bot_status(PHASE_ERROR,0);
                }
                /** The internal state changes to the CBW status phase  */
                usbms_change_internal_state(SBW_PROCESS);
                break;
            case READ_CAPACITY:
                /** Process the RBC READ_CAPACITY command   */
                USBMS_SEND_TRACE("USBMS: RBC READ_CAPACITY",RV_TRACE_LEVEL_DEBUG_LOW);
                status = rbc_read_capacity(&bot_cdb->read_capacity);
                /** the processing is in progress...    */
                //usbms_change_internal_state(USB_SEND_NO_GBI);
                break;
            case TEST_UNIT_READY:
                /** Process the RBC TEST_UNIT_READY command */
                USBMS_SEND_TRACE("USBMS: RBC TEST_UNIT_READY",RV_TRACE_LEVEL_DEBUG_LOW);
                status = rbc_test_unit_ready(&bot_cdb->test_unit_ready);
                if (RV_OK == status)
                {
                    /** the request has been successfully handled   */
                    send_bot_status(GOOD_STATUS,0);
                }
                else
                {
                    /** an error ocured during the processing...    */
                    send_bot_status(COMMAND_FAILED,0);
                }
                /** The internal state changes to the CBW status phase  */
                usbms_change_internal_state(SBW_PROCESS);
                break;
            case READ_10:
                /** Process the RBC READ_10 command */
                USBMS_SEND_TRACE("USBMS: RBC READ_10",RV_TRACE_LEVEL_DEBUG_LOW);
                status = rbc_test_unit_ready(NULL);
                if (USBMS_OK == status)
                {
                    status = rbc_read_10(&bot_cdb->read);
                    /** the processing is in progress   */
                    usbms_change_internal_state(GET_FROM_GBI);
                }
                else
                {
                    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[cbw->bCBWLUN].fail_reason  = COMMAND_FAILED;
                    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[cbw->bCBWLUN].write_request.buffer_size =  (cbw->dCBWDataTransferLength_3<<24)
                                                                                                            |(cbw->dCBWDataTransferLength_2<<16)
                                                                                                            |(cbw->dCBWDataTransferLength_1<<8)
                                                                                                            |(cbw->dCBWDataTransferLength_0);
                    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[cbw->bCBWLUN].write_request.transferred_size = 0;
                    /** send the read capability structure  */

                    status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                                EP_TX,
                                                usbms_env_ctrl_blk_p->usbms_specific.usb_lun[cbw->bCBWLUN].write_request.data_buff,
                                                0,
                                                TRUE    );
                    usbms_change_internal_state(USB_SEND_NO_GBI);

                    /*
                    status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                                EP_TX,
                                                NULL,
                                                usbms_env_ctrl_blk_p->usbms_specific.usb_lun[cbw->bCBWLUN].write_request.buffer_size,
                                                TRUE    );
                    send_bot_status(COMMAND_FAILED,usbms_env_ctrl_blk_p->usbms_specific.usb_lun[cbw->bCBWLUN].write_request.buffer_size);
                    usbms_change_internal_state(SBW_PROCESS);
                    */
                }
                break;
            case WRITE_10:
                /** Process the RBC WRITE_10 command    */
                USBMS_SEND_TRACE("USBMS: RBC WRITE_10",RV_TRACE_LEVEL_DEBUG_LOW);
                status = rbc_write_10(&bot_cdb->write);
                /** the processing is in progress   */
                usbms_change_internal_state(GET_FROM_USB);
                break;
            case VERIFY:
                /** Process the RBC VERIFY command  */
                USBMS_SEND_TRACE("USBMS: RBC VERIFY",RV_TRACE_LEVEL_DEBUG_LOW);
                status = rbc_verify(&bot_cdb->verify);
                if (RV_OK == status)
                {
                    /** the request has been successfully handled   */
                    send_bot_status(GOOD_STATUS,0);
                }
                else
                {
                    /** an error ocured during the processing...    */
                    send_bot_status(PHASE_ERROR,0);
                }
                /** The internal state changes to the CBW status phase  */
                usbms_change_internal_state(SBW_PROCESS);
                break;
            case REQUEST_SENSE:
                /** Process the RBC REQUEST_SENSE command   */
                USBMS_SEND_TRACE("USBMS: RBC REQUEST_SENSE",RV_TRACE_LEVEL_DEBUG_LOW);
                rbc_request_sense(&bot_cdb->request_sense);
                /** the internal remains unchanged  */
                //usbms_change_internal_state(IDLE);
                usbms_change_internal_state(USB_SEND_NO_GBI);
                break;
            case MODE_SENSE:
                /** Process the RBC REQUEST_SENSE command   */
                USBMS_SEND_TRACE("USBMS: RBC MODE_SENSE",RV_TRACE_LEVEL_DEBUG_LOW);
                status = rbc_mode_sense(&bot_cdb->mode_sense);
                /** the process is in progress, the internal state changes to USB_SEND_NO_GBI   */
                usbms_change_internal_state(USB_SEND_NO_GBI);
                break;
            case 0x23:
                USBMS_SEND_TRACE("USBMS: RBC 0x23",RV_TRACE_LEVEL_DEBUG_LOW);
//              if (USBMS_OK == rbc_test_unit_ready(NULL))
//              {
                    request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request;
                    memset(request->data_buff,0,12);
                    request->data_buff[3]=0x08;
#if 0
                    /** Number of blocks*/
                    temp32 = 131072;//usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.nmb_of_blocks;
                    temp16 = 1024;//usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.bytes_per_block;
                    request->data_buff[4]   = (temp32&0xff000000)>>24;
                    request->data_buff[5]   = (temp32&0x00ff0000)>>16;
                    request->data_buff[6]   = (temp32&0x0000ff00)>>8;
                    request->data_buff[7]   = temp32&0x000000ff;
                    /** Descriptor type */
                    request->data_buff[8]   = 0x03;
                    /** Length in bytes of each logical block   */
                    request->data_buff[10]  = (temp16&0xff00)>>8;
                    request->data_buff[11]  = temp16&0x00ff;
#endif

/*KG*/
                    /** Number of blocks*/
                    temp32 = usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.nmb_of_blocks;
                    temp16 = usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.bytes_per_block;
                    request->data_buff[4]   = (temp32&0xff000000)>>24;
                    request->data_buff[5]   = (temp32&0x00ff0000)>>16;
                    request->data_buff[6]   = (temp32&0x0000ff00)>>8;
                    request->data_buff[7]   = temp32&0x000000ff;

                    /** Descriptor type */
                    if(usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].media_info.media_pressent)
                        request->data_buff[8]   = 0x02;
                    else
                        request->data_buff[8]   = 0x03;

                    /** Length in bytes of each logical block   */
                    request->data_buff[10]  = (temp16&0xff00)>>8;
                    request->data_buff[11]  = temp16&0x00ff;



//              }
//              else
//              {
//                  request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request;
//                  memset(request->data_buff,0,12);
//                  request->data_buff[3]=0x08;
//                  /** Descriptor type */
//                  request->data_buff[8]   = 0x03;
//              }

                request->buffer_size = 0xFC;
                request->transferred_size = 12;
                status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                            EP_TX,
                                            request->data_buff,
                                            request->transferred_size,
                                            TRUE    );
                usbms_change_internal_state(USB_SEND_NO_GBI);
                break;
            default:
                USBMS_SEND_TRACE("USBMS: RBC default",RV_TRACE_LEVEL_DEBUG_LOW);
                /*
                status = usb_set_rx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                            EP_RX,
                                            (UINT8*)usbms_env_ctrl_blk_p->usbms_specific.buffer_rx_temp.buff,
                                            usbms_env_ctrl_blk_p->usbms_specific.buffer_rx_temp.size    );
                */
                break;
            }
            break;
        }
        break;
    case USB_TX_BUFFER_EMPTY_MSG:
        usb_tx_buffer_empty = (T_USB_TX_BUFFER_EMPTY_MSG    *)message_p;
        switch(usb_tx_buffer_empty->endpoint)
        {
        case EP_CTRL:               /** The "Get Max Lun" class request is fully completed  */
            /** Provides a buffer for the next class request    */
            status = usb_set_rx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                    EP_CTRL,
                                    (UINT8*)&usbms_env_ctrl_blk_p->usbms_specific.class_request,
                                    sizeof(T_USBMS_CLASS_REQUEST) );
            /** the internal state remains unchanged    */
            usbms_change_internal_state(IDLE);
            break;
        case EP_TX:
            break;
        default:
            USBMS_SEND_TRACE("USBMS: default",RV_TRACE_LEVEL_DEBUG_LOW);
            break;
        }
        break;
    case USB_BUS_DISCONNECTED_MSG:
        /** the USB is disconnected */
        usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_CTRL );
        usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_RX   );
        /** the internal state changes to UNPLUGGED */
        usbms_change_internal_state(UNPLUGGED);
        status = usbms_restart(message_p);
/* ********************************************************************* */
         rfs_mount(RFS_FAT);  /* UnMount the file system */

/* ********************************************************************* */
			

		
        break;
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    default:
        break;
    }

    return status;
}


/**
 * Function for managing the USB_SEND_NO_GBI state
 *
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_usb_send_no_gbi(T_RV_HDR *message_p)
{
    T_RV_RET                    status                  = RV_OK;
    T_USB_TX_BUFFER_EMPTY_MSG   *usb_tx_buffer_empty    = (T_USB_TX_BUFFER_EMPTY_MSG *)message_p;
    T_USBMS_XFER                *request                = NULL;



    switch (message_p->msg_id)
    {
    case USB_TX_BUFFER_EMPTY_MSG:
        /** the RBC request is completed    */
        request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request;
        //status = send_bot_status(GOOD_STATUS,request->buffer_size-request->transferred_size);
        status = send_bot_status((T_COMMAND_BLOCK_STATUS)(usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].fail_reason), request->buffer_size-request->transferred_size   );
        memset(request,0,sizeof(T_USBMS_XFER));
		/* Overusing The read Buffer , here it is used for writing. */
        request->data_buff = xfer_buffer->data_xfer_buff.buffer[0];
        if (status == RV_OK)
        {
            /** the internal state changes to the CBW status phase  */
            usbms_change_internal_state(SBW_PROCESS);
        }
        else
        {
            usbms_change_internal_state(RESET_IN_PROGRESS);
        }
        break;
    case USB_BUS_DISCONNECTED_MSG:
//      /** the usb is disconnected */
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_CTRL );
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_TX   );
//      /** the internal state changes to UNPLUGGED */
//      usbms_change_internal_state(UNPLUGGED);
//      break;
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    default:
        usbms_store_message (message_p);
        break;
    }
    return status;
}


/**
 * Function for managing the GET_FROM_USB state
 *
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_get_from_usb(T_RV_HDR *message_p)
{
    T_RV_RET                    status              = RV_OK;
    T_USB_RX_BUFFER_FULL_MSG    *usb_rx_buffer_full = (T_USB_RX_BUFFER_FULL_MSG*)message_p;
    UINT32                      remaining_data;
    T_USBMS_XFER                *request            = NULL;



    switch (message_p->msg_id)
    {
    case USB_RX_BUFFER_FULL_MSG:
        request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request;

        /** update the transfered data  */
        request->transferred_size += request->command_size;

        /** is there still data to get from the usb */
        remaining_data  = request->buffer_size - request->transferred_size;

        if (remaining_data > 0)
        {
            /** update the request parameters */
            if (remaining_data >= MAX_TRANSFER_SIZE)
            {
                request->command_size   = MAX_TRANSFER_SIZE;
            }
            else
            {
                request->command_size   = remaining_data/*request->buffer_size*/;
            }


            /** Read the data from the USB */
            status = usb_set_rx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                        EP_RX,
                                        &request->data_buff[request->transferred_size],
                                        request->command_size );

            /** the internal state remains unchanged    */
            usbms_change_internal_state(GET_FROM_USB);
        }
        else    /**  there is no more data to get from the USB, so we send the stored data to the GBI   */
        {
            /** write the data blocks on the GBI    */
            status = gbi_write( usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.media_nmb,
                                PARTITION_NMB,
                                request->first_block_nmb,
                                request->current_block,
                                0,
                                (UINT32*)request->data_buff,
                                usbms_env_ctrl_blk_p->return_path   );
            /** the internal state changes to SEND_TO_GBI   */
            usbms_change_internal_state(SEND_TO_GBI);
        }
        break;
    case USB_BUS_DISCONNECTED_MSG:
//      /** The usb is disconnected */
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_CTRL );
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_TX   );
//      /** the internal state changes to UNPLUGGED */
//      usbms_change_internal_state(UNPLUGGED);
//      break;
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    default:
        usbms_store_message (message_p);
        break;
    }

    return status;
}


/**
 * Function for managing the GET_FROM_GBI state
 *
 * The message in parameter is freed in this function.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_get_from_gbi(T_RV_HDR *message_p)
{
    T_RV_RET            status          = RV_OK;
    T_GBI_READ_RSP_MSG  *gbi_read_rsp   = (T_GBI_READ_RSP_MSG   *)message_p;
    T_USBMS_XFER        *request        = NULL;


    switch (message_p->msg_id)
    {
    case GBI_READ_RSP_MSG:
        USBMS_TRACE_PARAM("USBMS: Read Status = ", gbi_read_rsp->result);
        request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request;
        if (gbi_read_rsp->result == RV_OK)  /** the read operation is successful    */
        {
            //T_USBMS_XFER  *request    = NULL;
            UINT16          max_block_per_xfer;
            T_USB_LUN       *lun;
            UINT16          remaining_block;
            UINT8           temp_op_index = 0;

#if TEMP_WCP_PROF
            UINT8           *temp_ptr = (dbg_buffer + 6);
            prf_LogPointOfInterest("GBI-Read-Complete");
#endif

            USBMS_SEND_TRACE("USBMS:GBI Read SUCCESSFUL", RV_TRACE_LEVEL_ERROR);

            /** initialize the temporary variables  */
            //request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request;
            lun     = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun];

            request->data_read_buff->buffer_status[request->data_read_buff->gbi_op_buf_index] = AVAILABLE;

/*----->> Check if we have to read more data as part of our read request----->>*/
/*----->> Check if previous USB operation is over ----->>*/

            if(request->data_read_buff->buffer_status[request->data_read_buff->usb_op_buf_index] == AVAILABLE)
            {
                /** we got one part of the data from the GBI, now we send it to the USB **/
                /** Initialize the Transfer parameters  **/

                USBMS_SEND_TRACE("USBMS: usbms_get_from_gbi - USB AV for Tx",RV_TRACE_LEVEL_ERROR);

                request->command_size = request->data_read_buff->gbi_tx_count;

                temp_op_index = request->data_read_buff->usb_op_buf_index;

                request->data_read_buff->usb_op_buf_index = request->data_read_buff->gbi_op_buf_index;

                request->data_read_buff->buffer_status[request->data_read_buff->usb_op_buf_index] = USB_OP;

                /** sends the data to the USB   */
                USBMS_SEND_TRACE("USBMS: usbms_get_from_gbi Send Data to USB", RV_TRACE_LEVEL_ERROR);

#if TEMP_WCP_PROF
                sprintf(temp_ptr,"%d",request->command_size);
                prf_LogPointOfInterest("Set Tx Buffer");
#endif
                status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                            EP_TX,
                                            request->data_read_buff->buffer[request->data_read_buff->usb_op_buf_index],
                                            request->command_size,
                                            FALSE );
                /** update the transaction parameters   */
                request->first_block_nmb    += request->current_block;
                remaining_block             = request->number_of_blocks - request->first_block_nmb;
                max_block_per_xfer          = MAX_BYTE_PER_READ_XFER / lun->partition_info.bytes_per_block;

                if (remaining_block > 0)    /** there is still data to get from the GBI */
                {
                    if(remaining_block > max_block_per_xfer)
                    {
                        request->current_block  = max_block_per_xfer;
                    }
                    else
                    {
                        request->current_block  = remaining_block;
                    }

                    request->buffer_size        = request->current_block * lun->partition_info.bytes_per_block;

                    request->data_read_buff->gbi_op_buf_index = temp_op_index;

                    request->data_read_buff->buffer_status[request->data_read_buff->gbi_op_buf_index]
                                                = GBI_OP;

                    request->data_read_buff->gbi_tx_count = request->buffer_size;

                    USBMS_SEND_TRACE("USBMS: usbms_get_from_gbi Get From GBI", RV_TRACE_LEVEL_ERROR);

#if TEMP_WCP_PROF
                prf_LogPointOfInterest("GBI-Read");
#endif
                    /** reads some data blocks from the GBI */
                    status = gbi_read(  usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.media_nmb,
                                        PARTITION_NMB,
                                        request->first_block_nmb,
                                        request->current_block,
                                        0,
                                        (UINT32*)request->data_read_buff->buffer[request->data_read_buff->gbi_op_buf_index],
                                        usbms_env_ctrl_blk_p->return_path   );

                    /** the internal state changes to SEND_TO_USB_GET_FROM_GBI  */
                     usbms_change_internal_state(SEND_TO_USB_GET_FROM_GBI);
                    //usbms_change_internal_state(SEND_TO_USB);
                }
                else
                {
                    USBMS_SEND_TRACE("USBMS: usbms_get_from_gbi ST - SEND_TO_USB, REQ_READ_COMP", RV_TRACE_LEVEL_ERROR);
                    /** the internal state changes to SEND_TO_USB   */
                    usbms_change_internal_state(SEND_TO_USB);
                }
            }
            else
            {
                USBMS_SEND_TRACE("USBMS: usbms_get_from_gbi ST - SEND_TO_USB, USB_BUF-NA", RV_TRACE_LEVEL_ERROR);
                /** the internal state changes to SEND_TO_USB   */
                usbms_change_internal_state(SEND_TO_USB);
            }
        }
        else    /** Read Failure return stall   */
        {
            /** Stall the Endpoint  */
            USBMS_SEND_TRACE("USBMS: Stall the Endpoint", RV_TRACE_LEVEL_ERROR);
            /*
            usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].fail_reason  = PHASE_ERROR;
            usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].fail_request = TRUE;
            usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.buffer_size = request->buffer_size;
            usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.transferred_size = request->transferred_size;

            status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                        EP_TX,
                                        request->data_buff,
                                        0,
                                        TRUE    );
            usbms_change_internal_state(USB_SEND_NO_GBI);
            */

            status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                        EP_TX,
                                        NULL,
                                        request->buffer_size,
                                        TRUE    );

            /** the internal state changes to USB_SEND_NO_GBI   */
            send_bot_status(COMMAND_FAILED/*PHASE_ERROR*/,request->buffer_size-request->transferred_size);
            usbms_change_internal_state(SBW_PROCESS);
        }
        break;
    case USB_BUS_DISCONNECTED_MSG:
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    default:
        usbms_store_message (message_p);
        break;
    }

    return USBMS_OK;
}


/**
 * Function for managing the SEND_TO_USB state
 *
 * The message in parameter is freed in this function.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_send_to_usb(T_RV_HDR *message_p)
{
    T_RV_RET                    status                  = RV_OK;
    T_USB_TX_BUFFER_EMPTY_MSG   *usb_tx_buffer_empty    = (T_USB_TX_BUFFER_EMPTY_MSG    *)message_p;
    T_USBMS_XFER                *request                = NULL;
    UINT16                      remaining_data          = 0;
    T_USB_LUN                   *lun                    = NULL;

    UINT16  remaining_block;
    UINT16  max_block_per_xfer;
    UINT8   temp_op_index;

#if TEMP_WCP_PROF
    UINT8   *temp_ptr = (dbg_buffer + 6);
#endif

    switch (message_p->msg_id)
    {
    case USB_TX_BUFFER_EMPTY_MSG:
        /** temporary variables initialization  */
        request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request;
        lun     = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun];

        /** update the transfered size  */
        //request->transferred_size += request->command_size;//request->data_read_buff->buffer_req_size[request->data_read_buff->usb_op_buf_index];
        request->data_read_buff->usb_tx_count += request->command_size;

        request->data_read_buff->buffer_status[request->data_read_buff->usb_op_buf_index] = AVAILABLE;

#if TEMP_WCP_PROF
        prf_LogPointOfInterest("TX-COMPLETE");
#endif

/*----->> Check if we have to read more data as part of our read request----->>*/
/*----->> Check if previous GBI operation is over ----->>*/

        if(request->data_read_buff->buffer_status[request->data_read_buff->gbi_op_buf_index] == AVAILABLE)
        {
            if( request->data_read_buff->usb_tx_count <
                (request->data_read_buff->requested_tx_count) )
            {
                temp_op_index = request->data_read_buff->usb_op_buf_index;

                request->data_read_buff->usb_op_buf_index = request->data_read_buff->gbi_op_buf_index;

                request->data_read_buff->buffer_status[request->data_read_buff->usb_op_buf_index] = USB_OP;

                request->command_size = request->data_read_buff->gbi_tx_count;//request->buffer_size;
                // request->data_read_buff->buffer_req_size[request->data_read_buff->usb_op_buf_index];

                /** sends the data to the USB   */
                USBMS_SEND_TRACE("USBMS: Send Data to USB", RV_TRACE_LEVEL_ERROR);


#if TEMP_WCP_PROF
                sprintf(temp_ptr,"%d",request->command_size);
                prf_LogPointOfInterest("Set Tx Buffer");
#endif
                status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                            EP_TX,
                                            request->data_read_buff->buffer[request->data_read_buff->usb_op_buf_index],
                                            request->command_size,
                                            FALSE );
                /** update the transaction parameters   */
                request->first_block_nmb    += request->current_block;
                remaining_block     = request->number_of_blocks - request->first_block_nmb;
                max_block_per_xfer  = MAX_BYTE_PER_READ_XFER / lun->partition_info.bytes_per_block;

                if (remaining_block > 0)    /** there is still data to get from the GBI */
                {
                    if(remaining_block > max_block_per_xfer)
                    {
                        request->current_block  = max_block_per_xfer;
                    }
                    else
                    {
                        request->current_block  = remaining_block;
                    }

                    request->buffer_size        = request->current_block * lun->partition_info.bytes_per_block;

                    request->data_read_buff->gbi_op_buf_index = temp_op_index;

                    request->data_read_buff->buffer_status[request->data_read_buff->gbi_op_buf_index]
                                                = GBI_OP;

                    request->data_read_buff->gbi_tx_count = request->buffer_size;

#if TEMP_WCP_PROF
                prf_LogPointOfInterest("GBI-Read");
#endif
                    /** reads some data blocks from the GBI */
                    status = gbi_read(  usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.media_nmb,
                                        PARTITION_NMB,
                                        request->first_block_nmb,
                                        request->current_block,
                                        0,
                                        (UINT32*)request->data_read_buff->buffer[request->data_read_buff->gbi_op_buf_index],
                                        usbms_env_ctrl_blk_p->return_path   );

                    /** the internal state changes to SEND_TO_USB_GET_FROM_GBI  */
                    usbms_change_internal_state(SEND_TO_USB_GET_FROM_GBI);
                    //usbms_change_internal_state(GET_FROM_GBI);
                }
                else
                {
                    usbms_change_internal_state(SEND_TO_USB);
                }
            }
            else    /** transaction completed   */
            {
                if(request->transferred_size == 512)
                {
                    request->transferred_size = 512;
                }
                /** computes the CBW status phase   */
                send_bot_status(GOOD_STATUS,0);
                /** the internal state changes to SBW_PROCESS   */
                usbms_change_internal_state(SBW_PROCESS);
            }
        }
        else
        {
            /** the internal state changes to GET_FROM_GBI  */
            usbms_change_internal_state(GET_FROM_GBI);
        }
        break;
    case USB_BUS_DISCONNECTED_MSG:
//      /** the usb is disconnected */
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_CTRL );
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_TX   );
//      /** the internal state changes to UNPLUGGED */
//      usbms_change_internal_state(UNPLUGGED);
//      break;
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    default:
        usbms_store_message (message_p);
        break;
    }
    return status;
}


/**
 * Function for managing the SEND_TO_GBI state
 *
 * The message in parameter is freed in this function.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_send_to_gbi(T_RV_HDR *message_p)
{
    T_RV_RET            status          = RV_OK;
    T_GBI_WRITE_RSP_MSG *gbi_write_rsp  = (T_GBI_WRITE_RSP_MSG*)message_p;
    T_USB_LUN           *lun            = NULL;
    T_USBMS_XFER        *request        = NULL;
    UINT32              remaining_data  = 0;
    UINT16  remaining_block;
    UINT16  max_block_per_xfer;



    switch (message_p->msg_id)
    {
    case GBI_WRITE_RSP_MSG:
        /** temporary variables initialization  */
        lun     = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun];
        request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request;

        max_block_per_xfer = MAX_BYTE_PER_XFER / lun->partition_info.bytes_per_block;

        if (gbi_write_rsp->result == RV_OK)     /** the write operation is successful   */
        {
            /** update the transfer parameters  */
            request->first_block_nmb +=request->current_block;
            remaining_block = request->number_of_blocks - request->first_block_nmb;
            if (remaining_block > 0)    /** there is still data to send to the GBI*/
            {

                if (remaining_block >= max_block_per_xfer)
                {
                    request->buffer_size = max_block_per_xfer * lun->partition_info.bytes_per_block;
                    request->current_block=max_block_per_xfer;
                }
                else
                {
                    request->buffer_size = remaining_block * lun->partition_info.bytes_per_block;
                    request->current_block=remaining_block;
                }

                /** Read the data from the USB */
                if (request->buffer_size >= MAX_TRANSFER_SIZE)
                {
                    request->command_size   = MAX_TRANSFER_SIZE;
                }
                else
                {
                    request->command_size   = request->buffer_size;
                }
                request->transferred_size=0;

                /** provides a buffer for receiving the data    */
                status = usb_set_rx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                            EP_RX,
                                            request->data_buff,
                                            request->command_size );

                /** the internal state changes to GET_FROM_USB  */
                usbms_change_internal_state(GET_FROM_USB);
            }
            else                        /** transaction completed   */
            {
                /** build and send the CBW status phase */
                send_bot_status(GOOD_STATUS,0);
                /** the internal state changes to SBW_PROCESS   */
                usbms_change_internal_state(SBW_PROCESS);
            }
        }
        else    /** an error occured during the write operation */
        {
            USBMS_SEND_TRACE("USBMS: GBI_WRITE_RSP_MSG report an error",RV_TRACE_LEVEL_ERROR);
            /** Set the error markup    */
            lun->fail_reason    = COMMAND_FAILED;//PHASE_ERROR;
            lun->fail_request   = TRUE;
/*###==> ANO-USBMS  08102004 YL*/
            /** fail the host write request */
            status = usb_set_rx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                                    EP_RX,
                                                    NULL,
                                                    1);

            /** indicate the mount of data processed */
            /** request->number_of_blocks is in fact the last bllock number*/
            remaining_block = request->number_of_blocks - request->first_block_nmb;
            remaining_data  = remaining_block*lun->partition_info.bytes_per_block;;
            /** build and send the CBW status phase */
            send_bot_status((T_COMMAND_BLOCK_STATUS)(lun->fail_reason),remaining_data);
/*<==### ANO-USBMS*/
            /** the internal state changes to SBW_PROCESS   */
            usbms_change_internal_state(SBW_PROCESS);
        }
        break;
    case USB_BUS_DISCONNECTED_MSG:
//      /** the usb is disconnected */
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_CTRL );
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_TX   );
//      /** the internal state changes to UNPLUGGED */
//      usbms_change_internal_state(UNPLUGGED);
//      break;
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    default:
        usbms_store_message (message_p);
        break;
    }
    return status;
}


/**
 * Function for managing the SBW state
 *
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_sbw_process(T_RV_HDR *message_p)
{
    T_RV_RET                    status              = RV_OK;
    UINT8                       current_lun         = 0;
    UINT8                       dbg_val;
    T_USB_TX_BUFFER_EMPTY_MSG   *usb_tx_buffer_empty    = NULL;



    switch (message_p->msg_id)
    {
    case USB_TX_BUFFER_EMPTY_MSG:   /** the CBW status phase is completed   */
        usb_tx_buffer_empty = (T_USB_TX_BUFFER_EMPTY_MSG *)message_p;

        /*debug only, set a strategic breakpoint*/
        //DBG YL 10132004
        if(usbms_env_ctrl_blk_p->usbms_specific.usb_lun[current_lun].fail_request != FALSE)
            dbg_val = 1;

        /** Initialize the temporary variable   */
        current_lun = usbms_env_ctrl_blk_p->usbms_specific.current_lun;

        /** reset some parameters   */
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[current_lun].fail_reason = GOOD_STATUS;
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[current_lun].fail_request= FALSE;

        /** provides a buffer for the next RBC request  */
        usbms_env_ctrl_blk_p->usbms_specific.buffer_rx_temp.size = MAX_COMMAND_SIZE;
        status = usb_set_rx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                    EP_RX,
                                    (UINT8*)usbms_env_ctrl_blk_p->usbms_specific.buffer_rx_temp.buff,
                                    usbms_env_ctrl_blk_p->usbms_specific.buffer_rx_temp.size    );
        //DBG YL 10132004
        if(status != RV_OK)
        {
            USBMS_SEND_TRACE("USBMS: usbms_sbw_process : no RX buffer",RV_TRACE_LEVEL_ERROR);
            USBMS_TRACE_PARAM("USBMS: usb_set_rx_buffer : status = ", status);
        }

        /** the internal state changes to IDLE  */
        usbms_change_internal_state(IDLE);
        break;
    case USB_BUS_DISCONNECTED_MSG:
//      /** the usb is disconnected     */
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_CTRL );
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_TX   );
//      /** the inetrnal state changes to UNPLUGGED */
//      usbms_change_internal_state(UNPLUGGED);
//      break;
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    default:
        usbms_store_message (message_p);
        break;
    }
    return status;
}


/**
 * Function for managing the REINIT_USB state
 *
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_reinit_usb(T_RV_HDR *message_p)
{
    T_RV_RET                        status                      = RV_OK;
    T_USB_FM_RESULT_MSG             *usb_fm_subscribe_msg       = NULL;
    T_USB_RX_BUFFER_FULL_MSG        *usb_rx_buffer_full_msg     = NULL;


    switch (message_p->msg_id)
    {
//  case USB_RX_BUFFER_FULL_MSG:
//      usb_rx_buffer_full_msg = (T_USB_RX_BUFFER_FULL_MSG *)message_p;
//      if(usb_rx_buffer_full_msg->endpoint == EP_CTRL)
//      {
//          /** reclaim the buffer */
//          usb_reclaim_rx_buffer(usbms_env_ctrl_blk_p->interface_id, EP_RX);
//      }
//      else
//      {
//          /** Unsubscribe from the USB */
//          usb_fm_unsubscribe(usbms_env_ctrl_blk_p->interface_id);
//      }
//      break;
    case USB_FM_RESULT_MSG:
        /** ask for the gbi media info  */
        //gbi_get_media_info(   usbms_env_ctrl_blk_p->prim_mb_id,
        //                  usbms_env_ctrl_blk_p->return_path   );
        /** REINIT completed, change the state to INIT  */
        //usbms_change_internal_state(INIT);
        status = usb_fm_subscribe(  usbms_env_ctrl_blk_p->interface_id,
                                    usbms_env_ctrl_blk_p->return_path   );
        usbms_change_internal_state(UNPLUGGED);
        break;
    default:
        usbms_store_message (message_p);
        break;
    }
    return USBMS_OK;
}

/**
 * Function that formats and send the BOT status
 *
 *
 * @param   command_block_status    Status of the BOT
 * @param   data_residue            amount of data that have not been processed
 * @return  status of the operation.
 */
T_RV_RET send_bot_status(   T_COMMAND_BLOCK_STATUS  command_block_status,
                            UINT32                  data_residue    )
{
    USBMS_SEND_TRACE("USBMS: send_bot_status",RV_TRACE_LEVEL_DEBUG_LOW);
    USBMS_TRACE_PARAM("USBMS: Status = ", command_block_status);
    USBMS_TRACE_PARAM("USBMS: data residue = ", data_residue);

    /** build the CSW structure */
    //usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWSignature        = CSW_SIGNATURE;
    usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWSignature_0    = 0x55;
    usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWSignature_1    = 0x53;
    usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWSignature_2    = 0x42;
    usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWSignature_3    = 0x53;
    // usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWTag         = usbms_env_ctrl_blk_p->usb_lun.bot_tag;
    //usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWDataResidue  = data_residue;
    usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWDataResidue_0  = data_residue&0xff;
    usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWDataResidue_1  = (data_residue&0xff00)>>8;
    usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWDataResidue_2  = (data_residue&0xff0000)>>16;
    usbms_env_ctrl_blk_p->usbms_specific.csw.dCSWDataResidue_3  = (data_residue&0xff000000)>>24;
    usbms_env_ctrl_blk_p->usbms_specific.csw.bCSWStatus         = command_block_status;

    /** reset the bot state */
    /*
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.buffer_size        = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.current_block      = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.first_block_nmb    = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.number_of_blocks   = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.number_of_blocks   = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.transferred_size   = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.command_size       = 0;

    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request.buffer_size         = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request.current_block       = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request.first_block_nmb     = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request.number_of_blocks    = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request.number_of_blocks    = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request.transferred_size    = 0;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request.command_size        = 0;
    */


    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].fail_reason  = GOOD_STATUS;
    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].fail_request = FALSE;


    /** sends the BOT status    */
    return usb_set_tx_buffer(   usbms_env_ctrl_blk_p->interface_id,
                                EP_TX,
                                (UINT8*)&usbms_env_ctrl_blk_p->usbms_specific.csw,
                                13/*sizeof(T_USBMS_CSW)*/,
                                FALSE   );
}





/** RBC command processing  */


/**
 * Function that processes the rbc inquiry command
 *
 *
 * @param   inquiry:    Structure defining the inquiry command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_inquiry(T_USBMS_INQUIRY *inquiry)
{
    T_USBMS_RETURN  status = USBMS_OK;
    UINT32          size;


    size = sizeof(T_USBMS_INQUIRY_DATA);

    if (inquiry->enable_vpd==0 && inquiry->page_code==0 )
    {
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.buffer_size        = 0;
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.current_block      = 0;
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.first_block_nmb    = 0;
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.number_of_blocks   = 0;
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.number_of_blocks   = 0;
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.transferred_size   = 0;
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.command_size       = 0;
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].fail_reason  = GOOD_STATUS;
        if(usbms_env_ctrl_blk_p->usbms_specific.current_lun == 0){
            strcpy((char*)usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.product_revision_level,"1000");
        }else
        {
            strcpy((char*)usbms_env_ctrl_blk_p->usbms_specific.inquiry_data.product_revision_level,"1001");
        }


        /** standard inquiry data are required  */
        status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                    EP_TX,
                                    (UINT8*)&usbms_env_ctrl_blk_p->usbms_specific.inquiry_data,
                                    inquiry->length /*sizeof(T_USBMS_INQUIRY_DATA)*/,
                                    FALSE   );
    }
    else
    {
/*Defect fix for OMAPS00097663 send status as failed*/
            send_bot_status(COMMAND_FAILED/*PHASE_ERROR*/,0);
            usbms_change_internal_state(SBW_PROCESS);
    }
//  if (inquiry->enable_vpd==1 && inquiry->page_code==0x80)
//  {
//      /** serial page vital product data are required */
//      status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
//                                  EP_TX,
//                                  (UINT8*)&usbms_env_ctrl_blk_p->usbms_specific.serial_number_page,
//                                  sizeof(T_USBMS_SERIAL_PAGE),
//                                  FALSE   );
//  }

//  if (inquiry->enable_vpd==1 && inquiry->page_code==0x83)
//  {
//      /** device ID page vital product data are required  */
//      status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
//                                  EP_TX,
//                                  (UINT8*)&usbms_env_ctrl_blk_p->usbms_specific.device_id_page,
//                                  sizeof(T_USBMS_DEVICE_ID_PAGE),
//                                  FALSE   );
//  }

    return status;
}



/**
 * Function that processes the rbc MODE_SELECT command
 *
 *
 * @param   mode_select :   Structure defining the MODE_SELECT command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_mode_select(T_USBMS_MODE_SELECT *mode_select)
{
    return USBMS_OK;
}



/**
 * Function that processes the rbc MODE_SENSE command
 *
 *
 * @param   mode_sense  :   Structure defining the MODE_SENSE command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_mode_sense(T_USBMS_MODE_SENSE *mode_sense)
{
    return usb_set_tx_buffer(   usbms_env_ctrl_blk_p->interface_id,
                                EP_TX,
                                (UINT8*)&usbms_env_ctrl_blk_p->usbms_specific.mode_parameter_head,
                                sizeof(T_USBMS_MODE_PARAMETER_HEAD),
                                (mode_sense->allocation_length>sizeof(T_USBMS_MODE_PARAMETER_HEAD))?TRUE:FALSE  );
}



/**
 * Function that processes the rbc PREVENT_ALLOW_REMOVAL command
 *
 *
 * @param   media_removal   :   Structure defining the PREVENT_ALLOW_REMOVAL command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_prevent_allow_removal(T_USBMS_MEDIA_REMOVAL *media_removal)
{
#define LUN usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun]
    /** storing the removal capability  */
    LUN.media_state = media_removal->prevent;
    return USBMS_OK;
#undef LUN
}



/**
 * Function that processes the rbc START_STOP_UNIT command
 *
 *
 * @param   start_stop_unit :   Structure defining the START_STOP_UNIT command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_start_stop_unit(T_USBMS_START_STOP_UNIT *start_stop_unit)
{
    //start_stop_unit->flags&PWR_CONDITION
    //start_stop_unit->flags&START
    return USBMS_OK;
}



/**
 * Function that processes the rbc READ_CAPACITY command
 *
 *
 * @param   read_capacity   :   Structure defining the READ_CAPACITY command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_read_capacity(T_USBMS_READ_CAPACITY *read_capacity)
{
    UINT32  last_block_nmb;
    UINT16  block_size;

    /** fill the read capability structure  */
    last_block_nmb  = usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.nmb_of_blocks - 1;
    block_size      = usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.bytes_per_block;

    usbms_env_ctrl_blk_p->usbms_specific.read_capacity_data.block_address_0 = last_block_nmb&0x000000ff;
    usbms_env_ctrl_blk_p->usbms_specific.read_capacity_data.block_address_1 = (last_block_nmb&0x0000ff00)>>8;
    usbms_env_ctrl_blk_p->usbms_specific.read_capacity_data.block_address_2 = (last_block_nmb&0x00ff0000)>>16;
    usbms_env_ctrl_blk_p->usbms_specific.read_capacity_data.block_address_3 = (last_block_nmb&0xff000000)>>24;
    usbms_env_ctrl_blk_p->usbms_specific.read_capacity_data.block_len_0     = block_size&0x00ff;
    usbms_env_ctrl_blk_p->usbms_specific.read_capacity_data.block_len_1     = (block_size&0xff00)>>8;
    usbms_env_ctrl_blk_p->usbms_specific.read_capacity_data.block_len_2     = 0;
    usbms_env_ctrl_blk_p->usbms_specific.read_capacity_data.block_len_3     = 0;


    if (USBMS_OK == rbc_test_unit_ready(NULL))
    {
        //usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].fail_reason= GOOD_STATUS;
        //usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.buffer_size = 0;
        //usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.transferred_size = 0;
        /** send the read capability structure  */
        /*return */

        USBMS_TRACE_PARAM("USBMS: rbc_read_capacity - last_block_nmb = ", last_block_nmb);
        USBMS_TRACE_PARAM("USBMS: rbc_read_capacity - block_size = ", block_size);

        usb_set_tx_buffer(  usbms_env_ctrl_blk_p->interface_id,
                                    EP_TX,
                                    (UINT8*)&usbms_env_ctrl_blk_p->usbms_specific.read_capacity_data,
                                    sizeof(T_USBMS_READ_CAPACITY_DATA),
                                    FALSE   );
        usbms_change_internal_state(USB_SEND_NO_GBI);

    }
    else
    {

        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].fail_reason=COMMAND_FAILED;
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.buffer_size = sizeof(T_USBMS_READ_CAPACITY_DATA);
        usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request.transferred_size = 0;
        /** send the read capability structure  */

        /*return*/ usb_set_tx_buffer(   usbms_env_ctrl_blk_p->interface_id,
                                    EP_TX,
                                    (UINT8*)&usbms_env_ctrl_blk_p->usbms_specific.read_capacity_data,
                                    0,
                                    TRUE    );
        usbms_change_internal_state(USB_SEND_NO_GBI);


        /*
        usb_set_tx_buffer(  usbms_env_ctrl_blk_p->interface_id,
                                    EP_TX,
                                    NULL,
                                    sizeof(T_USBMS_READ_CAPACITY_DATA),
                                    TRUE    );

        send_bot_status(COMMAND_FAILED,sizeof(T_USBMS_READ_CAPACITY_DATA));
        usbms_change_internal_state(SBW_PROCESS);
        */
    }

    return USBMS_OK;
}



/**
 * Function that processes the rbc TEST_UNIT_READY command
 *
 *
 * @param   test_unit_ready :   Structure defining the TEST_UNIT_READY command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_test_unit_ready(T_USBMS_TEST_UNIT_READY *test_unit_ready)
{
#define LUN usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun]
    if (    (LUN.media_info.media_pressent == TRUE)
            &&(usbms_env_ctrl_blk_p->usbms_specific.media_number != 0)  )
    {
        /** the storage device is present   */
        USBMS_SEND_TRACE("USBMS: READY",RV_TRACE_LEVEL_DEBUG_LOW);
        return USBMS_OK;
    }
    else
    {
        /** the storage device is not present   */
        USBMS_SEND_TRACE("USBMS: NOT READY",RV_TRACE_LEVEL_DEBUG_LOW);
        USBMS_TRACE_PARAM("USBMS: LUN.media_info.media_pressent = ", LUN.media_info.media_pressent);
        USBMS_TRACE_PARAM("USBMS: usbms_env_ctrl_blk_p->usbms_specific.media_number = ", usbms_env_ctrl_blk_p->usbms_specific.media_number);
        return (T_USBMS_RETURN)USBMS_ERROR;
    }
#undef LUN
}



/**
 * Function that processes the rbc READ_10 command
 *
 *
 * @param   read_10 :   Structure defining the READ_10 command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_read_10(T_USBMS_READ_10 *read_10)
{
    T_USBMS_XFER    *request            = NULL;
    UINT16          max_block_per_xfer  = 0;
    T_USB_LUN       *lun                = NULL;
    UINT16          number_of_block     = 0;


    /** initialize the temporary variables  */
    request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].read_request;
    lun     = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun];



    /** Initialization of the request parameter */
    request->first_block_nmb    =   VALUE_32(   read_10->block_address_0,
                                                read_10->block_address_1,
                                                read_10->block_address_2,
                                                read_10->block_address_3    );

    request->number_of_blocks   =   request->first_block_nmb + VALUE_32(    read_10->xfer_len_0,
                                                                            read_10->xfer_len_1,
                                                                            0,
                                                                            0   );


    /** definition of the number of block to read from GBI  */
    max_block_per_xfer  = MAX_BYTE_PER_READ_XFER / lun->partition_info.bytes_per_block;
    number_of_block     = request->number_of_blocks - request->first_block_nmb;

    request->data_read_buff->requested_tx_count = number_of_block * lun->partition_info.bytes_per_block;

    if (number_of_block >= max_block_per_xfer)
    {
        request->current_block      = max_block_per_xfer;
    }
    else
    {
        request->current_block      = number_of_block;//request->number_of_blocks;
    }

    request->transferred_size   = 0;
    request->buffer_size        = request->current_block * lun->partition_info.bytes_per_block;
    request->command_size       = 0;/*request->number_of_blocks
                                * usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.bytes_per_block;
                                */
    request->data_read_buff->usb_tx_count       = 0;

    request->data_read_buff->gbi_tx_count       = request->buffer_size;

    /* --- Start of a new read operation -- */
    request->data_read_buff->gbi_op_buf_index = 0;
    request->data_read_buff->usb_op_buf_index = 1;

    request->data_read_buff->buffer_status[request->data_read_buff->gbi_op_buf_index] = GBI_OP;
    request->data_read_buff->buffer_status[request->data_read_buff->usb_op_buf_index] = AVAILABLE;

#if TEMP_WCP_PROF
                prf_LogPointOfInterest("GBI-Read");
#endif
    /** read a part or all the data from the gbi    */
    return gbi_read(usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].partition_info.media_nmb,
                    PARTITION_NMB,
                    request->first_block_nmb,
                    request->current_block,
                    0,
                    (UINT32*)request->data_read_buff->buffer[request->data_read_buff->gbi_op_buf_index],
                    usbms_env_ctrl_blk_p->return_path);
}


/**
 * Function that processes the rbc WRITE_10 command
 *
 *
 * @param   write_10    :   Structure defining the WRITE_10 command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_write_10(T_USBMS_WRITE_10 *write_10)
{
    T_USBMS_XFER    *request            = NULL;
    UINT16          max_block_per_xfer  = 0;
    T_USB_LUN       *lun                = NULL;
    UINT16          nmb_of_block        = 0;



    /** initialize the temporary variables  */
    request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request;
    lun     = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun];


    /** Transaction Initialization  */
    request->first_block_nmb =  VALUE_32(   write_10->block_address_0,
                                            write_10->block_address_1,
                                            write_10->block_address_2,
                                            write_10->block_address_3   );

    request->number_of_blocks=  request->first_block_nmb + VALUE_32(    write_10->xfer_len_0,
                                                                        write_10->xfer_len_1,
                                                                        0,
                                                                        0   );

    nmb_of_block    = (write_10->xfer_len_1<<8)|write_10->xfer_len_0;

    max_block_per_xfer = MAX_BYTE_PER_XFER / lun->partition_info.bytes_per_block;

    if (/*request->number_of_blocks*/nmb_of_block >= max_block_per_xfer)
    {
        request->buffer_size = max_block_per_xfer * lun->partition_info.bytes_per_block;
        request->current_block=max_block_per_xfer;
    }
    else
    {
        request->buffer_size = /*request->number_of_blocks*/nmb_of_block * lun->partition_info.bytes_per_block;
        request->current_block=/*request->number_of_blocks*/nmb_of_block;
    }


    /** Read the data from the USB */
    if (request->buffer_size >= MAX_TRANSFER_SIZE)
    {
        request->command_size   = MAX_TRANSFER_SIZE;
    }
    else
    {
        request->command_size   = request->buffer_size;
    }
    request->transferred_size=0;

    /** provides a buffer for receiving the data    */
    return usb_set_rx_buffer(   usbms_env_ctrl_blk_p->interface_id,
                                EP_RX,
                                request->data_buff,
                                request->command_size );
}



/**
 * Function that processes the rbc VERIFY command
 *
 *
 * @param   verify  :   Structure defining the VERIFY command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_verify(T_USBMS_VERIFY *verify)
{
    return USBMS_OK;
}

/**
 * Function that processes the rbc REQUEST_SENSE command
 *
 *
 * @param   request_sense   :   Structure defining the REQUEST_SENSE command
 * @return  status of the operation.
 */
T_USBMS_RETURN rbc_request_sense(T_USBMS_REQUEST_SENSE *request_sense)
{
    T_USBMS_XFER    *request    = NULL;
    T_USBMS_RETURN  status      = USBMS_OK;

    request = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun].write_request;
    memset(request->data_buff,0,0x12);

    request->data_buff[0]   = 0x70;
    request->data_buff[2]   = 0x02;
    request->data_buff[7]   = 0x0c;
    request->data_buff[12]  = 0x3a;


    request->buffer_size = 0x12;
    request->transferred_size = 0x12;
    status = usb_set_tx_buffer( usbms_env_ctrl_blk_p->interface_id,
                                EP_TX,
                                request->data_buff,
                                request->transferred_size,
                                FALSE   );
    return status;
}



/** Class Request   */

/**
 * Function that processes the "Get Max Lun" class request
 *
 *
 * @return  status of the operation.
 */
T_USBMS_RETURN cr_get_max_lun(void)
{
    /** retrieves the number of LUN manage by by the USB-MS */
    //usbms_env_ctrl_blk_p->usbms_specific.class_request.data = usbms_env_ctrl_blk_p->usbms_specific.media_number-1;
    usbms_env_ctrl_blk_p->usbms_specific.class_request.data = MAX_LUN_NUM-1;
    //USBMS_TRACE_PARAM("USBMS:Max Lun = ", usbms_env_ctrl_blk_p->usbms_specific.media_number);
    return usb_set_tx_buffer(   usbms_env_ctrl_blk_p->interface_id,
                                EP_CTRL,
                                &usbms_env_ctrl_blk_p->usbms_specific.class_request.data,
                                1,FALSE );
}


/**
 * Function that processes the "Mass Storage Reset" class request
 *
 *
 * @return  status of the operation.
 */
T_USBMS_RETURN cr_mass_storage_reset(void)
{
/*###==> ANO-USBMS  08102004 YL*/
/*add some coherancy here*/
    T_USB_LUN           *lun            = NULL;

    lun     = &usbms_env_ctrl_blk_p->usbms_specific.usb_lun[usbms_env_ctrl_blk_p->usbms_specific.current_lun];
    lun->fail_request   = FALSE;
/*<==### ANO-USBMS*/

    return usb_set_rx_buffer(   usbms_env_ctrl_blk_p->interface_id,
                                EP_CTRL,
                                (UINT8*)&usbms_env_ctrl_blk_p->usbms_specific.class_request,
                                sizeof(T_USBMS_CLASS_REQUEST) );
}



/**
 * Function that manage the USB "DISCONNECT" event and the GBI "INSERTION/REMOVEAL" event
 *
 *
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_restart(T_RV_HDR *message_p)
{
    T_RV_RET    status  = RV_OK;
	int i=0;

    USBMS_SEND_TRACE("USBMS: Restart",RV_TRACE_LEVEL_ERROR);

    if (message_p->msg_id == GBI_EVENT_IND_MSG) /** GBI Event   */
    {
        switch(((T_GBI_EVENT_MSG*)message_p)->event)
        {
        case GBI_EVENT_MEDIA_INSERT:
            usbms_env_ctrl_blk_p->media_change_info |= USBMS_MEDIA_INSERTED;
            status = gbi_get_media_info(    usbms_env_ctrl_blk_p->prim_mb_id,
                                usbms_env_ctrl_blk_p->return_path   );
            break;
        case GBI_EVENT_MEDIA_REMOVEAL:
            //usbms_env_ctrl_blk_p->media_change_info |= USBMS_MEDIA_REMOVED;
            for(i=0;i<usbms_env_ctrl_blk_p->usbms_specific.media_number;i++)
            {
			    if(usbms_env_ctrl_blk_p->usbms_specific.usb_lun[i].media_info.media_type == GBI_MMC ||
				    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[i].media_info.media_type == GBI_SD )
                {
                    usbms_env_ctrl_blk_p->usbms_specific.usb_lun[i].media_info.media_pressent = FALSE;
                }
            }

            for(;usbms_env_ctrl_blk_p->usbms_specific.usb_lun[--i].media_info.media_pressent == FALSE;
            usbms_env_ctrl_blk_p->usbms_specific.media_number--){;}
            //usbms_env_ctrl_blk_p->media_change_info &= (~USBMS_MEDIA_REMOVED);
                break;
        default:
            break;
        }

#if 0		
    	/** ask for the gbi media info  */
        status = gbi_get_media_info(    usbms_env_ctrl_blk_p->prim_mb_id,
                                        usbms_env_ctrl_blk_p->return_path   );
        /** REINIT completed, change the state to INIT  */
        usbms_change_internal_state(REINIT_GBI);
#endif		
    }
    else                                        /** USB Event */
    {
        /** Unsubscribe from the USB */
//      status = usb_fm_unsubscribe(usbms_env_ctrl_blk_p->interface_id);
//      if (status == RV_OK)
//      {
//          /** Set the internal state to REINIT_USB    */
//          usbms_change_internal_state(REINIT_USB);
//      }
//      else
//      {
//          /** ask for the gbi media info  */
//          //status = gbi_get_media_info(  usbms_env_ctrl_blk_p->prim_mb_id,
//          //                              usbms_env_ctrl_blk_p->return_path   );
//          /** REINIT completed, change the state to INIT  */
//          //usbms_change_internal_state(INIT);
//          usbms_change_internal_state(UNPLUGGED);
//      }
        usbms_change_internal_state(UNPLUGGED);
    }
    return status;
}


/**
 * Function for managing the SEND_TO_USB_GET_FROM_GBI state
 *
 * The message in parameter is freed in this function.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  status of the operation.
 */
T_USBMS_RETURN usbms_send_to_usb_get_from_gbi(T_RV_HDR *message_p)
{
    T_RV_RET            status          = RV_OK;
    T_GBI_READ_RSP_MSG  *gbi_read_rsp   = (T_GBI_READ_RSP_MSG   *)message_p;
    T_USBMS_XFER        *request        = NULL;

    switch (message_p->msg_id)
    {
    case GBI_READ_RSP_MSG:

        USBMS_SEND_TRACE("USBMS: S_T_U_G_F_G GBI_RD_RSP", RV_TRACE_LEVEL_DEBUG_LOW);
        status = usbms_get_from_gbi(message_p);
        break;

    case USB_TX_BUFFER_EMPTY_MSG:

        USBMS_SEND_TRACE("USBMS: S_T_U_G_F_G USB Tx Empty ", RV_TRACE_LEVEL_DEBUG_LOW);
        status = usbms_send_to_usb(message_p);
        break;

    case USB_BUS_DISCONNECTED_MSG:
//      /** the usb is disconnected */
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_CTRL );
//      usb_reclaim_rx_buffer(  usbms_env_ctrl_blk_p->interface_id, EP_TX   );
//      /** the internal state changes to UNPLUGGED */
//      usbms_change_internal_state(UNPLUGGED);
//      break;
    case GBI_EVENT_IND_MSG:
        status = usbms_restart(message_p);
        break;
    default:
        usbms_store_message (message_p);
        break;
    }
    return status;
}


