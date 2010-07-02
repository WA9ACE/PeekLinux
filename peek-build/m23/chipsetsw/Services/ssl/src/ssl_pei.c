/* 
   +------------------------------------------------------------------------------
   |  File:       ssl_pei.c
   +------------------------------------------------------------------------------
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
   |  Purpose :  This module implements the PEI interface
   |             for the entity ssl.
   +----------------------------------------------------------------------------- 
   */ 

#define SSL_PEI_C

#define ENTITY_SSL

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get standard types */
#include "vsi.h"        /* to get a lot of macros */
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */
#include "mon_ssl.h"    /* to get mon-definitions */
#include "OMX_Types.h"
#include "OMX_Core.h"
#include "OMX_SSLComponent.h" /* Defines for the Client-Core interface */
#include "OMX_SSL_Private.h"
#include "omx_tiimage.h"
#include "ssl.h"        /* to get the global entity definitions */
#include "alpha_overlay.h" /*Alpha Blending*/
#include "exp_colorconv_rotate_scaling.h" /* Rotation Testing  Header*/
#include "lcd_interface.h" /*LCD Interface*/

/*==== CONSTS ================================================================*/
/* 
 * Wait as long as possible. This is the time in ms that is waited for a 
 * message in the input queue in the active variant. 
 * It can be decreased by the customer.  
 */   
#define SSL_TIMEOUT     0xffffffffL   

/*==== TYPES =================================================================*/

/*==== LOCALS ================================================================*/

static  BOOL            first_access    = TRUE;
static  BOOL            exit_flag       = FALSE;
static  T_MONITOR       ssl_mon;
extern  T_HANDLE        mm_ext_data_pool_handle;
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
    OMX_SSL_CMDTYPE  tCMD;
    OMX_PTR          pCoreRetPtr;
    OMX_ERRORTYPE    tRetErr = OMX_ErrorNone;
    T_HANDLE         entityHandle;

    OMX_SSL_MESSAGE_STRUCTTYPE * tSSLMsg  =  (OMX_SSL_MESSAGE_STRUCTTYPE *)P2D(primptr);
    OMX_COMPONENTTYPE * ptSSLClientHandle = tSSLMsg->pComponentHandle;
    SSL_PLANE_PROPERTYTYPE * pPlaneHandle = getPlaneHandle(ptSSLClientHandle);  

    tCMD                = tSSLMsg->tCMD;
    ssl_data            = &ssl_data_base;
    entityHandle        = e_running[os_MyHandle()];       

    OMX_ENTRYEXITFUNC_TRACE("SSL PEI PRIMITIVE");

    switch(tCMD)
    {
        case OMX_SSL_CMD_SETPLANEPROPERTIES:
            if(pPlaneHandle == NULL)
            {
                tRetErr = OMX_ErrorNotReady;
            }
            else
            {
                //A valid plane already exists - need to update the plane config parameters
                OMX_SSL_PLANE_CONFIGTYPE * pSSLParams = (OMX_SSL_PLANE_CONFIGTYPE *)(tSSLMsg->pArgument);
                pPlaneHandle->bDeferredUpdate         = pSSLParams->bDeferredUpdate;

                if(pPlaneHandle->bDeferredUpdate == OMX_FALSE)
                {
                    OMX_PROFILESTUB("Refresh in setconfig in case of flase deferred update");

                    pPlaneHandle->nXOffset              = pSSLParams->nXOffset;
                    pPlaneHandle->nYOffset              = pSSLParams->nYOffset;
                    pPlaneHandle->nXLen                 = pSSLParams->nXLen;
                    pPlaneHandle->nYLen                 = pSSLParams->nYLen;
                    pPlaneHandle->tAlpha                = pSSLParams->tAlpha;
                    pPlaneHandle->tPlaneImgFormat       = pSSLParams->tPlaneImgFormat;
                    pPlaneHandle->bAlwaysOnTop          = pSSLParams->bAlwaysOnTop;
                    pPlaneHandle->tPlaneBlendType       = (SSL_PLANE_BLENDTYPE)pSSLParams->tPlaneBlendType;
                    pPlaneHandle->unPlaneAlpha          = pSSLParams->unPlaneAlpha;
                    pPlaneHandle->unPlaneTransparency   = pSSLParams->unPlaneTransparency;
                    pPlaneHandle->nActiveDispID         = pSSLParams->nActiveDispID;
                    pPlaneHandle->bDSAPlane             = pSSLParams->bDSAPlane;

                    //refresh if no active DSA planes present and the plane is active
                    if(ssl_data->nDSAActiveIndex == 0xFFFFFFFF &&
                            pPlaneHandle->bActive == OMX_TRUE &&
                            pPlaneHandle->pSourcePtr != NULL &&
                            pPlaneHandle->pShadowPtr != NULL)
                    {
                        tRetErr  = ssl_refresh(pPlaneHandle);
                        
                    }
                }
                else
                {
                    SSL_PLANE_PROPERTYTYPE * pShadowConfig = (SSL_PLANE_PROPERTYTYPE*)pPlaneHandle->ptShadowConfig;

                    pShadowConfig->nXOffset             = pSSLParams->nXOffset;
                    pShadowConfig->nYOffset             = pSSLParams->nYOffset;
                    pShadowConfig->nXLen                = pSSLParams->nXLen;
                    pShadowConfig->nYLen                = pSSLParams->nYLen;
                    pShadowConfig->tAlpha               = pSSLParams->tAlpha;
                    pShadowConfig->tPlaneImgFormat      = pSSLParams->tPlaneImgFormat;
                    pShadowConfig->bAlwaysOnTop         = pSSLParams->bAlwaysOnTop;
                    pShadowConfig->tPlaneBlendType      = (SSL_PLANE_BLENDTYPE)pSSLParams->tPlaneBlendType;
                    pShadowConfig->unPlaneAlpha         = pSSLParams->unPlaneAlpha;
                    pShadowConfig->unPlaneTransparency  = pSSLParams->unPlaneTransparency;
                    pShadowConfig->nActiveDispID        = pSSLParams->nActiveDispID;
                }                
            }
            break;

        case OMX_SSL_CMD_CREATEPLANE:
            {                
                SSL_PLANE_PROPERTYTYPE   *pShadowConfig;
                OMX_SSL_PLANE_CONFIGTYPE *pSSLParams = (OMX_SSL_PLANE_CONFIGTYPE *)(tSSLMsg->pArgument);                

                OMX_PROFILESTUB("Create Plane");

                pPlaneHandle = getPlaneHandle(NULL);                

                if(pPlaneHandle == NULL)
                {
                    tRetErr = OMX_ErrorInsufficientResources;
                    break;
                }

                pPlaneHandle->nXOffset                  = pSSLParams->nXOffset;
                pPlaneHandle->nYOffset                  = pSSLParams->nYOffset;
                pPlaneHandle->nXLen                     = pSSLParams->nXLen;
                pPlaneHandle->nYLen                     = pSSLParams->nYLen;
                pPlaneHandle->tAlpha                    = pSSLParams->tAlpha;
                pPlaneHandle->tPlaneImgFormat           = pSSLParams->tPlaneImgFormat;
                pPlaneHandle->bAlwaysOnTop              = pSSLParams->bAlwaysOnTop;
                pPlaneHandle->bDSAPlane                 = pSSLParams->bDSAPlane;
                pPlaneHandle->nActiveDispID             = pSSLParams->nActiveDispID;
                pPlaneHandle->bDeferredUpdate           = pSSLParams->bDeferredUpdate;
                pPlaneHandle->tPlaneBlendType           = (SSL_PLANE_BLENDTYPE)pSSLParams->tPlaneBlendType;
                pPlaneHandle->unPlaneAlpha              = pSSLParams->unPlaneAlpha;
                pPlaneHandle->unPlaneTransparency       = pSSLParams->unPlaneTransparency;
                pPlaneHandle->hClientComponent          = ptSSLClientHandle;
                pPlaneHandle->bActive                   = OMX_FALSE;
                pPlaneHandle->bSuspended                = OMX_FALSE;

                //copy config data to shadow config 
                pShadowConfig = (SSL_PLANE_PROPERTYTYPE*)pPlaneHandle->ptShadowConfig;
                pShadowConfig->nXOffset                 = pSSLParams->nXOffset;
                pShadowConfig->nYOffset                 = pSSLParams->nYOffset;
                pShadowConfig->nXLen                    = pSSLParams->nXLen;
                pShadowConfig->nYLen                    = pSSLParams->nYLen;
                pShadowConfig->tAlpha                   = pSSLParams->tAlpha;
                pShadowConfig->tPlaneImgFormat          = pSSLParams->tPlaneImgFormat;
                pShadowConfig->bDSAPlane                = pSSLParams->bDSAPlane;
                pShadowConfig->bAlwaysOnTop             = pSSLParams->bAlwaysOnTop;
                pShadowConfig->tPlaneBlendType          = (SSL_PLANE_BLENDTYPE)pSSLParams->tPlaneBlendType;
                pShadowConfig->unPlaneAlpha             = pSSLParams->unPlaneAlpha;
                pShadowConfig->unPlaneTransparency      = pSSLParams->unPlaneTransparency;

                //initialize plane buffer pointers to null
                pPlaneHandle->pSourcePtr                = NULL;
                pPlaneHandle->pShadowPtr                = NULL;
                pPlaneHandle->pSSLDSAFramePtr           = NULL;

                //initialize return DSA pointer as NULL
                pCoreRetPtr                             = NULL;

                if(pPlaneHandle->bDSAPlane == OMX_TRUE)
                {                 
                    //Only one DSA plane : if more requested, return failure
                    if(ssl_data->bDSAPlane == OMX_TRUE)
                    {
                        tRetErr = OMX_ErrorInsufficientResources;                       
                    }
                    else
                    {
                        //calculate pointer in DSA plane with the requested offset
                        pPlaneHandle->pSSLDSAFramePtr = ssl_data->ptFrameBuffer + 
                            (((pPlaneHandle->nXOffset + 
                               pPlaneHandle->nYOffset * LCD_MAXWIDTH) *  
                              LCD_BITSPERPIXEL)>>3 * sizeof(char));

                        ssl_data->unNumPlanes       = ssl_data->unNumPlanes + 1;
                        pCoreRetPtr                 = pPlaneHandle->pSSLDSAFramePtr;
                        ssl_data->bDSAPlane         = OMX_TRUE;
                        ssl_data->nDSAActiveIndex   = 0xFFFFFFFF;
                    }
                }                               
                else
                {                    
                    //Create the shadow buffer 
                    pPlaneHandle->pShadowPtr = (OMX_BYTE)OMX_ALLOC((pPlaneHandle->nXLen * pPlaneHandle->nYLen * LCD_BITSPERPIXEL )>> 3 * sizeof(char));

                    if(pPlaneHandle->pShadowPtr == NULL)
                    {
                        tRetErr = OMX_ErrorInsufficientResources;
                    }
                    else
                    {                    
                        ssl_data->unNumPlanes       = ssl_data->unNumPlanes + 1;
                    }
                }
                //free the parameter structure
                OMX_FREE(pSSLParams);
            } 
        OMX_PROFILESTUB("Plane created");
            break;      

        case OMX_SSL_CMD_ACTIVATEPLANE:

            if(pPlaneHandle == NULL)
            {
                tRetErr = OMX_ErrorNotReady;
            }
            else
            {
                 OMX_PROFILESTUB("Activate Plane");
        pPlaneHandle->nActiveIndex      = ssl_data->nActiveCount;
                pPlaneHandle->bActive           = OMX_TRUE;

                //push plane handle to active queue
                ssl_data->pptActivePlaneData[pPlaneHandle->nActiveIndex] = pPlaneHandle;

                if(pPlaneHandle->bDSAPlane == OMX_TRUE)
                {
                    ssl_data->nDSAActiveIndex = pPlaneHandle->nActiveIndex;
                }

                ssl_data->nActiveCount       = ssl_data->nActiveCount + 1;
                //refresh if no active DSA planes present and the plane is active
                if(ssl_data->nDSAActiveIndex == 0xFFFFFFFF &&
                        pPlaneHandle->bActive == OMX_TRUE &&
                        pPlaneHandle->pSourcePtr != NULL &&
                        pPlaneHandle->pShadowPtr != NULL &&
                        pPlaneHandle->bDeferredUpdate == OMX_FALSE)
                {
                    OMX_PROFILESTUB("Refresh");
            tRetErr = ssl_refresh(pPlaneHandle);
                }

            }
        OMX_PROFILESTUB("Plane activated");
            break;      

        case OMX_SSL_CMD_SUSPENDPLANE:

            if(pPlaneHandle == NULL)
            {
                tRetErr = OMX_ErrorNotReady;
            }
            else
            {
                pPlaneHandle->bSuspended = OMX_TRUE;
            }
            //ssl_refresh(pPlaneHandle);
            break;

        case OMX_SSL_CMD_DESTROYPLANE:
        OMX_PROFILESTUB("Destroy Plane");
            if(pPlaneHandle->bActive == OMX_TRUE)
            {
                OMX_U8 i;
                //Remove the plane from the active queue and rearrange
                for(i = pPlaneHandle->nActiveIndex; i < (ssl_data->nActiveCount -1); i++)
                {                      
                    ssl_data->pptActivePlaneData[i+1]->nActiveIndex--;
                    ssl_data->pptActivePlaneData[i] = ssl_data->pptActivePlaneData[i+1];
                }

                //for last plane in the queue
                ssl_data->pptActivePlaneData[i] = NULL;

                ssl_data->nActiveCount--;
            }

            if(pPlaneHandle->bDSAPlane == OMX_TRUE)
            {
                ssl_data->nDSAActiveIndex       = 0xFFFFFFFF;
                ssl_data->bDSAPlane             = OMX_FALSE;
            }
            else
            {
                //free shadow memory space
                OMX_FREE(pPlaneHandle->pShadowPtr);
            }

            pPlaneHandle->pSSLDSAFramePtr = NULL;        
            pPlaneHandle->pSourcePtr        = NULL;
            pPlaneHandle->pShadowPtr        = NULL;
            pPlaneHandle->hClientComponent  = NULL;
            pPlaneHandle->nActiveIndex      = -1;
            pPlaneHandle->bActive           = OMX_FALSE;
            pPlaneHandle->bSuspended        = OMX_FALSE;
            pPlaneHandle->bDSAPlane         = OMX_FALSE;

            ssl_data->unNumPlanes--;
        OMX_PROFILESTUB("Plane destroyed");
            //tRetErr = ssl_refresh(pPlaneHandle);
            break;

        case OMX_SSL_CMD_DEACTIVATEPLANE:
            OMX_PROFILESTUB("Deactivate plane");
            if(pPlaneHandle->bActive ==OMX_TRUE)
            {
                OMX_U8 i;
                //Remove the plane from the active queue and rearrange
                for(i = pPlaneHandle->nActiveIndex; i < (ssl_data->nActiveCount - 1); i++)
                {                      
                    ssl_data->pptActivePlaneData[i+1]->nActiveIndex--;
                    ssl_data->pptActivePlaneData[i] = ssl_data->pptActivePlaneData[i+1];
                }
                //for last plane in the queue
                ssl_data->pptActivePlaneData[i] = NULL;

                ssl_data->nActiveCount--;
                pPlaneHandle->bActive = OMX_FALSE;
                pPlaneHandle->nActiveIndex = -1;

                if(pPlaneHandle->bDSAPlane == OMX_TRUE)
                {
                    ssl_data->nDSAActiveIndex = 0xFFFFFFFF;
                }
            }

            //if(pPlaneHandle->pSourcePtr != NULL)
            //tRetErr = ssl_refresh(pPlaneHandle);
        OMX_PROFILESTUB("Plane deactivated");
            break; 

        case OMX_SSL_CMD_RESUMEPLANE:
            if(pPlaneHandle == NULL)
            {
                tRetErr = OMX_ErrorNotReady;
            }
            else
            {
                pPlaneHandle->bSuspended = OMX_FALSE;
                OMX_PROFILESTUB("Refresh in Resume Plane");
            }
            //tRetErr = ssl_refresh(pPlaneHandle);   
            break;       

        case OMX_SSL_CMD_SETINBUFF:            
            //Set the return buffer for callback
            if(pPlaneHandle == NULL)
            {
                tRetErr = OMX_ErrorNotReady;
            }
            else
            {
                pCoreRetPtr = (OMX_PTR)tSSLMsg->pArgument;
                if(pPlaneHandle->bActive == OMX_TRUE)
                {
                    if(pPlaneHandle->bDeferredUpdate == OMX_TRUE)
                    {
                        /* Copy from the Shadow Config to the Plane handle */
                        SSL_PLANE_PROPERTYTYPE * pShadowConfig = (SSL_PLANE_PROPERTYTYPE*)pPlaneHandle->ptShadowConfig;
                        pPlaneHandle->nXOffset          = pShadowConfig->nXOffset;
                        pPlaneHandle->nYOffset          = pShadowConfig->nYOffset;
                        pPlaneHandle->nXLen             = pShadowConfig->nXLen;
                        pPlaneHandle->nYLen             = pShadowConfig->nYLen;
                        pPlaneHandle->tAlpha            = pShadowConfig->tAlpha;
                        pPlaneHandle->tPlaneImgFormat   = pShadowConfig->tPlaneImgFormat;
                        pPlaneHandle->bAlwaysOnTop      = pShadowConfig->bAlwaysOnTop;
                        pPlaneHandle->tPlaneBlendType   = (SSL_PLANE_BLENDTYPE)pShadowConfig->tPlaneBlendType;
                        pPlaneHandle->unPlaneAlpha      = pShadowConfig->unPlaneAlpha;
                        pPlaneHandle->unPlaneTransparency  = pShadowConfig->unPlaneTransparency;
                        pPlaneHandle->nActiveDispID     = pShadowConfig->nActiveDispID;

                        pPlaneHandle->bDeferredUpdate   = OMX_FALSE;
                    }

                }

                if(pPlaneHandle->bDSAPlane == OMX_FALSE)
                {
                    OMX_BUFFERHEADERTYPE tBuffer    = *(OMX_BUFFERHEADERTYPE *)(tSSLMsg->pArgument);
                    pPlaneHandle->pSourcePtr        = tBuffer.pBuffer;

                    if(pPlaneHandle->pSourcePtr != NULL &&
                            pPlaneHandle->pShadowPtr != NULL)
                    {
                        sslcore_datacopy(pPlaneHandle);
                    }

                    //refresh if pointers are valid and the plane is active
                    if(pPlaneHandle->bActive == OMX_TRUE &&
                            pPlaneHandle->pShadowPtr != NULL&&ssl_data->nDSAActiveIndex == 0xFFFFFFFF)
                    {
                        tRetErr = ssl_refresh(pPlaneHandle);
                    }            
                }
                else
                {
                    pPlaneHandle->pSourcePtr = pPlaneHandle->pSSLDSAFramePtr;

                    //for dsa planes, always refresh for new data
                    tRetErr = ssl_refresh(pPlaneHandle);
                }
            }

            break;    

        default:
            tRetErr = OMX_ErrorNotReady;
            break;
    }

    vsi_c_pfree((T_VOID_STRUCT **)&tSSLMsg);

    __OMX_SSL_Callback(ptSSLClientHandle, 
            tRetErr, 
            tCMD, 
            pCoreRetPtr);

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
    OMX_U32 i;
    SSL_PLANE_PROPERTYTYPE ** pSSLPlaneProp;
    T_HANDLE entityHandle;

    entityHandle = e_running[os_MyHandle()];     
    TRACE_FUNCTION ("pei_init");
    /* Initialize task handle */
    SSL_handle = handle;

    // Get my own handle. Needed in pei_run() for ACTIVE_BODY configuration
    if (hCommSSL < VSI_OK)
    {
        if ((hCommSSL = vsi_c_open (VSI_CALLER MY_NAME)) < VSI_OK)
            return PEI_ERROR;
    }

    /*
     * Initialize global pointer ssl_data. This is required to access all
     * entity data.
     */
    ssl_data                    = &ssl_data_base;
    
     //Do all initializations
    ssl_data->unNumPlanes       = 0;
    ssl_data->nNumDisplays      = 1;
    ssl_data->bDSAPlane         = OMX_FALSE;
    ssl_data->nDSAActiveIndex   = 0xFFFFFFFF;
    ssl_data->nActiveCount      = 0;

    //create plane handles
    ssl_data->pptPlaneData = (SSL_PLANE_PROPERTYTYPE **)OMX_ALLOC(SSL_MAX_PLANES * sizeof(SSL_PLANE_PROPERTYTYPE *));

    //initialize plane properties with default values
    pSSLPlaneProp = ssl_data->pptPlaneData;
    for(i=0;i<SSL_MAX_PLANES;i++)
    {
        pSSLPlaneProp[i] = (SSL_PLANE_PROPERTYTYPE*)OMX_ALLOC(sizeof(SSL_PLANE_PROPERTYTYPE));
        pSSLPlaneProp[i]->hClientComponent      = NULL;
        pSSLPlaneProp[i]->nActiveIndex          = -1;
        pSSLPlaneProp[i]->ptShadowConfig        = (SSL_PLANE_PROPERTYTYPE*)OMX_ALLOC(sizeof(SSL_PLANE_PROPERTYTYPE));
    }

    /*
     * Create the Framebuffer; width * height * bits per pixel / 8 bytes
     */
    ssl_data->ptFrameBuffer = (OMX_BYTE)OMX_ALLOC((LCD_MAXWIDTH * LCD_MAXHEIGHT * LCD_BITSPERPIXEL * sizeof(char)) >> 3);

    /*
     * Data Processing Queue creations
     */
    ssl_data->pptActivePlaneData = (SSL_PLANE_PROPERTYTYPE **)OMX_ALLOC(SSL_MAX_PLANES * sizeof(SSL_PLANE_PROPERTYTYPE *));

    /*
     * Initialize entity data (call init function of every service)
     */
    lcd_display(DISPLAY_MAIN_LCD, (UINT16 *)ssl_data->ptFrameBuffer, NULL);

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
    TRACE_FUNCTION ("pei_timeout");

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
    TRACE_FUNCTION ("pei_signal");

    /* Process signal */
    switch (opc)
    {
        default:
            TRACE_ERROR("Unknown Signal OPC");
            return PEI_ERROR;
    }
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
    SSL_PLANE_PROPERTYTYPE ** pSSLPlaneProp;
    OMX_U8 i;
    TRACE_FUNCTION ("pei_exit");

    OMX_FREE(ssl_data->ptFrameBuffer FILE_LINE_MACRO);
    OMX_FREE(ssl_data->pptActivePlaneData FILE_LINE_MACRO);

    pSSLPlaneProp = ssl_data->pptPlaneData;

    for(i=0;i<SSL_MAX_PLANES;i++)
    {
    OMX_FREE(pSSLPlaneProp[i]->ptShadowConfig);
        OMX_FREE(pSSLPlaneProp[i]);
    }
    OMX_FREE(ssl_data->pptPlaneData);

    exit_flag = TRUE;
    //Free up all allocated buffers

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
    unsigned long timeout = SSL_TIMEOUT;
    OS_QDATA Msg;
    T_HANDLE entityHandle;

    entityHandle = e_running[os_MyHandle()];     
  
    TRACE_FUNCTION ("pei_run");
 
    while (!exit_flag)
    {
        //os_ReceiveFromQueue(0, pf_TaskTable[hCommSSL].QueueHandle, &Msg, -1);
        if(vsi_c_await (hCommSSL, hCommSSL, &message, timeout) == VSI_OK)
        {
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
    }

    exit_flag = FALSE;

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
    TRACE_FUNCTION ("pei_config");
    TRACE_FUNCTION (in_string);

    if ( ConfigTimer ( VSI_CALLER in_string, NULL ) == VSI_OK )
        return PEI_OK;

    /*
     *  further dynamic configuration
     */
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
    TRACE_FUNCTION ("pei_monitor");

    /*
     * Version = "0.S" (S = Step).
     */
    ssl_mon.version = "ssl 0.1";
    *out_monitor = &ssl_mon;

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
        "SSL",                 /* name */
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
        1024,          /* stack size */
        10,            /* queue entries */
        101,//15,           /* priority (1->low, 255->high) */
        10,            /* number of timers */
        COPY_BY_REF|TRC_NO_SUSPEND|PRIM_NO_SUSPEND|INT_DATA_TASK          /* flags: bit 0   active(0) body/passive(1) */ //ACTIVE BODY
    };               /*        bit 1   com by copy(0)/reference(1) */

    /*
     * Export startup configuration data
     */
    *info = &pei_info;

    return PEI_OK;
}

SSL_PLANE_PROPERTYTYPE * getPlaneHandle(OMX_COMPONENTTYPE * ptSSLClientHandle)
{
    OMX_U8 i;
    SSL_PLANE_PROPERTYTYPE ** ppPlaneTemp = ssl_data->pptPlaneData;

    for(i=0; i<SSL_MAX_PLANES; i++)
    {
        if(ppPlaneTemp[i]->hClientComponent == ptSSLClientHandle)
        {
            return ppPlaneTemp[i];
        }
    }
    return NULL;
}

OMX_ERRORTYPE ssl_refresh(SSL_PLANE_PROPERTYTYPE * pCurPlane)
{
    //This is the most important function in this module!!!!
    /*
     * This function retrieves elements from the Active and Always on Top Queue; treat DSA planes separately
     * call Alpha Blending, and compose the framebuffer before calling the LCD refresh.
     * DSA planes are already present in the framebuffer. 
     */
    OMX_U8 i,j,k;
    OMX_BOOL bOverlap = OMX_FALSE;
    SSL_PLANE_PROPERTYTYPE * ptCurPlaneHandle;   
    tBaseVideoFrame_JDEmz tSrcFrame, tDstFrame;
    OMX_U32 unAlpha;

    OMX_ENTRYEXITFUNC_TRACE("SSL REFRESH");

    /*
     * For non-DSA, non-Suspended, non-Always on Top active planes
     */
    for(i=0;i<ssl_data->nActiveCount;i++)
    {
        ptCurPlaneHandle = ssl_data->pptActivePlaneData[i];
        if(  (OMX_FALSE== ptCurPlaneHandle->bDSAPlane) &&
                (OMX_FALSE== ptCurPlaneHandle->bSuspended) &&
                (OMX_FALSE== ptCurPlaneHandle->bAlwaysOnTop) &&
                (NULL != ptCurPlaneHandle->pSourcePtr))
        {                

            if(ptCurPlaneHandle->tPlaneBlendType == SSL_PLANEBLEND_OVERLAY)
            {    
                tDstFrame.lum = ptCurPlaneHandle->pShadowPtr;
                tDstFrame.width = ptCurPlaneHandle->nXLen;
                tDstFrame.height = ptCurPlaneHandle->nYLen;

                tSrcFrame.lum = ssl_data->ptFrameBuffer;
                tSrcFrame.width = LCD_MAXWIDTH;
                tSrcFrame.height = LCD_MAXHEIGHT;    
                unAlpha = (OMX_U32)ptCurPlaneHandle->tAlpha;
                if(IPEmz_gOverLay(tSrcFrame.lum, tDstFrame.lum,  tDstFrame.lum,
                            tSrcFrame.width, tSrcFrame.height,
                            tDstFrame.width, tDstFrame.height,
                            ptCurPlaneHandle->nXOffset,ptCurPlaneHandle->nYOffset,
                            ptCurPlaneHandle->unPlaneTransparency, 0) !=0)
                {
                    return OMX_ErrorStreamCorrupt;
                }     
            }

            if(ptCurPlaneHandle->tPlaneBlendType == SSL_PLANEBLEND_OVERLAP)
            {
                OMX_U16* ptSrc = (OMX_U16 *)ptCurPlaneHandle->pShadowPtr;
                OMX_U16* ptDst = (OMX_U16 *)ssl_data->ptFrameBuffer + LCD_MAXWIDTH * ptCurPlaneHandle->nYOffset + ptCurPlaneHandle->nXOffset;
                OMX_U16  nXlen = ptCurPlaneHandle->nXLen;
                OMX_U16  nYlen = ptCurPlaneHandle->nYLen;
                OMX_U16 *ptSrcTemp, *ptDestTemp;

                for(j=0;j<nYlen;j++)
                {
                    ptSrcTemp = ptSrc + nXlen* j;
                    ptDestTemp = ptDst +  LCD_MAXWIDTH* j;
                    memcpy(ptDestTemp, ptSrcTemp, nXlen*sizeof(OMX_U16));
                }            
            }

            if(ptCurPlaneHandle->tPlaneBlendType == SSL_PLANEBLEND_ALPHA)
            {
                tDstFrame.lum = ptCurPlaneHandle->pShadowPtr;
                tDstFrame.width = ptCurPlaneHandle->nXLen;
                tDstFrame.height = ptCurPlaneHandle->nYLen;                   

                tSrcFrame.lum = ssl_data->ptFrameBuffer;
                tSrcFrame.width = LCD_MAXWIDTH;
                tSrcFrame.height = LCD_MAXHEIGHT;   

                if(IPEmz_gAlphaBlending(&tSrcFrame, 
                            &tDstFrame,
                            &tSrcFrame,
                            ptCurPlaneHandle->nXOffset,
                            ptCurPlaneHandle->nYOffset,
                            ptCurPlaneHandle->unPlaneAlpha) !=0)
                {
                    return OMX_ErrorStreamCorrupt;
                }                      
            }

            if(ptCurPlaneHandle->tPlaneBlendType == SSL_PLANEBLEND_OSD)
            {
                tDstFrame.lum = ptCurPlaneHandle->pShadowPtr;
                tDstFrame.width = ptCurPlaneHandle->nXLen;
                tDstFrame.height = ptCurPlaneHandle->nYLen;

                tSrcFrame.lum = ssl_data->ptFrameBuffer;
                tSrcFrame.width = LCD_MAXWIDTH;
                tSrcFrame.height = LCD_MAXHEIGHT;    
                unAlpha = (OMX_U32)ptCurPlaneHandle->tAlpha;
                if(IPEmz_gOverLay(tSrcFrame.lum, tDstFrame.lum,  tDstFrame.lum,
                            tSrcFrame.width, tSrcFrame.height,
                            tDstFrame.width, tDstFrame.height,
                            ptCurPlaneHandle->nXOffset,ptCurPlaneHandle->nYOffset,
                            ptCurPlaneHandle->unPlaneTransparency, 0) !=0)
                {
                    return OMX_ErrorStreamCorrupt;
                }

                tDstFrame.lum = ptCurPlaneHandle->pShadowPtr;
                tDstFrame.width = ptCurPlaneHandle->nXLen;
                tDstFrame.height = ptCurPlaneHandle->nYLen;                   

                tSrcFrame.lum = ssl_data->ptFrameBuffer;
                tSrcFrame.width = LCD_MAXWIDTH;
                tSrcFrame.height = LCD_MAXHEIGHT;   

                if(IPEmz_gAlphaBlending(&tSrcFrame, 
                            &tDstFrame,
                            &tSrcFrame,
                            ptCurPlaneHandle->nXOffset,
                            ptCurPlaneHandle->nYOffset,
                            ptCurPlaneHandle->unPlaneAlpha) !=0)
                {
                    return OMX_ErrorStreamCorrupt;
                }                 
            }

        }
    }

    /*
     * For non-DSA, non-Suspended, Always on Top active planes
     */
    for(i=0;i<ssl_data->nActiveCount;i++)
    {
        ptCurPlaneHandle = ssl_data->pptActivePlaneData[i];
        if((OMX_FALSE == ptCurPlaneHandle->bDSAPlane) &&
                (OMX_FALSE == ptCurPlaneHandle->bSuspended) &&
                (OMX_TRUE == ptCurPlaneHandle->bAlwaysOnTop)&&
                (NULL != ptCurPlaneHandle->pSourcePtr))
        {                
            if(ptCurPlaneHandle->tPlaneBlendType == SSL_PLANEBLEND_OVERLAY)
            {
                tDstFrame.lum = ptCurPlaneHandle->pShadowPtr;
                tDstFrame.width = ptCurPlaneHandle->nXLen;
                tDstFrame.height = ptCurPlaneHandle->nYLen;                   

                tSrcFrame.lum = ssl_data->ptFrameBuffer;
                tSrcFrame.width = LCD_MAXWIDTH;
                tSrcFrame.height = LCD_MAXHEIGHT;   
                unAlpha = (OMX_U32)ptCurPlaneHandle->tAlpha; 
                if(IPEmz_gOverLay(tSrcFrame.lum, tDstFrame.lum,  tDstFrame.lum,
                            tSrcFrame.width, tSrcFrame.height,
                            tDstFrame.width, tDstFrame.height,
                            ptCurPlaneHandle->nXOffset,ptCurPlaneHandle->nYOffset,
                            ptCurPlaneHandle->unPlaneTransparency, 0) !=0)
                {
                    return OMX_ErrorStreamCorrupt;
                }                      
            }

            if(ptCurPlaneHandle->tPlaneBlendType == SSL_PLANEBLEND_OVERLAP)
            {
                {
                    OMX_U16* ptSrc = (OMX_U16 *)ptCurPlaneHandle->pShadowPtr;
                    OMX_U16* ptDst = (OMX_U16 *)ssl_data->ptFrameBuffer + LCD_MAXWIDTH * ptCurPlaneHandle->nYOffset + ptCurPlaneHandle->nXOffset;
                    OMX_U16  nXlen = ptCurPlaneHandle->nXLen;
                    OMX_U16  nYlen = ptCurPlaneHandle->nYLen;
                    OMX_U16 nXOff = ptCurPlaneHandle->nXOffset;
                    OMX_U16 nYOff = ptCurPlaneHandle->nYOffset;
                    OMX_U16 *ptSrcTemp, *ptDestTemp;


                    for(j=0;j<nYlen;j++)
                    {
                        ptSrcTemp = ptSrc + nXlen* j;
                        ptDestTemp = ptDst +  LCD_MAXWIDTH* j;
                        memcpy(ptDestTemp, ptSrcTemp, nXlen*sizeof(OMX_U16));
                    }
                }
            }

            if(ptCurPlaneHandle->tPlaneBlendType == SSL_PLANEBLEND_ALPHA)
            {
                tDstFrame.lum = ptCurPlaneHandle->pShadowPtr;
                tDstFrame.width = ptCurPlaneHandle->nXLen;
                tDstFrame.height = ptCurPlaneHandle->nYLen;                   

                tSrcFrame.lum = ssl_data->ptFrameBuffer;
                tSrcFrame.width = LCD_MAXWIDTH;
                tSrcFrame.height = LCD_MAXHEIGHT;   

                if(IPEmz_gAlphaBlending(&tSrcFrame, 
                            &tDstFrame,
                            &tSrcFrame,
                            ptCurPlaneHandle->nXOffset,
                            ptCurPlaneHandle->nYOffset,
                            ptCurPlaneHandle->unPlaneAlpha) !=0)
                {
                    return OMX_ErrorStreamCorrupt;
                }                      
            }

            if(ptCurPlaneHandle->tPlaneBlendType == SSL_PLANEBLEND_OSD)
            {
                tDstFrame.lum = ptCurPlaneHandle->pShadowPtr;
                tDstFrame.width = ptCurPlaneHandle->nXLen;
                tDstFrame.height = ptCurPlaneHandle->nYLen;

                tSrcFrame.lum = ssl_data->ptFrameBuffer;
                tSrcFrame.width = LCD_MAXWIDTH;
                tSrcFrame.height = LCD_MAXHEIGHT;    
                unAlpha = (OMX_U32)ptCurPlaneHandle->tAlpha;
                if(IPEmz_gOverLay(tSrcFrame.lum, tDstFrame.lum,  tDstFrame.lum,
                            tSrcFrame.width, tSrcFrame.height,
                            tDstFrame.width, tDstFrame.height,
                            ptCurPlaneHandle->nXOffset,ptCurPlaneHandle->nYOffset,
                            ptCurPlaneHandle->unPlaneTransparency, 0) !=0)
                {
                    return OMX_ErrorStreamCorrupt;
                }

                tDstFrame.lum = ptCurPlaneHandle->pShadowPtr;
                tDstFrame.width = ptCurPlaneHandle->nXLen;
                tDstFrame.height = ptCurPlaneHandle->nYLen;                   

                tSrcFrame.lum = ssl_data->ptFrameBuffer;
                tSrcFrame.width = LCD_MAXWIDTH;
                tSrcFrame.height = LCD_MAXHEIGHT;   

                if(IPEmz_gAlphaBlending(&tSrcFrame, 
                            &tDstFrame,
                            &tSrcFrame,
                            ptCurPlaneHandle->nXOffset,
                            ptCurPlaneHandle->nYOffset,
                            ptCurPlaneHandle->unPlaneAlpha) !=0)
                {
                    return OMX_ErrorStreamCorrupt;
                }                 
            }        
        }         
    }  

    //display the frame buffer
    if (RV_OK != lcd_display(DISPLAY_MAIN_LCD, (UINT16 *)ssl_data->ptFrameBuffer, NULL))
        return OMX_ErrorNotReady;
   else
        return OMX_ErrorNone;
}

void   sslcore_datacopy(SSL_PLANE_PROPERTYTYPE * pPlaneHandle)
{
    //Copy data from source from shadow bufffer
    OMX_BYTE pSrcPtr = pPlaneHandle->pSourcePtr;
    OMX_BYTE pDstPtr = pPlaneHandle->pShadowPtr;
    OMX_U32 unCopySize = ((pPlaneHandle->nXLen * pPlaneHandle->nYLen * LCD_BITSPERPIXEL) >> 3)*sizeof(char);
    OMX_U32 i;
    memcpy(pDstPtr, pSrcPtr, unCopySize);
}
/*==== END OF FILE ==========================================================*/
