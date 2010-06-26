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
*   FILE NAME:      btl_bip_xml.h
*
*   DESCRIPTION:    This file defines the API of the BTL BIP client XML parser and builder.
*					Used to parse a folder listing in an XML format revceived
*					from the server and built to send to the server.
*
*   AUTHOR:         Rene Kuiken
*
\*******************************************************************************/

#ifndef __BTL_BIP_XML
#define __BTL_BIP_XML


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "osapi.h"
#include "bttypes.h"
#include "bthal_fs.h"
#include "btl_config.h"
#include "btl_utils.h"
#include "btl_debug.h"

#include "btl_bip_xml.h"
#include "btl_bipint.h"
#include "btl_obex_utils.h"
#include <btl_common.h>
#include "btl_unicode.h"

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
 * Function declarations
 *
 *******************************************************************************/
/* XML Build Descriptor routines, */
extern void btl_bip_XmlBuildDescrImageHandles(S8 *xmlDescr, BtlBipFilter *filter);
extern void btl_bip_XmlBuildDescrImage(S8 *xmlDescr,
                                           BtlBipEncoding encoding,
                                           BtlBipPixel *pixelRange,
                                           U32 size,
                                           U32 maxSize,
                                           BtlBipTranformation transformation);

extern void btl_bip_XmlBuildAttrPixel(S8 *xmlDescr, BtlBipPixel *pixelRange);

/* XML Parse Attribute routines, */
extern BtStatus btl_bip_XmlParseGetImagePropertiesResponse(const BtlObject *dataToParse,
                							BtlBipintImgPropertiesElements		*elementsData,
                							BtlBipintImgPropertiesElementsMask	*elementsMaskFound );
extern BtStatus btl_bip_XmlParseGetCapabilitiesResponse(const BtlObject *dataToParse,
                							BtlBipintCapabilitiesElements		*elementsData,
                							BtlBipintCapabilitiesElementsMask	*elementsMaskFound );
extern BtStatus btl_bip_XmlParseGetImagesListResponse(U16 nrLatestImgHandles,
                                            const BtlObject *dataToParse,
                							BtlBipintImagesListMetaData          *elementsMetaData,
                							BtlBipintImagesListElementsMask	     *elementsMaskFound );
extern BtStatus btl_bip_XmlParseNextFromImagesList(BtlBipintImagesListMetaData *elementsMetaData, BtlBipImagesListing *imageInfo);

#endif /* __BTL_BIP_XML */
