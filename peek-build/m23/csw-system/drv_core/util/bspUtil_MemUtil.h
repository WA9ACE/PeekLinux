/*=============================================================================
 *    Copyright Texas Instruments 2001-2002. All Rights Reserved. 
 */
#ifndef BSPUTIL_MEMUTIL_HEADER
#define BSPUTIL_MEMUTIL_HEADER

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspUtil_MemUtil
 *     This file contains utility functions to read and write to regions of
 *     memory. The MemUtil component is setup in the BSP to allow applications
 *     to easily redirect the low level memory reads and writes. Typically this
 *     would be used when host testing and device driver reads and writes to
 *     memory need to be redirected.
 * 
 *     The application using macros and/or functions defined in this component
 *     must check the source and destination memory types and use the
 *     appropriate data type/unit to prevent an error. These macros and
 *     functions assume that the source and destination memory can be accessed
 *     in data sizes and on boundaries that match the selected data type/unit.
 *     For example: calling the BSPUTIL_MEMUTIL_READ macro with a dataType of
 *     32-bits (BSPUTIL_BITUTIL_DATAUNIT_32) will force a 32-bit access of the
 *     source memory. If the passed in address is not on a 32-bit boundary there
 *     will be an error.
 */

#include "bspUtil_BitUtil.h"

/*=============================================================================
 * Utility Macros
 */

/*===========================================================================
 */
/*!
 * @define BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION 
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This define forces the macros to either call the memUtil function or
 *    the bitUtil macros. Currently only host builds are going to use the 
 *    functions to access memory.
 */
#if defined (HOST_BUILD) || defined (ARMULATOR_BUILD)
#    define BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION  1
#else
#    define BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION  0
#endif

/*===========================================================================
 */
/*!
 * @define BSPUTIL_MEMUTIL_MEMCPY
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro reads the specified number of data units from the specified
 *    source location and copies them to the specified destination location.
 *    No check is done to make sure the source and destination addresses are
 *    on valid boundaries.
 *
 * <b> Context </b><br>
 *    Can be called anywhere.
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *    The number of data units copied.
 * 
 * Param  _dstAdr
 *            This is the address of the memory to write to.
 * 
 * Param  _srcAdr
 *            This is the address of the memory to read from.
 * 
 * Param  _dataType
 *            This is the type of memory being accessed 8-bit, 16-bit or
 *            32-bit. Must match BspUtil_BitUtil_DataUnit enum values.
 * 
 * Param  _numData
 *            This is the number of data units to copy. The number of bits per
 *            data unit depends on the _dataType.
 */
#define BSPUTIL_MEMUTIL_MEMCPY( _dstPtr,                   \
                                _srcPtr,                   \
                                _dataType,                 \
                                _numData )                 \
(                                                          \
    ( (_dataType) ==  BSPUTIL_BITUTIL_DATAUNIT_32 ) ?      \
        bspUtil_MemUtil_memcpy32( ((Uint32 *)(_dstPtr)),   \
                                  ((Uint32 *)(_srcPtr)),   \
                                  (_numData) ) :           \
    ( (_dataType) ==  BSPUTIL_BITUTIL_DATAUNIT_16 ) ?      \
        bspUtil_MemUtil_memcpy16( ((Uint16 *)(_dstPtr)),   \
                                  ((Uint16 *)(_srcPtr)),   \
                                  (_numData) ) :           \
    ( (_dataType) ==  BSPUTIL_BITUTIL_DATAUNIT_8 )  ?      \
        bspUtil_MemUtil_memcpy8( ((Uint8 *)(_dstPtr)),     \
                                 ((Uint8 *)(_srcPtr)),     \
                                 (_numData) ) :            \
    0                                                      \
)

/*===========================================================================
 */
/*!
 * @define BSPUTIL_MEMUTIL_READ 
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro reads the specified number of bits from the specified
 *    location.
 *
 * <b> Context </b><br>
 *    Can be called anywhere when compiled to skip the funciton call.
 *    When using the memUtil function calls this macro may only be used
 *    after the memory has been initialized that the memUtil functions
 *    use.
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *    The data at the requested at the memory location.
 * 
 * Param  _srcAdr
 *            This is the address of the memory to read from.
 * 
 * Param  _dataType
 *            This is the type of memory being accessed 8-bit, 16-bit or
 *            32-bit. Must match BspUtil_BitUtil_DataUnit enum values.
 * 
 * Param  _bitOffset
 *            This is the offset from the least significant bit.
 * 
 * Param  _numBits
 *            This is the number of bits to read.
 */
#define BSPUTIL_MEMUTIL_READ( _srcAdr,                                 \
                              _dataType,                               \
                              _bitOffset,                              \
                              _numBits )                               \
(                                                                      \
    ( BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION == 1 ) ?              \
        bspUtil_MemUtil_read( ((void *)(_srcAdr)),                     \
                              ((BspUtil_BitUtil_DataUnit)(_dataType)), \
                              ((Uint8)(_bitOffset)),                   \
                              ((Uint16)(_numBits)) )       :           \
        BSPUTIL_BITUTIL_BIT_FIELD_GET( (_srcAdr),                      \
                                       (_dataType),                    \
                                       (_bitOffset),                   \
                                       (_numBits) )                    \
)


/*===========================================================================
 */
/*!
 * @define BSPUTIL_MEMUTIL_READ_MULTI 
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro reads the specified number of units from the specified
 *    location.
 *
 * <b> Context </b><br>
 *    Can be called anywhere when compiled to skip the funciton call.
 *    When using the memUtil function calls this macro may only be used
 *    after the memory has been initialized that the memUtil functions
 *    use.
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *    The number of units read from the location.
 * 
 * Param  _dstAdr
 *            This is the address to write the read memory.
 * 
 * Param  _srcAdr
 *            This is the address of the memory to read from.
 * 
 * Param  _dataType
 *            This is the type of memory being accessed 8-bit, 16-bit or
 *            32-bit. Must match BspUtil_BitUtil_DataUnit enum values.
 * 
 * Param  _numUnits
 *            This is the number of data units to read.
 */
#define BSPUTIL_MEMUTIL_READ_MULTI( _dstAdr,                                \
                                    _srcAdr,                                \
                                    _dataType,                              \
                                    _numUnits )                             \
(                                                                           \
    ( BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION == 1 ) ?                   \
        bspUtil_MemUtil_readMulti( ((void *)(_dstAdr)),                     \
                                   ((void *)(_srcAdr)),                     \
                                   ((BspUtil_BitUtil_DataUnit)(_dataType)), \
                                   ((Uint16)(_numUnits)) ) :                \
        (Uint32)BSPUTIL_MEMUTIL_MEMCPY( ((void *)(_dstAdr)),                \
                                        ((void *)(_srcAdr)),                \
                                        (_dataType),                        \
                                        (_numUnits) )                       \
)

/*===========================================================================
 */
/*!
 * @define BSPUTIL_MEMUTIL_WRITE 
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro writes the new value passed in into the pointed to memory
 *    location. The memory location may be read before written to if the bit
 *    offset is not 0 and the bit width is smaller than the dataType size
 *
 * <b> Context </b><br>
 *    Can be called anywhere when compiled to skip the funciton call.
 *    When using the memUtil function calls this macro may only be used
 *    after the memory has been initialized that the memUtil functions
 *    use.
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *    None.
 * 
 * Param  _dstAdr
 *            This is the address of the memory to write to.
 * 
 * Param  _newVal
 *            This is new value to write to the specified location.
 * 
 * Param  _dataType
 *            This is the type of memory being accessed 8-bit, 16-bit or
 *            32-bit. Must match BspUtil_BitUtil_DataUnit enum values.
 * 
 * Param  _bitOffset
 *            This is the offset from the least significant bit of the
 *            destination data that the data will be writen to.
 * 
 * Param  _numBits
 *            This is the number of bits to write.
 */
#define BSPUTIL_MEMUTIL_WRITE( _dstAdr,                      \
                               _newVal,                      \
                               _dataType,                    \
                               _bitOffset,                   \
                               _numBits )                    \
{                                                            \
    if( BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION == 1 )    \
    {                                                        \
        bspUtil_MemUtil_write( ((void *)(_dstAdr)),          \
                               ((Uint32)(_newVal)),          \
                               ((BspUtil_BitUtil_DataUnit)(_dataType)), \
                               ((Uint8)(_bitOffset)),        \
                               ((Uint16)(_numBits)) );       \
    }                                                        \
    else                                                     \
    {                                                        \
        BSPUTIL_BITUTIL_BIT_FIELD_SET( (_dstAdr),            \
                                       (_newVal),            \
                                       (_dataType),          \
                                       (_bitOffset),         \
                                       (_numBits) );         \
    }                                                        \
}


/*===========================================================================
 */
/*!
 * @define BSPUTIL_MEMUTIL_WRITE_MULTI 
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro writes the specified data to the specified location
 *
 * <b> Context </b><br>
 *    Can be called anywhere when compiled to skip the funciton call.
 *    When using the memUtil function calls this macro may only be used
 *    after the memory has been initialized that the memUtil functions
 *    use.
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *    The number of units written.
 * 
 * Param  _dstAdr
 *            This is the address to write the read memory.
 * 
 * Param  _srcAdr
 *            This is the address of the memory to read from.
 * 
 * Param  _dataType
 *            This is the type of memory being accessed 8-bit, 16-bit or
 *            32-bit. Must match BspUtil_BitUtil_DataUnit enum values.
 * 
 * Param  _numUnits
 *            This is the number of data units to read.
 */
#define BSPUTIL_MEMUTIL_WRITE_MULTI( _dstAdr,                                \
                                     _dataPtr,                               \
                                     _dataType,                              \
                                     _numUnits )                             \
{                                                                            \
    ( BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION == 1 )  ?                   \
        bspUtil_MemUtil_writeMulti( ((void *)(_dstAdr)),                     \
                                    ((void *)(_dataPtr)),                    \
                                    ((BspUtil_BitUtil_DataUnit)(_dataType)), \
                                    ((Uint16)(_numUnits)) ) :                \
        (Uint32)BSPUTIL_MEMUTIL_MEMCPY( ((void *)(_dstAdr)),                 \
                                        ((void *)(_dataPtr)),                \
                                        (_dataType),                         \
                                        (_numUnits) );                       \
}


/*===========================================================================
 */
/*!
 * @function bspUtil_MemUtil_memcpy32
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro reads the specified number of 32-bit data units from the
 *    specified source location and copies them to the specified destination
 *    location. No check is done to make sure the source and destination
 *    addresses are on 32-bit boundaries.
 * 
 * @param dstAdr
 *            This is the address of the buffer that the read data will be
 *            written
 * 
 * @param srcAdr
 *            This is the address of the source data to copy.
 * 
 * @param numData
 *            This is the number of 32-bit data units to copy.
 * 
 * @result
 *     The data will be copied and the number of units copied will be
 *     returned. The number returned should always be the same as the
 *     numData passed in.
 */
Uint32
bspUtil_MemUtil_memcpy32( Uint32 *dstAdr,
                          Uint32 *srcAdr,
                          Uint32 numData );


/*===========================================================================
 */
/*!
 * @function bspUtil_MemUtil_memcpy16
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro reads the specified number of 16-bit data units from the
 *    specified source location and copies them to the specified destination
 *    location. No check is done to make sure the source and destination
 *    addresses are on 16-bit boundaries.
 * 
 * @param dstAdr
 *            This is the address of the buffer that the read data will be
 *            written
 * 
 * @param srcAdr
 *            This is the address of the source data to copy.
 * 
 * @param numData
 *            This is the number of 16-bit data units to copy.
 * 
 * @result
 *     The data will be copied and the number of units copied will be
 *     returned. The number returned should always be the same as the
 *     numData passed in.
 */
Uint32
bspUtil_MemUtil_memcpy16( Uint16 *dstAdr,
                          Uint16 *srcAdr,
                          Uint32 numData );


/*===========================================================================
 */
/*!
 * @function bspUtil_MemUtil_memcpy8
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro reads the specified number of 8-bit data units from the
 *    specified source location and copies them to the specified destination
 *    location. No check is done to make sure the source and destination
 *    addresses are on 8-bit boundaries.
 * 
 * @param dstAdr
 *            This is the address of the buffer that the read data will be
 *            written
 * 
 * @param srcAdr
 *            This is the address of the source data to copy.
 * 
 * @param numData
 *            This is the number of 8-bit data units to copy.
 * 
 * @result
 *     The data will be copied and the number of units copied will be
 *     returned. The number returned should always be the same as the
 *     numData passed in.
 */
Uint32
bspUtil_MemUtil_memcpy8( Uint8  *dstAdr,
                         Uint8  *srcAdr,
                         Uint32 numData );

/*********************
 * The following functions and data types could be conditionally compiled, because
 * they should only be needed when BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION is set
 * to 1. They are conditionally compiled in the .c file, but they are needed here
 * to prevent warnings from some compilers. The macros above conditionally refer to the
 * functions defined below.
 *********************/
/*===========================================================================*/
/*!
 * @typedef BspUtil_MemUtil_ReadFunctionPtr
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the funciton pointer type for the memory read function.
 * 
 * @field dstAdr
 *            This the address of the buffer that the read data will be written
 * 
 * @field srcAdr
 *            This the address of the memory to read.
 * 
 * @field dataType
 *            This is the type (8-bit, 16-bit, or 32-bit) of memory and data
 *            being read.
 * 
 * @field numUnits
 *             This is the number of data units to read. If this is set to 1 and
 *             the dataType indicates 32-bits then 4 bytes will be read and
 *             returned
 * 
 *  @result
 *     The function must read the requested memory and return the number of
 *     units read. The data read will be in the passed in dstAdr buffer
 */
typedef Uint32 (*BspUtil_MemUtil_ReadFunctionPtr)( void                      *dstAdr,
                                                   void                      *srcAdr,
                                                   BspUtil_BitUtil_DataUnit  dataType,
                                                   Uint16                    numUnits );

/*===========================================================================*/
/*!
 * @typedef BspUtil_MemUtil_WriteFunctionPtr
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the funciton pointer type for the memory write function.
 * 
 * @field dstAdr
 *            This the address of the buffer that the data will be written to.
 * 
 * @field dataPtr
 *            This is a pointer to the data that is to be written to the
 *            dstAdr
 * 
 * @field dataType
 *            This is the type (8-bit, 16-bit, or 32-bit) of memory and data
 *            being read.
 * 
 * @field numUnits
 *             This is the number of data units to read. If this is set to 1 and
 *             the dataType indicates 32-bits then 4 bytes will be read and
 *             returned
 * 
 *  @result
 *     The function must write the requested data to teh requested memory and
 *     return the number of units written.
 */
typedef Uint32 (*BspUtil_MemUtil_WriteFunctionPtr)( void                      *dstAdr,
                                                    void                      *dataPtr,
                                                    BspUtil_BitUtil_DataUnit  dataType,
                                                    Uint16                    numUnits );

/*===========================================================================
 */
/*!
 * @function bspUtil_MemUtil_setReadFunction
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This function sets the read function to be called in place of the
 *    memUtil read macros
 * 
 * <b> Context  </b><br>
 *    This function is only available when the pre-processor define 
 *    BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION is set to 1.
 * 
 * @param readFunctionPtr
 *            The new function to use when reading memory using the MEMUTIL
 *            read macros
 * 
 * @result
 *     TRUE if the funciton pointer was stored, FALSE otherwise.
 */
Bool
bspUtil_MemUtil_setReadFunction( BspUtil_MemUtil_ReadFunctionPtr readFunctionPtr );

/*===========================================================================
 */
/*!
 * @function bspUtil_MemUtil_setWriteFunction
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This function sets the write function to be called in place of the
 *    memUtil read macros
 * 
 * <b> Context  </b><br>
 *    This function is only available when the pre-processor define 
 *    BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION is set to 1.
 * 
 * @param writeFunctionPtr
 *            The new function to use when writing memory using the MEMUTIL
 *            write macros
 * 
 * @result
 *     TRUE if the funciton pointer was stored, FALSE otherwise.
 */
Bool
bspUtil_MemUtil_setWriteFunction( BspUtil_MemUtil_WriteFunctionPtr writeFunctionPtr );

/*===========================================================================
 */
/*!
 * @function bspUtil_MemUtil_read
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This function reads memory and returns the result of the read. Shifting
 *    and masking are also applied so only certain bits may be read. This read
 *    function is only capable or reading from a single data unit. 
 * 
 * <b> Context  </b><br>
 *    This function is only available when the pre-processor define 
 *    BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION is set to 1.
 * 
 * @param srcAdr
 *            The source address of the memory to read.
 * 
 * @param dataType
 *            The type of the memory/data being read 8-bit, 16-bit, or 32-bit
 *            
 * @param bitOffset
 *            This is the bit offset from the least significant bit being read.
 * 
 * @param numBits
 *            This is the number of bits to read from the memory.
 * 
 * @result
 *     The memory location will be read and the value will be returned shifted
 *     so the least significant bit is all the way to the right
 */
Uint32
bspUtil_MemUtil_read( void                      *srcAdr,
                      BspUtil_BitUtil_DataUnit  dataType,
                      Uint8                     bitOffset,
                      Uint16                    numBits );

/*===========================================================================
 */
/*!
 * @function bspUtil_MemUtil_readMulti
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This function reads memory into a passed in buffer location. This read
 *    function is capable or reading multiple data units, but it cannot shift
 *    inline with the read operation. 
 * 
 * <b> Context  </b><br>
 *    This function is only available when the pre-processor define 
 *    BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION is set to 1.
 * 
 * @param dstAdr
 *            This the address of the buffer that the read data will be written
 * 
 * @param srcAdr
 *            The source address of the memory to read.
 * 
 * @param dataType
 *            The type of the memory/data being read 8-bit, 16-bit, or 32-bit
 *            
 * @param numUnits
 *            This is the number of units to read from the memory.
 * 
 * @result
 *     The memory location will be read and the values will be copied into the
 *     passed in buffer. The number of units read will be returned.
 */
Uint32
bspUtil_MemUtil_readMulti( void                      *dstAdr,
                           void                      *srcAdr,
                           BspUtil_BitUtil_DataUnit  dataType,
                           Uint16                    numUnits );

/*===========================================================================
 */
/*!
 * @function bspUtil_MemUtil_write
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function is used to write memory and properly shift and mask in any
 *   offset data. This function may need to do a read-modify-write if the bit
 *   offset and bit width do not cover the entire data unit.
 * 
 * <b> Context  </b><br>
 *    This function is only available when the pre-processor define 
 *    BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION is set to 1.
 * 
 * @param dstAdr
 *            The address of the memory to write to.
 * 
 * @param newValue
 *            The data to write to the dstAdr.
 * 
 * @param dataType
 *            The type of the memory/data being written 8-bit, 16-bit, or 32-bit
 *            
 * @param bitOffset
 *            This is the bit offset from the least significant bit being written
 *            to.
 * 
 * @param numBits
 *            This is the number of least significant bits of newValue to write
 *            to the memory.
 * 
 * @result
 *     The memory location will be read and the value will be returned shifted
 *     so the least significant bit is all the way to the right
 */
Uint32
bspUtil_MemUtil_write( void                      *dstAdr,
                       Uint32                    newValue,
                       BspUtil_BitUtil_DataUnit  dataType,
                       Uint8                     bitOffset,
                       Uint16                    numBits );

/*===========================================================================
 */
/*!
 * @function bspUtil_MemUtil_writeMulti
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function is used to write data into the passe in memory location.
 *   This is esentially a wrapper for the local write function.
 * 
 * <b> Context  </b><br>
 *    This function is only available when the pre-processor define 
 *    BSPUTIL_MEMUTIL_USE_MEMORY_ACCESS_FUNCTION is set to 1.
 * 
 * @param dstAdr
 *            This the address of the buffer that the read data will be written
 * 
 * @param dataPtr
 *            This is a pointer to the data to write to memory.
 * 
 * @param dataType
 *            The type of the memory/data being read 8-bit, 16-bit, or 32-bit
 *            
 * @param numUnits
 *            This is the number of units to read from the memory.
 * 
 * @result
 *     The data will be written and the number of units written will be
 *     returned. The number returned should always be the same as the
 *     numUnits passed in.
 */
Uint32
bspUtil_MemUtil_writeMulti( void                      *dstAdr,
                            void                      *dataPtr,
                            BspUtil_BitUtil_DataUnit  dataType,
                            Uint16                    numUnits );

#endif
