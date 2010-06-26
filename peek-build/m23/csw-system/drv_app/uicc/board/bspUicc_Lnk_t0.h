/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_LNK_T0_HEADER
#define BSP_UICC_LNK_T0_HEADER

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspUicc_Lnk_t0.h
 *  This component manages the communication with the card for the t0 protocol
 */

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_T0ReturnCode
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the return code/status. 
 */
enum
{
    BSP_UICC_LNK_T0_RETURN_CODE_FAILURE       = (-10),
    BSP_UICC_LNK_T0_RETURN_CODE_SUCCESS       = (0)
};
typedef SYS_WORD32 BspUicc_Lnk_T0ReturnCode;



/*============================================================================*/
/*!
 * @function bspUicc_Lnk_t0WriteCommand
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function Sends the command and returns the command result
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer State machine
 *
 * <b> Returns </b><br>
 *    BspUicc_Lnk_T0ReturnCode
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
 *    Indicates the type of command.
 * 
 */
BspUicc_Lnk_T0ReturnCode bspUicc_Lnk_t0WriteCommand( BspUicc_Lnk_CommandClass           class,
                                                     BspUicc_Lnk_CommandInstruction     instruction,
                                                     BspUicc_Lnk_CommandParameter1      parameter1,
                                                     BspUicc_Lnk_CommandParameter2      parameter2,
                                                     BspUicc_Lnk_CommandWriteDataSize   writeDataSize,
                                                     BspUicc_Lnk_CommandWriteData       *writeDataPtr,
                                                     BspUicc_Lnk_CommandReadDataSize    readDataSize,
                                                     BspUicc_Lnk_CommandReadData        *readDataPtr,
                                                     BspUicc_Lnk_CommandReadDataSize    *resultSizePtr,
                                                     BspUicc_Lnk_CommandType            commandType );

#endif
