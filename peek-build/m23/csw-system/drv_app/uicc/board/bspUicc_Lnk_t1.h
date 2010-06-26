/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_LNK_T1_HEADER
#define BSP_UICC_LNK_T1_HEADER

#include "bspUicc_Phy.h"

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspUicc_Lnk_t1.h
 *  This component manages the communication with the card for the T1 protocol
 */


/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_T1ReturnCode
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the return code/status. 
 */
enum
{
    BSP_UICC_LNK_T1_RETURN_CODE_SUCCESS              = ( 0 ),
    BSP_UICC_LNK_T1_RETURN_CODE_FAILURE              = ( -1 ),
    BSP_UICC_LNK_T1_RETURN_CODE_NATR_ERROR           = BSP_UICC_PHY_NATR_ERROR,
    BSP_UICC_LNK_T1_RETURN_CODE_CHAR_UNDERFLOW_ERROR = BSP_UICC_PHY_CHAR_UNDERFLOW_ERROR,
    BSP_UICC_LNK_T1_RETURN_CODE_CHAR_OVERFLOW_ERROR  = BSP_UICC_PHY_CHAR_OVERFLOW_ERROR,
    BSP_UICC_LNK_T1_RETURN_CODE_CHAR_TIMEOUT_ERROR   = BSP_UICC_PHY_CHAR_TIMEOUT_ERROR,
    BSP_UICC_LNK_T1_RETURN_CODE_BLOCK_TIMEOUT_ERROR  = BSP_UICC_PHY_BLOCK_TIMEOUT_ERROR,
    BSP_UICC_LNK_T1_RETURN_CODE_MAX_RESENT_ERROR     = BSP_UICC_PHY_MAX_RESENT_ERROR,
    BSP_UICC_LNK_T1_RETURN_CODE_TS_DECODE_ERROR      = BSP_UICC_PHY_TS_DECODE_ERROR,
    BSP_UICC_LNK_T1_RETURN_CODE_EDC_ERROR            = ( -9 ),
    BSP_UICC_LNK_T1_RETURN_CODE_PCB_INVALID          = ( -10 ),
    BSP_UICC_LNK_T1_RETURN_CODE_LENGTH_INVALID       = ( -11 ),
    BSP_UICC_LNK_T1_RETURN_CODE_BWT_TIMEOUT          = ( -12 ),
    BSP_UICC_LNK_T1_RETURN_CODE_RESYNCH_FAIL         = ( -13 ),
    BSP_UICC_LNK_T1_RETURN_CODE_NAD_INVALID          = ( -14 )
   
};
typedef SYS_WORD32 BspUicc_Lnk_T1ReturnCode;


/*============================================================================*/
/*!
 * @function bspUicc_Lnk_t1WriteCommand
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function Sends the command and returns the command result
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer State machine
 *
 * <b> Returns </b><br>
 *    BspUicc_Lnk_T1ReturnCode
 *
 * @param  class
 *    Class of the command
 *
 * @param  instruction
 *    Instruction of the command
 *
 * @param  parameter1
 *    Parameter1 of the command
 *
 * @param  parameter2
 *    Parameter2 of the command
 *
 * @param  writeDataSize
 *    Size of command data to be sent
 *
 * @param  writeDataPtr
 *    Pointer to the Command data to be sent
 *
 * @param  readDataSize
 *    Size of command data to be read, if 0 read all that the card sends
 *
 * @param  readDataPtr
 *    Pointer to the Command data to be read
 *
 * @param  commandType
 *    Indicates the type of command
 * 
 */
BspUicc_Lnk_T1ReturnCode bspUicc_Lnk_t1WriteCommand( BspUicc_Lnk_CommandClass          class,
                                                     BspUicc_Lnk_CommandInstruction    instruction,
                                                     BspUicc_Lnk_CommandParameter1     parameter1,
                                                     BspUicc_Lnk_CommandParameter2     parameter2,
                                                     BspUicc_Lnk_CommandWriteDataSize  writeDataSize,
                                                     BspUicc_Lnk_CommandWriteData      *writeDataPtr,
                                                     BspUicc_Lnk_CommandReadDataSize   readDataSize,
                                                     BspUicc_Lnk_CommandReadData       *readDataPtr,
                                                     BspUicc_Lnk_CommandReadDataSize   *resultSizePtr,
                                                     BspUicc_Lnk_CommandType           commandType );

#endif
