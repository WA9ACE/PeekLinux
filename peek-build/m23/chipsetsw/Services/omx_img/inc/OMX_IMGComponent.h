/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file OMX_TIImage.h
*
* This file contains the structure definitions specific to Imaging. 
*
* @path  chipsetsw\services\omx_img\
*
* @rev  0.1
*
*/
/* ------------------------------------------------------------------------- */
/* ==========================================================================
*!
*! Revision History
*! ===================================
*! 15-Feb-2006 anandhi@ti.com: Initial Release

*!
* ========================================================================== */
#ifndef OMX_IMGComponent_h
#define OMX_IMGComponent_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "OMX_Types.h"
#include "omx_tiimage.h"
#include "OMX_Component.h"


/** Component name for the OMX Imaging component **/
#define OMX_TIIMG_COMPONENT "OMX.TI.IMAGE.IMAGING"

/* ==========================================================================*/
/*
 * OMX_TIIMAGE_EFFECTTYPE structure contains the list of possible effects 
 * available for image processing. This is passed as a configuration parameter 
 * in the OMX_TIIMAGE_EFFECT_PARAMTYPE structure. 
 *
 * @param OMX_TIIMAGE_SEPIAEFFECT	Sepia toning of the image
 *
 * @param OMX_TIIMAGE_GRAYEFFECT	Gray toning of the image
 */
/* ==========================================================================*/
typedef enum OMX_TIIMAGE_EFFECTTYPE   {
    
    /* effect types*/
	OMX_TIIMAGE_EFFECTUNUSED = 0,		
	OMX_TIIMAGE_SEPIAEFFECT,			/**< Sepia toning of the image*/
	OMX_TIIMAGE_GRAYEFFECT,				/**<Gray toning of the image*/
	OMX_TIIMAGE_EFFECTMAX =	0x7FFFFFFF
    
} OMX_TIIMAGE_EFFECTTYPE;

/* ==========================================================================*/
/*
 * OMX_TIIMAGE_OVERLAYTYPE structure contains the list of possible overlay 
 * styles available for image processing. This is passed as a configuration  
 * parameter in the OMX_TIIMAGE_OVERLAY_PARAMTYPE structure. 
 *
 * @param OMX_TIIMAGE_OVERLAY		Colour key replacement	
 *
 * @param OMX_TIIMAGE_ALPHABLEND	Blending of images based on alpha constant
 */
/* ==========================================================================*/
typedef enum OMX_TIIMAGE_OVERLAYTYPE   {
    
    /* overlay types*/
	OMX_TIIMAGE_OVERLAYUNUSED = 0,		
	OMX_TIIMAGE_OVERLAY,				
	OMX_TIIMAGE_ALPHABLEND,				
	OMX_TIIMAGE_ALPHAOVERLAY,			
	OMX_TIIMAGE_OVERLAYMAX =	0x7FFFFFFF	
    
} OMX_TIIMAGE_OVERLAYTYPE;

/* ==========================================================================*/
/*
 * OMX_TIIMAGE_WINDOWTYPE structure contains the structure fields to specify a crop window.
 * This is passed along with the decode parameters to decode a portion of an image. 
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_WINDOWTYPE {

	OMX_U16		nXOffset; /** <Offset from the left  */
	OMX_U16		nYOffset; /** <Offset from the top  */
	OMX_U16		nWidth; /** <Width of the window  */
	OMX_U16		nHeight; /** <Height of the window  */
	
}OMX_TIIMAGE_WINDOWTYPE;

typedef struct OMX_TIIMAGE_YUVFRAMETYPE{
	OMX_U8	 *lum;
	OMX_U8	 *cb;
	OMX_U8   *cr;
}OMX_TIIMAGE_YUVFRAMETYPE;

typedef enum OMX_TIIMAGE_STREAMINGTYPE{
	OMX_TIIMAGE_STREAMINGNONE=0,
	OMX_TIIMAGE_STREAMINGOUTPUT,
	OMX_TIIMAGE_STREAMINGINPUT,
	OMX_TIIMAGE_STREAMINGBOTH
}OMX_TIIMAGE_STREAMINGTYPE;
/* ==========================================================================*/
/*
 * OMX_TIIMAGE_ENCODE_PARAMTYPE structure contains the list of encoding 
 * parameters. This is passed along with the OMX_TIIMAGE_ENCODE_PARAMINDEX
 * enum type in a setParameter call. 
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_ENCODE_PARAMTYPE   {
    
    /*encode paramters*/

	OMX_IMAGE_CODINGTYPE	tImageCodingType ;	/**< The only supported format in 
												 *   this version is 
												 *   OMX_IMAGE_CodingJPEG */
	OMX_U32					nQualityFactor ;	/**< value from 1-100
												 *   1 -> best compression, worst quality
												 *   100 -> worst compression, best quality 
												 */
	OMX_U16					nImageWidth ;		/**< Width of image to be encoded */
	OMX_U16					nImageHeight ;		/**< Width of image to be encoded */
	OMX_U16					nStride ;			/**< No. of bytes per span of image */
	OMX_TICOLOR_FORMATTYPE	tInputImageFormat ;	/**< Input image format 
												 *   Can only be among YUV420Planar, 
												 *   YUV422Planar and YUYV  */
	OMX_TICOLOR_FORMATTYPE	tImageEncodeFormat ;/**< Encoding Format */
	OMX_U16					nEncodeAUSize ;		/**< Access Unit size. 
												 *   If 0, one shot encoding is performed
												 *   else the number of access units
												 *   specified is encoded */
	OMX_BOOL			bInsertHeader ;		/**< If OMX_TRUE, header is inserted to 
											 *   generated bitstream, else not. */
	OMX_TIIMAGE_YUVFRAMETYPE	tYUVFrameBuf;			
	OMX_TIIMAGE_STREAMINGTYPE	tStreamingMode;	
    
} OMX_TIIMAGE_ENCODE_PARAMTYPE;


/* ==========================================================================*/
/*
 * OMX_TIIMAGE_DECODE_PARAMTYPE structure contains the list of decoding 
 * parameters. This is passed along with the OMX_TIIMAGE_DECODE_PARAMINDEX
 * enum type in a setParameter call. 
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_DECODE_PARAMTYPE   {
    
    /*decode paramters*/

	OMX_U16					nStride ;			/**< Width of the buffer to be 
												 *   supplied */
	OMX_U16					nImageScalingFactor;/**< Scaling factor for output*/
	OMX_U16					nDecodeAUSize ;		/**< Access Unit size. 
												 *   If 0, one shot decoding is performed
												 *   else the number of access units
												 *   specified is decoded */
	OMX_TIIMAGE_WINDOWTYPE	tCropWindow;		/**< Crop Window for portion of image
												 *	to be decoded */
	OMX_TICOLOR_FORMATTYPE tImageOutPutFormat ;/**< Format of decoded data */
    
} OMX_TIIMAGE_DECODE_PARAMTYPE;


/* ==========================================================================*/
/*
 * OMX_TIIMAGE_ROTATE_PARAMTYPE structure contains the list of rotation 
 * parameters. This is passed along with the OMX_TIIMAGE_ROTATE_PARAMINDEX
 * enum type in a setParameter call. 
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_ROTATE_PARAMTYPE   {
    
    /*rotation paramters*/

	OMX_U16					nImageWidth ;		/**< Width of input image */
	OMX_U16					nImageHeight ;		/**< Height of input image */
	OMX_TICOLOR_FORMATTYPE tInputImageFormat ;	/**< Format of input image 
												 *   Can be YUYV or RGB565 */
	OMX_TIIMAGE_ROTATETYPE tRotate ;			/**< Rotation factor */
	OMX_U8					nRotateManual ;		/** <User Specified degree of rotation if tRotate is RotateManual */
	OMX_U16					nStride ;			/**< No. of bytes per span of image */
	OMX_BOOL				bInPlace ;			/**< If OMX_TRUE, separate output 
												 *   buffer not needed. */
    
} OMX_TIIMAGE_ROTATE_PARAMTYPE;

/* ==========================================================================*/
/*
 * OMX_TIIMAGE_RESCALE_PARAMTYPE structure contains the list of rotation 
 * parameters. This is passed along with the OMX_TIIMAGE_RESCALE_PARAMINDEX
 * enum type in a setParameter call. 
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_RESCALE_PARAMTYPE   {
    
    /* rescale and zoom paramters */

	OMX_U16					nInputImageWidth ;	/**< Width of input image */
	OMX_U16					nInputImageHeight ;	/**< Height of input image */
	OMX_TICOLOR_FORMATTYPE tInputImageFormat ;	/**< Format of input image 
												 * Can be YUYV or RGB565 */
	OMX_U16					nOutputImageWidth ;	/**< Width of input image */
	OMX_U16					nOutputImageHeight ;/**< Height of input image */
	OMX_TIIMAGE_WINDOWTYPE tCropWindow;		/**< Window for the portion of the image to be rescaled */
	OMX_U32					nStride; 			/**< Buffer width */
	OMX_U16					nZoomFactor ;		/**< Zoom factor */
	OMX_BOOL				bAutoZoom ;			/**< If OMX_TRUE, Auto Zoom Mode active */
	OMX_U16					nStartZoomFactor ;	/**< Start zoom factor in auto zoom mode */
	OMX_U16					nFinalZoomFactor ;	/**< Final zoom factor in auto zoom mode */
	OMX_U16					nIncrement ;		/**< Zoom Factor increment value */
	OMX_BOOL				bZoomReset ;		/**< If OMX_TRUE, zoom factor is reset to 
												 *   starting value when it reaches final 
												 *   value in auto zoom mode */
	OMX_BOOL				bInPlace ;			/**< If OMX_TRUE, separate output 
												 *   buffer not needed. */
    
} OMX_TIIMAGE_RESCALE_PARAMTYPE;


/* ==========================================================================*/
/*
 * OMX_TIIMAGE_EFFECT_PARAMTYPE structure contains the list of effects 
 * available. This is passed along with the OMX_TIIMAGE_EFFECT_PARAMINDEX
 * enum type in a setParameter call. 
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_EFFECT_PARAMTYPE   {
    
    /*effects paramters*/

	OMX_U16					nImageWidth ;		/**< Width of input image */
	OMX_U16					nImageHeight ;		/**< Height of input image */
	OMX_TICOLOR_FORMATTYPE	tInputImageFormat ;	/**< Format of input image 
												 *   Can be YUYV or RGB565 */
	OMX_TIIMAGE_EFFECTTYPE 	tEffect ;			/**< Rotation factor */
	OMX_U16					nStride ;			/**< No. of bytes per span of image */
	OMX_BOOL			bInPlace ;			/**< If OMX_TRUE, separate output 
												 *   buffer not needed. */
    
} OMX_TIIMAGE_EFFECT_PARAMTYPE;


/* ==========================================================================*/
/*
 * OMX_TIIMAGE_OVERLAY_PARAMTYPE structure contains the list of effects 
 * available. This is passed along with the OMX_TIIMAGE_OVERLAY_PARAMINDEX
 * enum type in a setParameter call. 
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_OVERLAY_PARAMTYPE   {
    
    /*overlay paramters*/

	OMX_U16					nSrcImageWidth ;		/**< Width of input image */
	OMX_U16					nSrcImageHeight ;		/**< Height of input image */
	OMX_TICOLOR_FORMATTYPE	tSrcImageFormat ;		/**< Format of input image */
	OMX_U16					nOverlayImageWidth ;	/**< Width of Overlay image */
	OMX_U16					nOverlayImageHeight;	/**< Height of Overlay image */
	OMX_TICOLOR_FORMATTYPE tOverlayImageFormat ;	/**< Format of Overlay image */
	OMX_U16					nSrcXOffset ;			/**< X Offset */
	OMX_U16					nSrcYOffset ;			/**< Y Offset */
	OMX_TIIMAGE_OVERLAYTYPE	tOverlay ;				/**< Overlay or Alpha Blending */
	OMX_TIIMAGE_ALPHABLENDTYPE	tTransmissivity;	/**< Transmissivity Value for alpha blending */
	OMX_U16					nAlpha ;				/** <Alpha Value if the type is manual in tTransmissivity */
	OMX_U16					nOverlay;	/**< Index value for transparency */
	OMX_U16					nStride ;			/**< No. of bytes per span of image */
	OMX_U8 *				pOverlayBuffer;		/** <Pointer to the overlay buffer */ 
	OMX_BOOL				bInPlace ;			/**< If OMX_TRUE, separate output 
												  *   buffer not needed. */    
}OMX_TIIMAGE_OVERLAY_PARAMTYPE ;

/* ==========================================================================*/
/*
 * OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE structure contains the list of effects 
 * available. This is passed along with the OMX_TIIMAGE_COLORCONVERSION_PARAMINDEX
 * enum type in a setParameter call. 
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE   {
    
    /*color conversion paramters*/

	OMX_U16					nSrcImageWidth ;		/**< Width  of input image */
	OMX_U16					nSrcImageHeight ;		/**< Height of input image */
	OMX_U16					nSrcXOffset ;			/**< X Offset */
	OMX_U16					nSrcYOffset ;			/**< Y Offset */
	OMX_TICOLOR_FORMATTYPE	tInputImageFormat ;		/**< Format of input image */
	OMX_TICOLOR_FORMATTYPE	tOutputImageFormat ;	/**< Format of output image */
	OMX_U16					nStride ;			/**< No. of bytes per span of image */
	OMX_BOOL			bInPlace ;				/**< If OMX_TRUE, separate output 
													 *   buffer not needed. */    
} OMX_TIIMAGE_COLORCONVERSION_PARAMTYPE;


/* ==========================================================================*/
/*
 * OMX_TIIMAGE_DECODE_IMAGEINFOTYPE  structure contains the parameters returned 
 * on the creation of the decoder instance
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_DECODE_IMAGEINFOTYPE    {
    
    /*decoder output image parameters*/

	OMX_U16					nWidth ;		/**< Extended Width  of output image */
	OMX_U16					nHeight ;		/**< Extended Height of output image */
	OMX_U16					nActWidth ;		/**< Actual width of output image */
	OMX_U16					nActHeight ;	/**< Actual height of output image */
	OMX_TICOLOR_FORMATTYPE	tImageFormat ;	/**< Format of output image */
      OMX_U8                            nPlanarFormat;
      OMX_U8                            nProgressiveFormat;
} OMX_TIIMAGE_DECODE_IMAGEINFOTYPE;


/* ==========================================================================*/
/*
 * OMX_TIIMAGE_ENCODE_IMAGEINFOTYPE  structure contains the parameters returned 
 * when the encoder is queried for the output buffer size.
 */
/* ==========================================================================*/
typedef struct OMX_TIIMAGE_ENCODE_IMAGEINFOTYPE    {
    
    /*encoder output image parameters*/

	OMX_U32					nOutputSize ;	/**< Size of encoded image */
    	OMX_BOOL				bEncComplete;  /** status of encoder **/
	
} OMX_TIIMAGE_ENCODE_IMAGEINFOTYPE;

#define OMX_IMG_ENCODE_COMPLETE 0x100

#define OMX_IMG_DECODE_COMPLETE 0x200

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
/* File EOF */
