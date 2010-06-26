/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_LNK_HEADER
#define BSP_UICC_LNK_HEADER

#include "bspUicc_Lnk_command.h"

/*============================================================================
 * Component Description:
 *    The Link layer component of the BSP_UICC device driver
 */
/*!  
 * @header bspUicc_Lnk.h
 *  Header file for the BSP_UICC Link layer. Link layer is between the the top layer
 * of the BSP_UICC driver and the Physical layer. 
 */

#define BSP_UICC_LNK_CARD_NOT_PRESENT_ERROR                  (-9)

#define BSP_UICC_LNK_CARD_NOT_PRESENT_STATUS                 0x00
/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_ReturnCode
 * 
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the return code
 */
enum
{
    BSP_UICC_LNK_RETURN_CODE_FAILURE          = (-1),
    BSP_UICC_LNK_RETURN_CODE_SUCCESS          = (0),
    BSP_UICC_LNK_RETURN_CODE_ERR_NO_CARD      = (1),
    BSP_UICC_LNK_RETURN_CODE_ERR_ME_FAIL      = (3)
};
typedef SYS_WORD16 BspUicc_Lnk_ReturnCode;


/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_Reset_ReturnCode
 * 
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the return code for Reset for Link Layer.
 */
enum
{
    BSP_UICC_LNK_RETURN_CARD_INSERTED         = 0,
    BSP_UICC_LNK_RETURN_CARD_NOT_INSERTED     = 1,
    BSP_UICC_LNK_RETURN_INVALID_CARD          = 2,
    BSP_UICC_LNK_RETURN_ME_FAILURE            = 3
};
typedef SYS_UWORD8 BspUicc_Lnk_Reset_ReturnCode;



/*============================================================================= */
/*!
 * @typedef BspUicc_Lnk_ResetData
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the ATR characters
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetData;

/*============================================================================= */
/*!
 * @typedef BspUicc_Lnk_ResetDataSize
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the number of ATR characters
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetDataSize;


/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_ReaderId
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for Reader Id
 */
typedef SYS_UWORD8 BspUicc_Lnk_ReaderId;


/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_VoltClass
 *
 * @discussion
 * <b> Description  </b><br>
 *     Specifies the type for for the voltage Class.
 */
typedef Uint8  BspUicc_Lnk_VoltClass;


/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_ResetNewVoltClass
 *
 * @discussion
 * <b> Description  </b><br>
 *     Specifies the type for indicatiing if the reset request is due to the card
 *     not having the uicc confiuration characterics byte in the initial reset sequence.
 */
typedef Bool BspUicc_Lnk_ResetNewVoltClass;

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
    BSP_UICC_LNK_VOLTAGE_SELECT_REQUIRED              = 0x0,
    BSP_UICC_LNK_VOLTAGE_SELECT_OMIT                  = 0x1
};
typedef SYS_UWORD8 BspUicc_Lnk_VoltageSelect;

/*===========================================================================*/
/*!
 * @typedef BspUicc_Lnk_CardAbsentDetectHandler
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the type used for a card detect handler if the card is absent
 */
typedef void (*BspUicc_Lnk_CardAbsentDetectHandler)( void );


/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_InitReq
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the Initialize event
 */
typedef struct
{
    BspUicc_Lnk_CardAbsentDetectHandler removeFunc;
}BspUicc_Lnk_InitReq;



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
    BSP_UICC_LNK_OMIT_CONFIG_CHARACTERISTICS                 = 0x0,
    BSP_UICC_LNK_REQUEST_CONFIG_CHARACTERISTICS              = 0x1
    };
typedef SYS_UWORD8 BspUicc_Lnk_ConfigReq;

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
    BSP_UICC_Lnk_CLOCK_STOP_NOT_SUPPORTED                    = 0x00
};
typedef SYS_UWORD8 BspUicc_Lnk_ClockStopSuported;


/*=============================================================================*/
/*!
 *  @typedef BspUicc_ClkStopPreference
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the clock stop prefernce of the MF
 */
typedef SYS_UWORD8  BspUicc_Lnk_ClkStopPreference;
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
     BspUicc_Lnk_ClkStopPreference       clkStopPreference;
}BspUicc_Lnk_ConfigChars; 

/*=============================================================================*/
/*!
 *  @typedef BspUicc_AtrSize
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the size of the ATR data rturned by the card
 */
typedef SYS_UWORD8  BspUicc_Lnk_AtrSize;
/*=============================================================================*/
/*!
 *  @typedef BspUicc_AtrData
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies ATR data returned by the card
 */
typedef SYS_UWORD8  BspUicc_Lnk_AtrData;

/*=============================================================================*/
/*!
 * @typedef BspUicc_AtrData
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the Atr Data
 */
typedef struct
{
     BspUicc_Lnk_AtrSize   atrSize;
     BspUicc_Lnk_AtrData  *atrDataPtr;
    
}BspUicc_Lnk_AtrDataInfo;


/*===========================================================================*/
/*!
 * @typedef BspUicc_Lnk_CardPresentDetectHandler
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the type used for a card detect handler if the card is detected
 */
typedef void (*BspUicc_Lnk_CardPresentDetectHandler)
                          ( BspUicc_Lnk_AtrDataInfo  *atrData,
                            BspUicc_Lnk_ConfigReq     configReq,
                            BspUicc_Lnk_ConfigChars  *configChars);

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_ResetReq
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the Reset request event
 */
typedef struct
{
 BspUicc_Lnk_ReaderId                         readerId;
 BspUicc_Lnk_VoltageSelect                    voltageSelect; 
 BspUicc_Lnk_ResetData                        *resetDataPtr;
 BspUicc_Lnk_ResetDataSize                    *resetDataSizePtr;
 BspUicc_Lnk_ResetNewVoltClass                resetNewVoltClass;
 BspUicc_Lnk_VoltClass                        newVoltClass;
}BspUicc_Lnk_ResetReq;


/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_SendCommandReq
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the Reset request event
 */
typedef struct
{
    BspUicc_Lnk_CommandClass           class;
    BspUicc_Lnk_CommandInstruction     instruction;
    BspUicc_Lnk_CommandParameter1      parameter1;
    BspUicc_Lnk_CommandParameter2      parameter2;
    BspUicc_Lnk_CommandWriteDataSize   writeDataSize;
    BspUicc_Lnk_CommandWriteData       *writeDataPtr;
    BspUicc_Lnk_CommandReadDataSize    readDataSize;
    BspUicc_Lnk_CommandReadData        *readDataPtr;
    BspUicc_Lnk_CommandReadDataSize    *resultSizePtr;
    BspUicc_Lnk_CommandType            commandType;
}BspUicc_Lnk_SendCommandReq;

/*=============================================================================*/
/*!
 * @function bspUicc_Lnk_init
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function initializes the Link layer at bootup.
 *
 * <b> Context </b><br>
 *    Called from boot code
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param
 *    None
 */
void bspUicc_Lnk_init( void );

/*=============================================================================*/
/*!
 * @function bspUicc_Lnk_initReq
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function initializes the Link layer on the init event occurence
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC top layer
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param
 *    None
 */
BspUicc_Lnk_ReturnCode bspUicc_Lnk_initReq( BspUicc_Lnk_InitReq *eventDataPtr );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_resetReq
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function sends a Link layer reset request event and
 *   starts link layer ATR processing
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer State machine
 *
 * <b> Returns </b><br>
 *    BspUicc_Lnk_ReturnCode
 *
 * @param
 *    eventDataPtr
 *    Pointer to the write request event data structure
 *
 */
BspUicc_Lnk_ReturnCode bspUicc_Lnk_resetReq( BspUicc_Lnk_ResetReq *eventDataPtr );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_sendCommandReq
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function is called to send any command
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer State machine
 *
 * <b> Returns </b><br>
 *    BspUicc_Lnk_ReturnCode
 *
 * @param
 *    eventDataPtr
 *    Pointer to the write request event data structure
 *
 */
BspUicc_Lnk_ReturnCode bspUicc_Lnk_sendCommandReq( BspUicc_Lnk_SendCommandReq *eventDataPtr );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_PowerOffReq
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function dispatches the powerOff event to the Link layer
 *   state machine
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer
 *
 * <b> Returns </b><br>
 *    BspUicc_Lnk_ReturnCode
 *
 * @param
 *    None
 *
 */
BspUicc_Lnk_ReturnCode bspUicc_Lnk_powerOffReq( void );

#endif

