
/*=============================================================================
 *    Copyright 1996-2003 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_CMD_HEADER
#define BSP_UICC_CMD_HEADER

#include "sys_types.h"

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspUicc.h
 *  Header file with prototypes for Generic commands that can be sent to the
 *  UICC. These functions will be made accessable to the application layer.
 *  Protocol stack and application layer mean the same in this context, and
 *  are used interchangeably.
 */


#define BSP_UICC_CMD_MIN_DATA_SIZE                     0x01

#define BSP_UICC_CMD_MAX_DATA_SIZE                     0xFF

#define BSP_UICC_CMD_MIN_RESULT_SIZE                   0x01

#define BSP_UICC_CMD_MAX_RESULT_SIZE                   0x100

#define BSP_UICC_CMD_UNKNOWN_RESULT_SIZE               0xFFFF 

#define BSP_UICC_MAX_READERS                           0x02

#define BSP_UICC_MAX_ATR_DATA_SIZE                     0x21

#define BSP_UICC_CLOCK_STOP_MASK		0x0D

/*=============================================================================*/
/*!
 * @typedef BspUicc_CmdResult
 *
 * @discussion
 * <b> Description  </b><br>
 * The Result of the command has the status bytes returned by the card.
 */
enum
{
    BSP_UICC_CMD_STATUS_ERR_NO_CARD              =  0x0001,
    BSP_UICC_CMD_STATUS_ERR_NOT_RESET            =  0x0002,
    BSP_UICC_CMD_STATUS_ERR_ME_FAIL              =  0x0003,
    BSP_UICC_CMD_STATUS_ERR_RETRY_FAIL           =  0x0004,
    BSP_UICC_CMD_STATUS_ERR_PARAM_WRONG          =  0x0005
};
typedef SYS_UWORD16 BspUicc_CmdResult;

/*===========================================================================*/
/*!
 * @typedef BspUicc_CmdClass
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the class for each generic commad
 */
enum
{
    BSP_UICC_GSM_CLASS_BYTE                      = 0xA0,
    BSP_UICC_UMTS_CLASS_BYTE                     = 0x80,
    BSP_UICC_UICC_CLASS_BYTE                     = 0x00
};
typedef SYS_UWORD8 BspUicc_CmdClass;

/*============================================================================*/
/*!
 * @typedef BspUicc_PowerLevel
 *
 * @discussion
 * <b> Description  </b><br>
 *     Specifies the type for the card power level configuration
 *     This is encoded as per section 6.5.6 in ISO/IEC 7816-3
 */
enum
{
    BSP_UICC_POWER_LEVEL_18_3           = 6,
    BSP_UICC_POWER_LEVEL_18             = 4,
    BSP_UICC_POWER_LEVEL_3_OR_5         = 3,
    BSP_UICC_POWER_LEVEL_3              = 2,
    BSP_UICC_POWER_LEVEL_5              = 1
};
typedef SYS_UWORD16  BspUicc_PowerLevel;



/*=============================================================================*/
/*!
 * @typedef BspUicc_CmdInstruction
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the instruction for each command
 */
typedef SYS_UWORD8 BspUicc_CmdInstruction;

/*=============================================================================*/
/*!
 *  @typedef BspUicc_CmdParameter1
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the first parameter for a command
 */
typedef SYS_UWORD8 BspUicc_CmdParameter1;

/*=============================================================================*/
/*!
 *  @typedef BspUicc_CmdParameter2
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the second parameter for a command
 */
typedef SYS_UWORD8 BspUicc_CmdParameter2;

/*=============================================================================*/
/*!
 *  @typedef BspUicc_CmdHeader
 * 
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the Command header.
 */
typedef struct
{
    BspUicc_CmdClass         cmdClass;
    BspUicc_CmdInstruction   cmdInstruction;
    BspUicc_CmdParameter1    cmdParameter1;
    BspUicc_CmdParameter2    cmdParameter2;
}BspUicc_CmdHeader;    


/*=============================================================================*/
/*!
 * @typedef BspUicc_CmdWriteData
 * 
 * @discussion
 * <b> Description  </b><br>
 * Specifies the type for the data to be sent to the USIM
 * when issuing a command  
 */
typedef SYS_UWORD8 BspUicc_CmdWriteData;


/*=============================================================================*/
/*!
 * @typedef BspUicc_CmdWriteDataSize
 *  
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for size of the data to be sent to the USIM
 *  when issuing a command  
 */
typedef SYS_UWORD8 BspUicc_CmdWriteDataSize;


/*=============================================================================*/
/*!
 * @typedef BspUicc_CmdWriteDataInfo
 * 
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the Send Data Buffer
 */
typedef struct
{
     BspUicc_CmdWriteData       *cmdWriteDataPtr;
     BspUicc_CmdWriteDataSize    cmdWriteDataSize;
}BspUicc_CmdWriteDataInfo;


/*=============================================================================*/
/*!
 *  @typedef BspUicc_CmdReadData
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the response data, or data to be received
 *  when issuing a command to the USIM
 */
typedef SYS_UWORD8 BspUicc_CmdReadData;


/*=============================================================================*/
/*!
 *  @typedef BspUicc_CmdReadDataSize
 *
 * @discussion
 * <b> Description  </b><br>
 * Specifies the type for size of the response data, or data to be received
 *  when issuing a command to the USIM
 */
typedef SYS_UWORD16 BspUicc_CmdReadDataSize;

/*=============================================================================*/
/*!
 *  @typedef BspUicc_CmdResultSize
 *
 * @discussion
 * <b> Description  </b><br>
 * Specifies the type for size of data actually received for the command
 */
typedef SYS_UWORD16 BspUicc_CmdResultSize;


/*=============================================================================*/
/*!
 *  @typedef BspUicc_CmdReadDataInfo
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the result buffer.
 */
typedef struct
{
     BspUicc_CmdReadData       *cmdReadDataPtr;
     BspUicc_CmdReadDataSize    cmdReadDataSize;
     BspUicc_CmdResultSize     *cmdResultSizePtr;
}BspUicc_CmdReadDataInfo;


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
    BSP_UICC_OMIT_CONFIG_CHARACTERISTICS            = 0x0,
    BSP_UICC_REQUEST_CONFIG_CHARACTERISTICS         = 0x1
    };
typedef SYS_UWORD8 BspUicc_ConfigReq;

/*=============================================================================*/
/*!
 *  @typedef BspUicc_ClkStopPreference
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the clock stop preferance of the MF
 */
typedef SYS_UWORD8  BspUicc_ClkStopPreference;

/*============================================================================*/
/*!
 * @typedef BspUicc_voltClassPreference
 *
 * @discussion
 * <b> Description  </b><br>
 *     This type specifies the voltage class preference of the MF.
 */
typedef SYS_UWORD8  BspUicc_VoltClassPreference;

/*=============================================================================*/
/*!
 * @typedef BspUicc_SClockLevel
 * 
 * @discussion
 * <b> Description  </b><br>
 *   Specifies the type for Clock stop parameter used to enable/disable sleep mode
 */
enum
{
    BSP_UICC_STOP_CLOCK_NO_STOP    = 0, /* Clock stop is not Supported */
    BSP_UICC_STOP_CLOCK_LEVEL_LOW  = 1,
    BSP_UICC_STOP_CLOCK_LEVEL_HIGH = 2
};
typedef SYS_UWORD8 BspUicc_SClkLev;


/*=============================================================================*/
/*!
 * @typedef BspUicc_ClockStopSuported
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type to indicate if clock stop is supported
 */
enum
{
    BSP_UICC_CLOCK_STOP_NOT_SUPPORTED = 0x00,
    BSP_UICC_CLOCK_STOP_SUPPORTED_ANY = 0x01,
    BSP_UICC_CLOCK_STOP_SUPPORTED_HIGHONLY = 0x04,
    BSP_UICC_CLOCK_STOP_SUPPORTED_HIGH = 0x05,
    BSP_UICC_CLOCK_STOP_SUPPORTED_LOWONLY  = 0x08,
    BSP_UICC_CLOCK_STOP_SUPPORTED_LOW = 0x09
    
};
typedef SYS_UWORD8 BspUicc_ClockStopSuported;


/*=============================================================================*/
/*!
 *  @typedef BspUicc_ConfigChars
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies Configuration Characteristics delivered by PS
 */
typedef struct
{
     BspUicc_ClkStopPreference       clkStopPreference;
}BspUicc_ConfigChars;    

/*=============================================================================*/
/*!
 *  @typedef BspUicc_AtrSize
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the size of the ATR data rturned by the card
 */
typedef SYS_UWORD8  BspUicc_AtrSize;
/*=============================================================================*/
/*!
 *  @typedef BspUicc_AtrData
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies ATR data returned by the card
 */
typedef SYS_UWORD8  BspUicc_AtrData;

/*=============================================================================*/
/*!
 * @typedef BspUicc_AtrDataInfo
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the Atr Data
 */
typedef struct
{
     BspUicc_AtrSize   atrSize;
     BspUicc_AtrData  *atrDataPtr;
}BspUicc_AtrDataInfo;


/*===========================================================================*/
/*!
 * @typedef BspUicc_CardAbsentDetectHandler
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the type used for a card detect handler if the card is absent
 */
typedef void (*BspUicc_CardAbsentDetectHandler)( void );

/*===========================================================================*/
/*!
 * @typedef BspUicc_CardPresentDetectHandler
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the type used for a card detect handler if the card is detected
 */
typedef void (*BspUicc_CardPresentDetectHandler)
                          ( BspUicc_AtrDataInfo     *atrData,
                            BspUicc_ConfigReq       configReq,
                            BspUicc_ConfigChars     *configChars);

/*=============================================================================*/
/*!
 * @typedef BspUicc_ResetResult
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the reseult type for the Reset event
 */
enum
{
    BSP_UICC_RETURN_CODE_CARD_INSERTED         = 0,
    BSP_UICC_RETURN_CODE_CARD_NOT_INSERTED     = 1,
    BSP_UICC_RETURN_CODE_INVALID_CARD          = 2,
    BSP_UICC_RETURN_CODE_ME_FAILURE            = 3
};
typedef SYS_UWORD8 BspUicc_ResetResult;



/*=============================================================================*/
/*!
 * @typedef BspUicc_ReaderId
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the redaer id
 */
typedef SYS_UWORD8 BspUicc_ReaderId;


/*=============================================================================*/
/*!
 * @typedef BspUicc_VoltageSelect
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the Voltage Selection
 */
enum
{
    BSP_UICC_VOLTAGE_SELECT_REQUIRED              = 0x0,
    BSP_UICC_VOLTAGE_SELECT_OMIT                  = 0x1
};
typedef SYS_UWORD8 BspUicc_VoltageSelect;

/*===========================================================================*/
/*!
 * @function bspUicc_driver_register
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function is called to establish communication with the card
 *   driver.
 *
 * <b> Context </b><br>
 *    Called from Protocol Stack
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param  insert
 *         Pointer to the function to be called when the card is inserted.
 *
 * @param  remove
 *         Pointer to the function to be called when the card is removed.
 *
 *
 */
void bspUicc_drvRegister( BspUicc_CardPresentDetectHandler     insert,
                          BspUicc_CardAbsentDetectHandler      remove );


/*===========================================================================*/
/*!
 * @function bspUicc_xchApdu
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function is called to exchange APDU commands 
 *   driver.
 *
 * <b> Context </b><br>
 *    Called from Protocol Stack
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param  readerId
 *         Reader id for which the command is issued
 *
 * @param  cmdHeader
 *         Indicates the transparent command Header
 *
 * @param  cmdWriteDataInfo
 *         Data sent to the USIM when issuing a command
 *
 * @param  cmdReadDataInfo
 *         Response data retrieved from USIM when issuing a command

 */
BspUicc_CmdResult  bspUicc_xchApdu( BspUicc_ReaderId           cmdReaderId,
                                    BspUicc_CmdHeader          cmdHeader,
                                    BspUicc_CmdWriteDataInfo   cmdWriteDataInfo,
                                    BspUicc_CmdReadDataInfo    cmdReadDataInfo
                                  );



/*===========================================================================*/
/*!
 * @function bspUicc_reset
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function is called to initialize communication between the SIM entity
 * and the SIM driver.
 *
 * <b> Context </b><br>
 *    Called from Protocol Stack
 *
 * <b> Returns </b><br>
 *    BspUicc_ResetResult
 *
 * @param  readerId
 *         Reader id for which the reset command is issued
 *
 * @param  voltageSelect
 *         Indicates whether a voltage selection needs to be performed
 *
 */
BspUicc_ResetResult bspUicc_reset( BspUicc_ReaderId readerId,
                                   BspUicc_VoltageSelect voltageSelect );


/*===========================================================================*/
/*!
 * @function bspUicc_powerOff
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function is called to deactivate the UICC
 *
 * <b> Context </b><br>
 *    Called from Protocol Stack
 *
 * @param readerId
 *        Reader id for which the PowerOff  command is issued
 *    
 */
void bspUicc_powerOff( BspUicc_ReaderId readerId );


/*===========================================================================*/
/*!
 * @function bspUicc_bootInit
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function sets up the Uicc interrupt handlers
 *
 * <b> Context </b><br>
 *    Called at system startup
 *
 * @param
 *    none
 */
void bspUicc_bootInit();

#endif




