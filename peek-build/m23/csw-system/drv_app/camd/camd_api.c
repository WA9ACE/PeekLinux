
/**
 * @file	camd_api.c
 *
 * API for CAMD SWE.
 *
 * @author
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/12/2003	raymond zandbergen (ICT)		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "camd/camd_i.h"
#include "camd/camd_env.h"
#include "camd/camcore/Camcore_api.h "

#ifdef CAMD_ENABLE_TRACE
#ifndef _WINDOWS
#include "inth/iq.h"
#include "Nucleus.h"
#include "sys_dma.h"
#include "dma/board/dma_inth.h"

#define AGILENT 0
#define MICRON 1



extern T_DMA_CHANNEL camd_available_dma_channel;

#endif
#endif
/* BENCHMARKING BENCHMARKING BENCHMARKING BENCHMARKING */
/*                     Framerate.                      */

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_get_request_ms                                      */
/*                                                                              */
/*    Purpose:     Allocates a request message with checks and fills in         */
/*                 some general values that are the same for all bridge         */
/*                 functions.                                                   */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - rp_p : return path                                             */
/*             - msg_pp : address of a message pointer                          */
/*             - msg_id : the request message id                                */
/*                                                                              */
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    static T_RV_RET
camd_get_request_msg (T_RV_RETURN * rp_p, T_CAMD_MSG ** msg_pp, UINT32 msg_id)
{
    /* CAMD task ready & initialised? */
    if (camd_env_ctrl_blk_p == NULL
            || camd_env_ctrl_blk_p->initialised == FALSE)
    {
        CAMD_SEND_TRACE ("camd_get_request_msg: CAMD is not running",
                RV_TRACE_LEVEL_ERROR);
        return RV_NOT_READY;
    }
    /* reserve message buffer */
    if (rvf_get_msg_buf (camd_env_ctrl_blk_p->prim_mb_id, sizeof (T_CAMD_MSG),
                msg_id, (T_RV_HDR **) msg_pp) == RVF_RED)
    {
        CAMD_SEND_TRACE ("camd_get_request_msg: out of memory",
                RV_TRACE_LEVEL_ERROR);
        return RV_MEMORY_ERR;
    }
    /* compose message */
    (*msg_pp)->rp.addr_id = rp_p->addr_id;
    (*msg_pp)->rp.callback_func = rp_p->callback_func;
    return RV_OK;
}

/* ---------------------- non-bridge functions ---------------------- */

/********************************************************************************/
/*                                                                              */
/*    Function Name:   cama_get_sw_version				                          */
/*                                                                              */
/*    Purpose:     Returns software version							*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             None.															*/
/*    Return :                                                                  */
/*             Software version.							                  */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
    UINT32
camd_get_sw_version (void)
{
    T_RVM_INFO_SWE swe_info;
    CAMD_API_TRACE ("CAMD API entering camd_get_sw_version (non-bridge)");
    camd_get_info (&swe_info);
    return swe_info.type_info.type3.version;
}


/* ---------------------- bridge functions ---------------------- */

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_enable_sensor                                       */
/*                                                                              */
/*    Purpose:  This function switches the sensor between the enabled  and		*/
/* 				disabled state.It is disabled by default. When switching to the */
/* 				enabled state, the driver will perform the necessary hardware 	*/
/*				initialisations. The camera must be enabled before any other	*/ 
/* 			API entries (except camd_get_sw_version()) may be used.				*/
/*                                                              				*/
/*    Input Parameters:                                                         */
/*             - enable_camera : a boolean value                             	*/
/*             - rp : return path                                             	*/
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
camd_registerclient (BOOL enable_sensor, T_RV_RETURN rp)
{
    T_CAMD_MSG *msg_p;
    T_RV_RET ret_val;

    CAMD_API_TRACE ("CAMD API entering camd_registerclient(bridge)");
    ret_val = camd_get_request_msg (&rp, &msg_p, CAMD_REGISTERCLIENT_REQ_MSG);
    if (ret_val == RV_OK)
    {
        msg_p->body.enable_sensor = enable_sensor;
        ret_val = rvf_send_msg (camd_env_ctrl_blk_p->addr_id, msg_p);
    }
    return ret_val;
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_set_snapshot_parameters                           	*/
/*                                                                              */
/*    Purpose:     This function sets all snapshot parameters. 	  				*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - param_p:T_CAMD_VIEWFINDER_PARAMETERS                          	*/
/*             - rp : return path                                             	*/
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
camd_set_configparams (T_CAMD_PARAMETERS * param_p,
        T_RV_RETURN rp)
{
    T_CAMD_MSG *msg_p;
    T_RV_RET ret_val;

    CAMD_API_TRACE ("CAMD API entering camd_set_configparams (bridge)");
    ret_val =
        camd_get_request_msg (&rp, &msg_p, CAMD_SET_CONFIGPARAMS_REQ_MSG);
    if (ret_val == RV_OK)
    {
        msg_p->body.configparams = *param_p;
        ret_val = rvf_send_msg (camd_env_ctrl_blk_p->addr_id, msg_p);
    }
    return ret_val;
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_get_snapshot_parameters                           	*/
/*                                                                              */
/*    Purpose:     This function retrieves all snapshot parameters currently	*/ 
/*				 in use												          */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - rp : return path                                             	*/
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
camd_get_configparams (T_RV_RETURN rp)
{
    T_CAMD_MSG *msg_p;
    T_RV_RET ret_val;

    CAMD_API_TRACE ("CAMD API entering camd_get_configparams (bridge)");
    ret_val =
        camd_get_request_msg (&rp, &msg_p, CAMD_GET_CONFIGPARAMS_REQ_MSG);
    if (ret_val == RV_OK)
    {
        ret_val = rvf_send_msg (camd_env_ctrl_blk_p->addr_id, msg_p);
    }
    return ret_val;
}


/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_usebuff		   				                        */
/*                                                                              */
/*    Purpose:     This function instructs CAMD to take a frame and send back the buffer.		*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - rp : return path                                             	*/
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/

/*Forward declaration */

void camd_flush(void);

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_stopviewfinder 				                        */
/*                                                                              */
/*    Purpose:     This function instructs the camera driver to stop            */   
/*    capturing data from the sensor , this should be called whena              */
/*    menu item is activated or whenever                                        */
/*    the state is changes from viewfinder to another state             		*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - rp : NONE                                                   	*/
/*    Return :                                                                  */
/*             - RV_OK : success                                                */
/*             - RV_NOT_READY : CAMD task not ready                             */
/*             - RV_MEMORY_ERR : no memory                                      */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/

T_RV_RET camd_stopviewfinder()
{

    /*stop sensor*/
#if(CAM_SENSOR==AGILENT)
    camd_stop_sensor();
#endif
    /*camd_stop_sensor_clock();*/

    camcore_disable();
    /*stop DMA*/
    f_dma_channel_disable(camd_available_dma_channel);



    /*Camera Fifio clear*/
    CameraCore_Clear_FIFO();
    /*camcore_reset(CAMCORE_RESET_FSM);*/
    CAMD_SEND_TRACE ("stopVF",
            RV_TRACE_LEVEL_ERROR);

    /*Flush buffers*/
    camd_flush();
    camd_env_ctrl_blk_p->start_viewfinder=TRUE;
    return RV_OK;


}
/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_usebuff		   				                        */
/*                                                                              */
/*    Purpose:     This function instructs CAMD to take a frame and send back the buffer.		*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - rp : return path                                             	*/
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
camd_usebuff (UINT8 *buff, T_RV_RETURN rp)
{

    T_CAMD_MSG *msg_p;
    T_RV_RET ret_val;


    /*  CAMD_API_TRACE ("CAMD API entering camd_usebuff (bridge)");*/

    CAMD_SEND_TRACE_PARAM ("buffer",buff,RV_TRACE_LEVEL_DEBUG_HIGH);

    ret_val = camd_get_request_msg (&rp, &msg_p, CAMD_USEBUFF_REQ_MSG);
    if (ret_val == RV_OK)
    {
        msg_p->body.buff = buff;

        if (camd_env_ctrl_blk_p->start_viewfinder==TRUE||camd_env_ctrl_blk_p->capturemode==TRUE)
        {
            /*irrespective of wether snapshot or viewfinder*/
            CAMD_SEND_TRACE_PARAM ("first",msg_p->body.buff,
                    RV_TRACE_LEVEL_DEBUG_HIGH );

            ret_val = rvf_send_msg (camd_env_ctrl_blk_p->addr_id, msg_p);
            camd_env_ctrl_blk_p->Camd_App_buffer=NULL;
            camd_env_ctrl_blk_p->Camd_dma_buffer=msg_p;

            camd_env_ctrl_blk_p->start_viewfinder=FALSE;	
        }
        else if(camd_env_ctrl_blk_p->start_viewfinder==FALSE && camd_env_ctrl_blk_p->capturemode==FALSE)
        {


            camd_enqueue(msg_p);

            /*ret_val = rvf_enqueue (&camd_env_ctrl_blk_p->return_queue.queue_obj, msg_p);*/
            CAMD_SEND_TRACE_PARAM ("seco",msg_p->body.buff,
                    RV_TRACE_LEVEL_DEBUG_HIGH);

        }
        else
        {  
            ret_val=RVF_INTERNAL_ERR;
            CAMD_SEND_TRACE("This should not happen", RV_TRACE_LEVEL_ERROR);

        }

    }
    return ret_val;
}




extern CAM_SENSOR_CAPABILITIES cam_current_sensor_capabilities;
T_RV_RET camd_get_sensor_capabilities(CAM_SENSOR_CAPABILITIES * sensor_capabilities)
{

    sensor_capabilities->zoom_support=
        cam_current_sensor_capabilities.zoom_support ; 
    sensor_capabilities->mirror_support=cam_current_sensor_capabilities.mirror_support ;
    return RV_OK;       

}


T_RV_RET camd_zoom(T_CAMD_ZOOM zoom)

{

#if(CAM_SENSOR==AGILENT)
    return RV_NOT_READY;
#else


    if (!camd_env_ctrl_blk_p->sensor_enabled)
    {
        return CAMD_NOT_READY;
    }

    else

    {

        camd_set_zoom(zoom);
        return RV_OK;

    } 

#endif
}





/*****************NOTE****************************************************
 *   The API's Listed down here are not to be used by external application
 *    it is for Further use and some are for internal use.
 *************************************************************************
 */







/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_start		   					                        */
/*                                                                              */
/*    Purpose:     This function instructs CAMD to start for capture.							*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - rp : return path                                             	*/
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
camd_pause (T_RV_RETURN rp)
{
    T_CAMD_MSG *msg_p;
    T_RV_RET ret_val;

    CAMD_API_TRACE ("CAMD API entering camd_pause (bridge)");
    ret_val = camd_get_request_msg (&rp, &msg_p, CAMD_PAUSE_REQ_MSG);
    if (ret_val == RV_OK)
    {
        ret_val = rvf_send_msg (camd_env_ctrl_blk_p->addr_id, msg_p);
    }
    return ret_val;
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camd_start		   					                    */
/*                                                                              */
/*    Purpose:     This function instructs CAMD to start for capture.	    	*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - rp : return path                                             	*/
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
camd_start_capture(T_RV_RETURN rp)
{
    T_CAMD_MSG *msg_p;
    T_RV_RET ret_val;

    CAMD_API_TRACE ("CAMD API entering camd_start_capture(bridge)");
    ret_val = camd_get_request_msg (&rp, &msg_p, CAMD_START_REQ_MSG);
    if (ret_val == RV_OK)
    {
        ret_val = rvf_send_msg (camd_env_ctrl_blk_p->addr_id, msg_p);
    }
    return ret_val;
}


CAM_BUF_Q cam_q[3]={{NULL,0},{NULL,0},{NULL,0}};

void camd_enqueue(T_CAMD_MSG *msg_p)
{
    UINT32 lockstate=0;
    UINT8 flag=0,i=0;

    lockstate=NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS);
    for(i=0;i<3;i++) 
    {

        if(cam_q[i].count==0) {
           cam_q[i].count=1;
           cam_q[i].data=msg_p;
           flag=1;
           break;
        }

    }


    if(flag==0)
    {
        CAMD_SEND_TRACE_PARAM("CAMD ENQUEUE ERROR",msg_p,
                RV_TRACE_LEVEL_ERROR);
        while(1);	
    }

    lockstate=NU_Local_Control_Interrupts(lockstate);
    CAMD_SEND_TRACE_PARAM("ENQUEUE",msg_p,
            RV_TRACE_LEVEL_DEBUG_HIGH );
    return;
}


T_CAMD_MSG* camd_dequeue()
{

    UINT8 i=0;

    UINT8 flag=0;
    T_CAMD_MSG * return_ptr=NULL;

    UINT32 lockstate=0;

    lockstate=NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS);

    if(cam_q[0].count==1)  {
       return_ptr=cam_q[0].data;
       cam_q[0].count=0;


       for(i=0;i<2;i++)   {
          cam_q[i].data=cam_q[i+1].data;
          cam_q[i].count=cam_q[i+1].count;     

       }

    } 

    lockstate=NU_Local_Control_Interrupts(lockstate);
    return (return_ptr);
}


void camd_flush(void)
{

    int i=0;

    UINT32 lockstate=0;

   // lockstate=NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS);

    for(i=0;i<3;i++)
    {
          if(cam_q[i].count!=0)
	    {
		    if (rvf_free_buf (cam_q[i].data) != RVF_OK)
                {
                    CAMD_SEND_TRACE ("CAMD CORE unable to free message",
                            RV_TRACE_LEVEL_ERROR);
                     
                }

	}
        cam_q[i].count=0;
    }
   //4 March 2008, OMAPS00165017 - x0083029 (Jessica) - Start
   /* 01/24/2008, Herman OMAP151386 Start */
   /*CAMERA CHANGES FOR MEMORY LEAK + CRASH*/
   if (rvf_free_buf (camd_env_ctrl_blk_p->Camd_dma_buffer) != RVF_OK)
      while(1); 
    /* 01/24/2008, Herman OMAP151386 End */
   //4 March 2008, OMAPS00165017 - x0083029 (Jessica) - End

   // lockstate=NU_Local_Control_Interrupts(lockstate);
    CAMD_SEND_TRACE ("flush",RV_TRACE_LEVEL_ERROR);
}


extern UINT8 Overflow_flag;
extern UINT8 Underflow_flag;
extern UINT8 Overflowcount;


void camd_trace_error()
{

    if(Underflow_flag)
    {
        CAMD_SEND_TRACE_PARAM("Underflow!!",Underflow_flag,RV_TRACE_LEVEL_ERROR);
        Underflow_flag = 0;
    }
    if(Overflow_flag)
    {
        CAMD_SEND_TRACE_PARAM("Overflow!!",Overflowcount,RV_TRACE_LEVEL_ERROR);
        Overflow_flag = 0;

    }
    return;
}

#ifdef CAM_BUFFER_TRACE

int cam_count_got_buf=0;
int cam_count_no_buf=0;
#endif


UINT8 Cam_Dma_callback(UINT16 dma_status)

{
    if ((dma_status & DMA_CICR_BLOCK_IE_MASK) == 
            DMA_CICR_BLOCK_IE_MASK)
    {


        if(camd_env_ctrl_blk_p->capturemode==TRUE)
        {

            return 0;
        }  

        else
        {
            T_CAMD_MSG * buf_local=camd_dequeue();
            if(buf_local!=NULL)	
            {
#ifdef CAM_BUFFER_TRACE
                cam_count_got_buf++;
#endif
                camd_env_ctrl_blk_p->Camd_App_buffer=camd_env_ctrl_blk_p->Camd_dma_buffer;
                camd_env_ctrl_blk_p->Camd_dma_buffer=buf_local;

                camd_env_ctrl_blk_p->camd_flag=1;


                /*Set up DMA*/
#ifdef CAM_BUFFER_TRACE
                cam_count_got_buf++;
#endif		
                C_DMA_CDSA_L_REG(camd_available_dma_channel)= ((UINT32)(camd_env_ctrl_blk_p->Camd_dma_buffer->body.buff)); /* is a mask requested ? */
                C_DMA_CDSA_U_REG(camd_available_dma_channel)=  ((UINT32)(camd_env_ctrl_blk_p->Camd_dma_buffer->body.buff) >> 16 ); /* is a mask requested ? */
                f_dma_channel_enable(camd_available_dma_channel);

                return 0;

            }
            else
            {

                C_DMA_CDSA_L_REG(camd_available_dma_channel)= ((UINT32)(camd_env_ctrl_blk_p->Camd_dma_buffer->body.buff)); /* is a mask requested ? */
                C_DMA_CDSA_U_REG(camd_available_dma_channel)=  ((UINT32)(camd_env_ctrl_blk_p->Camd_dma_buffer->body.buff) >> 16 ); /* is a mask requested ? */
                f_dma_channel_enable(camd_available_dma_channel);
#ifdef CAM_BUFFER_TRACE
                cam_count_no_buf++;
#endif		
                return 1;
            }

        } 
    }
    else
    {
        return 1;

    }

}








