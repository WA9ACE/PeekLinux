#define AGILENT 0
#define MICRON 1

#if (CAM_SENSOR==AGILENT)
/*AGILENT SENSOR*/

/* 
   +----------------------------------------------------------------------------- 
   |  Project :  Riv2Gpf
   |  Module  :  CAMD
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
   |             for the entity CAMD
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

#ifndef _WINDOWS
#include "nucleus.h"
#include "memif/mem.h"
#endif

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

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "camd/camd_i.h"
#include "camd/camd_env.h"

#if CHIPSET ==15
extern void camd_init_camera(void);
extern void camcore_setGpioPins(void);
#endif


/*============================ Macros ========================================*/
#define VSI_CALLER CAMD_handle, 
#define pei_create camd_pei_create
#define RVM_TRACE_DEBUG_HIGH(string) \
    rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_HANDLE CAMD_handle;
T_HANDLE camd_hCommCAMD = -1;

static BOOL first_access = TRUE;

T_CAMD_ENV_CTRL_BLK *camd_env_ctrl_blk_p = 0;

const T_CAMD_PARAMETERS camd_default_viewfinder_parameters = {

   FALSE,							//capturemode
   CAMD_QCIF,					//resolution
   CAMD_CONTINUOUS,			//mode
#if CHIPSET !=15
   CAMD_RGB_888,				//encoding
#else
   CAMD_RGB_565,				//encoding
#endif  
   CAMD_GAMMA_CORR_DEFAULT,	//gamma_correction,
   176,							//imagewidth
   144,							//imageheight
   FALSE,						//black_and_white
   FALSE,						//flip_x
   FALSE,						//flip_y
   0,							//rotate
   0,							//zoom
   //{NULL},						//bitmap_data_p;
   NULL						//start_transfer_cb
};

const T_CAMD_PARAMETERS camd_default_snapshot_parameters = {
   TRUE,							//capturemode
   CAMD_VGA,					//resolution
   CAMD_CONTINUOUS,			//mode
   CAMD_YUYV_INTERLEAVED,		//encoding
   CAMD_GAMMA_CORR_DEFAULT,	//gamma_correction
   640,							//imagewidth
   480,							//imageheight
   FALSE,						//black_and_white
   FALSE,						//flip_x; 
   FALSE,						//flip_y; 
   0,						//rotate
   0,							//zoom
   //{0},							//mb_id
   //{NULL},						//bitmap_data_p
   //{0},							//buf_size
   //{TRUE}						//dma_transfer   
   NULL						//start_transfer_cb

};


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
#if 0
LOCAL SHORT pei_monitor (void ** out_monitor)
{
    RVM_TRACE_DEBUG_HIGH ("CAMD: pei_monitor");

    return PEI_OK;

} /* End pei_monitor(..) */


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

LOCAL SHORT pei_config (char *inString)
{
    RVM_TRACE_DEBUG_HIGH ("CAMD: pei_config");

    return PEI_OK;

}/* End of pei_config(..) */


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
LOCAL SHORT pei_timeout (unsigned short index)
{
    RVM_TRACE_DEBUG_HIGH ("CAMD: pei_timeout");

    return PEI_OK;

}/* End of pei_timeout(..) */


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
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
    RVM_TRACE_DEBUG_HIGH ("CAMD: pei_signal");

    return PEI_OK;

}/* End of pei_signal(..) */
#endif

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
    RVM_TRACE_DEBUG_HIGH ("CAMD: pei_exit");

    /*
     * Close communication channels
     */

    vsi_c_close (VSI_CALLER camd_hCommCAMD);
    camd_hCommCAMD = VSI_ERROR;

    camd_stop(NULL);

    camd_kill();

    return PEI_OK;

}/* End pei_exit(..) */


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
#if 0
LOCAL SHORT pei_primitive (void * primptr)
{
    T_RV_HDR* msg_p ; 

    RVM_TRACE_DEBUG_HIGH("CAMD: pei_primitive");

    msg_p = (T_RV_HDR*) P2D(primptr);
    camd_handle_message(msg_p);

    return PEI_OK;

}/* End of pei_primitive(..) */

#endif

/*
   +------------------------------------------------------------------------------
   | Function    : pei_run
   +------------------------------------------------------------------------------
   | Description : This function is called by the frame when entering the main
   |               loop. This fucntion is only required in the active variant.
   |
   |               This function is not used.
   |
   | Parameters  :  taskhandle  - handle of current process
   |                comhandle   - queue handle of current process
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

    RVM_TRACE_DEBUG_HIGH("CAMD: pei_run()");   

    /* loop to process messages */
    while (error_occured == FALSE)
    {
        /* Wait for the necessary events. */
        received_event = rvf_wait ( 0xffff,0);

        if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
        {
            msg_p = (T_RV_HDR*) rvf_read_mbox(0);

            sprintf( buf, "msg_p= %u ", (unsigned int *)msg_p);
            RVM_TRACE_DEBUG_HIGH(buf);

            camd_handle_message(msg_p);
        }
    }

    return PEI_OK;
}


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
    T_RV_RET ret = RV_OK;

    RVM_TRACE_DEBUG_HIGH("CAMD: pei_init");

    /*
     * Initialize task handle
     */
    CAMD_handle = handle;
    camd_hCommCAMD = -1;

    if(CAMD_handle != gsp_get_taskid())
    {
        RVM_TRACE_DEBUG_HIGH("CAMD_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( CAMD_handle, "CAMD" ) != RVF_OK)
    {
        RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (camd_hCommCAMD < VSI_OK)
    {
        if ((camd_hCommCAMD = vsi_c_open (VSI_CALLER "CAMD" )) < VSI_OK)
            return PEI_ERROR;
    }

    /* Create instance gathering all the variable used by EXPL instance */
    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_CAMD_ENV_CTRL_BLK),
                (T_RVF_BUFFER**)&camd_env_ctrl_blk_p) != RVF_GREEN)
    {
        /* The environemnt will cancel the EXPL instance creation. */
        CAMD_SEND_TRACE ("CAMD: Error to get memory ", RV_TRACE_LEVEL_ERROR);
        return RVM_MEMORY_ERR;
    }

    /* Store the pointer to the error function */
    camd_env_ctrl_blk_p->error_ft = rvm_error;

    /* Store the mem bank id. */
    camd_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;
    camd_env_ctrl_blk_p->sec_mb_id = EXT_MEM_POOL;

    /* Store the address id */
    camd_env_ctrl_blk_p->addr_id = CAMD_handle;
    camd_env_ctrl_blk_p->initialised = FALSE;
    camd_env_ctrl_blk_p->start_viewfinder=FALSE;

    camd_init();
    camd_start();

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
GLOBAL SHORT camd_pei_create (T_PEI_INFO **info)
{

    static const T_PEI_INFO pei_info =
    {
        "CAMD",        /* name */
        {              /* pei-table */
            pei_init,    /* pei_init */
            pei_exit,    /* pei_exit */
            NULL,        /* pei_primitive */
            NULL,        /* pei_timeout */
            NULL,        /* pei_signal */
            pei_run,     /*-- ACTIVE Entity--*/
            NULL,        /* pei_config */
            NULL         /* pei_monitor */
        },
        2048, //CAMD_STACK_SIZE,     /* stack size */
        10,       /* queue entries */
        (255 - CAMD_TASK_PRIORITY),      /* priority (1->low, 255->high) */
        0,        /* number of timers */
        COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND // | PASSIVE_BODY 
    };

    RVM_TRACE_DEBUG_HIGH("CAMD: pei_create");

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
}



/*
   +------------------------------------------------------------------------------
   | Function    : camd_init
   +------------------------------------------------------------------------------
   | Description : This function is called to initialise.
   |
   | Parameters  : None
   |
   | Return      : PEI_OK            - successful
   |               PEI_ERROR         - not successful
   +------------------------------------------------------------------------------
   */



#ifdef CAMD_TEST
#include "nucleus"
NU_TIMER camd_trace_timer;


extern int cam_count_got_buf;
extern int cam_count_no_buf;
extern int stopvf;
extern int cam_error;
extern int dma_time_current;
//extern int dma_time_current1;
//extern int dma_time_current2;

void camd_timer_expiry(UNSIGNED expiry_id)
{

    CAMD_SEND_TRACE_PARAM ("no",(cam_count_no_buf),
            RV_TRACE_LEVEL_ERROR);

    CAMD_SEND_TRACE_PARAM ("stpvf",(stopvf),
            RV_TRACE_LEVEL_ERROR);

    CAMD_SEND_TRACE_PARAM ("error",(cam_error),
            RV_TRACE_LEVEL_ERROR);


    CAMD_SEND_TRACE_PARAM ("D_time",(dma_time_current),
            RV_TRACE_LEVEL_ERROR);


}

#endif

T_RVM_RETURN camd_init (void)
{
    CAMD_SEND_TRACE ("CAMD: camd_init()", RV_TRACE_LEVEL_WARNING);

    camd_env_ctrl_blk_p->state = CAMD_INITIALISED;

    camd_env_ctrl_blk_p->sensor_enabled = FALSE;
    camd_env_ctrl_blk_p->snapshot_parameters_valid = FALSE;
    camd_env_ctrl_blk_p->viewfinder_parameters_valid = FALSE;
    camd_return_queue_init (camd_env_ctrl_blk_p->addr_id,
            CAMD_RETURN_QUEUE_EVENT,
            &camd_env_ctrl_blk_p->path_to_return_queue);
    camd_env_ctrl_blk_p->initialised = TRUE;
    camd_env_ctrl_blk_p->viewfinder_parameters =
        camd_default_viewfinder_parameters;
    camd_env_ctrl_blk_p->snapshot_parameters = camd_default_snapshot_parameters;

#if CHIPSET ==15

#ifdef CAMD_TEST
    /*timer to measure time taken for DMA completion*/    
    *(volatile UINT16 *)(0xFFFFF804)=0xF5;
    *(volatile UINT16 *)(0xFFFFF804)=0xA0;
    *(volatile UINT16 *)(0xFFFFF800)=0x0000;
    NU_Create_Timer(&camd_trace_timer, "1tdma",camd_timer_expiry , 
            11, 2000, 500 ,NU_ENABLE_TIMER);
#endif
    camd_init_camera();
    CAMD_SEND_TRACE ("CAMD init camera done", RV_TRACE_LEVEL_WARNING);
    camcore_setGpioPins();
    CAMD_SEND_TRACE ("CAMD gpio pins set", RV_TRACE_LEVEL_WARNING);

#endif

    return RVM_OK;
}


/*
   +------------------------------------------------------------------------------
   | Function    : camd_start
   +------------------------------------------------------------------------------
   | Description : Called to start the CAMD SWE.
   |
   | Parameters  : None
   |
   | Return      : PEI_OK            - successful
   |               PEI_ERROR         - not successful
   +------------------------------------------------------------------------------
   */
T_RVM_RETURN camd_start (void)
{
    CAMD_SEND_TRACE ("CAMD: camd_start()", RV_TRACE_LEVEL_WARNING);

    camd_env_ctrl_blk_p->state = CAMD_CAMERA_DISABLED;

    return RVM_OK;
}


/*
   +------------------------------------------------------------------------------
   | Function    : camd_stop
   +------------------------------------------------------------------------------
   | Description : Called to stop the CAMD SWE.
   |
   | Parameters  : Message
   |
   | Return      : PEI_OK            - successful
   |               PEI_ERROR         - not successful
   +------------------------------------------------------------------------------
   */
T_RVM_RETURN camd_stop (T_RV_HDR * unused)
{
    CAMD_SEND_TRACE ("CAMD: camd_stop()", RV_TRACE_LEVEL_WARNING);
    camd_env_ctrl_blk_p->state = CAMD_STOPPED;

    return RVM_OK;
}


/*
   +------------------------------------------------------------------------------
   | Function    : camd_kill
   +------------------------------------------------------------------------------
   | Description : Called to stop the CAMD SWE.
   |
   | Parameters  : None
   |
   | Return      : PEI_OK            - successful
   |               PEI_ERROR         - not successful
   +------------------------------------------------------------------------------
   */
T_RVM_RETURN camd_kill (void)
{
    CAMD_SEND_TRACE ("CAMD: camd_kill()", RV_TRACE_LEVEL_WARNING);

    rvf_free_buf (camd_env_ctrl_blk_p);

    camd_env_ctrl_blk_p->state = CAMD_KILLED;
    return RVM_OK;
}

/*
   +------------------------------------------------------------------------------
   | Function    : camd_get_info
   +------------------------------------------------------------------------------
   | Description : Called to get version Information.
   |
   | Parameters  : None
   |
   | Return      : PEI_OK            - successful
   |               PEI_ERROR         - not successful
   +------------------------------------------------------------------------------
   */
T_RVM_RETURN camd_get_info (T_RVM_INFO_SWE * swe_info)
{
    CAMD_SEND_TRACE ("CAMD: camd_get_info()", RV_TRACE_LEVEL_WARNING);

    swe_info->type_info.type3.version = BUILD_VERSION_NUMBER (0, 2, 0);

    return RVM_OK;
}


#else
/* 
   +----------------------------------------------------------------------------- 
   |  Project :  Riv2Gpf
   |  Module  :  CAMD
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
   |             for the entity CAMD
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

#ifndef _WINDOWS
#include "nucleus.h"
#include "memif/mem.h"
#endif

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

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "camd/camd_i.h"
#include "camd/camd_env.h"

#if CHIPSET ==15
extern void camd_init_camera(void);
extern void camcore_setGpioPins(void);
#endif


/*============================ Macros ========================================*/
#define VSI_CALLER CAMD_handle, 
#define pei_create camd_pei_create
#define RVM_TRACE_DEBUG_HIGH(string) \
    rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_HANDLE CAMD_handle;
T_HANDLE camd_hCommCAMD = -1;

static BOOL first_access = TRUE;

T_CAMD_ENV_CTRL_BLK *camd_env_ctrl_blk_p = 0;

/*const T_CAMD_PARAMETERS camd_default_viewfinder_parameters = {

   FALSE,							//capturemode
   CAMD_QCIF,					//resolution
   CAMD_CONTINUOUS,			//mode
#if CHIPSET !=15
   CAMD_RGB_888,				//encoding
#else
   CAMD_RGB_565,				//encoding
#endif  
   CAMD_GAMMA_CORR_DEFAULT,	//gamma_correction,
   176,							//imagewidth
   144,							//imageheight
   FALSE,						//black_and_white
   FALSE,						//flip_x
   FALSE,						//flip_y
   0,							//rotate
   0,							//zoom
   //{NULL},						//bitmap_data_p;
   NULL						//start_transfer_cb
};

const T_CAMD_PARAMETERS camd_default_snapshot_parameters = {
   TRUE,							//capturemode
   CAMD_SXGA,					//resolution
   CAMD_CONTINUOUS,			//mode
   CAMD_YUYV_INTERLEAVED,		//encoding
   CAMD_GAMMA_CORR_DEFAULT,	//gamma_correction
   1280,
   1024,						//imagewidth
   //imageheight
   FALSE,						//black_and_white
   FALSE,						//flip_x; 
   FALSE,						//flip_y; 
   0,						//rotate
   0,							//zoom
   //{0},							//mb_id
   //{NULL},						//bitmap_data_p
   //{0},							//buf_size
   //{TRUE}						//dma_transfer   
   NULL						//start_transfer_cb

};*/


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
#if 0
LOCAL SHORT pei_monitor (void ** out_monitor)
{
    RVM_TRACE_DEBUG_HIGH ("CAMD: pei_monitor");

    return PEI_OK;

} /* End pei_monitor(..) */


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

LOCAL SHORT pei_config (char *inString)
{
    RVM_TRACE_DEBUG_HIGH ("CAMD: pei_config");

    return PEI_OK;

}/* End of pei_config(..) */


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
LOCAL SHORT pei_timeout (unsigned short index)
{
    RVM_TRACE_DEBUG_HIGH ("CAMD: pei_timeout");

    return PEI_OK;

}/* End of pei_timeout(..) */


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
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
    RVM_TRACE_DEBUG_HIGH ("CAMD: pei_signal");

    return PEI_OK;

}/* End of pei_signal(..) */
#endif

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
    RVM_TRACE_DEBUG_HIGH ("CAMD: pei_exit");

    /*
     * Close communication channels
     */

    vsi_c_close (VSI_CALLER camd_hCommCAMD);
    camd_hCommCAMD = VSI_ERROR;

    camd_stop(NULL);

    camd_kill();

    return PEI_OK;

}/* End pei_exit(..) */


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
#if 0
LOCAL SHORT pei_primitive (void * primptr)
{
    T_RV_HDR* msg_p ; 

    RVM_TRACE_DEBUG_HIGH("CAMD: pei_primitive");

    msg_p = (T_RV_HDR*) P2D(primptr);
    camd_handle_message(msg_p);

    return PEI_OK;

}/* End of pei_primitive(..) */

#endif

/*
   +------------------------------------------------------------------------------
   | Function    : pei_run
   +------------------------------------------------------------------------------
   | Description : This function is called by the frame when entering the main
   |               loop. This fucntion is only required in the active variant.
   |
   |               This function is not used.
   |
   | Parameters  :  taskhandle  - handle of current process
   |                comhandle   - queue handle of current process
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

    RVM_TRACE_DEBUG_HIGH("CAMD: pei_run()");   

    /* loop to process messages */
    while (error_occured == FALSE)
    {
        /* Wait for the necessary events. */
        received_event = rvf_wait ( 0xffff,0);

        if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
        {
            msg_p = (T_RV_HDR*) rvf_read_mbox(0);

            sprintf( buf, "msg_p= %u ", (unsigned int *)msg_p);
            RVM_TRACE_DEBUG_HIGH(buf);

            camd_handle_message(msg_p);
        }
    }

    return PEI_OK;
}


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
    T_RV_RET ret = RV_OK;

    RVM_TRACE_DEBUG_HIGH("CAMD: pei_init");

    /*
     * Initialize task handle
     */
    CAMD_handle = handle;
    camd_hCommCAMD = -1;

    if(CAMD_handle != gsp_get_taskid())
    {
        RVM_TRACE_DEBUG_HIGH("CAMD_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( CAMD_handle, "CAMD" ) != RVF_OK)
    {
        RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (camd_hCommCAMD < VSI_OK)
    {
        if ((camd_hCommCAMD = vsi_c_open (VSI_CALLER "CAMD" )) < VSI_OK)
            return PEI_ERROR;
    }

    /* Create instance gathering all the variable used by EXPL instance */
    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_CAMD_ENV_CTRL_BLK),
                (T_RVF_BUFFER**)&camd_env_ctrl_blk_p) != RVF_GREEN)
    {
        /* The environemnt will cancel the EXPL instance creation. */
        CAMD_SEND_TRACE ("CAMD: Error to get memory ", RV_TRACE_LEVEL_ERROR);
        return RVM_MEMORY_ERR;
    }

    /* Store the pointer to the error function */
    camd_env_ctrl_blk_p->error_ft = rvm_error;

    /* Store the mem bank id. */
    camd_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;
    camd_env_ctrl_blk_p->sec_mb_id = EXT_MEM_POOL;

    /* Store the address id */
    camd_env_ctrl_blk_p->addr_id = CAMD_handle;
    camd_env_ctrl_blk_p->initialised = FALSE;
    camd_env_ctrl_blk_p->start_viewfinder=FALSE;

    camd_init();
    camd_start();

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
GLOBAL SHORT camd_pei_create (T_PEI_INFO **info)
{

    static const T_PEI_INFO pei_info =
    {
        "CAMD",        /* name */
        {              /* pei-table */
            pei_init,    /* pei_init */
            pei_exit,    /* pei_exit */
            NULL,        /* pei_primitive */
            NULL,        /* pei_timeout */
            NULL,        /* pei_signal */
            pei_run,     /*-- ACTIVE Entity--*/
            NULL,        /* pei_config */
            NULL         /* pei_monitor */
        },
        2048, //CAMD_STACK_SIZE,     /* stack size */
        10,       /* queue entries */
        (255 - CAMD_TASK_PRIORITY),      /* priority (1->low, 255->high) */
        0,        /* number of timers */
        COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND // | PASSIVE_BODY 
    };

    RVM_TRACE_DEBUG_HIGH("CAMD: pei_create");

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
}



/*
   +------------------------------------------------------------------------------
   | Function    : camd_init
   +------------------------------------------------------------------------------
   | Description : This function is called to initialise.
   |
   | Parameters  : None
   |
   | Return      : PEI_OK            - successful
   |               PEI_ERROR         - not successful
   +------------------------------------------------------------------------------
   */

#define CAMD_TEST

#ifdef CAMD_TEST
#include "nucleus"
NU_TIMER camd_trace_timer;



//extern int dma_time_current1;
//extern int dma_time_current2;

void camd_timer_expiry(UNSIGNED expiry_id)
{

    camd_trace_error();


}

#endif

T_RVM_RETURN camd_init (void)
{
    CAMD_SEND_TRACE ("CAMD: camd_init()", RV_TRACE_LEVEL_WARNING);

    camd_env_ctrl_blk_p->state = CAMD_INITIALISED;

    camd_env_ctrl_blk_p->sensor_enabled = FALSE;
    camd_env_ctrl_blk_p->snapshot_parameters_valid = FALSE;
    camd_env_ctrl_blk_p->viewfinder_parameters_valid = FALSE;
    camd_return_queue_init (camd_env_ctrl_blk_p->addr_id,
            CAMD_RETURN_QUEUE_EVENT,
            &camd_env_ctrl_blk_p->path_to_return_queue);
    camd_env_ctrl_blk_p->initialised = TRUE;
    //camd_env_ctrl_blk_p->viewfinder_parameters =
      //  camd_default_viewfinder_parameters;
  //  camd_env_ctrl_blk_p->snapshot_parameters = camd_default_snapshot_parameters;

#if CHIPSET ==15

#ifdef CAMD_TEST
    /*timer to measure time taken for DMA completion*/    
    /**(volatile UINT16 *)(0xFFFFF804)=0xF5;
     *(volatile UINT16 *)(0xFFFFF804)=0xA0;
     *(volatile UINT16 *)(0xFFFFF800)=0x0000;*/
    NU_Create_Timer(&camd_trace_timer, "1tdma",camd_timer_expiry , 
            11, 2000, 500 ,NU_ENABLE_TIMER);
#endif
    camd_init_camera();
    CAMD_SEND_TRACE ("CAMD init camera done", RV_TRACE_LEVEL_WARNING);
    camcore_setGpioPins();
    CAMD_SEND_TRACE ("CAMD gpio pins set", RV_TRACE_LEVEL_WARNING);

#endif

    return RVM_OK;
}


/*
   +------------------------------------------------------------------------------
   | Function    : camd_start
   +------------------------------------------------------------------------------
   | Description : Called to start the CAMD SWE.
   |
   | Parameters  : None
   |
   | Return      : PEI_OK            - successful
   |               PEI_ERROR         - not successful
   +------------------------------------------------------------------------------
   */
T_RVM_RETURN camd_start (void)
{
    CAMD_SEND_TRACE ("CAMD: camd_start()", RV_TRACE_LEVEL_WARNING);

    camd_env_ctrl_blk_p->state = CAMD_CAMERA_DISABLED;

    return RVM_OK;
}


/*
   +------------------------------------------------------------------------------
   | Function    : camd_stop
   +------------------------------------------------------------------------------
   | Description : Called to stop the CAMD SWE.
   |
   | Parameters  : Message
   |
   | Return      : PEI_OK            - successful
   |               PEI_ERROR         - not successful
   +------------------------------------------------------------------------------
   */
T_RVM_RETURN camd_stop (T_RV_HDR * unused)
{
    CAMD_SEND_TRACE ("CAMD: camd_stop()", RV_TRACE_LEVEL_WARNING);
    camd_env_ctrl_blk_p->state = CAMD_STOPPED;

    return RVM_OK;
}


/*
   +------------------------------------------------------------------------------
   | Function    : camd_kill
   +------------------------------------------------------------------------------
   | Description : Called to stop the CAMD SWE.
   |
   | Parameters  : None
   |
   | Return      : PEI_OK            - successful
   |               PEI_ERROR         - not successful
   +------------------------------------------------------------------------------
   */
T_RVM_RETURN camd_kill (void)
{
    CAMD_SEND_TRACE ("CAMD: camd_kill()", RV_TRACE_LEVEL_WARNING);

    rvf_free_buf (camd_env_ctrl_blk_p);

    camd_env_ctrl_blk_p->state = CAMD_KILLED;
    return RVM_OK;
}

/*
   +------------------------------------------------------------------------------
   | Function    : camd_get_info
   +------------------------------------------------------------------------------
   | Description : Called to get version Information.
   |
   | Parameters  : None
   |
   | Return      : PEI_OK            - successful
   |               PEI_ERROR         - not successful
   +------------------------------------------------------------------------------
   */
T_RVM_RETURN camd_get_info (T_RVM_INFO_SWE * swe_info)
{
    CAMD_SEND_TRACE ("CAMD: camd_get_info()", RV_TRACE_LEVEL_WARNING);

    swe_info->type_info.type3.version = BUILD_VERSION_NUMBER (0, 2, 0);

    return RVM_OK;
}




#endif
