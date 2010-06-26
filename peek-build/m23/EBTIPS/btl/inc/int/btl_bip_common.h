/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:    btl_bip_common.h
*
*   DESCRIPTION:  This file defines the API of the BTL Basic Image Profile for
*                 both Initiator and Responder role.
*
*   AUTHOR:       Arnoud van Riessen
*
\*******************************************************************************/

#ifndef __BTL_BIP_COMMON_H
#define __BTL_BIP_COMMON_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "xatypes.h" /* for types such as U8, U16, S8, S16,... */
#include "btl_common.h"
#include "btl_unicode.h"
#include "bip.h"
#include "bthal_fs.h"

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/
#define BTL_BIP_MAX_SIZE_CHARSET        40
#define BTL_BIP_MAX_SIZE_CONTENT_TYPE   40
#define BTL_BIP_MAX_SIZE_FILE_NAME      40

#define BTL_BIP_IMAGE_HANDLE_LEN			7

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * BtlBipInitState type
 *
 *     Defines the initialization state of this whole module.
 *     whether it (Responder or Initiator) is initialized or not
 */
typedef enum _BtlBipInitState
{
    BTL_BIP_INIT_STATE_NOT_INITIALIZED,
    BTL_BIP_INIT_STATE_INITIALIZED,
    BTL_BIP_INIT_STATE_INITIALIZATION_FAILED
} BtlBipInitState;

/*-------------------------------------------------------------------------------
 * BtlBipContextState type
 *
 *     Defines the state of a specific context.
 */
typedef enum _BtlBipContextState
{
    BTL_BIP_CONTEXT_STATE_IDLE,       /* Created, waiting for Enable        */
    BTL_BIP_CONTEXT_STATE_IN_USE,     /* In use as a Initiator or Responder */
    BTL_BIP_CONTEXT_STATE_DISABLING   /* In process of Disabling.           */
} BtlBipContextState;

/*-------------------------------------------------------------------------------
 * BtlBipChannelState type
 *
 *     Defines the state of a specific BIP channel.
 */
typedef enum _BtlBipChannelState
{
    BTL_BIP_CHANNEL_STATE_DISCONNECTED, /* channel is free. */
    BTL_BIP_CHANNEL_STATE_DISCONNECTING,
    BTL_BIP_CHANNEL_STATE_CONNECTING,
    BTL_BIP_CHANNEL_STATE_CONNECTED
} BtlBipChannelState;


typedef enum _BtlBipEncoding
{
    BTL_BIP_ENCODING_JPEG,
    BTL_BIP_ENCODING_JPEG2000,
    BTL_BIP_ENCODING_PNG,
    BTL_BIP_ENCODING_GIF,
    BTL_BIP_ENCODING_BMP,
    BTL_BIP_ENCODING_WBMP,
    BTL_BIP_ENCODING_ALL        /* Indication for a don't care (all encoding allowed */
} BtlBipEncoding;

typedef enum _BtlBipDateTimeFormat
{
    BTL_BIP_DATETIME_FORMAT_NONE,
    BTL_BIP_DATETIME_FORMAT_NUMERIC,
    BTL_BIP_DATETIME_FORMAT_STRING
} BtlBipDateTimeFormat;

typedef enum _BtlBipTranformation
{
    BTL_BIP_TRANSFORMATION_NONE,
    BTL_BIP_TRANSFORMATION_STRETCH,
    BTL_BIP_TRANSFORMATION_FILL,
    BTL_BIP_TRANSFORMATION_CROP
} BtlBipTranformation;

/*-------------------------------------------------------------------------------
 * BtlBipObjStoreType type
 *
 *     Defines the object storage type (memory or file)
 */
typedef enum _BtlBipObjStoreType
{
    BTL_BIP_OBJ_STORE_TYPEHANDLE, /* object store is in teh file-system.    */
    BTL_BIP_OBJ_STORE_TYPEMEMPTR, /* object is stored in (volatile) memory  */
    BTL_BIP_OBJ_STORE_TYPENONE    /* object storage type is not defined yet */
} BtlBipObjStoreType;


/*-------------------------------------------------------------------------------
 * BtlBipObjStore structure
 *
 *     Represents the file info.
 */
typedef struct _BtlBipObjStore
{
    BtlBipObjStoreType type; /* tag-field for next union.*/
    union
    {
        /* type = BTL_BIP_OBJ_STORE_TYPEHANDLE */
        BthalFsFileDesc fileHandle; /* File handle for the object.         */
        /* type = BTL_BIP_OBJ_STORE_TYPEMEMPTR */
        void            *mem;   /* Memory pointer where the object is stored.*/
/*gesl: QUESTION: why a void* instead of U8*?*/
    } location;
} BtlBipObjStore;
/*gesl: QUESTION: why not use the same fields from BtlObject?*/

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/* 
 * Max number of bytes (characters, including 0-termination) in the 
 * string in BtlBipDateAndTime.                                 
 */
#define	BTL_BIP_IMG_HANDLE_STRING_LEN	 ((U8) 7)
#define	BTL_BIPINT_MAX_SIZE_ATTACHMENT_NAME	 ((U8) 40)

/*-------------------------------------------------------------------------------
 * BtlBipPixel structure
 *
 *     Represents BTL BIP Pixel width and height of an image.
 *     The number of pixels can be expressed in the range of 0..65535
 *     It can represent a fixed size, a range or a range with fixed aspect ratio:
 *     - fixed: (W1 == W2) && (H1 == H2)
 *     - range with fixed aspect ratio: (NOT 'fixed')  && (H1 == 0)
 *       (H1 = heightSmall)
 *     - range: other values.
 */
typedef struct _BtlBipPixel
{
    U16 widthSmall;  /* W1: lower limit of the pixel width.           */
    U16 heightSmall; /* H1: lower limit of the pixel height.          */
                      /*     0 = Range with fixed aspect ratio.        */
                      /*     The 'heightSmall' can be calculated from  */
                      /*     the other 3 parameters: H2 = (W1*H2)/W2   */
    U16 widthLarge;  /* W2: higher limit of the pixel width.          */
    U16 heightLarge; /* H2: higher limit of the pixel height.         */
} BtlBipPixel;


/*-------------------------------------------------------------------------------
 * BtlBipFilter structure
 *
 *   Filter for retrieving an ImageList from the remote Responder
 *   (see BTL_BIPINT_GetImagesList)
 *   When a specific filter element should be ignored (don't care) then
 *   fill it with NULL if it is a pointer.
 */
typedef struct _BtlBipFilter
{
    /* 'created' Date range. */
    BOOL                       filledCreatedStartDate; /* TRUE = next field is filled with data. */
    BtlDateAndTimeStruct    createdStartDate;        /* Not filled = don't care */
    BOOL                       filledCreatedEndDate;    /* TRUE = next field is filled with data. */
    BtlDateAndTimeStruct    createdEndDate;          /* Not filled = don't care */

    /* 'modified' Date range. */
    BOOL                       filledModifiedStartDate;   /* TRUE = next field is filled with data. */
    BtlDateAndTimeStruct    modifiedStartDate;       /* Not filled = don't care */
    BOOL                       filledModifiedEndDate;   /* TRUE = next field is filled with data. */
    BtlDateAndTimeStruct    modifiedEndDate;          /* Not filled = don't care */

    BtlBipEncoding             encoding; /* Only 1 encoding type can be filtered. */
                                          /*  BTL_BIP_ENCODING_ALL = no filter on  */
                                          /*  encoding type.                       */
    BOOL                       filledPixelRange; /* TRUE = next field is filled with data. */
    BtlBipPixel                pixelRange; /* Image size filtering range.         */
                                            /* Not filled = don't care             */
} BtlBipFilter;
#if 0
gesl: OLD below
typedef struct _BtlBipFilter
{
    /* 'created' Date range. */
    BtlDateAndTimeStruct   *createdStartDate;  /* NULL = don't care */
    BtlDateAndTimeStruct   *createdEndDate;    /* NULL = don't care */

    /* 'modified' Date range. */
    BtlDateAndTimeStruct   *modifiedStartDate; /* NULL = don't care */
    BtlDateAndTimeStruct   *modifiedEndDate;   /* NULL = don't care */

    BtlBipEncoding       encoding; /* Only 1 encoding type can be filtered. */
                                    /*  BTL_BIP_ENCODING_ALL = no filter on  */
                                    /*  encoding type.                       */
    BtlBipPixel        *pixelRange; /* Image size filtering range.         */
                                      /* NULL = don't care                   */
} BtlBipFilter;
#endif

/*-------------------------------------------------------------------------------
 * ImageProperties structure
 -------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------
 * BtlBipNativeEncoding structure
 *
 *     Represents BTL BIP native encoding for a ResponseImageProperties.
 */
typedef struct _BtlBipNativeEncoding
{
    BtlBipEncoding  encoding;
    BtlBipPixel     pixel;
    U32             size;
} BtlBipNativeEncoding;

/*-------------------------------------------------------------------------------
 * BtlBipVariantEncoding structure
 *
 *     Represents BTL BIP variant encoding for a ResponseImageProperties.
 */
typedef struct _BtlBipVariantEncoding
{
    BtlBipEncoding      encoding;
    BtlBipPixel         pixel;
    U32                 maxSize;
    BtlBipTranformation transformation;
} BtlBipVariantEncoding;

/*-------------------------------------------------------------------------------
 * BtlBipAttachment structure
 *
 *     Represents BTL BIP attachment for a ResponseImageProperties.
 */
 
typedef struct _BtlBipAttachment
{
    /* (mandatory) 0-terminated string */
    S8                         contentType[BTL_BIP_MAX_SIZE_CONTENT_TYPE]; 
    /* (optional).0-terminated string; len = 0 meanse that this field is not used.*/
    S8                         charset[BTL_BIP_MAX_SIZE_CHARSET];           
    /* (mandatory) filename as 0-terminated string */
    S8                         name[BTL_BIP_MAX_SIZE_FILE_NAME];            
    U32                        size;     /* Optional */
    BOOL                       filledCreated; /* TRUE = next field is filled with data. */
    BtlDateAndTimeStruct    created;  /* (optional) Not filled = don't care  */
    BOOL                       filledModified; /* TRUE = next field is filled with data. */
    BtlDateAndTimeStruct    modified; /* (optional) Not filled = don't care */
} BtlBipAttachment;

#if 0
/*
//gesl: OLD struct below
//typedef struct _BtlBipAttachment
//{
//    S8                       *contentType;
//    S8                       *charset;
//    S8                        *name;
//    U32                        size;
//    BtlDateAndTimeStruct   *created;
//    BtlDateAndTimeStruct   *modified;
//} BtlBipAttachment;
*/
#endif

/*-------------------------------------------------------------------------------
 * ImageCapabilities structure
 -------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
 * BtlBipPreferredFormat structure
 *
 *     Represents the Preferred Fromat for the ResponseImageCapabilities
 */
typedef struct _BtlBipPreferredFormat
{
    BtlBipEncoding      encoding;
    BOOL                filledPixel; /* TRUE = next field is filled with data. */
    BtlBipPixel         pixel;
    BtlBipTranformation transformation;
    U32                 maxSize;
} BtlBipPreferredFormat;

/*-------------------------------------------------------------------------------
 * BtlBipImageFormats structure
 *
 *     Represents the supported Image Format for the ResponseImageCapabilities
 */
typedef struct _BtlBipImageFormats
{
    BtlBipEncoding      encoding;
    BOOL                filledPixel; /* TRUE = next field is filled with data. */
    BtlBipPixel         pixel;   /* (optional) Not filled = all formats supported. */
    U32                 maxSize; /* 0 == all sizes supported */
} BtlBipImageFormats;

/*gesl: QUESTION: change formats into format? (align with BtlBipPreferredFormat).*/

/*-------------------------------------------------------------------------------
 * BtlBipAttachmentFormats structure
 *
 *     Represents the supported Attachment Format for the ResponseImageCapabilities
 */
typedef struct _BtlBipAttachmentFormats
{
    BOOL  filledContentType; /* TRUE = next field is filled with data. */
    /* (mandatory) 0-terminated string */
    S8    contentType[BTL_BIP_MAX_SIZE_CONTENT_TYPE]; 
    BOOL  filledCharset; /* TRUE = next field is filled with data. */
    /* (optional).0-terminated string; len = 0 meanse that this field is not used.*/
    S8    charset[BTL_BIP_MAX_SIZE_CHARSET];           
} BtlBipAttachmentFormats;

/*-------------------------------------------------------------------------------
 * BtlBipFilteringParameters structure
 *
 *     Represents the supported Filtering Parameters for the ResponseImageCapabilities
 */
typedef struct _BtlBipFilteringParameters
{
    BOOL            created;
    BOOL            modified;
    BOOL            encoding;
    BOOL     pixel;
} BtlBipFilteringParameters;


extern BtStatus btl_bip_FileOpen(BOOL create,
                                 const BtlUtf8 *path,
                                 const BtlUtf8 *fileName,
                                 BthalFsFileDesc *fileHandle,
                                 BipData *bipData);
extern BtStatus btl_bip_FileClose(BthalFsFileDesc *fileHandle);
extern BtStatus btl_bip_FileDelete(BtlUtf8 *fileName);
extern BtStatus btl_bip_DataRead(BipData *bipData);
extern BtStatus btl_bip_DataWrite(BipData *bipData);

extern BOOL btl_bip_IsHandleValid(const BipImgHandle handle);


extern S8* btl_bip_GetBipEncodingString(BtlBipEncoding bipEncoding, S8* string );
extern const char *btl_bip_GetBipOpString(BipOp bipOp);
extern const char *btl_bip_GetBipEventString(BipEvent bipEvent);
extern const char *btl_bip_GetBipDataTypeString(BipDataType dataType);


#endif /* __BTL_BIP_COMMON_H */

