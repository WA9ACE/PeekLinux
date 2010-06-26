/*******************************************************************************
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
*******************************************************************************/
/*******************************************************************************
*
*   FILE NAME:      bthal_pragmas.h
*
*   BRIEF:          BTIPS Hardware Adaptation Layer Pragma Directive Definitions.
*
*   DESCRIPTION:    
*
*     The pragma directives in this file configure the BTHAL layer for a specific 
*     platform and project.
*
*     The pragma directives offer a way for each compiler to offer machine- and 
*     operating-system-specific features while retaining overall compatibility with 
*     the C language.
*
*     The pragma directives will place the defined function or variables in an 
*     internal RAM for faster execution.
*
*	The values in this specific file are tailored for a Windows distribution. To add a 
*	pragma directive, simply add it in the corresponding section.
*
*   AUTHOR:         Ronen Levy
*
*****************************************************************************/

#ifndef __BTHAL_PRAGMAS_H
#define __BTHAL_PRAGMAS_H


#ifdef WIN_WARNING_PRAGMAS
/*-------------------------------------------------------------------------------
 * WARNINGS HANDLING
 *
 *	Disable the "Conditional Expression is Constant" warning that is to be ignored
 */
#pragma warning( disable :  4127) 
#undef WIN_WARNING_PRAGMAS
#endif


#ifdef BTL_A2DP_PRAGMAS
/*-------------------------------------------------------------------------------
 * BTL_A2DP.c 
 *
 *     Represents pragma directives in btl_a2dp.c.
 *     These pragmas will place the function in internal RAM for faster execution 
 */
#pragma DATA_SECTION(sbcEncoder,".l1s_global")

/* #pragma CODE_SECTION(btlA2dpLimitBitPoolToSbcLen, ".emifconf") */

/* #pragma CODE_SECTION(btlA2dpProcessPcmBlock, ".emifconf") */

/* #pragma CODE_SECTION(btlA2dpInsertSbcPacketToQueue, ".emifconf") */

/* #pragma CODE_SECTION(btlA2dpGetSbcPacketFromPool, ".emifconf") */

/* #pragma CODE_SECTION(btlA2dpInsertSbcPacketToPool, ".emifconf") */

/* #pragma CODE_SECTION(btlA2dpAdjustBitPool, ".emifconf") */

#undef BTL_A2DP_PRAGMAS
#endif

#ifdef BTL_SRC_PRAGMAS
/*-------------------------------------------------------------------------------
 * sample_rate_converter.c 
 *
 *     Represents pragma directives in sample_rate_converter.c.
 *     These pragmas will place the function and global data in internal RAM for faster execution 
 */
#pragma DATA_SECTION(coef,			".l1s_global")
#pragma DATA_SECTION(oldSamples,	".l1s_global")
#pragma DATA_SECTION(SRC_leap,		".l1s_global")

#pragma CODE_SECTION(filter12_sterio,	".emifconf")
#pragma CODE_SECTION(filter12_mono,	".emifconf")

#undef BTL_SRC_PRAGMAS
#endif

#ifdef UTILS_PRAGMAS
/*-------------------------------------------------------------------------------
 * Utils.c 
 *
 *     Represents pragma directives in utils.c.
 *     These pragmas will place the function in internal RAM for faster execution 
 */

#pragma CODE_SECTION(IsListCircular, ".emifconf")

#undef UTILS_PRAGMAS
#endif


#ifdef BTHAL_UART_PRAGMAS
/*-------------------------------------------------------------------------------
 * BTHAL_UART.c 
 *
 *     Represents pragma directives in bthal_uart.c.
 *     These pragmas will place the function in internal RAM for faster execution 
 */

#pragma CODE_SECTION(BTHAL_UART_Read, ".emifconf") 

#pragma CODE_SECTION(BTHAL_UART_Write, ".emifconf") 

#pragma CODE_SECTION(BTHAL_UART_EventThread, ".emifconf")

#undef BTHAL_UART_PRAGMAS
#endif

#ifdef BTHAL_OS_PRAGMAS
/*-------------------------------------------------------------------------------
 * BTHAL_OS.c 
 *
 *     Represents pragma directives in bthal_os.c.
 *     These pragmas will place the function in internal RAM for faster execution 
 */

#pragma CODE_SECTION(OsLockSemaphore, ".emifconf")

#pragma CODE_SECTION(OsUnlockSemaphore, ".emifconf")

#undef BTHAL_OS_PRAGMAS
#endif

#ifdef BTHAL_UTILS_PRAGMAS
/*-------------------------------------------------------------------------------
 * BTHAL_Utils.c 
 *
 *     Represents pragma directives in bthal_utils.c.
 *     These pragmas will place the function in internal RAM for faster execution 
 */

#pragma CODE_SECTION(BTHAL_UTILS_MemCopy, ".emifconf")

#undef BTHAL_UTILS_PRAGMAS
#endif


#ifdef UARTTRAN_PRAGMAS
/*-------------------------------------------------------------------------------
 * Uarttran.c 
 *
 *     Represents pragma directives in uarttran.c.
 *     These pragmas will place the function in internal RAM for faster execution 
 */

#pragma CODE_SECTION(UARTTRAN_SendData,".emifconf") 

#pragma CODE_SECTION(UARTTRAN_BufferAvailable,".emifconf")

#pragma CODE_SECTION(uartEventHandler,".emifconf")

#pragma CODE_SECTION(uartTransmitData,".emifconf") 

#pragma CODE_SECTION(uartReadData,".emifconf") 

#undef UARTTRAN_PRAGMAS
#endif


#endif /* __BTHAL_PRAGMAS_H */






