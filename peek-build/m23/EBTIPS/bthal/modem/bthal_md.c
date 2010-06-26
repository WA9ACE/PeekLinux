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
*   FILE NAME:      bthal_md.c
*
*   DESCRIPTION:    Implementation of BTHAL Modem Data API.
*
*                   Connection to Modem Data service is established via generic
*                   DIO (Device Input/Output) interface which sees connected
*                   party as a device, in our case, as single BT device, and
*                   implementation of BTHAL MD interface - as BT device driver.
*
*                   Names of constants, types, and function names, which relate
*                   to the specific DIO BT driver implementation, start with
*                   prefix DIO_BT.
*
*   AUTHOR:         V. Abram
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <string.h>
#include "typedefs.h"
#include "bthal_types.h"
#include "bthal_log.h"
#include "bthal_md.h"
#include "dio_bt.h"
#include "bthal_log_modules.h"

#define BTHAL_MODEM_LOG_ERROR(msg)		BTHAL_LOG_ERROR(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_MODEM, msg)	
#define BTHAL_MODEM_LOG_INFO(msg)		BTHAL_LOG_INFO(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_MODEM, msg)
#define BTHAL_MODEM_LOG_FUNCTION(msg)	BTHAL_LOG_FUNCTION(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_MODEM, msg)


/********************************************************************************
 *
 * Constants 
 *
 *******************************************************************************/

/* Virtual number of BT port */
#define DIO_BT_PORT_NUMBER              (1)

/* Number of BT ports */
#define DIO_BT_NUM_OF_PORTS             (1)

/* Number of DIO interface buffers */
#define DIO_BT_RX_BUF_NUM	            (2)
#define DIO_BT_TX_BUF_NUM               (2)



/********************************************************************************
 *
 * Internal Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BthalMdState type
 *
 *     Defines the BTHAL MD states.
 */
typedef BTHAL_U8 BthalMdState;

#define BTHAL_MD_STATE_IDLE       	    (0x00)
#define BTHAL_MD_STATE_INITIALIZED   	(0x01)
#define BTHAL_MD_STATE_REGISTERED       (0x02)
#define BTHAL_MD_STATE_CONNECTED		(0x03)

/*-------------------------------------------------------------------------------
 * DioBtDevState type
 *
 *     Defines the states of the DIO BT device.
 */
typedef BTHAL_U8 DioBtDevState;

#define DIO_BT_DEV_STATE_IDLE       	(0x00)
#define DIO_BT_DEV_STATE_CLOSED   	    (0x01)
#define DIO_BT_DEV_STATE_OPEN    	    (0x03)

/********************************************************************************
 *
 * Internal Data Structures
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * DioBtBuf structure
 *
 *	   Represents buffers supplied by the DIO interface.
 */
typedef struct _DioBtBuf
{
	T_dio_buffer	*buf;
	
    /* Counter of bytes */
    U16             cnt;

#if BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED

    /* Amount of data in segments returned with BTHAL_MD_ReturnDownloadBuf() */
    U16             cntRet;

#endif /* BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED */
    
    /* Amount of segments currently passed with BTHAL_MD_GetDownloadBuf() */
    U8              seg;

} DioBtBuf;

/*-------------------------------------------------------------------------------
 * DioBtDev structure
 *
 *	   Represents DIO BT device internal data.
 */
typedef struct _DioBtDev
{
	U8				    state;

	/* Unique ID to a device containing driver name, driver type and private
     * driver number */
    U32				    devID;

	/* The signal callback function provided by DIO BT user (PSI) */
    T_DRV_CB_FUNC	    sigCallback;

	T_DIO_DCB_SER	    dioDcbSpp;	
	T_DIO_CAP_SER	    dioCapSpp;

	SppControlSignals   btControlSignals;
    
    /* BT Control signals to be delivered to the DIO interface with 'dio_read()
     * function */
    U32				    rxLineState;
	
    /* Flag shows that BT control signals were changed */
    BOOL			    rxLineStateChanged;

	U32				    txLineState;
	
    /* Flag shows that modem's control signals were changed */



	DioBtBuf		    rxBuf[DIO_BT_RX_BUF_NUM];
	DioBtBuf		    txBuf[DIO_BT_TX_BUF_NUM];

	/* This is the handle given by the upper layer */
    U16				    drvHandle;

	U8				    rxGetBufIdx;
	U8				    rxReadBufIdx;
	U8				    rxNewBufIdx;

	U8				    txGetBufIdx;
	U8				    txReadBufIdx;
	U8				    txNewBufIdx;
} DioBtDev;


/*-------------------------------------------------------------------------------
 * BthalMdContext structure
 *
 *	   Represents BTHAL MD internal data used by an implementer of the specific
 *	   modem data interface.
 */
typedef struct _BthalMdContext
{
    /* State of the MD */
    BTHAL_U8        state;

    /* Should be at the beginning in order its state to be initialized to
     * DIO_BT_DEV_STATE_IDLE */
	DioBtDev        dioBtDev;

    /* Callback for delivering BTHAL MD events */
	BthalMdCallback mdCallback;

    /* Communication settings */
    SppComSettings  comSettings;
	
} BthalMdContext;


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/

static void BthalMdSetState(BTHAL_U8 state);
static const char *BthalMdStateName(void);

/*-------------------------------------------------------------------------------
 * DIO defined functions
 *
 *      Functions defined by DIO interface in file dio_drv.h and which are
 *      implemented in current module.
 */
void dio_user_ready_bt(U32 deviceRangeStart,
                       U32 deviceRangeEnd,
                       U16 drvHandle,
                       T_DRV_CB_FUNC sigCallback);
U16 dio_user_not_ready_bt(U32 deviceRangeStart, U32 deviceRangeEnd);
U16 dio_get_capabilities_bt(U32 device, T_DIO_CAP **cap);
U16 dio_get_config_bt(U32 device, T_DIO_DCB *dcb);
U16 dio_set_config_bt(U32 device, T_DIO_DCB *dcb);
U16 dio_set_rx_buffer_bt(U32 device, T_dio_buffer *buffer);
U16 dio_read_bt(U32 device, T_DIO_CTRL *controlInfo, T_dio_buffer **buffer);
void dio_exit_bt(void);
U16 dio_write_bt(U32 device, T_DIO_CTRL *controlInfo, T_dio_buffer *buffer);
U16 dio_get_tx_buffer_bt(U32 device, T_dio_buffer **buffer);
U16 dio_clear_bt(U32 device);
U16 dio_flush_bt(U32 device);
U16 dio_close_device_bt(U32 device);


/*-------------------------------------------------------------------------------
 * Internal functions-utilities used for DIO implementation
 */
U16 DioBtIsDevOpen(U32 device);
void DioBtSendSignal(USHORT signal);
U32 dioConvertSppToDioLineState(int btStatus, BOOL escSequence);
int dioConvertDioToSppLineState(U32 dioLineState);


/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/* BTHAL MD context with internal data */
BthalMdContext bthalMdContext = {BTHAL_MD_STATE_IDLE, {DIO_BT_DEV_STATE_IDLE}};

/* Functions which are exported to DIO interface by DIO BT driver */
static T_DIO_FUNC dio_func_bt =
 {
    dio_user_ready_bt,
    dio_user_not_ready_bt,
    dio_exit_bt,
    dio_set_rx_buffer_bt,
    dio_read_bt,
    dio_write_bt,
    dio_get_tx_buffer_bt,
    dio_clear_bt,
    dio_flush_bt,
    dio_get_capabilities_bt,
    dio_set_config_bt,
    dio_get_config_bt,
    dio_close_device_bt
 };


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/
/*-------------------------------------------------------------------------------
 * BTHAL_MD_Init()
 *
 *	   	Initializes modem's data service to be used by caller application.
 */
BthalStatus BTHAL_MD_Init(BthalCallBack	callback)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_Init"));

    if (bthalMdContext.state >= BTHAL_MD_STATE_INITIALIZED)
    {
	    BTHAL_MODEM_LOG_ERROR(("BTHAL MD module is already initialized"));
        return (BTHAL_STATUS_FAILED);
    }
    
    BthalMdSetState(BTHAL_MD_STATE_INITIALIZED);
    return (BTHAL_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BTHAL_MD_Deinit()
 *
 *	   	Releases modem's data service and resources which were used by caller
 *		application.
 */
BthalStatus BTHAL_MD_Deinit(void)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_Deinit"));

    if (bthalMdContext.state < BTHAL_MD_STATE_INITIALIZED)
    {
	    BTHAL_MODEM_LOG_ERROR(("BTHAL MD module is already deinitialized"));
        return (BTHAL_STATUS_FAILED);
    }

    BthalMdSetState(BTHAL_MD_STATE_IDLE);
    return (BTHAL_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BTHAL_MD_Register()
 *
 *	   	Registers with modem's data service to be used by caller application.
 */
BthalStatus BTHAL_MD_Register(const BthalMdCallback mdCallback,
						      const SppComSettings *comSettings)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_Register"));

    if (bthalMdContext.state >= BTHAL_MD_STATE_REGISTERED)
    {
	    BTHAL_MODEM_LOG_ERROR(("BTHAL MD module is already registered"));
        return (BTHAL_STATUS_FAILED);
    }

    /* Save callback function and configuration settings and set new state */
    bthalMdContext.mdCallback = mdCallback;
    bthalMdContext.comSettings = *comSettings;
    BthalMdSetState(BTHAL_MD_STATE_REGISTERED);

    return (BTHAL_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BTHAL_MD_Deregister()
 *
 *	   	Deregisters from the modem's data service.
 */
BthalStatus BTHAL_MD_Deregister()
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_Deregister"));

    if (bthalMdContext.state < BTHAL_MD_STATE_REGISTERED)
    {
	    BTHAL_MODEM_LOG_ERROR(("BTHAL MD module is already deregistered"));
        return (BTHAL_STATUS_FAILED);
    }

    /* Clear callback function and configuration settings and set new state */
    bthalMdContext.mdCallback = NULL;
    memset((void *)&bthalMdContext.comSettings,
           0,
           sizeof(bthalMdContext.comSettings));
    BthalMdSetState(BTHAL_MD_STATE_INITIALIZED);

    return (BTHAL_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BTHAL_MD_Configure()
 */
BthalStatus BTHAL_MD_Configure(const SppComSettings *comSettings)
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_Configure"));

    if (bthalMdContext.state < BTHAL_MD_STATE_REGISTERED)
    {
	    BTHAL_MODEM_LOG_ERROR(("BTHAL MD module is not registered"));
        return (BTHAL_STATUS_FAILED);
    }
    
    /* Save configuration settings */
    bthalMdContext.comSettings = *comSettings;

    return (BTHAL_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BTHAL_MD_Connect()
 *
 *     Connects to modem's data service.
 */
BthalStatus BTHAL_MD_Connect()
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_Connect"));
	
	dioBtDev->txNewBufIdx   = 0;
	dioBtDev->txReadBufIdx  = 0;
	dioBtDev->txGetBufIdx   = 0;
	dioBtDev->txLineState   = 0;



	dioBtDev->rxNewBufIdx   = 0;
	dioBtDev->rxReadBufIdx  = 0;
	dioBtDev->rxGetBufIdx   = 0;
	dioBtDev->rxLineState   = 0;
	dioBtDev->rxLineStateChanged = FALSE;

    /* Check, if MD is initialized */
    if (bthalMdContext.state < BTHAL_MD_STATE_INITIALIZED)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD is not initialized"));
        return (BTHAL_STATUS_FAILED);
    }

    /* Check, whether MD is not already connected */
    if (bthalMdContext.state >= BTHAL_MD_STATE_CONNECTED)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD is already connected"));
        return (BTHAL_STATUS_FAILED);
    }
    
    /* Check if DIO BT device is initialized */
    if (dioBtDev->state < DIO_BT_DEV_STATE_CLOSED)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD: DIO BT device is not initialized"));
        return (BTHAL_STATUS_FAILED);
    }

	/* Send CONNECT signal to DIO interface to open DIO BT device and set a new
     * state */
	DioBtSendSignal(DRV_SIGTYPE_CONNECT);
    BthalMdSetState(BTHAL_MD_STATE_CONNECTED);

    return (BTHAL_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BTHAL_MD_Disconnect()
 *
 *     Disconnects from modem's data service.
 */
BthalStatus BTHAL_MD_Disconnect()
{
	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_Disconnect"));

    /* Check, if MD is connected */
    if (bthalMdContext.state < BTHAL_MD_STATE_CONNECTED)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD is not connected"));
        return (BTHAL_STATUS_FAILED);
    }
    
    /* Send DISCONNECT signal to DIO interface to close DIO BT device */
    DioBtSendSignal(DRV_SIGTYPE_DISCONNECT);

    /* Set new state */
    BthalMdSetState(BTHAL_MD_STATE_REGISTERED);
        
    return (BTHAL_STATUS_SUCCESS);
}

#if BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED
 /*-------------------------------------------------------------------------------
 * BTHAL_MD_GetDownloadBuf()
 *
 *     Gets pointer to buffer with downloaded data and its length.
 */
BthalStatus BTHAL_MD_GetDownloadBuf(BTHAL_U8 **buffer, BTHAL_INT *len)
{
    T_dio_segment *dioSeg;
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;
	DioBtBuf *dioTxBuf = &dioBtDev->txBuf[dioBtDev->txReadBufIdx];

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_GetDownloadBuf"));

    /* Check, if MD is connected */
    if (bthalMdContext.state < BTHAL_MD_STATE_CONNECTED)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD is not connected"));
        return (BTHAL_STATUS_FAILED);
    }

    /* Check whether there is any unread downloaded data */
    if ((NULL == dioTxBuf->buf) ||
        (dioTxBuf->cnt == dioTxBuf->buf->length) ||
	(dioTxBuf->seg == dioTxBuf->buf->c_dio_segment))
    {
	    BTHAL_MODEM_LOG_ERROR(("BTHAL MD: no more downloaded data to copy"));
        return (BTHAL_STATUS_FAILED);
    }

    /* Current segment of the data buffer */
    dioSeg = (T_dio_segment *)&dioTxBuf->buf->ptr_dio_segment[dioTxBuf->seg];

    /* Fill return values: pointer to data buffer and its length */
    *buffer = (BTHAL_U8 *)dioSeg->ptr_data;
    *len = (BTHAL_INT)dioSeg->c_data;

    /* Update length of passed data in the current buffer */
    dioTxBuf->cnt += dioSeg->c_data;

    /* Increment amount of segments passed with this function */
    dioTxBuf->seg++;

    return (BTHAL_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BTHAL_MD_ReturnDownloadBuf()
 *
 *     Returns buffer to modem after downloaded data was copied from it.
 */
BthalStatus BTHAL_MD_ReturnDownloadBuf(BTHAL_U8 *buffer, BTHAL_INT len)
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;
	DioBtBuf *dioTxBuf = &dioBtDev->txBuf[dioBtDev->txReadBufIdx];

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_ReturnDownloadBuf"));

    /* Check, if MD is connected */
    if (bthalMdContext.state < BTHAL_MD_STATE_CONNECTED)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD is not connected"));
        return (BTHAL_STATUS_FAILED);
    }

    /* Update amount of data existed in returned segments */
    dioTxBuf->cntRet += len;

    /* Check whether all buffer's segments were returned */
    if (dioTxBuf->cntRet == dioTxBuf->buf->length)
    {
        /* Increase the new TX read buffer's index */
        if (DIO_BT_TX_BUF_NUM == ++dioBtDev->txReadBufIdx)
        {
            dioBtDev->txReadBufIdx = 0;
        }

        /* Send signal to DIO interface that buffer is processed */
        DioBtSendSignal(DRV_SIGTYPE_WRITE);
    }
    
    return (BTHAL_STATUS_SUCCESS);
}
#endif /* BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED */

#if BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED
/*-------------------------------------------------------------------------------
 * BTHAL_MD_GetUploadBuf()
 *
 *     Gets pointer to buffer and its size for data to be uploaded.
 */
BthalStatus BTHAL_MD_GetUploadBuf(BTHAL_INT size, BTHAL_U8 **buffer, BTHAL_INT *len)
{
    T_dio_segment *dioSeg;
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;
	DioBtBuf *dioRxBuf = &dioBtDev->rxBuf[dioBtDev->rxReadBufIdx];

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_GetUploadBuf"));

    /* Check, if MD is connected */
    if (bthalMdContext.state < BTHAL_MD_STATE_CONNECTED)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD is not connected"));
        return (BTHAL_STATUS_FAILED);
    }

    /* Check whether there is any free buffer for uploading data. Note that
     * we send buffer to modem with any amount of data, if reading from BT is
     * finished. */
    if ((NULL == dioRxBuf->buf) ||
        (dioRxBuf->cnt == dioRxBuf->buf->length) ||
        (dioRxBuf->seg == dioRxBuf->buf->c_dio_segment))
    {
	    BTHAL_MODEM_LOG_INFO(("BTHAL MD: no free buffers for uploading data"));
        return (BTHAL_STATUS_FAILED);
    }

    /* Current buffer's segment and its length */
    dioSeg = (T_dio_segment *)&dioRxBuf->buf->ptr_dio_segment[dioRxBuf->seg];

    /* Fill return values: pointer to data buffer and its length */
    *buffer = (BTHAL_U8 *)dioSeg->ptr_data;
    *len = (BTHAL_INT)dioSeg->c_data;
    
    /* Increment current segment's index */
    dioRxBuf->seg++;

    return (BTHAL_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BTHAL_MD_UploadDataReady()
 *
 *     Notifies that upload buffer is filled with data.
 */
BthalStatus BTHAL_MD_UploadDataReady(BTHAL_U8 *buffer,
                                     BTHAL_INT len,
                                     BTHAL_BOOL endOfData)
{
	BthalStatus mdStatus = BTHAL_STATUS_SUCCESS;
    BTHAL_BOOL escSequenceFound = BTHAL_FALSE;
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;
	DioBtBuf *dioRxBuf = &dioBtDev->rxBuf[dioBtDev->rxReadBufIdx];

	BTHAL_MODEM_LOG_FUNCTION(("BTHAL_MD_UploadDataReady"));

    /* Check, if MD is connected */
    if (bthalMdContext.state < BTHAL_MD_STATE_CONNECTED)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD is not connected"));
        return (BTHAL_STATUS_FAILED);
    }

    /* Check data for the escape sequence.
     * Assume, at least for now, that it may appear only at the beginning of
     * buffer. This means that we always have enough DIO buffers for reading
     * data from BT or, and this often happens, that escape sequence is sent
     * after some timeout without data. We also assume that there is no data
     * after the sequence, at least, we throw it away */
    if ((len >= 3) && (NULL != buffer) && 
        (*buffer++ == '+') && (*buffer++ == '+') && (*buffer == '+'))
    {
	    BTHAL_MODEM_LOG_INFO(("BTHAL MD: ESC sequence detected"));

        /* Notify modem that escape sequence was detected */
        mdStatus = BTHAL_MD_TranslateBtSignalsToModem(&dioBtDev->btControlSignals,
                                                      BTHAL_TRUE);
		if (BTHAL_STATUS_SUCCESS != mdStatus)
		{
			BTHAL_MODEM_LOG_INFO(("BTHAL_MD_TranslateBtSignalsToModem status: %s",
							BTHAL_StatusName(mdStatus)));
		}
        else
        {
            escSequenceFound = BTHAL_TRUE;
        }
    }
    else
    {
        /* Update amount of passed data for uploading */
        dioRxBuf->cnt += len;
    }

    /* Check whether all buffer is filled or this is the end of data chunk for
     * uploading or escape sequence was detected or only control information was
     * passed */
    if (endOfData ||
        (dioRxBuf->cnt == dioRxBuf->buf->length) ||
        escSequenceFound ||
        !len)
    {
        /* Move read buffer index to the next buffer */
        if (DIO_BT_RX_BUF_NUM == ++dioBtDev->rxReadBufIdx)
        {
	        dioBtDev->rxReadBufIdx = 0;
        }

        /* Notify DIO interface that buffer is ready for reading */
		DioBtSendSignal(DRV_SIGTYPE_READ);
    }

    return (mdStatus);
}
#endif /* BTHAL_MD_UPLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED */

/*-------------------------------------------------------------------------------
 * BTHAL_MD_TranslateBtSignalsToModem()
 *
 *     Translates serial signals from bits used by SPP port to modem rules.
 */
BthalStatus BTHAL_MD_TranslateBtSignalsToModem(const SppControlSignals *btSignals,
                                               BTHAL_BOOL escSeqFound)
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;
    U32 mdLineState;

    /* Check whether new control signals were passed */
    if (NULL == btSignals)
    {
        return (BTHAL_STATUS_INVALID_PARM);
    }

    /*  DIO_SB: ON = 0 */
    /*  DIO_RING = 0 */
    mdLineState = 0;

    /* DIO_SA - bit 31 */
    if (!(btSignals->signals & SPP_CONTROL_SIGNAL_DTR))
    {
        mdLineState |= DIO_SA;
    }

    /*  DIO_X */
    if (!(btSignals->signals & SPP_CONTROL_SIGNAL_RTS))
    {
        mdLineState |= DIO_X;
    }

    /* DIO_BRKLEN - bits 7-0 */
    mdLineState |= btSignals->breakLen;
    
    /* Check whether escape sequence was found */
    if (escSeqFound)
    {
	    mdLineState |= DIO_ESC;
    }
    
    /* Set flag that BT control signals were changed and store new state only,
     * if it has been changed */
    if (dioBtDev->rxLineState != mdLineState)
    {
        dioBtDev->rxLineState = mdLineState;
        dioBtDev->rxLineStateChanged = TRUE;
        BTHAL_MODEM_LOG_INFO(("BTHAL_MD_TranslateBtSignalsToModem: rxLineState 0x%08x",
                        dioBtDev->rxLineState));
    }

    return (BTHAL_STATUS_SUCCESS);
}
						    
/*-------------------------------------------------------------------------------
 * BTHAL_MD_TranslateModemSignalsToBt()
 *
 *     Translates serial signals from bits used by modem port to SPP rules.
 */
BthalStatus BTHAL_MD_TranslateModemSignalsToBt(SppControlSignals *btSignals)
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;
    btSignals->signals = 0;

	/* DIO_SA - bit 31 */
	if (!(dioBtDev->txLineState & DIO_SA))
	{
        btSignals->signals |= SPP_CONTROL_SIGNAL_DSR;
	}
	
	/* DIO_SB - bit 30 */
	if (!(dioBtDev->txLineState & DIO_SB))
	{
		btSignals->signals |= SPP_CONTROL_SIGNAL_CD;
	}

	/*  DIO_X  - bit 29 */
	/* It was advised by Matthias  from TIB to constantly set this bit to 1 */
	if (!(dioBtDev->txLineState & DIO_X))
	{
		btSignals->signals |= SPP_CONTROL_SIGNAL_CTS;
	}

	/*  DIO_RING  - bit 28 */
	if (dioBtDev->txLineState & DIO_RING)
	{
		btSignals->signals |= SPP_CONTROL_SIGNAL_RI;
	}

	/* DIO_BRKLEN - bits 7-0 */
	btSignals->breakLen = dioBtDev->txLineState & DIO_BRKLEN;

    BTHAL_MODEM_LOG_INFO(("BTHAL_MD_TranslateModemSignalsToBt: signals 0x%02x, breakLen %d",
                    btSignals->signals,
                    btSignals->breakLen));

    return (BTHAL_STATUS_SUCCESS);
}

/*-------------------------------------------------------------------------------
 * BthalMdSetState()
 *
 *		Gets state's name.
 */
static void BthalMdSetState(BTHAL_U8 state)
{
    bthalMdContext.state = state;
    BTHAL_MODEM_LOG_INFO(("BTHAL MD: state %s", BthalMdStateName()));
}

/*-------------------------------------------------------------------------------
 * BthalMdStateName()
 *
 *		Gets state's name.
 */
static const char *BthalMdStateName()
{
    switch (bthalMdContext.state)
    {
        case BTHAL_MD_STATE_IDLE:
            return "BTHAL_MD_STATE_IDLE";

        case BTHAL_MD_STATE_INITIALIZED:
            return "BTHAL_MD_STATE_INITIALIZED";

        case BTHAL_MD_STATE_REGISTERED:
            return "BTHAL_MD_STATE_REGISTERED";

        case BTHAL_MD_STATE_CONNECTED:
            return "BTHAL_MD_STATE_CONNECTED";

        default:
            return "BTHAL MD unknown state";
    }
}

/*-------------------------------------------------------------------------------
 * DioBtIsDevOpen()
 *
 *	   	Verifies that the DIO BT device is configured.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	device [in] - DIO device number.
 *
 * Returns:
 *      DRV_OK - device is configured.
 *
 *      DRV_NOTCONFIGURED - device is valid, but not configured.
 *
 *      DRV_INVALID_PARAMS - device ID is NOT valid.
 *
 *      DRV_INITFAILURE - DIO has not yet been initialized
 */
U16 DioBtIsDevOpen(U32 device)
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

	/* Check device ID */
    if (device != dioBtDev->devID)
    {
        BTHAL_MODEM_LOG_ERROR(("Wrong DIO BT device ID"));
        return (DRV_INVALID_PARAMS);
    }

    /* Check if DIO BT device is initialized */
    if (dioBtDev->state < DIO_BT_DEV_STATE_CLOSED)
    {
        BTHAL_MODEM_LOG_ERROR(("DIO BT device is not initialized"));
        return (DRV_INITFAILURE);
    }

    /* Check if DIO BT device is open */
    if (dioBtDev->state < DIO_BT_DEV_STATE_OPEN)
    {
        BTHAL_MODEM_LOG_ERROR(("DIO BT device is not open"));
        return (DRV_NOTCONFIGURED);
    }

	return (DRV_OK);
}

/*-------------------------------------------------------------------------------
 * DioBtSendSignal()
 *
 *	   	Sends signal to DIO interface.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	signal [in] - a signal to be sent.
 *
 * Returns:
 *      None.
 */
void DioBtSendSignal(USHORT signal)
{
	T_DRV_SIGNAL  btSig;
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;
			
	BTHAL_MODEM_LOG_FUNCTION(("DioBtSendSignal"));
	
	/* Fill signal information and send the signal with callback function */
	btSig.DrvHandle  = dioBtDev->drvHandle;
	btSig.SignalType = signal;
	btSig.DataLength = sizeof(U32);
	btSig.UserData   = &dioBtDev->devID;
	dioBtDev->sigCallback (&btSig);
}

/*-------------------------------------------------------------------------------
 * dio_init_bt()
 *
 *	   	This function actually initialized the driver.
 *      Each driver has its own initialization function. The function is not
 *          called directly by the user of the DIO interface. The DIO interface
 *          layer calls the initialization functions of all DIO drivers, when
 *          dio_init() is called.
 *      The function returns DRV_INITIALIZED if the driver has already been
 *          initialized and is ready to be used or is already in use.
 *      In case of an initialization failure, (i.g. the configuration given with
 *          drv_init can not be used), the function returns DRV_INITFAILURE. In
 *          this case the drvier can not be used.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	drv_init [in] - pointer to a structure with a max number of devices.
 *
 * Returns:
 *      DRV_OK - initialization successful.
 *
 *      DRV_INITFAILURE - initialization failed.
 *
 *      DRV_INITIALIZED - interface already initialized and is ready to be used
 *          or is already in use.
 */
U16 dio_init_bt (T_DIO_DRV *drv_init)
{
	BTHAL_U8 idx;
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;
    T_DIO_CAP_SER *dioCapSpp = &dioBtDev->dioCapSpp;

	BTHAL_MODEM_LOG_FUNCTION(("dio_init_bt"));
	
	if (drv_init->max_devices > DIO_BT_NUM_OF_PORTS)
	{
		return DRV_INITFAILURE;
	}

	/* Data structure initialization */
	dioBtDev->drvHandle     = NULL;
	dioBtDev->sigCallback   = NULL;
	dioBtDev->state         = DIO_BT_DEV_STATE_IDLE;

	/* Configure Capabilities for each device */
	dioCapSpp->device_type		= DIO_TYPE_SER;
	
    /* DIO_FLAG_SLEEP is zero */
    dioCapSpp->device_flags	    = DIO_SLEEP_DISABLE;
	
    /* Read/write control buffer */
    dioCapSpp->mtu_control		= sizeof(T_DIO_CTRL_LINES);
	dioCapSpp->mtu_data		    = 1;
	dioCapSpp->driver_name	    = "BT";
	
    /* Auto baudrate not supported */
    dioCapSpp->baudrate_auto	= 0x000000;

//	dioCapSpp->baudrate_fixed 	= (DIO_BAUD_921600 |
//								   DIO_BAUD_812500 |
//								   DIO_BAUD_460800 |
//								   DIO_BAUD_406250 |
//								   DIO_BAUD_230400 |
//								   DIO_BAUD_203125 |
	dioCapSpp->baudrate_fixed 	= (DIO_BAUD_115200 |
								   DIO_BAUD_57600  |
								   DIO_BAUD_38400  |
								   DIO_BAUD_33900  |
								   DIO_BAUD_28800  |
								   DIO_BAUD_19200  |
								   DIO_BAUD_14400  |
                                   DIO_BAUD_9600);
	
    /* Support character framing with 8 data bits */
    dioCapSpp->char_frame 	= DIO_CF_8N1;
	
    /* Support RTS-CTS flow control */
    dioCapSpp->flow_control = DIO_FLOW_RTS_CTS;
	
    /* Escape sequence detection is supported */
    dioCapSpp->ser_flags	= DIO_FLAG_SER_ESC;

	/* Initialize send and receive buffers */
	for (idx=0; idx<DIO_BT_RX_BUF_NUM; idx++)
	{
		dioBtDev->rxBuf[idx].buf = NULL;
		dioBtDev->rxBuf[idx].cnt = 0;
	}

	for (idx=0; idx<DIO_BT_TX_BUF_NUM; idx++)
	{
		dioBtDev->txBuf[idx].buf = NULL;
		dioBtDev->txBuf[idx].cnt = 0;
	}

	dioBtDev->txNewBufIdx   = 0;
	dioBtDev->txReadBufIdx  = 0;
	dioBtDev->txGetBufIdx   = 0;
	dioBtDev->txLineState   = 0;



	dioBtDev->rxNewBufIdx   = 0;
	dioBtDev->rxReadBufIdx  = 0;
	dioBtDev->rxGetBufIdx   = 0;
	dioBtDev->rxLineState   = 0;
	dioBtDev->rxLineStateChanged = FALSE;

	dioBtDev->devID	= DIO_DRV_BT | DIO_TYPE_SER | DIO_BT_PORT_NUMBER;
    dioBtDev->state = DIO_BT_DEV_STATE_CLOSED;

	return (DRV_OK);
}

/*-------------------------------------------------------------------------------
 * dio_export_bt()
 *
 *      This function returns a list of functions which are exported by the
 *          driver.
 *      Each driver has its own export function.
 *      dio_export_drv() is not called directly by he user of the DIO interface.
 *          The DIO interface layer calls the export functions of all DIO
 *          drivers, when dio_init() is called.
 *      This function needs to be provided by each DIO driver in order to enable
 *          more than one driver using the DIO interface.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *      dio_func [out] - with this parameter the driver returns a pointer to the
 *          list of functions exported by the driver.
 *
 * Returns:
 *      None.
 */
void dio_export_bt(T_DIO_FUNC ** dio_func)
{
	BTHAL_MODEM_LOG_FUNCTION(("dio_export_bt"));
	
	*dio_func = &dio_func_bt;
}

/*-------------------------------------------------------------------------------
 * dio_user_ready_bt()
 *
 *      The function allows the driver to open the devices which have device
 *          numbers of the given range.  Each driver has its own ready function.
 *      The function is not called directly by the user of the DIO interface.
 *      The DIO interface layer calls the ready functions of the DIO driver,
 *          when dio_user_init() is called.
 *      The driver uses the given sigCallback function for devices of the given
 *          device number range.
 *      The driver stores the drvHandle and passes it in the T_DRV_SIGNAL
 *          structure of the Signal parameter to the calling process every time
 *          the callback function is called.
 *      This function needs to be provided by each DIO driver in order to enable
 *          more than one user using the DIO interface.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *      deviceRangeStart [in] - first device number of a range of device numbers.
 *
 *      deviceRangeEnd [in] - last device number of a range of device numbers.
 *
 *      drvHandle [in] - unique handle of the DIO user.
 *
 *      sigCallback - this parameter points to the function that is called at
 *          the time an event occurs that is to be signaled.
 *
 * Returns:
 *      None.
 */
void dio_user_ready_bt(U32 deviceRangeStart,
                       U32 deviceRangeEnd,
                       U16 drvHandle,
                       T_DRV_CB_FUNC sigCallback)
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

	BTHAL_MODEM_LOG_FUNCTION(("dio_user_ready_bt"));
	
	/* Check range of passed devices */
    if ((dioBtDev->devID < deviceRangeStart) || (dioBtDev->devID > deviceRangeEnd))
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD: wrong DIO BT device ID"));
        return;
    }

	/* Check if DIO BT device is initialized */
    if (dioBtDev->state < DIO_BT_DEV_STATE_CLOSED)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD: DIO BT device is not initialized"));
        return;
    }

	/* Configure DIO BT device */
    dioBtDev->drvHandle = drvHandle;
    dioBtDev->sigCallback = sigCallback;
}

/*-------------------------------------------------------------------------------
 * dio_user_not_ready_bt()
 *
 *      The function is used to clear the sigCallback / device association which
 *          was previously set with the dio_user_ready(). That means the
 *          sigCallback function of devices of the given device number range must
 *          not be called any more.
 *      All devices of the given device number range need to be closed before the
 *          function can be called.
 *      The function returns DRV_OK if it was able to clear the sigCallback /
 *          device associations successfully.
 *      In case, there is no device in the given device number range the function
 *          also returns DRV_OK.
 *      If there is still an open device in the given device number range the
 *          function returns DRV_INVALID_PARAMS. In this case, the sigCallback
 *          function can still be used by the driver.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *      deviceRangeStart [in] - first device number of a range of device numbers
 *          which cannot be handled by a DIO user any more.
 *
 *      deviceRangeEnd [in] - last device number of a range of device numbers
 *          which cannot be handled by a DIO user any more.
 *
 *      drvHandle [in] - unique handle of the DIO user.
 *
 *      sigCallback - this parameter points to the function that is called at
 *          the time an event occurs that is to be signaled.
 *
 * Returns:
 *      DRV_OK - user operation successfully terminated.
 *
 *      DRV_INVALID_PARAMS - invalid parameters.
 *
 *      DRV_INITFAILURE - DIO BT device is not initialized.
 *
 *      DRV_INTERNAL_ERROR - internal driver's error.
 */
U16 dio_user_not_ready_bt(U32 deviceRangeStart, U32 deviceRangeEnd)
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

    BTHAL_MODEM_LOG_FUNCTION(("dio_user_not_ready_bt"));
	
	/* Check range of passed devices */
    if ((dioBtDev->devID < deviceRangeStart) || (dioBtDev->devID > deviceRangeEnd))
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD: wrong DIO BT device ID"));
        return (DRV_INVALID_PARAMS);
    }
	
    /* Check if DIO BT device is initialized */
    if (dioBtDev->state < DIO_BT_DEV_STATE_CLOSED)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD: DIO BT device is not initialized"));
        return (DRV_INITFAILURE);
    }

    /* Check if DIO BT device is still in use */
    if (dioBtDev->state >= DIO_BT_DEV_STATE_OPEN)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD: DIO BT device is still in use"));
        return (DRV_INTERNAL_ERROR);
    }

	/* Mark device as not in use */
	dioBtDev->sigCallback = NULL;

    return (DRV_OK);
}

/*-------------------------------------------------------------------------------
 * dio_exit_bt()
 *
 *      The function is called when the DIO interface functionality is no longer
 *          required. The function deallocates the resources (interrupts,
 *          buffers, etc.).
 *      The DIO drivers terminate regardless of any outstanding data to be sent.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *      None.
 *
 * Returns:
 *      None.
 */
void dio_exit_bt(void)
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

	BTHAL_MODEM_LOG_FUNCTION(("dio_exit_bt"));

	dioBtDev->state = DIO_BT_DEV_STATE_IDLE;
}

/*-------------------------------------------------------------------------------
 * dio_set_rx_buffer_bt()
 *
 *      This function provides a receive buffer to the driver.
 *
 *      The driver should use this buffer to store the received data of the
 *          specified device.
 *      The function should always return immediately after overtaking the
 *          buffer, without waiting for data.
 *      To avoid reception gaps more than one receive buffer should be provided
 *          to the driver via several calls of this function. The provided
 *          buffers should be used in the order they were provided.
 *      If the driver is not able to take over the provided buffer (e.g. because
 *          its internal data buffer queue is full) the function returns
 *          DRV_BUFFER_FULL.
 *      The driver uses the DRV_SIGTYPE_READ signal, when data is filled into
 *          the buffer.
 *      
 * Type:
 *		Asynchronous or synchronous.
 *
 * Parameters:
 *      device [in] - number of device which gets the buffer.
 *
 *      buffer [in] - pointer to the descriptor of the received buffer.
 *
 * Returns:
 *      DRV_OK - the operation is successful. The DRV_SIGTYPE_READ will be sent
 *          to the DIO interface, when the data is filled into the buffer.
 *
 *      DRV_BUFFER_FULL - queue for buffer descriptors is full.
 *
 *      DRV_INVALID_PARAMS - the specified device does not exist or the data
 *          buffer is not big enough.
 *
 *      DRV_INTERNAL_ERROR - internal driver's error.
 *
 *      DRV_NOTCONFIGURED  - the device is not yet configured.
 */
U16 dio_set_rx_buffer_bt(U32 device, T_dio_buffer *buffer)
{
	U16 status;
    BthalMdEvent mdEvent;
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;
	DioBtBuf *dioRxBuf = &dioBtDev->rxBuf[dioBtDev->rxNewBufIdx];

	BTHAL_MODEM_LOG_FUNCTION(("dio_set_rx_buffer_bt"));
	
	/* Check if requested device is valid and successfully opened */
	if (DRV_OK != (status = DioBtIsDevOpen(device)))
	{
		return (status);
	}

    /* Check whether we have room to store the buffer's descriptor */
    if (NULL != dioRxBuf->buf)
    {
        BTHAL_MODEM_LOG_ERROR(("BTHAL MD: no room for Rx buffer's descriptor"));
        return (DRV_BUFFER_FULL);
    }

	/* Save buffer information and initialize segments counter and counter
     * of bytes which were filled into buffer's segments */
    dioRxBuf->cnt = 0;
    dioRxBuf->seg = 0;
    dioRxBuf->buf = buffer;

	/* Increment index of new Rx buffers and check boundaries of the descriptors
     * array */
	if (DIO_BT_RX_BUF_NUM == ++dioBtDev->rxNewBufIdx)
	{
		dioBtDev->rxNewBufIdx = 0;
	}



    /* Send event to MDG application */
    mdEvent.eventType = BTHAL_MD_EVENT_UPLOAD_BUF;
    bthalMdContext.mdCallback(&mdEvent);

    return (DRV_OK);
}

/*-------------------------------------------------------------------------------
 * dio_read_bt()
 *
 *      This function returns a receive buffer and control information.
 *      It should always return immediately after changing internal states,
 *          without waiting for any more data. The receive buffers should be
 *          returned in the same order as provided with the dio_set_rx_buffer()
 *          calls (First-In-First-Out). The returned buffer is not in control of
 *          the driver any more.
 *      The buffer should be returned even if it is empty.
 *      If there is no receive buffer in control of the driver anymore, then
 *          the buffer is set to NULL. In this case, only control information is
 *          delivered.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *      device [in] - number of device which gets the buffer.
 *
 *      controlInfo [out] - pointer to the provided control buffer into which
 *          the driver copies control information.
 *
 *      buffer [out] - returned data buffer's descriptor.
 *
 * Returns:
 *      DRV_OK - the operation is successful.
 *
 *      DRV_INVALID_PARAMS - the specified device does not exist.
 *
 *      DRV_INTERNAL_ERROR - internal driver's error.
 *
 *      DRV_NOTCONFIGURED  - the device is not yet configured.
 */
U16 dio_read_bt(U32 device, T_DIO_CTRL *controlInfo, T_dio_buffer **buffer)
{
	U8 bufIndex;
	U16 status;
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

	BTHAL_MODEM_LOG_FUNCTION(("dio_read_bt"));

	/* Check if requested device is valid and successfully opened */
	if (DRV_OK != (status = DioBtIsDevOpen(device)))
	{
		return (status);
	}

	/* Extract oldest non-read data buffer. */
	bufIndex = dioBtDev->rxGetBufIdx;
	*buffer = dioBtDev->rxBuf[bufIndex].buf;

	/* If data buffer was available, modify header length to reflect actual data,
     * and mark associated DIO buffer as read. */
	if (NULL != *buffer)
	{
		/* Buffer is active */
		(*buffer)->length = dioBtDev->rxBuf[bufIndex].cnt;
		
		/* Mark buf as read and freed */
		dioBtDev->rxBuf[bufIndex].buf = NULL;

		/* Increase the get buffer to return to DIO IL */
		if (DIO_BT_RX_BUF_NUM == ++bufIndex)
		{
			bufIndex = 0;
		}
		dioBtDev->rxGetBufIdx = bufIndex;
	}

	/* Copy latest control information for user, if there were changes in it. */
	if (NULL != controlInfo)
	{
		if (TRUE == dioBtDev->rxLineStateChanged)
		{
			T_DIO_CTRL_LINES *pCtrlLines = (T_DIO_CTRL_LINES *)controlInfo;
			
	        pCtrlLines->state		 = dioBtDev->rxLineState;
	        pCtrlLines->control_type = DIO_CTRL_LINES;
	        pCtrlLines->length		 = sizeof(T_DIO_CTRL_LINES);

			dioBtDev->rxLineStateChanged = FALSE;

			BTHAL_MODEM_LOG_INFO(("BTHAL MD: BT control state 0x%08x",
                           pCtrlLines->state));
		}
		else
		{
			controlInfo = NULL;
		}
	}

    return (DRV_OK);
}

/*-------------------------------------------------------------------------------
 * dio_write_bt()
 *
 *      This function provides a send buffer to the driver which contains data
 *          to send. This function should return immediately after overtaking
 *          the buffer.
 *      To avoid transmission gaps more than one send buffer should be provided
 *          to the driver via several calls of this function. The provided send
 *          buffers should be sent in the order they were provided.
 *      If the driver is not able to take over the provided buffer (e.g. because
 *          its internal buffer queue is full), the function returns
 *          DRV_BUFFER_FULL.
 *      If buffer is set to NULL, then the driver only copies the provided
 *          control information.
 *      
 * Type:
 *		Asynchronous or synchronous.
 *
 * Parameters:
 *      device [in] - number of device which gets the buffer.
 *
 *      controlInfo [in] - pointer to the provided control buffer from which the
 *          driver copies control information.
 *
 *      buffer [in] - provided data buffer's descriptor.
 *
 * Returns:
 *      DRV_OK - the operation is successful. The driver uses the
 *          DRV_SIGTYPE_WRITE signal, when the data of the buffer is sent.
 *
 *      DRV_INVALID_PARAMS - the specified device does not exist or the data
 *          buffer is too big.
 *
 *      DRV_INTERNAL_ERROR - internal driver's error.
 *
 *      DRV_NOTCONFIGURED - the device is not yet configured.
 *
 *      DRV_BUFFER_FULL - queue for buffer descriptors is full.
 */
U16 dio_write_bt(U32 device, T_DIO_CTRL *controlInfo, T_dio_buffer *buffer)
{
	U16 tcnt = 0;
	U16 idx;
	U16 status;
    BthalMdEvent mdEvent;
	T_dio_segment * segPtr;
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;
	DioBtBuf *dioTxBuf = &dioBtDev->txBuf[dioBtDev->txNewBufIdx];

	BTHAL_MODEM_LOG_FUNCTION(("dio_write_bt"));

	/* Check if requested device is valid and successfully opened */
	if (DRV_OK != (status = DioBtIsDevOpen(device)))
	{
		return (status);
	}

	if (NULL != buffer)
	{
		/* Check if both write buffers are already in use */
		if (NULL != dioTxBuf->buf)
		{
            BTHAL_MODEM_LOG_ERROR(("BTHAL MD: DRV_BUFFER_FULL in dio_write_bt()"));
			return (DRV_BUFFER_FULL);
		}
		  
		 /* Validate received data lengths make sense */
		for (idx=0;idx<buffer->c_dio_segment;idx++)
		{
			segPtr = &buffer->ptr_dio_segment[idx];
			tcnt = tcnt + segPtr->c_data;
		}
		
		if (tcnt != buffer->length)
		{
            BTHAL_MODEM_LOG_ERROR(("BTHAL MD: DRV_INVALID_PARAMS in dio_write_bt()"));
			return (DRV_INVALID_PARAMS);
		}

		/* Save buffer information and initialize segments counter and counter
         * of bytes which existed in returned segments */
		dioTxBuf->cnt = 0;
        dioTxBuf->seg = 0;
		dioTxBuf->buf = buffer;

#if BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED

        dioTxBuf->cntRet = 0;

#endif /* BTHAL_MD_DOWNLOAD_BUF_OWNER_MODEM == BTL_CONFIG_ENABLED */

		/* Increase the new TX buffer's index */
		if (DIO_BT_TX_BUF_NUM == ++dioBtDev->txNewBufIdx)
		{
			dioBtDev->txNewBufIdx = 0;
		}

	    /* Send event to the MDG application */
        mdEvent.eventType = BTHAL_MD_EVENT_DOWNLOAD_DATA;
        mdEvent.downloadDataLen = buffer->length;
        bthalMdContext.mdCallback(&mdEvent);
	}

	if (NULL != controlInfo)
	{
		T_DIO_CTRL_LINES *pCtrlLines = (T_DIO_CTRL_LINES *)controlInfo;


		/* Copy the modem control line information from DIO_IL, if it was
         * changed */
		if (pCtrlLines->state != dioBtDev->txLineState)
		{
			BTHAL_MODEM_LOG_INFO(("BTHAL MD: modem control state 0x%08x",
                           pCtrlLines->state));
			dioBtDev->txLineState = pCtrlLines->state;


            /* Send event to the MDG application */
            mdEvent.eventType = BTHAL_MD_EVENT_CONTROL_SIGNALS;
            bthalMdContext.mdCallback(&mdEvent);
		}
	}

    return (DRV_OK);
}

/*-------------------------------------------------------------------------------
 * dio_get_tx_buffer_bt()
 *
 *      This function returns a send buffer provided via dio_write().
 *      It should always return immediately after changing internal states,
 *          without waiting for any outstanding events.
 *      The send buffers should be returned in the same order as provided with
 *          the dio_write() calls (First-In-First-Out). The returned send buffer
 *          is not in control of the driver any more.
 *      If there is no send buffer in control of the driver any more, then
 *          buffer is set to NULL.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *      device [in] - number of device which gets the buffer.
 *
 *      buffer [out] - descriptor of a data buffer previously provided with
 *          dio_write_bt() function.
 *
 * Returns:
 *      DRV_OK - the operation is successful.
 *
 *      DRV_INVALID_PARAMS - the specified device does not exist.
 *
 *      DRV_INTERNAL_ERROR - internal driver's error.
 *
 *      DRV_NOTCONFIGURED - the device is not yet configured.
 */
U16 dio_get_tx_buffer_bt(U32 device, T_dio_buffer **buffer)
{

	USHORT getBufIdx;
	U16 status;
    DioBtBuf *dioTxBuf;
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

	BTHAL_MODEM_LOG_FUNCTION(("dio_get_tx_buffer_bt"));

	/* Check if requested device is valid and successfully opened */
	if (DRV_OK != (status = DioBtIsDevOpen(device)))
	{
		return (status);
	}

	getBufIdx = dioBtDev->txGetBufIdx;
    dioTxBuf = &dioBtDev->txBuf[getBufIdx];

	/* Can't get Tx buffer if copy hasn't completed */
	if ((NULL != dioTxBuf) && (dioTxBuf->cnt != dioTxBuf->buf->length))
	{
		*buffer = NULL;
		return (DRV_INVALID_PARAMS);
	}

	/* Pass back the buffer */
	*buffer = dioBtDev->txBuf[getBufIdx].buf;
	dioBtDev->txBuf[getBufIdx].buf = NULL;

	/* Move write buffer pointer if copy hasn't completed   */
	if ((NULL != *buffer) &&
		(dioBtDev->txBuf[getBufIdx].cnt != (*buffer)->length) &&
		(dioBtDev->txNewBufIdx == getBufIdx))
	{
		/* Increase the new TX buffer's index */
		if (DIO_BT_TX_BUF_NUM == ++dioBtDev->txNewBufIdx)
		{
			dioBtDev->txNewBufIdx = 0;
		}
	}

	if (NULL != *buffer)
	{
		/* Increase the Get TX buffer's index */
		if (DIO_BT_TX_BUF_NUM == ++dioBtDev->txGetBufIdx)
		{
			dioBtDev->txGetBufIdx = 0;
		}
	}
	
    return (DRV_OK);
}

/*-------------------------------------------------------------------------------
 * dio_clear_bt()
 *
 *      This function is used to clear the hardware send buffer.
 *      It should always return immediately after changing internal states and
 *          resetting internal values, without waiting for any outstanding
 *          events.
 *      If the driver could not clear the hardware send buffer at once, the
 *          function returns DRV_INPROCESS. In this case the driver will send
 *          the signal DRV_SIGTYPE_CLEAR to the protocol stack, when the
 *          hardware send buffer is cleared completely.
 *      If the driver was able to clear the hardware buffer at once, the
 *          function returns DRV_OK. In this case the signal DRV_SIGTYPE_CLEAR
 *          is not sent to the protocol stack.
 *      
 * Type:
 *		Asynchronous or synchronous.
 *
 * Parameters:
 *      device [in] - data device number.
 *
 * Returns:
 *      DRV_OK - the function successfully completed.
 *
 *      DRV_INVALID_PARAMS - the specified device does not exist.
 *
 *      DRV_INTERNAL_ERROR - internal driver's error.
 *
 *      DRV_NOTCONFIGURED - the device is not yet configured.
 *
 *      DRV_INPROCESS - the driver is busy clearing the buffer.
 */
U16 dio_clear_bt(U32 device)
{
    U16 status;

	BTHAL_MODEM_LOG_FUNCTION(("dio_clear_bt"));

	/* Check if requested device is valid and successfully opened */
	if (DRV_OK != (status = DioBtIsDevOpen(device)))
	{
		return (status);
	}
    else
    {
        /* Implement clearing in the future */
        return (DRV_OK);
    }
}

/*-------------------------------------------------------------------------------
 * dio_flush_bt()
 *
 *      With this function the driver is requested to inform the protocol stack,
 *          when the data in the hardware send buffer has been written
 *          successfully.
 *      That means the hardware send buffer is empty. The function should always
 *          return immediately after changing internal states without waiting
 *          for any outstanding events.
 *
 *      This function can be used by the protocol stack to ensure that no more
 *          data is to be sent (e.g. before switching between command mode and
 *          data mode or before changing the settings of the driver).
 *
 *      If the driver could not complete flushing the buffer at once, the
 *          function returns DRV_INPROCESS. In this case the driver will send
 *          the signal DRV_SIGTYPE_FLUSH to the protocol stack when the buffer
 *          is flushed completely.
 *
 *      If the hardware send buffer is already empty then the function returns
 *          DRV_OK. In this case the signal DRV_SIGTYPE_FLUSH is not sent to the
 *          protocol stack.
 *      
 * Type:
 *		Asynchronous or synchronous.
 *
 * Parameters:
 *      device [in] - data device number.
 *
 * Returns:
 *      DRV_OK - the function successfully completed.
 *
 *      DRV_INVALID_PARAMS - the specified device does not exist.
 *
 *      DRV_INTERNAL_ERROR - internal driver's error.
 *
 *      DRV_NOTCONFIGURED - the device is not yet configured.
 *
 *      DRV_INPROCESS - the driver is busy flushing the buffer.
 */
U16 dio_flush_bt(U32 device)
{
    U16 status;
    
	BTHAL_MODEM_LOG_FUNCTION(("dio_flush_bt"));

	/* Check if requested device is valid and successfully opened */
	if (DRV_OK != (status = DioBtIsDevOpen(device)))
	{
		return (status);
	}
    else
    {
        /* Implement flushing in the future */
        return (DRV_OK);
    }
}

/*-------------------------------------------------------------------------------
 * dio_get_capabilities_bt()
 *
 *      This function is used to retrieve the capabilities of a device.
 *      The driver returns a pointer to a static struct of constant values.
 *      It is not allowed to change these values neither by the driver nor by
 *          the protocol stack.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *      device [in] - data device number.
 *
 *      cap [out] - returned pointer to the device capabilities.
 *
 * Returns:
 *      DRV_OK - the function successfully completed.
 *
 *      DRV_INVALID_PARAMS - the specified device does not exist.
 *
 *      DRV_INTERNAL_ERROR - internal driver's error.
 */
U16 dio_get_capabilities_bt(U32 device, T_DIO_CAP **cap)
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

	BTHAL_MODEM_LOG_FUNCTION(("dio_get_capabilities_bt"));
	
	/* Verify that DIO BT was initialized */
    if (dioBtDev->state < DIO_BT_DEV_STATE_CLOSED)
    {
		return (DRV_INITFAILURE);
    }

	*cap = (T_DIO_CAP *)&dioBtDev->dioCapSpp;

	return (DRV_OK);
}

/*-------------------------------------------------------------------------------
 * dio_set_config_bt()
 *
 *      This function is used to configure a device (transmission rate, flow
 *          control, etc). A device can be configured at any time.
 *      The dcb points to a Device Control Block. The parameters that can be
 *          configured are included in the Device Control Block.
 *      If any value of the configuration is out of range, not supported or
 *          invalid in combination with any other value of the configuration,
 *          the function returns DRV_INVALID_PARAMS.
 *      Each device needs to be configured after the reception of a
 *          DRV_SIGTYPE_CONNECT signal. Only dio_get_capabilities(),
 *          dio_set_config() and dio_close_device() can be called while the
 *          device is not configured. All other device specific functions return
 *          DRV_NOTCONFIGURED.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *      device [in] - data device number.
 *
 *      dcb [in] - pointer to a Device Control Block.
 *
 * Returns:
 *      DRV_OK - the function successfully completed.
 *
 *      DRV_INVALID_PARAMS - one or more values are out of range or invalid in
 *          that combination.
 *
 *      DRV_INTERNAL_ERROR - internal driver's error.
 */
U16 dio_set_config_bt(U32 device, T_DIO_DCB *dcb)
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

	BTHAL_MODEM_LOG_FUNCTION(("dio_set_config_bt"));
	
	/* Verify that DIO BT was initialized */
    if (dioBtDev->state < DIO_BT_DEV_STATE_CLOSED)
    {
		return (DRV_INITFAILURE);
    }
	
	/* Store passed parameters */
    dioBtDev->dioDcbSpp = *((T_DIO_DCB_SER*)dcb);

	/* Check whether received device type is correct */
    if (DIO_TYPE_SER != dioBtDev->dioDcbSpp.device_type)
    {
		return DRV_INVALID_PARAMS;
    }
	
	/* Change device state */
    dioBtDev->state = DIO_BT_DEV_STATE_OPEN;
	
	return DRV_OK;
}

/*-------------------------------------------------------------------------------
 * dio_get_config_bt()
 *
 *      This function is used to retrieve the configuration of a device.
 *      The driver copies the configuration into the Device Control Block
 *          provided with dcb.
 *      To ensure that the configuration data will be copied in an appropriate
 *          struct the parameter device_type of the Device Control Block needs
 *          to be set properly before the function is called.
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *      device [in] - data device number.
 *
 *      dcb [out] - pointer to a Device Control Block.
 *
 * Returns:
 *      DRV_OK - the function successfully completed.
 *
 *      DRV_INVALID_PARAMS - the specified device does not exist or wrong Device
 *          Control Block provided.
 *
 *      DRV_INTERNAL_ERROR - internal driver's error.
 */
U16 dio_get_config_bt(U32 device, T_DIO_DCB *dcb)
{
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

	BTHAL_MODEM_LOG_FUNCTION(("dio_get_config_bt"));

	/* Provide configuration of the DIO BT device */
	*((T_DIO_DCB_SER*)dcb) = dioBtDev->dioDcbSpp;
	
    return (DRV_OK);

}

/*-------------------------------------------------------------------------------
 * dio_close_device_bt()
 *
 *      This function is used to close a device.
 *      The driver returns DRV_OK, if it was able to close the device
 *          successfully.
 *      In case the specified device does not exist, the driver also returns
 *          DRV_OK.
 *      If the driver still controls a protocol stack's buffer for this device,
 *          then it returns DRV_INVALID_PARAMS. In this case the device is not
 *          closed. In order to get the remaining buffers the protocol stack
 *          needs to call the functions dio_read() and dio_get_tx_buffer().
 *      
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *      device [in] - data device number.
 *
 * Returns:
 *      DRV_OK - the function successfully completed.
 *
 *      DRV_INVALID_PARAMS - the specified device does not exist or wrong Device
 *          Control Block provided.
 */
U16 dio_close_device_bt(U32 device)
{
	BTHAL_U8 idx;
    DioBtDev *dioBtDev = &bthalMdContext.dioBtDev;

	BTHAL_MODEM_LOG_FUNCTION(("dio_close_device_bt"));

	/* Verify that DIO BT was initialized */
    if (dioBtDev->state < DIO_BT_DEV_STATE_CLOSED)
    {
		/* Device is closed since there is currently no open device */
		return (DRV_OK);
    }

	/* Verify DIO BT device port number and initialization status */
    if ((DIO_BT_PORT_NUMBER != (device & DIO_DEVICE_MASK)) ||
        (dioBtDev->state < DIO_BT_DEV_STATE_CLOSED))
	{
		/* Device is invalid or not initialized */
		return (DRV_OK);
	}

	/* Check if any receive of transmit buffers are still in DIO use */
	for (idx=0; idx<DIO_BT_RX_BUF_NUM; idx++)
	{
		if (NULL != dioBtDev->rxBuf[idx].buf)
		{
			return DRV_INVALID_PARAMS;
		}
	}

	for (idx=0; idx<DIO_BT_TX_BUF_NUM; idx++)
	{
		if (NULL != dioBtDev->txBuf[idx].buf)
		{
			return DRV_INVALID_PARAMS;
		}
	}

	/* Close DIO BT device */
    dioBtDev->state = DIO_BT_DEV_STATE_CLOSED;

	return (DRV_OK);
}



