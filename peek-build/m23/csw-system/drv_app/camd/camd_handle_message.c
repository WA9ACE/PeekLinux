#define AGILENT 0
#define MICRON 1

#if (CAM_SENSOR==AGILENT)
/**
 * @file	camd_handle_message.c
 *
 * CAMD handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author	raymond zandbergen ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/12/2003	raymond zandbergen ()		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */


#include <string.h>
#include "rvf/rvf_api.h"
#include "camd/camd_api.h"
#include "camd/camd_i.h"
#include "camd/camd_operations.h"

int cam_time1=0;
int cam_time2=0;


    T_RVM_RETURN
camd_send_response_to_client (UINT32 msg_id, T_CAMD_MSG * msg_p)
{
    if (msg_p->status != CAMD_OK && msg_p->status != CAMD_TRANSFER_COMPLETE)
    {
        CAMD_SEND_TRACE ("CAMD CORE returning error status",
                RV_TRACE_LEVEL_ERROR);
    }
    /* follow return path: callback or mailbox */
    msg_p->os_hdr.msg_id = msg_id;
    if (msg_p->rp.callback_func != NULL)
    {
        CAMD_API_TRACE_PARAM ("CAMD CORE callback, msg ID =",
                msg_p->os_hdr.msg_id);
        CAMD_API_TRACE_PARAM ("CAMD CORE callback, cb addr =",
                msg_p->rp.callback_func);
        msg_p->rp.callback_func (msg_p);
    }
    else
    {
        CAMD_API_TRACE_PARAM ("CAMD CORE posting response, msg ID =",
                msg_p->os_hdr.msg_id);
        CAMD_API_TRACE_PARAM ("CAMD CORE posting response, addr ID =",
                msg_p->rp.addr_id);
        if (rvf_send_msg (msg_p->rp.addr_id, msg_p) != RVF_OK)
        {
            CAMD_SEND_TRACE ("CAMD CORE could not send response msg",
                    RV_TRACE_LEVEL_ERROR);
            return RVM_MEMORY_ERR;
        };
    }

    cam_time2=cam_time1;
    cam_time1=rvf_get_tick_count();

    CAMD_SEND_TRACE_PARAM ("time",(cam_time1-cam_time2),
            RV_TRACE_LEVEL_DEBUG_HIGH);


    return RVM_OK;
}






/* store message in return queue, raise an event */
    static void
return_queue_callback (void *msg_p)
{

    /*CAMD_SEND_TRACE ("CAMD RETURN QUEUE return que callback begin",
      RV_TRACE_LEVEL_DEBUG_HIGH);

      CAMD_SEND_TRACE_PARAM ("CAMD RETURN QUEUE addr_id",
      camd_env_ctrl_blk_p->return_queue.addr_id,
      RV_TRACE_LEVEL_DEBUG_HIGH); */

    CAMD_SEND_TRACE ("Camd rq ",
            RV_TRACE_LEVEL_DEBUG_HIGH);



    if((camd_env_ctrl_blk_p->capturemode==TRUE))
    {

        CAMD_SEND_TRACE ("DMA msg snap", RV_TRACE_LEVEL_ERROR); 

    }

    else   if((camd_env_ctrl_blk_p->camd_flag ==1)&&(camd_env_ctrl_blk_p->capturemode==FALSE))
    {
        camd_env_ctrl_blk_p->Camd_App_buffer->status=CAMD_TRANSFER_COMPLETE;
        
        CAMD_SEND_TRACE_PARAM ("D MSL",(UINT32)camd_env_ctrl_blk_p->Camd_App_buffer->body.buff, RV_TRACE_LEVEL_DEBUG_HIGH);
        CAMD_SEND_TRACE_PARAM ("DMA act buf",(UINT32)camd_env_ctrl_blk_p->Camd_dma_buffer->body.buff, RV_TRACE_LEVEL_DEBUG_HIGH);				   
        camd_send_response_to_client (CAMD_VIEWFINDER_DATA_RSP_MSG, camd_env_ctrl_blk_p->Camd_App_buffer);
        camd_env_ctrl_blk_p->camd_flag =0;
        rvf_free_buf(msg_p);
        return;				
    }


    if (RVF_OK !=
            rvf_enqueue (&camd_env_ctrl_blk_p->return_queue.queue_obj, msg_p)
            || RVF_OK != rvf_send_event (camd_env_ctrl_blk_p->return_queue.addr_id,
                camd_env_ctrl_blk_p->return_queue.event))
    {
        CAMD_SEND_TRACE ("CAMD RETURN QUEUE lost message",
                RV_TRACE_LEVEL_ERROR);
    }
    else
    {

        CAMD_SEND_TRACE_PARAM ("enqueue",camd_env_ctrl_blk_p->capturemode, RV_TRACE_LEVEL_DEBUG_HIGH);
    }

    /*
       CAMD_SEND_TRACE_PARAM ("CAMD RETURN QUEUE return_queue.event",
       camd_env_ctrl_blk_p->return_queue.event,
       RV_TRACE_LEVEL_DEBUG_HIGH);


       CAMD_SEND_TRACE ("CAMD RETURN QUEUE return que callback end",
       RV_TRACE_LEVEL_DEBUG_HIGH);
       */



}

    void
camd_return_queue_flush (void)
{


    while (camd_env_ctrl_blk_p->return_queue.queue_obj.count > 0)
    {
        rvf_wait (camd_env_ctrl_blk_p->return_queue.event, 1);
        rvf_free_buf (rvf_dequeue
                (&camd_env_ctrl_blk_p->return_queue.queue_obj));
    }
}

/* get a message from the return queue */
    void *
camd_return_queue_get_msg (void)
{

    UINT16 event;

    /*Wait infinitely for a message*/ 
    rvf_wait (camd_env_ctrl_blk_p->return_queue.event, 0);

    return rvf_dequeue (&camd_env_ctrl_blk_p->return_queue.queue_obj);
}

/* init return queue object */
    void
camd_return_queue_init (T_RVF_G_ADDR_ID rq_addr_id, UINT16 rq_event,
        T_RV_RETURN_PATH * path_to_return_queue_p)
{
    camd_env_ctrl_blk_p->return_queue.addr_id = rq_addr_id;
    camd_env_ctrl_blk_p->return_queue.event = rq_event;
    camd_env_ctrl_blk_p->return_queue.queue_obj.p_first = NULL;
    camd_env_ctrl_blk_p->return_queue.queue_obj.p_last = NULL;
    camd_env_ctrl_blk_p->return_queue.queue_obj.count = 0;
    path_to_return_queue_p->addr_id = RVF_INVALID_ADDR_ID;
    path_to_return_queue_p->callback_func = return_queue_callback;
    CAMD_SEND_TRACE_PARAM ("CAMD RETURN QUEUE callback =",
            (UINT32) path_to_return_queue_p->callback_func,RV_TRACE_LEVEL_DEBUG_HIGH);
}

static T_RVF_BUFFER_Q camd_req_queue;

    T_RVF_RET
camd_queue_request (T_CAMD_MSG * msg_p)
{
    return rvf_enqueue (&camd_req_queue, msg_p);
}

/**
 * Called every time the SW entity is in WAITING state 
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param	msg_p	Pointer on the header of the message.
 * @return	RVM_OK or RVM_MEMORY_ERR.
 */




extern UINT8 Overflow_flag;
extern UINT8 Underflow_flag;

T_RVM_RETURN

camd_handle_message (T_RV_HDR * rv_hdr_p)
{
    T_CAMD_MSG *msg_p = (T_CAMD_MSG *) rv_hdr_p;
    T_RVM_RETURN ret_val = RVM_OK;

    camd_req_queue.p_first = NULL;
    camd_req_queue.p_last = NULL;
    camd_req_queue.count = 0;
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
    if (msg_p == NULL)
    {
        CAMD_SEND_TRACE ("CAMD CORE ignoring null message",
                RV_TRACE_LEVEL_ERROR);
        return ret_val;
    }
    do
    {
        /* accepting requests from clients */

        switch (msg_p->os_hdr.msg_id)
        {
            case CAMD_REGISTERCLIENT_REQ_MSG:

                ret_val = camd_int_enable_sensor (msg_p);
                break;

            case CAMD_SET_CONFIGPARAMS_REQ_MSG:

                camd_env_ctrl_blk_p->capturemode =msg_p->body.configparams.capturemode;
                if(msg_p->body.configparams.capturemode==TRUE)
                {	

                    ret_val = camd_int_set_snapshot_parameters (msg_p);

                    break;
                }
                else if(msg_p->body.configparams.capturemode==FALSE)
                {      
                    ret_val = camd_int_set_viewfinder_parameters (msg_p);
                    camd_env_ctrl_blk_p->start_viewfinder=TRUE;

                    break;
                }


            case CAMD_GET_CONFIGPARAMS_REQ_MSG:			

                if(camd_env_ctrl_blk_p->capturemode ==TRUE)
                {
                    ret_val = camd_int_get_snapshot_parameters (msg_p);
                    break;
                }
                else if(camd_env_ctrl_blk_p->capturemode ==FALSE)
                {
                    ret_val = camd_int_get_viewfinder_parameters (msg_p);
                    break;
                }

            case CAMD_START_REQ_MSG:

                ret_val = RVM_OK;
                break;



            case CAMD_USEBUFF_REQ_MSG:

                if(camd_env_ctrl_blk_p->capturemode ==TRUE)
                {	  

                    ret_val = camd_int_get_snapshot (msg_p);
                    break;
                }
                else if(camd_env_ctrl_blk_p->capturemode ==FALSE)
                {

                    ret_val = camd_int_get_viewfinder_frames (msg_p);
                    break;
                }

            case CAMD_PAUSE_REQ_MSG:

                ret_val = RVM_OK;
                break;

            case CAMD_TRANSFER_INTERNAL_DMA:
                {




                }
                break;


            default:
                CAMD_SEND_TRACE_PARAM ("CAMD CORE ignoring unknown message",
                        msg_p->os_hdr.msg_id - CAMD_MESSAGE_OFFSET,
                        RV_TRACE_LEVEL_ERROR);

                if (rvf_free_buf (msg_p) != RVF_OK)
                {
                    CAMD_SEND_TRACE ("CAMD CORE unable to free message",
                            RV_TRACE_LEVEL_ERROR);
                    ret_val = RVM_MEMORY_ERR;
                }
                break;
        }						/* end SWITCH */

        /* stop if any errors OR no queued requests */
        if (ret_val != RVM_OK || camd_req_queue.count == 0)
        {
            break;
        }
        CAMD_SEND_TRACE ("CAMD CORE handling queued request",
                RV_TRACE_LEVEL_DEBUG_HIGH);
        msg_p = rvf_dequeue (&camd_req_queue);
    }
    while (msg_p != NULL);
    return ret_val;
}





void camd_send_internal_message()

{






}

#else
/**
 * @file	camd_handle_message.c
 *
 * CAMD handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
 *
 * @author	raymond zandbergen ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/12/2003	raymond zandbergen ()		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */


#include <string.h>
#include "rvf/rvf_api.h"
#include "camd/camd_api.h"
#include "camd/camd_i.h"
#include "camd/camd_operations.h"



int cam_time1=0;
int cam_time2=0;





    T_RVM_RETURN
camd_send_response_to_client (UINT32 msg_id, T_CAMD_MSG * msg_p)
{
    if (msg_p->status != CAMD_OK && msg_p->status != CAMD_TRANSFER_COMPLETE)
    {
        CAMD_SEND_TRACE ("CAMD CORE returning error status",
                RV_TRACE_LEVEL_ERROR);
    }
    /* follow return path: callback or mailbox */
    msg_p->os_hdr.msg_id = msg_id;
    if (msg_p->rp.callback_func != NULL)
    {
        CAMD_API_TRACE_PARAM ("CAMD CORE callback, msg ID =",
                msg_p->os_hdr.msg_id);
        CAMD_API_TRACE_PARAM ("CAMD CORE callback, cb addr =",
                msg_p->rp.callback_func);
        msg_p->rp.callback_func (msg_p);
    }
    else
    {
        CAMD_API_TRACE_PARAM ("CAMD CORE posting response, msg ID =",
                msg_p->os_hdr.msg_id);
        CAMD_API_TRACE_PARAM ("CAMD CORE posting response, addr ID =",
                msg_p->rp.addr_id);
        if (rvf_send_msg (msg_p->rp.addr_id, msg_p) != RVF_OK)
        {
            CAMD_SEND_TRACE ("CAMD CORE could not send response msg",
                    RV_TRACE_LEVEL_ERROR);
            return RVM_MEMORY_ERR;
        };
    }

    cam_time2=cam_time1;
    cam_time1=rvf_get_tick_count();

    CAMD_SEND_TRACE_PARAM ("time",(cam_time1-cam_time2),
            RV_TRACE_LEVEL_ERROR);


    return RVM_OK;
}



/*extern volatile int snap;*/


/* store message in return queue, raise an event */
    static void
return_queue_callback (void *msg_p)
{

    /*CAMD_SEND_TRACE ("CAMD RETURN QUEUE return que callback begin",
      RV_TRACE_LEVEL_DEBUG_HIGH);

      CAMD_SEND_TRACE_PARAM ("CAMD RETURN QUEUE addr_id",
      camd_env_ctrl_blk_p->return_queue.addr_id,
      RV_TRACE_LEVEL_DEBUG_HIGH); */



    /*if(snap==1);
      {
      snap=0;
      }*/
    if((camd_env_ctrl_blk_p->capturemode==TRUE))
    {

        CAMD_SEND_TRACE ("DMA msg snap", RV_TRACE_LEVEL_ERROR); 


    }

    else   if((camd_env_ctrl_blk_p->camd_flag ==1)&&(camd_env_ctrl_blk_p->capturemode==FALSE))
    {

        /*  Useful traces when debugging , but not necessary in system build
         *
         *  CAMD_SEND_TRACE_PARAM ("D MSL",(UINT32)camd_env_ctrl_blk_p->Camd_App_buffer->body.buff, RV_TRACE_LEVEL_DEBUG_HIGH);*/
        /*CAMD_SEND_TRACE_PARAM ("DMA act buf",(UINT32)camd_env_ctrl_blk_p->Camd_dma_buffer->body.buff, RV_TRACE_LEVEL_DEBUG_HIGH);*/				   

        camd_env_ctrl_blk_p->Camd_App_buffer->status=CAMD_TRANSFER_COMPLETE;

        camd_send_response_to_client (CAMD_VIEWFINDER_DATA_RSP_MSG, camd_env_ctrl_blk_p->Camd_App_buffer);
        camd_env_ctrl_blk_p->camd_flag =0;
        rvf_free_buf(msg_p);
        return;				
    }


    if (RVF_OK !=
            rvf_enqueue (&camd_env_ctrl_blk_p->return_queue.queue_obj, msg_p)
            || RVF_OK != rvf_send_event (camd_env_ctrl_blk_p->return_queue.addr_id,
                camd_env_ctrl_blk_p->return_queue.event))
    {
        CAMD_SEND_TRACE ("CAMD RETURN QUEUE lost message",
                RV_TRACE_LEVEL_ERROR);
    }
    else
    {

        /*CAMD_SEND_TRACE_PARAM ("enqueue",camd_env_ctrl_blk_p->capturemode, RV_TRACE_LEVEL_DEBUG_HIGH);*/
    }

    /*
       CAMD_SEND_TRACE_PARAM ("CAMD RETURN QUEUE return_queue.event",
       camd_env_ctrl_blk_p->return_queue.event,
       RV_TRACE_LEVEL_DEBUG_HIGH);


       CAMD_SEND_TRACE ("CAMD RETURN QUEUE return que callback end",
       RV_TRACE_LEVEL_DEBUG_HIGH);
       */



}

    void
camd_return_queue_flush (void)
{


    while (camd_env_ctrl_blk_p->return_queue.queue_obj.count > 0)
    {
        rvf_wait (camd_env_ctrl_blk_p->return_queue.event, 1);
        rvf_free_buf (rvf_dequeue
                (&camd_env_ctrl_blk_p->return_queue.queue_obj));
    }
}

/* get a message from the return queue */
    void *
camd_return_queue_get_msg (void)
{

    UINT16 event;

    /*CAMD_SEND_TRACE ("CAMD RETURN QUEUE que begin", RV_TRACE_LEVEL_DEBUG_HIGH);*/


    /* no messages queued, block until msg available */
    /* event = */

    rvf_wait (camd_env_ctrl_blk_p->return_queue.event, 0);

    return rvf_dequeue (&camd_env_ctrl_blk_p->return_queue.queue_obj);
}

/* init return queue object */
    void
camd_return_queue_init (T_RVF_G_ADDR_ID rq_addr_id, UINT16 rq_event,
        T_RV_RETURN_PATH * path_to_return_queue_p)
{
    camd_env_ctrl_blk_p->return_queue.addr_id = rq_addr_id;
    camd_env_ctrl_blk_p->return_queue.event = rq_event;
    camd_env_ctrl_blk_p->return_queue.queue_obj.p_first = NULL;
    camd_env_ctrl_blk_p->return_queue.queue_obj.p_last = NULL;
    camd_env_ctrl_blk_p->return_queue.queue_obj.count = 0;
    path_to_return_queue_p->addr_id = RVF_INVALID_ADDR_ID;
    path_to_return_queue_p->callback_func = return_queue_callback;
    CAMD_SEND_TRACE_PARAM ("CAMD RETURN QUEUE callback =",
            (UINT32) path_to_return_queue_p->callback_func,RV_TRACE_LEVEL_DEBUG_HIGH);
}

static T_RVF_BUFFER_Q camd_req_queue;

    T_RVF_RET
camd_queue_request (T_CAMD_MSG * msg_p)
{
    return rvf_enqueue (&camd_req_queue, msg_p);
}

/**
 * Called every time the SW entity is in WAITING state 
 * and get a new message in its mailbox.
 *
 * The message in parameter is freed in this function.
 *
 * @param	msg_p	Pointer on the header of the message.
 * @return	RVM_OK or RVM_MEMORY_ERR.
 */




extern UINT8 Overflow_flag;
extern UINT8 Underflow_flag;

T_RVM_RETURN

camd_handle_message (T_RV_HDR * rv_hdr_p)
{
    T_CAMD_MSG *msg_p = (T_CAMD_MSG *) rv_hdr_p;
    T_RVM_RETURN ret_val = RVM_OK;



    camd_trace_error();

    if (msg_p == NULL)
    {
        CAMD_SEND_TRACE ("CAMD CORE ignoring null message",
                RV_TRACE_LEVEL_ERROR);
        return ret_val;
    }
    do
    {
        /* accepting requests from clients */

        switch (msg_p->os_hdr.msg_id)
        {
            case CAMD_REGISTERCLIENT_REQ_MSG:

                ret_val = camd_int_enable_sensor (msg_p);
                break;

            case CAMD_SET_CONFIGPARAMS_REQ_MSG:

                camd_env_ctrl_blk_p->capturemode =msg_p->body.configparams.capturemode;
                if(msg_p->body.configparams.capturemode==TRUE)
                {	
                    ret_val = camd_int_set_snapshot_parameters (msg_p);

                    break;
                }
                else if(msg_p->body.configparams.capturemode==FALSE)
                {     

                    ret_val = camd_int_set_viewfinder_parameters (msg_p);
                    camd_env_ctrl_blk_p->start_viewfinder=TRUE;

                    break;
                }


            case CAMD_GET_CONFIGPARAMS_REQ_MSG:			

                if(camd_env_ctrl_blk_p->capturemode ==TRUE)
                {
                    ret_val = camd_int_get_snapshot_parameters (msg_p);
                    break;
                }
                else if(camd_env_ctrl_blk_p->capturemode ==FALSE)
                {
                    ret_val = camd_int_get_viewfinder_parameters (msg_p);
                    break;
                }

            case CAMD_START_REQ_MSG:
                ret_val = RVM_OK;
                break;



            case CAMD_USEBUFF_REQ_MSG:

                if(camd_env_ctrl_blk_p->capturemode ==TRUE)
                {	  
                    ret_val = camd_int_get_snapshot (msg_p);
                    break;
                }
                else if(camd_env_ctrl_blk_p->capturemode ==FALSE)
                {

                    ret_val = camd_int_get_viewfinder_frames (msg_p);
                    break;
                }

            case CAMD_PAUSE_REQ_MSG:

                ret_val = RVM_OK;
                break;

            case CAMD_TRANSFER_INTERNAL_DMA:
                {




                }
                break;


            default:
                CAMD_SEND_TRACE_PARAM ("CAMD CORE ignoring unknown message",
                        msg_p->os_hdr.msg_id - CAMD_MESSAGE_OFFSET,
                        RV_TRACE_LEVEL_ERROR);

                if (rvf_free_buf (msg_p) != RVF_OK)
                {
                    CAMD_SEND_TRACE ("CAMD CORE unable to free message",
                            RV_TRACE_LEVEL_ERROR);
                    ret_val = RVM_MEMORY_ERR;
                }
                break;
        }						/* end SWITCH */

        /* stop if any errors OR no queued requests */
        if (ret_val != RVM_OK || camd_req_queue.count == 0)
        {
            break;
        }
        CAMD_SEND_TRACE ("CAMD CORE handling queued request",
                RV_TRACE_LEVEL_DEBUG_HIGH);
        msg_p = rvf_dequeue (&camd_req_queue);
    }
    while (msg_p != NULL);
    return ret_val;
}





void camd_send_internal_message()

{






}



#endif
