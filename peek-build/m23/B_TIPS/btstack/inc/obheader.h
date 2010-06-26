#ifndef __OBHEADERH
#define __OBHEADERH

/****************************************************************************
 *
 * File:          obheader.h
 *
 * Description:   This file specifies the upper level OBEX header interface.
 * 
 * Created:       August 5, 1998
 *
 * Version:       MTObex 3.5
 *
 * Copyright 1998-2006 Extended Systems, Inc.
 * Portions copyright 2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "obex.h"

/*---------------------------------------------------------------------------
 * OBEX Header Library layer
 *
 *     The OBEX header library is a set of types and utility functions that
 *     allow the client and server to build and parse OBEX headers. OBEX
 *     Headers are used to convey attributes of the object that will be
 *     exchanged. Each of these functions will be discussed below. This
 *     library is used by both the client and server.
 */

/****************************************************************************
 *
 * Section: Types
 *
 ****************************************************************************/

/*--------------------------------------------------------------------------
 * ObexHeaderType type
 *
 *     Used to specify the type of a header. The header's type implies the
 *     meaning of the contents and also the data format type, which is listed
 *     with each header type below.  When creating or parsing headers, be sure
 *     to use the correct function for the header's internal format.
 */
typedef U8 ObexHeaderType;

#define OBEXH_COUNT       0xC0 /* (4-byte) Number of objects */
#define OBEXH_NAME        0x01 /* (Unicode) Object name */
#define OBEXH_TYPE        0x42 /* (ByteSeq) MIME type of object */
#define OBEXH_LENGTH      0xC3 /* (4-byte) Object length */
#define OBEXH_TIME_ISO    0x44 /* (ByteSeq, ISO 8601 format) Creation or
                                * modification time for object (preferred format). */
#define OBEXH_TIME_COMPAT 0xC4 /* (4-byte) Creation or modification time for
                                * object for backward-compatibility. */
#define OBEXH_DESCRIPTION 0x05 /* (Unicode) Text description of object */
#define OBEXH_TARGET      0x46 /* (ByteSeq) Target ID for operation */
#define OBEXH_HTTP        0x47 /* (ByteSeq) An HTTP 1.x header (URL for object) */
#define OBEXH_BODY        0x48 /* Not for use by OBEX applications */
#define OBEXH_END_BODY    0x49 /* Not for use by OBEX applications */
#define OBEXH_WHO         0x4A /* (ByteSeq) Who ID identifies service providing
                                * the object */
#define OBEXH_CONNID      0xCB /* (4-byte) Identifies the connection for which
                                * the operation is directed */
#define OBEXH_APP_PARAMS  0x4C /* (ByteSeq) Application parameters */
#define OBEXH_AUTH_CHAL   0x4D /* (ByteSeq) Authentication challenge */
#define OBEXH_AUTH_RESP   0x4E /* (ByteSeq) Authentication response */
#define OBEXH_CREATOR_ID  0xCF /* (4-byte)  Creator ID contains Palm/MAC files
                                * creator code */
#define OBEXH_WAN_UUID    0x50 /* (ByteSeq) Network client UUID */
#define OBEXH_OBJ_CLASS   0x51 /* (ByteSeq) Object class */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
#define OBEXH_SESSION_PARAMS    0x52 /* (ByteSeq) Session Parameters */
#define OBEXH_SESSION_SEQ_NUM   0x93 /* (1-byte) Session Sequence Number */
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/* End of ObexHeaderType */

/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/

#if OBEX_TLV_PARSER == XA_ENABLED
/*--------------------------------------------------------------------------
 * ObexTlv structure
 * 
 *    Used by OBEXH_ParseTlv to return information about a TLV 
 *    (Type-Length-Value) triplet.
 *
 *    Note that TLVs with a length equal to or smaller than
 *    OBEX_TLV_BUFF_SIZE will always be provided in a single return
 *    from OBEXH_ParseTlv. In this case "remainLen" will be 0.
 *
 *    TLVs with a length greater than OBEX_TLV_BUFF_SIZE may arrive in one
 *    or more successful returns from OBEXH_ParseTlv. In these cases, each
 *    successful return will indicate more of the TLV (as specified by
 *    "value" and "valueLen"). "remainLen" will indicate how much of the
 *    full value remains to be indicated.
 */
typedef struct _ObexTlv {
    U8          tag;        /* "Type" byte of the T-L-V triplet */
    U8          length;     /* "Length" byte of the T-L-V triplet */
    U8          *value;     /* Pointer to value data */
    U8          valueLen;   /* Length of actual data in "value" */

    /* Number of bytes yet to be indicated for this TLV */
    U16         remainLen;  
} ObexTlv;
#endif /* OBEX_TLV_PARSER == XA_ENABLED */


/****************************************************************************
 *
 * Section: Header Construction Functions
 *
 ****************************************************************************/

#if OBEX_HEADER_BUILD == XA_ENABLED

/*---------------------------------------------------------------------------
 * OBEXH_BuildByteSeq()
 *
 *     Builds a Byte Sequence style header for transmission with the client
 *     request or server response. Byte Sequence headers have the 7th & 8th
 *     bits in their type set to zero and one, respectively. Type, ISO Time,
 *     Target, HTTP and Who headers are Byte Sequence style.
 *
 * Requires:
 *     OBEX_HEADER_BUILD set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 *     Type - OBEXH_ type of header to build.  This value is taken from the
 *         ObexHeaderType list or can be a user defined type.
 *
 *     Value - Pointer to buffer to copy into header.
 *
 *     Len - Length of data pointed to by Value.
 *
 * Returns:
 *     TRUE - Success.
 *
 *     FALSE - Header exceeded buffer size.
 */
BOOL OBEXH_BuildByteSeq(ObexAppHandle *AppHndl, ObexHeaderType Type, const U8 *Value, U16 Len);

/*---------------------------------------------------------------------------
 * OBEXH_BuildUnicode()
 *
 *     Builds a Unicode style header for transmission with the client request
 *     or server response. Unicode headers have the 7th & 8th bits in their
 *     type set to zero. Name and Description headers are Unicode style.
 *
 * Requires:
 *     OBEX_HEADER_BUILD set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 *     Type - OBEXH_ type of header to build. This value is taken from the
 *         ObexHeaderType list or can be a user-defined type.
 *
 *     Value - Pointer to buffer to copy into header.
 *
 *     Len - Length of data pointed to by Value.
 *
 * Returns:
 *     TRUE - Success.
 *
 *     FALSE - Header exceeded buffer size.
 */
BOOL OBEXH_BuildUnicode(ObexAppHandle *AppHndl, ObexHeaderType Type, const U8 *Value, U16 Len);

/*---------------------------------------------------------------------------
 * OBEXH_Build4Byte()
 *
 *     Builds a 4-Byte style header for transmission with the client request
 *     or server response. 4-Byte headers have the 7th & 8th bits in their
 *     type set to one. Length, Count and Compatibility Time headers are 4-Byte
 *     style.
 *
 * Requires:
 *     OBEX_HEADER_BUILD set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 *     Type - OBEXH_ type of header to build. This value is taken from the
 *         ObexHeaderType list or can be a user defined type.
 *
 *     Value - 4 byte value to place in header.
 *
 * Returns:
 *     TRUE - Success.
 *
 *     FALSE - Header exceeded buffer size.
 */
BOOL OBEXH_Build4Byte(ObexAppHandle *AppHndl, ObexHeaderType Type, U32 Value);

/*---------------------------------------------------------------------------
 * OBEXH_Build1Byte()
 *
 *     Builds a Byte style header for transmission with the client request or
 *     server response. Byte headers have the 7th & 8th bits in their type set
 *     to one and zero, respectively. The OBEX protocol does not define any
 *     Byte headers.
 *
 * Requires:
 *     OBEX_HEADER_BUILD set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 *     Type - OBEXH_ type of header to build. This value is taken from the
 *         ObexHeaderType list or can be a user defined type.
 *
 *     Value - 1 byte value to place in header
 *
 * Returns:
 *     TRUE - Success.
 *
 *     FALSE - Header exceeded buffer size.
 */
BOOL OBEXH_Build1Byte(ObexAppHandle *AppHndl, ObexHeaderType Type, U8 Value);

/*---------------------------------------------------------------------------
 * OBEXH_GetAvailableTxHeaderLen()
 *
 *      Returns the amount of header space available for adding more
 *      headers. This value is based on the OBEX Packet size, the
 *      application provided header buffer size and any headers which are
 *      already present in the buffer.
 *
 * Requires:
 *     OBEX_HEADER_BUILD set to XA_ENABLED.
 *
 * Parameters:
 *      AppHndl - Application handle provided during initialization.
 *
 * Returns:
 *      Available header buffer length in bytes.
 */
U16 OBEXH_GetAvailableTxHeaderLen(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBEXH_FlushHeaders()
 *
 *     Flushes header buffer of unsent headers. This function flushes headers
 *     which were built, but not passed to the OBEX protocol to send. In the
 *     normal course of client and server execution this function is
 *     unnecessary. However if the application were to build some headers and
 *     then encounter an error that results in a cancellation of the operation
 *     without initiating the operation, the buffer needs to be flushed.
 *
 * Requires:
 *     OBEX_HEADER_BUILD set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 */
void OBEXH_FlushHeaders(ObexAppHandle *AppHndl);

#endif /* OBEX_HEADER_BUILD == XA_ENABLED */


/****************************************************************************
 *
 * Section: Header Parsing Functions
 *
 ****************************************************************************/

#if OBEX_HEADER_PARSE == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEXH_GetHeaderType()
 *
 *     Returns the type of the current header. This function may 
 *     only be called during an OBxx_HEADER_RX event.
 *                
 * Requires:
 *     OBEX_HEADER_PARSE set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 * Returns:
 *     ObexHeaderType
 */
ObexHeaderType OBEXH_GetHeaderType(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBEXH_GetTotalHeaderLen()
 *
 *     Returns the total length of the current header. For use
 *     only with Byte Sequence and Unicode headers.  The GetHeaderType()
 *     function should have been called previously to insure that the format of
 *     the header is either Byte Sequence or Unicode before calling this
 *     function. This function may only be called during an OBxx_HEADER_RX
 *     event. Note that the entire header may not be available when the header
 *     is indicated. In this case the remainder of the header will be made
 *     available in subsequent OBxx_HEADER_RX indications. The GetHeaderAvailLen()
 *     function should be used to determine if the indicated header is complete.
 *                
 * Requires:
 *     OBEX_HEADER_PARSE set to XA_ENABLED.
 *                
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 * Returns:
 *     Total header length.
 */
U16 OBEXH_GetTotalHeaderLen(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBEXH_GetHeaderAvailLen()
 *
 *     Returns the currently available (to copy) header length. For use
 *     only with Byte Sequence and Unicode headers. The GetHeaderType()
 *     function should have been called previously to insure that the format
 *     of the header is either Byte Sequence or Unicode before calling this
 *     function. This function may only be called during an OBxx_HEADER_RX
 *     event. Note that for Byte Sequence and Unicode type headers, the entire
 *     header may not be available when the header is indicated. In this case the
 *     remainder of the header will be made available in subsequent HEADER_RX
 *     indications. The function GetTotalHeaderLen() should be used in
 *     conjunction to determine if the entire header is present.
 *                
 * Requires:
 *     OBEX_HEADER_PARSE set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 * Returns:
 *     Available header length in bytes.
 */
U16 OBEXH_GetHeaderAvailLen(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBEXH_GetHeaderBuff()
 *
 *     Returns a pointer to the current headers buffer. For use
 *     only with Byte Sequence and Unicode headers. The GetHeaderType function
 *     should have been called previously to insure that the format of the
 *     header is either Byte Sequence or Unicode before calling this function.
 *     This function may only be called during an OBxx_HEADER_RX event. Note
 *     that the entire header may not be available when the header is indicated.
 *     In this case only the amount indicated by the function GetHeaderAvailLen()
 *     should be read from the returned buffer pointer.
 *                
 * Requires:
 *     OBEX_HEADER_PARSE set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 * Returns:
 *     Pointer to header's data buffer.
 */
U8 *OBEXH_GetHeaderBuff(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBEXH_GetHeader4Byte()
 *
 *     Returns the value of a 4 byte style header. The GetHeaderType() function
 *     should have been called previously to insure that the format of the
 *     header is 4-byte before calling this function. This function may
 *     only be called during an OBxx_HEADER_RX event.
 *                
 * Requires:
 *     OBEX_HEADER_PARSE set to XA_ENABLED.
 *                
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 * Returns:
 *     The 4-byte value of header.
 */
U32 OBEXH_GetHeader4Byte(ObexAppHandle *AppHndl);

/*---------------------------------------------------------------------------
 * OBEXH_GetHeader1Byte()
 *
 *     Returns the value of a 1 byte style header. The GetHeaderType() function
 *     should have been called previously to insure that the format of the
 *     header is 1-byte before calling this function. This function may
 *     only be called during an OBxx_HEADER_RX event.
 *                
 * Requires:
 *     OBEX_HEADER_PARSE set to XA_ENABLED.
 *                
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 * Returns:
 *     The 1-byte value of header.
 */
U8 OBEXH_GetHeader1Byte(ObexAppHandle *AppHndl);

#endif /* OBEX_HEADER_PARSE == XA_ENABLED */


#if OBEX_TLV_PARSER == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBEXH_ParseTlv()
 *
 *     Extracts information about the next TLV (Type-Length-Value) triplet 
 *     in the current header. This function must be called during handling
 *     of an OBCE_HEADER_RX or OBSE_HEADER_RX event containing a Byte
 *     Sequence header.
 *
 *     The header is expected to be encoded with a standard OBEX TLV format
 *     (one byte for type, one byte for length, 0-255 bytes for value). 
 *
 *     Because any number of TLVs may be present in the header, the 
 *     application must repeatedly call OBEXH_ParseTlv to parse each TLV
 *     until this function returns FALSE. Also, if the header is provided
 *     across multiple HEADER_RX events, OBEXH_ParseTlv should be called
 *     during each event until the header is completely parsed.
 *
 * Requires: 
 *     OBEX_TLV_PARSER set to XA_ENABLED.
 * 
 * Parameters:
 *     AppHndl - Application handle provided during initialization.
 *
 *     Tlv - If this function returns true, this structure will be filled
 *         with information about the current TLV being parsed. Note that if 
 *         "Tlv.value" points to a buffer, that buffer is valid only until
 *         the next OBEXH_ParseTlv call, or until returning from the current
 *         HEADER_RX event, whichever comes first.
 *
 * Returns:
 *     TRUE - Success. "Tlv" now contains valid information about the
 *         current TLV being parsed out of the current header.
 *         OBEXH_ParseTlv should be invoked again to search for more
 *         TLVs in the current HEADER_RX data.
 *
 *     FALSE - Failure. AppHndl or Tlv parameters were invalid, or
 *         not enough HEADER_RX data was available to indicate a TLV. This
 *         function should be called again when more HEADER_RX data is
 *         available.
 */
BOOL OBEXH_ParseTlv(ObexAppHandle *AppHndl, ObexTlv *Tlv);
#endif /* OBEX_TLV_PARSER == XA_ENABLED */

/****************************************************************************
 *
 * Section: Helper Macros
 *
 ****************************************************************************
 *
 *     The following simple macros can be used to determine the header
 *     style based on the header type.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * OBEXH_Is4Byte()
 *
 *     Determines the format of an OBEX header from its type.
 *
 * Parameters:
 *     type - OBEX header's type
 *
 * Returns:
 *     TRUE - The header's format matches.
 *
 *     FALSE - The header's format is different.
 */
BOOL OBEXH_Is4Byte(ObexHeaderType Type);
#define OBEXH_Is4Byte(_Type)    (((_Type) & 0xC0) == 0xC0 ? TRUE : FALSE)


/*---------------------------------------------------------------------------
 * OBEXH_Is1Byte()
 *
 *     Determines the format of an OBEX header from its type.
 *
 * Parameters:
 *     type - OBEX header's type
 *
 * Returns:
 *     TRUE - The header's format matches.
 *
 *     FALSE - The header's format is different.
 */
BOOL OBEXH_Is1Byte(ObexHeaderType Type);
#define OBEXH_Is1Byte(_Type)    (((_Type) & 0xC0) == 0x80 ? TRUE : FALSE)


/*---------------------------------------------------------------------------
 * OBEXH_IsByteSeq()
 *
 *     Determines the format of an OBEX header from its type.
 *
 * Parameters:
 *     type - OBEX header's type
 *
 * Returns:
 *     TRUE - The header's format matches.
 *
 *     FALSE - The header's format is different.
 */
BOOL OBEXH_IsByteSeq(ObexHeaderType Type);
#define OBEXH_IsByteSeq(_Type)  (((_Type) & 0xC0) == 0x40 ? TRUE : FALSE)


/*---------------------------------------------------------------------------
 * OBEXH_IsUnicode()
 *
 *     Determines the format of an OBEX header from its type.
 *
 * Parameters:
 *     type - OBEX header's type
 *
 * Returns:
 *     TRUE - The header's format matches.
 *
 *     FALSE - The header's format is different.
 */
BOOL OBEXH_IsUnicode(ObexHeaderType Type);
#define OBEXH_IsUnicode(_Type)  (((_Type) & 0xC0) == 0x00 ? TRUE : FALSE)


#if (OBEX_HEADER_PARSE == XA_ENABLED) && (OBEX_USE_HEADER_MACROS == XA_ENABLED)

/*---------------------------------------------------------------------------
 *
 * Here are the macro versions of the functions prototyped in this file.
 * They are only used if OBEX_USE_HEADER_MACROS is enabled. If it is
 * disabled the function versions found in obheader.c are used.
 * These macros use the baseline functions that are defined in obheader.c.
 * Refer to obheader.c and the above prototypes for inline documentation.
 */
#include "sys/obparse.h"

/*---------------------------------------------------------------------------
 *            Header Parsing Functions
 *---------------------------------------------------------------------------
 */
#define OBEXH_GetHeaderType(_D) \
    (Assert( (((_D)->parser).rxState == OBSC_RX_PUT_HEAD3) || \
             (((_D)->parser).rxState == OBSC_RX_PUT_HEAD2) ), \
     ((_D)->parser).header)

#define OBEXH_GetTotalHeaderLen(_D) \
    (Assert( (((_D)->parser).rxState == OBSC_RX_PUT_HEAD3) || \
             (((_D)->parser).rxState == OBSC_RX_PUT_HEAD2) ), \
     ((_D)->parser).totalHeaderLen)

#define OBEXH_GetHeaderAvailLen(_D) \
    (Assert( (((_D)->parser).rxState == OBSC_RX_PUT_HEAD3) || \
             (((_D)->parser).rxState == OBSC_RX_PUT_HEAD2) ), \
     ((_D)->parser).dataLen)

#define OBEXH_GetHeaderBuff(_D) \
   (Assert( (((_D)->parser).rxState == OBSC_RX_PUT_HEAD3) || \
            (((_D)->parser).rxState == OBSC_RX_PUT_HEAD2) ), \
    ((_D)->parser).rxBuff)

#define OBEXH_GetHeader4Byte(_D) \
    (Assert( ((_D)->parser).rxState == OBSC_RX_PUT_HEAD2 ), \
     BEtoHost32(((_D)->parser).stageBuff))

#define OBEXH_GetHeader1Byte(_D) \
    (Assert( ((_D)->parser).rxState == OBSC_RX_PUT_HEAD2 ), \
     ((_D)->parser).stageBuff[0])

#endif /* (OBEX_HEADER_PARSE == XA_ENABLED) && */
       /* (OBEX_USE_HEADER_MACROS == XA_ENABLED) */

#endif /* __OBHEADERH */

