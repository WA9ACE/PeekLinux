/*============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_LNK_MSG_UTIL_HEADER
#define BSP_UICC_LNK_MSG_UTIL_HEADER

#include "bspUtil_BitUtil.h"

/*=============================================================================
 * Component Description:
 *   Defines coponents and macros for the messages handled by BSP_UICC Link layer
 */

/*=============================================================================
 * Constants
 */

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Initial Character- TS
-----------------------------------------------------------------
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                            TS Character                       |
|                                                               |
-----------------------------------------------------------------
*/

#define BSP_UICC_LNK_MSG_UTIL_TS_OFFSET                             0
#define BSP_UICC_LNK_MSG_UTIL_TS_WIDTH                              8
#define BSP_UICC_LNK_MSG_UTIL_TS_NUM_OCTETS                         1

#define BSP_UICC_LNK_MSG_UTIL_TS_CONVENTION_OCTET_OFFSET            0
#define BSP_UICC_LNK_MSG_UTIL_TS_CONVENTION_BIT_OFFSET              0
#define BSP_UICC_LNK_MSG_UTIL_TS_CONVENTION_BIT_WIDTH               8

#define BSP_UICC_LNK_MSG_UTIL_TS_DIRECT_CONVENTION_VALUE            0x3B
#define BSP_UICC_LNK_MSG_UTIL_TS_INVERSE_CONVENTION_VALUE           0x03
#define BSP_UICC_LNK_MSG_UTIL_TS_INVERSE_INTERPRETED_VALUE           0x3F

/*
#define BSP_UICC_LNK_MSG_UTIL_TS_GET_CONVENTION( _msgPtr )                       \
(   PUF_BITUTIL_BIT_FIELD_GET( (_msgPtr),                                        \
                               BSP_UICC_LNK_MSG_UTIL_TS_CONVENTION_OCTET_OFFSET, \
                               BSP_UICC_LNK_MSG_UTIL_TS_CONVENTION_BIT_OFFSET,   \
                               BSP_UICC_LNK_MSG_UTIL_TS_CONVENTION_BIT_WIDTH )   \
)
*/

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Format Character- T0
-----------------------------------------------------------------
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|  TD1  |  TC1  |  TB1  |  TA1  |      Num Historical           |
|Present|Present|Present|Present|        Bytes                  |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_INTERFACE_BYTE_NUM_OCTETS             1
#define BSP_UICC_LNK_MSG_UTIL_T0_NUM_OCTETS                         1

#define BSP_UICC_LNK_MSG_UTIL_T0_NUM_HISTORICAL_OCTET_OFFSET        0
#define BSP_UICC_LNK_MSG_UTIL_T0_NUM_HISTORICAL_BIT_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_T0_NUM_HISTORICAL_BIT_WIDTH           4

#define BSP_UICC_LNK_MSG_UTIL_T0_TA1_PRESENCE_OCTET_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_T0_TA1_PRESENCE_BIT_OFFSET            4
#define BSP_UICC_LNK_MSG_UTIL_T0_TA1_PRESENCE_BIT_WIDTH             1

#define BSP_UICC_LNK_MSG_UTIL_T0_TB1_PRESENCE_OCTET_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_T0_TB1_PRESENCE_BIT_OFFSET            5
#define BSP_UICC_LNK_MSG_UTIL_T0_TB1_PRESENCE_BIT_WIDTH             1

#define BSP_UICC_LNK_MSG_UTIL_T0_TC1_PRESENCE_OCTET_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_T0_TC1_PRESENCE_BIT_OFFSET            6
#define BSP_UICC_LNK_MSG_UTIL_T0_TC1_PRESENCE_BIT_WIDTH             1

#define BSP_UICC_LNK_MSG_UTIL_T0_TD1_PRESENCE_OCTET_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_T0_TD1_PRESENCE_BIT_OFFSET            7
#define BSP_UICC_LNK_MSG_UTIL_T0_TD1_PRESENCE_BIT_WIDTH             1

/*===========================================================================
 * Description:
 *  These macros are used to retreive the bits from the
 *  Interface Character- TA1
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|  Clock Rate Conversion factor |  Baud Rate Conversion factor  |
|                FI             |           DI                  |
-----------------------------------------------------------------
*/

#define BSP_UICC_LNK_MSG_UTIL_TA1_DI_OCTET_OFFSET                   0
#define BSP_UICC_LNK_MSG_UTIL_TA1_DI_BIT_OFFSET                     0
#define BSP_UICC_LNK_MSG_UTIL_TA1_DI_BIT_WIDTH                      4

#define BSP_UICC_LNK_MSG_UTIL_TA1_FI_OCTET_OFFSET                   0
#define BSP_UICC_LNK_MSG_UTIL_TA1_FI_BIT_OFFSET                     4
#define BSP_UICC_LNK_MSG_UTIL_TA1_FI_BIT_WIDTH                      4

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Interface Character- TB1
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|   xx  | Programming   |           Programming Voltage         |
|       |  current II   |                PI1                    |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_TB1_PROG_VOLTAGE_OCTET_OFFSET         0
#define BSP_UICC_LNK_MSG_UTIL_TB1_PROG_VOLTAGE_BIT_OFFSET           0
#define BSP_UICC_LNK_MSG_UTIL_TB1_PROG_VOLTAGE_BIT_WIDTH            5

#define BSP_UICC_LNK_MSG_UTIL_TB1_PROG_CURRENT_OCTET_OFFSET         0
#define BSP_UICC_LNK_MSG_UTIL_TB1_PROG_CURRENT_BIT_OFFSET           5
#define BSP_UICC_LNK_MSG_UTIL_TB1_PROG_CURRENT_BIT_WIDTH            2

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Interface Character- TC1
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                  Extra Guard Time Referrence                  |
|                              N                                |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_TC1_EXTRA_GUARD_TIME_REF_OCTET_OFFSET 0
#define BSP_UICC_LNK_MSG_UTIL_TC1_EXTRA_GUARD_TIME_REF_BIT_OFFSET   0
#define BSP_UICC_LNK_MSG_UTIL_TC1_EXTRA_GUARD_TIME_REF_BIT_WIDTH    8

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Interface Character- TB2 for protocol T0
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                       Programming Voltage                     |
|                               PI2                             |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_TB2_PROG_VOLTAGE_OCTET_OFFSET         0
#define BSP_UICC_LNK_MSG_UTIL_TB2_PROG_VOLTAGE_BIT_OFFSET           0
#define BSP_UICC_LNK_MSG_UTIL_TB2_PROG_VOLTAGE_BIT_WIDTH            8

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Interface Character- TA2 Specific Mode Byte
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5     |   4   |   3   |   2   |  1  |
-----------------------------------------------------------------
|Mode Ch|     Not       |Parameter|    Specific Mode Protocol   |
|  Bit  |    Used       |defn bit |          T                  |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_TA2_SPECIFIC_PROTOCOL_OCTET_OFFSET    0
#define BSP_UICC_LNK_MSG_UTIL_TA2_SPECIFIC_PROTOCOL_BIT_OFFSET      0
#define BSP_UICC_LNK_MSG_UTIL_TA2_SPECIFIC_PROTOCOL_BIT_WIDTH       4

#define BSP_UICC_LNK_MSG_UTIL_TA2_PARAMETER_DEFN_OCTET_OFFSET       0
#define BSP_UICC_LNK_MSG_UTIL_TA2_PARAMETER_DEFN_BIT_OFFSET         4
#define BSP_UICC_LNK_MSG_UTIL_TA2_PARAMETER_DEFN_BIT_WIDTH          1

#define BSP_UICC_LNK_MSG_UTIL_TA2_MODE_CHANGE_OCTET_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_TA2_MODE_CHANGE_BIT_OFFSET            7
#define BSP_UICC_LNK_MSG_UTIL_TA2_MODE_CHANGE_BIT_WIDTH             1

#define BSP_UICC_LNK_RESET_MGR_MODE_CHANGE_NOT_SUPPORTED_VALUE      1

#define BSP_UICC_LNK_MSG_UTIL_TA2_PARAMETER_DEFN_IMPLICIT_VALUE     1

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Interface Character- First protocol specific TA
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                 Max Block size card can receive               |
|                            IFSC                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_TAI_BLOCK_SIZE_OCTET_OFFSET           0
#define BSP_UICC_LNK_MSG_UTIL_TAI_BLOCK_SIZE_BIT_OFFSET             0
#define BSP_UICC_LNK_MSG_UTIL_TAI_BLOCK_SIZE_BIT_WIDTH              8

/*===========================================================================
 * Description:
 *  These macros are used to retreive the bits from the
 *  Interface Character- TA1
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|     Character Wait Integer    |      Block Wait Integer       |
|              CWI              |           BWI                 |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_TBI_CHAR_WAIT_INT_OCTET_OFFSET        0
#define BSP_UICC_LNK_MSG_UTIL_TBI_CHAR_WAIT_INT_BIT_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_TBI_CHAR_WAIT_INT_BIT_WIDTH           4

#define BSP_UICC_LNK_MSG_UTIL_TBI_BLOCK_WAIT_INT_OCTET_OFFSET       0
#define BSP_UICC_LNK_MSG_UTIL_TBI_BLOCK_WAIT_INT_BIT_OFFSET         4
#define BSP_UICC_LNK_MSG_UTIL_TBI_BLOCK_WAIT_INT_BIT_WIDTH          4


/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Interface Character- TDi. The TA_PRESENCE macros indicate
 * presence of  TA(i+1).
-----------------------------------------------------------------
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
| TD(i) | TC(i) | TB(i) | TA(i) |      Protocol referrence/     |
|Present|Present|Present|Present|    Interface byte qualifier   |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_OCTET_OFFSET             0
#define BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_BIT_OFFSET               0
#define BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_BIT_WIDTH                4

#define BSP_UICC_LNK_MSG_UTIL_TDI_TA_PRESENCE_OCTET_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_TDI_TA_PRESENCE_BIT_OFFSET            4
#define BSP_UICC_LNK_MSG_UTIL_TDI_TA_PRESENCE_BIT_WIDTH             1

#define BSP_UICC_LNK_MSG_UTIL_TDI_TB_PRESENCE_OCTET_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_TDI_TB_PRESENCE_BIT_OFFSET            5
#define BSP_UICC_LNK_MSG_UTIL_TDI_TB_PRESENCE_BIT_WIDTH             1

#define BSP_UICC_LNK_MSG_UTIL_TDI_TC_PRESENCE_OCTET_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_TDI_TC_PRESENCE_BIT_OFFSET            6
#define BSP_UICC_LNK_MSG_UTIL_TDI_TC_PRESENCE_BIT_WIDTH             1

#define BSP_UICC_LNK_MSG_UTIL_TDI_TD_PRESENCE_OCTET_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_TDI_TD_PRESENCE_BIT_OFFSET            7
#define BSP_UICC_LNK_MSG_UTIL_TDI_TD_PRESENCE_BIT_WIDTH             1

#define BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_T0_VALUE                 0x0
#define BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_T1_VALUE                 0x1
#define BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_T15_VALUE                0xF

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Interface Character- TAi. This caracter can occur after
 * the first presence of T=15
-----------------------------------------------------------------
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
| Clock Stop Ref|              Class Indicator Ref.             |
|      XI       |                       UI                      |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_TAI_CLASS_REF_OCTET_OFFSET            0
#define BSP_UICC_LNK_MSG_UTIL_TAI_CLASS_REF_BIT_OFFSET              0
#define BSP_UICC_LNK_MSG_UTIL_TAI_CLASS_REF_BIT_WIDTH               6

#define BSP_UICC_LNK_MSG_UTIL_TAI_CLOCK_STOP_REF_OCTET_OFFSET       0
#define BSP_UICC_LNK_MSG_UTIL_TAI_CLOCK_STOP_REF_BIT_OFFSET         6
#define BSP_UICC_LNK_MSG_UTIL_TAI_CLOCK_STOP_REF_BIT_WIDTH          2

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Interface Character- T1
-----------------------------------------------------------------
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                          Check Byte                           |
|                             TCK                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_TCK_NUM_OCTETS                        1

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * First PPS Character- PPSS.
-----------------------------------------------------------------
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                       PPS First Character                     |
|                             PPSS                              |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_PPSS_NUM_OCTETS                       1

#define BSP_UICC_LNK_MSG_UTIL_PPSS_OCTET_OFFSET                     0
#define BSP_UICC_LNK_MSG_UTIL_PPSS_BIT_OFFSET                       0
#define BSP_UICC_LNK_MSG_UTIL_PPSS_BIT_WIDTH                        8

#define BSP_UICC_LNK_MSG_UTIL_PPSS_VALUE                            0xFF

/*===========================================================================
 * Description:
 * These macros are used to retreive the bits from the
 * Format Character- PPS0
-----------------------------------------------------------------
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|  Not  |  PPS3 | PPS2  |  PPS1 |           Protocol            |
|  Used |Present|Present|Present|              T                |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_PPS0_NUM_OCTETS                       1
#define BSP_UICC_LNK_MSG_UTIL_PPS0_OCTET_OFFSET                     1

#define BSP_UICC_LNK_MSG_UTIL_PPS0_PROTOCOL_OCTET_OFFSET            1
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PROTOCOL_BIT_OFFSET              0
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PROTOCOL_BIT_WIDTH               4

#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS1_PRESENCE_OCTET_OFFSET       1
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS1_PRESENCE_BIT_OFFSET         4
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS1_PRESENCE_BIT_WIDTH          1

#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS2_PRESENCE_OCTET_OFFSET       1
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS2_PRESENCE_BIT_OFFSET         5
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS2_PRESENCE_BIT_WIDTH          1

#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS3_PRESENCE_OCTET_OFFSET       1
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS3_PRESENCE_BIT_OFFSET         6
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS3_PRESENCE_BIT_WIDTH          1

#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS4_PRESENCE_OCTET_OFFSET       1
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS4_PRESENCE_BIT_OFFSET         7
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PPS4_PRESENCE_BIT_WIDTH          1

#define BSP_UICC_LNK_MSG_UTIL_PPS0_PRESENCE_VALUE_PRESENT           1
#define BSP_UICC_LNK_MSG_UTIL_PPS0_PRESENCE_VALUE_NOT_PRESENT       0
/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  PPS Character- PPS1
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|  Clock Rate Conversion factor |  Baud Rate Conversion factor  |
|                FI             |           DI                  |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_PPS1_NUM_OCTETS                       1
#define BSP_UICC_LNK_MSG_UTIL_PPS1_OCTET_OFFSET                     2

#define BSP_UICC_LNK_MSG_UTIL_PPS1_DI_OCTET_OFFSET                  2
#define BSP_UICC_LNK_MSG_UTIL_PPS1_DI_BIT_OFFSET                    0
#define BSP_UICC_LNK_MSG_UTIL_PPS1_DI_BIT_WIDTH                     4

#define BSP_UICC_LNK_MSG_UTIL_PPS1_FI_OCTET_OFFSET                  2
#define BSP_UICC_LNK_MSG_UTIL_PPS1_FI_BIT_OFFSET                    4
#define BSP_UICC_LNK_MSG_UTIL_PPS1_FI_BIT_WIDTH                     4

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  PPS Character- PCK
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                           Check Byte                          |
|                              PCK                              |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_PCK_NUM_OCTETS                        1

#define BSP_UICC_LNK_MSG_UTIL_PCK_OCTET_OFFSET                      3
#define BSP_UICC_LNK_MSG_UTIL_PCK_BIT_OFFSET                        0
#define BSP_UICC_LNK_MSG_UTIL_PCK_BIT_WIDTH                         8

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  Class field of a Command APDU (Application Protocol Data Unit)
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                           Class Byte                          |
|                              CLA                              |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_APDU_CLA_NUM_OCTETS                   1

#define BSP_UICC_LNK_MSG_UTIL_APDU_CLA_OCTET_OFFSET                 0
#define BSP_UICC_LNK_MSG_UTIL_APDU_CLA_BIT_OFFSET                   0
#define BSP_UICC_LNK_MSG_UTIL_APDU_CLA_BIT_WIDTH                    8

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  Instruction field of a Command APDU
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                       Instruction Byte                        |
|                              INS                              |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_APDU_INS_NUM_OCTETS                   1

#define BSP_UICC_LNK_MSG_UTIL_APDU_INS_OCTET_OFFSET                 1
#define BSP_UICC_LNK_MSG_UTIL_APDU_INS_BIT_OFFSET                   0
#define BSP_UICC_LNK_MSG_UTIL_APDU_INS_BIT_WIDTH                    8

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  Parameter1 field of a Command APDU
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                         Parameter 1                           |
|                              P1                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_APDU_P1_NUM_OCTETS                    1

#define BSP_UICC_LNK_MSG_UTIL_APDU_P1_OCTET_OFFSET                  2
#define BSP_UICC_LNK_MSG_UTIL_APDU_P1_BIT_OFFSET                    0
#define BSP_UICC_LNK_MSG_UTIL_APDU_P1_BIT_WIDTH                     8

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  Parameter2 field of a Command APDU
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                         Parameter 1                           |
|                              P2                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_APDU_P2_NUM_OCTETS                    1

#define BSP_UICC_LNK_MSG_UTIL_APDU_P2_OCTET_OFFSET                  3
#define BSP_UICC_LNK_MSG_UTIL_APDU_P2_BIT_OFFSET                    0
#define BSP_UICC_LNK_MSG_UTIL_APDU_P2_BIT_WIDTH                     8

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  Parameter3 field of a Command APDU
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                         Parameter 1                           |
|                              P3                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_APDU_P3_NUM_OCTETS                    1

#define BSP_UICC_LNK_MSG_UTIL_APDU_P3_OCTET_OFFSET                  4
#define BSP_UICC_LNK_MSG_UTIL_APDU_P3_BIT_OFFSET                    0
#define BSP_UICC_LNK_MSG_UTIL_APDU_P3_BIT_WIDTH                     8

#define BSP_UICC_LNK_MSG_UTIL_APDU_P3_CASE1_VALUE                   0x0

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  SW1 field of a Response APDU
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                         Status Byte                           |
|                             SW1                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_APDU_SW1_NUM_OCTETS                   1

#define BSP_UICC_LNK_MSG_UTIL_APDU_SW1_OCTET_OFFSET                 0
#define BSP_UICC_LNK_MSG_UTIL_APDU_SW1_BIT_OFFSET                   0
#define BSP_UICC_LNK_MSG_UTIL_APDU_SW1_BIT_WIDTH                    8

#define BSP_UICC_LNK_MSG_UTIL_APDU_SW1_MORE_DATA_PROC_VALUE         0x61
#define BSP_UICC_LNK_MSG_UTIL_APDU_SW1_CORRECT_LEN_PROC_VALUE       0x6C
#define BSP_UICC_LNK_MSG_UTIL_APDU_SW1_WRONG_LEN_PROC_VALUE         0x67
#define BSP_UICC_LNK_MSG_UTIL_APDU_SW1_MORE_DATA_GSM_PROC_VALUE     0x9F

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  SW2 field of a Response APDU
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                         Status Byte                           |
|                             SW2                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_APDU_SW2_NUM_OCTETS                   1

#define BSP_UICC_LNK_MSG_UTIL_APDU_SW2_OCTET_OFFSET                 1
#define BSP_UICC_LNK_MSG_UTIL_APDU_SW2_BIT_OFFSET                   0
#define BSP_UICC_LNK_MSG_UTIL_APDU_SW2_BIT_WIDTH                    8


/*===========================================================================
 * Description:
 *  Message Utilities specific to BSP_UICC generic commands 
 */  
/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P1 field of the Select File command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                     Selection Control                         |
|                                                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_SELECT_P1_SEL_TYPE_OCTET_OFFSET       0
#define BSP_UICC_LNK_MSG_UTIL_SELECT_P1_SEL_TYPE_BIT_OFFSET         0
#define BSP_UICC_LNK_MSG_UTIL_SELECT_P1_SEL_TYPE_BIT_WIDTH          8

#define BSP_UICC_LNK_MSG_UTIL_SELECT_P1_SEL_TYPE_FILE_ID_VALUE      0x0
#define BSP_UICC_LNK_MSG_UTIL_SELECT_P1_SEL_TYPE_AID_VALUE          0x4

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P2 field of the Select File command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                     Return Result Data                        |
|                                                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_SELECT_P2_RET_TYPE_OCTET_OFFSET       0
#define BSP_UICC_LNK_MSG_UTIL_SELECT_P2_RET_TYPE_BIT_OFFSET         0
#define BSP_UICC_LNK_MSG_UTIL_SELECT_P2_RET_TYPE_BIT_WIDTH          8

#define BSP_UICC_LNK_MSG_UTIL_SELECT_P2_RET_TYPE_FCP_VALUE          0x4
#define BSP_UICC_LNK_MSG_UTIL_SELECT_P2_RET_TYPE_NO_FCP_VALUE       0x5   

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P1 field of the Status command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                     Application Indication                    |
|                                                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_STATUS_P1_APP_IND_OCTET_OFFSET        0
#define BSP_UICC_LNK_MSG_UTIL_STATUS_P1_APP_IND_BIT_OFFSET          0
#define BSP_UICC_LNK_MSG_UTIL_STATUS_P1_APP_IND_BIT_WIDTH           8

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P2 field of the Status command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                    Return Result Data                         |
|                                                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_STATUS_P2_RET_TYPE_OCTET_OFFSET       0
#define BSP_UICC_LNK_MSG_UTIL_STATUS_P2_RET_TYPE_BIT_OFFSET         0
#define BSP_UICC_LNK_MSG_UTIL_STATUS_P2_RET_TYPE_BIT_WIDTH          8

#define BSP_UICC_LNK_MSG_UTIL_STATUS_P2_RET_TYPE_FCP_VALUE          0x4
#define BSP_UICC_LNK_MSG_UTIL_STATUS_P2_RET_TYPE_NO_FCP_VALUE       0x5

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P1 field of the Read Binary command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|  SFI  |                    Offset                             |
|  Ref. |                to read the first byte                 |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_READ_BINARY_P1_OFFSET_OCTET_OFFSET    0
#define BSP_UICC_LNK_MSG_UTIL_READ_BINARY_P1_OFFSET_BIT_OFFSET      0
#define BSP_UICC_LNK_MSG_UTIL_READ_BINARY_P1_OFFSET_BIT_WIDTH       7

#define BSP_UICC_LNK_MSG_UTIL_READ_BINARY_P1_SFI_OCTET_OFFSET       0
#define BSP_UICC_LNK_MSG_UTIL_READ_BINARY_P1_SFI_BIT_OFFSET         7
#define BSP_UICC_LNK_MSG_UTIL_READ_BINARY_P1_SFI_BIT_WIDTH          1

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P1 field of the Read Record command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                          Record Number                        |
|                                                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_READ_RECORD_P1_REC_NUM_OCTET_OFFSET   0
#define BSP_UICC_LNK_MSG_UTIL_READ_RECORD_P1_REC_NUM_BIT_OFFSET     0
#define BSP_UICC_LNK_MSG_UTIL_READ_RECORD_P1_REC_NUM_BIT_WIDTH      8

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P2 field of the Read Record command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|             File Id                   |      Read Mode        |
|                                       |                       |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_READ_RECORD_P2_READ_MODE_OCTET_OFFSET 0
#define BSP_UICC_LNK_MSG_UTIL_READ_RECORD_P2_READ_MODE_BIT_OFFSET   0
#define BSP_UICC_LNK_MSG_UTIL_READ_RECORD_P2_READ_MODE_BIT_WIDTH    3

#define BSP_UICC_LNK_MSG_UTIL_READ_RECORD_P2_FILE_ID_OCTET_OFFSET   0
#define BSP_UICC_LNK_MSG_UTIL_READ_RECORD_P2_FILE_ID_BIT_OFFSET     3
#define BSP_UICC_LNK_MSG_UTIL_READ_RECORD_P2_FILE_ID_BIT_WIDTH      5

#define BSP_UICC_LNK_MSG_UTIL_READ_RECORD_P2_CURRENT_FILE_VALUE     0x00

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P1 field of the Update Record command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                          Record Number                        |
|                                                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_UPDATE_RECORD_P1_REC_NUM_OCTET_OFFSET 0
#define BSP_UICC_LNK_MSG_UTIL_UPDATE_RECORD_P1_REC_NUM_BIT_OFFSET   0
#define BSP_UICC_LNK_MSG_UTIL_UPDATE_RECORD_P1_REC_NUM_BIT_WIDTH    8

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P2 field of the Update Record command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|               xxxxx                   |           Mode        |
|                                       |                       |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_UPDATE_RECORD_P2_MODE_OCTET_OFFSET    0
#define BSP_UICC_LNK_MSG_UTIL_UPDATE_RECORD_P2_MODE_BIT_OFFSET      0
#define BSP_UICC_LNK_MSG_UTIL_UPDATE_RECORD_P2_MODE_BIT_WIDTH       3

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P2 field of the Verify PIN command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
| Pin   |               |               Ref Data                |
| Qual  |               |                                       |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_VERIFY_PIN_P2_REF_DATA_OCTET_OFFSET   0
#define BSP_UICC_LNK_MSG_UTIL_VERIFY_PIN_P2_REF_DATA_BIT_OFFSET     0
#define BSP_UICC_LNK_MSG_UTIL_VERIFY_PIN_P2_REF_DATA_BIT_WIDTH      5

#define BSP_UICC_LNK_MSG_UTIL_VERIFY_PIN_P2_PIN_QUAL_OCTET_OFFSET   0
#define BSP_UICC_LNK_MSG_UTIL_VERIFY_PIN_P2_PIN_QUAL_BIT_OFFSET     7
#define BSP_UICC_LNK_MSG_UTIL_VERIFY_PIN_P2_PIN_QUAL_BIT_WIDTH      1

#define BSP_UICC_LNK_MSG_UTIL_VERIFY_PIN_P2_APP_PIN_VALUE           0x1
#define BSP_UICC_LNK_MSG_UTIL_VERIFY_PIN_P2_GLOBAL_PIN_VALUE        0x0
 
/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P2 field of the Change PIN command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
| Pin   |               |               Ref Data                |
| Qual  |               |                                       |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_CHANGE_PIN_P2_REF_DATA_OCTET_OFFSET   0
#define BSP_UICC_LNK_MSG_UTIL_CHANGE_PIN_P2_REF_DATA_BIT_OFFSET     0
#define BSP_UICC_LNK_MSG_UTIL_CHANGE_PIN_P2_REF_DATA_BIT_WIDTH      5

#define BSP_UICC_LNK_MSG_UTIL_CHANGE_PIN_P2_PIN_QUAL_OCTET_OFFSET   0
#define BSP_UICC_LNK_MSG_UTIL_CHANGE_PIN_P2_PIN_QUAL_BIT_OFFSET     7
#define BSP_UICC_LNK_MSG_UTIL_CHANGE_PIN_P2_PIN_QUAL_BIT_WIDTH      1

#define BSP_UICC_LNK_MSG_UTIL_CHANGE_PIN_P2_APP_PIN_VALUE           0x1
#define BSP_UICC_LNK_MSG_UTIL_CHANGE_PIN_P2_GLOBAL_PIN_VALUE        0x0
 
/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  P2 field of the Change PIN command
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                       |          Auth context                 |
|                       |            Ref Data                   |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_UMTS_AUTH_P2_REF_DATA_OCTET_OFFSET   0
#define BSP_UICC_LNK_MSG_UTIL_UMTS_AUTH_P2_REF_DATA_BIT_OFFSET     0
#define BSP_UICC_LNK_MSG_UTIL_UMTS_AUTH_P2_REF_DATA_BIT_WIDTH      5

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  NAD byte in the Block header/Prologue
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|  RFU  |  Destination Address  |  RFU  |    Source Address     |
|       |        DAD            |       |        SAD            |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD_OCTET_OFFSET               0
#define BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD_BIT_OFFSET                 0
#define BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD_BIT_WIDTH                  3

#define BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD_OCTET_OFFSET               0
#define BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD_BIT_OFFSET                 4
#define BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD_BIT_WIDTH                  3

#define BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD_VALUE                      0x0
#define BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD_VALUE                      0x0

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  PCB byte in the I Block header/Prologue
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
| Block |Seq Num|  More |             RFU                       |
| Type  |   N   |  Data |                                       |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_TYPE_OCTET_OFFSET      1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_TYPE_BIT_OFFSET        7
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_TYPE_BIT_WIDTH         1

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_SEQ_NUM_OCTET_OFFSET   1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_SEQ_NUM_BIT_OFFSET     6
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_SEQ_NUM_BIT_WIDTH      1

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_MORE_DATA_OCTET_OFFSET 1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_MORE_DATA_BIT_OFFSET   5
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_MORE_DATA_BIT_WIDTH    1

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_TYPE_VALUE             0x0
/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  PCB byte in the R Block header/Prologue
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|  Block Type   | Not   |Seq Num|          Error Ind            |
|     R         | Used  |   N   |             Err               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_TYPE_OCTET_OFFSET      1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_TYPE_BIT_OFFSET        6
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_TYPE_BIT_WIDTH         2

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_UNDEF_OCTET_OFFSET     1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_UNDEF_BIT_OFFSET       5
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_UNDEF_BIT_WIDTH        1

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_SEQ_NUM_OCTET_OFFSET   1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_SEQ_NUM_BIT_OFFSET     4
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_SEQ_NUM_BIT_WIDTH      1

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_ERR_IND_OCTET_OFFSET   1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_ERR_IND_BIT_OFFSET     0
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_ERR_IND_BIT_WIDTH      4

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_NO_ERROR_VALUE         0x0
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_EDC_ERROR_VALUE        0x1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_OTHER_ERROR_VALUE      0x2

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_UNDEF_VALUE            0x0

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_TYPE_VALUE             0x2

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  PCB byte in the S Block header/Prologue
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|  Block Type   | Direc |              Block Info               |
|     S         | -tion |                  Info                 |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_TYPE_OCTET_OFFSET      1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_TYPE_BIT_OFFSET        6
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_TYPE_BIT_WIDTH         2

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_OCTET_OFFSET 1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_BIT_OFFSET   5
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_BIT_WIDTH    1

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_OCTET_OFFSET      1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_BIT_OFFSET        0
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_BIT_WIDTH         5

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_TYPE_VALUE             0x3

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_REQ_VALUE    0x0
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_RSP_VALUE    0x1

#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_RESYNCH_VALUE     0x0
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_IFS_VALUE         0x1
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_ABORT_VALUE       0x2
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_WTX_VALUE         0x3
#define BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_VPP_ERROR_VALUE   0x4


/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  Length byte of a block
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                            Length                             |
|                             Len                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH_OCTET_OFFSET             2
#define BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH_BIT_OFFSET               0
#define BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH_BIT_WIDTH                8

/*===========================================================================
 * Description:
 *  These macros are used to set and get the bits from the
 *  epilogue field of a block, No octet offset as the inf
 *  size can vary.
----------------------------------------------------------------- 
|   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |
-----------------------------------------------------------------
|                          Epilogue                             |
|                                                               |
-----------------------------------------------------------------
*/
#define BSP_UICC_LNK_MSG_UTIL_BLOCK_EPILOGUE_OCTET_OFFSET           0
#define BSP_UICC_LNK_MSG_UTIL_BLOCK_EPILOGUE_BIT_OFFSET             0
#define BSP_UICC_LNK_MSG_UTIL_BLOCK_EPILOGUE_BIT_WIDTH              8



#define BSP_UICC_LNK_MSG_UTIL_GET( _name,                       \
                                   _msgPtr )                    \
(                                                               \
    BSPUTIL_BITUTIL_BIT_FIELD_GET                               \
        ( (((SYS_UWORD8 *)(_msgPtr)) + (_name ##_OCTET_OFFSET)),     \
          BSPUTIL_BITUTIL_DATAUNIT_8,                           \
          _name ##_BIT_OFFSET,                                  \
          _name ##_BIT_WIDTH )                                  \
)

#define BSP_UICC_LNK_MSG_UTIL_SET( _name ,                      \
                                   _msgPtr ,                    \
                                   _newValue )                  \
{                                                               \
    BSPUTIL_BITUTIL_BIT_FIELD_SET                               \
        ( (((SYS_UWORD8 *)(_msgPtr)) + (_name ##_OCTET_OFFSET)),     \
             _newValue,                                         \
             BSPUTIL_BITUTIL_DATAUNIT_8,                        \
             _name ##_BIT_OFFSET,                               \
             _name ##_BIT_WIDTH );                              \
}

#endif

