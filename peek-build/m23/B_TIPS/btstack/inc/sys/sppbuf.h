/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:		sppbuf.h
*
*   DESCRIPTION:	Type definitions and function prototypes for the
*					sppbuf.c module implementing interim buffers for SPP profile.
*
*   AUTHOR:			V. Abram
*
\*******************************************************************************/
#ifndef __SPPBUF_H
#define __SPPBUF_H

#include "bttypes.h"
#include "btconfig.h"
#include "osapi.h"
#include "spp.h"
#include "ring.h"


/*---------------------------------------------------------------------------
 * Serial Port Profile Configuration API layer
 *
 *     The constants in this layer provide general configuration to
 *     communication protocol stacks compiled into the system. To change
 *     a constant, simply #define it in your overide.h include file.
 *
 *     Configuration constants here and in other configuration API files
 *     are used to make the stack more appropriate for a particular
 *     environment. Constants can be modified to allow tradeoffs
 *     between code size, RAM usage, functionality, and throughput.
 *
 *     Some constants are numeric, and others indicate whether a feature
 *     is enabled (defined as XA_ENABLED) or disabled (defined as
 *     XA_DISABLED).
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/* Represents usage of SPPBUF device */
#define SPPBUF_DEV_FREE					(0)
#define SPPBUF_DEV_IN_USE				(1)

#define SPPBUF_FLOW_DATA_COMBINED	    (XA_ENABLED)

/*
 *  Ring buffer sizes.
 *  127 is the default RFCOMM buffer size. For tx, we'll arbitrarily make
 *  it 3x.
 */
#if SPP_USE_INTERIM_RX_BUFFER == BTL_CONFIG_ENABLED

#define SPPBUF_RX_BUF_SIZE				(RF_MAX_FRAME_SIZE * BTL_CONFIG_SPP_SIZE_OF_INTERIM_RX_BUFFER)

#endif /* SPP_USE_INTERIM_RX_BUFFER == XA_ENABLED */

#if SPP_USE_INTERIM_TX_BUFFER == BTL_CONFIG_ENABLED

#if SPPBUF_FLOW_DATA_COMBINED == XA_ENABLED
#define SPPBUF_TX_BUF_SIZE				((RF_MAX_FRAME_SIZE -1) * BTL_CONFIG_SPP_SIZE_OF_INTERIM_TX_BUFFER)
#elif
#define SPPBUF_TX_BUF_SIZE				((RF_MAX_FRAME_SIZE) * BTL_CONFIG_SPP_SIZE_OF_INTERIM_TX_BUFFER)
#endif /* SPPBUF_FLOW_DATA_COMBINED == XA_ENABLED */

#endif /* SPP_USE_INTERIM_TX_BUFFER == XA_ENABLED */


 /****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*----------------------------------------------------------------------
 * SppBuf type
 *      Serial Port Profile Device structure for Windows implementation.
 *      This structure contains all the fields required for a serial 
 *      device as implemented by the Serial Port Profile. The first member
 *      MUST be of type SppDev. Any other members are for use by 
 *      implementation or OS-specific code.
 */
#if (TI_CHANGES == XA_ENABLED)
struct _SppBuf
#else
typedef struct _SppBuf
#endif
{
	BOOL 		inUse;			/* Structure is in use or free */
	
#if SPP_USE_INTERIM_TX_BUFFER == XA_ENABLED

	/* Control structure and actual write buffers */
    RingBuf		ringWBuf;
	U8			wBuffer[SPPBUF_TX_BUF_SIZE];
	
#endif /* #if SPP_USE_INTERIM_TX_BUFFER == XA_ENABLED */

#if SPP_USE_INTERIM_RX_BUFFER == XA_ENABLED

	/* Control structure and actual read buffers */
	RingBuf		ringRBuf;
	U8			rBuffer[SPPBUF_RX_BUF_SIZE];
	
#endif /* #if SPP_USE_INTERIM_RX_BUFFER == XA_ENABLED */

#if (TI_CHANGES == XA_ENABLED)
} ;
#else
} SppBuf;
#endif

/****************************************************************************
 *
 * Function Prototypes
 *
 ****************************************************************************/
/****************************************************************************
 * 
 * Section: OS-Provided APIs
 *
 * The following functions are OS-specific in how they are implemented. 
 * The Serial Port Profile calls them in order to carry out its tasks.
 * The implementer needs to port these functions to the platform.
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * SPPBUF_Init() 
 *		Initializes SPPBUF module.
 *
 * Parameters:	
 *		None.
 *
 * Returns:
 *		None.
 */
void SPPBUF_Init(void);

/*---------------------------------------------------------------------------
 * SPPBUF_InitPortBufs() 
 *		Initializes a new SppBuf structure.
 *
 * Parameters:
 *      dataPathTypes [in] - types of Rx and Tx data paths for the port.
 *
 *		sppBuf [out] - pointer to SppBuf structure of the initialized device.
 *
 *      maxNumOfRxAsyncBuf [out] - max amount of Rx Async buffers.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the initialization was successful.
 *
 *		BT_STATUS_NO_RESOURCES - if the initialization failed.
 */
BtStatus SPPBUF_InitPortBufs(SppPortDataPathType dataPathTypes,
                             SppBuf **sppBuf,
                             U8 *maxNumOfRxAsyncBuf);

/*---------------------------------------------------------------------------
 * SPPBUF_DeinitPortBufs() 
 *		Deinitializes previously initialized SPPBUF device.
 *
 * Parameters:	
 *		sppBuf - pointer to SppBuf structure of the device to be deinitialized.
 *
 * Returns:
 *		BT_STATUS_SUCCESS - if the deinitialized was successful.
 *
 *		BT_STATUS_FAILED - if the deinitialized failed.
 */
BtStatus SPPBUF_DeinitPortBufs(SppBuf *sppBuf);

/*---------------------------------------------------------------------------
 * SPPBUF_CopyRxDataToBuf() 
 *      Process characters received from the serial device.
 *
 *      This function is called by SPP in function rfCallbackFunc(), when
 *		characters are received from RFCOMM. Currently, ring buffer in module
 *		ring.c is used to copy the data but it may be replaced with another
 *		implementation of application buffers, while keeping in mind 2 rules:
 *		
 *		1. The received data should be copied on the context of the RFCOMM
 *			callback, as the buffer with the received data could not be kept.
 *		2. Until function SPP_Read() is called, no RFCOMM credits will be
 *			advanced. Currently, SPP_Read() function, called from the
 *			application	layer, calls SPPBUF_ReadBuf(), which, actually, reads
 *			data from the ring buffer. In case the received data was already
 *			read in function SPPOS_CopyRxDataToBuf(), function SPPBUF_ReadBuf()
 *			may be left empty.
 *
 *      The function should handle any and all special-character handling such
 *		as software flow control, backspace and end-of-line handling etc.
 *      It is entirely up to the implementer to decide how much special-
 *      character handling is to be done. Typically, this function places
 *      bytes in a ring buffer. If your OS routine handles only a single
 *      character, then this function should call it iteratively for each
 *      byte in the buffer.
 *     
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 *      buf  - buffer of characters received from RFCOMM
 *      len  - number of characters in buf; on successful
 *             return this is set to number of bytes in buf
 *
 * Returns:
 *      BT_STATUS_SUCCESS - *len is the number of bytes written
 *      BT_STATUS_FAILED - error as determined by platform implementation
 */
BtStatus SPPBUF_CopyRxDataToBuf(SppBuf *sppBuf, U8 buf[], U16 *len);

/*---------------------------------------------------------------------------
 * SPPBUF_ReadRxDataFromBuf() 
 *      This function is called by SPP in function SPP_Read(), when the caller
 *		is reading the serial device (RFCOMM).
 *
 *      It should handle any and all special-character handling such as
 *      software flow control, backspace and end-of-line handling etc.
 *      It is entirely up to the implementer to decide how much special-
 *      character handling is to be done. Typically, this function extracts
 *      bytes from a ring buffer. If your OS routine handles only a single
 *      character, then this function should call it iteratively for each
 *      byte returned.
 *     
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 *      buf  - allocated buffer to receive bytes
 *      len [in/out]  - pointer to max number of characters to be put in buf; on
 *             successful return this is set to number of bytes actually read
 *
 * Returns:
 *      BT_STATUS_SUCCESS - len contains actual number of read bytes
 *      BT_STATUS_FAILED - error as determined by platform implementation
 */
BtStatus SPPBUF_ReadRxDataFromBuf(SppBuf *sppBuf, U8 buf[], U16 *len);

/*---------------------------------------------------------------------------
 * SPPBUF_WriteTxDataToBuf() 
 *      Process characters to be transmitted using the serial device.
 *
 *      This function is called by the SPP in function SPP_Write(), when the
 *		caller is writing bytes to the serial device (RFCOMM).
 *      Typically, this function places bytes in a ring buffer. 
 *      If your OS routine handles only a single character, then this 
 *      function should call it iteratively for each byte in the buffer.
 *     
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 *      buf  - buffer of characters received from RFCOMM
 *      len [in/out] - pointer to number of characters in buf; on successful
 *             return this is set to number of bytes actually written
 *
 * Returns:
 *      BT_STATUS_SUCCESS - len contains number of bytes in buf
 *      BT_STATUS_FAILED - error as determined by platform implementation
 */
BtStatus SPPBUF_WriteTxDataToBuf(SppBuf *sppBuf, U8 buf[], U16 *len);

/*---------------------------------------------------------------------------
 * SPPBUF_GetTxDataFromBuf() 
 *      Get the next characters to be transmitted via RFCOMM.
 *
 *      This function is called by SPP in function sendToRfComm(), in order to
 *		retrieve bytes to be sent via RFCOMM. Currently, this routine removes
 *		characters from a ring buffer.
 *
 *      It should handle any and all special-character handling such as
 *      software flow control, backspace and end-of-line handling etc.
 *      It is entirely up to the implementer to decide how much special-
 *      character handling is to be done.
 *     
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 *
 *      buf  - On output, filled with a char pointer full of data. If "len"
 *             is 0, a NULL pointer may be provided. If non-NULL, this pointer
 *             is owned by the SPP layer until SPPBUF_ReturnBuf is called.
 *              
 *      len  - On output, contains the actual number of bytes in "buf".
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Indicates that "buf" contains bytes for transmission.
 *
 *      BT_STATUS_FAILED  - no more characters to send
 */
BtStatus SPPBUF_GetTxDataFromBuf(SppBuf *sppBuf, U8 **buf, U16 *len);

/*---------------------------------------------------------------------------
 * SPPBUF_ReturnTxBuf() 
 *      Notify the application that a Tx buffer is no longer needed and may be
 *		reused or unallocated. The buffer being freed is one that was allocated
 *		by using function SPPBUF_WriteTxDataToBuf().
 *
 *      This function is called by SPP in function rfCallbackFunc(), when
 *		event RFEVENT_PACKET_HANDLED is received from RFCOMM.
 *
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 *      buf  - buffer being returned
 *      len  - length of buf
 */
void SPPBUF_ReturnTxBuf(SppBuf *sppBuf, U8 *buf, U16 len);

/*---------------------------------------------------------------------------
 * SPPBUF_RxBytes() 
 *		Determines the number of bytes received in Rx buffer and available for
 *		reading by application.
 *
 *      This function is called by SPP in function advanceCredit(), when
 *		amount of free room for reading is calculated.
 *
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 *
 * Returns:
 *      max number of bytes to read
 */
U16 SPPBUF_RxBytes(SppBuf *sppBuf);

/*---------------------------------------------------------------------------
 * SPPBUF_TxBytes() 
 *     Determines number of bytes in Tx buffer which still have not been sent.
 *
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 *
 * Returns:
 *      Number of bytes to be sent
 */
U16 SPPBUF_TxBytes(SppBuf *sppBuf);

/*---------------------------------------------------------------------------
 * SPPBUF_RxFree() 
 *		Determines the number of bytes available for receiving bytes from 
 *		the device.
 *
 *		This function is called in SPP in function setHighWaterMark().
 *
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 *
 * Returns:
 *      max number of bytes to read
 */
U16 SPPBUF_RxFree(SppBuf *sppBuf);

/*---------------------------------------------------------------------------
 * SPPBUF_TxFree() 
 *     Determines the number of bytes available for writing into Tx buffer.
 *
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 *
 * Returns:
 *      max number of bytes still waiting to be written
 */
U16 SPPBUF_TxFree(SppBuf *sppBuf);

/*---------------------------------------------------------------------------
 * SPPBUF_FlushRx() 
 *      Flushes all the unread bytes received on the device.
 *
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 */
void SPPBUF_FlushRx(SppBuf *sppBuf);

/*---------------------------------------------------------------------------
 * SPPBUF_FlushTx() 
 *      Flushes all the bytes that have not been transmitted on the device.
 *
 * Parameters:
 *		sppBuf - pointer to SppBuf structure of the device.
 */
void SPPBUF_FlushTx(SppBuf *sppBuf);

#endif /* __SPPBUF_H */
