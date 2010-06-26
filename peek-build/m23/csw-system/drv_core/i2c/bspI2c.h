/*=============================================================================
 * Copyright 2002-2004 Texas Instruments Incorporated. All Rights Reserved.
 */                                                                       

#ifndef BSP_I2C_HEADER
#define BSP_I2C_HEADER

#include "types.h"
#include "bspI2c_Platform.h"
#undef Bsp_I2C_DMA_Enable
/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspI2c
 *    Public interface to the I2C MultiMaster device driver. 
 *
 *    This device driver is the interface to the I2C MultiMaster hardware block
 *    Initialization occurs once at powerup.
 * 
 *    Only the following functions are reentrant:
 *      bspI2c_write()
 *      bspI2c_read()
 *      bspI2c_activateTransaction()
 * 
 */


/*=============================================================================
 *  Defines
 *============================================================================*/
/*=============================================================================
 *  Enumerations and Types
 *============================================================================*/
/*===========================================================================*/
/*!
 * @typedef BspI2c_ReturnCode
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for return codes for the I2C functions.
 */
enum
{
    BSP_I2C_RETURN_CODE_SUCCESS_OPERATION_NOT_COMPLETE = 1,
    BSP_I2C_RETURN_CODE_SUCCESS                        = 0,
    BSP_I2C_RETURN_CODE_FAILURE_DATA_LENGTH            = -1,
    BSP_I2C_RETURN_CODE_FAILURE_DEVICE                 = -2,
    BSP_I2C_RETURN_CODE_FAILURE_DATA                   = -3
};
typedef Int32 BspI2c_ReturnCode;


/*===========================================================================*/
/*!  
 * @typedef BspI2c_DataAddress 
 *
 * @discussion
 * <b> Description </b><br>
 *   Each BSP_I2C device can have several internal data addresss. This type is
 *   used to choose a particular address within a device.
 */
typedef Uint8 BspI2c_DataAddress;


/*===========================================================================*/
/*!  
 * @typedef BspI2c_DataCount
 *
 * @discussion
 * <b> Description </b><br>
 *   This type is used to keep track of how much data is being read or written
 *   the I2C device.
 */
typedef Uint16 BspI2c_DataCount;


/*===========================================================================*/
/*!  
 * @typedef BspI2c_Data 
 *
 * @discussion
 * <b> Description </b><br>
 *   This is the data type for the data that is passed into/out of the I2C
 *   driver.
 */
typedef Uint8 BspI2c_Data;


/*===========================================================================*/
/*!  
 * @typedef BspI2c_AddressAutoIncrement
 *
 * @discussion
 * <b> Description </b><br>
 *   This is a type used to describe an I2C device. Some devices auto increment
 *   the address internally so the address doesn't need to be sent for
 *   consecutive writes to consecutive addresses. Some devices don't do this.
 *
 *   This parameter applies only to Write transactions.
 *
 *   BSP_I2C_ADDRESS_AUTO_INCREMENT_NOT_SUPPORTED ==>
 *      S - DevId - A - D1 - A+1 - D2 - A+2 - ... - P
 *
 *   BSP_I2C_ADDRESS_AUTO_INCREMENT_SUPPORTED ==>
 *     S - DevId - A - D1 - D2 - ... - P 
 */
enum
{
    BSP_I2C_ADDRESS_AUTO_INCREMENT_NOT_SUPPORTED,
    BSP_I2C_ADDRESS_AUTO_INCREMENT_SUPPORTED
};
typedef Uint8 BspI2c_AddressAutoIncrement;

/*===========================================================================*/
/*!  
 * @typedef BspI2c_DeviceBusSpeed
 *
 * @discussion
 * <b> Description </b><br>
 *   This is a type used to indicate what bus speed a device can handle.
 */
enum
{
    BSP_I2C_DEVICE_BUS_SPEED_100K,
    BSP_I2C_DEVICE_BUS_SPEED_400K
};
typedef Uint8 BspI2c_DeviceBusSpeed;

/*===========================================================================*/
/*!  
 * @typedef BspI2c_DeviceAddressBitLength
 *
 * @discussion
 * <b> Description </b><br>
 *   This is a type used to indicate the bit length of the device address.
 */
enum
{
    BSP_I2C_DEVICE_ADDRESS_LENGTH_7BIT  = 7,
    BSP_I2C_DEVICE_ADDRESS_LENGTH_10BIT = 10
};
typedef Uint8 BspI2c_DeviceAddressBitLength;

/*===========================================================================*/
/*!  
 * @typedef BspI2c_AddressOctetLength
 *
 * @discussion
 * <b> Description </b><br>
 *   This is a type used to indicate the length in octets for the internal
 *   address of the device.
 */
enum
{
    BSP_I2C_ADDRESS_OCTET_LENGTH_1 = 1,
    BSP_I2C_ADDRESS_OCTET_LENGTH_2 = 2,
    BSP_I2C_ADDRESS_OCTET_LENGTH_4 = 4
};
typedef Uint8 BspI2c_AddressOctetLength;


/*===========================================================================*/
/*!  
 * @typedef BspI2c_MultiByteRead
 *
 * @discussion
 * <b> Description </b><br>
 *   This is a type used to describe an I2C device. Some devices can support
 *  "multi-byte read", or can read more than a single byte following a
 *  Read Cycle or Read Combined Cycle.
 *
 *   This parameter applies only to Read transactions.
 *
 *   BSP_I2C_MULTI_BYTE_READ_NOT_SUPPORTED ==>
 *    the logical transaction must be broken into multiple transfers:
 *     S - DevId - A - S - DevId - D1 - P
 *     S - DevId - A - S - DevId - D2 - P
 *     S - DevId - A - S - DevId - D3 - P
 *     etc.
 *
 *   BSP_I2C_ADDRESS_AUTO_INCREMENT_SUPPORTED ==>
 *     S - DevId - A - S - DevId - D1 - D2 - ... - P 
 */
enum
{
    BSP_I2C_MULTI_BYTE_READ_NOT_SUPPORTED,
    BSP_I2C_MULTI_BYTE_READ_SUPPORTED
};
typedef Uint8 BspI2c_MultiByteRead;

/*===========================================================================*/
/*!  
 * @typedef BspI2c_AddressByteOrder
 *
 * @discussion
 * <b> Description </b><br>
 *   This is a type used to describe an I2C device. Some devices can require
 *   their internal addressing to be High Byte to Low Byte, some devices
 *   require their internal address to be Low Byte to High Byte.
 */
enum
{
    BSP_I2C_ADDRESS_ORDER_LOW_TO_HIGH,
    BSP_I2C_ADDRESS_ORDER_HIGH_TO_LOW
};
typedef Uint8 BspI2c_AddressByteOrder;


/*===========================================================================*/
/*!  
 * @typedef BspI2c_ConfigInfo 
 *
 * @discussion
 * <b> Description </b><br>
 *     This is the configuration information needed by the I2C bus.
 * 
 *  @field busSpeed
 *            This determines whether the device can handle 400K or 100k
 *            bus speeds.
 * 
 *  @field addressOctetLength
 * 
 * 
 *  @field addressAutoIncrement
 *            This indicates if the device data addresses auto increment
 *            internally
 * 
 *  @field deviceAddressBitLength        
 *            This is type of I2C addressing the device requires.
 *
 *  @field deviceAddress
 *             This is the I2C Device Address for the device.
 * 
 *  @field multiByteRead
 *             This is an intication whether the device supports the I2C
 *             standard method of reading multiple bytes consecutively.
 *             If supported then the device id and internal address are sent
 *             only once at the beginning of the I2C transaction and the
 *             transaction contiunues unbroken until all bytes are read.
 *             Note: Even if the device supports this some OMAP I2C HW
 *                   is not capable of supporting this. Reading multiple
 *                   bytes with HW that doesn't support multi-byte reads
 *                   will result in multiple independant read transactions.
 * 
 *  @field addressByteOrder
 *             This is an indication of the byte ordering of the device's
 *             internal address (not the device I2C address).
 */
typedef struct{
    BspI2c_DeviceBusSpeed          busSpeed;
    BspI2c_AddressOctetLength      addressOctetLength;
    BspI2c_AddressAutoIncrement    addressAutoIncrement;
    BspI2c_DeviceAddressBitLength  deviceAddressBitLength;
    BspI2c_DeviceAddress           deviceAddress;
    BspI2c_MultiByteRead           multiByteRead;
    BspI2c_AddressByteOrder        addressByteOrder;
} BspI2c_ConfigInfo;

/*===========================================================================*/
/*!
 * @typedef BspI2c_TransactionId
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for the transaction identifier. Note
 *   that this value is provided by the client, and returned by
 *   the I2c driver when the transaction is completed.
 */
typedef Uint8 BspI2c_TransactionId;

/*===========================================================================*/
/*!
 * @typedef BspI2c_TransactionDoneCallback
 *
 * @discussion
 * <b> Description </b><br>
 *   This is the type for the transaction complete callbacks provided by
 *   the client of this driver.
 */
typedef void (*BspI2c_TransactionDoneCallback)( BspI2c_TransactionId );

/*===========================================================================*/
/*!
 * @typedef BspI2c_slaveTransactionDoneCallback
 *
 * @discussion
 * <b> Description </b><br>
 *   This is the type for the transaction complete callbacks provided by
 *   the client of this driver.
 */
typedef void (*BspI2c_slaveTransactionDoneCallback)( void );

/*===========================================================================*/
/*!
 * @typedef BspI2c_LockObtainFunctionPtr
 *
 * @discussion
 * <b> Description </b><br>
 *   This is the type for the function pointer that will check for availability
 *   of the I2C Task Manager, wait for availability, and when available, lock.
 */
typedef void (*BspI2c_LockObtainFunctionPtr)( void );

/*===========================================================================*/
/*!
 * @typedef BspI2c_LockReleaseFunctionPtr
 *
 * @discussion
 * <b> Description </b><br>
 *   This is the type for the function pointer that will unlock access to the
 *   I2C Task Manager.
 */
typedef void (*BspI2c_LockReleaseFunctionPtr)( void );

/*===========================================================================*/
/*!
 * @typedef BspI2c_SignalFunctionPtr
 *
 * @discussion
 * <b> Description </b><br>
 *   This is the type for the signal function provided by the client. Typically
 *   this would be a function that makes use of an Operating System signal. One
 *   method for implementation of such a signal would be to release a semaphore.
 */
typedef void (*BspI2c_SignalFunctionPtr)( void );

/*===========================================================================*/
/*!
 * @typedef BspI2c_WaitForSignalFunctionPtr
 *
 * @discussion
 * <b> Description </b><br>
 *   This is the type for the wait for signal function provided by the client.
 *   Typically this would be a function that makes use of an Operating System
 *   signal. One method for implementation of such a function would be to pend
 *   on a semaphore that the signal function is going to release.
 */
typedef void (*BspI2c_WaitForSignalFunctionPtr)( void );

/*===========================================================================*/
/*!
 * @typedef BspI2c_DelayValue
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for the delay function value. Note
 *   that this value is provided by the client, and returned by
 *   the I2c driver when the delay function is called.
 */
typedef Uint8 BspI2c_DelayValue;

/*===========================================================================*/
/*!
 * @typedef BspI2c_DelayFunctionPtr
 *
 * @discussion
 * <b> Description </b><br>
 *   This is the type for the delay function pointer  provided by
 *   the client of this driver.
 */
typedef void (*BspI2c_DelayFunctionPtr)( BspI2c_DelayValue );

/*===========================================================================*/
/*!
 * @typedef BspI2c_DmaRequestId
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for the I2C transaction (master or slave)
 *   DMA ReqId. Note that the client must use the correct DMA Request ID
 *   as defiend by the DMA driver or if no DMA is required, set this 
 *   to BSP_DMA_REQ_NONE.
 */
 enum
{
    BSP_DMA_REQ_NONE,
    BSP_DMA_REQ
 };
typedef Uint8 BspI2c_DmaRequestId;


/*=============================================================================
 *  Public Functions
 *============================================================================*/

/*============================================================================*/
/*!
 * @function bspI2c_init
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to initialize the I2C driver. It is called once at
 *   powerup.
 *
 * <b> Context </b><br>
 *   This must be called before any other I2C services and functions
 *
 *  @result <br>
 *     The I2C device driver will be initialized.
 */
BspI2c_ReturnCode
bspI2c_init( void );


/*============================================================================*/
/*!
 * @function bspI2c_config
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to configure the I2C driver.  The I2C must be
 *   configured once before reading and writing to a particular device. 
 *
 * <b> Context </b><br>
 *   The I2C must be configured once before reading and writing. 
 *
 *  @param deviceId
 *            This is the compile time assigned device id for the device
 *            being configured.
 *
 *  @param configInfoPtr
 *            This is the configuration information for the I2C bus. 
 *
 *  @result <br>
 *     The configuration will be stored in hardware.
 */
BspI2c_ReturnCode
bspI2c_config( BspI2c_DeviceId         deviceId,
               const BspI2c_ConfigInfo *configInfoPtr );


/*============================================================================*/
/*!
 * @function bspI2c_write
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to write across the I2C interface. This function will
 *   write as much of the data as possible with a single I2C command. It is
 *   assumed that the data address of the second data is the (startAddress + 1)
 *   If you want to write multiple times to the same address then call this
 *   function multiple times writing one data unit at a time.
 *
 *
 *  @param deviceId
 *            This is the compile-time assigned device id for the device
 *            to write to. The configure for that device must be done prior to
 *            calling this function. 
 * 
 *  @param dataStartAddress
 *             This is the address within the device to write to.
 * 
 *  @param dataCount
 *             This is the number of data to be written.
 * 
 *  @param dataPtr
 *             This is the data to be written.
 * 
 *  @result <br>
 *     The data will be written out using the stored configuration.
 */
BspI2c_ReturnCode
bspI2c_write( BspI2c_DeviceId        deviceId,
              BspI2c_DataAddress     dataStartAddress,
              BspI2c_DataCount       dataCount,
              BspI2c_Data            *dataPtr );


/*============================================================================*/
/*!
 * @function bspI2c_read
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to read data across the I2C interface. This function
 *   will read multiple data from the device starting at the specified start
 *   address. If you want to read multiple times from the same address then call
 *   this function multiple times reading one data unit at a time.
 *
 *
 *  @param deviceId
 *            This is the compile-time assigned device id for the device
 *            to read from. The configure for that device must be done prior to
 *            calling this function. 
 * 
 *  @param dataStartAddress
 *             This is the address within the device to read from.
 * 
 *  @param dataCount
 *             This is the number of data to be read.
 * 
 *  @param dataPtr
 *             This is a pointer to the location where the read data will be
 *             stored. This may be passed in as NULL if the callback is
 *             supplied. 
 * 
 *  @result <br>
 *     Data will be read from an I2C device on the I2C bus.
 */
BspI2c_ReturnCode
bspI2c_read( BspI2c_DeviceId        deviceId,
             BspI2c_DataAddress     dataStartAddress,
             BspI2c_DataCount       dataCount,
             BspI2c_Data            *dataPtr );



/*===========================================================================*/
/*!
 * @typedef BspI2c_Priority
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for  I2C transaction priorities.
 */
enum
{
    BSP_I2C_PRIORITY_LOW,
    BSP_I2C_PRIORITY_HIGH,
    BSP_I2C_NUMBER_OF_PRIORITIES
};
typedef Uint8 BspI2c_Priority;

/*===========================================================================*/
/*!
 * @typedef BspI2c_InstanceId
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for the Instance identifier.
 */
 


#if (LOCOSTO_LITE==0)
enum
{

 BSP_I2C_1,
 BSP_I2C_2,
BSP_I2C_NUM_DEVICES
};
#else
enum
{
 BSP_I2C_2,
 BSP_I2C_NUM_DEVICES
};
#endif
typedef Uint8 BspI2c_InstanceId;
/*===========================================================================*/
/*!  
 * @typedef BspI2cMaster_TransactionType
 *
 * @discussion
 * <b> Description </b><br>
 *       This is a type used to describe a trnsaction in the transaction array of
 *   scenario execution. 
 * <b> Note that only  BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ and 
 *     BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE are valid values for the
 *     user of the API. </b> <br>
 */
enum
{
    BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_UNKNOWN,
    BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_READ,
    BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE,
    BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE_READ,
    BSP_I2C_MULTI_MASTER_TRANSACTION_TYPE_WRITE_NOSTOP
};
typedef Uint8 BspI2cMaster_TransactionType;

/*===========================================================================*/
/*!  
 * @typedef BspI2c_TransactionCount
 *
 * @discussion
 * <b> Description </b><br>
 *   This type is used to indicate the number of transaction in a scenario.
 */
typedef Uint32 BspI2c_TransactionCount;

/*=============================================================================
 * Description:
 *   This enum value controls the generation of STOP condition after the 
 * completetion of the transaction. If the "busControl" is set to 
 * BSP_I2C_BUS_RELEASE then stop condition will be generated and other master
 * can take control of the bus, if it is set to BSP_I2C_BUS_NO_RELEASE then 
 * stop condition will not be generated and device will pull down the clock till
 * a re-start condition is generated or stop condition is generated explicitly.
 */
enum
{
    BSP_I2C_BUS_RELEASE,
    BSP_I2C_BUS_NO_RELEASE
};
typedef Uint8 BspI2cMaster_busControl;

/*===========================================================================*/
/*!  
 * @typedef BspI2c_TransactionRequest
 *
 * @discussion
 * <b> Description </b><br>
 *     This is the  type of transaction information array to be submitted for
 *     scenario execution.
 *
 * @field  id 
 *              This is the instance id of the I2C device to be used for this
 *              trsacation.
 *
 *  @field dataStartAddress
 *             This is the address within the device to read from.
 * 
 *  @field dataCount
 *             This is the number of data to be read.
 * 
 *  @field dataPtr
 *             This is a pointer to the location where the read data will be
 *             stored. 
 * 
 *  @field busControl
 *             This field is reserved and values will be ignored by driver. 
 */
typedef struct {
    BspI2cMaster_TransactionType            transactionType;
    BspI2c_DataAddress                      dataStartAddress;
    BspI2c_Data                             *dataPtr;
    Uint8						typeId;	
   #ifdef Bsp_I2C_DMA_Enable	
    BspI2c_DmaRequestId                     dmaRequestId;
#endif
    BspI2cMaster_busControl                 busControl;
    BspI2c_DataCount                        dataCount;
	
} BspI2c_TransactionRequest;

/*===========================================================================*/
/*!  
 * @typedef BspI2c_ScenarioDescriptor
 *
 * @discussion
 * <b> Description </b><br>
 *     This type gives the information common for all the transaction in  
 *     scenario execution.
 *
 * @field  count 
 *            This is gives number of transaction in the scenario.
 *            trsacation.
 *
 *  @param deviceId
 *            This is the compile-time assigned device id for the device
 *            to read from. The configure for that device must be done prior to
 *            calling this function. 
 * 
 *  @param transactionDoneCallback
 *            This is the  function that the driver will use to signal
 *            the requested transaction has completed.
 *            If NULL is passed in for this then blocking/polling methods will 
 *                        be used.
 */
typedef struct {
    BspI2c_TransactionCount                 count;
    BspI2c_DeviceId                         deviceId;
    BspI2c_TransactionDoneCallback          transactionDoneCallback;
    BspI2c_TransactionId                    transactionId;
    BspI2c_DelayFunctionPtr                 delayFunctionPtr;
    BspI2c_DelayValue                       delayValue;
    BspI2c_Priority                         priority;
} BspI2c_ScenarioDescriptor;


/*=============================================================================
 *  Public Functions
 *============================================================================*/
/*============================================================================*/
/*!
 * @function bspI2cMaster_init
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to initialize the I2C Master HW block It is called
 *   once at powerup.
 *
 * <b> Context </b><br>
 *   This must be called before any other I2C services and functions
 *
 *  @result <br>
 *     The I2C Master HW and internal SW driver will be initialized.
 */
void
bspI2cMaster_init( void );

/*============================================================================*/
/*!
 * @function bspI2c_highLevelread
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to read data across the I2C interface. This function
 *   will read multiple data from the device starting at the specified start
 *   address. If you want to read multiple times from the same address then call
 *   this function multiple times reading one data unit at a time.
 *
 * @param  id 
 *            This is the instance id of the I2C device to be used for this
 *            trsacation.
 *
 *  @param deviceId
 *            This is the compile-time assigned device id for the device
 *            to read from. The configure for that device must be done prior to
 *            calling this function. 
 * 
 *  @param dataStartAddress
 *             This is the address within the device to read from.
 * 
 *  @param dataCount
 *             This is the number of data to be read.
 * 
 *  @param dataPtr
 *             This is a pointer to the location where the read data will be
 *             stored. 
 * 
 *  @param transactionDoneCallback
 *            This is the  function that the driver will use to signal
 *            the requested transaction has completed.
 *            If NULL is passed in for this then blocking/polling methods will 
 *                        be used.
 *
 *  @result <br>
 *     Data will be read from an I2C device on the I2C bus.
 */
BspI2c_ReturnCode
bspI2c_highLevelread( BspI2c_InstanceId                 id,
                      BspI2c_DeviceId                   deviceId,
                      BspI2c_DataAddress                dataStartAddress,
                      BspI2c_DataCount                  dataCount,
                      BspI2c_Data                       *dataPtr,
                      BspI2c_DmaRequestId               dmaRequestId,
                      BspI2c_TransactionDoneCallback    transactionDoneCallback,
                      BspI2c_TransactionId              transactionId,
                      BspI2c_DelayFunctionPtr           delayFunctionPtr,
                      BspI2c_DelayValue                 delayValue );

/*============================================================================*/
/*!
 * @function bspI2c_highLevelwrite
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to write data across the I2C interface. This function
 *   will write multiple data from the device starting at the specified start
 *   address. 
 *
 * @param  id 
 *            This is the instance id of the I2C device to be used for this
 *            trsacation.
 *
 *  @param deviceId
 *            This is the compile-time assigned device id for the device
 *            to read from. The configure for that device must be done prior to
 *            calling this function. 
 * 
 *  @param dataStartAddress
 *             This is the address within the device to read from.
 * 
 *  @param dataCount
 *             This is the number of data to be read.
 * 
 *  @param dataPtr
 *             This is a pointer to the location where the read data will be
 *             stored. 
 * 
 *  @param transactionDoneCallback
 *            This is the  function that the driver will use to signal
 *            the requested transaction has completed.
 *            If NULL is passed in for this then blocking/polling methods will 
 *                        be used.
 *
 *  @result <br>
 *     Data will be written to an I2C device on the I2C bus.
 */
BspI2c_ReturnCode
bspI2c_highLevelwrite( BspI2c_InstanceId                id,
                       BspI2c_DeviceId                  deviceId,
                       BspI2c_DataAddress               dataStartAddress,
                       BspI2c_DataCount                 dataCount,
                       BspI2c_Data                      *dataPtr,
                       BspI2c_DmaRequestId              dmaRequestId,
                       BspI2c_TransactionDoneCallback   transactionDoneCallback,
                       BspI2c_TransactionId             transactionId,
                       BspI2c_DelayFunctionPtr          delayFunctionPtr,
                       BspI2c_DelayValue                delayValue );


/*============================================================================*/
/*!
 * @function bspI2c_activateTransaction
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to retry an activate I2C transmission, in case the
 *    was Busy (BusBusy) or there was an error on the Bus during the last
 *    transaction attempt. This is called both internally within the I2C
 *    driver under normal conditions, as well as externally from the client
 *
 * @param  id 
 *               This is the instance id of the I2C device to be used for this
 *               trsacation.
 *
 *  @result <br>
 *   The next transaction in the I2C queue will be activated if the Bus is
 *     available.
 */
void
bspI2c_activateTransaction( BspI2c_InstanceId id, Bool bspI2c_inInterruptContext);

/*============================================================================*/
/*!
 * @function bspI2c_masterSlaveConfig
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to setup the Slave Receive environment.
 *
 * @param  id 
 *                        This is the instance id of the I2C device to be used for this
 *                        trsacation.
 *
 *  @param dataCount
 *             This is the number of data to be read.
 * 
 *  @param dataPtr
 *             This is a pointer to the location where the read data will be
 *             stored. 
 *
 *  @result <br>
 *   If data is received in Slave mode, it is moved accordingly.
 */
BspI2c_ReturnCode
bspI2c_masterSlaveConfig( BspI2c_InstanceId  id,
                          BspI2c_DeviceId     slaveId, 
                          BspI2c_Data         *dataPtr,
                          BspI2c_DataCount    dataCount,
                          BspI2c_DmaRequestId dmaRequestId,
                          BspI2c_slaveTransactionDoneCallback transactionDoneCallback );

/*============================================================================*/
/*!
 * @function bspI2c_submitTransaction
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is used to start a scenario of connected transaction at I2C.
 *   This function gaurentees the atomocity of the transactions. 
 *
 * @param  id 
 *                        This is the instance id of the I2C device to be used for this
 *                        trsacation.
 *
 * @param  scenarioDesc 
 *                       Information common to the scenario. 
 *
 * @param  transactionArray 
 *                        Array of transactions.
 *
 *
 *  @result <br>
 *   All the enqueued trsaction are completed without generatin STOP condition 
 *   but for last transaction. 
 */
BspI2c_ReturnCode
bspI2c_submitTransaction(BspI2c_InstanceId id,
                BspI2c_ScenarioDescriptor *scenarioDesc,
                BspI2c_TransactionRequest transactionArray[]);

/*============================================================================*/
/*!
 * @function bspI2_Handlerx
 *
 * @discussion
 * <b> Description </b><br>
 *   This function is the Main Function Handler for the I2C 
 *
 * <b> Context </b><br>
 *   This execute in ISR context. 
 *
 *  @result <br>
 *     The I2C HISR will be triggered.
 */
void bspI2c_Handeler1(void);
void bspI2c_Handeler2(void);

#endif
