/* =============================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * =========================================================================== */
/**
 * @file img_pei.c
 *
 * This file implements pei interface for img entity
 *
 * @path  chipsetsw\services\img\src
 *
 * @rev  0.1
 *
 */
/* ------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 31-Jan-2006 Ramesh Anandhi: Initial Release
 *!
 * ========================================================================= */

/****************************************************************
 *  constants
 ****************************************************************/
#define IMG_PEI_C

#define ENTITY_IMG
/* 
 * Wait as long as possible. This is the time in ms that is waited for a 
 * message in the input queue in the active variant. 
 * It can be decreased by the customer.  
 */   
#define IMG_TIMEOUT     0xFFFFFFFF   

/****************************************************************
 *  INCLUDE FILES
 ****************************************************************/
#include "typedefs.h"   /* to get standard types */
#include "vsi.h"        /* to get a lot of macros */
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */
#include "mon_img.h"    /* to get mon-definitions */
#include "OMX_Types.h"
#include "omx_imgcomponent.h" /* Defines for the Client-Core interface */
#include "OMX_IMG_Private.h"
#include "OMX_TIImage.h"
#include "common.h"
#include "baseImageEnc.h"
#include "JpegEncoder_rom_jmptab.h"
#include "jd_emz_rom_jmptab.h"
#include "img.h"        /* to get the global entity definitions */
#include "img_pool_size.h" /* to get stack size */

extern const T_JPEGENC_ROM_JMPTAB JpegEnc_rom_jmptab;
extern const T_JDEMZ_ROM_JMPTAB jd_emz_jmptab;
const T_JPEGENC_ROM_JMPTAB *JpegEnc_rom_jmptab_p;
const T_JDEMZ_ROM_JMPTAB *jd_emz_jmptab_p;

/* reserve a section for jpeg in the command file */ 
#pragma DATA_SECTION(Emuzed_JpegEnc_rom_jmptab, "JPEG_JUMP_TABLE") 
T_JDEMZ_ROM_JMPTAB Emuzed_JpegEnc_rom_jmptab;

#pragma DATA_SECTION(Emuzed_jd_emz_jmptab, "JPEG_JUMP_TABLE") 
T_JDEMZ_ROM_JMPTAB Emuzed_jd_emz_jmptab;



extern T_HANDLE MmGroupHandle;
/****************************************************************
 *  global initializations
 ****************************************************************/
T_IMG_ENV_CTRL_BLK *img_env_ctrl_blk_p = NULL;

static  OMX_BOOL        first_access  = OMX_TRUE;
static  OMX_BOOL        exit_flag = OMX_FALSE;
static  T_MONITOR       img_mon;

//local function prototype
LOCAL IMG_CORE_OPERATIONTYPE * getIMGCompHandle(OMX_HANDLETYPE * ptIMGClientHandle);

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
LOCAL SHORT pei_primitive (void * primptr)
{
    OMX_IMG_CMDTYPE             tCMD;
    IMG_CORE_OPERATIONTYPE      *pOp;
    OMX_HANDLETYPE              *hComp;
	OMX_PTR                     pCompParam;
        
    OMX_IMG_CORE_MSGTYPE *img_msg       = (OMX_IMG_CORE_MSGTYPE *) P2D(primptr);    
    OMX_ERRORTYPE retVal                = OMX_ErrorNone;

    OMX_ENTRYEXITFUNC_TRACE("Enter pei_primitive of IMG");
    tCMD                                = img_msg->nCmd;
    hComp                               = (OMX_HANDLETYPE)img_msg->pComponentHandle ;
	
	/* dereference parameter */
    pCompParam                          = img_msg->tCompParam;

    if (primptr!= NULL)
    {
        if (img_msg->nCmd == OMX_IMG_CMD_CORE_INIT) 
        {                                  
            /* get a free handle */
            pOp                 = getIMGCompHandle(NULL);

            /* intialize the operation structure from the message parameters */
            pOp->hComp          = hComp;
                       
			pOp->pCoreCallback          = img_msg->pIMGCallback;
            pOp->tCoreSts               = IMG_CORE_STS_IDLE;

			retVal = (OMX_ERRORTYPE)img_core_setconfig(pOp, pCompParam);			
        }
        else     
        {
            pOp = (IMG_CORE_OPERATIONTYPE *)getIMGCompHandle(hComp);
            
            switch (img_msg->nCmd)
            {                
                case OMX_IMG_CMD_STATE_CHANGE:
                    {
                        OMX_STATETYPE pState = *((OMX_STATETYPE *) pCompParam);
                        retVal= (OMX_ERRORTYPE)img_core_state_change(pOp, pState);
						break;
                    }
                case OMX_IMG_CMD_QUEUE_INPBUF:
                    {
                        retVal=(OMX_ERRORTYPE) img_queue_buf(pOp, pCompParam, IMG_INPUT);
                        break;
                    }
                case OMX_IMG_CMD_QUEUE_OUTBUF:
                    {
                        retVal= (OMX_ERRORTYPE)img_queue_buf(pOp, pCompParam, IMG_OUTPUT);
                        break;
                    }
                case OMX_IMG_CMD_SETCONFIG:
                    {
                        retVal= (OMX_ERRORTYPE)img_core_setconfig(pOp, pCompParam);
                        break;
                    }
                case OMX_IMG_CMD_GETCONFIG:
                    {
                        retVal=(OMX_ERRORTYPE) img_core_getconfig(pOp, pCompParam);
                        break;
                    }   
				case OMX_IMG_CMD_CORE_ABORT: 
			{
                       	retVal= (OMX_ERRORTYPE)img_core_state_change(pOp, OMX_StateIdle);
                       	break;
			}
                case OMX_IMG_CMD_CORE_FREE:
                    {
                        /* free up the component by setting the component handle as NULL */
                        pOp->hComp              = NULL;
                        pOp->tCoreSts           = IMG_CORE_STS_IDLE;
                        break;
                    }
                default: 
                    /* Unknown message has been received */
                    TRACE_ERROR("IMG: Received unknown message type");
                    return PEI_ERROR; 
            }

           }
               
        if ((tCMD != OMX_IMG_CMD_QUEUE_INPBUF) && (tCMD != OMX_IMG_CMD_QUEUE_OUTBUF))
            pOp->pCoreCallback(hComp, retVal, tCMD, img_msg->tCompParam);       

	if(pOp->tCoreSts == IMG_CORE_STS_RUNNING &&
		(tCMD == OMX_IMG_CMD_QUEUE_INPBUF ||
		tCMD == OMX_IMG_CMD_QUEUE_OUTBUF ||
		tCMD == OMX_IMG_CMD_STATE_CHANGE))
        {
                //now call the actual data processing function
                img_core_running(pOp);
	 }
        vsi_c_pfree((T_VOID_STRUCT **)&img_msg);
    }
    OMX_ENTRYEXITFUNC_TRACE("Exit pei_primitive of IMG");
    return PEI_OK;   
}/* End of pei_primitive(..) */

/*==== PRIVATE FUNCTIONS ====================================================*/
/*
   +------------------------------------------------------------------------------
   |  Function     : pei_init
   +------------------------------------------------------------------------------
   |  Description  :  Initialize Protocol Stack Entity
   |
   |  Parameters   :  handle    - task handle
   |
   |  Return       :  PEI_OK    - entity initialised
   |                  PEI_ERROR - entity not (yet) initialised
   +------------------------------------------------------------------------------
   */
static short pei_init (T_HANDLE handle)
{
    OMX_U8 i;
    OMX_U8 *jpeg_jmp_tab_end_addr_p = (OMX_U8 *)JPEG_JMP_TAB_LOC_BOTTOM;    
    jd_emz_jmptab_p            =  (T_JDEMZ_ROM_JMPTAB*)((OMX_U8*)jpeg_jmp_tab_end_addr_p -  sizeof(T_JDEMZ_ROM_JMPTAB));        
    JpegEnc_rom_jmptab_p   = (T_JPEGENC_ROM_JMPTAB *)((OMX_U8 *)jd_emz_jmptab_p- sizeof(T_JPEGENC_ROM_JMPTAB));   

    OMX_ENTRYEXITFUNC_TRACE ("Enter pei_init of IMG");
    /* Initialize task handle */
    IMG_handle = handle;
    
    /*
     * Get my own handle. Needed in pei_run() for ACTIVE_BODY configuration
     */
    if (hCommIMG < VSI_OK)
    {
        if ((hCommIMG = vsi_c_open (VSI_CALLER MY_NAME)) < VSI_OK)
            return PEI_ERROR;
    }

    //allocate memory for current operation structure
    img_env_ctrl_blk_p= (T_IMG_ENV_CTRL_BLK*)OMX_ALLOC(sizeof(T_IMG_ENV_CTRL_BLK));
    if(img_env_ctrl_blk_p == NULL)
                return PEI_ERROR;
//    rvf_send_trace("imgpei alloc ", 12, sizeof(T_IMG_ENV_CTRL_BLK), RV_TRACE_LEVEL_ERROR,0);
    /* Store the address ID as core handle */
    img_env_ctrl_blk_p->addr_id = IMG_handle;
    
    if(img_env_ctrl_blk_p->addr_id != NULL)
    {
        //allocate memory for img components
        for(i=0; i<IMG_MAX_COMPONENTS; i++)
        {
            img_env_ctrl_blk_p->apIMGComp[i] = (IMG_CORE_OPERATIONTYPE *)OMX_ALLOC(sizeof(IMG_CORE_OPERATIONTYPE));
//        rvf_send_trace("imgpei alloc ", 12, sizeof(IMG_CORE_OPERATIONTYPE), RV_TRACE_LEVEL_ERROR,0);
           if(img_env_ctrl_blk_p->apIMGComp[i] == NULL)
                return PEI_ERROR;
           img_env_ctrl_blk_p->apIMGComp[i]->hComp = NULL;
	    img_env_ctrl_blk_p->apIMGComp[i]->pInpBufQueue.nLast = 0;
	    img_env_ctrl_blk_p->apIMGComp[i]->pOutBufQueue.nLast = 0;
       }           
    }
    else
    {
        return PEI_ERROR;
    }
    memcpy((OMX_PTR)JpegEnc_rom_jmptab_p, &JpegEnc_rom_jmptab, sizeof(T_JPEGENC_ROM_JMPTAB));       
    memcpy((OMX_PTR)jd_emz_jmptab_p, &jd_emz_jmptab, sizeof(T_JDEMZ_ROM_JMPTAB ));
    
    OMX_ENTRYEXITFUNC_TRACE("Exit pei_init of IMG");
    return (PEI_OK);
}

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
static short pei_timeout (unsigned short index)
{
    OMX_ENTRYEXITFUNC_TRACE ("Enter pei_timeout of IMG");

    /* Process timeout */
    switch (index)
    {
        case 0:
            /* Call of timeout routine */
            break;
        default:
            TRACE_ERROR("Unknown Timeout");
            return PEI_ERROR;
    }
    OMX_ENTRYEXITFUNC_TRACE("Exit pei_timeout of IMG");
    return PEI_OK;
}

/*
   +------------------------------------------------------------------------------
   |  Function     :  pei_signal
   +------------------------------------------------------------------------------
   |  Description  :  Process signal.
   |
   |  Parameters   :  opc       - signal operation code
   |                  data      - pointer to primitive
   |
   |  Return       :  PEI_OK    - signal processed
   |                  PEI_ERROR - signal not processed
   +------------------------------------------------------------------------------
   */
static short pei_signal (unsigned long opc, void* data)
{
    OMX_ENTRYEXITFUNC_TRACE ("Enter pei_signal of IMG");

    /* Process signal */
    switch (opc)
    {
        default:
            TRACE_ERROR("Unknown Signal OPC");
		OMX_ENTRYEXITFUNC_TRACE("Exit pei_signal of IMG");
            return PEI_ERROR;
    }
    //OMX_ENTRYEXITFUNC_TRACE("Exit pei_signal of IMG");
    //return PEI_OK;
}

/*
   +------------------------------------------------------------------------------
   |  Function     :  pei_exit
   +------------------------------------------------------------------------------
   |  Description  :  Close Resources and terminate.
   |
   |  Parameters   :            - 
   |
   |  Return       :  PEI_OK    - exit sucessful
   +------------------------------------------------------------------------------
   */
static short pei_exit (void)
{
    U8 i;

    OMX_ENTRYEXITFUNC_TRACE ("Enter pei_exit of IMG");
    //delete memory allocated for components
    for(i=0; i<IMG_MAX_COMPONENTS; i++)
    {
       OMX_FREE(img_env_ctrl_blk_p->apIMGComp[i]);        
    }

    OMX_FREE(img_env_ctrl_blk_p);
    
    exit_flag = OMX_TRUE;
    OMX_ENTRYEXITFUNC_TRACE("Exit pei_exit of IMG");
    return PEI_OK;
}

/*
   +------------------------------------------------------------------------------
   |  Function     :  pei_run
   +------------------------------------------------------------------------------
   |  Description  :  Process Primitives, main loop is located in the
   |                  Protocol Stack Entity.
   |                  Only needed in active body variant
   |
   |  Parameters   :  taskhandle  - handle of current process
   |                  comhandle   - queue handle of current process
   |
   |  Return       :  PEI_OK      - sucessful
   |                  PEI_ERROR   - not successful
   +------------------------------------------------------------------------------
   */
static short pei_run (T_HANDLE taskhandle, T_HANDLE comhandle )
{
    T_QMSG message;
    T_HANDLE entityHandle;

    OMX_ENTRYEXITFUNC_TRACE("Enter pei_run of IMG");
    entityHandle = e_running[os_MyHandle()];     
	while (!exit_flag)
    {    
        //wait for messages to core
        vsi_c_await(hCommIMG, hCommIMG, &message, IMG_TIMEOUT);

        switch (message.MsgType)
        {
            case MSG_PRIMITIVE:
                if (((T_PRIM_HEADER*)message.Msg.Primitive.Prim)->opc & SYS_MASK)
                    {
                      vsi_c_primitive (entityHandle, message.Msg.Primitive.Prim);
                    }
                    else
                    {
                      pei_primitive (message.Msg.Primitive.Prim );
                    }
                break;
            case MSG_SIGNAL:
                pei_signal ( (USHORT)message.Msg.Signal.SigOPC, message.Msg.Signal.SigBuffer );
                break;
            case MSG_TIMEOUT:
                pei_timeout ( (USHORT)message.Msg.Timer.Index );
                break;
            default:
                TRACE_ERROR("Unknown Message Type");
                break;
        }
    }

    exit_flag = OMX_FALSE;
    OMX_ENTRYEXITFUNC_TRACE("Exit pei_run of IMG");
    return PEI_OK;
}

/*
   +------------------------------------------------------------------------------
   |  Function     :  pei_config
   +------------------------------------------------------------------------------
   |  Description  :  Dynamic Configuration.
   |
   |  Parameters   :  in_string   - configuration string
   |
   |  Return       :  PEI_OK      - sucessful
   |                  PEI_ERROR   - not successful
   +------------------------------------------------------------------------------
   */
static short pei_config (char *in_string)
{
    OMX_ENTRYEXITFUNC_TRACE ("Enter pei_config of IMH");
    OMX_ENTRYEXITFUNC_TRACE (in_string);

    if ( ConfigTimer ( VSI_CALLER in_string, NULL ) == VSI_OK ) {
        OMX_ENTRYEXITFUNC_TRACE("Exit pei_config of IMG");     
         return PEI_OK;
    }
      return PEI_OK;
}

/*
   +------------------------------------------------------------------------------
   |  Function     :  pei_monitor
   +------------------------------------------------------------------------------
   |  Description  :  Monitoring of physical Parameters.
   |
   |  Parameters   :  out_monitor - return the address of the data to be monitored
   |
   |  Return       :  PEI_OK      - sucessful (address in out_monitor is valid)
   |                  PEI_ERROR   - not successful
   +------------------------------------------------------------------------------
   */
static short pei_monitor (void ** out_monitor)
{
    OMX_ENTRYEXITFUNC_TRACE ("Enter pei_monitor of IMG");

    /*
     * Version = "0.S" (S = Step).
     */
    img_mon.version = "img 0.1";
    *out_monitor = &img_mon;
    OMX_ENTRYEXITFUNC_TRACE ("Exit pei_monitor of IMG");
    return PEI_OK;
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
   +------------------------------------------------------------------------------
   |  Function     :  pei_create
   +------------------------------------------------------------------------------
   |  Description  :  Create the Protocol Stack Entity.
   |
   |  Parameters   :  info        - Pointer to the structure of entity parameters
   |
   |  Return       :  PEI_OK      - entity created successfully
   |                  
   +------------------------------------------------------------------------------
   */
short pei_create (T_PEI_INFO **info)
{
   
    static T_PEI_INFO pei_info =
    {
        "IMG",                 /* name */
        {                        /* pei-table */
            pei_init,
            pei_exit,
            NULL,                /*pei_primitive */
            NULL,                /* pei_timeout */
            NULL,                /* pei_signal */
            pei_run,
            NULL,                /* pei_config */
            NULL,                /* pei_monitor*/
        },
        IMG_STACK_SIZE,          /* stack size */
        10,            /* queue entries */
        14,           /* priority (1->low, 255->high) */
        10,            /* number of timers */
        COPY_BY_REF|TRC_NO_SUSPEND|PRIM_NO_SUSPEND|INT_DATA_TASK  /* flags: bit 0   active(0) body/passive(1) */ //ACTIVE BODY
    };               /*        bit 1   com by copy(0)/reference(1) */

    /*
     * Export startup configuration data
     */
 OMX_ENTRYEXITFUNC_TRACE ("Enter pei_create of IMG");
    /*
     * Close Resources if open
     */
    if (first_access)
        first_access = OMX_FALSE;
    else
        pei_exit();

    /*
     * Export startup configuration data
     */
    *info = (T_PEI_INFO *)&pei_info;

    OMX_ENTRYEXITFUNC_TRACE ("Exit pei_create of IMG");
    return PEI_OK;

}


/*
   +------------------------------------------------------------------------------
   |  Function     :  getIMGCompHandle
   +------------------------------------------------------------------------------
   |  Description  :  Returns handle to a component from the global list
   |
   |  Parameters   :  info        - Pointer to the structure of entity parameters
   |
   |  Return       :  
   |                  
   +------------------------------------------------------------------------------
   */ 
LOCAL IMG_CORE_OPERATIONTYPE * getIMGCompHandle(OMX_HANDLETYPE * ptIMGClientHandle)
{
    OMX_U8 i;
    IMG_CORE_OPERATIONTYPE ** apIMGComp = img_env_ctrl_blk_p->apIMGComp;
    
    OMX_ENTRYEXITFUNC_TRACE ("Enter IMG_CORE_OPERATIONTYPE");
    for(i=0;i<IMG_MAX_COMPONENTS;i++)
    {
        if(apIMGComp[i]->hComp == ptIMGClientHandle)
        {
            OMX_ENTRYEXITFUNC_TRACE ("Exit IMG_CORE_OPERATIONTYPE ");
            return apIMGComp[i];
        }
    }
    OMX_ENTRYEXITFUNC_TRACE ("Exit IMG_CORE_OPERATIONTYPE ");
    return NULL;
}

/*==== END OF FILE ==========================================================*/

