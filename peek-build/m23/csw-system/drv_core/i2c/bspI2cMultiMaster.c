/*
 * =============================================================================
 *    Copyright 2002-2005 Texas Instruments Inc. All rights reserved.
 */
#include "chipset.cfg"

#include "bspUtil_Assert.h"
#include "bspI2c.h"
#include "bspI2cMultiMaster.h"
#include "bspI2cMultiMaster_Llif.h"
#include "bspI2cMultiMaster_Map.h"

#include "sys_inth.h"
#include "sys_dma.h"

#include "bspI2cDelay.h"
#include "Nucleus.h"

#include "typedefs.h"
#include "general.h"
#include "rvm/rvm_use_id_list.h"
#include "rv/rv_trace.h"

/* In Typedef in TCS3.x */

#define BspDma_ChannelId T_DMA_TYPE_CHANNEL_HW_SYNCH
/*#define BSP_DMA_REQ_NONE 0xFF*/
/*=============================================================================
 *  Defines
 *============================================================================*/
#define BSP_I2C_Q_LOCK_BY_INTERRUPT

#define BSP_I2C_AL_INT     1
#define BSP_I2C_NACK_INT   2
#define BSP_I2C_ARDY_INT   3
#define BSP_I2C_RRDY_INT   4
#define BSP_I2C_XRDY_INT   5

#define BSP_I2C_ARDY_CLEAR_VALUE 0x0004
#define BSP_I2C_RRDY_CLEAR_VALUE 0x0008
#define BSP_I2C_XRDY_CLEAR_VALUE 0x0010

#define TICKS_TO_WAIT      5


/*=============================================================================
 * Description:
 *   This macro is used to create a lock state Variable used for entering in 
 *   critical sections;
 */
#define BSP_I2C_MULTI_MASTER_DEFINE_LOCK_STATE_VAR(lockState) \
Uint32   lockState

/*=============================================================================
 * Description:
 *   This macro is used to begin a critical section of code. It simply disables
 *   interrupts.
 */
#define BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState) \
lockState=NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS)

/*=============================================================================
 * Description:
 *   This macro is used to begin a critical section of code. It simply restores
 *   interrupts to their previous state.
 */
#define BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState) \
lockState=NU_Local_Control_Interrupts(lockState)

/*=============================================================================
 * Description:
 *   This macro simply enables the interrupt that fires when a read or write
 *   transaction is complete
 */
#define BSP_I2C_MULTI_MASTER_INTERRUPT_ENABLE(_id)                          \
{                                                                           \
    Uint32 *baseAddress = bspI2c_Info[(_id)].baseAddress;                   \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_ARDY,   \
            BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),               \
            BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED);                \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY,   \
            BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),               \
            BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED);                \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_XRDY,   \
            BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),               \
            BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED);                \
}

/*=============================================================================
 * Description:
 *   This macro simply disables the interrupt that fires when a read or write
 *   transaction is complete
 */
#define BSP_I2C_MULTI_MASTER_INTERRUPT_DISABLE(_id)                         \
{                                                                           \
    Uint32 *baseAddress = bspI2c_Info[(_id)].baseAddress;                   \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_AL,     \
            BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),               \
            BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED);                \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_NACK,   \
            BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),               \
            BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED);                \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_ARDY,   \
            BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),               \
            BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED);                \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY,   \
            BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),               \
            BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED);                \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_XRDY,   \
            BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),               \
            BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED);                \
}

/*=============================================================================
 * Description:
 *   This macro sets the slave address of the I2C_MASTER device to read or write to.
 */
#define BSP_I2C_MULTI_MASTER_SLAVE_ADDRESS_SET(_id,_slaveAddress)         \
{                                                                           \
    Uint32 *baseAddress = bspI2c_Info[(_id)].baseAddress;                   \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_SA,        \
            BSP_I2C_MULTI_MASTER_MAP_SA_PTR(baseAddress),               \
            (_slaveAddress));                                          \
}


/*=============================================================================
 * Description:
 *   This macro sets our own address of this I2C_MASTER device
 */
#define BSP_I2C_MULTI_MASTER_OWN_ADDRESS_SET(_baseAddress,_ownAddress)             \
{                                                                           \
    *(BSP_I2C_MULTI_MASTER_MAP_OA_PTR(_baseAddress))=(_ownAddress);     \
}

/*=============================================================================
 * Description:
 *  This macro sets the I2C_CNT register, which is the number of bytes that
 *  will be read into the read FIFO or transferred out of the write FIFO before
 *  an ARDY interrupt will trigger (note that if DCOUNT is greater than bytes in
 *  write FIFO, an XRDY interrupt will trigger. If DCOUNT is greater than the
 *  number of bytes that have been read from read FIFO, a RRDY interrupt will
 *  trigger. ARDY interrupt has a higher priority than XRDY or RRDY.
 */
#define BSP_I2C_MULTI_MASTER_DCOUNT_SET(_baseAddress,_numBytes)                    \
{                                                                           \
    *(BSP_I2C_MULTI_MASTER_MAP_CNT_PTR(_baseAddress))=(_numBytes);                    \
}

/*=============================================================================
 * Description:
 *   This macro is the reused shell for all trigger types. This should not be
 *   called directly. Instead use one of the following macros:
 *   TRIGGER_READ
 *   TRIGGER_WRITE
 *   TRIGGER_WRITE_NO_STOP
 */
#define BSP_I2C_MULTI_MASTER_TRIGGER_TRANSACTION(_id, _stopOrNot,            \
        _readWrite)                                                         \
{                                                                            \
    volatile Uint16 tmpReg;                                                  \
    Uint32 *baseAddress = bspI2c_Info[(_id)].baseAddress;                    \
    \
    /* Use a temp reg to reduce HW accesses */                               \
    tmpReg = 0x0000;                                                         \
    \
    /* Setup operation and trigger */                                        \
    BSP_I2C_MULTI_MASTER_LLIF_SET_LOCAL(BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN,\
            &tmpReg,BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_ENABLED);             \
    \
    BSP_I2C_MULTI_MASTER_LLIF_SET_LOCAL(BSP_I2C_MULTI_MASTER_LLIF_CON_MST,   \
            &tmpReg,BSP_I2C_MULTI_MASTER_LLIF_CON_MST_ENABLED);             \
    \
    BSP_I2C_MULTI_MASTER_LLIF_SET_LOCAL(BSP_I2C_MULTI_MASTER_LLIF_CON_TRX,   \
            &tmpReg,(_readWrite));                                          \
    \
    \
    BSP_I2C_MULTI_MASTER_LLIF_SET_LOCAL(BSP_I2C_MULTI_MASTER_LLIF_CON_STT,   \
            &tmpReg,1);                                                     \
    \
    BSP_I2C_MULTI_MASTER_LLIF_SET_LOCAL(BSP_I2C_MULTI_MASTER_LLIF_CON_STP,   \
            &tmpReg, _stopOrNot);                                       \
    \
    \
    /* Write out to the hardware */                                          \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_COMPLETE_REG,   \
            BSP_I2C_MULTI_MASTER_MAP_CON_PTR(baseAddress),tmpReg);          \
    \
}

/*=============================================================================
 * Description:
 *   This function triggers a "standard" write operation,
 *        which means:
 *              S       -       A       -       D(write)...(n)  -       P
 */
#define BSP_I2C_MULTI_MASTER_TRIGGER_WRITE(_baseAddress,_id)                             \
{                                                                           \
    *(BSP_I2C_MULTI_MASTER_MAP_CON_PTR(_baseAddress))	=((BSP_I2C_MULTI_MASTER_LLIF_CON_STT_MASK)|(BSP_I2C_MULTI_MASTER_LLIF_CON_STP_MASK)|(BSP_I2C_MULTI_MASTER_LLIF_CON_MST_MASK)  |(BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN_MASK)|(BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_MASK));     \
    bspI2c_Info[_id].busOwner = 0;                                                  \
}

/*=============================================================================
 * Description:
 *   This function triggers a "no stop" write operation,
 *        which means:
 *              S       -       A       -       D...(n)
 */
#define BSP_I2C_MULTI_MASTER_TRIGGER_WRITE_NO_STOP(_baseAddress,_id)     \
{                                                                           \
    *(BSP_I2C_MULTI_MASTER_MAP_CON_PTR((_baseAddress)))=((BSP_I2C_MULTI_MASTER_LLIF_CON_STT_MASK)|(BSP_I2C_MULTI_MASTER_LLIF_CON_MST_MASK)  |(BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN_MASK)|(BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_MASK));     \
    bspI2c_Info[_id].busOwner = 1;                                          \
}



/*=============================================================================
 * Description:
 *   This function triggers a read operation operation.
 *        which means:
 *              S       -       A       -       D(read)...(n)   -       P
 */
#define BSP_I2C_MULTI_MASTER_TRIGGER_READ(_baseAddress,_id)                              \
{                                                                           \
    *(BSP_I2C_MULTI_MASTER_MAP_CON_PTR((_baseAddress)))=((BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN_MASK|BSP_I2C_MULTI_MASTER_LLIF_CON_MST_MASK|BSP_I2C_MULTI_MASTER_LLIF_CON_STT_MASK|BSP_I2C_MULTI_MASTER_LLIF_CON_STP_MASK)&(~BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_MASK));     \
    bspI2c_Info[_id].busOwner = 0;                                          \
}


#ifdef Bsp_I2C_DMA_Enable 
/*=============================================================================
 * Description:
 *   This macro simply enables the DMA Read request
 */
#define BSP_I2C_MULTI_MASTER_DMA_READ_ENABLE(_id)                            \
{                                                                            \
    Uint32 *baseAddress = bspI2c_Info[(_id)].baseAddress;                    \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_BUF_RDMA_EN,    \
            BSP_I2C_MULTI_MASTER_MAP_BUF_PTR(baseAddress),                   \
            BSP_I2C_MULTI_MASTER_LLIF_BUF_RDMA_ENABLED);                    \
}

/*=============================================================================
 * Description:
 *   This macro simply disables the DMA Read request
 */
#define BSP_I2C_MULTI_MASTER_DMA_READ_DISABLE(_id)                           \
{                                                                            \
    Uint32 *baseAddress = bspI2c_Info[(_id)].baseAddress;                    \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_BUF_RDMA_EN,    \
            BSP_I2C_MULTI_MASTER_MAP_BUF_PTR(baseAddress),                   \
            BSP_I2C_MULTI_MASTER_LLIF_BUF_RDMA_DISABLED);                   \
}


/*=============================================================================
 * Description:
 *   This macro simply enables the DMA Write request
 */
#define BSP_I2C_MULTI_MASTER_DMA_WRITE_ENABLE(_id)                           \
{                                                                            \
    Uint32 *baseAddress = bspI2c_Info[(_id)].baseAddress;                    \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_BUF_XDMA_EN,    \
            BSP_I2C_MULTI_MASTER_MAP_BUF_PTR(baseAddress),                   \
            BSP_I2C_MULTI_MASTER_LLIF_BUF_XDMA_ENABLED);                    \
}


/*=============================================================================
 * Description:
 *   This macro simply disables the DMA Read request
 */
#define BSP_I2C_MULTI_MASTER_DMA_WRITE_DISABLE(_id)                          \
{                                                                            \
    Uint32 *baseAddress = bspI2c_Info[(_id)].baseAddress;                    \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_BUF_XDMA_EN,    \
            BSP_I2C_MULTI_MASTER_MAP_BUF_PTR(baseAddress),                   \
            BSP_I2C_MULTI_MASTER_LLIF_BUF_XDMA_DISABLED);                   \
}



/*=============================================================================
 * Description:
 *   This function triggers a DMA read operation operation.
 *        which means:
 *              S       -       D(read)...(n)   -       P
 *   Note that no address is implied since that is a write operation
 */
#define BSP_I2C_MULTI_MASTER_TRIGGER_DMA_READ(_id)                           \
{                                                                            \
    Uint32 *baseAddress = bspI2c_Info[(_id)].baseAddress;                    \
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY,        \
            BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),                    \
            BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED);                     \
    \
    BSP_I2C_MULTI_MASTER_DMA_READ_ENABLE(_id);                               \
    \
    if (bspI2c_transactionInfo[id].busControl == BSP_I2C_BUS_RELEASE)            \
    {  BSP_I2C_MULTI_MASTER_TRIGGER_TRANSACTION(_id,                         \
            BSP_I2C_MULTI_MASTER_LLIF_CON_STP_STOP,                          \
            BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_READ);                        \
        bspI2c_Info[_id].busOwner = 1;                                                   \
    }                                                                                                                                                \
    else                                                                                                                                     \
    {  BSP_I2C_MULTI_MASTER_TRIGGER_TRANSACTION(_id,                         \
            BSP_I2C_MULTI_MASTER_LLIF_CON_STP_NO_STOP,                       \
            BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_READ);                        \
        bspI2c_Info[_id].busOwner = 0;                                                   \
    }                                                                                                                                                \
}

#endif

/*=============================================================================
 * Description:
 *   This function triggers a "no stop" read operation operation.
 *        which means:
 *              S       -       A       -       D(read)...(n)*/
#define BSP_I2C_MULTI_MASTER_TRIGGER_READ_NO_STOP(_baseAddress,_id)                      \
{                                                                           \
    *(BSP_I2C_MULTI_MASTER_MAP_CON_PTR((_baseAddress)))=((BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN_MASK|BSP_I2C_MULTI_MASTER_LLIF_CON_MST_MASK|BSP_I2C_MULTI_MASTER_LLIF_CON_STT_MASK)&(~BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_MASK));     \
    bspI2c_Info[_id].busOwner = 0;                                          \
}

/*=============================================================================
 * Description:
 *   This macro sets up a Master transaction that is about to begin
 */


#ifdef BSP_I2C_DMA_Enable
#define BSP_I2C_MULTI_MASTER_STORE_TRANSACTION_INFO(_id,                   \
        _transType,                                                         \
        _deviceId,                                                          \
        _startAddress,                                                      \
        _dataPtr,                                                           \
        _dataCount,                                                         \
        _dmaRequestId,                                                      \
        _callback,                                                          \
        _callbackId,                                                        \
        _busControl)                                                       \
{                                                                           \
    bspI2c_transactionInfo[(_id)].transactionType   = (_transType);         \
    bspI2c_transactionInfo[(_id)].returnCode = BSP_I2C_RETURN_CODE_SUCCESS; \
    bspI2c_transactionInfo[(_id)].deviceId          = (_deviceId);          \
    bspI2c_transactionInfo[(_id)].dataStartAddress  = (_startAddress);      \
    bspI2c_transactionInfo[(_id)].currentAddress    = (_startAddress);      \
    bspI2c_transactionInfo[(_id)].dataPtr           = (_dataPtr);           \
    bspI2c_transactionInfo[(_id)].dataCount         = (_dataCount);         \
    bspI2c_transactionInfo[(_id)].subAddressCount   = (_dataCount);         \
    bspI2c_transactionInfo[(_id)].fifoCount         = (_dataCount);         \
    bspI2c_transactionInfo[(_id)].dataLeftoverCount = (_dataCount);         \
    bspI2c_transactionInfo[(_id)].subAddrIndex      = 0;                    \
    bspI2c_transactionInfo[(_id)].dataIndex         = 0;                    \
    bspI2c_transactionInfo[(_id)].dmaRequestId      = (_dmaRequestId);      \
    bspI2c_transactionInfo[(_id)].transactionDoneCallback = _callback;      \
    bspI2c_transactionInfo[(_id)].transactionId     = _callbackId;          \
    bspI2c_transactionInfo[(_id)].busControl        = _busControl;          \
}
#else
#define BSP_I2C_MULTI_MASTER_STORE_TRANSACTION_INFO(_id,                   \
        _transType,                                                         \
        _deviceId,                                                          \
        _startAddress,                                                      \
        _dataPtr,                                                           \
        _dataCount,                                                         \
        _callback,                                                          \
        _callbackId,                                                        \
        _busControl)                                                       \
{                                                                           \
    bspI2c_transactionInfo[(_id)].transactionType   = (_transType);         \
    bspI2c_transactionInfo[(_id)].returnCode = BSP_I2C_RETURN_CODE_SUCCESS; \
    bspI2c_transactionInfo[(_id)].deviceId          = (_deviceId);          \
    bspI2c_transactionInfo[(_id)].dataStartAddress  = (_startAddress);      \
    bspI2c_transactionInfo[(_id)].currentAddress    = (_startAddress);      \
    bspI2c_transactionInfo[(_id)].dataPtr           = (_dataPtr);           \
    bspI2c_transactionInfo[(_id)].dataCount         = (_dataCount);         \
    bspI2c_transactionInfo[(_id)].subAddressCount   = (_dataCount);         \
    bspI2c_transactionInfo[(_id)].fifoCount         = (_dataCount);         \
    bspI2c_transactionInfo[(_id)].dataLeftoverCount = (_dataCount);         \
    bspI2c_transactionInfo[(_id)].subAddrIndex      = 0;                    \
    bspI2c_transactionInfo[(_id)].dataIndex         = 0;                    \
    bspI2c_transactionInfo[(_id)].transactionDoneCallback = _callback;      \
    bspI2c_transactionInfo[(_id)].transactionId     = _callbackId;          \
    bspI2c_transactionInfo[(_id)].busControl        = _busControl;          \
}

#endif
/*=============================================================================
 * Description:
 *   This macro sets up a transaction that has been requested
 */
#ifdef Bsp_I2C_DMA_Enable
#define BSP_I2C_MULTI_MASTER_STORE_REQUEST_INFO(_transType,                \
        _deviceId,                                                          \
        _startAddress,                                                      \
        _dataPtr,                                                           \
        _dataCount,                                                         \
        _dmaRequestId,                                                      \
        _callback,                                                          \
        _callbackId,                                                        \
        _delayFunctionPtr,                                                  \
        _delayValue,                                                        \
        _busControl)                                                       \
{                                                                           \
    bspI2c_currentTransactionRequest.transactionType   = (_transType);      \
    bspI2c_currentTransactionRequest.deviceId          = (_deviceId);       \
    bspI2c_currentTransactionRequest.dataStartAddress  = (_startAddress);   \
    bspI2c_currentTransactionRequest.dataPtr           = (_dataPtr);        \
    bspI2c_currentTransactionRequest.dataCount         = (_dataCount);      \
    bspI2c_currentTransactionRequest.dmaRequestId      = (_dmaRequestId);   \
    bspI2c_currentTransactionRequest.transactionDoneCallback = (_callback); \
    bspI2c_currentTransactionRequest.transactionId     = (_callbackId);     \
    bspI2c_currentTransactionRequest.delayFunctionPtr  =(_delayFunctionPtr);\
    bspI2c_currentTransactionRequest.delayValue        = (_delayValue);     \
    bspI2c_currentTransactionRequest.busControl        = (_busControl);     \
}
#else
#define BSP_I2C_MULTI_MASTER_STORE_REQUEST_INFO(_transType,                \
        _deviceId,                                                          \
        _startAddress,                                                      \
        _dataPtr,                                                           \
        _dataCount,                                                         \
        _callback,                                                          \
        _callbackId,                                                        \
        _delayFunctionPtr,                                                  \
        _delayValue,                                                        \
        _busControl)                                                       \
{                                                                           \
    bspI2c_currentTransactionRequest.transactionType   = (_transType);      \
    bspI2c_currentTransactionRequest.deviceId          = (_deviceId);       \
    bspI2c_currentTransactionRequest.dataStartAddress  = (_startAddress);   \
    bspI2c_currentTransactionRequest.dataPtr           = (_dataPtr);        \
    bspI2c_currentTransactionRequest.dataCount         = (_dataCount);      \
    bspI2c_currentTransactionRequest.transactionDoneCallback = (_callback); \
    bspI2c_currentTransactionRequest.transactionId     = (_callbackId);     \
    bspI2c_currentTransactionRequest.delayFunctionPtr  =(_delayFunctionPtr);\
    bspI2c_currentTransactionRequest.delayValue        = (_delayValue);     \
    bspI2c_currentTransactionRequest.busControl        = (_busControl);     \
}

#endif
/*=============================================================================
 * Description:
 *   This macro sets up the Slave Recevie transaction info
 */

#if (LOCOSTO_LITE==0)
#ifdef BSP_I2C_DMA_Enable
#define BSP_I2C_MULTI_MASTER_SLAVE_TRANSACTION_INFO(_id,                   \
        _deviceId,                                                          \
        _dataPtr,                                                           \
        _dataCount,                                                         \
        _dmaRequestId,                                                      \
        _callback)                                                         \
{                                                                           \
    bspI2c_slaveTransactionInfo[(_id)].deviceId     = (_deviceId);          \
    bspI2c_slaveTransactionInfo[(_id)].dataPtr      = (_dataPtr);           \
    bspI2c_slaveTransactionInfo[(_id)].dataCount    = (_dataCount);         \
    bspI2c_slaveTransactionInfo[(_id)].currentCount = 0;                    \
    bspI2c_slaveTransactionInfo[(_id)].dmaRequestId = (_dmaRequestId);      \
    bspI2c_slaveTransactionInfo[(_id)].transactionDoneCallback = _callback; \
}
#else
#define BSP_I2C_MULTI_MASTER_SLAVE_TRANSACTION_INFO(_id,                   \
        _deviceId,                                                          \
        _dataPtr,                                                           \
        _dataCount,                                                         \
        _callback)                                                         \
{                                                                           \
    bspI2c_slaveTransactionInfo[(_id)].deviceId     = (_deviceId);          \
    bspI2c_slaveTransactionInfo[(_id)].dataPtr      = (_dataPtr);           \
    bspI2c_slaveTransactionInfo[(_id)].dataCount    = (_dataCount);         \
    bspI2c_slaveTransactionInfo[(_id)].currentCount = 0;                    \
    bspI2c_slaveTransactionInfo[(_id)].transactionDoneCallback = _callback; \
}
#endif
#endif
/*=============================================================================
 *  Types
 *============================================================================*/

/*===========================================================================*/
/*!
 * @typedef BspI2cMultiMaster_ClockPrescaler
 *
 * @discussion
 * <b> Description </b><br>
 *   These are the prescale clock divisors used in the configuration setup.
 *   This value is Divisor_1 in the following formula:
 *
 *   CLK_FUNC_REF = Master_Clock_Freq/(Divisor_2 + 1)
 *                = (External_Clock_Freq / (Divisor_1 * (Divisor2 + 1)))
 *
 *   SCL_OUT = (CLK_FUNC_REF / 3)
 */
enum
{
    BSP_I2C_MULTI_MASTER_CLOCK_PRESCALER_DIVISOR_1  = 0X0,
    BSP_I2C_MULTI_MASTER_CLOCK_PRESCALER_DIVISOR_2  = 0X1,
    BSP_I2C_MULTI_MASTER_CLOCK_PRESCALER_DIVISOR_4  = 0X3,
    BSP_I2C_MULTI_MASTER_CLOCK_PRESCALER_DIVISOR_8  = 0X7,
    BSP_I2C_MULTI_MASTER_CLOCK_PRESCALER_DIVISOR_16 = 0X15
};
typedef Uint8 BspI2cMultiMaster_ClockPrescaler;



/*===========================================================================*/
/*!
 * @typedef BspI2cMultiMaster_ClockLowPeriod
 *
 * @discussion
 * <b> Description </b><br>
 *   This determines the period of time the I2C clock will stay low.Range 1-256
 *   Note this time value is relative to the ICLK, whereby ICLK is defined:
 *
 *   ICLK = Master_Clock_Freq/(PSC + 1)
 *
 */
typedef Uint8 BspI2cMultiMaster_ClockLowPeriod;

/*===========================================================================*/
/*!
 * @typedef BspI2cMultiMaster_ClockHighPeriod
 *
 * @discussion
 * <b> Description </b><br>
 *   This determines the period of time the I2C clock will stay high.Range 1-256
 *   Note this time value is relative to the ICLK, whereby ICLK is defined:
 *
 *   ICLK = Master_Clock_Freq/(PSC + 1)
 *
 */
typedef Uint8 BspI2cMultiMaster_ClockHighPeriod;

/*=============================================================================
 * Description:
 */
enum
{
    BSP_I2C_INTERRUPT_MODE,
    BSP_I2C_POLLED_MODE
};
typedef Uint8 BspI2cMaster_ModeType;
/*============================================================================
 *Description
 */
typedef Uint16	BspIntC_SourceId;

/*=============================================================================
 * Description:
 */
typedef struct {
   BspI2cMaster_TransactionType  transactionType;
   BspI2c_DeviceId         deviceId;
#ifdef  BSP_I2C_DMA_Enable
   BspI2c_DmaRequestId             dmaRequestId;
#endif
   BspI2c_DataAddress      dataStartAddress;
   BspI2c_DataAddress      currentAddress;
   BspI2c_TransactionId            transactionId;
   BspI2cMaster_busControl                 busControl;
   Uint8                           subAddrIndex;
   Uint8                   subAddress[4];
   BspI2c_DataCount        dataIndex;
   BspI2c_DataCount        dataCount;
   BspI2c_DataCount        subAddressCount;
   BspI2c_TransactionDoneCallback  transactionDoneCallback;
   BspI2c_Data             *dataPtr;
   BspI2c_ReturnCode       returnCode;
   Int32                               fifoCount;
   Int32                               dataLeftoverCount;
} BspI2cMaster_TransactionInfo;

/*=============================================================================
 * Description:
 */
typedef struct {
   BspI2cMultiMaster_ClockPrescaler   clockPrescalerDivFactor;
   BspI2cMultiMaster_ClockLowPeriod   clockLowPeriod;
   BspI2cMultiMaster_ClockHighPeriod  clockHighPeriod;
   BspI2c_ConfigInfo                  configInfo;
} BspI2cMaster_ConfigInfo;

/*=============================================================================
 * Description:
 *
 *  This is the structure that will store queued request information
 *
 */
typedef struct {
   BspI2cMaster_TransactionType            transactionType;
   BspI2c_DeviceId                         deviceId;
   BspI2c_DmaRequestId                     dmaRequestId;
   BspI2c_TransactionId                    transactionId;
   BspI2c_DelayValue                       delayValue;
   BspI2c_DataCount                        dataCount;
   BspI2c_DataAddress                      dataStartAddress;
   BspI2c_Data                             *dataPtr;
   BspI2c_TransactionDoneCallback          transactionDoneCallback;
   BspI2c_DelayFunctionPtr                 delayFunctionPtr;
   BspI2cMaster_busControl                 busControl;
} BspI2cMaster_TransactionRequest;


/*=============================================================================
 * Description:
 *
 *  This is the structure that will store Slave Receive transaction info
 *
 */
typedef struct {
   BspI2c_DeviceId                         deviceId;
   BspI2c_Data                             *dataPtr;
   BspI2c_DataCount                        dataCount;
   BspI2c_DataCount                        currentCount;
#ifdef BSP_I2C_DMA_Enable
   BspI2c_DmaRequestId                     dmaRequestId;
#endif
   BspI2c_slaveTransactionDoneCallback     transactionDoneCallback;
} BspI2cSlave_TransactionInfo;


/*=============================================================================
 * Description:
 *
 *  This is the structure that will store instance specific device info. 
 *
 */
typedef struct {
   Uint32                  *baseAddress;
   BspI2cMaster_ModeType    mode;
   BspIntC_SourceId         sourceId;
   Uint16                   ownAddress;
#ifdef BSP_I2C_DMA_Enable
   BspDma_ChannelId         rxchannel;
   BspDma_ChannelId         txchannel;
#endif  
   Uint8                    busOwner;
   char                 *semaphoreName;

   NU_HISR*		     hisrRefPtr;
   void(*hisr_entry)(void);
} BspI2c_DeviceInfo;

/*=============================================================================
 *   Local Data
 *============================================================================*/
#define I2C_HISR_PRIO	     2
#ifndef HISR_STACK_SHARING
LOCAL Uint8		     I2c_Hisr_Stack[256*3];
#endif
/*=============================================================================
 * Description:
 *
 *  This structure contains instance specific information for I2C devices. 
 */
/* Forward Declaration */
#if (CHIPSET==15)
#if (LOCOSTO_LITE==0)
static  void bspI2c_Hisr1(void);
#endif
static  void bspI2c_Hisr2(void);
#else
#error "This configuration is not supported."
#endif

#if (CHIPSET==15)

NU_HISR  hisrRef[BSP_I2C_NUM_DEVICES];
/*This code should be kept for all devices with 2 Instances of I2C
 * ToDo: change the Interrupt Id Value.
 */

    BspI2c_DeviceInfo bspI2c_Info[BSP_I2C_NUM_DEVICES]={
#if (LOCOSTO_LITE==0)
       {   (Uint32*)BSP_I2C1_MULTI_MASTER_MAP_BASE_ADDRESS,
           BSP_I2C_INTERRUPT_MODE,
           C_INTH_I2C_IT,
           BSP_I2C_DEVICE_ADDRESS_I2C_1,
#ifdef BSP_I2C_DMA_Enable
           C_DMA_CHANNEL_I2C_RX,
           C_DMA_CHANNEL_I2C_TX,
#endif
           0,
           "BLSEM_2",
           NULL,
           bspI2c_Hisr1},
#endif
       {   (Uint32*)BSP_I2C2_MULTI_MASTER_MAP_BASE_ADDRESS,
           BSP_I2C_INTERRUPT_MODE,
           C_INTH_I2C_TRITON_IT,
           BSP_I2C_DEVICE_ADDRESS_I2C_2,
#ifdef BSP_I2C_DMA_Enable
           C_DMA_CHANNEL_I2C_TRITON_RX,
           C_DMA_CHANNEL_I2C_TRITON_TX,
#endif
           0,
           "BLSEM_1",
           NULL,
           bspI2c_Hisr2
       }
    }; 
#else
#error "This configuration is not supported."
#endif

/*=============================================================================
 * Description:
 *
 *  This structure contains all the required information for the currently
 *  active Master transaction.
 */
BspI2cMaster_TransactionInfo bspI2c_transactionInfo[BSP_I2C_NUM_DEVICES];
Uint32 NACK_AL_CNT=0;
/*=============================================================================
 * Description:
 *
 *  This structure contains all the required information for the Slave Receive
 *  transaction.
 */
#if (LOCOSTO_LITE==0)
BspI2cSlave_TransactionInfo bspI2c_slaveTransactionInfo[BSP_I2C_NUM_DEVICES];
#endif
/*=============================================================================
 * Description:
 *
 *  This structure contains all the required information for all of the confirgued
 *  I2C devices.
 */
BspI2cMaster_ConfigInfo bspI2c_deviceInfo[ BSP_I2C_DEVICE_ID_NUM_DEVICES ];

/*=============================================================================
 * Description:
 */
#if (LOCOSTO_LITE==0) 
#define MAX_QUEUED_TRANSACTIONS 100
#else
#define MAX_QUEUED_TRANSACTIONS 50
#endif


BspI2cMaster_TransactionRequest transactionRequestQueue[BSP_I2C_NUM_DEVICES][MAX_QUEUED_TRANSACTIONS ];

Uint8 transactionRequestCount[BSP_I2C_NUM_DEVICES];

Uint8 transactionActiveIndex[BSP_I2C_NUM_DEVICES];

Uint8 transactionAvailableIndex[BSP_I2C_NUM_DEVICES];


/*=============================================================================
 * Description:
 *  Some local flags for managing master/slave state and dma enable/disable
 */
volatile Bool bspI2c_busLocked[BSP_I2C_NUM_DEVICES] = {FALSE};

LOCAL NU_SEMAPHORE *bspI2c_busLockedSemaphore[BSP_I2C_NUM_DEVICES] = {NULL};
LOCAL NU_SEMAPHORE i2cSemArray[BSP_I2C_NUM_DEVICES];
#ifndef BSP_I2C_Q_LOCK_BY_INTERRUPT
LOCAL Bool bspI2c_queueLocked[BSP_I2C_NUM_DEVICES] = {FALSE};
#endif

/*LOCAL BOOL bus_locked_without_semaphore=FALSE;*/

extern int TCD_Interrupt_Count;
#if (LOCOSTO_LITE==0)
#ifdef BSP_I2C_DMA_Enable
LOCAL Bool bspI2c_SlaveReadDMA[BSP_I2C_NUM_DEVICES] = {FALSE};
#endif
#if !((CHIPSET==15))
/* law DEVELOPER NOTE: If you leave the I2C peripheral enabled and in Slave mode
 * read, but with no interrupts enabled (no XRDY, RRDY, ARDY serviced) and you 
 * receive I2C bytes targeted for the I2C peripheral, it will cause CLK to go 
 * low, locking up the bus. A solution to this bad behaviour is to disable the 
 * peripheral when Slave mode has not been enabled (rather tham enabling 
 * interrupts and servicing them to do nothing (wasted bandwidth).
 * Unfortunately this does not work on the OMAP I2C Peripheral, reasons unknown.
 * The symptom is when you enable the I2C, both CLK and DATA go low
 */ 

LOCAL Bool bspI2c_SlaveOn[BSP_I2C_NUM_DEVICES] = {FALSE};
#else
#ifdef Bsp_I2C_SLAVE_SUPPORT
LOCAL Bool bspI2c_SlaveOn[BSP_I2C_NUM_DEVICES] = {FALSE};
#endif
#endif
#endif

#if CHIPSET == 15
Uint16 BspI2cError=0;
int sleepFlag=1;
extern void i2c_vote_deepsleepstatus(void);
#endif

/*=============================================================================
 *   Local Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 * This Function sets up the clock parameter for the device with which 
 * transaction is going to be started.
 */
    static void
bspI2cMultiMaster_configDevice(BspI2c_InstanceId id,BspI2c_DeviceId deviceId)
{
    /* Instead of storing this three parameter with each device we should
     * have a structure for 400KHZ and 100KHZ an should get the values from 
     * there.
     */

    Uint32 *baseAddress = bspI2c_Info[id].baseAddress;

    /* Initialize OA ("Own Address") here*/

    BSP_I2C_MULTI_MASTER_OWN_ADDRESS_SET (baseAddress,bspI2c_Info[id].ownAddress);

    /* Setup the clock data that goes into PSC, SCLL and SCLH regs */

    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_PSC,
            BSP_I2C_MULTI_MASTER_MAP_PSC_PTR(baseAddress),
            bspI2c_deviceInfo[ deviceId ].clockPrescalerDivFactor);
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_SCLL,
            BSP_I2C_MULTI_MASTER_MAP_SCLL_PTR(baseAddress),
            bspI2c_deviceInfo[ deviceId ].clockLowPeriod);
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_SCLH,
            BSP_I2C_MULTI_MASTER_MAP_SCLH_PTR(baseAddress),
            bspI2c_deviceInfo[ deviceId ].clockHighPeriod);

    /* Set the Slave Address */

    BSP_I2C_MULTI_MASTER_SLAVE_ADDRESS_SET(id,
            bspI2c_deviceInfo[ deviceId ].configInfo.deviceAddress);
    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_COMPLETE_REG,   
            BSP_I2C_MULTI_MASTER_MAP_CON_PTR(baseAddress), BSP_I2C_MULTI_MASTER_LLIF_CON_MST_ENABLED);          



    return;
}


/*=============================================================================
 * Description:
 * Wait in a tight loop till the bus is busy.
 */

    static void
bspI2cMultiMaster_waitNotBusy(BspI2c_InstanceId id, Uint32 *baseAddress)
{
    volatile Uint16 tmpReg2;

    /* Wait for the I2C Bus to NOT be busy */

    do
    {
        tmpReg2 = BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),  
                BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_MASK);

    } while (tmpReg2 == BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_BUS_BUSY);
}

/*=============================================================================
 * Description:
 * Setup the slave device internal address. If the transaction is Write-Read
 * or Read then put the address in FIFO else store in transaction array so that
 * it can be packed with data to be written.
 */

static BspI2c_DataCount
bspI2cMultiMaster_setAddress(BspI2c_InstanceId id,
        BspI2c_ConfigInfo * config,
        BspI2c_DataAddress address,
        Uint32 * baseAddress
        )
{
    BspI2c_DataCount    fifoCount;
    Uint16  endianSwapAddr=0;
    int                 i;


    /* For Write-Read or Read transactions, go ahead and load subaddress into 
     *  Write FIFO since no other Write FIFO loads will need to be performed 
     */
    if (bspI2c_transactionInfo[id].transactionType == 
            BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ ||
            bspI2c_transactionInfo[id].transactionType == 
            BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE_READ)   {

       if (config->addressOctetLength == BSP_I2C_ADDRESS_OCTET_LENGTH_1)   { 

          BSPUTIL_BITUTIL_SET16_ONLY(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress),
                  (Uint16)(address));

          fifoCount = 1;
       }
       else if (config->addressOctetLength == BSP_I2C_ADDRESS_OCTET_LENGTH_2)   {
          if (config->addressByteOrder == 
                  BSP_I2C_ADDRESS_ORDER_HIGH_TO_LOW)   {

#ifdef BSP_I2C_BE_ENABLE		  
             endianSwapAddr =  (address << 8)|(address>>8);
#endif

             BSPUTIL_BITUTIL_SET16_ONLY(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress),
                     (Uint16)(endianSwapAddr));
          }
          else   {
             BSPUTIL_BITUTIL_SET16_ONLY(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress),
                     (Uint16)(address & 0xffff));
          }
          fifoCount = 2;

       }
       else   {



          BSPUTIL_BITUTIL_SET16_ONLY(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress),
                  (Uint16)(address & 0xffff));

          BSPUTIL_BITUTIL_SET16_ONLY(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress),
                  (Uint16)(((Uint32)address << 8)));
          fifoCount = 4;
       }
    }
    /* Note that even though address is only 8bits, register is 16-bits
     * long Fortunately all of our slave devices that require an address
     * (actually a sub-address), also require another start condition. 
     * If we were to make this more generic, we would have to worry
     * about any proceeding actual data needing to go into the upper 
     * byte of the FIFO
     */

    /* For Write transactions, buffer what will be loaded into the Write FIFO 
     *  since it needs to be packed with subsequent Write data 
     */

    else   {
       if (config->addressOctetLength == 
               BSP_I2C_ADDRESS_OCTET_LENGTH_1)   {
          fifoCount = 1;
          for (i = 0; i < fifoCount ; i++)   {
             bspI2c_transactionInfo[id].subAddress[ i ] = 
             (Uint8)(address >> (i*8));
          }
       }
       else if (config->addressOctetLength == 
               BSP_I2C_ADDRESS_OCTET_LENGTH_2)   {
          fifoCount = 2;
          for (i = 0; i < fifoCount ; i++)   {
             if (config->addressByteOrder 
                     == BSP_I2C_ADDRESS_ORDER_HIGH_TO_LOW)   {
                bspI2c_transactionInfo[id].subAddress[ i ] = 
                (Uint8)(address >> ((1-i)*8));
             }
             else   {
                bspI2c_transactionInfo[id].subAddress[ i ] = 
                (Uint8)(address >> (i*8));
             }
          }
       }
       else   {
          fifoCount = 1;
          for (i = 0; i < fifoCount ; i++)   {
             bspI2c_transactionInfo[id].subAddress[ i ] = 
             (Uint8)(address >> (i*8));
          }
       }
    }

    return(fifoCount);
}


/*=============================================================================
 * Description:
 * This function write to FIFO sub-address if any followed by the data to be 
 * written.
 */

    static void
bspI2cMultiMaster_WriteFifo(BspI2c_InstanceId id,BspI2c_ConfigInfo * config,Uint32* baseAddress)
{

    /* Pack 8-bit data into 16-bit FIFO, including slave subaddress byte(s) 
    */

    if (bspI2c_transactionInfo[id].subAddrIndex == 0 && 
            config->addressOctetLength == BSP_I2C_ADDRESS_OCTET_LENGTH_1)  {
       BSPUTIL_BITUTIL_SET16_ONLY(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress),
               ((Uint16) bspI2c_transactionInfo[id].subAddress 
                [ bspI2c_transactionInfo[id].subAddrIndex ] + 
                ((Uint16)bspI2c_transactionInfo[id].dataPtr
                 [ bspI2c_transactionInfo[id].dataIndex ] << 8))); 

       bspI2c_transactionInfo[id].dataIndex = 1;
    }
    else if (bspI2c_transactionInfo[id].subAddrIndex == 0 &&
            config->addressOctetLength == BSP_I2C_ADDRESS_OCTET_LENGTH_2)   {
       BSPUTIL_BITUTIL_SET16_ONLY(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress),
               ((Uint16)bspI2c_transactionInfo[id].subAddress
                [ bspI2c_transactionInfo[id].subAddrIndex ] + 
                ((Uint16)bspI2c_transactionInfo[id].subAddress
                 [ bspI2c_transactionInfo[id].subAddrIndex + 1 ] << 8)));
    }
    else if ((bspI2c_transactionInfo[id].subAddrIndex == 0 || 
                bspI2c_transactionInfo[id].subAddrIndex == 2) &&
            config->addressOctetLength == BSP_I2C_ADDRESS_OCTET_LENGTH_4)   {
       BSPUTIL_BITUTIL_SET16_ONLY(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress),
               ((Uint16)bspI2c_transactionInfo[id].subAddress
                [ bspI2c_transactionInfo[id].subAddrIndex ] + 
                ((Uint16)bspI2c_transactionInfo[id].subAddress
                 [ bspI2c_transactionInfo[id].subAddrIndex + 1 ] << 8)));
    }
    else   {

       (*((volatile Uint16 *)(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress)))) =  (Uint16)(((Uint16)bspI2c_transactionInfo[id].dataPtr
                                                                                             [ bspI2c_transactionInfo[id].dataIndex ] + 
                                                                                             ((Uint16)bspI2c_transactionInfo[id].dataPtr
                                                                                              [ bspI2c_transactionInfo[id].dataIndex + 1 ] << 8)));
       bspI2c_transactionInfo[id].dataIndex += 2;
    }

    bspI2c_transactionInfo[id].subAddrIndex += 2;
    bspI2c_transactionInfo[id].fifoCount -= 2;
    return;
}

/*=============================================================================
 * Description:
 */

    static void
bspI2cMultiMaster_doRead(BspI2c_InstanceId id,BspI2c_DeviceId deviceId)
{
    volatile Uint16         tmpReg2;
    BspI2c_DataCount        bytesToRead;

    Uint32 *baseAddress = bspI2c_Info[id].baseAddress;

#ifdef Bsp_I2C_DMA_Enable
    /* Add relevant DMA Code here for TCS3.x */
    BspDma_ChannelConfiguration chanConfigI2CRead = {
       BSP_DMA_DATA_TYPE_S16,
       BSP_DMA_ADDRESSING_MODE_CONSTANT,
       BSP_DMA_ADDRESSING_MODE_POST_INCREMENT,
       0, 
       BSP_DMA_BURST_MODE_SINGLE,
       BSP_DMA_BURST_MODE_SINGLE,
       0,
       FALSE,
       FALSE,
       FALSE,
       TRUE,
       1,
       NULL
    };
#endif


    /* At this point the address has been sent, so we can switch back to a read 
     *  operation and read the next location 
     */

    bspI2c_transactionInfo[id].transactionType = 
    BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ;

    /* Set the Data Counter Register */

    if (bspI2c_deviceInfo[ bspI2c_transactionInfo[id].deviceId ].configInfo.multiByteRead 
            == BSP_I2C_MULTI_BYTE_READ_SUPPORTED)   {
       bytesToRead = bspI2c_transactionInfo[id].dataLeftoverCount;
    }
    else   {
       bytesToRead = 1;
    }

    BSP_I2C_MULTI_MASTER_DCOUNT_SET(baseAddress,bytesToRead);
    /* If The device doesn't support multi byte read then we need 
     *  multiple Write Read transaction 
     */

    if (bytesToRead == bspI2c_transactionInfo[id].dataLeftoverCount
            &&  bspI2c_transactionInfo[id].busControl == BSP_I2C_BUS_RELEASE)   {
       BSP_I2C_MULTI_MASTER_TRIGGER_READ(baseAddress,id)
    }
    else   {
       BSP_I2C_MULTI_MASTER_TRIGGER_READ_NO_STOP(baseAddress,id)
    }	

#ifdef Bsp_I2C_DMA_Enable
    if (bspI2c_transactionInfo[id].dmaRequestId == BSP_DMA_REQ_NONE)   {


    }
    else   {


       /* FIXME TBD DMA porting for TCS3.x */
       /* DMA driven Read */

       /* Setup DMA Controller */

       /* If It is DMA Operation then Device Should Support multi byte read */ 
       BSPUTIL_ASSERT(bytesToRead == 
               bspI2c_transactionInfo[id].dataLeftoverCount);

       /* Due to fact that FIFO is always 16-bit, dataCount must be even for
          DMA transactions. */

       BSPUTIL_ASSERT(!(bspI2c_transactionInfo[id].dataCount & 0x01));

       chanConfigI2CRead.dmaRequestId = 
       bspI2c_transactionInfo[id].dmaRequestId;

       /* Set this transaction tracker to 0 since DMA can't decrement. 
          This allows ARDY int handler to do the right thing */
       bspI2c_transactionInfo[id].dataLeftoverCount = 0;

       /* Configure DMA channel */

       bspDma_channelConfig(bspI2c_Info[id].rxchannel,&chanConfigI2CRead);

       bspDma_startTransfer(bspI2c_Info[id].rxchannel,
               (void *)(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress)),
               bspI2c_transactionInfo[id].dataPtr,
               bytesToRead);

       /* Trigger Read start on I2C */
       BSP_I2C_MULTI_MASTER_TRIGGER_DMA_READ(id);
    } 

#endif
}

#ifdef BSP_I2C_Q_LOCK_BY_INTERRUPT
BSP_I2C_MULTI_MASTER_DEFINE_LOCK_STATE_VAR(bspI2cQLockState);
#endif

/*=============================================================================
 * Description:
 *   This function will wait for the queue lock to be obtained before
 *   returning.
 */

    static void
bspI2cMultiMaster_queue_lock(BspI2c_InstanceId id)   
{

    BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(bspI2cQLockState);
    return;
}

/*=============================================================================
 * Description:
 *   This function will unlock the I2C queue.
 */
    static void
bspI2cMultiMaster_queue_unlock(BspI2c_InstanceId id)
{

    BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(bspI2cQLockState);
    return;
}


/*=============================================================================
 * Description:
 *   This function will wait for the transaction lock to be obtained before
 *   returning.
 */
    static void
bspI2cMultiMaster_lock(BspI2c_InstanceId id, Bool bspI2c_inInterruptContext)
{
    Bool gotLock;
    BSP_I2C_MULTI_MASTER_DEFINE_LOCK_STATE_VAR(lockState);
    Uint32 *baseAddress = bspI2c_Info[id].baseAddress;
    gotLock = FALSE;


    /* 
     * Check whether the function is called from interrupt (LISR) context.
     * 
     * If TRUE, do not attempt semaphore lock.
     * 
     * If FALSE, attempt a semaphore lock before obtaining the bspI2c_busLocked
     * flag.
     */
    if (FALSE == bspI2c_inInterruptContext)
    {
        /* Obtain the semaphore only if you are in task context*/
#if(OP_L1_STANDALONE==0)
        if ((TCD_Interrupt_Count == 0) && (0 != os_MyHandle()) && (NU_NULL == NU_Current_HISR_Pointer()))
            NU_Obtain_Semaphore(bspI2c_busLockedSemaphore[id], NU_SUSPEND);
#endif
#if(OP_L1_STANDALONE==1)
        if ((TCD_Interrupt_Count == 0) && (NU_NULL == NU_Current_HISR_Pointer()))
            NU_Obtain_Semaphore(bspI2c_busLockedSemaphore[id], NU_SUSPEND);
#endif

    }

    while(gotLock == FALSE)
    {
        while( bspI2c_busLocked[id] == TRUE )
        {
        	gsp_delay(1);
        }

        BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState);

        /* Still need to check if it's available because there's
         * a slight chance that we lost it before we disabled
         * interrupts
         */
        if( bspI2c_busLocked[id]== FALSE )
        {
            bspI2c_busLocked[id]= TRUE;
            gotLock=TRUE;

        }
        BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);
    }
    if (FALSE == bspI2c_inInterruptContext)
    {
        /* Release the semaphore */
        //                   vsi_s_release(0, bspI2c_busLockedSemaphore[id]);
        /*#if(OP_L1_STANDALONE==0)
          if ((TCD_Interrupt_Count == 0) && (0 != os_MyHandle()) && (NU_NULL == NU_Current_HISR_Pointer()))
          NU_Release_Semaphore(bspI2c_busLockedSemaphore[id]);
#endif
#if(OP_L1_STANDALONE==1)
if ((TCD_Interrupt_Count == 0) && (NU_NULL == NU_Current_HISR_Pointer()))
NU_Release_Semaphore(bspI2c_busLockedSemaphore[id]);
#endif*/

    }
    BSP_I2C_MULTI_MASTER_LLIF_SET( BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN,
            BSP_I2C_MULTI_MASTER_MAP_CON_PTR(baseAddress),
            BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_ENABLED );
    return;

}

/*=============================================================================
 * Description:
 *   This function will unlock the I2C bus.
 */

    static void
bspI2cMultiMaster_unlock(BspI2c_InstanceId id, Bool bspI2c_inInterruptContext)
{
    BSP_I2C_MULTI_MASTER_DEFINE_LOCK_STATE_VAR(lockState);
    Uint32 *baseAddress = bspI2c_Info[id].baseAddress;

    /*if (!bspI2c_SlaveOn[id])   */
    /*   Always force it to turn off untill slave is supported*/


#ifndef Bsp_I2C_SLAVE_SUPPORT


    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN,
            BSP_I2C_MULTI_MASTER_MAP_CON_PTR(baseAddress),
            BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_DISABLED);
    BSPUTIL_BITUTIL_BIT_SINGLE_CLEAR16(BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),
            ~((BSP_I2C_MULTI_MASTER_LLIF_IE_ARDY_MASK
                    |BSP_I2C_MULTI_MASTER_LLIF_IE_XRDY_MASK
                    |BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY_MASK
                    |BSP_I2C_MULTI_MASTER_LLIF_IE_NACK_MASK
                    |BSP_I2C_MULTI_MASTER_LLIF_IE_AL_MASK)));


#else
    if(!bspI2c_SlaveOn[id])	{   

       /* If Slave not on, turn off I2C peripheral.
        * This prevents any transactions targeted to this peripheral from
        * holding CLK low (a bad behaviour from this peripheral, but there it is)
        */

       BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN,
               BSP_I2C_MULTI_MASTER_MAP_CON_PTR(baseAddress),
               BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_DISABLED);
    }
    else   {

       /* Turn I2C peripheral on */

       BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_EN,
               BSP_I2C_MULTI_MASTER_MAP_CON_PTR(baseAddress),
               BSP_I2C_MULTI_MASTER_LLIF_CON_I2C_ENABLED);

#ifdef BSP_I2C_DMA_Enable
       /* If Slave Read DMA enabled, turn it on and turn off RRDY */

       if (bspI2c_SlaveReadDMA[id])   {
          BSP_I2C_MULTI_MASTER_DMA_READ_ENABLE(id);
          BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY,
                  BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),
                  BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_DISABLED);
       }

       else if(bspI2c_Info[id].mode == BSP_I2C_INTERRUPT_MODE)   {
          BSP_I2C_MULTI_MASTER_DMA_READ_DISABLE(id);
          BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY,
                  BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),
                  BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_ENABLED);
       }
#endif	   
    }


#ifdef Bsp_I2C_DMA_Enable 
    /* Disable XDMA_ENA while in slave mode */

    BSP_I2C_MULTI_MASTER_DMA_WRITE_DISABLE(id);

    /* Put into Read mode */

    BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_CON_TRX,
            BSP_I2C_MULTI_MASTER_MAP_CON_PTR(baseAddress),
            BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_READ);
#endif

#endif
    /* 
     * Check whether the function is called from interrupt (LISR) context.
     * 
     * If TRUE, do not attempt semaphore lock.
     * 
     * If FALSE, attempt a semaphore lock before obtaining the bspI2c_busLocked
     * flag.
     */

    BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState);

    bspI2c_busLocked[id]= FALSE;

    BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);
    if (FALSE == bspI2c_inInterruptContext)   {

#if(OP_L1_STANDALONE==0)

       if ((TCD_Interrupt_Count == 0) && (0 != os_MyHandle()) && (NU_NULL == NU_Current_HISR_Pointer()))
           NU_Release_Semaphore(bspI2c_busLockedSemaphore[id]);
#endif
#if(OP_L1_STANDALONE==1)

       if ((TCD_Interrupt_Count == 0)  && (NU_NULL == NU_Current_HISR_Pointer()))
           NU_Release_Semaphore(bspI2c_busLockedSemaphore[id]);
#endif

    }

    return;
}


/*************************************************************************/
    static void
bspI2c_castVoteInDeepSleepElection()
{
    /* FIXME Port Power Management to Locosto */

    Uint32 temp;
    Uint8 transactionsQueuedUp = 0;

    BSP_I2C_MULTI_MASTER_DEFINE_LOCK_STATE_VAR(i2cSleepVoteLock);

    BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(i2cSleepVoteLock);

    /* Loop through all the transaction queue counters for all the i2c devices.
     * If a transaction queue counter value is > 0 then there are pending 
     * transactions and a vote is cast against deep sleep.
     * If there are no pending transactions then a vote is cast in favour
     * of deep sleep.
     */

    for(temp =0; temp < BSP_I2C_NUM_DEVICES; temp++)   {
       transactionsQueuedUp += ((transactionRequestCount[temp] > 0) ? 1:0);
    }
    if(transactionsQueuedUp > 0)   {
       sleepFlag = 1;
#if (L1_POWER_MGT != 0x00)
       i2c_vote_deepsleepstatus();
#endif
    }
    else
        sleepFlag = 0;

    BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(i2cSleepVoteLock);
}

/***************************************************************************/

#ifdef Bsp_I2C_DMA_Enable

    static void
i2cInternalDMAWriteCallback(  BspDma_ChannelEvent channelEvent,
        BspDma_ChannelId    channelId)
{
    BspI2c_DeviceId id;
    for(id=0;id < BSP_I2C_NUM_DEVICES;id++)   {
       if(bspI2c_Info[id].txchannel == channelId)   {
          break;
       }
    }
    /* ToDo: */
    /* Turn off channel transfer */
    bspDma_stopChannelTransfer(bspI2c_transactionInfo[id].dmaRequestId);

}
#endif

extern Uint32 BspI2cTickCounter;
void Bsp_I2c_get_tick(Uint32 * Counter)
{

    if(BspI2cTickCounter==0xFFFF-(TICKS_TO_WAIT*2))  {

       BspI2cTickCounter=0;

    }

    *Counter=BspI2cTickCounter;

    return;
}
/*=============================================================================
 * Description:
 *   This function will Intiate the write transaction.
 *   Assumption:
 *   1. Address_Auto_Increment is supported (for Polled mode).
 *   
 */

    static BspI2c_Result
bspI2c_lowLevelwrite(BspI2c_InstanceId id, 
        BspI2cMaster_TransactionRequest *transactionRequest)
{
    BspI2c_DataCount   fifoDataCount;
    Uint32             loads;
    Uint16             tmpReg2;


    Uint32             *baseAddress = bspI2c_Info[id].baseAddress;
    BspI2c_ConfigInfo  *config;

    volatile  Uint32 Counter=0;	

#ifdef Bsp_I2C_DMA_Enable 
    /* FIXME Port DMA */
    BspDma_ChannelConfiguration chanConfigI2CWrite = {
       BSP_DMA_DATA_TYPE_S16,
       BSP_DMA_ADDRESSING_MODE_POST_INCREMENT,
       BSP_DMA_ADDRESSING_MODE_CONSTANT,
       0, 
       BSP_DMA_BURST_MODE_SINGLE,
       BSP_DMA_BURST_MODE_SINGLE,
       0,
       FALSE,
       FALSE,
       FALSE,
       TRUE,
       1,
       NULL
    };
#endif



    /* Port Power Management */
    /* vote against deep sleep */

    sleepFlag = 1;


    /* Configure the I2C for the passed device */

    bspI2cMultiMaster_configDevice(id, transactionRequest->deviceId);

    /* Store off the Transaction information */

    BSP_I2C_MULTI_MASTER_STORE_TRANSACTION_INFO(id,
            BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE,
            transactionRequest->deviceId,
            transactionRequest->dataStartAddress,
            transactionRequest->dataPtr,
            transactionRequest->dataCount,
            transactionRequest->transactionDoneCallback,
            transactionRequest->transactionId,
            transactionRequest->busControl);
    config = &bspI2c_deviceInfo[ transactionRequest->deviceId ].configInfo;

    /* Determine number of bytes that can be transferred for a write.
     *  Note that if the slave device cannot support "address auto increment", 
     *  that for writes all we can do is transfer the slave subaddress bytes and
     *  a single data byte.
     *  If the slave device does support "address auto increment" then we 
     *  transfer starting subaddress bytes and as many data bytes as are 
     *  specified (or max of 65536).  
     *  ASSUME  THAT dataCount DOES NOT EXCEED 65536-4 (4: MAX SUBADDRESS BYTES) 
     */

    if (config->addressAutoIncrement==BSP_I2C_ADDRESS_AUTO_INCREMENT_SUPPORTED)   {
       fifoDataCount = transactionRequest->dataCount;
    }
    else   {
       fifoDataCount = 1;
    }

    /* Cache slave subaddress byte(s) since they need to be packed with data 
     * into FIFO 
     */

    bspI2c_transactionInfo[id].subAddressCount =
    bspI2cMultiMaster_setAddress(id,config,
            bspI2c_transactionInfo[id].currentAddress,baseAddress);

    /* Keep track of how many bytes are being written to the write FIFO */

    bspI2c_transactionInfo[id].fifoCount = fifoDataCount + 
    bspI2c_transactionInfo[id].subAddressCount;

    BSP_I2C_MULTI_MASTER_DCOUNT_SET(baseAddress,
            bspI2c_transactionInfo[id].fifoCount);	
#ifdef BSP_I2C_DMA_ENABLE
    if (bspI2c_transactionInfo[id].dmaRequestId == BSP_DMA_REQ_NONE)   {

       /* Set the Data Counter Register */

       BSP_I2C_MULTI_MASTER_DCOUNT_SET(baseAddress,
               bspI2c_transactionInfo[id].fifoCount);
    }
#endif
    /* Verify Bus Availability 
     * If BusBusy, return deferred status, otherwise start transaction 
     */

    if ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_MASK) 
                !=0) 
            && (!bspI2c_Info[id].busOwner))   {
       if(bspI2c_Info[id].mode == BSP_I2C_INTERRUPT_MODE)   {
          return BSP_I2C_RETURN_CODE_SUCCESS_OPERATION_NOT_COMPLETE;
       }

       /* Polled Mode: Busy waiting for bus to get free */

       bspI2cMultiMaster_waitNotBusy(id,baseAddress);

    }


    /* DMA Write requires that the entire transaction must start with DMA 
     *  (since the DMA needs to be setup before any XRDY occurs. This implies 
     *  that the dataPtr must include the slave subaddress byte(s).
     *  For now, we leave this to the client to provide in the passed dataPtr 
     *  field. 
     */
#ifdef Bsp_I2C_DMA_Enable
    if (bspI2c_transactionInfo[id].dmaRequestId != BSP_DMA_REQ_NONE)   {

       /* FIXME Port DMA */
       chanConfigI2CWrite.dmaRequestId = 
       bspI2c_transactionInfo[id].dmaRequestId;
       chanConfigI2CWrite.channelEventCallback = i2cInternalDMAWriteCallback;

       /* If It is DMA Operation then Device Should Support Address auto inc */
       BSPUTIL_ASSERT(fifoDataCount == transactionRequest->dataCount);
       /* Due to fact that FIFO is always 16-bit, dataCount must be even for 
          DMA transactions */
       BSPUTIL_ASSERT(!(bspI2c_transactionInfo[id].dataCount & 0x01));

       BSP_I2C_MULTI_MASTER_DCOUNT_SET(id,
               bspI2c_transactionInfo[id].dataCount);
       /* Configure DMA channel */
       bspDma_channelConfig(bspI2c_Info[id].txchannel, &chanConfigI2CWrite);

       BSP_I2C_MULTI_MASTER_DMA_WRITE_ENABLE(id);
       bspDma_startTransfer(bspI2c_Info[id].txchannel,
               bspI2c_transactionInfo[id].dataPtr,
               (void *)(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress)),
               bspI2c_transactionInfo[id].dataCount);

       /* Turn on Interrupts */
       BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_ARDY,
               BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),
               BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_ENABLED);
       BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY,
               BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),
               BSP_I2C_MULTI_MASTER_LLIF_IE_IRQ_ENABLED);

       /* Trigger the operation 
        * Sub Address followed by Data will be written by the DMA.
        */
       if (bspI2c_transactionInfo[id].busControl ==  BSP_I2C_BUS_RELEASE)   {
          BSP_I2C_MULTI_MASTER_TRIGGER_WRITE(baseAddress,id);
       }

       else  {

          BSP_I2C_MULTI_MASTER_TRIGGER_WRITE_NO_STOP(baseAddress,id);

       }


       while(1);
    }
#endif		


    if(bspI2c_Info[id].mode == BSP_I2C_INTERRUPT_MODE)   {

       /* Rest of the Bytes will be written in the interrupt handler */
       /* Turn on Interrupts */

       BSPUTIL_BITUTIL_BIT_SINGLE_SET16(BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),
               (BSP_I2C_MULTI_MASTER_LLIF_IE_ARDY_MASK
                |BSP_I2C_MULTI_MASTER_LLIF_IE_XRDY_MASK
                |BSP_I2C_MULTI_MASTER_LLIF_IE_NACK_MASK
                |BSP_I2C_MULTI_MASTER_LLIF_IE_AL_MASK));

    }
    if(fifoDataCount != transactionRequest->dataCount
            || bspI2c_transactionInfo[id].busControl ==  BSP_I2C_BUS_NO_RELEASE)   {

       /* Auto Increment Not supported  or transaction is part of a 
        * scenario and bus is not be freed
        */

       BSP_I2C_MULTI_MASTER_TRIGGER_WRITE_NO_STOP(baseAddress,id)


       if(bspI2c_Info[id].mode == BSP_I2C_INTERRUPT_MODE)   {

          return BSP_I2C_RETURN_CODE_SUCCESS;
       }
    }
    else  {

       /* Trigger the operation */

       BSP_I2C_MULTI_MASTER_TRIGGER_WRITE(baseAddress,id);
       if(bspI2c_Info[id].mode == BSP_I2C_INTERRUPT_MODE)   {

          return BSP_I2C_RETURN_CODE_SUCCESS;

       }
    }



    do
    {

        Bsp_I2c_get_tick((Uint32*)(&Counter));

        while(((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                            (BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK)))==BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE)
                ||((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                            (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL))	{

           if(BspI2cTickCounter>(Counter+5))	{
              return BSP_I2C_TRANSACTION_FAILED;
           }
        }

        /*See if the while loop exited becasue of NACK or AL, if YES return Failed status*/

        if((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                        (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
                BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL)   {

           /*store the value in an error variable*/	
           BspI2cError=*(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress));

           /*Clear Nack or AL bits*/
           *(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress))&=(BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK|
                   BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK); 

           return BSP_I2C_TRANSACTION_FAILED;

        }

        if((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                        (BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK)))!=BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_FALSE)  {

           break;

        }

        /*XRDY should be set by now*/

        Bsp_I2c_get_tick((Uint32 *)(&Counter));

        while(BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK)
                ==BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE)	   {

           if(BspI2cTickCounter>(Counter+TICKS_TO_WAIT))	{

              return BSP_I2C_TRANSACTION_FAILED;

           }
        }

        /* Write FIFO ready, go fill */

        for (loads = 0;
                loads < 2 && bspI2c_transactionInfo[id].fifoCount > 0;
                (loads+=2))   {
           bspI2cMultiMaster_WriteFifo(id,config,baseAddress);        
        }

        /* Clear XRDY bit */

        BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                BSP_I2C_XRDY_CLEAR_VALUE);

    } while (1);

    /*ARDY should be set by now*/

    while ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK)) == BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_FALSE);  

    /* Clear the ARDY bit */

    BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
            BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK);


    /* cast vote in deep sleep election */

    bspI2c_castVoteInDeepSleepElection();

    return BSP_I2C_RETURN_CODE_SUCCESS;
}

/*=============================================================================
 * Description:
 *   This function will Intiate the read transaction 
 *   Assumption:
 *   1. Address_Auto_Increment is supported.
 *   2. Multi Byte Read is Supported.
 */

    static BspI2c_Result
bspI2c_lowLevelread(BspI2c_InstanceId id,
        BspI2cMaster_TransactionRequest *transactionRequest)
{

    Uint32                  *baseAddress = bspI2c_Info[id].baseAddress;
    BspI2c_ConfigInfo       *config;
    Uint16                  tmpReg2;

    volatile Uint32 Counter =0;

    /* Port Power Management */
    /* vote against deep sleep */
    sleepFlag = 1;

    config = &bspI2c_deviceInfo[ transactionRequest->deviceId ].configInfo;

    /* Configure the I2C for the passed device */

    bspI2cMultiMaster_configDevice(id, transactionRequest->deviceId);


    /* Perform a write the just the subaddress bytes */

    BSP_I2C_MULTI_MASTER_STORE_TRANSACTION_INFO(id,
            BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE_READ,
            transactionRequest->deviceId,
            transactionRequest->dataStartAddress,
            transactionRequest->dataPtr,
            transactionRequest->dataCount,
            transactionRequest->transactionDoneCallback,
            transactionRequest->transactionId,
            transactionRequest->busControl);

    /* Set the Data Counter Register */

    BSP_I2C_MULTI_MASTER_DCOUNT_SET(baseAddress, config->addressOctetLength);

    /* Verify Bus Availability */

    if ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_BB_MASK)) 
            && (!bspI2c_Info[id].busOwner))   {
       if(bspI2c_Info[id].mode == BSP_I2C_INTERRUPT_MODE)   {
          return BSP_I2C_RETURN_CODE_SUCCESS_OPERATION_NOT_COMPLETE;
       }

       /* Polled Mode: Busy waiting for bus to get free */

       bspI2cMultiMaster_waitNotBusy(id,baseAddress);
    }

    if(bspI2c_Info[id].mode == BSP_I2C_INTERRUPT_MODE)   {

       BSPUTIL_BITUTIL_BIT_SINGLE_SET16(BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),
               (BSP_I2C_MULTI_MASTER_LLIF_IE_ARDY_MASK
                |BSP_I2C_MULTI_MASTER_LLIF_IE_XRDY_MASK
                |BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY_MASK
                |BSP_I2C_MULTI_MASTER_LLIF_IE_NACK_MASK
                |BSP_I2C_MULTI_MASTER_LLIF_IE_AL_MASK));

    }

    BSP_I2C_MULTI_MASTER_TRIGGER_WRITE(baseAddress,id)

    if(bspI2c_Info[id].mode == BSP_I2C_INTERRUPT_MODE)   {

       /* The Bytes Will be Actually written to FIFO on recieving XRDY 
        * interrupt.
        */

       return BSP_I2C_RETURN_CODE_SUCCESS;
    }

    /* See That Address Write Transaction Succeded */
    /* Wait for XRDY,NACK,AL */

    Bsp_I2c_get_tick((Uint32*)(&Counter));

    while ((((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                            (BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK)))==BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE)
                ||((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                            (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL)))  {

       if(BspI2cTickCounter>(Counter+TICKS_TO_WAIT))	{

          return BSP_I2C_TRANSACTION_FAILED;

       }
    }

    if((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                    (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
            BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL)   {


       /*store the value in an error variable*/	
       BspI2cError=*(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress));

       /*Clear Nack or AL bits*/
       *(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress))&=(BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK|
               BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK); 
       return BSP_I2C_TRANSACTION_FAILED;
    }


    /* XRDY should be setby now */

    while((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                    (BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK)))==
            BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE);

    /* Set the subaddress byte(s) into the FIFO */

    bspI2cMultiMaster_setAddress(id,config, 
            bspI2c_transactionInfo[id].currentAddress,baseAddress);

    /* Clear XRDY status */

    BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
            BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK);


    /* Wait for Write, no Stop to complete ,ARDY to be set */

    while (BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK)
            == BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_FALSE);

    /* Clear the ARDY bit */

    BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
            BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK);

    bspI2c_transactionInfo[id].transactionType = 
    BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ;

    /* Assuming MultiByte Read Support */

    BSP_I2C_MULTI_MASTER_DCOUNT_SET(baseAddress,transactionRequest->dataCount);

    /* If in the middle of a scenario or not */

    if (transactionRequest->busControl == BSP_I2C_BUS_RELEASE)   {       
       BSP_I2C_MULTI_MASTER_TRIGGER_READ(baseAddress,id);
    }
    else   {
       BSP_I2C_MULTI_MASTER_TRIGGER_READ_NO_STOP(baseAddress,id);
    }

    do
    {
        Bsp_I2c_get_tick((Uint32*)(&Counter)); 

        while(((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                            (BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_MASK)))==BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_FALSE)
                ||((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                            (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL))	{

           if(BspI2cTickCounter>(Counter+5))	{

              return BSP_I2C_TRANSACTION_FAILED;

           }
        }

        /*If NACK or AL occured returned Return ERROR */

        if((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                        (BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK|BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK)))!=
                BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_NORMAL)   {


           /*store the value in an error variable*/	
           BspI2cError=*(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress));

           /*Clear Nack or AL bits*/
           *(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress))&=(BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK|
                   BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK); 	

           return BSP_I2C_TRANSACTION_FAILED;
        }

        if((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                        (BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK)))!=BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_FALSE)  {

           break;
        }


        /*RRDY should be set by now*/

        while (BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_MASK)== BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_FALSE);

        tmpReg2 =*(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress));


        /* RRDY is cleared after reading data from the Data register */


        BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                BSP_I2C_RRDY_CLEAR_VALUE);


        bspI2c_transactionInfo[id].dataPtr [ 
        (bspI2c_transactionInfo[id].dataCount 
         - bspI2c_transactionInfo[id].dataLeftoverCount) 
        ] = (Uint8) tmpReg2;
        bspI2c_transactionInfo[id].dataLeftoverCount--;
        bspI2c_transactionInfo[id].currentAddress++;
        if (bspI2c_transactionInfo[id].dataLeftoverCount > 0)   {
           bspI2c_transactionInfo[id].dataPtr[ 
           (bspI2c_transactionInfo[id].dataCount 
            - bspI2c_transactionInfo[id].dataLeftoverCount) 
           ] = (Uint8) (tmpReg2 >> 8);
           bspI2c_transactionInfo[id].dataLeftoverCount--;
           bspI2c_transactionInfo[id].currentAddress++;
        }
    }while(1);

    /*bspI2c_transactionInfo[id].dataLeftoverCount > 0);*/

    /* Wait for Read transaction to complete */

    /* ARDY should be set by now */

    while ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK))== BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_FALSE);


    /* Clear the ARDY bit */

    BSPUTIL_BITUTIL_BIT_SINGLE_SET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
            BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK);

    /* cast vote in deep sleep election */

    bspI2c_castVoteInDeepSleepElection();

    return BSP_I2C_RETURN_CODE_SUCCESS;
}


/*=============================================================================
 * Description:
 *   This function will activate an I2C transaction if there is at least one
 *   transaction in the queue and the bus is not locked (internal bus usage)-or-
 *   the bus is not busy (external bus usage).
 *   If external bus usage, call delayFunction if defined.
 */
    void
bspI2c_activateTransaction(BspI2c_InstanceId id, Bool bspI2c_inInterruptContext)
{
    BspI2c_Result   result;
    BspI2cMaster_TransactionRequest *transaction;
    BSP_I2C_MULTI_MASTER_DEFINE_LOCK_STATE_VAR(lockState);
    Uint32 *baseAddress = bspI2c_Info[id].baseAddress;

    for(;;)
    {
        /* At least one request transaction in queue, 
           need to move from request queue to current active queue  */
        if (  transactionRequestCount[id] > 0 )
        {


            /* Decrement transaction count since this one is on its way */
            BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState);            
            transactionRequestCount[id]--;
            BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState); 

            transaction = 
            &transactionRequestQueue[id][transactionActiveIndex[id]];
            if ( transaction->transactionType 
                    == BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE )
            {
                result = bspI2c_lowLevelwrite( id,transaction);
            }
            else
            {
                result = bspI2c_lowLevelread( id,transaction);
            }

            /* See if transaction needs to be deferred due to BusBusy */
            if ( result == BSP_I2C_RETURN_CODE_SUCCESS_OPERATION_NOT_COMPLETE )
            {
                /* Increment transaction queue, since this one failed to go */

                BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState); 
                transactionRequestCount[id]++;
                BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);

                bspI2cMultiMaster_unlock(id, bspI2c_inInterruptContext);



                /* If a delay Function has been defined, call it */
                if ( transaction->delayFunctionPtr != NULL )
                {
                    /* The Implementer of Delay will call Activate Later */
                    (*(transaction->delayFunctionPtr))(transaction->delayValue);
                    break;

                }
                else if( bspI2c_Info[id].mode == BSP_I2C_INTERRUPT_MODE)
                {
                    bspI2cDefaultDelay(id);
                    break;

                }
            }
            else
            {
                break;

            }
        }
        else
        {
            /* cast vote in deep sleep election */
            /*It should not come here since activation should never be done without nothing in the queue*/			

            bspI2c_castVoteInDeepSleepElection();
            break;

        }
        bspI2cMultiMaster_waitNotBusy(id,baseAddress);
    }



}

/*=============================================================================
 * Description:
 *   This function will Queue the transaction's to a FIFO and will activate it 
 *   if it is the only transaction in the queue. Will be used in Interrupt mode
 *   only. 
 */

    static Uint8
bspI2c_queueTransaction(BspI2c_InstanceId id,
        BspI2c_ScenarioDescriptor *sd,
        BspI2c_TransactionRequest transactionRequest[])
{
    Uint8       transactionIndex;
    BspI2cMaster_TransactionRequest *transaction;
    unsigned int i=0;
    int trigger=0;
    BSP_I2C_MULTI_MASTER_DEFINE_LOCK_STATE_VAR(lockState);
    bspI2cMultiMaster_queue_lock(id);

    while(i<sd->count)   {

       /* Make sure we haven't overrun the queue */

       BSPUTIL_ASSERT(((transactionRequestCount[id]+1) 
                   <MAX_QUEUED_TRANSACTIONS));
       {
           /* Increment transaction count */

           transactionRequestCount[id]++;

           /* Move information into transaction request buffer */

           transaction = 
           &(transactionRequestQueue[id][transactionAvailableIndex[id]]);

           transaction->deviceId = sd->deviceId;
           transaction->transactionDoneCallback = NULL;
           transaction->transactionId = sd->transactionId;
           transaction->delayFunctionPtr = sd->delayFunctionPtr;
           transaction->delayValue = sd->delayValue;

           transaction->transactionType = transactionRequest[i].transactionType;
           transaction->dataStartAddress = 
           transactionRequest[i].dataStartAddress;
           transaction->dataPtr = transactionRequest[i].dataPtr;
           transaction->dataCount = transactionRequest[i].dataCount;
#ifdef Bsp_I2C_DMA_Enable
           transaction->dmaRequestId = transactionRequest[i].dmaRequestId;
#endif
           transaction->busControl = transactionRequest[i].busControl;

           transactionIndex = transactionAvailableIndex[id];

           /* Adjust Available Slot */

           if (++transactionAvailableIndex[id]== MAX_QUEUED_TRANSACTIONS)   {
              transactionAvailableIndex[id]= 0;     

              /* circular */
           }
       }
       i++;
    }
    transaction->transactionDoneCallback = sd->transactionDoneCallback;
    if (transactionRequestCount[id] == sd->count)   {
       trigger=1;
    }


    /* If only one queued transaction, trigger it */
    if ( trigger && !bspI2c_busLocked[id])
    {
        bspI2cMultiMaster_lock(id, TRUE);
        /*This should happen in interrupt free context*/    
        bspI2cMultiMaster_queue_unlock(id);
        
        bspI2c_activateTransaction(id, TRUE);
    }
    else   {
    bspI2cMultiMaster_queue_unlock(id);
     /*  BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);*/
    }

    return transactionIndex;

}

/*=============================================================================
 * Description:
 *   This function will De-Queue the transaction from FIFO. Will be called after
 *   transaction is completed.
 */
    static void
bspI2c_dequeueTransaction(BspI2c_InstanceId id)
{
    /* Indicate transaction done */

    transactionRequestQueue[id][transactionActiveIndex[id]].transactionType 
    = BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_UNKNOWN;

    /* Adjust Active Slot */

    if (++transactionActiveIndex[id]== MAX_QUEUED_TRANSACTIONS)   {
       transactionActiveIndex[id]= 0;     /* circular */
    }
    return;
}


/*=============================================================================
 * Description:
 *   This is the interrupt handler registered with the main interrupt controller
 *   It checks to see whether a read or write operation was in progress then
 *   continues the operation if more data is to be read or written.
 *   Assumption:
 *   If MULTI_BYTE_READ supported means ADDRESS_AUTO_INCREMENT is supported.
 */
    static void
bspI2cMultiMaster_intHandler(BspIntC_SourceId sourceId)
{
    int                     i=0;
    volatile Uint16         statReg=0;
    volatile Uint16         dataRead=0;
    volatile Uint16         tmpReg=0;
    Uint32                  *baseAddress=NULL;
    BspI2c_InstanceId       id=0;
    Uint8                   readFlag=0;
    Uint8                   writeFlag=0;
    BspI2cMaster_TransactionInfo *curTrans=NULL;
    BspI2c_ConfigInfo * config=NULL;
    BSP_I2C_MULTI_MASTER_DEFINE_LOCK_STATE_VAR(lockState);

    if(sourceId==bspI2c_Info[BSP_I2C_2].sourceId)    {
       id=BSP_I2C_2;
    }
#if(LOCOSTO_LITE==0)
    if(sourceId==bspI2c_Info[BSP_I2C_1].sourceId)   {
       id=BSP_I2C_1;
    }	
#endif

    config=&bspI2c_deviceInfo[bspI2c_transactionInfo[id].deviceId ].configInfo;

    baseAddress = bspI2c_Info[id].baseAddress;
    curTrans = &bspI2c_transactionInfo[id];

    /* To reduce HW accesses read in the status to a temp register */

    statReg = BSP_I2C_MULTI_MASTER_LLIF_GET( 
            BSP_I2C_MULTI_MASTER_LLIF_COMPLETE_REG,
            BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress));

    /* =================Arbitration Lost=====================*/

    if ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(&statReg,
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK))
            !=0x0000)   {

       /* Clear the AL bit */

       BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_STAT_AL,
               BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
               BSP_I2C_MULTI_MASTER_LLIF_STAT_CLEAR_INT);
       
       NACK_AL_CNT=BSP_I2C_MULTI_MASTER_LLIF_STAT_AL_MASK;

       BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState); 
       transactionRequestCount[id]++;
       BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState); 


       bspI2c_Info[id].busOwner=0;
       bspI2c_activateTransaction(id, TRUE);


    }

    /*===================NACK=======================*/	
    else if ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(&statReg,
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK)) 
            !=0x0000)   
    {

        NACK_AL_CNT=BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK_MASK;

        /* Clear the NACK bit */

        BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_STAT_NACK,
                BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                BSP_I2C_MULTI_MASTER_LLIF_STAT_CLEAR_INT);
        /* Should Assert here */
        /* This Handling needs a Re-Look */

        curTrans->returnCode = BSP_I2C_RETURN_CODE_FAILURE_DEVICE;

        /* Don't continue processing the data. This will force the transaction 
         * to stop 
         */

        curTrans->transactionType=BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_UNKNOWN;

        

        /* This fixes the issue when device is not connceted and I2C command is passed 
         * Remove the transaction request from queue. 
         */

        BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState);	

        bspI2c_dequeueTransaction(id);


        BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);

        if(curTrans->transactionDoneCallback!=NULL)	{

           (*(curTrans->transactionDoneCallback))((Uint8)BSP_I2C_RETURN_CODE_FAILURE_DEVICE);

        }




        BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState);	
        if(transactionRequestCount[id]>0)  	{
           BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);
           bspI2c_activateTransaction(id, TRUE);

        }

        else    {

           bspI2cMultiMaster_unlock(id, TRUE);	
           BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);
        }




    }


    /* Checking RRDY First as we want ARDY to processed after FIFO is cleared */


    /*=============ARDY=============*/

    else if ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(&statReg,
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_ARDY_MASK))
            !=0x0000)    {

       /* Clear the ARDY bit */

       BSPUTIL_BITUTIL_BIT_SINGLE_SET16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
               BSP_I2C_MULTI_MASTER_LLIF_IE_ARDY_MASK);




       if(curTrans->transactionType==BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ
               && curTrans->dataLeftoverCount > 0)   {

          /* In case Address Auto Increment is Not supported for 
             Reading. This logical Transaction is done. Trigger the next
             read transaction */

          readFlag=1;

       }

       else if(curTrans->transactionType == 
               BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE &&
               curTrans->dataIndex < curTrans->dataCount)   {

          /* In case Address Auto Increment is Not supported for 
             writing. This logical Transaction is done. Trigger the next
             write transaction */

          curTrans->currentAddress++;
          writeFlag=1;

       }

       else if(curTrans->transactionType == 
               BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE_READ)   {

          /* Write Part start the Read transaction */

          bspI2cMultiMaster_doRead(id,curTrans->deviceId);
       }

       else   {

          /* Remove the transaction request from queue. */

          /* Remove the transaction request from queue. */
          bspI2c_dequeueTransaction(id);

          /* Perform callback if defined */
          if ( curTrans->transactionDoneCallback != NULL )   {
             /*Lock the bus and then call activate transaction*/

             (*(curTrans->transactionDoneCallback))(curTrans->transactionId);

             /*If abb interrupt happens at this point transaction count will ioncrease but bus is still locked*/	            
             BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState);
             if(transactionRequestCount[id]>0)  	{
                BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);
                bspI2c_activateTransaction(id, TRUE);

             }

             else    {
                /*There is no elements in the queue, so all scenarios are done and we can release the bus*/
                bspI2cMultiMaster_unlock(id, TRUE);	

		bspI2c_castVoteInDeepSleepElection();
                BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);
             }

          }
          else   { 	

             if((bspI2c_busLocked[id])&&(transactionRequestCount[id]>0)){
                /*bus should  be locked at this point*/ 
                bspI2c_activateTransaction(id, TRUE);

             }
             else 	{
                /*There is a problem here, this should not happen , for the momment to catch the 
                  exception this is done*/
                while(1);
             }
          }

       }

    }
    /*==============RRDY=============*/
    else if (BSPUTIL_BITUTIL_BIT_SINGLE_GET16(&statReg,
                BSP_I2C_MULTI_MASTER_LLIF_STAT_RRDY_MASK)
            != 0x0000)   {

       /* RRDY: Read FIFO has a byte for us */

       /* Check for Slave Mode */

       if (!bspI2c_busLocked[id])   {

          dataRead = BSP_I2C_MULTI_MASTER_LLIF_GET(
                  BSP_I2C_MULTI_MASTER_LLIF_DATA,
                  BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress));

          /* RRDY can be cleared here, since data is read */

          BSP_I2C_MULTI_MASTER_LLIF_SET( BSP_I2C_MULTI_MASTER_LLIF_COMPLETE_REG,
                  BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                  BSP_I2C_RRDY_CLEAR_VALUE );

       }
       else   {

          /* Get Read data out of Read FIFO 
           * The RRDY check for additional data rcvd is not Done for CHIPSET
           * 15, because the RRDY does not get cleared when the first data is 
           * received. 
           */

          if ((curTrans->transactionType != BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ) && (curTrans->transactionType != BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE_READ))   {

             /* This is an ERROR Condition */
             /* Disable RRDY interrupt */

             BSPUTIL_BITUTIL_BIT_SINGLE_CLEAR16(BSP_I2C_MULTI_MASTER_MAP_IE_PTR(baseAddress),
                     ~(BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY_MASK));   


             BSPUTIL_BITUTIL_BIT_SINGLE_CLEAR16(BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                     ~(BSP_I2C_MULTI_MASTER_LLIF_IE_RRDY_MASK)); 

             /* Use a temp reg to reduce HW accesses */           

             tmpReg = 0x0000;                                                         

             BSP_I2C_MULTI_MASTER_LLIF_SET_LOCAL(BSP_I2C_MULTI_MASTER_LLIF_CON_MST,   
                     &tmpReg,BSP_I2C_MULTI_MASTER_LLIF_CON_MST_ENABLED);           

             BSP_I2C_MULTI_MASTER_LLIF_SET_LOCAL(BSP_I2C_MULTI_MASTER_LLIF_CON_TRX,   
                     &tmpReg, BSP_I2C_MULTI_MASTER_LLIF_CON_TRX_WRITE); 


             /* Write out to the hardware */                                          

             BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_COMPLETE_REG,   
                     BSP_I2C_MULTI_MASTER_MAP_CON_PTR(baseAddress),tmpReg);          


          }
          else   {

             dataRead =*((volatile Uint16*)(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress)));


             /* RRDY can be cleared here, since data is read */     

             BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_COMPLETE_REG,
                     BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                     BSP_I2C_RRDY_CLEAR_VALUE); 


             curTrans->dataPtr[ 
             (curTrans->dataCount - curTrans->dataLeftoverCount) ] =
             (Uint8) dataRead;
             curTrans->dataLeftoverCount--;
             curTrans->currentAddress++;
             if (curTrans->dataLeftoverCount > 0)   {

                /* Not To Sure of this Check should be valid for slave
                 * Supporting multibyte Read 
                 */

                curTrans->dataPtr[ (
                        curTrans->dataCount - curTrans->dataLeftoverCount) ] =
                (Uint8) (dataRead >> 8);
                curTrans->dataLeftoverCount--;
                curTrans->currentAddress++;
             }

             /* law DEVELOPER NOTE
              * law  Normally this shouldn't happen, but for unknown reasons
              * law  under some circumstances with an interrupt(ADC)triggering
              * law  I2C transactions, RRDY int seems to get lost, however
              * law  the status is available immediately(?)from this interrupt
              * law  --normally we shouldn't get an RRDY for 2.5us 
              * (400kHz) * 18 bits = 45us from current one
              */ 
          }



       }

    }
    /*====================xrdy==============*/
    else if ((BSPUTIL_BITUTIL_BIT_SINGLE_GET16(&statReg,
                    BSP_I2C_MULTI_MASTER_LLIF_STAT_XRDY_MASK))
            !=0x0000)         {

       if (curTrans->transactionType == 
               BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE_READ)   {

          /* Set the subaddress byte(s) into the FIFO */

          bspI2cMultiMaster_setAddress(id,config,
                  curTrans->currentAddress,baseAddress);

          /* Clear the XRDY bit */

          BSP_I2C_MULTI_MASTER_LLIF_SET(BSP_I2C_MULTI_MASTER_LLIF_COMPLETE_REG,
                  BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                  BSP_I2C_XRDY_CLEAR_VALUE);

       }
       /***************************
        * Check for Write
        ***************************/
       else   {

          /* XRDY: See if we need to load data the Write FIFO  */

          if (curTrans->fifoCount > 0)  {
             bspI2cMultiMaster_WriteFifo(id,config,baseAddress);        
          }


          BSP_I2C_MULTI_MASTER_LLIF_SET(
                  BSP_I2C_MULTI_MASTER_LLIF_COMPLETE_REG,
                  BSP_I2C_MULTI_MASTER_MAP_STAT_PTR(baseAddress),
                  BSP_I2C_XRDY_CLEAR_VALUE);
       }
    }


    if(readFlag)   {

       /* Happens Only if device dosen't support multi byte read */
       /* Start Next Read Transaction */

       /* Let interrupt know this is a Write-Read transaction */

       curTrans->transactionType = 
       BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE_READ;

       /* Set the subaddress byte(s) into the FIFO */

       curTrans->fifoCount =bspI2cMultiMaster_setAddress(id,config,
               curTrans->currentAddress,baseAddress);

       /* Set the Data Counter Register */

       BSP_I2C_MULTI_MASTER_DCOUNT_SET(baseAddress,curTrans->fifoCount);

       BSP_I2C_MULTI_MASTER_TRIGGER_WRITE_NO_STOP(baseAddress,id)
    }
    else if(writeFlag)   {

       /* Happens Only if device dosen't support address auto increment */
       /* Start Next Write Transaction */

       curTrans->subAddressCount = bspI2cMultiMaster_setAddress(id, 
               config,
               curTrans->currentAddress,baseAddress);
       curTrans->fifoCount = curTrans->subAddressCount + 1;

       /* Set the Data Counter Register */

       BSP_I2C_MULTI_MASTER_DCOUNT_SET(baseAddress,curTrans->fifoCount);
       if(curTrans->dataLeftoverCount!=1 || 
               curTrans->busControl == BSP_I2C_BUS_NO_RELEASE)   {
          BSP_I2C_MULTI_MASTER_TRIGGER_WRITE_NO_STOP(baseAddress,id)
       }
       else   {
          BSP_I2C_MULTI_MASTER_TRIGGER_WRITE(baseAddress,id)
       }
    }
#ifdef ARDY_BUG
    if(ivReg == BSP_I2C_RRDY_INT && curTrans->dataLeftoverCount < 0)   {

       /* Transaction Done Stop Condition Generated */
       /* Remove the transaction request from queue. */

       bspI2c_dequeueTransaction(id);

       bspI2cMultiMaster_unlock(id, TRUE);


       /* Perform callback if defined */

       if (curTrans->transactionDoneCallback != NULL)   {
          (*(curTrans->transactionDoneCallback))(curTrans->transactionId);
       }

       /* Trigger any queued up transaction */

       bspI2c_activateTransaction(id, TRUE);

    }
#endif
}

#if (CHIPSET==15)

/*=============================================================================
 * Description:
 *   HISR for I2C 1; 
 */
#if(LOCOSTO_LITE==0) 
static  void bspI2c_Hisr1(void)
{
    bspI2cMultiMaster_intHandler(C_INTH_I2C_IT);
    F_INTH_ENABLE_ONE_IT(C_INTH_I2C_IT);
}
#endif

/*=============================================================================
 * Description:
 *   HISR for I2C 2; 
 */
static  void bspI2c_Hisr2(void)
{
    bspI2cMultiMaster_intHandler(C_INTH_I2C_TRITON_IT);
    F_INTH_ENABLE_ONE_IT(C_INTH_I2C_TRITON_IT);
}
#endif


/*=============================================================================
 *   Public Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 *   This function is used to initialize the I2CM driver. This function will
 *   initialize variables to be used by the device driver.
 */
    BspI2c_ReturnCode
bspI2c_init(void)
{
    BspI2c_InstanceId   id;

    /* Initialize transaction queue data */

    for(id=0;id<BSP_I2C_NUM_DEVICES;id++)   {
       transactionRequestCount[id]= 0;
       transactionActiveIndex[id]= 0;
       transactionAvailableIndex[id]= 0;
       bspI2c_Info[id].busOwner = 0;
       bspI2c_Info[id].hisrRefPtr=&hisrRef[id];
       NU_Create_HISR((bspI2c_Info[id].hisrRefPtr),"I2C",
               bspI2c_Info[id].hisr_entry,	I2C_HISR_PRIO,
#ifndef HISR_STACK_SHARING			
               I2c_Hisr_Stack,sizeof(I2c_Hisr_Stack));
#else
       HISR_STACK_PRIO2,HISR_STACK_PRIO2_SIZE);
#endif
       bspI2c_busLockedSemaphore[id] = NULL;
       /* Set default Slave Receive parameters */

#if (LOCOSTO_LITE==0)
       bspI2c_slaveTransactionInfo[id].dataPtr = NULL;
#endif

       if (NULL == bspI2c_busLockedSemaphore[id])   {

          if (NU_SUCCESS!=  NU_Create_Semaphore(&i2cSemArray[id], 
                      bspI2c_Info[id].semaphoreName, 
                      1,
                      NU_PRIORITY))   {
             return BSP_I2C_RETURN_CODE_FAILURE_DEVICE;            
          }
          bspI2c_busLockedSemaphore[id]=&i2cSemArray[id];
       }
       F_INTH_ENABLE_ONE_IT(bspI2c_Info[id].sourceId)
    }


    return BSP_I2C_RETURN_CODE_SUCCESS;
}

/*=============================================================================
 *   Public Generic API Implementation Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 *   This function is used to configure the I2CM.
 */
    BspI2c_ReturnCode
bspI2c_config(BspI2c_DeviceId         deviceId,
        const BspI2c_ConfigInfo *configInfoPtr)   
{
    BspI2cMultiMaster_ClockPrescaler    clockPrescalar;
    BspI2cMultiMaster_ClockLowPeriod    clockLowPeriod=53;
    BspI2cMultiMaster_ClockHighPeriod   clockHighPeriod=53;
    BSP_I2C_MULTI_MASTER_DEFINE_LOCK_STATE_VAR(lockState);

    BSPUTIL_ASSERT(((configInfoPtr->busSpeed == BSP_I2C_DEVICE_BUS_SPEED_100K)||
                (configInfoPtr->busSpeed == BSP_I2C_DEVICE_BUS_SPEED_400K)));

    if (configInfoPtr->busSpeed == BSP_I2C_DEVICE_BUS_SPEED_400K)   {
       clockPrescalar         = BSP_I2C_MULTI_MASTER_CLOCK_PRESCALER_DIVISOR_1;
       /*law DEVELOPER NOTE
        *       law Due to a silicon issue (Data line slow due to loading problem) 
        *       law with Garibaldi I2C, Garibaldi 2.0 cannot be run reliably at 
        *       law 400kHz. 333kHz seems to be a good compromise for most platforms
        *       clockLowPeriod         = 7;this allows for slightly less than 400kHz
        *       clockHighPeriod        = 7;
        *       Reduce down to 333kHz, until Garibaldi 2.1 is available 
        *       clockLowPeriod         = 9;
        *       clockHighPeriod        = 9;
        */       
    }
    else   {
       clockPrescalar         = BSP_I2C_MULTI_MASTER_CLOCK_PRESCALER_DIVISOR_1;

       /*this allows for slightly less than 100kHz*/

       clockLowPeriod         = 53;    
       clockHighPeriod        = 53;
    }

    BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState);

    bspI2c_deviceInfo[ deviceId ].clockPrescalerDivFactor = clockPrescalar;
    bspI2c_deviceInfo[ deviceId ].clockLowPeriod          = clockLowPeriod;
    bspI2c_deviceInfo[ deviceId ].clockHighPeriod         = clockHighPeriod;
    bspI2c_deviceInfo[ deviceId ].configInfo              = *configInfoPtr;


    BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);

    return(BSP_I2C_RETURN_CODE_SUCCESS);
}



static Uint8   i2c_wait;

    static void
i2c_internal_callback(Uint8 id)
{
    i2c_wait = 1;
}


/*=============================================================================
 * Description:
 *   This function writes the passed in data to the I2CM bus. The function will
 *   wait for the operation to complete if the callback is passed in as NULL.
 */

    BspI2c_ReturnCode
bspI2c_write(BspI2c_DeviceId     deviceId,
        BspI2c_DataAddress  dataStartAddress,
        BspI2c_DataCount    dataCount,
        BspI2c_Data         *dataPtr)
{
    BspI2c_ReturnCode  returnCode;
    BspI2c_TransactionDoneCallback    transactionDoneCallback;


    transactionDoneCallback = i2c_internal_callback;
    i2c_wait = 0;

    /*Always Use First Instance*/

    returnCode =  bspI2c_highLevelwrite(0,
            deviceId,
            dataStartAddress,
            dataCount,
            dataPtr,
            0xFF,
            transactionDoneCallback,
            0,
            NULL,
            0);

    /* Internal spin */
    while (!i2c_wait);


    return returnCode;

}

/*=============================================================================
 * Description:
 *   This function performs a read transaction.
 */

    BspI2c_ReturnCode
bspI2c_read(BspI2c_DeviceId     deviceId,
        BspI2c_DataAddress  dataStartAddress,
        BspI2c_DataCount    dataCount,
        BspI2c_Data         *dataPtr)
{
    BspI2c_ReturnCode   returnCode;
    BspI2c_TransactionDoneCallback    transactionDoneCallback;


    transactionDoneCallback = i2c_internal_callback;
    i2c_wait = 0;


    /*Always Use First Instance*/

    returnCode =  bspI2c_highLevelread(0,
            deviceId,
            dataStartAddress,
            dataCount,
            dataPtr,
            BSP_DMA_REQ_NONE,
            transactionDoneCallback,
            0,
            NULL,
            0);

    /* Internal spin */
    while (!i2c_wait);

    return(returnCode);
}


/*=============================================================================
 * Description:
 *   This function performs a read transaction, The function will wait for the
 *   operation to complete if the callback is passed in as NULL.
 */
    BspI2c_ReturnCode
bspI2c_highLevelread(BspI2c_InstanceId id,
        BspI2c_DeviceId                   deviceId,
        BspI2c_DataAddress                dataStartAddress,
        BspI2c_DataCount                  dataCount,
        BspI2c_Data                       *dataPtr,
        BspI2c_DmaRequestId               dmaRequestId,
        BspI2c_TransactionDoneCallback    transactionDoneCallback,
        BspI2c_TransactionId              transactionId,
        BspI2c_DelayFunctionPtr           delayFunctionPtr,
        BspI2c_DelayValue                 delayValue)
{
    BspI2c_ReturnCode   returnCode;



    if(!transactionDoneCallback)   {
       /*Polled Mode Write Handling */
       BspI2cMaster_TransactionRequest bspI2c_currentTransactionRequest;

       /* Store off the request information */
       BSP_I2C_MULTI_MASTER_STORE_REQUEST_INFO( 
               BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ,
               deviceId,
               dataStartAddress,
               dataPtr,
               dataCount,
               transactionDoneCallback,
               transactionId,
               delayFunctionPtr,
               delayValue,
               BSP_I2C_BUS_RELEASE);


       bspI2cMultiMaster_lock(id, FALSE);
       BSP_I2C_MULTI_MASTER_INTERRUPT_DISABLE(id);

       /** We are not handling NACK and AL for Now */
       bspI2c_Info[id].mode = BSP_I2C_POLLED_MODE;
       /* Force Not to use DMA */
       bspI2c_currentTransactionRequest.dmaRequestId = BSP_DMA_REQ_NONE;
       returnCode = bspI2c_lowLevelread(id,&bspI2c_currentTransactionRequest);
       bspI2c_Info[id].mode = BSP_I2C_INTERRUPT_MODE;
       bspI2cMultiMaster_unlock(id, FALSE);
    }
    else   {
       /* Interrupt Mode Handling */
       /* Put request into queue */
       BspI2c_TransactionRequest  transactionReq[1];
       BspI2c_ScenarioDescriptor  sd;

       sd.count = 1;
       sd.deviceId = deviceId;
       sd.transactionDoneCallback = transactionDoneCallback;
       sd.transactionId = transactionId;
       sd.delayFunctionPtr = delayFunctionPtr;
       sd.delayValue = delayValue;

       transactionReq[0].transactionType = 
       BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ;
       transactionReq[0].dataStartAddress = dataStartAddress; 
       transactionReq[0].dataPtr = dataPtr; 
       transactionReq[0].dataCount = dataCount; 
#ifdef Bsp_I2C_DMA_Enable 
       transactionReq[0].dmaRequestId = dmaRequestId; 
#endif
       transactionReq[0].busControl = BSP_I2C_BUS_RELEASE; 

       bspI2c_queueTransaction(id,&sd,transactionReq);

       returnCode = BSP_I2C_RETURN_CODE_SUCCESS;
    }

    return(returnCode);
}

/*=============================================================================
 * Description:
 *   This function writes the passed in data to the I2C bus. The function will
 *   wait for the operation to complete if the callback is passed in as NULL.
 */

    BspI2c_ReturnCode
bspI2c_highLevelwrite(BspI2c_InstanceId id,
        BspI2c_DeviceId                  deviceId,
        BspI2c_DataAddress               dataStartAddress,
        BspI2c_DataCount                 dataCount,
        BspI2c_Data                      *dataPtr,
        BspI2c_DmaRequestId              dmaRequestId,
        BspI2c_TransactionDoneCallback   transactionDoneCallback,
        BspI2c_TransactionId             transactionId,
        BspI2c_DelayFunctionPtr          delayFunctionPtr,
        BspI2c_DelayValue                delayValue)
{
    BspI2c_ReturnCode  returnCode;



    if(!transactionDoneCallback)   {
       /*Polled Mode Write Handling */

       BspI2cMaster_TransactionRequest bspI2c_currentTransactionRequest;
       /* Store off the request information */
       BSP_I2C_MULTI_MASTER_STORE_REQUEST_INFO( 
               BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE,
               deviceId,
               dataStartAddress,
               dataPtr,
               dataCount,
               transactionDoneCallback,
               transactionId,
               delayFunctionPtr,
               delayValue,
               BSP_I2C_BUS_RELEASE);

       bspI2cMultiMaster_lock(id, FALSE);
       BSP_I2C_MULTI_MASTER_INTERRUPT_DISABLE(id);

       /** We are not handling NACK and AL for Now */
       bspI2c_Info[id].mode = BSP_I2C_POLLED_MODE;
       /* Force Not to use DMA */
       bspI2c_currentTransactionRequest.dmaRequestId = BSP_DMA_REQ_NONE;
       returnCode = bspI2c_lowLevelwrite(id,&bspI2c_currentTransactionRequest);
       bspI2c_Info[id].mode = BSP_I2C_INTERRUPT_MODE;
       bspI2cMultiMaster_unlock(id, FALSE);
    }
    else   {
       /* Interrupt Mode Handling */
       /* Put request into queue */
       BspI2c_TransactionRequest  transactionReq[1];
       BspI2c_ScenarioDescriptor  sd;

       sd.count = 1;
       sd.deviceId = deviceId;
       sd.transactionDoneCallback = transactionDoneCallback;
       sd.transactionId = transactionId;
       sd.delayFunctionPtr = delayFunctionPtr;
       sd.delayValue = delayValue;

       transactionReq[0].transactionType = 
       BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE;
       transactionReq[0].dataStartAddress = dataStartAddress; 
       transactionReq[0].dataPtr = dataPtr; 
       transactionReq[0].dataCount = dataCount; 
#ifdef  Bsp_I2C_DMA_Enable
       transactionReq[0].dmaRequestId = dmaRequestId; 
#endif
       transactionReq[0].busControl = BSP_I2C_BUS_RELEASE; 

       bspI2c_queueTransaction(id,&sd,transactionReq);

       returnCode = BSP_I2C_RETURN_CODE_SUCCESS;
    }

    return returnCode;
}

#ifdef Bsp_I2C_DMA_Enable

/* FIXME Port DMA */
/*=============================================================================
 * Description:
 *   Callback Function for the Slave DMA Transaction.
 */

    static void
i2cInternalDMACallback(BspDma_ChannelEvent channelEvent,
        BspDma_ChannelId    channelId)
{

    BspI2c_DeviceId id;
    for(id=0;id < BSP_I2C_NUM_DEVICES;id++)   {
       if(bspI2c_Info[id].rxchannel == channelId)   {
          break;
       }
    }
    /* Turn off channel transfer */
#if (LOCOSTO_LITE==0)
    bspDma_stopChannelTransfer(bspI2c_slaveTransactionInfo[id].dmaRequestId);
    /* Perform client callback if defined */
    if (bspI2c_slaveTransactionInfo[id].transactionDoneCallback != NULL)
        (*bspI2c_slaveTransactionInfo[id].transactionDoneCallback)();
#endif
}

#endif	

/*=============================================================================
 * Description:
 *   This function defines the parameters used for Slave Receive..
 */
#if (LOCOSTO_LITE==0)
    BspI2c_ReturnCode
bspI2c_masterSlaveConfig(BspI2c_InstanceId   id,
        BspI2c_DeviceId     slaveId, 
        BspI2c_Data         *dataPtr,
        BspI2c_DataCount    dataCount,
        BspI2c_DmaRequestId dmaRequestId,
        BspI2c_slaveTransactionDoneCallback transactionDoneCallback)
{

#ifdef Bsp_I2C_SLAVE_SUPPORT
    Uint32                  *baseAddress = bspI2c_Info[id].baseAddress;
#ifdef Bsp_I2C_DMA_Enable
    /* FIXME Port DMA */
    BspDma_ChannelConfiguration chanConfigI2CRead = {
       BSP_DMA_DATA_TYPE_S16,
       BSP_DMA_ADDRESSING_MODE_CONSTANT,
       BSP_DMA_ADDRESSING_MODE_POST_INCREMENT,
       0, 
       BSP_DMA_BURST_MODE_SINGLE,
       BSP_DMA_BURST_MODE_SINGLE,
       0,
       FALSE,
       FALSE,
       FALSE,
       TRUE,
       1,
       NULL
    };
#endif

    bspI2c_Info[id].ownAddress =  
    bspI2c_deviceInfo[ slaveId ].configInfo.deviceAddress;

    /* See if Slave servicing is being turned off */
    if (dataPtr == NULL)   {

#ifdef BSP_I2C_DMA_Enable		
       bspI2c_SlaveReadDMA[id] = FALSE;
       bspI2c_SlaveOn[id] = FALSE;
#endif	


    }
    else   {

       /* Initialize OA ("Own Address") here*/

       BSP_I2C_MULTI_MASTER_OWN_ADDRESS_SET (id,bspI2c_Info[id].ownAddress);

       /* Slave servicing being turned on */

       bspI2c_SlaveOn[id] = TRUE;

       /* Store off Slave Receive transaction info into local structure */

       BSP_I2C_MULTI_MASTER_SLAVE_TRANSACTION_INFO(id, 
               slaveId,
               dataPtr,
               dataCount,
               transactionDoneCallback);


       if (dmaRequestId == BSP_DMA_REQ_NONE)   {

#ifdef BSP_I2C_DMA_Enable  	
          bspI2c_SlaveReadDMA[id] = FALSE;
#endif
       }
       else   {

#ifdef Bsp_I2C_DMA_Enable       
          bspI2c_SlaveReadDMA[id] = TRUE;

          chanConfigI2CRead.dmaRequestId = dmaRequestId;
          chanConfigI2CRead.channelEventCallback = i2cInternalDMACallback;

          /* Due to fact that FIFO is always 16-bit, dataCount must be even 
             for DMA transactions */
          BSPUTIL_ASSERT(!(dataCount & 0x01));

          /* Configure DMA channel */
          bspDma_channelConfig(bspI2c_Info[id].rxchannel,&chanConfigI2CRead);
          bspDma_startTransfer(bspI2c_Info[id].rxchannel,
                  (void *)(BSP_I2C_MULTI_MASTER_MAP_DATA_PTR(baseAddress)),
                  dataPtr,
                  dataCount);
#else
          while(1);
#endif
       }
    }

    /* If no Master bus activity, turn on Slave Read.
     * If currently in Master mode, then when done it will get
     * turned on via bspI2cMultiMaster_unlock() 
     */

    if (!bspI2c_busLocked[id])   {
       bspI2cMultiMaster_unlock(id, FALSE);
    }

    return BSP_I2C_RETURN_CODE_SUCCESS;
#endif	

    return BSP_I2C_RETURN_CODE_FAILURE_DATA;

}
#endif
/*=============================================================================
 * Description:
 *   This function excutes the scenario comprising of read and write 
 *   transactions on I2C bus. All the transactions in the scenario can be for 
 *   single device only. The scenario will be executed in polled mode if the 
 *   callback is passed in as NULL.
 */
    BspI2c_ReturnCode
bspI2c_submitTransaction(BspI2c_InstanceId id,
        BspI2c_ScenarioDescriptor *scenarioDesc,
        BspI2c_TransactionRequest transactionArray[])
{
    BspI2c_ReturnCode  returnCode;
    unsigned int i=scenarioDesc->count;
    BSP_I2C_MULTI_MASTER_DEFINE_LOCK_STATE_VAR(lockState);


    transactionArray[--i].busControl = BSP_I2C_BUS_RELEASE;

    while(i)   {
       transactionArray[--i].busControl = BSP_I2C_BUS_NO_RELEASE;
    }



    if (scenarioDesc->transactionDoneCallback)   {
       bspI2c_queueTransaction(id,scenarioDesc,transactionArray);

       returnCode = BSP_I2C_RETURN_CODE_SUCCESS;

    }
    else   {
       /*Polled Mode Write Handling */
       BspI2cMaster_TransactionRequest bspI2c_currentTransactionRequest;
       i=0;

       bspI2cMultiMaster_lock(id, FALSE);
       BSP_I2C_MULTI_MASTER_INTERRUPT_DISABLE(id);
       bspI2c_Info[id].mode = BSP_I2C_POLLED_MODE;

       while(i<scenarioDesc->count)   {

          /* Store off the request information */
          BSP_I2C_MULTI_MASTER_STORE_REQUEST_INFO( 
                  transactionArray[i].transactionType,
                  scenarioDesc->deviceId,
                  transactionArray[i].dataStartAddress,
                  transactionArray[i].dataPtr,
                  transactionArray[i].dataCount,
                  scenarioDesc->transactionDoneCallback,
                  scenarioDesc->transactionId,
                  scenarioDesc->delayFunctionPtr,
                  scenarioDesc->delayValue,
                  transactionArray[i].busControl);
          /** We are not handling NACK and AL for Now */
          /* Force Not to use DMA */
          bspI2c_currentTransactionRequest.dmaRequestId = BSP_DMA_REQ_NONE;
          if (transactionArray[i].transactionType 
                  == BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE)   {
             returnCode =bspI2c_lowLevelwrite(id,&bspI2c_currentTransactionRequest);
          }
          else   {
             returnCode =bspI2c_lowLevelread(id,&bspI2c_currentTransactionRequest);
          }

          if(returnCode==BSP_I2C_TRANSACTION_FAILED)   {
             break;
          }	

          i++;
       }

       bspI2c_Info[id].mode = BSP_I2C_INTERRUPT_MODE;

       bspI2cMultiMaster_unlock(id, FALSE);

       if(returnCode==BSP_I2C_TRANSACTION_FAILED)   {

          return BSP_I2C_RETURN_CODE_FAILURE_DEVICE;

       }	

       BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_ENTER(lockState);
       /*ACtivate transaction only if bus is free and there is something in the queue*/
       if ( (!bspI2c_busLocked[id]) &&(transactionRequestCount[id]>0))	{

          bspI2cMultiMaster_lock(id, TRUE);
          BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);	
          /*At this point if abb interrupt comes it will just queue the transactions and return since the bus is locked*/
          bspI2c_activateTransaction(id, TRUE);

       }
       else   {

          BSP_I2C_MULTI_MASTER_CRITICAL_SECTION_EXIT(lockState);

       }

    }

    return returnCode;
}


#if (CHIPSET==15)
/*=============================================================================
 * Description:
 *   Interrupt handler for First Instance of I2C. 
 */
void bspI2c_Handeler1(void)
{
    F_INTH_DISABLE_ONE_IT(C_INTH_I2C_IT);
#if (LOCOSTO_LITE==0)
    NU_Activate_HISR(bspI2c_Info[BSP_I2C_1].hisrRefPtr);
#endif
}


/*=============================================================================
 * Description:
 *   Interrupt handler for Second Instance of I2C. 
 */
void bspI2c_Handeler2(void)
{
    F_INTH_DISABLE_ONE_IT(C_INTH_I2C_TRITON_IT);
    NU_Activate_HISR(bspI2c_Info[BSP_I2C_2].hisrRefPtr);
}
#endif

