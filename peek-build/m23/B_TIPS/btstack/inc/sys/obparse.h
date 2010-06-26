#ifndef __OBPARSE_H
#define __OBPARSE_H
/****************************************************************************
 *
 * File:          obparse.h
 *
 * Description:   This file contains definitions for the OBEX Protocol Parser.
 * 
 * Created:       February 10, 1997
 *
 * Version:       MTObex 3.5
 *
 * Copyright 1997-2006 Extended Systems, Inc.
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

#include "xatypes.h"
#include "obheader.h"
#include "sys/obstkif.h"


/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*--------------------------------------------------------------------------
 *
 * Length of an OBEX connect packet and a connect response packet. 
 */
#define OB_CONNECT_LEN      7

/*--------------------------------------------------------------------------
 *
 * Default OBEX Packet Length for connect-less operations
 */
#define OBEX_DEFAULT_PACKET_SIZE    255

/*--------------------------------------------------------------------------
 *
 * OBEX Parser Events
 */
typedef U8 ObexParserEvent;

#define OBPE_NO_EVENT           0x00
#define OBPE_PUT_HEADER         0x01
#define OBPE_WRITE_HEADER       0x02
#define OBPE_OPER_IND_START     0x03
#define OBPE_OPER_IND           0x04
#define OBPE_DISCONNECT         0x05
#define OBPE_OPER_COMPLETE      0x06
#define OBPE_BUILD_RESPONSE     0x07    /* Used internally by observer */
#if (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED)
#define OBPE_UNACKED_OPER_IND   0x08
#define OBPE_SEND_COMMAND       0x09
#endif /* (IRDA_STACK == XA_ENABLED) && (IR_SIMPLE == IR_ENABLED) */


typedef struct _ObexParser ObexParser;

/*--------------------------------------------------------------------------
 *
 * Event Indication function. The parser generates events for the command
 * interpreter layer.
 */
typedef void (*ObEventInd)(ObexAppHandle *AppHndl, ObexParserEvent event);

/*--------------------------------------------------------------------------
 *
 * OBEX HeaderBlock - Header block specifies the header information to
 * be sent in an OBEX packet. This type can be set to anything as needed
 * by the command interpreter, from a pointer to a structure to a U8
 * integer.
 */
typedef U8 ObHeaderBlock;

/*--------------------------------------------------------------------------
 *
 * OBEX Parser flags
 */
typedef U8 ObexParserFlags;

#define OBPF_CLIENT     0x01  /* Client Parser Structure */
#define OBPF_SERVER     0x02  /* Server Parser Structure */
#define OBPF_CONNECTED  0x04  /* OBEX Connect Packet was received */
#define OBPF_PENDING    0x08  /* A Send[Response/Command] request is pending
                               * because no transmit buffers are available. */
#define OBPF_DEFER_OPER_IND 0x10  /* Indicate OPER_IND after first response is sent. */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/*--------------------------------------------------------------------------
 *
 * OBEX Parser. This structure encapsulates an OBEX packet parser. One
 * is needed for the server and one for the client.
 *
 * The parser structures are encapsulated in the Obex...Context structures 
 * used by the command interpreter layer.
 */

struct _ObexParser {
    U8                stageBuff[4];  /* Staging buffer */
    U8                *rxBuff;       /* Pointer to receive buffer */
    U16               rxLen;         /* Length of unparsed data in rxBuff */
    U16               headerLen;     /* Length of current header */
    U16               totalHeaderLen;/* Total length of current header */
    U16               maxTxPacket;   /* Maximum size packet that can be sent */
    U16               packetLen;     /* Size of current OBEX packet */
    U16               curPacketLen;  /* Amount of data received in current packet */
    U16               dataLen;       /* Length of data */
    struct _ObexTransport *stack;    /* Transport handle */
    ObEventInd        eventIndFunc;  /* Pointer to event handler function */
    ObHeaderBlock     handle;        /* Command interpreter, header TX state */
    ObexRespCode      rsp;           /* Current response code */
    ObexOpcode        opcode;        /* Current opcode (command) */
    ObexHeaderType    header;        /* Current header type */
    U8                rxState;       /* Receive state */
    U8                stageLen;      /* Number of bytes to stage */
    U8                curStageOff;   /* Number of bytes staged so far */
    U8                obexVersion;   /* Version of OBEX supported by other device */
    ObexParserFlags   flags;         /* Flags used by the Parser */
    ObexSetPathFlags  spFlags;       /* Flags sent/rcvd with the SetPath Opcode */
    BOOL              abort;         /* State of OBEX Abort */
};


/****************************************************************************
 *
 * OBEX Client Parser API Function reference
 *     Calls made into the client by the command interpreter.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ObParserSetAbort(ObexParser *obp)
 *
 * Description:   Set the parser into immediate client abort mode.
 *
 * Parameters:    obp    - Obex parser structure.
 *
 * Returns:       void
 */
void ObParserSetAbort(ObexParser *obp);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ObParserSetModeClient(ObexParser obp, ObEventInd cbFunc)
 *
 * Description:   Set the parser into client parser mode.
 *
 * Parameters:    obp    - Obex parser structure.
 *                cbFunc - The client command interpreter callback function.
 *
 * Returns:       void
 */
#define ObParserSetModeClient(obp, cbFunc) do {   \
                (obp).flags |= OBPF_CLIENT;       \
                (obp).eventIndFunc = (cbFunc); } while (0)

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObStatus ObClientSendCommand(ObexAppHandle *AppHndl, 
 *                                             ObexOpcode cmd, 
 *                                             ObHeaderBlock hb)
 *
 * Description:   Send an client OBEX command with optional headers. If the 
 *                command takes multiple OBEX packets then cmd should not have 
 *                the final bit set until the last packet. This function is
 *                called to initiate a command and to send additional
 *                packets in a multi packet command.
 *
 * Parameters:    AppHndl - Application handle provided during initialization.
 *
 *                cmd - opcode of command. If final bit is set then this is 
 *                the last OBEX packet and the last header block.
 *
 *                hb - Handle of header block to send. If 0 there is
 *                no header block.
 *
 * Returns:       OBSTACK_SendTxBuffer() response codes.
 */
ObStatus ObClientSendCommand(ObexAppHandle *AppHndl, ObexOpcode cmd, 
                             ObHeaderBlock hb);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ObClientDisconnect(void)
 *
 * Description:   Disconnect the Tiny TP connection on which the client OBEX 
 *                connection runs. In JetBeam systems without Tiny TP
 *                disconnect this will call IrLAP disconnect.
 *
 * Parameters:    none
 *
 * Returns:       void
 */
#define ObClientDisconnect() ObParserDisconnect(&OCH(parser))

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ObClientSetPathFlags(ObexSetPathFlags flags)
 *
 * Description:   Set the client's SetPath flags that will be sent in a 
 *                SET_PATH command.
 *
 * Parameters:    flags - the flags to send 
 *                
 * Returns:       void
 */
#define ObClientSetPathFlags(flags) (OCH(parser.spFlags) = flags)

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObClientMaxHeaderBlockSize(ObexOpcode operation)
 *
 * Description:   Return the maximum size allowed for a header block not 
 *                including header blocks sent in a connect command. 
 *
 * Parameters:    operation - the Obex opcode
 *
 * Returns:       Maximum header block size allowed
 */

#define ObClientMaxHeaderBlockSize(operation) \
             ObParserMaxHbSize(&OCH(parser), operation)

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexOpcode ObClientGetOpcode(void)
 *
 * Description:   Get the opcode of the current OBEX packet being sent by the
 *                client. This macro is only valid during calls to 
 *                ObClientEventInd(), ObReadHeaderBlock() and 
 *                ObGetHeaderBlockLen().
 *
 * Parameters:    none
 *
 * Returns:       Current OBEX packet opcode
 */
#define ObClientGetOpcode() (OCH(parser.opcode))

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexHeaderType ObClientGetHeaderType(void)
 *
 * Description:   Get the header type of the current header being parsed 
 *                from the current received buffer. This macro is valid only 
 *                during a call to ObClientEventInd() with the 
 *                OBPE_WRITE_HEADER or OBPE_PUT_HEADER events.
 *
 * Parameters:    none
 *
 * Returns:       Current OBEX header type
 */
#define ObClientGetHeaderType() (OCH(parser.header))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObClientGetHeaderLen(void)
 *
 * Description:   Get the total length of a header not including the type and
 *                length bytes themselves. Only for header types that have
 *                a length field. This macro is valid only valid during 
 *                ObClientEventInd() with the OBPE_PUT/WRITE_HEADER event.
 *
 * Parameters:    none
 *
 * Returns:       Total length of header.
 */
#define ObClientGetHeaderLen() (OCH(parser.totalHeaderLen))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObClientGetRemainHeaderLen(void)
 *
 * Description:   Get the remaining length of a header not including the type
 *                and length bytes themselves. Only for header types that have
 *                a length field. This macro is valid only valid during 
 *                ObClientEventInd() with the OBPE_PUT/WRITE_HEADER event.
 *
 * Parameters:    none
 *
 * Returns:       Remaining (unindicated) length of header.
 */
#define ObClientGetRemainHeaderLen() (OCH(parser.headerLen))

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode ObClientGetResponse(void)
 *
 * Description:   Get the response code of the current received OBEX packet. 
 *                This macro is valid only during calls to ObClientEventInd(), 
 *                ObReadHeaderBlock() and ObGetHeaderBlockLen().
 *
 * Parameters:    none
 *
 * Returns:       Current OBEX response code
 */
#define ObClientGetResponse() (OCH(parser.rsp))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U8* ObClientGetHeaderBuff(void)
 *
 * Description:   Get the buffer pointing to the data being written when
 *                ObClientEventInd is called with the OBPE_WRITE_HEADER event.
 *                This macro is only valid during this time.
 *
 * Parameters:    none
 *
 * Returns:       Pointer to buffer containing header
 */
#define ObClientGetHeaderBuff() (OCH(parser.rxBuff))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObClientGetHeaderWriteLen(void)
 *
 * Description:   Get the length of data that is being written when 
 *                ObClientEventInd() is called with the OBPE_WRITE_HEADER 
 *                event.This macro is only valid during that time.
 *
 * Parameters:    none
 *
 * Returns:       Length of header being written.
 */
#define ObClientGetHeaderWriteLen() (OCH(parser.dataLen))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 ObClientGetHeaderInt32(void)
 *
 * Description:   If the header type is a 32 bit integer then this macro
 *                should be used to retrieve it. It is only valid during
 *                ObClientEventInd() with the OBPE_PUT_HEADER event.
 *
 * Parameters:    none
 *
 * Returns:       32-bit integer.
 */
#define ObClientGetHeaderInt32() BEtoHost32(OCH(parser.stageBuff))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U8 ObClientGetHeaderByte(void)
 *
 * Description:   If the header type is a byte then this macro should
 *                be used to retrieve it. It is only valid during
 *                ObClientEventInd() with the OBPE_PUT_HEADER event.
 *
 * Parameters:    none
 *
 * Returns:       byte
 */
#define ObClientGetHeaderByte() (OCH(parser.stageBuff[0]))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U8 ObClientUpdateHeaderBlock(ObHeaderBlock hb)
 *
 * Description:   Updates the header block value stored in the parser.
 *
 * Parameters:    hb - new header block value
 *
 * Returns:       void
 */
#define ObClientUpdateHeaderBlock(hb) (OCH(parser.handle) = hb)

/****************************************************************************
 *
 * OBEX Server Parser API Function reference
 *     Calls made by the command interpreter to the Server parser.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ObParserSetModeServer(ObexParser obp, ObEventInd cbFunc)
 *
 * Description:   Set the parser into server parser mode.
 *
 * Parameters:    _OBP    - Obex parser structure.
 *                _CBFUNC - The server command interpreter callback function.
 *
 * Returns:       void
 */
#define ObParserSetModeServer(obp, cbFunc)  do {  \
            (obp).flags |= OBPF_SERVER;             \
            (obp).eventIndFunc = (cbFunc); } while (0)

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObStatus ObServerSendResponse(ObexAppHandle *AppHndl,
 *                                            ObexRespCode rsp, 
 *                                            ObHeaderBlock hb)
 *
 * Description:   Send a server OBEX response packet.
 *
 * Parameters:    AppHndl - Application handle provided during initialization.
 *
 *                rsp - response to send including final bit. If final
 *                bit is set then this is the last OBEX packet and the
 *                last header block.
 *
 *                hb - Handle of header block to send. If 0 there is
 *                no header block.
 *
 * Returns:       OBSTACK_SendTxBuffer() response codes.
 */
ObStatus ObServerSendResponse(ObexAppHandle *AppHndl, ObexRespCode rsp, ObHeaderBlock hb);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ObServerDisconnect(void)
 *
 * Description:   Disconnect the server OBEX connection.
 *
 * Parameters:    none
 *
 * Returns:       void
 */
#define ObServerDisconnect() ObParserDisconnect(&OSH(parser));

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObServerMaxHeaderBlockSize(void)
 *
 * Description:   Return the maximum size allowed for a header block not 
 *                including header blocks sent with a response to a connect 
 *                command. This macro is only valid during calls to 
 *                ObServEventInd(), ObReadHeaderBlock() and 
 *                ObGetHeaderBlockLen().
 *
 * Parameters:    none
 *
 * Returns:       Maximum header block size allowed
 */
#define ObServerMaxHeaderBlockSize() \
            ObParserMaxHbSize(&OSH(parser), OSH(parser.opcode))

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexOpcode ObServerGetOpcode(void)
 *
 * Description:   Get the opcode of the current received OBEX packet. This 
 *                macro is only valid during calls to ObServEventInd(), 
 *                ObReadHeaderBlock() and ObGetHeaderBlockLen().
 *
 * Parameters:    none
 *
 * Returns:       Current OBEX packet opcode
 */
#define ObServerGetOpcode() (OSH(parser.opcode))

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexRespCode ObServerGetResponse(void)
 *
 * Description:   Get the response code of the current OBEX packet being 
 *                sent.This macro is only valid during calls to 
 *                ObServEventInd(), ObReadHeaderBlock() and 
 *                ObGetHeaderBlockLen().
 *
 * Parameters:    none
 *
 * Returns:       Current OBEX response code
 */
#define ObServerGetResponse() (OSH(parser.rsp))

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexHeaderType ObServerGetHeaderType(void)
 *
 * Description:   Get the header type of the currently received OBEX packet. 
 *                This macro is only valid during calls to 
 *                ObServerEventInd(), ObReadHeaderBlock() and 
 *                ObGetHeaderBlockLen().
 *
 * Parameters:    none
 *
 * Returns:       Current OBEX header type
 */
#define ObServerGetHeaderType() (OSH(parser.header))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObServerGetHeaderLen(void)
 *
 * Description:   Get the total length of header not including the type and
 *                length bytes themselves. Only for header types that have
 *                a length field. This macro is valid only valid during 
 *                ObServerEventInd() with the OBPE_PUT/WRITE_HEADER event.
 *
 * Parameters:    none
 *
 * Returns:       Total length of header.
 */
#define ObServerGetHeaderLen() (OSH(parser.totalHeaderLen))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObServerGetRemainHeaderLen(void)
 *
 * Description:   Get the remaining length of header not including the type
 *                and length bytes themselves. Only for header types that
 *                have a length field. This macro is valid only valid during 
 *                ObServerEventInd() with the OBPE_PUT/WRITE_HEADER event.
 *
 * Parameters:    none
 *
 * Returns:       Remaining (unindicated) length of header.
 */
#define ObServerGetRemainHeaderLen() (OSH(parser.headerLen))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObServerGetHeaderWriteLen(void)
 *
 * Description:   Get the length of data that is being written when 
 *                ObClientEventInd() is called with the OBPE_WRITE_HEADER 
 *                event.This macro is only valid during that time.
 *
 * Parameters:    none
 *
 * Returns:       Length of header to write
 */
#define ObServerGetHeaderWriteLen() (OSH(parser.dataLen))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U8* ObServerGetHeaderBuff(void)
 *
 * Description:   Get the buffer pointing to the data being written when
 *                ObClientEventInd is called with the OBPE_WRITE_HEADER event.
 *                This macro is only valid during this time.
 *
 * Parameters:    none
 *
 * Returns:       Pointer to buffer containing header
 */
#define ObServerGetHeaderBuff() (OSH(parser.rxBuff))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U32 ObServerGetHeaderInt32(void)
 *
 * Description:   If the header type is a 32 bit integer then this macro
 *                should be used to retrieve it. It is only valid during
 *                ObServerEventInd() with the OBPE_PUT_HEADER event.
 *
 * Parameters:    none
 *
 * Returns:       32-bit integer.
 */
#define ObServerGetHeaderInt32() BEtoHost32(OSH(parser.stageBuff))

/*---------------------------------------------------------------------------
 *
 * Prototype:     U8 ObServerGetHeaderByte(void)
 *
 * Description:   If the header type is a byte then this macro should
 *                be used to retrieve it. It is only valid during
 *                ObServerEventInd() with the OBPE_PUT_HEADER event.
 *
 * Parameters:    none
 *
 * Returns:       byte
 */
#define ObServerGetHeaderByte() (OSH(parser.stageBuff[0]))

/*---------------------------------------------------------------------------
 *
 * Prototype:     ObexSetPathFlags ObServerGetPathFlags(void)
 *
 * Description:   Get the SetPath flags that were sent with the SET_PATH
 *                command. It is only valid during ObServerEventInd().
 *
 * Parameters:    none 
 *
 * Returns:       flags - the flags to send
 */
#define ObServerGetPathFlags() (OSH(parser.spFlags))

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ObServerSetParserEventIndFunc(ObEventInd function)
 *
 * Description:   Register the Command Interpreter Event Indication function
 *                for server event notification from the Packet Parser Component. 
 *
 * Parameters:    function - Server Command Interpreter event notification function
 *
 * Returns:       void 
 */

/****************************************************************************
 *
 * Macros used internally by the OBEX Parser.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL ObParserUnlinkTransport(ObexParser *obp)
 *
 * Description:   Unlinks the transport for the Obex parser.
 *
 * Parameters:    obp - Pointer to the Obex parser.
 *
 * Returns:       void
 */
#define ObParserUnlinkTransport(obp) (obp)->stack = 0

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL IsServerParser(ObexParser *obp)
 *
 * Description:   Determine if the parser is a server or not.
 *
 * Parameters:    obp - OBEX parser structure pointer
 *
 * Returns:       TRUE -  Parser structure is for a server parser.
 *                FALSE - Parser structure is for a client parser.
 */
#define IsServerParser(obp) ((obp)->flags & OBPF_SERVER ? TRUE : FALSE)

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL IsClientParser(ObexParser *obp)
 *
 * Description:   Determine if the parser is a client or not.
 *
 * Parameters:    obp - OBEX parser structure pointer
 *
 * Returns:       TRUE -  Parser structure is for a client parser.
 *                FALSE - Parser structure is for a server parser.
 */
#define IsClientParser(obp) ((obp)->flags & OBPF_CLIENT ? TRUE : FALSE)

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObParserDefaultObexPacketSize(void)
 *
 * Description:   Return the default value for the OBEX Packet Size.
 *
 * Parameters:    none
 *
 * Returns:       Default OBEX packet size.
 */
#define ObParserDefaultObexPacketSize() OBEX_DEFAULT_PACKET_SIZE

/****************************************************************************
 *
 * Types used internally by the OBEX Parser.
 *
 ****************************************************************************/

/* States for the OBEX Server and Client Parsers */

#define OBSC_RX_NOP       0 /* Ignore received data */
#define OBSC_RX_WAIT      1 /* Awaiting new command packet */
#define OBSC_RX_WAIT2     2 /* Awaiting continue command packet */
#define OBSC_RX_WAIT3     3 /* Response packet sent (Server only) */
#define OBSC_RX_PUT_HEAD1 4 /* Staged header type */
#define OBSC_RX_PUT_HEAD2 5 /* Staged header length or value for byte quantity headers */
#define OBSC_RX_PUT_HEAD3 6 /* rxBuff points to header data to copy out */
#define OBSC_RX_PATH      7 /* Staged SetPath command flags & constants */
#define OBSC_RX_CONNECT   8 /* Staged Connect command options */

/****************************************************************************
 *
 * Function prototypes used internally by the OBEX Driver.
 *
 ****************************************************************************/

void ObParseRx(ObexAppHandle *AppHndl, U8* rxBuff, U16 rxLen);
void ObParserDisconnectInd(ObexAppHandle *AppHndl);
void ObParserTxBuffAvail(ObexAppHandle *AppHndl);
void ObParserTxDone(ObexAppHandle *AppHndl);

void ObParserSetInitState(ObexParser *obp);
void ObParserDisconnect(ObexParser *obp);
U16  ObParserMaxHbSize(ObexParser *obp, ObexOpcode opcode);
BOOL ObParserLinkTransport(ObexParser *obp, struct _ObexTransport *trans);

#endif /* __OBPARSE_H */

