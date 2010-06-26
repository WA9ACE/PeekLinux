/*=============================================================================
 *    Copyright Texas Instruments 2000-2001. All Rights Reserved. 
 */
#ifndef BSPUTIL_BITUTIL_HEADER
#define BSPUTIL_BITUTIL_HEADER

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspUtil_BitUtil
 *     This file contains utility macros for bit field operations on the
 *     contents of 32-bit, 16-bit, and 8-bit unsigned typed memory locations,
 *     including volatile hardware addresses.
 */

/*===========================================================================*/
/*!
 * @enum BspUtil_BitUtil_DataUnit
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This is the type for the various readable and writable data units.
 *    The enum type must match the number of bits in the data unit. That
 *    restriction makes the data type more usefull.
 */
enum
{
    BSPUTIL_BITUTIL_DATAUNIT_8  = 8,
    BSPUTIL_BITUTIL_DATAUNIT_16 = 16,
    BSPUTIL_BITUTIL_DATAUNIT_32 = 32
};
typedef Uint8 BspUtil_BitUtil_DataUnit;

/*=============================================================================
 * Utility Macros
 */

/*===========================================================================
 */
/*!
 * @define BSPUTIL_BITUTIL_UNITS_TO_OCTETS 
 * 
 * @discussion
 * <b> Description  </b><br>
 *    Macro for converting number of data units to number of octets.
 *
 * <b> Context </b><br>
 *    This macro can be called from anywhere.
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *    The number of octets.
 * 
 * Param  _dataType
 *            This is the type of memory being accessed 8-bit, 16-bit or
 *            32-bit. Must match BspUtil_BitUtil_DataUnit enum values.
 * 
 * Param  _numUnits
 *            This is the number of data units to convert to octets.
 */
#define BSPUTIL_BITUTIL_UNITS_TO_OCTETS( _dataType,   \
                                         _numUnits )  \
(                                                     \
    ( ( (_dataType) * (_numUnits) ) >> 3 )            \
)

/*===========================================================================
 */
/*!
 * @define BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_BYTE_PTR 
 * 
 * @discussion
 * <b> Description  </b><br>
 *    Macros for converting a byte base and an byte offset to a pointer to a
 *    volatile memory location (hardware register).
 *
 * <b> Context </b><br>
 *    Can be called anywhere
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *     A properly typed pointer to a volatile memory location.
 * 
 * Param  _base
 *            A base address.
 * 
 * Param  _offset
 *            A byte index to add to the base address.
 * 
 */
#define BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_LONG_PTR( _base,       \
                                                     _offset )    \
( (volatile Uint32 *)( ((Uint32)(_base)) + ((Uint32)(_offset)) ) )

#define BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_WORD_PTR( _base,       \
                                                     _offset )    \
( (volatile Uint16 *)( ((Uint32)(_base)) + ((Uint32)(_offset)) ) )

#define BSPUTIL_BITUTIL_MAP_BASE_OFFSET_TO_BYTE_PTR( _base,       \
                                                     _offset )    \
( (volatile Uint8 *)( ((Uint32)(_base)) + ((Uint32)(_offset)) ) )

/*===========================================================================
 */
/*!
 * @define  BSPUTIL_BITUTIL_BIT_FIELD_MASK8
 * 
 * @discussion
 * <b> Description  </b><br>
 *    These macros generate a Bit Mask based on a field width and field
 *    offset. The shift left value is range checked on 16 and 8 bit masks
 *    to keep the compiler from complaining about invalid shift values. The
 *    invalid shift values come about in the macros that choose between 8,
 *    16, and 32 bit macros. The resulting code includes all 3 macro calls at
 *    first, then 2 are optimized out. Before the 2 invalid macros are optimized
 *    out the compiler complains about the invalid shift values for the invalid
 *    macros. The range checking is not exact on the shift right value because
 *    there is no simple operation to mask out invalid bits. The shift left
 *    operation is easy to range check singe the valid values can be obtained
 *    with a simple & operation.
 *
 * <b> Context </b><br>
 *    Can be called anywhere
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *     An 8-bit, 16-bit, or 32-bit 1s mask of the field described by the
 *     parameters
 * 
 * Param  _fieldWidth
 *            This is the number of bits in the field
 *            Range (1-32, 1-16, or 1-8 depending on macro)
 *            Note: Field width of 0 is invalid!
 * 
 * Param  _fieldOffset
 *            This is the number of bits the field is offset from
 *            bit 1 of the least significant bit.
 *            Range (0-31, 0-15, or 0-7 depending on macro)
 */
#define BSPUTIL_BITUTIL_BIT_FIELD_MASK32( _fieldWidth, _fieldOffset )       \
(                                                                           \
    ( ( 0xFFFFFFFF >> ( 32 - (_fieldWidth) ) ) << ((_fieldOffset) & 0x1F) ) \
)

#define BSPUTIL_BITUTIL_BIT_FIELD_MASK16( _fieldWidth, _fieldOffset )              \
(                                                                                  \
    ( ( 0xFFFF >> ( ( 16 - (_fieldWidth) ) & 0x0F ) ) << ((_fieldOffset) & 0x0F) ) \
)

#define BSPUTIL_BITUTIL_BIT_FIELD_MASK8( _fieldWidth, _fieldOffset )            \
(                                                                               \
    ( ( 0xFF >> ( ( 8 - (_fieldWidth) ) & 0x07 ) ) << ((_fieldOffset) & 0x07) ) \
)

/*===========================================================================
 */
/*!
 * @define  BSPUTIL_BITUTIL_BIT_FIELD_GET
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro returns the field described by the parameters passed in from
 *    the pointer passed in. A data unit will be used to select the proper
 *    internal macro to use.
 * 
 *    <b>Note:</b> When using this function the compiler will only optimize
 *                 out the conditionals if the data unit is a constant!
 *
 * <b> Context </b><br>
 *    Can be called anywhere
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *     The contents of the field.
 * 
 * Param _dataPtr
 *           This is the pointer to the data array that the
 *           field is in. It must be properly typed for the width
 *           of the operation.
 *
 * Param _dataUnit
 *           This is data unit (8-bit, 16-bit, 32-bit type) for the data that
 *           the field is in.
 *
 * Param _fieldWidth
 *           This is the number of bits in the field
 *           Range (1-32, 1-16, or 1-8 depending on _dataUnit)
 *
 * Param _fieldOffset
 *           This is the number of bits the field is offset
 *           from bit 1 of the least significant bit.
 *           Range (0-31, 0-15, or 0-7 depending on _dataUnit)
 */
#define BSPUTIL_BITUTIL_BIT_FIELD_GET( _dataPtr,                        \
                                       _dataUnit,                       \
                                       _fieldOffset,                    \
                                       _fieldWidth )                    \
(                                                                       \
    ( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_32 ) ?                   \
        BSPUTIL_BITUTIL_BIT_FIELD_GET32( (volatile Uint32 *)(_dataPtr), \
                                         (_fieldOffset),                \
                                         (_fieldWidth) ) :              \
    ( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_16 ) ?                   \
        BSPUTIL_BITUTIL_BIT_FIELD_GET16( (volatile Uint16 *)(_dataPtr), \
                                         (_fieldOffset),                \
                                         (_fieldWidth) ) :              \
    ( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_8 )  ?                   \
        BSPUTIL_BITUTIL_BIT_FIELD_GET8( (volatile Uint8 *)(_dataPtr),   \
                                        (_fieldOffset),                 \
                                        (_fieldWidth) )  :              \
    0                                                                   \
)

/*===========================================================================
 */
/*!
 * @define  BSPUTIL_BITUTIL_GET
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro returns the field described by the parameters passed in from
 *    the pointer passed in. A data unit will be used to select the proper
 *    internal macro to use. This macro adds the added convienence of not
 *    having to pass in the bitwidth and bit offset explicitly and instead
 *    pass in the common part of the defines that define the offset and width.
 * 
 *    <b>Note:</b> When using this function the compiler will only optimize
 *                 out the conditionals if the data unit is a constant!
 *
 * <b> Context </b><br>
 *    Can be called anywhere
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *     The contents of the field.
 * 
 * Param _dataPtr
 *           This is the pointer to the data array that the
 *           field is in. It must be properly typed for the width
 *           of the operation.
 *
 * Param _dataUnit
 *           This is data unit (8-bit, 16-bit, 32-bit type) for the data that
 *           the field is in.
 *
 * Param _name
 *           This is a portion of the preprocessor defines needed to comple
 *           For example: if you define DEFINE_REGISTERX_FIELDY_OFFSET to
 *           be 2 bits and DEFINE_REGISTERX_FIELDY_WIDTH to be 4 bits then
 *           you only need to pass in DEFINE_REGISTERX_FIELDY as the name.
 */
#define BSPUTIL_BITUTIL_GET( _dataPtr,                                      \
                             _dataUnit,                                     \
                             _name )                                        \
(                                                                           \
    ( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_32 ) ?                       \
        BSPUTIL_BITUTIL_BIT_FIELD_GET32( ((volatile Uint32 *)(_dataPtr)),   \
                                         _name ##_OFFSET,                   \
                                         _name ##_WIDTH ) :                 \
    ( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_16 ) ?                       \
        BSPUTIL_BITUTIL_BIT_FIELD_GET16( ((volatile Uint16 *)(_dataPtr)),   \
                                         _name ##_OFFSET,                   \
                                         _name ##_WIDTH ) :                 \
    ( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_8 )  ?                       \
        BSPUTIL_BITUTIL_BIT_FIELD_GET8( ((volatile Uint8 *)(_dataPtr)),     \
                                         _name ##_OFFSET,                   \
                                         _name ##_WIDTH ) :                 \
    0                                                                       \
)

/*===========================================================================
 */
/*!
 * @define  BSPUTIL_BITUTIL_BIT_FIELD_GET8
 * 
 * @discussion
 * <b> Description  </b><br>
 *    These macros return the field described by the parameters passed in from
 *    the pointer passed in.
 * 
 *    <b>Note:</b> If possible the <i>BSPUTIL_BITUTIL_BIT_FIELD_GET</i> should
 *                 be used instead of these macros.
 *
 * <b> Context </b><br>
 *    Can be called anywhere
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *     The contents of the field.
 * 
 * Param _dataPtr
 *           This is the pointer to the data array that the
 *           field is in. It must be properly typed for the width
 *           of the operation.
 *
 * Param _fieldWidth
 *           This is the number of bits in the field
 *           Range (1-32, 1-16, or 1-8 depending on macro)
 *
 * Param _fieldOffset
 *           This is the number of bits the field is offset
 *           from bit 1 of the least significant bit.
 *           Range (0-31, 0-15, or 0-7 depending on macro)
 */
#define BSPUTIL_BITUTIL_BIT_FIELD_GET32( _dataPtr,                        \
                                         _fieldOffset,                    \
                                         _fieldWidth )                    \
(                                                                         \
    (                                                                     \
        (*((volatile Uint32 *)(_dataPtr))) &                              \
        BSPUTIL_BITUTIL_BIT_FIELD_MASK32( (_fieldWidth), (_fieldOffset) ) \
    ) >> (_fieldOffset)                                                   \
)

#define BSPUTIL_BITUTIL_BIT_FIELD_GET16( _dataPtr,                        \
                                         _fieldOffset,                    \
                                         _fieldWidth )                    \
(                                                                         \
    (                                                                     \
        (*((volatile Uint16 *)(_dataPtr))) &                              \
        BSPUTIL_BITUTIL_BIT_FIELD_MASK16( (_fieldWidth), (_fieldOffset) ) \
    ) >> (_fieldOffset)                                                   \
)
#define BSPUTIL_BITUTIL_BIT_SINGLE_GET16(_dataPtr,	\
											_mask	)\
(													\
         (*((volatile Uint16*)(_dataPtr)))&(_mask)				\
)
#define BSPUTIL_BITUTIL_BIT_FIELD_GET8( _dataPtr,                        \
                                        _fieldOffset,                    \
                                        _fieldWidth )                    \
(                                                                        \
    (                                                                    \
        (*((volatile Uint8 *)(_dataPtr))) &                              \
        BSPUTIL_BITUTIL_BIT_FIELD_MASK8( (_fieldWidth), (_fieldOffset) ) \
    ) >> (_fieldOffset)                                                  \
)

/*===========================================================================
 */
/*!
 * @define  BSPUTIL_BITUTIL_GET8
 * 
 * @discussion
 * <b> Description  </b><br>
 *    These macros return the field described by the parameters passed in from
 *    the pointer passed in. A data unit will be used to select the proper
 *    internal macro to use. This macro adds the added convienence of not
 *    having to pass in the bitwidth and bit offset explicitly and instead
 *    pass in the common part of the defines that define the offset and width.
 * 
 *    <b>Note:</b> If possible the <i>BSPUTIL_BITUTIL_GET</i> should be used
 *                 instead of these macros.
 *
 * <b> Context </b><br>
 *    Can be called anywhere
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *     The contents of the field.
 * 
 * Param _dataPtr
 *           This is the pointer to the data array that the
 *           field is in. It must be properly typed for the width
 *           of the operation.
 * 
 * Param _name
 *           This is a portion of the preprocessor defines needed to comple
 *           For example: if you define DEFINE_REGISTERX_FIELDY_OFFSET to
 *           be 2 bits and DEFINE_REGISTERX_FIELDY_WIDTH to be 4 bits then
 *           you only need to pass in DEFINE_REGISTERX_FIELDY as the name.
 */
#define BSPUTIL_BITUTIL_GET32( _name,                 \
                               _dataPtr )             \
(                                                     \
    BSPUTIL_BITUTIL_BIT_FIELD_GET32( (_dataPtr),      \
                                     _name ##_OFFSET, \
                                     _name ##_WIDTH ) \
)

#define BSPUTIL_BITUTIL_GET16( _name,                 \
                               _dataPtr )             \
(                                                     \
    BSPUTIL_BITUTIL_BIT_FIELD_GET16( (_dataPtr),      \
                                     _name ##_OFFSET, \
                                     _name ##_WIDTH ) \
)

#define BSPUTIL_BITUTIL_GET8( _name,                 \
                              _dataPtr )             \
(                                                    \
    BSPUTIL_BITUTIL_BIT_FIELD_GET8( (_dataPtr),      \
                                    _name ##_OFFSET, \
                                    _name ##_WIDTH ) \
)

/*===========================================================================
 */
/*!
 * @define  BSPUTIL_BITUTIL_BIT_FIELD_SET
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro sets the field described by the parameters passed in from
 *    the pointer passed in. A data unit will be used to select the proper
 *    internal macro to use.
 * 
 *    <b>Note:</b> When using this function the compiler will only optimize
 *                 out the conditionals if the data unit is a constant!
 *
 * <b> Context </b><br>
 *    Can be called anywhere
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *    None.
 * 
 * Param _dataPtr
 *           This is the pointer to the data array that the
 *           field is in. It must be properly typed for the width
 *           of the operation.
 * 
 * Param _newValue
 *           This is the new value to set the field to.
 *
 * Param _dataUnit
 *           This is data unit (8-bit, 16-bit, 32-bit type) for the data that
 *           the field is in.
 *
 * Param _fieldWidth
 *           This is the number of bits in the field
 *           Range (1-32, 1-16, or 1-8 depending on _dataUnit)
 *
 * Param _fieldOffset
 *           This is the number of bits the field is offset
 *           from bit 1 of the least significant bit.
 *           Range (0-31, 0-15, or 0-7 depending on _dataUnit)
 */
#define BSPUTIL_BITUTIL_BIT_FIELD_SET( _dataPtr,            \
                                       _newValue,           \
                                       _dataUnit,           \
                                       _fieldOffset,        \
                                       _fieldWidth )        \
{                                                           \
    if( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_32 )       \
    {                                                       \
        BSPUTIL_BITUTIL_BIT_FIELD_SET32( (_dataPtr),        \
                                         (_newValue),       \
                                         (_fieldOffset),    \
                                         (_fieldWidth) );   \
    }                                                       \
    else if( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_16 )  \
    {                                                       \
        BSPUTIL_BITUTIL_BIT_FIELD_SET16( (_dataPtr),        \
                                         (_newValue),       \
                                         (_fieldOffset),    \
                                         (_fieldWidth) );   \
    }                                                       \
    else if( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_8 )   \
    {                                                       \
        BSPUTIL_BITUTIL_BIT_FIELD_SET8( (_dataPtr),         \
                                        (_newValue),        \
                                        (_fieldOffset),     \
                                        (_fieldWidth) );    \
    }                                                       \
}

/*===========================================================================
 */
/*!
 * @define  BSPUTIL_BITUTIL_SET
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro sets the field described by the parameters passed in from
 *    the pointer passed in. A data unit will be used to select the proper
 *    internal macro to use. This macro adds the added convienence of not
 *    having to pass in the bitwidth and bit offset explicitly and instead
 *    pass in the common part of the defines that define the offset and width.
 * 
 *    <b>Note:</b> When using this function the compiler will only optimize
 *                 out the conditionals if the data unit is a constant!
 *
 * <b> Context </b><br>
 *    Can be called anywhere
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *    None.
 * 
 * Param _dataPtr
 *           This is the pointer to the data array that the
 *           field is in. It must be properly typed for the width
 *           of the operation.
 *
 * Param _newValue
 *           This is the new value to set the field to.
 *
 * Param _dataUnit
 *           This is data unit (8-bit, 16-bit, 32-bit type) for the data that
 *           the field is in.
 *
 * Param _name
 *           This is a portion of the preprocessor defines needed to comple
 *           For example: if you define DEFINE_REGISTERX_FIELDY_OFFSET to
 *           be 2 bits and DEFINE_REGISTERX_FIELDY_WIDTH to be 4 bits then
 *           you only need to pass in DEFINE_REGISTERX_FIELDY as the name.
 */
#define BSPUTIL_BITUTIL_SET( _dataPtr,                      \
                             _newValue,                     \
                             _dataUnit,                     \
                             _name )                        \
{                                                           \
    if( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_32 )       \
    {                                                       \
        BSPUTIL_BITUTIL_SET32( _name,                       \
                               (_dataPtr),                  \
                               (_newValue) );               \
    }                                                       \
    else if( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_16 )  \
    {                                                       \
        BSPUTIL_BITUTIL_SET16( _name,                       \
                               (_dataPtr),                  \
                               (_newValue) );               \
    }                                                       \
    else if( (_dataUnit) ==  BSPUTIL_BITUTIL_DATAUNIT_8 )   \
    {                                                       \
        BSPUTIL_BITUTIL_SET8( _name,                        \
                              (_dataPtr),                   \
                              (_newValue) );                \
    }                                                       \
}

/*===========================================================================
 */
/*!
 * @define  BSPUTIL_BITUTIL_BIT_FIELD_SET8
 * 
 * @discussion
 * <b> Description  </b><br>
 *    These macros sets the field described by the parameters passed in from
 *    the pointer passed in.
 * 
 *    <b>Note:</b> If possible the <i>BSPUTIL_BITUTIL_BIT_FIELD_SET</i> should
 *                 be used instead of these macros.
 *
 * <b> Context </b><br>
 *    Can be called anywhere
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *    None.
 * 
 * Param _dataPtr
 *           This is the pointer to the data array that the
 *           field is in. It must be properly typed for the width
 *           of the operation.
 *
 * Param _newValue
 *           This is the new value to set the field to.
 *
 * Param _fieldWidth
 *           This is the number of bits in the field
 *           Range (1-32, 1-16, or 1-8 depending on macro)
 *
 * Param _fieldOffset
 *           This is the number of bits the field is offset
 *           from bit 1 of the least significant bit.
 *           Range (0-31, 0-15, or 0-7 depending on macro)
 */
#define BSPUTIL_BITUTIL_BIT_FIELD_SET32( _dataPtr,                                    \
                                         _newValue,                                   \
                                         _fieldOffset,                                \
                                         _fieldWidth )                                \
{                                                                                     \
    if( ( (_fieldOffset) == 0 ) && ( (_fieldWidth) == 32 ) )                          \
    {                                                                                 \
        (*((volatile Uint32 *)(_dataPtr))) =  (Uint32)(_newValue);                    \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
        (*((volatile Uint32 *)(_dataPtr))) =                                          \
            (                                                                         \
                (                                                                     \
                    (*((volatile Uint32 *)(_dataPtr))) &                              \
                    ~( BSPUTIL_BITUTIL_BIT_FIELD_MASK32( (_fieldWidth),               \
                                                         (_fieldOffset) ) )           \
                )|                                                                    \
                ( (Uint32)( ( (Uint32)(_newValue) << (_fieldOffset) ) &               \
                              BSPUTIL_BITUTIL_BIT_FIELD_MASK32( (_fieldWidth),        \
                                                                (_fieldOffset) ) ) )  \
            );                                                                        \
    }                                                                                 \
}

#define BSPUTIL_BITUTIL_BIT_FIELD_SET16( _dataPtr,                                    \
                                         _newValue,                                   \
                                         _fieldOffset,                                \
                                         _fieldWidth )                                \
{                                                                                     \
    if( ( (_fieldOffset) == 0 ) && ( (_fieldWidth) == 16 ) )                          \
    {                                                                                 \
        (*((volatile Uint16 *)(_dataPtr))) =  (Uint16)(_newValue);                    \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
        (*((volatile Uint16 *)(_dataPtr))) =                                          \
            (                                                                         \
                (                                                                     \
                    (*((volatile Uint16 *)(_dataPtr))) &                              \
                    ~( BSPUTIL_BITUTIL_BIT_FIELD_MASK16( (_fieldWidth),               \
                                                         (_fieldOffset) ) )           \
                )|                                                                    \
                ( (Uint16)( ( (Uint16)(_newValue) << ((_fieldOffset) & 0xF) ) &       \
                              BSPUTIL_BITUTIL_BIT_FIELD_MASK16( (_fieldWidth),        \
                                                                (_fieldOffset) ) ) )  \
            );                                                                        \
    }                                                                                 \
}

#define BSPUTIL_BITUTIL_BIT_SINGLE_SET16(_dataptr		\
										  ,_mask )		\
(														\
               (*((volatile Uint16*)(_dataptr)))|=_mask				\
)

#define BSPUTIL_BITUTIL_BIT_SINGLE_CLEAR16(_dataptr		\
										  ,_mask )		\
(														\
               *((volatile Uint16*)(_dataptr))&=_mask				\
)

#define BSPUTIL_BITUTIL_BIT_SINGLE_SETCLEAR16(_dataptr		\
										  ,_mask )			\
(															\
(*((volatile Uint16*)(_dataptr)))=_mask							\
)




#define BSPUTIL_BITUTIL_BIT_FIELD_SET8( _dataPtr,                                    \
                                        _newValue,                                   \
                                        _fieldOffset,                                \
                                        _fieldWidth )                                \
{                                                                                    \
    if( ( (_fieldOffset) == 0 ) && ( (_fieldWidth) == 8 ) )                          \
    {                                                                                \
        (*((volatile Uint8 *)(_dataPtr))) =  (Uint8)(_newValue);                     \
    }                                                                                \
    else                                                                             \
    {                                                                                \
        (*((volatile Uint8 *)(_dataPtr))) =                                          \
            (                                                                        \
                (                                                                    \
                    (*((volatile Uint8 *)(_dataPtr))) &                              \
                    ~( BSPUTIL_BITUTIL_BIT_FIELD_MASK8( (_fieldWidth),               \
                                                        (_fieldOffset) ) )           \
                )|                                                                   \
                ( (Uint8)( ( (Uint8)(_newValue) << ((_fieldOffset) & 0x7) ) &        \
                              BSPUTIL_BITUTIL_BIT_FIELD_MASK8( (_fieldWidth),        \
                                                               (_fieldOffset) ) ) )  \
            );                                                                       \
    }                                                                                \
}

/*===========================================================================
 */
/*!
 * @define  BSPUTIL_BITUTIL_SET8
 * 
 * @discussion
 * <b> Description  </b><br>
 *    These macros sets the field described by the parameters passed in from
 *    the pointer passed in. A data unit will be used to select the proper
 *    internal macro to use. This macro adds the added convienence of not
 *    having to pass in the bitwidth and bit offset explicitly and instead
 *    pass in the common part of the defines that define the offset and width.
 * 
 *    <b>Note:</b> If possible the <i>BSPUTIL_BITUTIL_SET</i> should be used
 *                 instead of these macros.
 *
 * <b> Context </b><br>
 *    Can be called anywhere
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *     The contents of the field.
 * 
 * Param _name
 *           This is a portion of the preprocessor defines needed to comple
 *           For example: if you define DEFINE_REGISTERX_FIELDY_OFFSET to
 *           be 2 bits and DEFINE_REGISTERX_FIELDY_WIDTH to be 4 bits then
 *           you only need to pass in DEFINE_REGISTERX_FIELDY as the name.
 *
 * Param _newValue
 *           This is the new value to set the field to.
 * 
 * Param _dataPtr
 *           This is the pointer to the data array that the
 *           field is in. It must be properly typed for the width
 *           of the operation.
 */
#define BSPUTIL_BITUTIL_SET32( _name,                 \
                               _ptr,                  \
                               _newValue )            \
{                                                     \
    BSPUTIL_BITUTIL_BIT_FIELD_SET32( (_ptr),          \
                                     (_newValue),     \
                                     _name ##_OFFSET, \
                                     _name ##_WIDTH ) \
}

#define BSPUTIL_BITUTIL_SET16( _name,                 \
                               _ptr,                  \
                               _newValue )            \
{                                                     \
    BSPUTIL_BITUTIL_BIT_FIELD_SET16( (_ptr),          \
                                     (_newValue),     \
                                     _name ##_OFFSET, \
                                     _name ##_WIDTH ) \
}

#define BSPUTIL_BITUTIL_SET8( _name,                 \
                              _ptr,                  \
                              _newValue )            \
{                                                    \
    BSPUTIL_BITUTIL_BIT_FIELD_SET8( (_ptr),          \
                                    (_newValue),     \
                                    _name ##_OFFSET, \
                                    _name ##_WIDTH ) \
}
#define BSPUTIL_BITUTIL_SET16_ONLY(_dataPtr,\
	_newValue)									\
{											\
(*((volatile Uint16 *)(_dataPtr))) =  (Uint16)(_newValue);	\
}


#endif
