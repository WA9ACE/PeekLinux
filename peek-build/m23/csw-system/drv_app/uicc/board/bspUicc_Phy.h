
/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_PHY_HEADER

#define BSP_UICC_PHY_HEADER
/*=============================================================================
 * Component Description:
 *    Header file for the Usim block specific functions
 */

/*=============================================================================*/

/*!
 * @#define BSP_UICC_READ_TIMEOUT_VALUE
 * 
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the sim timeout value ~ 18seconds
 */
#define BSP_UICC_READ_TIMEOUT_VALUE 0x2000

/*!
 * @typedef BspUicc_clockStopWAstate
 * 
 * @discussion
 * <b> Description  </b><br>
 * used to keep state in  STATEMACHINE for decoding response.
 */

enum {
  WAIT_FOR_ACK_NAK_SW_NULL=0,
  NAK_RECEIVED,
  ACK_RECEIVED,
  DATA_RECEIVED,
  SW1_RECEIVED
 };  
typedef SYS_UWORD8 BspUicc_clockStopWAstate;




/*!
 * @typedef BspUicc_Phy_ReturnCode
 * 
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the return code
 */
enum
{
    BSP_UICC_PHY_RETURN_CODE_FAILURE    = (-1),
    BSP_UICC_PHY_RETURN_CODE_SUCCESS    = (0)
};
typedef SYS_WORD16 BspUicc_Phy_ReturnCode;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_Protocol
 * 
 * @discussion
 * <b> Description  </b><br>
 *   Specifies the type for the Protocol used (Char or Block).
 */
enum
{
    BSP_UICC_PHY_T0_PROTOCOL            = 0,
    BSP_UICC_PHY_T1_PROTOCOL            = 1
};
typedef SYS_UWORD8 BspUicc_Phy_Protocol;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_ClockStop
 * 
 * @discussion
 * <b> Description  </b><br>
 *   Specifies the type for Clock stop parameter used to enable/disable sleep mode
 */
enum
{
    BSP_UICC_PHY_CLOCK_STOP_DISABLE = 0,
    BSP_UICC_PHY_CLOCK_STOP_ENABLE  = 1
};
typedef SYS_UWORD8 BspUicc_Phy_ClockStop;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_SClockLevel
 * 
 * @discussion
 * <b> Description  </b><br>
 *   Specifies the type for Clock stop parameter used to enable/disable sleep mode
 */
enum
{
    BSP_UICC_PHY_SCLOCK_NO_STOP    = 0, /* Clock stop is not Supported */
    BSP_UICC_PHY_SCLOCK_LEVEL_LOW  = 1,
    BSP_UICC_PHY_SCLOCK_LEVEL_HIGH = 2
};
typedef SYS_UWORD8 BspUicc_Phy_SClkLev;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_SclkDiv
 * 
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for number of Sclock division configuration paramater.
 */
enum
{
    BSP_UICC_PHY_SCLKDIV_13_2           = 0, /* for 13/2 MHz */
    BSP_UICC_PHY_SCLKDIV_13_4           = 1, /* for 13/4 MHz */
    BSP_UICC_PHY_SCLKDIV_13_8           = 2, /* for 13/8 MHz */
    BSP_UICC_PHY_SCLKDIV_13_1           = 3  /* for 13 MHz */
};
typedef SYS_UWORD8 BspUicc_Phy_SclkDiv;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_EtuPeriod
 * 
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for Elementary Time unit configuration paramater.
 */
enum
{
    BSP_UICC_PHY_ETU_PERIOD_8      = 0,  /* for 8 times 1/F sclk */
    BSP_UICC_PHY_ETU_PERIOD_372    = 1,
    BSP_UICC_PHY_ETU_PERIOD_512_8  = 2,
    BSP_UICC_PHY_ETU_PERIOD_512_16 = 3
};
typedef SYS_UWORD8 BspUicc_Phy_EtuPeriod;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_DataSize
 * 
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for Number of data units.
 */
typedef volatile SYS_UWORD16    BspUicc_Phy_DataSize;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_TxData
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for Data being transmitted/written.
 */
typedef volatile SYS_UWORD8     BspUicc_Phy_TxData;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_RxData
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for Data being received/read.
 *  Rx is bigger than a byte to accomodate a parity bit if needed.
 */
typedef volatile SYS_UWORD8    BspUicc_Phy_RxData;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_FifoData
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for Data in the Fifo
 */
typedef SYS_UWORD16 BspUicc_Phy_FifoData;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_TxFifoSize
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for size of the FIFO in the hardware.
 */
typedef SYS_UWORD16    BspUicc_Phy_TxFifoSize;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_RxFifoSize
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for size of the FIFO in the hardware.
 */
typedef SYS_UWORD16    BspUicc_Phy_RxFifoSize;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_Cgt
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for Character Guard Time.
 */
typedef SYS_UWORD16    BspUicc_Phy_Cgt;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_Cwt
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for Character Wait Time.
 */
typedef SYS_UWORD16    BspUicc_Phy_Cwt;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_Bwt
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for of Block Wait Time.
 */
typedef SYS_UWORD32    BspUicc_Phy_Bwt;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_FiValue
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for F value used in calculating ETU.
 */
typedef SYS_UWORD16    BspUicc_Phy_FiValue;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_DiValue
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for D value used in calculating ETU.
 */
typedef SYS_UWORD16    BspUicc_Phy_DiValue;

/*============================================================================= */
/*!
 * @typedef BspUicc_Phy_WaitI
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for WaitI configuration parameter
 */
typedef SYS_UWORD16    BspUicc_Phy_WaitI;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_Error
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type Physical layer errors
 */
enum
{
    BSP_UICC_PHY_NO_ERROR                   = ( 0 ),
    BSP_UICC_PHY_NATR_ERROR                 = ( -2 ),
    BSP_UICC_PHY_CHAR_UNDERFLOW_ERROR       = ( -3 ),
    BSP_UICC_PHY_CHAR_OVERFLOW_ERROR        = ( -4 ),
    BSP_UICC_PHY_CHAR_TIMEOUT_ERROR         = ( -5 ),
    BSP_UICC_PHY_BLOCK_TIMEOUT_ERROR        = ( -6 ),
    BSP_UICC_PHY_MAX_RESENT_ERROR           = ( -7 ),
    BSP_UICC_PHY_TS_DECODE_ERROR            = ( -8 ),
    BSP_UICC_PHY_NO_CARD_PRESENT_ERROR      = ( -9 ),
    BSP_UICC_PHY_READ_TIMEOUT_ERROR         = ( -10 )
};
typedef SYS_WORD16 BspUicc_Phy_Error;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_Convention
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for Physical layer data Code Convention
 */
enum
{
    BSP_UICC_PHY_CONVENTION_DIRECT          = 0,
    BSP_UICC_PHY_CONVENTION_INVERSE         = 1
};
typedef SYS_UWORD8 BspUicc_Phy_Convention;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_PowerLevel
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the card power level configuration
 * This is encoded as per section 6.5.6 in ISO/IEC 7816-3
 */
enum
{
    BSP_UICC_PHY_POWER_LEVEL_18             = 4,
    BSP_UICC_PHY_POWER_LEVEL_3_OR_5         = 3,
    BSP_UICC_PHY_POWER_LEVEL_3              = 2,
    BSP_UICC_PHY_POWER_LEVEL_5              = 1
};
typedef SYS_UWORD16 BspUicc_Phy_PowerLevel;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_BlockSize
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the size of a block
 */
typedef SYS_UWORD16 BspUicc_Phy_BlockSize;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Phy_ClkStopInd
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the Clock Stop Indicator in ATR
 */
typedef SYS_UWORD16 BspUicc_Phy_ClkStopInd;

/*============================================================================= */
/*!
 * @typedef BspUicc_Phy_ConfigType
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for WaitI configuration parameter
 */
enum
{
    BSP_UICC_PHY_CONFIG_TYPE_RESET          = 0,
    BSP_UICC_PHY_CONFIG_TYPE_READ_WRITE     = 1,
    BSP_UICC_PHY_CONFIG_TYPE_BWT_UPDATE     = 2,
    BSP_UICC_PHY_CONFIG_TYPE_CLKSTOP_UPDATE = 3     
};
typedef SYS_UWORD8  BspUicc_Phy_ConfigType;


/*============================================================================= */
/*!
 * @define
 *
 * @discussion
 * <b> Description  </b><br>
 *  The max Fifo size in the Usim hardware block. 
 */
#define BSP_UICC_PHY_MAX_FIFO_SIZE          16

/*============================================================================= */
/*!
 * @typedef BspUicc_Phy_Mode
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the ATR Mode, PPS Mode or READ_WRITE  Mode
 *  ReadWrite mode is used after a PPS i.e. for Commands.
 */
enum
{
    BSP_UICC_PHY_MODE_ATR            = 0,
    BSP_UICC_PHY_MODE_PPS            = 1, 
    BSP_UICC_PHY_MODE_READ_WRITE     = 2
};
typedef SYS_UWORD8  BspUicc_Phy_Mode;

/*============================================================================= */
/*!
 * @typedef  BspUicc_Phy_Direction
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the Tx or Rx Direction
 */
enum
{
    BSP_UICC_PHY_DIRECTION_RX        = 1,
    BSP_UICC_PHY_DIRECTION_TX        = 2
};
typedef SYS_UWORD8  BspUicc_Phy_Direction;

/*============================================================================= */
/*!
 * @typedef BspUicc_Phy_RxCtrl
 *
 * @discussion
 * <b> Description  </b><br>
 *  Control data for Receive mode
 */
typedef struct
{
    BspUicc_Phy_DataSize             rxIndex;
    BspUicc_Phy_DataSize             outIndex;
    BspUicc_Phy_DataSize             expectedData;
    BspUicc_Phy_DataSize             remainingData;
    BspUicc_Phy_RxData               rxBuffer[264];
}
BspUicc_Phy_RxCtrl;

/*============================================================================= */
/*!
 * @typedef BspUicc_Phy_TxCtrl
 *
 * @discussion
 * <b> Description  </b><br>
 *  Control data for Transmit mode
 */
typedef struct 
{
    BspUicc_Phy_DataSize             txIndex;
    BspUicc_Phy_DataSize             noTxBytes;
    BspUicc_Phy_TxData               txBuffer[264];
}
BspUicc_Phy_TxCtrl;


/*=============================================================================*/
/*!
 * @typedef BspUicc_ConfigReq
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for Configuration Characteristics requested by the Driver  
 */
enum
{
    BSP_UICC_PHY_OMIT_CONFIG_CHARACTERISTICS                 = 0x0,
    BSP_UICC_PHY_REQUEST_CONFIG_CHARACTERISTICS              = 0x1
};
typedef SYS_UWORD8 BspUicc_Phy_ConfigReq;

/*=============================================================================
 * Types
 * @discussion
 * This type specifies the clock stop prefernce of the MF
 */
typedef SYS_UWORD8  BspUicc_Phy_ClkStopPreference;


/*=============================================================================
 * Types
 * @discussion
 * This type specifies Configuration Characteristics delivered by PS
 */
typedef struct
{
     BspUicc_Phy_ClkStopPreference       clkStopPreference;
}BspUicc_Phy_ConfigChars; 

/*=============================================================================*/
/*!
 * @typedef BspUicc_AtrData
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the Atr Data
 */
typedef SYS_UWORD8 BspUicc_Phy_AtrData;

/*===========================================================================*/
/*!
 * @typedef BspUicc_Phy_CardAbsentDetectHandler
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the type used for a card removeddetect handler.
 *   It is a function pointer
 */
typedef void (*BspUicc_Phy_CardAbsentDetectHandler)( void );



/*============================================================================= */
/*!
 * @typedef BspUicc_Phy_Ctrl
 *
 * @discussion
 * <b> Description  </b><br>
 *  Generic phisical layer control data
 */
typedef struct 
{
    BspUicc_Phy_Protocol                   protocol;
    BspUicc_Phy_Mode                       mode;
    BspUicc_Phy_Direction                  dirn;
    BspUicc_Phy_Error                      error;
    BspUicc_Phy_SClkLev                    sClkLev;
    SYS_BOOL                                   parityCheck;
    BspUicc_Phy_CardAbsentDetectHandler    removeFunc;  
}
BspUicc_Phy_Ctrl;


/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_init
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function initializes the Usim hardware block.
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC physical layer State Machine
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param None
 *
 *
 */
void bspUicc_Phy_init( );

/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_init_CardDetectFuncs
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function  registers the handlers for the card insert and remove events.
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC physical layer State Machine
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param  insert
 *         Function pointer to the function to be called when a card is inserted
 *
 * @param  remove
 *         Function pointer to the function to be called when a card is removed
 *
 */
void bspUicc_Phy_init_CardDetectFuncs(  BspUicc_Phy_CardAbsentDetectHandler  *removeFunc  );

/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_reset
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function initiates a hardware warm reset of the Usim card.
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC physical layer State Machine
 *
 * <b> Returns </b><br>
 *   BspUicc_Phy_ReturnCode
 *   Function return status indicating sucess, failure or error code
 *
 * @param
 *   None
 *
 */
BspUicc_Phy_ReturnCode bspUicc_Phy_reset( void );

/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_configure
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function configures the Usim hardware block before any data transfer
 * occurs. Usim needs to be configured every time there is any change in any of the
 * configuration parameters.
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC physical layer State Machine
 *
 * <b> Returns </b><br>
 *   BspUicc_Phy_ReturnCode
 *   Function return status indicating sucess, failure or error code
 *
 * @param  eventDataPtr
 *   Pointer to the configReq structure.
 *
 */
BspUicc_Phy_ReturnCode bspUicc_Phy_configure( BspUicc_Phy_Protocol   protocol,
                                              BspUicc_Phy_SclkDiv    sclkDiv,
                                              BspUicc_Phy_EtuPeriod  etuPeriod,
                                              BspUicc_Phy_Cgt        cgt,
                                              BspUicc_Phy_Cwt        cwt,
                                              BspUicc_Phy_Bwt        bwt,
                                              BspUicc_Phy_FiValue    fiValue,
                                              BspUicc_Phy_DiValue    diValue,
                                              BspUicc_Phy_WaitI      waitIValue,
                                              BspUicc_Phy_SClkLev    sClkLev,
                                              BspUicc_Phy_ConfigType configType );
                                          
/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_write
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function writes or sends the data bytes specified, to the Usim
 * hardware block. The Hardware FIFO shall be used to send data efficiently.
 * This is done by setting the FIFO trigger level accordingly.
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC physical layer State Machine
 *
 * <b> Returns </b><br>
 *   BspUicc_Phy_ReturnCode
 *   Function return status indicating sucess with number of
 * bytes written, or failure
 *
 * @param  eventDataPtr
 *   Pointer to the writeReq structure.
 *
 */
BspUicc_Phy_ReturnCode bspUicc_Phy_write( BspUicc_Phy_DataSize   txDataSize,
                                          BspUicc_Phy_TxData     *dataPtr );

/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_read
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function reads the specified number of bytes from the Usim hardware block.
 *  The Hardware FIFO shall be used to send data efficiently. This is done by setting
 *  the FIFO trigger level accordingly.
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC physical layer State Machine
 *
 * <b> Returns </b><br>
 *   BspUicc_Phy_ReturnCode
 *   Function return status indicating sucess with number of
 * bytes read, or failure
 *
 * @param  eventDataPtr
 *   Pointer to the readReq structure.
 *
 */
BspUicc_Phy_ReturnCode bspUicc_Phy_read( BspUicc_Phy_DataSize   rxDataSize,
                                         BspUicc_Phy_RxData     *dataPtr );

/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_powerOff
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function powers off the Usim hardware block.
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC physical layer State Machine
 *
 * <b> Returns </b><br>
 *   BspUicc_Phy_ReturnCode
 *   Function return status indicating sucess, failure or error code
 *
 * @param  
 *  None
 */
BspUicc_Phy_ReturnCode bspUicc_Phy_powerOff( void );

/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_getCardInsertStatus
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function provides API to check if the Usim card has been inserted
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC physical layer 
 *
 * <b> Returns </b><br>
 *   BspUicc_Phy_ReturnCode
 *   Function return true status if card has been inserted
 *
 * @param  
 *  None
 */
BspUicc_Phy_ReturnCode bspUicc_Phy_getCardInsertStatus( void );

void  bspUicc_Phy_toggleCardInsertStatus( );

void bspUicc_Phy_sleepModeDisable( void );

void bspUicc_Phy_warmReset( void );


/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_getErrorStatus
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function gets the Error data 
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link layer 
 *
 * <b> Returns </b><br>
 *   BspUicc_Phy_Error
 *   Function returns the Error Status
 *
 * @param  
 *  None
 */
BspUicc_Phy_Error bspUicc_Phy_getErrorStatus( void );

/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_setMode
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function sets the Mode (ATR/PPS/READ-WRITE)
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link layer 
 *
 * <b> Returns </b><br>
 *   None
 *
 * @param mode
 *  Mode that needs to be set
 */
void  bspUicc_Phy_setMode( BspUicc_Phy_Mode mode  );


/*===========================================================================
 */
/*!
 * @function bspUicc_Phy_getMode
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function sets the Mode (ATR/PPS/READ-WRITE)
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link layer 
 *
 * <b> Returns </b><br>
 *    BspUicc_Phy_Mode
 *    Returns the current Mode setting
 *
 * @param 
 *  None
 */
BspUicc_Phy_Mode bspUicc_Phy_getMode( void );

/*=====================================================================
 */
/*!
 * @function bspUicc_Phy_setDir
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function sets the DIRECTION TO TX/RX
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link layer 
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param phyDirection
 * Direction can set set as RX or TX
 */
void  bspUicc_Phy_setDir( BspUicc_Phy_Direction phyDirection );

#endif
