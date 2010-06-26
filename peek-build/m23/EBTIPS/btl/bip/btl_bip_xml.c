/*******************************************************************************\
*                                                                               *
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION            *
*                                                                               *
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE          *
*   UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE       *
*   APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO       *
*   BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT        *
*   OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL         *
*   DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.          *
*                                                                               *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      btl_bip_xml.c
*
*   DESCRIPTION:    This file contains XML parse and build routines for
*                   BIP (Initiator)
*
*   AUTHOR:         Gerrit Slot / Rene Kuiken
*
\*******************************************************************************/

/*******************************************************************************\
*
* Example XML pattern Image properties :
* 
* <image-properties version="1.0" handle="10000010">
*     <native encoding="JPEG" pixel="1280*1024" size="1048576"/>
*     <variant encoding="JPEG" pixel="640*480" />
*     <variant encoding="JPEG" pixel="160*120" />
*     <variant encoding="GIF" pixel="80*60-640*480"/>
*     <attachment content-type="text/plain" name="ABCD0001.txt" size="5120"/>
*     <attachment content-type="audio/basic" name="ABCD0001.wav" size="102400"/>
* </image-properties>
* 
* Example XML pattern Image Capabilities :
* 
* <image-capabilities version="1.0">
*     <preferred-format encoding="JPEG" pixel="1280*960" />
*     <image-formats encoding="JPEG" pixel="160*120" maxsize="5000" />
*     <image-formats encoding="JPEG" pixel="320*240" />
*     <image-formats encoding="JPEG" pixel="640*480" />
*     <image-formats encoding="JPEG" pixel="1280*960" />
*     <attachment-formats content-type="audio/basic" />
*     <filtering-parameters created="1" modified="1" />
* </image-capabilities>
*   
* Example XML pattern Image Listing :
* 
* <image-listing version="1.0">
* 	<image handle="1000001" created="20000801T060000Z"/>
* 	<image handle="1000003" created="20000801T060115Z" modified="20000808T071500Z"/>
* 	<image handle="1000004" created="20000801T060137Z"/>
* 	</image-listing>
*
\*******************************************************************************/


#include "btl_config.h"
#if BTL_CONFIG_BIP ==   BTL_CONFIG_ENABLED

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "osapi.h"
#include "bttypes.h"
#include "bthal_fs.h"
#include "btl_utils.h"
#include "btl_debug.h"

#include "btl_obex_utils.h"
#include "btl_bip_common.h"
#include "btl_bip_xml.h"
#include "btl_bipint.h"
#include "bip.h"
#include "btl_obex_utils.h"

/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/
/* XML keys strings for the 'Image Handles Descriptor' (used in btl_bip_XmlBuildDescrImageHandles) */
#define XML_DESCR_IMAGE_HANDLES_START           "<image-handles-descriptor version=\"1.0\">\n"
#define XML_DESCR_IMAGE_HANDLES_FILTER          "<filtering-parameters "
#define XML_DESCR_IMAGE_HANDLES_FILTER_CREATED  "created=\""
#define XML_DESCR_IMAGE_HANDLES_FILTER_MODIFIED "modified=\""
#define XML_DESCR_IMAGE_HANDLES_FILTER_ENCODING "encoding=\""
#define XML_DESCR_IMAGE_HANDLES_FILTER_PIXEL    "pixel=\""
#define XML_DESCR_IMAGE_HANDLES_FILTER_END      "\"/>\n"
#define XML_DESCR_IMAGE_HANDLES_END             "</image-handles-descriptor>"

/* XML keys strings for the 'Image Descriptor' (used in btl_bip_XmlBuildDescrImage) */
#define XML_DESCR_IMAGE_START                   "<image-descriptor version=\"1.0\">\n"
#define XML_DESCR_IMAGE_ENCODING                "<image encoding=\""
#define XML_DESCR_IMAGE_PIXEL                   "pixel=\""
#define XML_DESCR_IMAGE_SIZE                    "size=\""
#define XML_DESCR_IMAGE_MAX_SIZE                "maxsize=\""
#define XML_DESCR_IMAGE_TRANSFORMATION          "transformation=\""
#define XML_DESCR_IMAGE_END                     "/>\n</image-descriptor>\n"

/* XML strings for the 'transformation' attribute (used in XmlBuildAttrTransformation) */
#define XML_ATTR_TRANSFORMATION_STRETCH_STR     "stretch"
#define XML_ATTR_TRANSFORMATION_CROP_STR        "crop"
#define XML_ATTR_TRANSFORMATION_FILL_STR        "fill"

/* XML strings for the 'encoding' attribute (used in XmlBuildAttrTransformation) */
#define XML_ATTR_ENCODING_JPEG_STR              "JPEG"
#define XML_ATTR_ENCODING_JPEG2000_STR          "JPEG2000"
#define XML_ATTR_ENCODING_PNG_STR               "PNG"
#define XML_ATTR_ENCODING_GIF_STR               "GIF"
#define XML_ATTR_ENCODING_BMP_STR               "BMP"
#define XML_ATTR_ENCODING_WBMP_STR              "WBMP"

/* XML strings for parsing image lists */
#define XML_IMAGE_LIST_HANDLE_TEXT              "image handle=\""
#define XML_IMAGE_LIST_CREATED_TEXT             "created=\""
#define XML_IMAGE_LIST_MODIFIED_TEXT            "modified=\""

/* XML strings for parsing image capabilities */
#define XML_CAPABILITIES_PREF_ENC_TEXT          "<preferred-format encoding=\""
#define XML_CAPABILITIES_PIXEL_TEXT             "pixel=\""
#define XML_CAPABILITIES_MAXSIZE_TEXT           "maxsize=\""
#define XML_CAPABILITIES_TRANS_TEXT             "transformation=\""
#define XML_CAPABILITIES_IMG_ENC_TEXT           "<image-formats encoding=\""
#define XML_CAPABILITIES_SIZE_TEXT              "size=\""
#define XML_CAPABILITIES_FLTR_TEXT              "filtering-parameters created=\""
#define XML_CAPABILITIES_FLTR_CR_TEXT           "created=\""
#define XML_CAPABILITIES_FLTR_MO_TEXT           "modified=\"" 
#define XML_CAPABILITIES_FLTR_ENC_TEXT          "encoding=\""
#define XML_CAPABILITIES_FLTR_PIXEL_TEXT        "pixel=\""
#define XML_CAPABILITIES_ATTCH_CONTYPE_TEXT     "content-type=\""
#define XML_CAPABILITIES_ATTCH_CHARSET_TEXT     "charset=\""
#define XML_CAPABILITIES_ATTCH_FORMAT_TEXT      "attachment-formats content-type=\""

/* XML strings for parsing image properties */
#define XML_PROPERTIES_HANDLE_TEXT              "handle=\""
#define XML_PROPERTIES_FRIENDLYNAME_TEXT        "friendly-name=\""
#define XML_PROPERTIES_NENC_ENC_TEXT            "native encoding=\""
#define XML_PROPERTIES_NENC_PIXEL_TEXT          "pixel=\""
#define XML_PROPERTIES_NENC_SIZE_TEXT           "size=\""
#define XML_PROPERTIES_VENC_ENC_TEXT            "variant encoding=\""
#define XML_PROPERTIES_VENC_PIXEL_TEXT          "pixel=\""
#define XML_PROPERTIES_VENC_SIZE_TEXT           "size=\""
#define XML_PROPERTIES_VENC_TRANS_TEXT          "transformation=\""
#define XML_PROPERTIES_ATTCH_CONTYPE_TEXT       "attachment content-type=\""
#define XML_PROPERTIES_ATTCH_CHARSET_TEXT       "charset\""
#define XML_PROPERTIES_ATTCH_NAME_TEXT          "name=\""
#define XML_PROPERTIES_ATTCH_SIZE_TEXT          "size=\""

#define U8_DEC_LENGTH       3
#define U16_DEC_LENGTH      5
#define U32_DEC_LENGTH      10
#define XML_STR_DUMMY_SIZE  20

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/


/********************************************************************************
 *
 * Local function prototypes
 *
 *******************************************************************************/
/* XML Build Attribute routines, */
static void XmlBuildAttrTransformation(S8 *xmlDescr, BtlBipTranformation transformation);

/* XML Build Attribute support routines */
static char bip_ntoc( U8 Nr );
static char *bip_itoa( U32 Nr, S8 *AddrString,BOOL bZeroFill, U8 nrOfChar);

/*-------------------------------------------------------------------------------
 * bip_GetBipEncodingString()
 *
 *      get image encoding string.
 * Parameters:
 *      bipEncoding [in] - input defined encoding.
 *      string [out]     - encoded string.
 * Returns:
 *      Return encoded string
 */
static S8* bip_GetBipEncodingString(BtlBipEncoding bipEncoding, S8* string );

/*-------------------------------------------------------------------------------
 * GetBipEncodingFromString()
 *
 *      get encoding from string.
 * Parameters:
 *      str [in]     - encoding string.
 * Returns:
 *      Return encoding
 */
static BtlBipEncoding GetBipEncodingFromString(S8* str);

/*-------------------------------------------------------------------------------
 * AssignPixelSizes()
 *
 *      get pixel format.
 * Parameters:
 *      str [in]     - pixel string.
 * Returns:
 *      Return Pixelformat
 */
static BtlBipPixel AssignPixelSizes(S8* str);

/*-------------------------------------------------------------------------------
 * StrFindFirstChar()
 *
 *    Find and return a pointer to the first occurance of the 'findChar'
 *    the 0-terminated 'str'.
 *
 * Parameters:
 *      str  [in]     -  string.
 *		findChar [in]     -  char to find.
 * Returns:
 *      Pointer to the first occurance of 'findChar' when it was present or
 *      pointer to the 0-termination character.
 */
static S8 *StrFindFirstChar(const S8 *str, S8 findChar);

/*-------------------------------------------------------------------------------
 * bip_StrNICmp()
 *
 *      Compare strings that are uppercased before comparing.
 * Parameters:
 *      Str2 [in] - string 1.
 *      Str2 [in] - string 2.
 *      n [in]    - nr of bytes to compare
 * Returns:
 *      Return 0 if equal
 */
static U32 bip_StrNICmp(const char *Str1, const char *Str2, U32 n);
/*-------------------------------------------------------------------------------
 * GetXmlKeyElementValue()
 *
 *      Find an XML key in the object given and return the value.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the Xml Object.
 *      field [in]      - Key to be found.
 *      attribute [out] - returned attribute value
 *      attrLen [in]    - length of attribute value
 *      maxLen [in]     - Max length of the Xml object to search in
 * Returns:
 *      Return the number of keys found
 */
static S8* GetXmlKeyElementValue(S8* descriptor, S8* field, S8* attribute, U16 attrLen, U16 maxLen);

/*-------------------------------------------------------------------------------
 * FindXmlKeyElement()
 *
 *      Find an XML key in the object given.
 * Parameters:
 *      pXmlObject [in]        - Pointer to a buffer that contains the Xml Object.
 *      pKey [in]              - Key to be found.
 *      maxLenXmlObject [in]   - Max length of the Xml object to search in
 *      uiNrOfKeysFound [out]  - number of Keys found
 * Returns:
 *      Return the number of keys found
 */
static BtStatus FindXmlKeyElement(S8* pXmlObject, S8* pKey, U16 maxLenXmlObject, U8 *uiNrOfKeysFound);

/*-------------------------------------------------------------------------------
 * FindXmlObjectLocation()
 *
 *      Find the location on which the XML object is stored.
 * Parameters:
 *      dataToParse [in]  - Pointer to a buffer that contains the Xml Object.
 *      ulObjectLen [out] - size of the object on the location.
 * Returns:
 *      Return the location and the size of the object
 */
static BtlUtf8* FindXmlObjectLocation(const BtlObject *dataToParse, U32* ulObjectLen);

/*-------------------------------------------------------------------------------
 * XmlParseHandleFromImagesListDescriptor()
 *
 *      Function to extract image handle of an ImagesList descriptor.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the descriptor.
 *      imgHandle [out] - Image handle.
 *      created [out]   - structure that will contain the created date/time
 *      modified [out]  - structure that will contain the modified date/time
 * Returns:
 *      Pointer to the descriptor. Pass this pointer as descriptor on subsequent calls
 *      to this function to get next element field.
 */
S8* XmlParseHandleFromImagesListDescriptor(S8                     *descriptor,
                                                      BipImgHandle              imgHandle,
                                                      BtlDateAndTimeStruct  *created,
                                                      BtlDateAndTimeStruct  *modified);

/*-------------------------------------------------------------------------------
 * XmlParsePreferredFormatFromCapabilitiesDescriptor()
 *
 *      Function to extract the preferred-format element field from an ImageCapabilities descriptor.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the descriptor.
 *      format [out]    - Pointer to a struct that will hold the preferred-format information.
 * Returns:
 *      BT_STATUS_SUCCESS   - Operation is successful.
 *      BT_STATUS_NOT_FOUND - Operation is not successful, element could not be found.
 */
BtStatus XmlParsePreferredFormatFromCapabilitiesDescriptor(S8                 *descriptor,
                                                                      BtlBipPreferredFormat *format);

/*-------------------------------------------------------------------------------
 * XmlParseImageFormatFromCapabilitiesDescriptor()
 *
 *      Function to extract a image-format element field from a ImageCapabilities descriptor.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the descriptor.
 *      format [out]    - Pointer to a struct that will hold the image-format information.
 * Returns:
 *      Pointer to the descriptor. Pass this pointer as descriptor on subsequent calls
 *      to this function to get next element field.
 */
S8* XmlParseImageFormatFromCapabilitiesDescriptor(S8              *descriptor,
                                                             BtlBipImageFormats *format);

/*-------------------------------------------------------------------------------
 * XmlParseAttachmentTypeFromCapabilitesDescriptor()
 *
 *      Function to extract an attachment-format element field from a ImageCapabilities descriptor.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the descriptor.
 *      format [out]    - Pointer to a struct that will hold the attachment-format information.
 * Returns:
 *      Pointer to the descriptor. Pass this pointer as descriptor on subsequent calls
 *      to this function to get next element field.
 */
S8* XmlParseAttachmentTypeFromCapabilitesDescriptor(S8                  *descriptor,
                                                               BtlBipAttachmentFormats *format);

/*-------------------------------------------------------------------------------
 * XmlParseFilteringParamFromCapabilitesDescriptor()
 *
 *      Function to extract the filtering-format element field from a ImageCapabilities descriptor.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the descriptor.
 *      format [out]    - Pointer to a struct that will hold the filtering-format information.
 * Returns:
 *      BT_STATUS_SUCCESS   - Operation is successful.
 *      BT_STATUS_NOT_FOUND - Operation is not successfull, element could not be found.
 */
BtStatus XmlParseFilteringParamFromCapabilitesDescriptor(S8                       *descriptor,
                                                                    BtlBipFilteringParameters *format);

/*-------------------------------------------------------------------------------
 * XmlParseHandleFromImagePropertiesDescriptor()
 *
 *      Function to extract the handle from the ImageProperties descriptor.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the descriptor.
 *      imgHandle [out] - Image handle.
 * Returns:
 *      Pointer to the descriptor. Pass this pointer as descriptor on subsequent calls
 *      to this function to get next element field.
 */
S8* XmlParseHandleFromImagePropertiesDescriptor(S8            *descriptor,
                                                           BipImgHandle      imgHandle);
/*-------------------------------------------------------------------------------
 * XmlParseFriendlyNameFromImagePropertiesDescriptor()
 *
 *      Function to extract the optional friendly name from the ImageProperties descriptor.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the descriptor.
 *      friendlyName [out] - Friendly name.
 * Returns:
 *      Pointer to the descriptor. Pass this pointer as descriptor on subsequent calls
 *      to this function to get next element field.
 */
S8* XmlParseFriendlyNameFromImagePropertiesDescriptor(S8 *descriptor, S8 *friendlyName);

/*-------------------------------------------------------------------------------
 * XmlParseNativeEncodingFromPropertiesDescriptor()
 *
 *      Function to extract the native-encoding element field from a ImageProperties descriptor.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the descriptor.
 *      format [out]    - Pointer to a struct that will hold the encoding format.
 * Returns:
 *      BT_STATUS_SUCCESS - Operation is successful.
 *      BT_STATUS_NOT_FOUND - Operation is not successfull, element could not be found.
 */
BtStatus XmlParseNativeEncodingFromPropertiesDescriptor(S8                *descriptor,
                                                                   BtlBipNativeEncoding *format);

/*-------------------------------------------------------------------------------
 * XmlParseVariantEncodingFromPropertiesDescriptor()
 *
 *      Function to extract the variant-encoding element field from a ImageProperties descriptor.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the descriptor.
 *      format [out]    - Pointer to a struct that will hold the encoding format.
 * Returns:
 *      Pointer to the descriptor. Pass this pointer as descriptor on subsequent calls
 *      to this function to get next element field.
 */
S8* XmlParseVariantEncodingFromPropertiesDescriptor(S8                 *descriptor,
                                                               BtlBipVariantEncoding *format);

/*-------------------------------------------------------------------------------
 * XmlParseAttachmentTypeFromPropertiesDescriptor()
 *
 *      Function to extract the attachment element field from a ImageProperties descriptor.
 * Parameters:
 *      descriptor [in] - Pointer to a buffer that contains the descriptor.
 *      format [out]    - Pointer to a struct that will hold the attachment information.
 * Returns:
 *      Pointer to the descriptor. Pass this pointer as descriptor on subsequent calls
 *      to this function to get next element field.
 */
S8* XmlParseAttachmentTypeFromPropertiesDescriptor(S8           *descriptor,
                                                              BtlBipAttachment *format);

/********************************************************************************
 *
 * Function implementation, (external)
 *
 *******************************************************************************/

/*****************
 *               *
 *    BUILDER    *
 *               *
 ****************/

/*-------------------------------------------------------------------------------
 * btl_bip_XmlBuildDescrImageHandles()
 *
 *     Build the XML 'Image Handles' Descriptor from the available parameters.
 *     It will be added to the 'xmlDescr' string.
 *     See 'Image Handles Descriptor' chapter in the BIP spec.
 */
void btl_bip_XmlBuildDescrImageHandles(S8 *xmlDescr,
                                         BtlBipFilter *filter)
{
    S8 str[25];
/*gesl: QUESTION: why 25?*/

    /* Create start of Descriptor.*/
    OS_StrCpy((char*)xmlDescr, XML_DESCR_IMAGE_HANDLES_START);

    /* Filter present? --> add it.*/
    if (NULL != filter)
    {
        /* Create start of 'filter'. */
        OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_HANDLES_FILTER);

        /* Created date present in filter? --> add it */
        if ((FALSE != filter->filledCreatedStartDate) ||
            (FALSE != filter->filledCreatedEndDate))
        {
            /* Create start of 'created' attribute. */
            OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_HANDLES_FILTER_CREATED);

            /* Add start date. */
            BTL_UTILS_XmlBuildAttrDateAndTime (&filter->createdStartDate, (U8*)str);
            OS_StrCat((char*)xmlDescr, (char*)str);
            /* Insert separator */
            OS_StrCat((char*)xmlDescr, "-");
            /* Add end date. */
            BTL_UTILS_XmlBuildAttrDateAndTime (&filter->createdEndDate, (U8*)str);
            OS_StrCat((char*)xmlDescr, (char*)str);

            /* Close this attribute*/
            OS_StrCat((char*)xmlDescr, "\""); /*gesl: QUESTION: this is necessary?*/
        }

        /* Modified date present in filter? --> add it */
        if ((FALSE != filter->filledModifiedStartDate) ||
            (FALSE != filter->filledModifiedEndDate))
        {
            /* Create start of 'modified' attribute. */
            OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_HANDLES_FILTER_MODIFIED);

            /* Add start date. */
            BTL_UTILS_XmlBuildAttrDateAndTime (&filter->modifiedStartDate, (U8*)str);
            OS_StrCat((char*)xmlDescr, (char*)str);
            /* Insert separator */
            OS_StrCat((char*)xmlDescr, "-");
            /* Add end date. */
            BTL_UTILS_XmlBuildAttrDateAndTime (&filter->modifiedEndDate, (U8*)str);
            OS_StrCat((char*) xmlDescr, (char*)str );

            /* Close this attribute*/
            OS_StrCat((char*)xmlDescr, "\"" );
        }

        /* Encoding present in filter? --> add it */
        if (BTL_BIP_ENCODING_ALL != filter->encoding)
        {
            /* Create start of 'modified' attribute. */
            OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_HANDLES_FILTER_ENCODING);

            /* Add encoding. */
            bip_GetBipEncodingString(filter->encoding,(S8*)str);
            OS_StrCat((char*)xmlDescr, (char*)str);

            /* Close this attribute*/
            OS_StrCat((char*)xmlDescr, "\"" );
        }

        /* Pixel range present in filter? --> add it */
        if (FALSE != filter->filledPixelRange)
        {
            /* Create start of 'pixel' attribute. */
            OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_HANDLES_FILTER_PIXEL);

            /* Add the 'pixel' attribute. */
            btl_bip_XmlBuildAttrPixel(xmlDescr,&filter->pixelRange);

            /* Close this attribute */
            OS_StrCat((char*)xmlDescr, "\"" );
        }

        /* Close this 'filter' */
        OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_HANDLES_FILTER_END );
    }

    /* Create end of Descriptor.*/
    OS_StrCat( (char*)xmlDescr, XML_DESCR_IMAGE_HANDLES_END );
}

/*-------------------------------------------------------------------------------
 * btl_bip_XmlBuildDescrImage()
 *
 *     Build the XML 'Image' Descriptor from the available parameters.
 *     It will be added to the 'xmlDescr' string.
 *     See 'Image Descriptor' chapter in the BIP spec.
 */
void btl_bip_XmlBuildDescrImage(S8 *xmlDescr,
                                           BtlBipEncoding encoding,
                                           BtlBipPixel *pixelRange,
                                           U32 size,
                                           U32 maxSize,
                                           BtlBipTranformation transformation)
{
    S8  str[U32_DEC_LENGTH];

    /* Create start of Descriptor.*/
    OS_StrCpy((char*)xmlDescr, XML_DESCR_IMAGE_START);

    /* Add (mandatory) 'encoding' attribute. */
    OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_ENCODING);
    /* Encoding required? --> fill it. */
    if (encoding != BTL_BIP_ENCODING_ALL)
    {
        bip_GetBipEncodingString(encoding, str);
        OS_StrCat((char*)xmlDescr, (char*)str);
    }
    /* Close this attribute */
    OS_StrCat((char*)xmlDescr, "\" ");
/*gesl: QUESTION: does empty indeed mean to double quotes ""?*/

    /* Add (mandatory) 'pixel' attribute. */
    OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_PIXEL);
    /* 'pixel' requirements present? --> fill it. */
    if (NULL != pixelRange)
    {
        btl_bip_XmlBuildAttrPixel(xmlDescr, pixelRange);
    }
    /* Close this attribute */
    OS_StrCat((char*)xmlDescr, "\" ");

    /* Add (optional) 'size' attribute. */
    if (0 != size)
    {
        OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_SIZE);
        bip_itoa( size, str,FALSE,0);
        OS_StrCat((char*)xmlDescr, (char*)str);
        /* Close this attribute */
        OS_StrCat((char*)xmlDescr, "\" ");
    }

    /* Add (optional) 'maxsize' attribute. */
    if (0 != maxSize)
    {
        OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_MAX_SIZE);
        bip_itoa( maxSize, str,FALSE,0);
        OS_StrCat((char*)xmlDescr, (char*)str);
        /* Close this attribute */
        OS_StrCat((char*)xmlDescr, "\" ");
    }

    /* Add (optional) 'transformation' attribute. */
    if (BTL_BIP_TRANSFORMATION_NONE != transformation)
    {
        /* Start of this attribute. */
        OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_TRANSFORMATION);

        XmlBuildAttrTransformation(xmlDescr, transformation);
        /* Close this attribute */
        OS_StrCat((char*)xmlDescr, "\" ");
    }

    /* Create end of Descriptor.*/
    OS_StrCat((char*)xmlDescr, XML_DESCR_IMAGE_END);
}


/*****************
 *               *
 *    PARSER     *
 *               *
 ****************/

/*-------------------------------------------------------------------------------
 * XmlParseHandleFromImagesListDescriptor()
 */
S8* XmlParseHandleFromImagesListDescriptor(S8                       *descriptor,
                                           BipImgHandle              imgHandle,
                                           BtlDateAndTimeStruct  *created,
                                           BtlDateAndTimeStruct  *modified)
{
    U16 desLen = OS_StrLen((char*)descriptor);
    S8  *dp    = descriptor;
	S8  *tempStr, *tempOffset;
    U16 iLen   = (U16)(OS_StrLen(XML_IMAGE_LIST_HANDLE_TEXT) + OS_StrLen(XML_IMAGE_LIST_CREATED_TEXT) + OS_StrLen(XML_IMAGE_LIST_MODIFIED_TEXT));
	U16 lengthXmlBlock;
    S8  str[XML_STR_DUMMY_SIZE + 1];

    /* Get Image handle and continue if we found the handle to find attributes belonging to this handle */
    dp = GetXmlKeyElementValue(descriptor,(S8*)XML_IMAGE_LIST_HANDLE_TEXT,(S8*)imgHandle,8,(U16)(desLen-iLen));
    if( dp != descriptor )
    {
		/* Continue only until xml block is ended by '/'                     */
		/* We need the length of the block as input to GetXmlKeyElementValue */
		tempStr = StrFindFirstChar(dp,'/');
		lengthXmlBlock = (U16)(tempStr - dp);

        /* Get 'created' attribute and return it parsed according to BtlDateAndTimeStruct */
		tempOffset = GetXmlKeyElementValue(dp,(S8*)XML_IMAGE_LIST_CREATED_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
		if( tempOffset != dp )
		{
			BTL_UTILS_XmlParseAttrDateAndTime((U8*)str, created);
		}

		/* Get 'modified' attribute and return it parsed according to BtlDateAndTimeStruct */
		tempOffset = GetXmlKeyElementValue(	dp,(S8*)XML_IMAGE_LIST_MODIFIED_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
		if( tempOffset != dp )
		{
			BTL_UTILS_XmlParseAttrDateAndTime((U8*)str, modified);
		}
		
		/* we are at the end of this XML block */
		dp = tempStr;
	}

    return dp;
}

/*-------------------------------------------------------------------------------
 * XmlParsePreferredFormatFromCapabilitiesDescriptor()
 */
BtStatus XmlParsePreferredFormatFromCapabilitiesDescriptor(S8 *descriptor, BtlBipPreferredFormat *format)
{
    BtStatus status = BT_STATUS_SUCCESS;
    S8       *dp    = descriptor;
    U16      desLen = OS_StrLen((char*)descriptor);
    S8       str[XML_STR_DUMMY_SIZE + 1];
    U16      iLen;
	S8  *tempStr, *tempOffset;
	U16 lengthXmlBlock;

    iLen = (U16)(OS_StrLen(XML_CAPABILITIES_PREF_ENC_TEXT) + OS_StrLen(XML_CAPABILITIES_PIXEL_TEXT) + OS_StrLen(XML_CAPABILITIES_MAXSIZE_TEXT));

	dp = GetXmlKeyElementValue(descriptor,(S8*)XML_CAPABILITIES_PREF_ENC_TEXT,str,XML_STR_DUMMY_SIZE,(U16)(desLen-iLen));
    if( dp != descriptor )
    {
		/* Continue only until xml block is ended by '/'                     */
		/* We need the length of the block as input to GetXmlKeyElementValue */
		tempStr = StrFindFirstChar(dp,'/');
		lengthXmlBlock = (U16)(tempStr - dp);

		/* assign the encoding */
        format->encoding = GetBipEncodingFromString(str);		

		/* assign the pixel, if any */
		tempOffset = GetXmlKeyElementValue(dp,(S8*)XML_CAPABILITIES_PIXEL_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
		if (tempOffset != dp)
		{
			format->pixel = AssignPixelSizes(str);
		}
        
		/* get the transformation if any */
		tempOffset = GetXmlKeyElementValue(dp,(S8*)XML_CAPABILITIES_TRANS_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
		if( tempOffset != dp )
		{
			if( OS_StrCmp( (char*)str, XML_ATTR_TRANSFORMATION_STRETCH_STR) == 0 )
				format->transformation = BTL_BIP_TRANSFORMATION_STRETCH;
			else if( OS_StrCmp( (char*)str, XML_ATTR_TRANSFORMATION_CROP_STR) == 0 )
				format->transformation = BTL_BIP_TRANSFORMATION_CROP;
			else if( OS_StrCmp( (char*)str, XML_ATTR_TRANSFORMATION_FILL_STR) == 0 )
				format->transformation = BTL_BIP_TRANSFORMATION_FILL;
			else
				format->transformation = BTL_BIP_TRANSFORMATION_NONE;
			dp = tempOffset;
		}
		
		/* assign the maxsize, if any */
		tempOffset = GetXmlKeyElementValue(dp,(S8*)XML_CAPABILITIES_MAXSIZE_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
		if( tempOffset != dp )
		{
			format->maxSize = OS_AtoU32((char*)dp);
		}
		else
		{
			/* if not available, set to 0 */
			format->maxSize = 0;
		}

		/* we are at the end of this XML block */
		dp = tempStr;
	}

    return status;
}

/*-------------------------------------------------------------------------------
 * XmlParseImageFormatFromCapabilitiesDescriptor()
 */
S8* XmlParseImageFormatFromCapabilitiesDescriptor(S8                 *descriptor,
                                                             BtlBipImageFormats *format)
{
    S8  *dp     = descriptor;
    U16 desLen  = OS_StrLen((char*)descriptor);
    S8  *dpTemp;
    S8  str[XML_STR_DUMMY_SIZE + 1];
    U16 iLen;
	S8  *tempStr, *tempOffset;
	U16 lengthXmlBlock;

    iLen = (U16)(OS_StrLen(XML_CAPABILITIES_IMG_ENC_TEXT) + OS_StrLen(XML_CAPABILITIES_PIXEL_TEXT) + OS_StrLen(XML_CAPABILITIES_SIZE_TEXT));

    dp = GetXmlKeyElementValue(descriptor,(S8*)XML_CAPABILITIES_IMG_ENC_TEXT,str,XML_STR_DUMMY_SIZE,(U16)(desLen-iLen));
    if( dp != descriptor )
    {
		/* Continue only until xml block is ended by '/'                     */
		/* We need the length of the block as input to GetXmlKeyElementValue */
		tempStr = StrFindFirstChar(dp,'/');
		lengthXmlBlock = (U16)(tempStr - dp);            

        /* get the required encoding */    
        format->encoding = GetBipEncodingFromString(str);		

        /* get pixel */
		tempOffset = GetXmlKeyElementValue(dp,(S8*)XML_CAPABILITIES_PIXEL_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
		if (tempOffset != dp)
		{
			format->pixel = AssignPixelSizes(str);			
			format->filledPixel = TRUE;
		}

        /* get size */
        dpTemp = GetXmlKeyElementValue(dp,(S8*)XML_CAPABILITIES_SIZE_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
        if( dpTemp != dp )
            format->maxSize = OS_AtoU32((char*)dpTemp);
        else
            format->maxSize = 0;

		/* we are at the end of this XML block */
		dp = tempStr;        
    }  

    return dp;
}

/*-------------------------------------------------------------------------------
 * XmlParseAttachmentTypeFromCapabilitesDescriptor()
 */
S8* XmlParseAttachmentTypeFromCapabilitesDescriptor(S8 *descriptor, BtlBipAttachmentFormats *format)
{
    S8  *dp    = descriptor;
    S8  *dpTemp;
    U16 desLen = OS_StrLen((char*)descriptor);
    U16 iLen   = OS_StrLen(XML_CAPABILITIES_ATTCH_CONTYPE_TEXT);
	S8  *tempStr;
	U16 lengthXmlBlock;

	/* do we have a content type? */
    dp = GetXmlKeyElementValue(descriptor,(S8*)XML_CAPABILITIES_ATTCH_CONTYPE_TEXT,format->contentType,20,(U16)(desLen-iLen));
    if( dp != descriptor )
    {
		/* Continue only until xml block is ended by '/'                     */
		/* We need the length of the block as input to GetXmlKeyElementValue */
		tempStr = StrFindFirstChar(dp,'/');
		lengthXmlBlock = (U16)(tempStr - dp);            

        /* we found the required content type */
        format->filledContentType = TRUE;
        
    	/* do we have a charset ?*/
        dpTemp = GetXmlKeyElementValue(dp,(S8*)XML_CAPABILITIES_ATTCH_CHARSET_TEXT,format->charset,20,lengthXmlBlock);
    	if( dpTemp != dp )
            format->filledCharset = TRUE;
        else
            format->filledCharset = FALSE;	
        
        /* we are at the end of this XML block */
		dp = tempStr; 
    }
    else
    {
       format->filledContentType = FALSE;        
    }

    return dp;
}

/*-------------------------------------------------------------------------------
 * XmlParseAttachmentTypeFromCapabilitesDescriptor()
 */
BtStatus XmlParseFilteringParamFromCapabilitesDescriptor(S8                        *descriptor,
                                                                    BtlBipFilteringParameters *format)
{
    BtStatus status  = BT_STATUS_NOT_FOUND;
    S8       *dp = descriptor;
    S8       *filterParams;
    U16      desLen  = OS_StrLen((char*)descriptor);
    U16      iLen    = OS_StrLen(XML_CAPABILITIES_FLTR_TEXT);
    U32      iValue = 0;
    S8       str[XML_STR_DUMMY_SIZE + 1];
	S8      *tempStr;
	U16      lengthXmlBlock;

    /* Is filtering set as capability ? */
    dp = GetXmlKeyElementValue(descriptor,(S8*)XML_CAPABILITIES_FLTR_TEXT,str,XML_STR_DUMMY_SIZE,(U16)(desLen-iLen));
    filterParams = dp;
    if( filterParams != descriptor )
    {
		/* Continue only until xml block is ended by '/'                     */
		/* We need the length of the block as input to GetXmlKeyElementValue */
		tempStr = StrFindFirstChar(dp,'/');
		lengthXmlBlock = (U16)(tempStr - dp);            

        /* Get 'created' capability */
		iValue = OS_AtoU32((char*)dp);
		format->created = iValue ? TRUE : FALSE;

        /* Get 'modified' capability */
        dp = GetXmlKeyElementValue(filterParams,(S8*)XML_CAPABILITIES_FLTR_MO_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
        if( dp != filterParams )
        {
            iValue = OS_AtoU32((char*)dp);
            format->modified = iValue != 0 ? TRUE : FALSE;
        }

        /* Get 'encoding' capability */
        dp = GetXmlKeyElementValue(filterParams,(S8*)XML_CAPABILITIES_FLTR_ENC_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
        if( dp != filterParams )
        {
            iValue = OS_AtoU32((char*)dp);
            format->encoding = iValue != 0 ? TRUE : FALSE;
        }

        /* Get 'pixel' capability */
        dp = GetXmlKeyElementValue(filterParams,(S8*)XML_CAPABILITIES_FLTR_PIXEL_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
        if( dp != filterParams )
        {
            iValue = OS_AtoU32((char*)dp);
            format->pixel = iValue != 0 ? TRUE : FALSE;
        }

        /* we are at the end of this XML block */
        dp = tempStr;

        status  = BT_STATUS_SUCCESS;        
    }

    return status;
}

/*-------------------------------------------------------------------------------
 * XmlParseHandleFromImagePropertiesDescriptor()
 */
S8* XmlParseHandleFromImagePropertiesDescriptor(S8  *descriptor, BipImgHandle imgHandle)
{
    U16 desLen = OS_StrLen((char*)descriptor);
    S8  *dp    = descriptor;
    U16 iLen   = (U16)(OS_StrLen(XML_PROPERTIES_HANDLE_TEXT));

    dp = GetXmlKeyElementValue(dp,(S8*)XML_PROPERTIES_HANDLE_TEXT,(S8*)imgHandle,8,(U16)(desLen-iLen));

    return dp;
}

/*-------------------------------------------------------------------------------
 * XmlParseFriendlyNameFromImagePropertiesDescriptor()
 */
S8* XmlParseFriendlyNameFromImagePropertiesDescriptor(S8 *descriptor, S8 *friendlyName)
{
    U16 desLen = OS_StrLen((char*)descriptor);
    S8 *dp    = descriptor;
    U16 iLen   = (U16)(OS_StrLen(XML_PROPERTIES_FRIENDLYNAME_TEXT));

    dp = GetXmlKeyElementValue(dp,(S8*)XML_PROPERTIES_FRIENDLYNAME_TEXT,friendlyName,BTL_BIPINT_MAX_SIZE_FRIENDLY_NAME,(U16)(desLen-iLen));

    return dp;
}

/*-------------------------------------------------------------------------------
 * XmlParseNativeEncodingFromPropertiesDescriptor()
 */
BtStatus XmlParseNativeEncodingFromPropertiesDescriptor(S8                   *descriptor,
                                                                   BtlBipNativeEncoding *format)
{
    S8  *dp     = descriptor;
    U16 desLen  = OS_StrLen((char*)descriptor);
    S8  *dpTemp;
    S8  str[XML_STR_DUMMY_SIZE + 1];
    U16 iLen;
	S8  *tempStr, *tempOffset;
	U16 lengthXmlBlock;

    iLen = (U16)(OS_StrLen(XML_PROPERTIES_NENC_ENC_TEXT) + OS_StrLen(XML_PROPERTIES_NENC_PIXEL_TEXT) + OS_StrLen(XML_PROPERTIES_NENC_SIZE_TEXT));

    /* get native encoding block */
    dp = GetXmlKeyElementValue(descriptor,(S8*)XML_PROPERTIES_NENC_ENC_TEXT,str,XML_STR_DUMMY_SIZE,(U16)(desLen-iLen));

    if( dp != descriptor )
    {
		/* Continue only until xml block is ended by '/'                     */
		/* We need the length of the block as input to GetXmlKeyElementValue */
		tempStr = StrFindFirstChar(dp,'/');
		lengthXmlBlock = (U16)(tempStr - dp);            

        /* get the required encoding */    
        format->encoding = GetBipEncodingFromString(str);		

        /* get pixel */
		tempOffset = GetXmlKeyElementValue(dp,(S8*)XML_PROPERTIES_NENC_PIXEL_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
		if (tempOffset != dp)
		{
			format->pixel = AssignPixelSizes(str);
		}

    	/* get the size */
        dpTemp = GetXmlKeyElementValue(dp,(S8*)XML_PROPERTIES_NENC_SIZE_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
        if( dpTemp != dp )
            format->size = OS_AtoU32((char*)dpTemp);
        else
            format->size = 0;

        /* we are at the end of this XML block */
        dp = tempStr;        
    }
    
    return BT_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * XmlParseVariantEncodingFromPropertiesDescriptor()
 */
S8* XmlParseVariantEncodingFromPropertiesDescriptor(S8                    *descriptor,
                                                               BtlBipVariantEncoding *format)
{
    S8  *dp     = descriptor;
    U16 desLen  = OS_StrLen((char*)descriptor);
    S8  *dpTemp;
    S8  str[XML_STR_DUMMY_SIZE + 1];
    U16 iLen;
	S8  *tempStr, *tempOffset;
	U16 lengthXmlBlock;

    iLen = (U16)(OS_StrLen(XML_PROPERTIES_VENC_ENC_TEXT) + OS_StrLen(XML_PROPERTIES_VENC_PIXEL_TEXT) + OS_StrLen(XML_PROPERTIES_VENC_SIZE_TEXT));

    /* get variant encoding block */
    dp = GetXmlKeyElementValue(descriptor,(S8*)XML_PROPERTIES_VENC_ENC_TEXT,str,XML_STR_DUMMY_SIZE,(U16)(desLen-iLen));
    if( dp != descriptor )
    {
		/* Continue only until xml block is ended by '/'                     */
		/* We need the length of the block as input to GetXmlKeyElementValue */
		tempStr = StrFindFirstChar(dp,'/');
		lengthXmlBlock = (U16)(tempStr - dp);  
        
        /* get the required encoding */    
        format->encoding = GetBipEncodingFromString(str);		

        /* get pixel */
		tempOffset = GetXmlKeyElementValue(dp,(S8*)XML_PROPERTIES_VENC_PIXEL_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
		if (tempOffset != dp)
		{
			format->pixel = AssignPixelSizes(str);
		}

        /* get transformation */
        dpTemp = GetXmlKeyElementValue(dp,(S8*)XML_PROPERTIES_VENC_TRANS_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
        if( dpTemp != dp )
        {
            if( OS_StrCmp( (char*)str, XML_ATTR_TRANSFORMATION_STRETCH_STR) == 0 )
                format->transformation = BTL_BIP_TRANSFORMATION_STRETCH;
            else if( OS_StrCmp( (char*)str, XML_ATTR_TRANSFORMATION_CROP_STR) == 0 )
                format->transformation = BTL_BIP_TRANSFORMATION_CROP;
            else if( OS_StrCmp( (char*)str, XML_ATTR_TRANSFORMATION_FILL_STR) == 0 )
                format->transformation = BTL_BIP_TRANSFORMATION_FILL;
            else
                format->transformation = BTL_BIP_TRANSFORMATION_NONE;
            dp = dpTemp;
        }

    	/* is maxSize available ? if not, make it 0 to tell the app */
        dpTemp = GetXmlKeyElementValue(dp,(S8*)XML_PROPERTIES_VENC_SIZE_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);
        if( dpTemp != dp )
            format->maxSize = OS_AtoU32((char*)dpTemp);
        else
            format->maxSize = 0;
        
        /* we are at the end of this XML block */
        dp = tempStr;        
    }

    return dp;
}

/*-------------------------------------------------------------------------------
 * XmlParseAttachmentTypeFromPropertiesDescriptor()
 */
S8* XmlParseAttachmentTypeFromPropertiesDescriptor(S8               *descriptor,
                                                              BtlBipAttachment *format)
{
    S8  *dp    = descriptor;
    S8  *dpTemp;
    U16 desLen = OS_StrLen((char*)descriptor);
    U16 iLen   = OS_StrLen(XML_PROPERTIES_ATTCH_CONTYPE_TEXT);
    S8  str[XML_STR_DUMMY_SIZE + 1];
	S8  *tempStr;
	U16 lengthXmlBlock;

    /* get required content type */
    dp = GetXmlKeyElementValue(descriptor,(S8*)XML_PROPERTIES_ATTCH_CONTYPE_TEXT,format->contentType,XML_STR_DUMMY_SIZE,(U16)(desLen-iLen));
    dpTemp = dp;
    if( dp != descriptor )
    {
		/* Continue only until xml block is ended by '/'                     */
		/* We need the length of the block as input to GetXmlKeyElementValue */
		tempStr = StrFindFirstChar(dp,'/');
		lengthXmlBlock = (U16)(tempStr - dp); 

        /* get chartype */
        dpTemp = GetXmlKeyElementValue(dp,(S8*)XML_PROPERTIES_ATTCH_CHARSET_TEXT,format->charset,20,lengthXmlBlock);
        
        /* get name */
        dpTemp = GetXmlKeyElementValue(dp,(S8*)XML_PROPERTIES_ATTCH_NAME_TEXT,format->name,20,lengthXmlBlock);
        
        /* get size */
        dpTemp = GetXmlKeyElementValue(dp,(S8*)XML_PROPERTIES_ATTCH_SIZE_TEXT,str,XML_STR_DUMMY_SIZE,lengthXmlBlock);

        if( dpTemp != dp )
            format->size = OS_AtoU32((char*)dpTemp);
        else
            format->size = 0;
    
        /* we are at the end of this XML block */
        dp = tempStr;        
    }

    return dp;
}

BtStatus btl_bip_XmlParseGetImagePropertiesResponse(const BtlObject *dataToParse,
                							BtlBipintImgPropertiesElements		*elementsData,
                							BtlBipintImgPropertiesElementsMask	*elementsMaskFound )
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32      ulImgPropsLen;
    BtlUtf8 *pImgProps = NULL;
	int i=0;
    U8 maxVariants = 0, maxAttachments = 0;
    S8* pTemp = NULL, *pReturn;
    U16 iKeyLen = 0;

	/* Initialization */
    *elementsMaskFound = 0;
	OS_MemSet(elementsData, 0, sizeof(*elementsData));

    /* Location on FS or in Memory ? */
    pImgProps = FindXmlObjectLocation(dataToParse, &ulImgPropsLen);

    /* Get Image handle */
    XmlParseHandleFromImagePropertiesDescriptor((S8 *)pImgProps, elementsData->imgHandle);

    /* Get Optional Friendly name */
    XmlParseFriendlyNameFromImagePropertiesDescriptor((S8 *)pImgProps, (S8*)elementsData->friendlyName);

    /* Get Native encoding and set mask if available */
    if (BT_STATUS_SUCCESS == XmlParseNativeEncodingFromPropertiesDescriptor((S8 *)pImgProps, &elementsData->nativeEncoding))
    {
      *elementsMaskFound |= BTL_BIPINT_IMG_PROPERTIES_ELEMENT_MASK_NATIVE_ENCODING;
    }

    /* Get all variants : first get number of variants and check overflow, then assign */
    iKeyLen = (U16)(OS_StrLen(XML_PROPERTIES_VENC_ENC_TEXT));
    if (BT_STATUS_SUCCESS == FindXmlKeyElement((S8 *)pImgProps, (S8*)XML_PROPERTIES_VENC_ENC_TEXT, (U16)(ulImgPropsLen-iKeyLen), &elementsData->numVariantEncodings))
    {
       /* do we have variant overflow ? */
	   if (elementsData->numVariantEncodings > BTL_BIPINT_MAX_NUM_OF_VARIANT_ENCODINGS)
       {
         elementsData->variantEncodingOverflow = TRUE;
         maxVariants = BTL_BIPINT_MAX_NUM_OF_VARIANT_ENCODINGS;
       }
       else
       {
         elementsData->variantEncodingOverflow = FALSE;
         maxVariants = elementsData->numVariantEncodings;
       }

       /* assign the variant encodings until max num of variants reached */
       pTemp = (S8*)pImgProps;
       for(i=0;i<maxVariants;i++)
       {
          pReturn = XmlParseVariantEncodingFromPropertiesDescriptor((S8 *)pTemp, &elementsData->variantEncodings[i]);

		  /* we passed the variant, goto next */
          pTemp = pReturn;
       }

       /* set the mask since we found it */
       *elementsMaskFound |= BTL_BIPINT_IMG_PROPERTIES_ELEMENT_MASK_VARIANT_ENCODING;
   }

    /* Get all attachments : first get number of attachments and check overflow, then assign */
    iKeyLen = (U16)(OS_StrLen(XML_PROPERTIES_ATTCH_CONTYPE_TEXT));
    if (BT_STATUS_SUCCESS == FindXmlKeyElement((S8 *)pImgProps, (S8*)XML_PROPERTIES_ATTCH_CONTYPE_TEXT, (U16)(ulImgPropsLen-iKeyLen), &elementsData->numAttachments))
    {
       /* do we have attachments overflow ? */
	   if (elementsData->numAttachments > BTL_BIPINT_MAX_NUM_OF_ATTACHMENTS)
       {
         elementsData->attachmentsOverflow = TRUE;
         maxAttachments = BTL_BIPINT_MAX_NUM_OF_ATTACHMENTS;
       }
       else
       {
		 elementsData->attachmentsOverflow = FALSE;
         maxAttachments = elementsData->numAttachments;
       }

       /* assign the attachments until max num of variants reached */
       pTemp = (S8*)pImgProps;
       for(i=0;i<maxAttachments;i++)
       {
          pReturn = XmlParseAttachmentTypeFromPropertiesDescriptor(pTemp, &elementsData->attachments[i]);

		  /* we passed the attachment, goto next */
          pTemp = pReturn;
       }

       /* set the mask since we found it */
       *elementsMaskFound |= BTL_BIPINT_IMG_PROPERTIES_ELEMENT_MASK_ATTACHMENT;
    }

	return status;
}

BtStatus btl_bip_XmlParseGetCapabilitiesResponse(const BtlObject *dataToParse,
                							BtlBipintCapabilitiesElements		*elementsData,
                							BtlBipintCapabilitiesElementsMask	*elementsMaskFound )
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32	ulImgCapsLen;
    BtlUtf8 *pImgCaps = NULL;
	int i=0;
    U8 maxImgFormats = 0, maxAttachmentFormats = 0;
    S8* pTemp, *pReturn;
    U16 iKeyLen = 0;

	/* Initialize */
	*elementsMaskFound = 0;
	OS_MemSet(elementsData, 0, sizeof(*elementsData));

    /* Location on FS or in Memory ? */
    pImgCaps = FindXmlObjectLocation(dataToParse, &ulImgCapsLen);

    /* Get Preferred Format and set mask if available */
    if (BT_STATUS_SUCCESS == XmlParsePreferredFormatFromCapabilitiesDescriptor((S8 *)pImgCaps, &elementsData->preferredFormat))
    {
      *elementsMaskFound |= BTL_BIPINT_CAPABILITIES_ELEMENT_MASK_PREFERRED_FORMAT;
    }

    /* Get all image formats : first get number of formats and check overflow, then assign */
    iKeyLen = (U16)(OS_StrLen(XML_CAPABILITIES_IMG_ENC_TEXT));
    if (BT_STATUS_SUCCESS == FindXmlKeyElement((S8 *)pImgCaps, (S8*)XML_CAPABILITIES_IMG_ENC_TEXT, (U16)(ulImgCapsLen-iKeyLen), &elementsData->numImageFormats))
    {
       /* do we have image formats overflow ? */
	   if (elementsData->numImageFormats > BTL_BIPINT_MAX_NUM_OF_IMAGE_FORMATS)
       {
         elementsData->imageFormatsOverflow= TRUE;
         maxImgFormats = BTL_BIPINT_MAX_NUM_OF_VARIANT_ENCODINGS;
       }
       else
       {
		 elementsData->imageFormatsOverflow= FALSE;
         maxImgFormats = elementsData->numImageFormats;
       }

       /* assign the image formats until max num of formats reached */
       pTemp = (S8*)pImgCaps;
       for(i=0;i<maxImgFormats;i++)
       {
          pReturn = XmlParseImageFormatFromCapabilitiesDescriptor((S8 *)pTemp, &elementsData->imageFormats[i]);

		  /* we passed the image format, goto next */
          pTemp = pReturn;
       }

       /* set the mask since we found it */
       *elementsMaskFound |= BTL_BIPINT_CAPABILITIES_ELEMENT_MASK_IMAGE_FORMATS;
   }

    /* Get all attachment formats: first get number of attachments formats and check overflow, then assign */
    iKeyLen = (U16)(OS_StrLen(XML_CAPABILITIES_ATTCH_FORMAT_TEXT));
    if (BT_STATUS_SUCCESS == FindXmlKeyElement((S8 *)pImgCaps, (S8*)XML_CAPABILITIES_ATTCH_FORMAT_TEXT, (U16)(ulImgCapsLen-iKeyLen), &elementsData->numAttachmentFormats))
    {
       /* do we have attachments formats overflow ? */
	   if (elementsData->numAttachmentFormats> BTL_BIPINT_MAX_NUM_OF_ATTACHMENT_FORMATS)
       {
         elementsData->attachmentFormatsOverflow = TRUE;
         maxAttachmentFormats = BTL_BIPINT_MAX_NUM_OF_ATTACHMENT_FORMATS;
       }
       else
       {
		 elementsData->attachmentFormatsOverflow = FALSE;
         maxAttachmentFormats = elementsData->numAttachmentFormats;
       }

       /* assign the attachments formats until max num of attachment formats reached */
       pTemp = (S8*)pImgCaps;
       for(i=0;i<maxAttachmentFormats;i++)
       {
          pReturn = XmlParseAttachmentTypeFromCapabilitesDescriptor((S8 *)pTemp, &elementsData->attachmentFormats[i]);

		  /* we passed the attachment format, goto next */
          pTemp = pReturn;
       }

       /* set the mask since we found it */
       *elementsMaskFound |= BTL_BIPINT_CAPABILITIES_ELEMENT_MASK_ATTACHMENT_FORMATS;
    }

    /* Get filtering parameters and set mask if available */
    if (BT_STATUS_SUCCESS == XmlParseFilteringParamFromCapabilitesDescriptor((S8 *)pImgCaps, &elementsData->filteringParameters))
    {
      *elementsMaskFound |= BTL_BIPINT_CAPABILITIES_ELEMENT_MASK_FILTERING_PARAMETERS;
    }

	return status;
}

BtStatus btl_bip_XmlParseGetImagesListResponse(U16 nrLatestImgHandles,
                                            const BtlObject *dataToParse,
                							BtlBipintImagesListMetaData     *elementsMetaData,
                							BtlBipintImagesListElementsMask	*elementsMaskFound )
{
	BtStatus status = BT_STATUS_SUCCESS;
	U32	ulImgListLen;
    BtlUtf8 *pImgList = NULL;
    U16 iKeyLen = 0;
    U16 NrOfFoundHandles = 0;

	UNUSED_PARAMETER(nrLatestImgHandles);
	
	/* Initialization */
    *elementsMaskFound = 0;
	OS_MemSet(elementsMetaData, 0, sizeof(*elementsMetaData));

    /* Location on FS or in Memory ? */
    pImgList= FindXmlObjectLocation(dataToParse, &ulImgListLen);

/*  Restriction : The response to a GetImageList may contain supported Filteringparameters from the responder.
    This is not supported by the ESI stack to pass the upper layer. Therefor , below code is commented
    and should be used in the future when this is supported by ESI */
    /* Do we have a filter ?                              */
    /* Get filtering parameters and set mask if available */
/*
    if (BT_STATUS_SUCCESS == XmlParseFilteringParamFromCapabilitesDescriptor((BtlUtf8 *)pImgList, &elementsMetaData->filter))
    {
      *elementsMaskFound |= BTL_BIPINT_IMG_LIST_ELEMENT_MASK_FILTER;
    }
*/
    /* Do we have a list of handles ? Set mask if available */
    iKeyLen = (U16)(OS_StrLen(XML_IMAGE_LIST_HANDLE_TEXT));
    if (BT_STATUS_SUCCESS == FindXmlKeyElement((S8 *)pImgList, (S8*)XML_IMAGE_LIST_HANDLE_TEXT,  (U16)(ulImgListLen-iKeyLen)  , (U8*)&NrOfFoundHandles))
    {
      *elementsMaskFound |= BTL_BIPINT_IMG_LIST_ELEMENT_MASK_LIST;
    }

    /* Save offset for the start of the XML image listing, needed for btl_bip_XmlParseNextFromImagesList */
    elementsMetaData->xmlImgListObjectOffset = (S8*)pImgList;

    /* return latest image handles */
    elementsMetaData->nbReturnedHandles = NrOfFoundHandles;

	return status;
}

BtStatus btl_bip_XmlParseNextFromImagesList(BtlBipintImagesListMetaData *elementsMetaData, BtlBipImagesListing *imageInfo)
{
	BtStatus status = BT_STATUS_SUCCESS;
    S8*      pTemp;

    /* Get Next Image Handle and Image details */
    pTemp = XmlParseHandleFromImagesListDescriptor(elementsMetaData->xmlImgListObjectOffset,
                                                   imageInfo->imgHandle,
                                                   &imageInfo->created,
                                                   &imageInfo->modified);

	if (pTemp == elementsMetaData->xmlImgListObjectOffset)
	{
		status = BT_STATUS_NOT_FOUND;
	}

    /* update offset in order to get the next imagehandle if requested */
    elementsMetaData->xmlImgListObjectOffset = pTemp;
	
	return status;
}

/********************************************************************************
 *
 * Function implementation, (local)
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * XmlBuildAttrTransformation()
 *
 *     This routine does add the 'transformation' attribute at the end of
 *     'xmlDescr'.
 */
static void XmlBuildAttrTransformation(S8 *xmlDescr,
                                BtlBipTranformation transformation)
{
    switch (transformation)
    {
        case BTL_BIP_TRANSFORMATION_STRETCH:
            OS_StrCat((char*)xmlDescr, XML_ATTR_TRANSFORMATION_STRETCH_STR);
            break;
        case BTL_BIP_TRANSFORMATION_FILL:
            OS_StrCat((char*)xmlDescr, XML_ATTR_TRANSFORMATION_CROP_STR);
            break;
        case BTL_BIP_TRANSFORMATION_CROP:
            OS_StrCat((char*)xmlDescr, XML_ATTR_TRANSFORMATION_FILL_STR);
            break;
        /* BTL_BIP_TRANSFORMATION_NONE: no transformation. Skip this attribute. */
        default:
            break;
    }
}

/*-------------------------------------------------------------------------------
 * btl_bip_XmlBuildAttrPixel()
 *
 *     This routine does add the 'pixel' attribute at the end of
 *     'xmlDescr'.
 */
void btl_bip_XmlBuildAttrPixel(S8 *xmlDescr,
                                BtlBipPixel *pixelRange)
{
    S8  str[U32_DEC_LENGTH];

    /* Fixed size? --> add it. */
    if ((pixelRange->widthSmall) == (pixelRange->widthLarge) &&
        (pixelRange->heightSmall) == (pixelRange->heightLarge))
    {
          /* Add W1. */
          bip_itoa(pixelRange->widthSmall,str,FALSE,0);
          OS_StrCat((char*)xmlDescr, (char*)str);
          OS_StrCat((char*)xmlDescr, "*");
          /* Add H1. */
          bip_itoa(pixelRange->heightSmall,str,FALSE,0);
          OS_StrCat((char*)xmlDescr, (char*)str );
    }
    /* Range with fixed aspect ratio? --> add it. */
    else if (pixelRange->heightSmall == pixelRange->heightLarge)
    {
          /* Add W1. */
          bip_itoa(pixelRange->widthSmall,str,FALSE,0);
          OS_StrCat((char*)xmlDescr, (char*)str);
          OS_StrCat((char*)xmlDescr, "**");

          /* Insert separator */
          OS_StrCat((char*)xmlDescr, "-");

          /* Add W2. */
          bip_itoa(pixelRange->widthLarge,str,FALSE,0);
          OS_StrCat((char*)xmlDescr, (char*)str);
          OS_StrCat((char*)xmlDescr, "*");
          /* Add H2. */
          bip_itoa(pixelRange->heightLarge,str,FALSE,0);
          OS_StrCat((char*)xmlDescr, (char*)str);
    }
    /* Range! --> add it. */
    else
    {
          /* Add W1. */
          bip_itoa(pixelRange->widthSmall,str,FALSE,0);
          OS_StrCat((char*)xmlDescr, (char*)str);
          OS_StrCat((char*)xmlDescr, "*");
          /* Add H1. */
          bip_itoa(pixelRange->heightSmall,str,FALSE,0);
          OS_StrCat((char*)xmlDescr, (char*)str);

          /* Insert separator */
          OS_StrCat((char*)xmlDescr, "-");

          /* Add W2. */
          bip_itoa(pixelRange->widthLarge,str,FALSE,0);
          OS_StrCat((char*)xmlDescr, (char*)str);
          OS_StrCat((char*)xmlDescr, "*");
          /* Add H2. */
          bip_itoa(pixelRange->heightLarge,str,FALSE,0);
          OS_StrCat((char*)xmlDescr, (char*)str);
    }
}

/*-------------------------------------------------------------------------------
 * bip_ntoc()
 */
static char bip_ntoc( U8 Nr )
{
    S8  c;

    if( Nr > 10 )
        return 0;

    c = (S8)(0x30 + Nr);
    return c;
}

/*-------------------------------------------------------------------------------
 * bip_itoa()
 */
static char *bip_itoa( U32 Nr, S8 *AddrString, BOOL bZeroFill, U8 nrOfChar)
{
    U8           u, l, t;

    AddrString[U32_DEC_LENGTH] = 0;

    for( u=0; u < U32_DEC_LENGTH; u++ )
    {
        AddrString[u] = bip_ntoc((S8)(Nr%10));
        Nr = Nr / 10;
    }
    u--;
    if( FALSE == bZeroFill )
    {
        while( (AddrString[u] == '0') && u!= 0 )
        {
            AddrString[u] = 0;
            u--;
        }
    }
    else
    {
        if( nrOfChar < U32_DEC_LENGTH )
        {
            AddrString[nrOfChar] = 0;
        }
    }
    l = (U8)(OS_StrLen((char*)AddrString)-1);
    u = 0;
    while( u < l )
    {
        t = AddrString[u];
        AddrString[u] = AddrString[l];
        AddrString[l] = t;
        u++;
        l--;
    }
    return (char*)AddrString;
}

/*-------------------------------------------------------------------------------
 * bip_GetBipEncodingString()
 */
static S8* bip_GetBipEncodingString(BtlBipEncoding bipEncoding, S8* string )
{
    switch( bipEncoding)
    {
        case BTL_BIP_ENCODING_JPEG:     OS_StrCpy((char*)string, XML_ATTR_ENCODING_JPEG_STR); break;
        case BTL_BIP_ENCODING_JPEG2000: OS_StrCpy((char*)string, XML_ATTR_ENCODING_JPEG2000_STR); break;
        case BTL_BIP_ENCODING_PNG:      OS_StrCpy((char*)string, XML_ATTR_ENCODING_PNG_STR); break;
        case BTL_BIP_ENCODING_GIF:      OS_StrCpy((char*)string, XML_ATTR_ENCODING_GIF_STR); break;
        case BTL_BIP_ENCODING_BMP:      OS_StrCpy((char*)string, XML_ATTR_ENCODING_BMP_STR); break;
        case BTL_BIP_ENCODING_WBMP:     OS_StrCpy((char*)string, XML_ATTR_ENCODING_WBMP_STR); break;
        default: OS_StrCpy((char*)string, "USR-TIP"); break;
/*gesl: QUESTION: why "USR-TIP"?*/
    }
    return string;
}

/*-------------------------------------------------------------------------------
 * bip_StrNICmp()
 */
static U32 bip_StrNICmp(const char *Str1, const char *Str2, U32 n)
{
    U32 idx;

    for (idx = 0; idx < n; ++idx) {
        if (ToUpper(Str1[idx]) != ToUpper(Str2[idx])) {
            return 1;
        }
        /* The first idx characters are already equal,
           so test only if Str1's end has been reached. */
        if (Str1[idx] == '\0') {
            return 0;
        }
    }
    return 0;
}

/*-------------------------------------------------------------------------------
 * GetXmlKeyElementValue()
 */
static S8* GetXmlKeyElementValue(S8* descriptor, S8* field, S8* attribute, U16 attrLen, U16 maxLen)
{
    S8 *dp = descriptor;
    S8 *dpRet;
    S8 *dpTemp;
    U16 iLen=0;
    U16 fieldLen = OS_StrLen((char*)field);
    BOOL fieldFound = FALSE;

	OS_MemSet(attribute, 0, attrLen+1);

	/* Find the field in the object */
    for(iLen=0;iLen<maxLen;iLen++)
    {
		/* upper case it before comparison to be more robust */
        if( bip_StrNICmp((char*)dp,(char*)field,fieldLen) == 0 )
        {
            fieldFound = TRUE;
            break;
        }
        dp++;
    }

	/* We found the field, now determine the end of the value, always ended by character ' " '  */
	/* and add the value to the attribute parameter, making sure that attribute is 0-terminated */
    if( TRUE == fieldFound )
    {
        while( (*dp != '"') && (iLen < maxLen) )
        {
            dp++;
        }
        if( '"' == *dp )
            dp++;
        else
            return dp;
        OS_StrnCpy((char*)attribute,(char*)dp,attrLen);
        dpRet = dp;
        dp = StrFindFirstChar(attribute, (S8)'"' );
        dpTemp = attribute;
        if( NULL != dp )
        {
            while( dpTemp != dp )
            {
                if( '"' == *dpTemp )
                {
                    *dpTemp = 0;
                    break;
                }
                dpTemp++;
            }
            if( dp == dpTemp )
                *dp = 0;
        }
    }
    else
        dpRet = descriptor;
    return dpRet;
}

static BtStatus FindXmlKeyElement(S8* pXmlObject, S8* pKey, U16 maxLenXmlObject, U8 *uiNrOfKeysFound)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    S8   *pLocal = pXmlObject;
    U16   i = 0;
    U16   uifieldLen = OS_StrLen((char*)pKey);
    U8    uiKeyCnt=0;

    /* search in the XML object for the key */
    for(i=0;i<maxLenXmlObject;i++)
    {
        /* Compare the string, uppercase to make sure we are more robust */
        if( bip_StrNICmp((char*)pLocal,(char*)pKey,uifieldLen) == 0 )
        {
            /* we found a key */
            uiKeyCnt++;
			status = BT_STATUS_SUCCESS;
        }
        pLocal++;
    }

    /* return the found results */
    *uiNrOfKeysFound = uiKeyCnt;
    return status;
}

static BtlUtf8* FindXmlObjectLocation(const BtlObject *dataToParse, U32* ulObjectLen)
{
    BtlUtf8* pObject = NULL;
    
	/* verify the location of the stored object, return the pointer and the sixe of the object */
    if (dataToParse->objectLocation == BTL_OBJECT_LOCATION_FS)
    {
	   *ulObjectLen = (U32)OS_StrLen((char*)dataToParse->location.fsLocation.fsPath);
       pObject      = (BtlUtf8*) dataToParse->location.fsLocation.fsPath;
    }
    else if (dataToParse->objectLocation == BTL_OBJECT_LOCATION_MEM)
    {
	   *ulObjectLen = dataToParse->location.memLocation.size;
       pObject      = (BtlUtf8* ) dataToParse->location.memLocation.memAddr;
    }

    return pObject;
}

static BtlBipEncoding GetBipEncodingFromString(S8* str)
{
	BtlBipEncoding encodingFormat;

	if( OS_StrCmp( (char*)str, XML_ATTR_ENCODING_JPEG_STR) == 0 )
		encodingFormat = BTL_BIP_ENCODING_JPEG;
	else if( OS_StrCmp( (char*)str, XML_ATTR_ENCODING_JPEG2000_STR) == 0 )
		encodingFormat = BTL_BIP_ENCODING_JPEG2000;
	else if( OS_StrCmp( (char*)str, XML_ATTR_ENCODING_PNG_STR) == 0 )
		encodingFormat = BTL_BIP_ENCODING_PNG;
	else if( OS_StrCmp( (char*)str, XML_ATTR_ENCODING_GIF_STR) == 0 )
		encodingFormat = BTL_BIP_ENCODING_GIF;
	else if( OS_StrCmp( (char*)str, XML_ATTR_ENCODING_BMP_STR) == 0 )
		encodingFormat = BTL_BIP_ENCODING_BMP;
	else if( OS_StrCmp( (char*)str, XML_ATTR_ENCODING_WBMP_STR) == 0 )
		encodingFormat = BTL_BIP_ENCODING_WBMP;
	else
		/* gesl - UDI Addition - Issue an error in this case */
		encodingFormat = BTL_BIP_ENCODING_ALL;
	
    return encodingFormat;
}

static BtlBipPixel AssignPixelSizes(S8* str)
{
    BtlBipPixel pixelFormat;
    S8  *dpTemp, *pixelStr;

	/* Is it a pixel range ? Eg. 80*60-640*480 */
	pixelStr = StrFindFirstChar(str,'-');
	if (*pixelStr != NULL)
	{   /* pixel range */
		*pixelStr = 0;
		pixelStr++;
		
		/* assign Small range */
		dpTemp = StrFindFirstChar(str,'*');
		*dpTemp = 0;
		dpTemp++;				
		pixelFormat.widthSmall = (U16)OS_AtoU32((char*)str);
		pixelFormat.heightSmall = (U16)OS_AtoU32((char*)dpTemp);
		
		/* assign Large range */
		dpTemp = StrFindFirstChar(pixelStr,'*');
		*dpTemp = 0;
		dpTemp++;
		pixelFormat.widthLarge = (U16)OS_AtoU32((char*)pixelStr);
		pixelFormat.heightLarge = (U16)OS_AtoU32((char*)dpTemp);			
	}
	else
	{   /*normal pixel resolution Eg. 640*480 */
		dpTemp = (S8*)OS_StrrChr((char*)str,'*');
		*dpTemp = 0;
		dpTemp++;
		
		/* no range defined, so Small sizes will be same as Large */
		pixelFormat.widthSmall = (U16)OS_AtoU32((char*)str);
		pixelFormat.heightSmall = (U16)OS_AtoU32((char*)dpTemp);
		pixelFormat.widthLarge = pixelFormat.widthSmall;
		pixelFormat.heightLarge = pixelFormat.heightSmall;				
	}

    return pixelFormat;
}


static S8 *StrFindFirstChar(const S8 *str, S8 findChar)
{
	
	S8 *strPtr = (S8*)str;
	while ((0 != *strPtr) && (findChar != *strPtr))
	{
		strPtr++;
	}
	
	return strPtr;
}

#endif /* BTL_CONFIG_BIP ==   BTL_CONFIG_ENABLED */


