/* 
+------------------------------------------------------------------------------
|  File:       ssl.h
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
|  Purpose :  Definitions for the Protocol Stack Entity ssl.
+----------------------------------------------------------------------------- 
*/ 

#ifndef SSL_H
#define SSL_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

#define MY_NAME                 "SSL"
#define VSI_CALLER              SSL_handle,
#define ENTITY_DATA             ssl_data
#define SSL_MAX_PLANES          5
#define SSL_MAX_VENDORID_LEN    20
#define SSL_MAX_VENDORNAME_LEN  20
#define SSL_MAX_NUM_DISP        2
#define LCD_MAXWIDTH            176
#define LCD_MAXHEIGHT           220
#define LCD_BITSPERPIXEL        16

#define ENCODE_OFFSET           0             /* Bitoffset for encoding/decoding */
#define hCommSSLClient          ssl_hCommSSLClient  /* Communication handle */

/* make the pei_create function unique */
#define pei_create              ssl_pei_create

/*==== TYPES =================================================================*/

/* SSL global typedefs */
/*
* @params 
*/

/*
 *  PLANE BLEND-TYPE ENUMERATION
 */
typedef enum
{
	SSL_PLANEBLEND_OVERLAP,
	SSL_PLANEBLEND_OVERLAY,
	SSL_PLANEBLEND_ALPHA,
	SSL_PLANEBLEND_OSD
}SSL_PLANE_BLENDTYPE;

typedef struct SSL_PLANE_PROPERTYTYPE SSL_PLANE_PROPERTYTYPE;
    
struct SSL_PLANE_PROPERTYTYPE
{
    OMX_U32 nXOffset;
    OMX_U32 nYOffset;
    OMX_U32 nXLen;
    OMX_U32 nYLen;
    OMX_SSL_PLANE_ALPHATYPE tAlpha;
    OMX_TICOLOR_FORMATTYPE tPlaneImgFormat;
    OMX_BOOL bActive;
    OMX_BOOL bAlwaysOnTop;
    OMX_BOOL bSuspended;
    OMX_BOOL bDSAPlane;
    OMX_BYTE pSourcePtr;
    OMX_BYTE pShadowPtr;
    OMX_BYTE pSSLDSAFramePtr;
    OMX_U32 nActiveDispID;      
    OMX_COMPONENTTYPE * hClientComponent;
    OMX_S32 nActiveIndex;
    /* Blend-Related properties */
    SSL_PLANE_BLENDTYPE tPlaneBlendType;
    OMX_U8 unPlaneAlpha;//Used if Alpha-Blending is chosen
    OMX_U16 unPlaneTransparency;//Used for Overlay
    /* Deferred Update */
    OMX_BOOL bDeferredUpdate;

    /* Shadow Config Structure */
    SSL_PLANE_PROPERTYTYPE * ptShadowConfig;
};

typedef struct                         /* T_SSL_DATA */
{
    unsigned char version;
    /*
    * entity parameters
    */
    OMX_U16 nNumDisplays;
    OMX_STRING pVendorNames;
    OMX_STRING pDeviceIDs;
    SSL_PLANE_PROPERTYTYPE ** pptPlaneData;
    SSL_PLANE_PROPERTYTYPE ** pptActivePlaneData;
    OMX_U32 unNumPlanes;
    OMX_BYTE ptFrameBuffer;
    OMX_S32 nActiveCount;
    OMX_BOOL bDSAPlane;
    OMX_U32 nDSAActiveIndex;
} T_SSL_DATA;


/*==== EXPORTS ===============================================================*/

short pei_create (T_PEI_INFO **info);
SSL_PLANE_PROPERTYTYPE * getPlaneHandle(OMX_COMPONENTTYPE * ptSSLClientHandle);
OMX_ERRORTYPE ssl_refresh(SSL_PLANE_PROPERTYTYPE * pCurPlane);
OMX_BOOL SSL_CheckOverlap(SSL_PLANE_PROPERTYTYPE * pPlaneHandle1, SSL_PLANE_PROPERTYTYPE * pPlaneHandle2);
void    sslcore_datacopy(SSL_PLANE_PROPERTYTYPE * pPlaneHandle);
#ifdef SSL_PEI_C

/* Entity data base */
T_SSL_DATA               ssl_data_base;
T_SSL_DATA               *ssl_data;

/* Communication handles */
T_HANDLE                 hCommSSLClient= VSI_ERROR;
T_HANDLE                 hCommSSL = VSI_ERROR;
T_HANDLE                 SSL_handle;

#else  /* SSL_PEI_C */

extern T_SSL_DATA       ssl_data_base, *ssl_data;
extern T_HANDLE          hCommSSLClient;
extern T_HANDLE          SSL_handle;

#endif /* SSL_PEI_C */

#endif /* !SSL_H */
