/**
 * @file  dma_i.c
 *
 * API for DMA SWE.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/2/2003   ()   Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "rvf/rvf_api.h"
#include "camd/camd_api.h"
#include "camd/camd_i.h"
#include "camd/camd_operations.h"
#include "camd/camcore/camcore_api.h"
#include "camd/camcore/camcore_hwapi.h"
#include "dma/board/dma_inth.h"

#include "dma/dma_i.h"
#include "dma/dma_api.h"
#include "dma/dma_message.h"
#include "inth/sys_inth.h"


#ifndef _WINDOWS
#include "dma/sys_dma.h"

#include "nucleus.h"
#include "inth/iq.h"
#include "chipset.cfg"
#endif


/**
 * Pointer on the structure gathering all the global variables
 * used by DMA instance.
 */
extern T_DMA_ENV_CTRL_BLK* dma_env_ctrl_blk_p;
extern UINT8 dma_channel_number;
extern UINT16 dma_it_status;

T_DMA_QUEUE_ARRAY   dma_queue_array   [DMA_MAX_QUEUED_REQUESTS];
T_DMA_CHANNEL_ARRAY dma_channel_array [DMA_MAX_NUMBER_OF_CHANNEL];
UINT8 dma_function_status [DMA_MAX_NUMBER_OF_CHANNEL];

UINT8 dma_channel_number_tab[DMA_MAX_NUMBER_OF_CHANNEL];
UINT8 dma_it_status_tab[DMA_MAX_NUMBER_OF_CHANNEL];
UINT8 total_active_dma_channels;
#ifndef _WINDOWS

/* Array of pointer on call back function with argument SYS_UWORD16. */
/* The size of the array is defined by the number of DMA channels available on the device */
T_DMA_CALL_BACK dma_call_back_address[DMA_MAX_NUMBER_OF_CHANNEL]=
{dma_call_back_it_chan0,
    dma_call_back_it_chan1,
    dma_call_back_it_chan2,
    dma_call_back_it_chan3,
    dma_call_back_it_chan4,
    dma_call_back_it_chan5
};
#endif

/**
 * function: dma_send_status_message
 *
 * this function will send a status message back to the client
 */
void dma_send_status_message(T_DMA_CHANNEL channel,
        T_DMA_RET status,T_RV_RETURN return_path)
{
    T_RVF_MB_STATUS mb_status;
    T_DMA_STATUS_RSP_MSG* dma_event;

    /* Subscriber must be notified */
    mb_status = rvf_get_msg_buf (dma_env_ctrl_blk_p->prim_mb_id,
            sizeof(T_DMA_STATUS_RSP_MSG),
            DMA_STATUS_RSP_MSG,
            (T_RV_HDR **) &dma_event);

    if (mb_status != RVF_RED) /* Memory allocation success */
    {
        /* Fill the message */
        dma_event->hdr.msg_id     = DMA_STATUS_RSP_MSG;
        dma_event->result.channel = channel;
        dma_event->result.status  = status;

        /* Send message to the client */
        if (return_path.callback_func != 0)
        {
            return_path.callback_func((void*) dma_event);
#ifdef DMA_FREE_MESSAGE
            rvf_free_buf(dma_event);
#endif
        }
        else
        {
            rvf_send_msg(return_path.addr_id, dma_event);
        }

    }
    else
    {
        DMA_SEND_TRACE("DMA Memory allocation error", DMA_TRACE_LEVEL);
    }
}


T_DMA_RET dma_channel_check (T_DMA_CHANNEL channel)
{
    /*
       This function does check the channel parameters.
       Returns DMA_PARAMS_OK is parameters are ok and returns
       DMA_INVALID_PARAMETER is not correct.
       */

    if ((channel >= DMA_MIN_CHANNEL) && (channel <= DMA_MAX_CHANNEL))
    {
        return DMA_PARAMS_OK;
    }
    else
    {
        DMA_SEND_TRACE_PARAM("DMA parameter Channel has invalid value", channel,
                DMA_TRACE_LEVEL);

        return DMA_INVALID_PARAMETER;
    }

}


T_DMA_RET dma_queue_spec_prio_check (T_DMA_QUEUE queue, T_DMA_QUEUE specific,
        T_DMA_SW_PRIORITY sw_priority)
{
    T_DMA_RET return_value = DMA_PARAMS_OK;

    /*
       This function does check the 'queue', 'specific' and 'sw_priority'
       parameter. Returns DMA_PARAMS_OK is parameters are ok and returns
       DMA_INVALID_PARAMETER is not correct.
       */

    switch (queue)
    {
        case DMA_QUEUE_DISABLE:
        case DMA_QUEUE_ENABLE:
            break;

        default:
            {
                return_value = DMA_INVALID_PARAMETER;
                DMA_SEND_TRACE("DMA Invalid parameter for dma_queue",
                        DMA_TRACE_LEVEL);
            }
            break;
    }

    switch (specific)
    {
        case DMA_CHAN_ANY:
        case DMA_CHAN_SPECIFIC:
            break;

        default:
            {
                return_value = DMA_INVALID_PARAMETER;
                DMA_SEND_TRACE("DMA Invalid parameter for dma_specific",
                        DMA_TRACE_LEVEL);
            }
            break;
    }

    /*
       Next code is skipped now as this is always true as long as
       DMA_SW_PRIORITY_HIGHEST == 0 and DMA_SW_PRIORITY_LOWEST == 255
       */

    /*
       if ((sw_priority < DMA_SW_PRIORITY_HIGHEST) !!
       (sw_priority > DMA_SW_PRIORITY_LOWEST))
       {
       return_value = DMA_INVALID_PARAMETER;
       DMA_SEND_TRACE("DMA Invalid parameter for dma_sw_priority",
       DMA_TRACE_LEVEL);

       }
       */

    return return_value;
}


T_DMA_RET dma_check_channel_info (T_DMA_CHANNEL_PARAMETERS *channel_info)
{
    T_DMA_RET return_value = DMA_PARAMS_OK;

    /*
       This function checks the channel_info struct
       */

    if ((channel_info->sync >= DMA_SYNC_DEVICE_MAX) ||
            (channel_info->nmb_frames   == 0) ||
            (channel_info->nmb_elements == 0))
    {
        return_value = DMA_INVALID_PARAMETER;
        DMA_SEND_TRACE("DMA Invalid parameter for nmb_frames, nmb_elements or dma_sync_devicee",
                DMA_TRACE_LEVEL);
    }
    else
    {
        switch (channel_info->data_width)
        {
            case DMA_DATA_S8:
                break;

            case DMA_DATA_S16:
                if (((channel_info->source_address % DMA_TWO_BYTES) != DMA_ZERO) ||
                        ((channel_info->destination_address % DMA_TWO_BYTES) != DMA_ZERO))
                {
                    DMA_SEND_TRACE("DMA source or destination adress not correct aligned according to data width (16 bits) ",
                            DMA_TRACE_LEVEL);
                    return_value = DMA_INVALID_PARAMETER;
                }
                break;

            case DMA_DATA_S32:
                if (((channel_info->source_address % DMA_FOUR_BYTES) != DMA_ZERO) ||
                        ((channel_info->destination_address % DMA_FOUR_BYTES) != DMA_ZERO))
                {
                    DMA_SEND_TRACE("DMA source or destination adress not correct aligned according to data width (32 bits) ",
                            DMA_TRACE_LEVEL);
                    return_value = DMA_INVALID_PARAMETER;
                }
                break;

            default:
                {
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_width",
                            DMA_TRACE_LEVEL);
                    return_value = DMA_INVALID_PARAMETER;
                }
                break;
        }

        switch (channel_info->hw_priority)
        {
            case DMA_HW_PRIORITY_LOW:
            case DMA_HW_PRIORITY_HIGH:
                break;

            default:
                return_value = DMA_INVALID_PARAMETER;
                DMA_SEND_TRACE("DMA Invalid parameter for dma_priority",
                        DMA_TRACE_LEVEL);
                break;
        }

        switch (channel_info->dma_mode)
        {
            case DMA_SINGLE:
            case DMA_CONTINIOUS:
                break;

            default:
                {
                    return_value = DMA_INVALID_PARAMETER;
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_repeat",
                            DMA_TRACE_LEVEL);
                }
                break;
        }

        switch (channel_info->flush)
        {
            case DMA_FLUSH_DISABLED:
            case DMA_FLUSH_ENABLED:
                break;

            default:
                {
                    return_value = DMA_INVALID_PARAMETER;
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_flush", DMA_TRACE_LEVEL);
                }
                break;
        }

        switch (channel_info->dma_end_notification)
        {
            case DMA_NO_NOTIFICATION:
            case DMA_NOTIFICATION:
                break;

            default:
                {
                    return_value = DMA_INVALID_PARAMETER;
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_end_notification",
                            DMA_TRACE_LEVEL);
                }
                break;
        }

        switch (channel_info->secure)
        {
            case DMA_NOT_SECURED:
            case DMA_SECURED:
                break;

            default:
                {
                    return_value = DMA_INVALID_PARAMETER;
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_secure", DMA_TRACE_LEVEL);
                }
                break;
        }

        switch (channel_info->source_address_mode)
        {
            case DMA_ADDR_MODE_CONSTANT:
            case DMA_ADDR_MODE_POST_INC:
            case DMA_ADDR_MODE_FRAME_INDEX:
                break;

            default:
                {
                    return_value = DMA_INVALID_PARAMETER;
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_source_adress_mode",
                            DMA_TRACE_LEVEL);
                    break;
                }
        }

        switch (channel_info->destination_address_mode)
        {
            case DMA_ADDR_MODE_CONSTANT:
            case DMA_ADDR_MODE_POST_INC:
            case DMA_ADDR_MODE_FRAME_INDEX:
                break;

            default:
                {
                    return_value = DMA_INVALID_PARAMETER;
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_destination_adress_mode",
                            DMA_TRACE_LEVEL);
                    break;
                }
        }

        switch (channel_info->source_packet)
        {
            case DMA_NOT_PACKED:
            case DMA_PACKED:
                break;

            default:
                {
                    return_value = DMA_INVALID_PARAMETER;
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_source_packet",
                            DMA_TRACE_LEVEL);
                }
                break;
        }

        switch (channel_info->destination_packet)
        {
            case DMA_NOT_PACKED:
            case DMA_PACKED:
                break;

            default:
                {
                    return_value = DMA_INVALID_PARAMETER;
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_destination_adress_mode",
                            DMA_TRACE_LEVEL);
                }
                break;
        }

        switch (channel_info->source_burst)
        {
            case DMA_NO_BURST:
                break;
                /* If burst is enabled, the source adress must be in the IMIF range */
            case DMA_BURST_ENABLED:
                if ((dma_determine_port (channel_info->source_address)) == DMA_IMIF_PORT)
                {
                    break;
                }
                
                /* Fall through when address is not in IMIF range */
            default:
                {
                    return_value = DMA_INVALID_PARAMETER;
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_source_burst",
                            DMA_TRACE_LEVEL);
                    DMA_SEND_TRACE("DMA Note: burst mode is only available on the IMIF port",
                            DMA_TRACE_LEVEL);
                }
                break;
        }

        switch (channel_info->destination_burst)
        {
            case DMA_NO_BURST:
                break;
                /* If burst is enabled, the destination adress must be in the IMIF range */
            case DMA_BURST_ENABLED:
		  break;		
               /* The following check has been removed as burst mode is now working fine , and it is needed for 
                camera snapshot of 1.3 MP*/
	        /* if ((dma_determine_port (channel_info->destination_address)) == DMA_IMIF_PORT)
                {
                    break;
                }*/
                /* Fall through when address is not in IMIF range */
            default:
                {
                    return_value = DMA_INVALID_PARAMETER;
                    DMA_SEND_TRACE("DMA Invalid parameter for dma_destination_burst",
                            DMA_TRACE_LEVEL);
                    DMA_SEND_TRACE("DMA Note: burst mode is only available on the IMIF port",
                            DMA_TRACE_LEVEL);
                }
                break;
        }

        switch (channel_info->dma_end_notification)
        {
            case DMA_NOTIFICATION:
            case DMA_NO_NOTIFICATION:
                break;

            default:
                return_value = DMA_INVALID_PARAMETER;
                DMA_SEND_TRACE("DMA Invalid parameter for dma_end_notification",
                        DMA_TRACE_LEVEL);
        }
    }

    /* Test if the source address refers to a valid port (RHEA,API, IMIF)
     * OR if the destination address refers to a valid port
     */
    if((dma_determine_port (channel_info->source_address) == DMA_PORT_ERR) ||
            (dma_determine_port (channel_info->destination_address) == DMA_PORT_ERR))
    {
        return_value = DMA_INVALID_PARAMETER;
        DMA_SEND_TRACE("DMA Problem with the source or dest. adress given:",
                DMA_TRACE_LEVEL);
        DMA_SEND_TRACE("DMA For instance: Adress out of range",
                DMA_TRACE_LEVEL);
    }

    return return_value;
}


void dma_internal_reserve_chan (T_RV_HDR *msg_p)
{
    T_DMA_CHANNEL dma_channel   = DMA_ALL_CHAN_FREE;
    T_DMA_RET     dma_status    = DMA_RESERVE_OK;
    BOOL          dma_queue_free;

    /* Create a pointer to the message information */
    T_DMA_RESERVE_CHANNEL_REQ_MSG* dma_reserve_channel_req_msg_p =
        (T_DMA_RESERVE_CHANNEL_REQ_MSG*) msg_p;


    /* Check if a specific or any dma channel is required */
    switch (dma_reserve_channel_req_msg_p->specific)
    {
        case DMA_CHAN_ANY:
            {
                /*
                   try to get a free channel
                   */
                dma_channel = DMA_MIN_CHANNEL;

                while (dma_channel_array[dma_channel].specific != DMA_CHANNEL_POS_FREE)
                {
                    dma_channel ++;
                    if (dma_channel == DMA_MAX_NUMBER_OF_CHANNEL)
                    {
                        break;
                    }
                }

                if (dma_channel < DMA_MAX_NUMBER_OF_CHANNEL)
                {
                    /*
                       Channel is free
                       Occupy the channel and store the information needed
                       */

                    dma_channel_array [dma_channel].specific    =
                        dma_reserve_channel_req_msg_p->specific;
                    dma_channel_array [dma_channel].channel     = dma_channel;
                    dma_channel_array [dma_channel].return_path =
                        dma_reserve_channel_req_msg_p->return_path;

                    dma_function_status [dma_channel] = DMA_FUNCTION_RESERVED;
                    dma_status                        = DMA_RESERVE_OK;

                    DMA_SEND_TRACE_PARAM("DMA reservated channel ", dma_channel, \
                            DMA_TRACE_LEVEL);

                    /* break out of the case switch */
                    break;
                }
                else
                {
                    /* There was no free channel available
                       In the next stage, it might be queued
                       */
                    dma_reserve_channel_req_msg_p->channel = DMA_MIN_CHANNEL;
                }

            }

            /*
               There was no free channel available. In the case above there is no
               break. We continue now as if a used specific channel is needed,
               which might be queued. This saves code.
               */

        case DMA_CHAN_SPECIFIC:
            {
                if (dma_channel_array[dma_reserve_channel_req_msg_p->channel].specific
                        != DMA_CHANNEL_POS_FREE)
                {
                    /* Channel is used, check if user wants it to be queued */
                    if (dma_reserve_channel_req_msg_p->queue == DMA_QUEUE_ENABLE)
                    {
                        /*
                           Channel is not free, but user wants to have it queued.
                           Find a free queue place and store the information.
                           */

                        dma_queue_free = FALSE;

                        /* first check if there is a queue place free */
                        for (dma_channel=DMA_ZERO; dma_channel < DMA_MAX_QUEUED_REQUESTS; dma_channel++)
                        {
                            if (dma_queue_array[dma_channel].specific == DMA_QUEUE_POS_FREE)
                            {
                                dma_queue_free = TRUE;
                                break;
                            }
                        }

                        if (dma_queue_free == TRUE)
                        { /* If a place is free in the queue the the request is inserted in
                             the queue based on its priority. This means that the queue is a
                             sorted list. This is done to protect the first-in/first-out
                             principle for similar priory requests.
                             */
                            for (dma_channel = DMA_ZERO; dma_channel <
                                    DMA_MAX_QUEUED_REQUESTS; dma_channel++)
                            {
                                if (dma_reserve_channel_req_msg_p->sw_priority <
                                        dma_queue_array[dma_channel].sw_priority)
                                {
                                    UINT8   index;
                                    /* free up a place by moving the queued request one place to the back */
                                    for (index = DMA_MAX_QUEUED_REQUESTS-1; index > dma_channel; index--)
                                    {
                                        dma_queue_array[index].specific    = dma_queue_array[index-1].specific;
                                        dma_queue_array[index].channel     = dma_queue_array[index-1].channel;
                                        dma_queue_array[index].sw_priority = dma_queue_array[index-1].sw_priority;
                                        dma_queue_array[index].return_path = dma_queue_array[index-1].return_path;
                                        dma_queue_array[index].queue_index = dma_queue_array[index-1].queue_index;
                                    }

                                    /* store the new request */
                                    dma_queue_array[dma_channel].specific    = dma_reserve_channel_req_msg_p->specific;
                                    dma_queue_array[dma_channel].channel     = dma_reserve_channel_req_msg_p->channel;
                                    dma_queue_array[dma_channel].sw_priority = dma_reserve_channel_req_msg_p->sw_priority;
                                    dma_queue_array[dma_channel].return_path = dma_reserve_channel_req_msg_p->return_path;
                                    dma_queue_array[dma_channel].queue_index = dma_env_ctrl_blk_p->dma_queue_index;

                                    dma_env_ctrl_blk_p->dma_queue_index += DMA_QUEUE_INDEX_STEPSIZE;

                                    /* if the queue index becomes too high, set it back. */
                                    if (dma_env_ctrl_blk_p->dma_queue_index == DMA_QUEUE_INDEX_MAX)
                                    {
                                        dma_env_ctrl_blk_p->dma_queue_index = DMA_QUEUE_INDEX_MIN;
                                    }

                                    dma_status = DMA_QUEUED;

                                    DMA_SEND_TRACE_PARAM("DMA Reservation has been queued. Queue index", \
                                            dma_queue_array[dma_channel].queue_index,\
                                            DMA_TRACE_LEVEL);
                                    /* break out of the first FOR loop */
                                    break;
                                }
                            }
                        }
                        else
                        {
                            /* Queue is full. Too bad for the user,
                             * but the request cannot be granted
                             */
                            DMA_SEND_TRACE("DMA Reservation not done as queue is full ",\
                                    DMA_TRACE_LEVEL);

                            dma_status = DMA_TOO_MANY_REQUESTS;
                        }

                    }
                    else
                    {
                        /* Channel not free, and user does not want it do be queued */
                        DMA_SEND_TRACE ("DMA Reservation is not done because no channel is available", \
                                DMA_TRACE_LEVEL);

                        dma_status = DMA_NO_CHANNEL;
                    }
                }
                else
                {
                    /* A specific channel is required, and it is free */

                    /* Reserve channel */
                    dma_channel_array[dma_reserve_channel_req_msg_p->channel].specific    = dma_reserve_channel_req_msg_p->specific;
                    dma_channel_array[dma_reserve_channel_req_msg_p->channel].channel     = dma_reserve_channel_req_msg_p->channel;
                    dma_channel_array[dma_reserve_channel_req_msg_p->channel].return_path = dma_reserve_channel_req_msg_p->return_path;
                    dma_function_status[dma_reserve_channel_req_msg_p->channel]           = DMA_FUNCTION_RESERVED;

                    /*
                       store the channel to be reservated. it is used
                       when a message is send back to the client
                       */
                    dma_channel = dma_reserve_channel_req_msg_p->channel;

                    DMA_SEND_TRACE_PARAM("DMA Reservated specific channel ", \
                            dma_reserve_channel_req_msg_p->channel, DMA_TRACE_LEVEL);

                    dma_status = DMA_RESERVE_OK;
                }
            }
            break;
    }


    /* Send a message back to the client, with the status included */
    dma_send_status_message ((T_DMA_CHANNEL)(dma_status == DMA_QUEUED ?
                dma_queue_array[dma_channel].queue_index :
                dma_channel_array[dma_channel].channel), dma_status,
            dma_reserve_channel_req_msg_p->return_path);

}


void dma_internal_remove_from_queue (T_RV_HDR *msg_p)
{
    UINT8 n;

    /* Create a pointer to the message information */
    T_DMA_REMOVE_FROM_QUEUE_REQ_MSG* dma_remove_from_queue_msg_p =
        (T_DMA_REMOVE_FROM_QUEUE_REQ_MSG*) msg_p;

    DMA_SEND_TRACE("DMA Remove from queue ENTERED", DMA_TRACE_LEVEL);

    if ((dma_remove_from_queue_msg_p->channel_queue_id >= DMA_QUEUE_INDEX_MIN) &&
            (dma_remove_from_queue_msg_p->channel_queue_id <= DMA_QUEUE_INDEX_MAX))
    {
        /* determine from the index the corresponding information */
        for (n = DMA_ZERO; n < DMA_MAX_QUEUED_REQUESTS; n++)
        {
            if (dma_queue_array[n].queue_index == dma_remove_from_queue_msg_p->channel_queue_id)
            {
                break;
            }
        }

        if (n == DMA_MAX_QUEUED_REQUESTS)
        {
            /* invalid parameter */
            DMA_SEND_TRACE("DMA Remove from queue: ILLEGAL PARAMETER", DMA_TRACE_LEVEL);
            DMA_SEND_TRACE("DMA Remove from queue: no message send back", DMA_TRACE_LEVEL);
        }
        else
        {
            /* valid parameter found */

            /*
               Send message back to the client.
               */
            dma_send_status_message (dma_queue_array[n].queue_index,
                    DMA_REMOVED_FROM_QUEUE,
                    dma_queue_array[n].return_path);
            DMA_SEND_TRACE_PARAM("DMA removed from queue with index ",
                    dma_queue_array[n].queue_index, DMA_TRACE_LEVEL);

            /* free and shift the queue place */
            for (; n < DMA_MAX_QUEUED_REQUESTS; n++)
            {
                if (n == (DMA_MAX_QUEUED_REQUESTS-1))
                {
                    dma_queue_array[n].specific    = DMA_QUEUE_POS_FREE;
                    dma_queue_array[n].queue_index = DMA_QUEUE_POS_FREE;
                }
                else
                {
                    dma_queue_array[n].specific =
                        dma_queue_array[n+1].specific;

                    dma_queue_array[n].channel =
                        dma_queue_array[n+1].channel;

                    dma_queue_array[n].queue_index =
                        dma_queue_array[n+1].queue_index;

                    dma_queue_array[n].sw_priority =
                        dma_queue_array[n+1].sw_priority;

                    dma_queue_array[n].return_path =
                        dma_queue_array[n+1].return_path;
                }
            }
        }
    }
    else
    {
        DMA_SEND_TRACE("DMA Illegal parameter provided", DMA_TRACE_LEVEL);
        DMA_SEND_TRACE("DMA Nomessage send back", DMA_TRACE_LEVEL);
    }
}


T_DMA_INTERNAL_TYPE_CHANNEL_PORT dma_determine_port (UINT32 address)
{
    /*
       Check are made according to the chance the occur. The bigger
       the chance, the earlier it is done to save time
       */

    /* For PC application IMIF is always returned except when the adress = 0,
       this is used for the tests */
#ifdef _WINDOWS
    if (address == 0) return DMA_PORT_ERR;
    else return DMA_IMIF_PORT;
#endif // _WINDOWS

    /* determine which port to use, based on the address adress */
#if (CHIPSET == 15)
    if ((address >= DMA_IPER_MIN) &&
            (address <= DMA_IPER_MAX))
    {
        return DMA_IPER_PORT;
    }
#endif

    if ((address >= DMA_IMIF_MIN) &&
            (address <= DMA_IMIF_MAX))
    {
        return DMA_IMIF_PORT;
    }

#if (CHIPSET == 15 || CHIPSET == 15 )
    if ((address >= DMA_EMIF_MIN) &&
            (address <= DMA_EMIF_MAX))
    {
        return DMA_EMIF_PORT;
    }
#endif

    if ((address >= DMA_RHEA_MIN1) &&
            (address <= DMA_RHEA_MAX1))
    {
        return DMA_RHEA_PORT;
    }

    if ((address >= DMA_RHEA_MIN2) &&
            (address <= DMA_RHEA_MAX2))
    {
        return DMA_RHEA_PORT;
    }

    if ((address >= DMA_API_MIN1) &&
            (address <= DMA_API_MAX1))
    {
        return DMA_API_PORT;
    }


    if ((address >= DMA_API_MAX2) &&
            (address <= DMA_API_MAX2))
    {
        return DMA_API_PORT;
    }

    /* if this return is reached, a wrong address has been provided */
    return DMA_PORT_ERR;
}



void dma_internal_set_chan_params (T_RV_HDR *msg_p)
{

    /* Create a pointer to the message information */
    T_DMA_SET_CHANNEL_PARAMETERS_REQ_MSG *dma_set_channel_parameters_req_p = (T_DMA_SET_CHANNEL_PARAMETERS_REQ_MSG*) msg_p;

    /* Pointer to the channel info structure of the low level driver */
    T_DMA_TYPE_CHANNEL_PARAMETER dma_ll_channel_info ;

    /* Check if the channel is already reserved */
    if (dma_function_status[dma_set_channel_parameters_req_p->channel] >=
            DMA_FUNCTION_RESERVED)
    {
#ifndef _WINDOWS
        dma_ll_channel_info.pf_dma_call_back_address = dma_call_back_address[dma_set_channel_parameters_req_p->channel];
#endif


        dma_ll_channel_info.d_dma_channel_number    = dma_set_channel_parameters_req_p->channel;

        dma_ll_channel_info.d_dma_channel_secured   = dma_set_channel_parameters_req_p->channel_info.secure;
        dma_ll_channel_info.d_dma_channel_data_type = dma_set_channel_parameters_req_p->channel_info.data_width;

        dma_ll_channel_info.d_dma_src_channel_packed   = dma_set_channel_parameters_req_p->channel_info.source_packet;
        dma_ll_channel_info.d_dma_src_channel_burst_en = dma_set_channel_parameters_req_p->channel_info.source_burst;
        dma_ll_channel_info.d_dma_dst_channel_packed   = dma_set_channel_parameters_req_p->channel_info.destination_packet;
        dma_ll_channel_info.d_dma_dst_channel_burst_en = dma_set_channel_parameters_req_p->channel_info.destination_burst;

        dma_ll_channel_info.d_dma_channel_hw_synch   = dma_set_channel_parameters_req_p->channel_info.sync;
        dma_ll_channel_info.d_dma_channel_priority   = dma_set_channel_parameters_req_p->channel_info.hw_priority;
        dma_ll_channel_info.d_dma_channel_fifo_flush = dma_set_channel_parameters_req_p->channel_info.flush;
        dma_ll_channel_info.d_dma_channel_auto_init  = dma_set_channel_parameters_req_p->channel_info.dma_mode;

        dma_ll_channel_info.d_dma_src_channel_addr_mode = dma_set_channel_parameters_req_p->channel_info.source_address_mode;
        dma_ll_channel_info.d_dma_dst_channel_addr_mode = dma_set_channel_parameters_req_p->channel_info.destination_address_mode;


        dma_ll_channel_info.d_dma_channel_it_time_out   = DMA_TRUE;
        dma_ll_channel_info.d_dma_channel_it_drop       = DMA_TRUE;
        dma_ll_channel_info.d_dma_channel_it_frame      = DMA_ZERO;

        dma_ll_channel_info.d_dma_channel_it_block      = dma_set_channel_parameters_req_p->channel_info.dma_end_notification;

        /* Only use the half block interrupt when double buffering is used */
        if ( dma_channel_array[dma_set_channel_parameters_req_p->channel].
                double_buf_mode != DMA_NO_DOUBLE_BUF )
        {
            dma_ll_channel_info.d_dma_channel_it_half_block = DMA_TRUE;
        }
        else
        {
            dma_ll_channel_info.d_dma_channel_it_half_block = DMA_ZERO;
        }

        dma_ll_channel_info.d_dma_channel_src_address = dma_set_channel_parameters_req_p->channel_info.source_address;
        dma_ll_channel_info.d_dma_channel_dst_address = dma_set_channel_parameters_req_p->channel_info.destination_address;

        dma_ll_channel_info.d_dma_channel_src_port = dma_determine_port (dma_ll_channel_info.d_dma_channel_src_address);
        dma_ll_channel_info.d_dma_channel_dst_port = dma_determine_port (dma_ll_channel_info.d_dma_channel_dst_address);

        dma_ll_channel_info.d_dma_channel_element_number = dma_set_channel_parameters_req_p->channel_info.nmb_elements;
        dma_ll_channel_info.d_dma_channel_frame_number   = dma_set_channel_parameters_req_p->channel_info.nmb_frames;

        /* this parameter is used in the hisr */
        dma_channel_array[dma_set_channel_parameters_req_p->channel].dma_end_notification_bool =
            dma_set_channel_parameters_req_p->channel_info.dma_end_notification;

        /* Store all the information about this channel */
        dma_channel_array[dma_set_channel_parameters_req_p->channel].
            channel_info = dma_ll_channel_info;

        /* set the actual parameters */
        f_dma_channel_parameter_set (&dma_ll_channel_info);

        DMA_SEND_TRACE_PARAM("DMA Parameters set for channel ",
                dma_set_channel_parameters_req_p->channel, DMA_TRACE_LEVEL);

        /* Send a message back to the client, with the status included */
        dma_send_status_message (dma_set_channel_parameters_req_p->channel,
                DMA_PARAM_SET,
                dma_channel_array
                [dma_set_channel_parameters_req_p->channel].return_path);

        /*
           Check if the user wants the transfer to start directly
           or wants to start the transfer itself
           */
        if (dma_set_channel_parameters_req_p->channel_info.transfer ==
                DMA_MODE_TRANSFER_ENABLE)
        {
            f_dma_channel_enable (dma_set_channel_parameters_req_p->channel);

            DMA_SEND_TRACE_PARAM("DMA TRANSFER STARTED, channel ",
                    dma_set_channel_parameters_req_p->channel, DMA_TRACE_LEVEL);
            dma_function_status[dma_set_channel_parameters_req_p->channel] =
                DMA_FUNCTION_CHANNEL_ENABLED;
        }
        else
        {
            dma_function_status[dma_set_channel_parameters_req_p->channel] =
                DMA_FUNCTION_PARAMS_SET_UP;
        }
    }
    else
    {
        DMA_SEND_TRACE("DMA Illegal sequence of API call", DMA_TRACE_LEVEL);
    }
}


void dma_internal_rel_chan (T_DMA_CHANNEL channel)
{
    T_DMA_CHANNEL dma_dummy_var = DMA_ALL_CHAN_FREE;
    //UINT8 dma_highest_prio = 0xFF;
    //UINT8 dma_highest_prio_chan = 0xFF;

    if (dma_function_status[channel] >= DMA_FUNCTION_RESERVED)
    {

        /* disable the DMA transfer for the channel */
        f_dma_channel_disable (channel);

        /* reset the channel to be used */
        f_dma_channel_soft_reset (channel);

        /* send message to the client that his channel is released now */
        dma_send_status_message (channel, DMA_CHANNEL_RELEASED,
                dma_channel_array[channel].return_path);

        DMA_SEND_TRACE_PARAM("DMA released channel ",
                channel, DMA_TRACE_LEVEL);

        /* free the channel in the buffer */
        dma_channel_array[channel].specific = DMA_CHANNEL_POS_FREE;
        /* default, we do not use double buffering */
        dma_channel_array[channel].double_buf_mode = DMA_NO_DOUBLE_BUF;

        /* Get the next request from the queue. */
        while (dma_dummy_var < DMA_MAX_QUEUED_REQUESTS)
        {
            /*
               Now we will search for the next request in the queue.
               The request can be for any free channel or
               it can be a specific request, but then of course, the channel
               requested bust be equal to the channel freed.
               */
            if ((dma_queue_array[dma_dummy_var].specific == DMA_CHAN_ANY) ||

                    ((dma_queue_array[dma_dummy_var].specific == DMA_CHAN_SPECIFIC) &&
                     (dma_queue_array[dma_dummy_var].channel  == channel)))
            {
                /* Break the while loop */
                break;
            }
            dma_dummy_var++;
        }

        if (dma_dummy_var < DMA_MAX_QUEUED_REQUESTS)
        {
            /* A request can be processed */

            dma_channel_array[channel].specific    =
                dma_queue_array[dma_dummy_var].specific;
            dma_channel_array[channel].channel     =
                channel; /* = specific channel or freed
                          *   channel (in both cases equal) */
            dma_channel_array[channel].return_path =
                dma_queue_array[dma_dummy_var].return_path;

            /* Send a message to the original request owner that the channel is free now */
            dma_send_status_message (dma_queue_array[channel].queue_index +
                    dma_channel_array[channel].channel,
                    DMA_QUEUE_PROC,
                    dma_channel_array[channel].return_path);

            /*
               This is the old situation. Introduced a new status message
               dma_send_status_message (dma_channel_array[channel].channel,
               DMA_RESERVE_OK,
               dma_channel_array[channel].return_path);
               */

            /* Shift the queue */
            for (; dma_dummy_var < DMA_MAX_QUEUED_REQUESTS; dma_dummy_var++)
            {
                if (dma_dummy_var == (DMA_MAX_QUEUED_REQUESTS-1))
                {
                    dma_queue_array[dma_dummy_var].specific = DMA_QUEUE_POS_FREE;
                }
                else
                {
                    dma_queue_array[dma_dummy_var].specific =
                        dma_queue_array[dma_dummy_var+1].specific;

                    dma_queue_array[dma_dummy_var].channel =
                        dma_queue_array[dma_dummy_var+1].channel;

                    dma_queue_array[dma_dummy_var].return_path =
                        dma_queue_array[dma_dummy_var+1].return_path;

                    dma_queue_array[dma_dummy_var].queue_index =
                        dma_queue_array[dma_dummy_var+1].queue_index;

                    dma_queue_array[dma_dummy_var].sw_priority =
                        dma_queue_array[dma_dummy_var+1].sw_priority;

                }
            }
        }
    }
    else
    {
        DMA_SEND_TRACE("DMA Illegal sequence of API call", DMA_TRACE_LEVEL);
    }
}

/* Function writes a destination address to HW register */
void write_hw_dest_adress (UINT16 dma_channel_number, UINT32 address)
{
#define DMA_DEBUG
#undef DMA_DEBUG
#ifndef _WINDOWS
    /* Write the lower part of the destination adress */
    C_DMA_CDSA_L_REG(dma_channel_number)=
        (dma_channel_array[dma_channel_number].channel_info.d_dma_channel_dst_address);
    /* Write the higher part of the destination adress */
    C_DMA_CDSA_U_REG(dma_channel_number)=
        (dma_channel_array[dma_channel_number].channel_info.d_dma_channel_dst_address >> 16 );
#endif
#ifdef DMA_DEBUG
    DMA_HISR_SEND_TRACE_PARAM ("DMA HISR Wrote destination adress ",
            dma_channel_array[dma_channel_number].channel_info.d_dma_channel_dst_address,
            DMA_TRACE_LEVEL);
#endif
}

/* Function writes a source address to HW register */
void write_hw_source_adress (UINT16 dma_channel_number, UINT32 address)
{
#define DMA_DEBUG
#undef DMA_DEBUG

#ifndef _WINDOWS
    /* Write the lower part of the source adress */
    C_DMA_CSSA_L_REG(dma_channel_number)=
        (dma_channel_array[dma_channel_number].channel_info.d_dma_channel_src_address);
    /* Write the higher part of the destination adress */
    C_DMA_CSSA_U_REG(dma_channel_number)=
        (dma_channel_array[dma_channel_number].channel_info.d_dma_channel_src_address >> 16 );
#endif
#ifdef DMA_DEBUG
    DMA_HISR_SEND_TRACE_PARAM ("DMA HISR Wrote source adress ",
            dma_channel_array[dma_channel_number].channel_info.d_dma_channel_src_address,
            DMA_TRACE_LEVEL);
#endif
}




#ifndef _WINDOWS
/******************************************************************************
 *
 *  FUNCTION NAME:dma_call_back_it_chan0
 *    The address of that function will be called by the DMA interrupt handler
 *
 *  ARGUMENT LIST:
 *
 *  Argument          Type         IO  Description
 *  ----------        ----------   --  -------------------------------------------
 *  dma_status       SYS_UWORD16    I
 *
 *
 * RETURN VALUE: none
 *
 *****************************************************************************/
void dma_call_back_it_chan0(UINT16 dma_status)
{


    /* If dma is not started, return immediately */
    if (dma_env_ctrl_blk_p == 0)
    {
        return;
    }
    else
    {
        dma_channel_number_tab[total_active_dma_channels] = CHANNEL0;
        dma_it_status_tab[total_active_dma_channels++] = dma_status;
        if (total_active_dma_channels==1)
        {

            if(Activate_DMA_HISR() == FALSE)
            {
                DMA_SEND_TRACE ("DMA HISR activation FAILED", DMA_TRACE_LEVEL);
            }
            else
                F_INTH_DISABLE_ONE_IT(C_INTH_DMA_IT);	   
        }
    }
}


/******************************************************************************
 *
 *  FUNCTION NAME:dma_call_back_it_chan1
 *    The address of that function will be called by the DMA interrupt handler
 *
 *  ARGUMENT LIST:
 *
 *  Argument          Type         IO  Description
 *  ----------        ----------   --  -------------------------------------------
 *  dma_status       SYS_UWORD16    I
 *
 *
 * RETURN VALUE: none
 *
 *****************************************************************************/
void dma_call_back_it_chan1(UINT16 dma_status)
{


    /* If dma is not started, return immediately */
    if (dma_env_ctrl_blk_p == 0)
    {
        return;
    }
    else
    {
        dma_channel_number_tab[total_active_dma_channels] = CHANNEL1;
        dma_it_status_tab[total_active_dma_channels++] = dma_status;
        if (total_active_dma_channels==1)
        {

            if(Activate_DMA_HISR() == FALSE)
            {
                DMA_SEND_TRACE ("DMA HISR activation FAILED", DMA_TRACE_LEVEL);
            }
            else
                F_INTH_DISABLE_ONE_IT(C_INTH_DMA_IT);	   
        }
    }
}

/******************************************************************************
 *
 *  FUNCTION NAME:dma_call_back_it_chan2
 *    The address of that function will be called by the DMA interrupt handler
 *
 *  ARGUMENT LIST:
 *
 *  Argument          Type         IO  Description
 *  ----------        ----------   --  -------------------------------------------
 *  dma_status       SYS_UWORD16    I
 *
 *
 * RETURN VALUE: none
 *
 *****************************************************************************/
void dma_call_back_it_chan2(UINT16 dma_status)
{


    /* If dma is not started, return immediately */
    if (dma_env_ctrl_blk_p == 0)
    {
        return;
    }
    else
    {
        dma_channel_number_tab[total_active_dma_channels] = CHANNEL2;
        dma_it_status_tab[total_active_dma_channels++] = dma_status;
        if (total_active_dma_channels==1)
        {

            if(Activate_DMA_HISR() == FALSE)
            {
                DMA_SEND_TRACE ("DMA HISR activation FAILED", DMA_TRACE_LEVEL);
            }
            else
                F_INTH_DISABLE_ONE_IT(C_INTH_DMA_IT);	   
        }
    }
}

/******************************************************************************
 *
 *  FUNCTION NAME:dma_call_back_it_chan3
 *    The address of that function will be called by the DMA interrupt handler
 *
 *  ARGUMENT LIST:
 *
 *  Argument          Type         IO  Description
 *  ----------        ----------   --  -------------------------------------------
 *  dma_status       SYS_UWORD16    I
 *
 *
 * RETURN VALUE: none
 *
 *****************************************************************************/
//T_CAMD_MSG * camd_dma_msg_p;


int dma_time_current=0;
//int dma_time_current1=0;
//int dma_time_current2=0;
extern T_CAMD_MSG * camd_dequeue();



volatile int flag;
void dma_call_back_it_chan3(UINT16 dma_status)
{


    /* If dma is not started, return immediately */
    if (dma_env_ctrl_blk_p == 0)
    {
        return;
    }
    else
    {

        dma_channel_number_tab[total_active_dma_channels] = CHANNEL3;
        dma_it_status_tab[total_active_dma_channels++] = dma_status;

        if (total_active_dma_channels==1)
        {

            if(Activate_DMA_HISR() == FALSE)
            {
                DMA_SEND_TRACE ("DMA HISR activation FAILED", DMA_TRACE_LEVEL);
            }
            else{

               F_INTH_DISABLE_ONE_IT(C_INTH_DMA_IT);	 
            }



        }


    } 

}
























/******************************************************************************
 *
 *  FUNCTION NAME:dma_call_back_it_chan4
 *    The address of that function will be called by the DMA interrupt handler
 *
 *  ARGUMENT LIST:
 *
 *  Argument          Type         IO  Description
 *  ----------        ----------   --  -------------------------------------------
 *  dma_status       SYS_UWORD16    I
 *
 *
 * RETURN VALUE: none
 *
 *****************************************************************************/
void dma_call_back_it_chan4(UINT16 dma_status)
{


    /* If dma is not started, return immediately */
    if (dma_env_ctrl_blk_p == 0)
    {
        return;
    }
    else
    {
        dma_channel_number_tab[total_active_dma_channels] = CHANNEL4;
        dma_it_status_tab[total_active_dma_channels++] = dma_status;
        if (total_active_dma_channels==1)
        {

            if(Activate_DMA_HISR() == FALSE)
            {
                DMA_SEND_TRACE ("DMA HISR activation FAILED", DMA_TRACE_LEVEL);
            }
            else
                F_INTH_DISABLE_ONE_IT(C_INTH_DMA_IT);	   
        }
    }
}


/******************************************************************************
 *
 *  FUNCTION NAME:dma_call_back_it_chan5
 *    The address of that function will be called by the DMA interrupt handler
 *
 *  ARGUMENT LIST:
 *
 *  Argument          Type         IO  Description
 *  ----------        ----------   --  -------------------------------------------
 *  dma_status       SYS_UWORD16    I
 *
 *
 * RETURN VALUE: none
 *
 *****************************************************************************/
void dma_call_back_it_chan5(UINT16 dma_status)
{


    /* If dma is not started, return immediately */
    if (dma_env_ctrl_blk_p == 0)
    {
        return;
    }
    else
    {
        dma_channel_number_tab[total_active_dma_channels] = CHANNEL5;
        dma_it_status_tab[total_active_dma_channels++] = dma_status;
        if (total_active_dma_channels==1)
        {

            if(Activate_DMA_HISR() == FALSE)
            {
                DMA_SEND_TRACE ("DMA HISR activation FAILED", DMA_TRACE_LEVEL);
            }
            else
                F_INTH_DISABLE_ONE_IT(C_INTH_DMA_IT);	   
        }
    }
}

#endif
