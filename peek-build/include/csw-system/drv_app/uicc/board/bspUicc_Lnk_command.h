/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_LNK_COMMAND_HEADER
#define BSP_UICC_LNK_COMMAND_HEADER

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspUicc_Lnk_command.h
 *  This component has the macros and defnitions for all commands which are
 *  common to both T1 and T0 protocols
 */

#define BSP_UICC_LNK_COMMAND_HEADER_NUM_OCTETS            5

#define BSP_UICC_LNK_COMMAND_STATUS_BYTES_NUM_OCTETS      2

#define BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS               1

#define BSP_UICC_LNK_COMMAND_CHAR_NUM_OCTETS              1

#define BSP_UICC_LNK_COMMAND_APDU_SW1_NORMAL_VALUE        0x90

#define BSP_UICC_LNK_COMMAND_APDU_SW1_VALID_VALUE         0x60

#define BSP_UICC_LNK_COMMAND_APDU_SW2_NORMAL_VALUE        0x00

#define BSP_UICC_LNK_COMMAND_APDU_NULL_PROCEDURE_VALUE    0x60

#define BSP_UICC_READ_MAX_COMMAND_RESULT_VALUE            0

#define BSP_UICC_NO_DATA_WRITE_VALUE                      0

#define BSP_UICC_CMD_PARAMETER_NOT_CURRENTLY_USED         0

#define BSP_UICC_LNK_COMMAND_STATUS_CARD_NOT_INSERTED     0x0001

#define BSP_UICC_LNK_COMMAND_STATUS_UNRECOVERABLE_FAILURE 0x0003

#define BSP_UICC_LNK_CARD_NOT_PRESENT_STATUS              0x00



/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandClass
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the class for each command
 */
enum
{
    BSP_UICC_LNK_COMMAND_CLASS_GENERIC_GSM                  = 0xA0,
    BSP_UICC_LNK_COMMAND_CLASS_SELECT_FILE                  = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_STATUS                       = 0x80,
    BSP_UICC_LNK_COMMAND_CLASS_READ_BINARY                  = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_UPDATE_BINARY                = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_READ_RECORD                  = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_UPDATE_RECORD                = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_SEARCH_RECORD                = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_INCREASE                     = 0x80,
    BSP_UICC_LNK_COMMAND_CLASS_VERIFY_PIN                   = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_CHANGE_PIN                   = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_DISABLE_PIN                  = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_ENABLE_PIN                   = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_UNBLOCK_PIN                  = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_REHABILITATE                 = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_INVALIDATE                   = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_AUTHENTICATE                 = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_GET_CHALLENGE                = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_TERMINAL_PROFILE             = 0x80,
    BSP_UICC_LNK_COMMAND_CLASS_ENVELOPE                     = 0x80,
    BSP_UICC_LNK_COMMAND_CLASS_FETCH                        = 0x80,
    BSP_UICC_LNK_COMMAND_CLASS_TERMINAL_RESPONSE            = 0x80,
    BSP_UICC_LNK_COMMAND_CLASS_MANAGE_CHANNEL               = 0x00,
    BSP_UICC_LNK_COMMAND_CLASS_GET_RESPONSE                 = 0x00
};
typedef SYS_UWORD8 BspUicc_Lnk_CommandClass;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandInstruction
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the instruction for each command
 */
enum
{
    BSP_UICC_LNK_COMMAND_INSTRUCTION_SELECT_FILE            = 0xA4,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_STATUS                 = 0xF2,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_READ_BINARY            = 0xB0,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_UPDATE_BINARY          = 0xD6,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_READ_RECORD            = 0xB2,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_UPDATE_RECORD          = 0xDC,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_SEARCH_RECORD          = 0xA2,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_INCREASE               = 0x32,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_VERIFY_PIN             = 0x20,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_CHANGE_PIN             = 0x24,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_DISABLE_PIN            = 0x26,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_ENABLE_PIN             = 0x28,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_UNBLOCK_PIN            = 0x2C,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_REHABILITATE           = 0x04,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_INVALIDATE             = 0x44,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_AUTHENTICATE           = 0x88,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_GET_CHALLENGE          = 0x84,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_TERMINAL_PROFILE       = 0x10,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_ENVELOPE               = 0xC2,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_FETCH                  = 0x12,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_TERMINAL_RESPONSE      = 0x14,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_MANAGE_CHANNEL         = 0x70,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_GET_RESPONSE           = 0xC0,
    BSP_UICC_LNK_COMMAND_INSTRUCTION_RUN_GSM_ALGORITHM      = 0xC0
};
typedef SYS_UWORD8 BspUicc_Lnk_CommandInstruction;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandParameter1
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the parameter1 for a command
 */
typedef SYS_UWORD8 BspUicc_Lnk_CommandParameter1;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandParameter2
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the parameter2 for a command
 */
typedef SYS_UWORD8 BspUicc_Lnk_CommandParameter2;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandParameter3
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the parameter3 for a command
 */
typedef SYS_UWORD8 BspUicc_Lnk_CommandParameter3;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandParameter3
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the parameter3 for a command
 */
enum
{
    BSP_UICC_LNK_COMMAND_PARAM3_NOT_PRESENT                 = 0,
    BSP_UICC_LNK_COMMAND_PARAM3_PRESENT                     = 1
};
typedef SYS_UWORD8 BspUicc_Lnk_CommandParam3Flag;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandType
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the type of command
 */
enum
{
    BSP_UICC_LNK_COMMAND_TYPE_1                 = 1,
    BSP_UICC_LNK_COMMAND_TYPE_2                 = 2,
    BSP_UICC_LNK_COMMAND_TYPE_3                 = 3,
    BSP_UICC_LNK_COMMAND_TYPE_4                 = 4
};
typedef SYS_UWORD8 BspUicc_Lnk_CommandType;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandWriteDataSize
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the size of data to be sent with a command
 */
typedef SYS_UWORD8 BspUicc_Lnk_CommandWriteDataSize;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandReadDataSize
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the size of the command result data
 */
typedef SYS_UWORD16 BspUicc_Lnk_CommandReadDataSize;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandWriteData
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the type for command data
 */
typedef SYS_UWORD8 BspUicc_Lnk_CommandWriteData;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandReadData
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the type for command data
 */
typedef SYS_UWORD8 BspUicc_Lnk_CommandReadData;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandReadDataFlag
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies if any data needs to be read after sending the command
 */
enum
{
    BSP_UICC_LNK_COMMAND_READ_DATA_NOT_REQUIRED       = 0,
    BSP_UICC_LNK_COMMAND_READ_DATA_REQUIRED           = 1
};
typedef SYS_UWORD8 BspUicc_Lnk_CommandReadDataFlag;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_CommandReadMoreDataFlag
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies if any data needs to be read after sending the command
 */
enum
{
    BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED       = 0,
    BSP_UICC_LNK_COMMAND_MORE_DATA_REQUIRED           = 1
};
typedef SYS_UWORD8 BspUicc_Lnk_CommandReadMoreDataFlag;

#endif
