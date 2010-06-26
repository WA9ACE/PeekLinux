/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */

#include "sys_types.h"
#include "types.h"
#include "general.h"

#undef FEATURE_UICC_DISABLE_PPS
#undef FEATURE_UICC_DISABLE_SPEED_ENHANCEMENT
#define FEATURE_UICC_DISABLE_T1_PROTOCOL 1

#include "bspUicc_Phy.h"
#include "bspUicc_Power.h"
#include "bspUicc_Lnk.h"
#include "bspUicc_Lnk_resetMgr.h"
#include "bspUicc_Lnk_dataConfig.h"
#include "bspUicc_Lnk_msgUtil.h"
#include "bspUicc_Phy_llif.h"
#include "bspUicc_Phy_map.h"


/*=============================================================================
 *  File Description:
 *  Source File with code specific to reception and interpretation of ATR
 *  First, refer to ISO/IEC 7816-3 spec to understand the ATR & PPS contents
 */


/*=============================================================================
 *   Defines
 *============================================================================*/
#define BSP_UICC_LNK_RESET_MGR_MAX_ATR_ATTEMPTS              6

#define BSP_UICC_LNK_RESET_MGR_WARM_RESET_ATTEMPTS           3

#define BSP_UICC_LNK_RESET_MGR_ATR_ATTEMPTS_PER_POWERLEVEL   3

#define BSP_UICC_LNK_RESET_MGR_PPS_REQUEST_SIZE              4

#define BSP_UICC_LNK_RESET_MGR_PPS_RESPONSE_SIZE             4

#define BSP_UICC_LNK_CARD_NOT_PRESENT                        0

#define BSP_UICC_LNK_RESET_MGR_MAX_PPS_ATTEMPTS              3

#define BSP_UICC_MODULE_FREQUENCY							13

#define BSP_UICC_MAX_INTERRUPT_LATENCY						400

#define BSP_UICC_TOTAL_NO_OF_ETUS_PER_BYTE					12
//#define FEATURE_UICC_DISABLE_PPS 1
#define CLOCK_STOP_ENABLE
/*=============================================================================
 *   Types and Enumerations
 *============================================================================*/

/*=============================================================================
 *  Description
 *  This type specifies the ATR interface character set, each set has
 * a TA character, TB character, TC character and a TD character.
 */
enum
{
    BSP_UICC_LNK_RESET_MGR_INTERFACE_CHAR_SET_FIRST   = 1,
    BSP_UICC_LNK_RESET_MGR_INTERFACE_CHAR_SET_SECOND  = 2,
    BSP_UICC_LNK_RESET_MGR_INTERFACE_CHAR_SET_THIRD   = 3
};
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrInterfaceCharSet ;

/*=============================================================================
 *  Description
 *  This type specifies the number of Interface characters present in
 *  each interface character set, i.e. TA, TB, TC, and TD
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrIntTotalPresent;

/*=============================================================================
 *  Description
 *  This type specifies presence of a particular interface character
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrIntPresentFlag;

/*=============================================================================
 *  Description
 *  This type specifies the first offered protocol after the Answer to reset
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrFirstProtocol;

/*=============================================================================
 *  Description
 *  This type specifies the second protocol
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrSecondProtocol;

/*=============================================================================
 *  Description
 *  This type specifies the specific protocol to be used. This is
 *  indicated in the TA2 character in the ATR
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrSpecificProtocol;

/*=============================================================================
 *  Description
 *  This type specifies the reference to the clock rate conversion factor
 *  Also called as Fi
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrClkConvFactor;

/*=============================================================================
 *  Description
 *  This type specifies the reference to the baud rate conversion factor
 *  Also called as Di
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrBaudConvFactor;

/*=============================================================================
 *  Description
 *  This type specifies the reference to the maximum programming current
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrProgCurrent;

/*=============================================================================
 *  Description
 *  This type specifies the reference to the maximum programming voltage
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrProgVoltage;

/*=============================================================================
 *  Description
 *  This type specifies the reference to compute the extra guard time
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrExtraGuardTime;
/*=============================================================================
 *  Description
 *  This type specifies the reference to compute the extra Wait Interval Value
 */
typedef SYS_UWORD16 BspUicc_Lnk_ResetMgrWaitIntervalValue;

/*=============================================================================
 *  Description
 *  This type specifies if the card supports clock stop
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrClockStopInd;

/*=============================================================================
 *  Description
 *  This type specifies the class of the card
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrCardClass;

/*=============================================================================
 *  Description
 *  This type specifies the maximum block size that can be received by the card
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrBlockSize;

/*=============================================================================
 *  Description
 *  This type specifies the Character waiting time i.e time delay between
 *  two consecutive characters in a block.
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrCharWaitInt;

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_ResetMgrBlockWaitInt
 *
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the Block Waiting time i.e time delay between last
 *  character of a block to first character of next block
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrBlockWaitInt;

/*=============================================================================
 *  Description
 *  This type specifies the total number of bytes received in the Atr
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrTotalAtrSize;

/*=============================================================================
 *  Description
 *  This type specifies the total number of historic bytes received in the Atr
 */
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrNumHistoricBytes;

/*=============================================================================
 *  Description
 *  This type specifies if the card supports change from negotiable mode to specific
 *  mode.
 */
enum
{
    BSP_UICC_LNK_RESET_MGR_ATR_MODE_SPECIFIC_MODE        = 0,
    BSP_UICC_LNK_RESET_MGR_ATR_MODE_NEGOTIABLE_MODE      = 1
};
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrAtrMode;

/*=============================================================================
 *  Description
 *  This type specifies the use of ATR interface bytes versus the default
 *  implicit values
 */
enum
{
    BSP_UICC_LNK_RESET_MGR_PARAMETER_DEFN_IMPLICIT         = 0,
    BSP_UICC_LNK_RESET_MGR_PARAMETER_DEFN_INTERFACE        = 1
};
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrParameterDefn;

/*=============================================================================
 *  Description
 *  This type specifies the use of ATR interface bytes versus the default
 *  implicit values
 */
enum
{
    BSP_UICC_LNK_RESET_MGR_MODE_CHANGEABLE_FALSE         = 0,
    BSP_UICC_LNK_RESET_MGR_MODE_CHANGEABLE_TRUE          = 1
};
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgrModeChangeable;

/*=============================================================================
 *  Description
 *  This type has all the decoded ATR data
 */
typedef struct
{
    BspUicc_Lnk_ResetMgrFirstProtocol     firstProtocol;
    BspUicc_Lnk_ResetMgrSecondProtocol    secondProtocol;
    BspUicc_Lnk_ResetMgrSpecificProtocol  specificProtocol;
    BspUicc_Lnk_ResetMgrBlockSize         blockSize;
    BspUicc_Lnk_ResetMgrCharWaitInt       charWaitInt;
    BspUicc_Lnk_ResetMgrBlockWaitInt      blockWaitInt;
    BspUicc_Lnk_ResetMgrClkConvFactor     clkConvFactor;
    BspUicc_Lnk_ResetMgrBaudConvFactor    baudConvFactor;
    BspUicc_Lnk_ResetMgrProgCurrent       progCurrent;
    BspUicc_Lnk_ResetMgrProgVoltage       progVoltage;
    BspUicc_Lnk_ResetMgrExtraGuardTime    extraGuardTime;
    BspUicc_Lnk_ResetMgrWaitIntervalValue waitIValue;
    BspUicc_Lnk_ResetMgrClockStopInd      clockStopInd;
    BspUicc_Lnk_ResetMgrCardClass         cardClass;
    BspUicc_Lnk_ResetMgrAtrMode           atrMode;
    BspUicc_Lnk_ResetMgrParameterDefn     parameterDefn;
    BspUicc_Lnk_ResetMgrModeChangeable    modeChangeable;
    BspUicc_Lnk_ResetMgrNumHistoricBytes  numHistoricBytes;
    BspUicc_Lnk_ResetMgrTotalAtrSize      totalAtrSize;
}BspUicc_Lnk_ResetMgrAtrDecodedData;

/*=============================================================================
 *  Description
 *  This type specifies the presence of interface characters in a character set
 * and the number of characters present.
 */
typedef struct
{
    BspUicc_Lnk_ResetMgrIntPresentFlag  taPresent;
    BspUicc_Lnk_ResetMgrIntPresentFlag  tbPresent;
    BspUicc_Lnk_ResetMgrIntPresentFlag  tcPresent;
    BspUicc_Lnk_ResetMgrIntPresentFlag  tdPresent;
    BspUicc_Lnk_ResetMgrIntTotalPresent totalPresent;
}BspUicc_Lnk_ResetMgrIntPresent;

/*=============================================================================
 *  Description
 *  This type is used to store the raw Atr characters before decoding
 */
typedef struct
{
    BspUicc_Lnk_ResetData  ts;
    BspUicc_Lnk_ResetData  t0;
    BspUicc_Lnk_ResetData  ta1;
    BspUicc_Lnk_ResetData  tb1;
    BspUicc_Lnk_ResetData  tc1;
    BspUicc_Lnk_ResetData  td1;
    BspUicc_Lnk_ResetData  ta2;
    BspUicc_Lnk_ResetData  tb2;
    BspUicc_Lnk_ResetData  tc2;
    BspUicc_Lnk_ResetData  td2;
    BspUicc_Lnk_ResetData  ta3;
    BspUicc_Lnk_ResetData  tb3;
    BspUicc_Lnk_ResetData  tc3;
    BspUicc_Lnk_ResetData  td3;
    BspUicc_Lnk_ResetData  ta4;
    BspUicc_Lnk_ResetData  tb4;
    BspUicc_Lnk_ResetData  tc4;
    BspUicc_Lnk_ResetData  td4;
    BspUicc_Lnk_ResetData  tck;
}BspUicc_Lnk_ResetMgrAtrData;

/*=============================================================================
 *  Description
 *  This type is used to store the raw pps characters before sending requests
 *  and receiving responses
 */
typedef struct
{
    BspUicc_Lnk_ResetData  ppss;
    BspUicc_Lnk_ResetData  pps0;
    BspUicc_Lnk_ResetData  pps1;
    BspUicc_Lnk_ResetData  pck;
}BspUicc_Lnk_ResetMgrPpsData;

/*=============================================================================
 *  Description
 *  Maximum number of acceptable frequency values in the table.
 */
#define BSP_UICC_LNK_RESET_MGR_MAX_FREQ_VALUES 16
#define BSP_UICC_LNK_RESET_MGR_MAX_FREQ_DIVIDER 4

/*=============================================================================
 *  Description
 *  Table for sim frequency (coded as in USIM register field CONFSCLKDIV
 *  in USIMCONF2) according to FI.
 *  ( FI = Indicated Freq conversion factor, This can have a max of 16
 *    different values, each value needs an acceptable freqency match for
 *    timing calculations )
 */
SYS_UWORD8 bspUicc_Lnk_resetMgrFreqValues[BSP_UICC_LNK_RESET_MGR_MAX_FREQ_VALUES]=
{
    1,1,1,0,
    0,3,3,1,
    1,1,0,0,
    3,3,1,1
};

/*=============================================================================
 *  Description
 *  Table for sim frequency divider(coded as in USIM register field CONFSCLKDIV
 *  in USIMCONF2) according to FI.
 *  
 */

SYS_UWORD8 bspUicc_Lnk_resetMgrFreqDivider[BSP_UICC_LNK_RESET_MGR_MAX_FREQ_DIVIDER]=
{
	2,4,8,1
};


/*=============================================================================
 *  Description
 *  Table for max sim frequency according to FI
 *  ( FI = Indicated Freq conversion factor, This can have a max of 16
 *    different values, each value needs a max freqency match for
 *    timing calculations )
 */
SYS_UWORD8 bspUicc_Lnk_resetMgrMaxFreqValues[BSP_UICC_LNK_RESET_MGR_MAX_FREQ_VALUES]=
{
     4, 5, 6, 8,
    12,16,20, 4,
     4,5,7,10,
    15, 20, 4, 4
};

/*==============================================================================
 * Description
 * Table for decoding the FI value according to the TA(1) character from ATR
 */
Uint16 Fi_Ta1_Mapping[16] =
{
    372,  372,  558,  744,
    1116, 1488, 1860, 372,
    372,   512,  768,  1024,
    1536, 2048, 372,  372
};


/*==============================================================================
 * Description
 * Table for decoding the DI value according to the TA(1) character from ATR
 */
Uint8 Di_Ta1_Mapping[16] =
{
    1,  1,  2,  4,
    8,  16, 32, 1,
    12, 20, 1,  1,
    1,  1,  1,  1
};

BspUicc_Lnk_ResetData  ppsResponse[ BSP_UICC_LNK_RESET_MGR_PPS_RESPONSE_SIZE ];
BspUicc_Lnk_ResetData  ppsRequest[ BSP_UICC_LNK_RESET_MGR_PPS_REQUEST_SIZE ];

BspUicc_Lnk_ResetMgrProgVoltage bspUicc_Lnk_CurrentVoltage;

BspUicc_Lnk_AtrDataInfo  answerToReset;
SYS_UWORD8 pps_request_data_size;
SYS_UWORD8 rx_trigger_value = 0x0F;


/*=============================================================================
 *   Public Data
 *============================================================================*/
/* Description:
 *   Control Data used by Physical layer
 */
extern BspUicc_Phy_Ctrl bspUicc_Phy_ctrl;
/*=============================================================================
 * Description:
 *   Contains Raw ATR data fetched by the bspUicc_Lnk_resetMgrGetAtrChars function
 */
BspUicc_Lnk_ResetMgrAtrData bspUicc_Lnk_resetMgrAtrData;

/*=============================================================================
 * Description:
 *   Contains the decoded ATR data.
 */
BspUicc_Lnk_ResetMgrAtrDecodedData bspUicc_Lnk_resetMgrAtrDecodedData;

/*=============================================================================
 * Description:
 *   Contains the Pps request
 */
BspUicc_Lnk_ResetMgrPpsData bspUicc_Lnk_resetMgrPpsDataRequest;

/*=============================================================================
 * Description:
 *   Contains the Pps response
 */
BspUicc_Lnk_ResetMgrPpsData bspUicc_Lnk_resetMgrPpsDataResponse;


/*==============================================================================
 * Description
 * This type specifies the if the TA(i) byte afet the first occurence of T=15
 * is present. refernce ISO/IEC 7816-3 section 6.5.1
 */
BspUicc_Lnk_ResetMgr_XiUiBytePresent bspUicc_Lnk_resetMgr_xiUiBytePresent=FALSE;


/*==============================================================================
 * Global  functions
 *============================================================================*/
Bool bspUicc_Lnk_ResetMgr_xiUiBytePresent(void)
{

    return (bspUicc_Lnk_resetMgr_xiUiBytePresent);

}

/*=============================================================================
 *   Private  functions
 *============================================================================*/


/*==============================================================================
 * Description
 This function computes Rx trigger based on F and D values and module frequency
  */

void bspUicc_Compute_RxTrigger()
{
	volatile SYS_UWORD16 fi;
	volatile SYS_UWORD16 di;
	volatile SYS_UWORD16 freq_divider;
	SYS_UWORD8	no_of_bytes;
	
	/*** Read F and F values from hardware registers ***/
	fi = BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_CONF_FI_PROG,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF5_REG_PTR);		
		

	di = BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_CONF_DI_PROG,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF5_REG_PTR);		
		
	freq_divider = BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_CONFSCLKDIV,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR);


	/*** BSP_UICC_MAX_INTERRUPT_LATENCY is set to 400micro seconds; etu = (F/D)*1/f, where F, D and f values are calculated
	based on the reister values. Multiplication factor BSP_UICC_TOTAL_NO_OF_ETUS_PER_BYTE represents 12 etus for each character. Addition of the expression
	((Fi_Ta1_Mapping[fi]*BSP_UICC_TOTAL_NO_OF_ETUS_PER_BYTE*bspUicc_Lnk_resetMgrFreqDivider[freq_divider])-1) is done to get the ceil value of the result  ***/
	no_of_bytes = (((BSP_UICC_MAX_INTERRUPT_LATENCY * Di_Ta1_Mapping[di] * BSP_UICC_MODULE_FREQUENCY) 
						+ ((Fi_Ta1_Mapping[fi]*BSP_UICC_TOTAL_NO_OF_ETUS_PER_BYTE*bspUicc_Lnk_resetMgrFreqDivider[freq_divider])-1))
						/ (Fi_Ta1_Mapping[fi]*BSP_UICC_TOTAL_NO_OF_ETUS_PER_BYTE*bspUicc_Lnk_resetMgrFreqDivider[freq_divider]));

	rx_trigger_value =	BSP_UICC_PHY_MAX_FIFO_SIZE - no_of_bytes;
	
}



/*=============================================================================
 * Description:
 *   Fetch the Presence of TA, TB, TC, and TD characters in the format byte
 *
 * Parameters
 *   resetDataPtr     -  Pointer to the ATR Data
 *
 * Returns
 *   status           -  Indicates the presence of interface characters in a character set
 *
 */
static BspUicc_Lnk_ResetMgrIntPresent
bspUicc_Lnk_resetMgrGetInterfaceCharPresence( BspUicc_Lnk_ResetData format )
{
    BspUicc_Lnk_ResetMgrIntPresent intCharsPresent;
    intCharsPresent.totalPresent = 0;

    if( (intCharsPresent.taPresent = BSP_UICC_LNK_MSG_UTIL_GET(
             BSP_UICC_LNK_MSG_UTIL_TDI_TA_PRESENCE,
             &format ) ) != 0 )
    {
        intCharsPresent.totalPresent++;
    }
    if( (intCharsPresent.tbPresent = BSP_UICC_LNK_MSG_UTIL_GET(
             BSP_UICC_LNK_MSG_UTIL_TDI_TB_PRESENCE,
             &format ) ) != 0 )
    {
        intCharsPresent.totalPresent++;
    }
    if( (intCharsPresent.tcPresent = BSP_UICC_LNK_MSG_UTIL_GET(
             BSP_UICC_LNK_MSG_UTIL_TDI_TC_PRESENCE,
             &format ) ) != 0 )
    {
        intCharsPresent.totalPresent++;
    }
    if( (intCharsPresent.tdPresent = BSP_UICC_LNK_MSG_UTIL_GET(
             BSP_UICC_LNK_MSG_UTIL_TDI_TD_PRESENCE,
             &format ) ) != 0 )
    {
        intCharsPresent.totalPresent++;
    }
    return intCharsPresent;
}


/*=============================================================================
 * Description:
 *   Initialize the Atr data structure
 *
 * Parameters
 *   None
 *
 * Returns
 *   None
 *
 */
static void
bspUicc_Lnk_resetMgrInitAtrData( void )
{
    bspUicc_Lnk_resetMgrAtrData.t0  = 0;
    bspUicc_Lnk_resetMgrAtrData.ta1 = 0;
    bspUicc_Lnk_resetMgrAtrData.tb1 = 0;
    bspUicc_Lnk_resetMgrAtrData.tc1 = 0;
    bspUicc_Lnk_resetMgrAtrData.td1 = 0;
    bspUicc_Lnk_resetMgrAtrData.ta2 = 0;
    bspUicc_Lnk_resetMgrAtrData.tb2 = 0;
    bspUicc_Lnk_resetMgrAtrData.tc2 = 0;
    bspUicc_Lnk_resetMgrAtrData.td2 = 0;
    bspUicc_Lnk_resetMgrAtrData.ta3 = 0;
    bspUicc_Lnk_resetMgrAtrData.tb3 = 0;
    bspUicc_Lnk_resetMgrAtrData.tc3 = 0;
    bspUicc_Lnk_resetMgrAtrData.td3 = 0;
    bspUicc_Lnk_resetMgrAtrData.ta4 = 0;
    bspUicc_Lnk_resetMgrAtrData.tb4 = 0;
    bspUicc_Lnk_resetMgrAtrData.tc4 = 0;
    bspUicc_Lnk_resetMgrAtrData.td4 = 0;
    bspUicc_Lnk_resetMgrAtrData.tck = 0;
    bspUicc_Lnk_resetMgrAtrDecodedData.firstProtocol = 0;
    bspUicc_Lnk_resetMgrAtrDecodedData.secondProtocol = 0;
    bspUicc_Lnk_resetMgrAtrDecodedData.extraGuardTime = 0;
    bspUicc_Lnk_resetMgrAtrDecodedData.clockStopInd = 0;
    bspUicc_Lnk_resetMgr_xiUiBytePresent = FALSE;
    bspUicc_Lnk_resetMgrAtrDecodedData.clkConvFactor = BSP_UICC_LNK_DATA_CONFIG_RESET_FI_VALUE;
    bspUicc_Lnk_resetMgrAtrDecodedData.baudConvFactor = BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE;
    bspUicc_Lnk_resetMgrAtrDecodedData.waitIValue = 0;

    /* Other Decoded Data params are not initialized,
       because the PS can ask the driver to reset, without
       a Power cycle being performed. In this case, the previously
       decoded data like the Power Level should be reused. */
}

/*=============================================================================
 * Description:
 *   Get the Initial Character TS
 *
 * Parameters
 *   resetDataPtr     -  Pointer to the ATR Data
 *
 * Returns
 *   status           -  Contains the total size of the data fetched
 *
 */
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrGetTS( BspUicc_Lnk_ResetData *resetDataPtr )
{
    BspUicc_Lnk_DataConfig          configReq;
    BspUicc_Phy_ReturnCode          bytesRead = 0;
    BspUicc_Lnk_ResetMgrReturnCode  status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS;

    bspUicc_Lnk_dataConfigGetPhyConfig( &configReq );

    /* read the first reset character TS */
    if( (bytesRead = bspUicc_Phy_read( BSP_UICC_LNK_MSG_UTIL_TS_NUM_OCTETS,
                                       resetDataPtr )) > 0 )
    {
        /* The read value has the code convention  */
        bspUicc_Lnk_resetMgrAtrData.ts = BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TS_CONVENTION,
                                                                    resetDataPtr );

        if( bspUicc_Lnk_resetMgrAtrData.ts == BSP_UICC_LNK_MSG_UTIL_TS_DIRECT_CONVENTION_VALUE )
        {
            configReq.convention = BSP_UICC_PHY_CONVENTION_DIRECT;
            status = bytesRead;

        }
        else if( bspUicc_Lnk_resetMgrAtrData.ts == BSP_UICC_LNK_MSG_UTIL_TS_INVERSE_CONVENTION_VALUE )
        {
            configReq.convention = BSP_UICC_PHY_CONVENTION_INVERSE;
	     bspUicc_Lnk_resetMgrAtrData.ts=BSP_UICC_LNK_MSG_UTIL_TS_INVERSE_INTERPRETED_VALUE;
	     *resetDataPtr	=BSP_UICC_LNK_MSG_UTIL_TS_INVERSE_INTERPRETED_VALUE;
            status = bytesRead;
        }
        else
        {
            status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
        }

    }
    else
    {
        bspUicc_Lnk_dataConfigGetPhyConfig( &configReq );
        status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    }

    /* Save the Code Convention */
    bspUicc_Lnk_dataConfigSetPhyConfig( &configReq );

    return status;
}

/*=============================================================================
 * Description:
 *   Get the Format Character T0, ( Character-T0 and T0 protocol are unrelated )
 *
 * Parameters
 *   resetDataPtr     -  Pointer to the ATR Data
 *
 * Returns
 *   status           -  Contains the total size of the data fetched
 *
 */
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrGetT0Chars( BspUicc_Lnk_ResetData *resetDataPtr )
{
    BspUicc_Lnk_ResetMgrIntPresent  intCharsPresent;
    BspUicc_Phy_ReturnCode          bytesRead = 0;
    BspUicc_Lnk_ResetMgrReturnCode  status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS;
    SYS_UWORD16                     waitIValue;



    if((bytesRead = bspUicc_Phy_read( BSP_UICC_LNK_MSG_UTIL_T0_NUM_OCTETS,
                                      resetDataPtr )) > 0 )
    {
        bspUicc_Lnk_resetMgrAtrData.t0 = *(resetDataPtr);

        bspUicc_Lnk_resetMgrAtrDecodedData.numHistoricBytes =
            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T0_NUM_HISTORICAL,
                                   &bspUicc_Lnk_resetMgrAtrData.t0 );

        intCharsPresent = bspUicc_Lnk_resetMgrGetInterfaceCharPresence( bspUicc_Lnk_resetMgrAtrData.t0 );

        /* Increment the pointer by T0 characters */
        resetDataPtr += bytesRead;

        if( intCharsPresent.totalPresent > 0 )
        {
            /* Get all the characters, indicated in TD1 as being present,
               at one time. This way the physical layer can make use of the Fifo
               and improve overall efficiency */
            if((bytesRead += bspUicc_Phy_read( intCharsPresent.totalPresent,
                                               resetDataPtr )) > 0)
            {
                if( intCharsPresent.taPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.ta1 = *(resetDataPtr);

                    bspUicc_Lnk_resetMgrAtrDecodedData.clkConvFactor =
                        BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TA1_FI,
                                                   resetDataPtr );

                    bspUicc_Lnk_resetMgrAtrDecodedData.baudConvFactor =
                        BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TA1_DI,
                                                   resetDataPtr );
                    resetDataPtr++;

                    /* There is no need to negotiate if the card supports
                       just the default F and D values */
                    if( ( bspUicc_Lnk_resetMgrAtrDecodedData.clkConvFactor !=
                          BSP_UICC_LNK_DATA_CONFIG_RESET_FI_VALUE ) ||
                        ( bspUicc_Lnk_resetMgrAtrDecodedData.baudConvFactor !=
                          BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE ) )
                    {
                        /* TA1 present, set the card in the Negotiable mode, as the card
                           is indicating it  supports some F and D values. If TA2 is present
                           after TA1, then the card will be set to specific mode and PPS will
                           not be done */
                        bspUicc_Lnk_resetMgrAtrDecodedData.atrMode
                            = BSP_UICC_LNK_RESET_MGR_ATR_MODE_NEGOTIABLE_MODE;
                    }

                }
                if( intCharsPresent.tbPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.tb1 = *(resetDataPtr);

                    bspUicc_Lnk_resetMgrAtrDecodedData.progCurrent =
                    BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TB1_PROG_CURRENT,
                                               resetDataPtr );

                    bspUicc_Lnk_resetMgrAtrDecodedData.progVoltage =
                    BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TB1_PROG_VOLTAGE,
                                               resetDataPtr );
                    resetDataPtr++;
                }
                if( intCharsPresent.tcPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.tc1 = *(resetDataPtr);

                    bspUicc_Lnk_resetMgrAtrDecodedData.extraGuardTime =
                        BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TC1_EXTRA_GUARD_TIME_REF,
                                                   resetDataPtr );
                    resetDataPtr++;
                }
                if( intCharsPresent.tdPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.td1 = *(resetDataPtr);

                    bspUicc_Lnk_resetMgrAtrDecodedData.firstProtocol =
                        BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL,
                                                   resetDataPtr );
                    resetDataPtr++;
                }
            }
        }
        status = bytesRead;
    }
    else
    {
        status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    }
    return status;
}

/*=============================================================================
 * Description:
 *   Fetch the TD1 interface Characters
 *
 * Parameters
 *   resetDataPtr     -  Pointer to the ATR Data
 *
 * Returns
 *   status           -  Contains the total size of the data fetched
 *
 */
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrGetTD1Chars( BspUicc_Lnk_ResetData *resetDataPtr )
{
    BspUicc_Lnk_ResetMgrReturnCode  status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    BspUicc_Lnk_ResetMgrIntPresent  intCharsPresent;
    SYS_UWORD8                           protocol = 0;
    BspUicc_Phy_ReturnCode          bytesRead = 0;

    resetDataPtr = resetDataPtr;

    if( bspUicc_Lnk_resetMgrAtrData.td1 )
    {
        protocol = BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL,
                                              &bspUicc_Lnk_resetMgrAtrData.td1 );

        intCharsPresent = bspUicc_Lnk_resetMgrGetInterfaceCharPresence( bspUicc_Lnk_resetMgrAtrData.td1 );

        if( intCharsPresent.totalPresent > 0 )
        {
            if( (bytesRead += bspUicc_Phy_read( intCharsPresent.totalPresent,
                                                resetDataPtr )) > 0 )
            {
                if( intCharsPresent.taPresent )
                {
                    /* TA2 Character is the Specific mode Character */
                    bspUicc_Lnk_resetMgrAtrData.ta2 = *(resetDataPtr);

                    bspUicc_Lnk_resetMgrAtrDecodedData.atrMode =
                        BSP_UICC_LNK_RESET_MGR_ATR_MODE_SPECIFIC_MODE;

                    bspUicc_Lnk_resetMgrAtrDecodedData.specificProtocol =
                        BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TA2_SPECIFIC_PROTOCOL,
                                                       resetDataPtr );

                    if( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TA2_MODE_CHANGE,
                                                   resetDataPtr ) ==
                        BSP_UICC_LNK_RESET_MGR_MODE_CHANGE_NOT_SUPPORTED_VALUE )
                    {
                        bspUicc_Lnk_resetMgrAtrDecodedData.modeChangeable =
                            BSP_UICC_LNK_RESET_MGR_MODE_CHANGEABLE_FALSE;
                    }

                    if( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TA2_PARAMETER_DEFN,
                                                   resetDataPtr ) ==
                        BSP_UICC_LNK_MSG_UTIL_TA2_PARAMETER_DEFN_IMPLICIT_VALUE )
                    {
                        bspUicc_Lnk_resetMgrAtrDecodedData.parameterDefn =
                            BSP_UICC_LNK_RESET_MGR_PARAMETER_DEFN_IMPLICIT;
                    }

                    resetDataPtr++;
                }
                else
                {
                    bspUicc_Lnk_resetMgrAtrDecodedData.atrMode =
                        BSP_UICC_LNK_RESET_MGR_ATR_MODE_NEGOTIABLE_MODE;
                }
                if( intCharsPresent.tbPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.tb2 = *(resetDataPtr);

                    if( protocol == BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_T0_VALUE )
                    {
                        /* Replace the prog voltage with the new value */
                        bspUicc_Lnk_resetMgrAtrDecodedData.progVoltage =
                            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TB2_PROG_VOLTAGE,
                                                       resetDataPtr );
                    }
                    else if( protocol == BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_T1_VALUE )
                    {
                        bspUicc_Lnk_resetMgrAtrDecodedData.charWaitInt =
                            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TBI_CHAR_WAIT_INT,
                                                       resetDataPtr );

                        bspUicc_Lnk_resetMgrAtrDecodedData.blockWaitInt =
                            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TBI_BLOCK_WAIT_INT,
                                                       resetDataPtr );
                    }
                    resetDataPtr++;
                }
                if( intCharsPresent.tcPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.tc2 = *(resetDataPtr);
                    resetDataPtr++;
                }
                if( intCharsPresent.tdPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.td2 = *(resetDataPtr);
                    resetDataPtr++;
                }
            }
        }
        status = bytesRead;
    }
    return status;
}

/*=============================================================================
 * Description:
 *   Fetch the TD2 interface Characters
 *
 * Parameters
 *   resetDataPtr     -  Pointer to the ATR Data
 *
 * Returns
 *   status           -  Contains the total size of the data fetched
 *
 */
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrGetTD2Chars( BspUicc_Lnk_ResetData *resetDataPtr )
{
    BspUicc_Lnk_ResetMgrReturnCode  status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    BspUicc_Lnk_ResetMgrIntPresent  intCharsPresent;
    SYS_UWORD8                           protocol = 0;
    BspUicc_Phy_ReturnCode          bytesRead = 0;

    resetDataPtr = resetDataPtr;

    if( bspUicc_Lnk_resetMgrAtrData.td2 )
    {
        protocol = BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL,
                                              &bspUicc_Lnk_resetMgrAtrData.td2 );

        if( protocol == BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_T1_VALUE )
        {
            /* The cars supports T=1 protocol, save this into decoded data */
            bspUicc_Lnk_resetMgrAtrDecodedData.secondProtocol = protocol;
        }


        intCharsPresent = bspUicc_Lnk_resetMgrGetInterfaceCharPresence( bspUicc_Lnk_resetMgrAtrData.td2 );

        if( intCharsPresent.totalPresent > 0 )
        {
            /* Get all the characters, indicated in TD2 as being present,
               at one time.  */
            if( (bytesRead += bspUicc_Phy_read( intCharsPresent.totalPresent,
                                                resetDataPtr )) > 0 )
            {
                if( intCharsPresent.taPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.ta3 = *(resetDataPtr);

                    if( protocol == BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_T1_VALUE )
                    {
                        bspUicc_Lnk_resetMgrAtrDecodedData.secondProtocol = protocol;
                        bspUicc_Lnk_resetMgrAtrDecodedData.blockSize =
                            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TAI_BLOCK_SIZE,
                                                       resetDataPtr );
                        /* Set the pending Config Data */
                        bspUicc_Lnk_dataConfigSetBlockSize( bspUicc_Lnk_resetMgrAtrDecodedData.blockSize );
                    }
                    else if( protocol == BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_T15_VALUE )
                    {
			   bspUicc_Lnk_resetMgr_xiUiBytePresent = TRUE;
                        bspUicc_Lnk_resetMgrAtrDecodedData.cardClass =
                            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TAI_CLASS_REF,
                                                       resetDataPtr );

                        bspUicc_Lnk_resetMgrAtrDecodedData.clockStopInd =
                            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TAI_CLOCK_STOP_REF,
                                                       resetDataPtr );
                        /* Set the pending Config Data */
                        bspUicc_Lnk_dataConfigSetClkStopInd( bspUicc_Lnk_resetMgrAtrDecodedData.clockStopInd );
                    }
                    (resetDataPtr)++;
                }
                if( intCharsPresent.tbPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.tb3 = *(resetDataPtr);
                    if( protocol == BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_T1_VALUE )
                    {
                        bspUicc_Lnk_resetMgrAtrDecodedData.charWaitInt =
                            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TBI_CHAR_WAIT_INT,
                                                   resetDataPtr );

                        bspUicc_Lnk_resetMgrAtrDecodedData.blockWaitInt =
                            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TBI_BLOCK_WAIT_INT,
                                                   resetDataPtr );
                    }
                    resetDataPtr++;
                }
                if( intCharsPresent.tcPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.tc3 = *(resetDataPtr);
                    resetDataPtr++;
                }
                if( intCharsPresent.tdPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.td3 = *(resetDataPtr);
                    resetDataPtr++;
                }
            }
        }
        status = bytesRead;
    }
    return status;
}

/*=============================================================================
 * Description:
 *   Fetch the TD3 interface Characters
 *
 * Parameters
 *   resetDataPtr     -  Pointer to the ATR Data
 *
 * Returns
 *   status           -  Contains the total size of the data fetched
 *
 */
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrGetTD3Chars( BspUicc_Lnk_ResetData *resetDataPtr )
{
    BspUicc_Lnk_ResetMgrReturnCode  status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    BspUicc_Lnk_ResetMgrIntPresent  intCharsPresent;
    SYS_UWORD8                        protocol = 0;
    BspUicc_Phy_ReturnCode          bytesRead = 0;

    resetDataPtr = resetDataPtr;

    if( bspUicc_Lnk_resetMgrAtrData.td3 )
    {
        protocol = BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL,
                                              &bspUicc_Lnk_resetMgrAtrData.td3 );

        intCharsPresent = bspUicc_Lnk_resetMgrGetInterfaceCharPresence( bspUicc_Lnk_resetMgrAtrData.td3 );

        if( intCharsPresent.totalPresent > 0 )
        {
            if( (bytesRead += bspUicc_Phy_read( intCharsPresent.totalPresent,
                                                resetDataPtr )) > 0 )
            {
                if( intCharsPresent.taPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.ta4 = *(resetDataPtr);

                    if( protocol == BSP_UICC_LNK_MSG_UTIL_TDI_PROTOCOL_T15_VALUE )
                    {
 			   bspUicc_Lnk_resetMgr_xiUiBytePresent = TRUE;
                        bspUicc_Lnk_resetMgrAtrDecodedData.cardClass =
                            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TAI_CLASS_REF,
                                                       resetDataPtr );

                        bspUicc_Lnk_resetMgrAtrDecodedData.clockStopInd =
                            BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_TAI_CLOCK_STOP_REF,
                                                       resetDataPtr );
                        /* Set the pending Config Data */
                        bspUicc_Lnk_dataConfigSetClkStopInd( bspUicc_Lnk_resetMgrAtrDecodedData.clockStopInd );
                    }

                    (resetDataPtr)++;
                }
                if( intCharsPresent.tbPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.tb4 = *(resetDataPtr);
                    resetDataPtr++;
                }
                if( intCharsPresent.tcPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.tc4 = *(resetDataPtr);
                    resetDataPtr++;
                }
                if( intCharsPresent.tdPresent )
                {
                    bspUicc_Lnk_resetMgrAtrData.td4 = *(resetDataPtr);
                    resetDataPtr++;
                }
            }
        }
        status = bytesRead;
    }
    return status;
}

/*=============================================================================
 * Description:
 *   Fetch the Historic Bytes
 *
 * Parameters
 *   resetDataPtr     -  Pointer to the ATR Data
 *
 * Returns
 *   status           -  Contains the total size of the data fetched
 *
 */
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrGetHistoricBytes( BspUicc_Lnk_ResetData *resetDataPtr )
{
    SYS_UWORD8                           numHistBytes;
    BspUicc_Phy_ReturnCode          bytesRead = 0;
    BspUicc_Lnk_ResetMgrReturnCode  status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS;


    resetDataPtr = resetDataPtr;

    numHistBytes = BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T0_NUM_HISTORICAL,
                                          &bspUicc_Lnk_resetMgrAtrData.t0 );


    if( (bytesRead = bspUicc_Phy_read( numHistBytes,
                                          resetDataPtr )) > 0 )
    {
        status = bytesRead;
    }
    else
    {
        status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    }

    return status ;
}

/*=============================================================================
 * Description:
 *   Function to Get the check Byte
 *
 * Parameters
 *   resetDataPtr     -  Pointer to the ATR Data
 *
 * Returns
 *   status           -  Contains the total size of the data fetched
 *
 */
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrGetCheckByte( BspUicc_Lnk_ResetData *resetDataPtr )
{
    BspUicc_Lnk_ResetMgrReturnCode  status;
    BspUicc_Phy_ReturnCode          bytesRead = 0;

    resetDataPtr = resetDataPtr;

    if( (bytesRead = bspUicc_Phy_read( BSP_UICC_LNK_MSG_UTIL_TCK_NUM_OCTETS,
                                       resetDataPtr )) > 0 )
    {
        status = bytesRead;
        bspUicc_Lnk_resetMgrAtrData.tck = *(resetDataPtr);
    }
    else
    {
        status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    }

    return status ;
}

/*=============================================================================
 * Description:
 *   Compute if the ATR is valid using the check byte
 *
 * Parameters
 *   resetDataPtr     - Pointer to the ATR Data
 *   totalAtrSize     - Total ATR Size
 *
 * Returns
 *   Status           - Indicates error detection check pass/fail status
 *
 */
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrCheckAtrChars( BspUicc_Lnk_ResetData             *resetDataPtr,
                                   BspUicc_Lnk_ResetMgrTotalAtrSize  totalAtrSize )
{
    BspUicc_Lnk_ResetMgrReturnCode  status;
    SYS_UWORD8 i;
    SYS_UWORD8 result = 0;

    resetDataPtr++; /* Skip the First Character */

    for( i=0; i<(totalAtrSize - 1); i++ )
    {
        /* TS character is not included in Error check */
        result ^= *resetDataPtr;
        resetDataPtr++;
    }
    if( result == NULL )
    {
        status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS;
    }
    else
    {
        status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    }

    return status;
}

/*=============================================================================
 * Description:
 *   Build and Send the PPS request message to the physical layer
 *
 * Parameters
 *   protocol       - Requested protocol
 *   clkConvFactor  - Requested Clock rate conversion Factor
 *   baudConvFactor - Requested Baud rate conversion factor
 *
 * Returns
 *   Status         - Indicates suceess or failure in sending the PPS request
 *
 */
#ifndef FEATURE_UICC_DISABLE_PPS
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrSendPpsRequest( BspUicc_Lnk_ResetMgr_Protocol      protocol,
                                    BspUicc_Lnk_ResetMgrClkConvFactor  clkConvFactor,
                                    BspUicc_Lnk_ResetMgrBaudConvFactor baudConvFactor )
{
    BspUicc_Lnk_ResetMgrReturnCode  status;
    BspUicc_Lnk_ResetData           checkByte = 0;
    BspUicc_Phy_ReturnCode          bytesSent;
    SYS_UWORD8                           i;
    BOOL invalidFDvaule=FALSE;
    if ((clkConvFactor==0x7)|| (clkConvFactor==0x8) || (clkConvFactor==0xE)|| (clkConvFactor==0xF)
        || (baudConvFactor==0x0) || (baudConvFactor==0x7)|| (baudConvFactor==0xA)|| (baudConvFactor==0xB)
        || (baudConvFactor==0xC) || (baudConvFactor==0xD) || (baudConvFactor==0xE) || (baudConvFactor==0xF))
		invalidFDvaule=TRUE;

    if (invalidFDvaule==FALSE)
    	{

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_PPSS,
                               ppsRequest,
                               BSP_UICC_LNK_MSG_UTIL_PPSS_VALUE );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_PPS0_PROTOCOL,
                               ppsRequest,
                               protocol );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_PPS0_PPS1_PRESENCE,
                               ppsRequest,
                               BSP_UICC_LNK_MSG_UTIL_PPS0_PRESENCE_VALUE_PRESENT );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_PPS0_PPS2_PRESENCE,
                               ppsRequest,
                               BSP_UICC_LNK_MSG_UTIL_PPS0_PRESENCE_VALUE_NOT_PRESENT );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_PPS0_PPS3_PRESENCE,
                               ppsRequest,
                               BSP_UICC_LNK_MSG_UTIL_PPS0_PRESENCE_VALUE_NOT_PRESENT );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_PPS0_PPS4_PRESENCE,
                               ppsRequest,
                               BSP_UICC_LNK_MSG_UTIL_PPS0_PRESENCE_VALUE_NOT_PRESENT );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_PPS1_DI,
                               ppsRequest,
                               baudConvFactor );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_PPS1_FI,
                               ppsRequest,
                               clkConvFactor );

    /* Exor the first 3 bytes and store it in the check byte */
    checkByte = 0;
    for( i = 0; i<(BSP_UICC_LNK_RESET_MGR_PPS_REQUEST_SIZE - 1); i++)
    {
        checkByte ^= ppsRequest[i];
    }

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_PCK,
                               ppsRequest,
                               checkByte );

	    bspUicc_Lnk_resetMgrPpsDataRequest.ppss = ppsRequest[BSP_UICC_LNK_MSG_UTIL_PPSS_OCTET_OFFSET];
	    bspUicc_Lnk_resetMgrPpsDataRequest.pps0 = ppsRequest[BSP_UICC_LNK_MSG_UTIL_PPS0_OCTET_OFFSET];
	    bspUicc_Lnk_resetMgrPpsDataRequest.pps1 = ppsRequest[BSP_UICC_LNK_MSG_UTIL_PPS1_OCTET_OFFSET];
	    bspUicc_Lnk_resetMgrPpsDataRequest.pck  = ppsRequest[BSP_UICC_LNK_MSG_UTIL_PCK_OCTET_OFFSET];
	    pps_request_data_size=BSP_UICC_LNK_RESET_MGR_PPS_REQUEST_SIZE;
    	}

	else
	{
	    bspUicc_Lnk_resetMgrPpsDataRequest.ppss = ppsRequest[BSP_UICC_LNK_MSG_UTIL_PPSS_OCTET_OFFSET]=0xFF;
	    bspUicc_Lnk_resetMgrPpsDataRequest.pps0 = ppsRequest[BSP_UICC_LNK_MSG_UTIL_PPS0_OCTET_OFFSET]=0x00;
	    bspUicc_Lnk_resetMgrPpsDataRequest.pck  = ppsRequest[BSP_UICC_LNK_MSG_UTIL_PPS1_OCTET_OFFSET]=0xFF;
	    pps_request_data_size=BSP_UICC_LNK_RESET_MGR_PPS_REQUEST_SIZE-1;
    	}

    /* Send the PPS Request */
    if( (bytesSent = bspUicc_Phy_write( pps_request_data_size,
                                        ppsRequest)) ==
        pps_request_data_size )
    {
        status = bytesSent;
    }
    else
    {
         status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    }
    return status;
}
#endif

/*=============================================================================
 * Description:
 *   Receive the PPS response message from the physical layer
 *
 * Parameters
 *   None
 *
 * Returns
 *   Status   - Indicates suceess or failure to get any PPS response
 *
 */
#ifndef FEATURE_UICC_DISABLE_PPS
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrGetPpsResponse( void )
{
    BspUicc_Lnk_ResetMgrReturnCode  status;
    BspUicc_Lnk_ResetData           checkByte;
    BspUicc_Phy_ReturnCode          bytesRead, totalBytesRead;
    SYS_UWORD8                           i;
    SYS_UWORD8                           rxDataSize;
    BspUicc_Lnk_ResetData           *ppsResponsePtr;

    status =  BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS;
    ppsResponsePtr = ppsResponse;
    checkByte = 0;

    /* Get PPSS and PPS0 Characters */
    rxDataSize = ( BSP_UICC_LNK_MSG_UTIL_PPSS_NUM_OCTETS + BSP_UICC_LNK_MSG_UTIL_PPS0_NUM_OCTETS );

    if( (bytesRead = bspUicc_Phy_read( rxDataSize,
                                       ppsResponsePtr )) > 0 )
    {
        /* Increment the receive data pointer */
        ppsResponsePtr += bytesRead;
        totalBytesRead = bytesRead;

        /* Store the pps response in the static variable */
        bspUicc_Lnk_resetMgrPpsDataResponse.ppss = ppsResponse[BSP_UICC_LNK_MSG_UTIL_PPSS_OCTET_OFFSET];
        bspUicc_Lnk_resetMgrPpsDataResponse.pps0 = ppsResponse[BSP_UICC_LNK_MSG_UTIL_PPS0_OCTET_OFFSET];

        if(( bspUicc_Lnk_resetMgrPpsDataResponse.ppss == bspUicc_Lnk_resetMgrPpsDataRequest.ppss ) &&
           ( BSP_UICC_LNK_MSG_UTIL_GET(BSP_UICC_LNK_MSG_UTIL_PPS0_PROTOCOL,
                                          ppsResponse)==
              BSP_UICC_LNK_MSG_UTIL_GET(BSP_UICC_LNK_MSG_UTIL_PPS0_PROTOCOL,
                                          ppsRequest) ))
        {
            if( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_PPS0_PPS1_PRESENCE,
                                           ppsResponse ) ==
                BSP_UICC_LNK_MSG_UTIL_PPS0_PRESENCE_VALUE_PRESENT )
            {
                /* Read the rest of the chars only if present */
                rxDataSize = ( BSP_UICC_LNK_MSG_UTIL_PPS1_NUM_OCTETS + BSP_UICC_LNK_MSG_UTIL_PCK_NUM_OCTETS );

                bytesRead =  bspUicc_Phy_read( rxDataSize,
                                                ppsResponsePtr );

               if(bytesRead > 0)
               {
                   totalBytesRead += bytesRead;

                    bspUicc_Lnk_resetMgrPpsDataResponse.pps1 = ppsResponse[BSP_UICC_LNK_MSG_UTIL_PPS1_OCTET_OFFSET];
                    bspUicc_Lnk_resetMgrPpsDataResponse.pck  = ppsResponse[BSP_UICC_LNK_MSG_UTIL_PCK_OCTET_OFFSET];
                }
                else
                {
                        status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
                }
            }
            else
            {

                bytesRead =  bspUicc_Phy_read( BSP_UICC_LNK_MSG_UTIL_PCK_NUM_OCTETS,
                                                 ppsResponsePtr );
                if(bytesRead > 0)
                {
                    totalBytesRead +=  bytesRead;

                    bspUicc_Lnk_resetMgrPpsDataResponse.pps1 = 0x11;
                    bspUicc_Lnk_resetMgrPpsDataResponse.pck  = ppsResponse[BSP_UICC_LNK_MSG_UTIL_PCK_OCTET_OFFSET-1];
                    /* set default F and D values if PPS1 is absent */
                    /* BSP_UICC_LNK_MSG_UTIL_SET(
                        BSP_UICC_LNK_MSG_UTIL_PPS1_DI,
                        ppsResponse,
                        BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE));

                    BSP_UICC_LNK_MSG_UTIL_SET(
                        BSP_UICC_LNK_MSG_UTIL_PPS1_FI,
                        ppsResponse,
                        BSP_UICC_LNK_DATA_CONFIG_RESET_FI_VALUE));*/
                }
                else
                {
                     status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
                }
            }

            if( status ==  BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS)
            {
                /* Check for correct reception of the PPS characters */
                for(i=0; i<pps_request_data_size; i++)
                {
                    checkByte ^= ppsResponse[i];
                }

                if( checkByte )
                {
                    status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
                }
                else
                {
                    status = totalBytesRead;
                }
            }
        }
        else
        {
            status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
        }
    }
    else
    {
        status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    }

    return status;
}
#endif

/*=============================================================================
 * Description:
 *   Negotiate the mode of the card
 *
 * Parameters
 *   None
 *
 * Returns
 *   Status   - Indicates suceess or failure to get any PPS characters
 *
 */
#ifndef FEATURE_UICC_DISABLE_PPS
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrNegotiateMode(  BspUicc_Lnk_ResetMgrClkConvFactor  clkConvFactor,
                                    BspUicc_Lnk_ResetMgrBaudConvFactor baudConvFactor,
                                    BspUicc_Lnk_ResetMgr_Protocol      protocol
                                    )
{
    BspUicc_Lnk_ResetMgrReturnCode     status;
    BspUicc_Lnk_ResetMgr_Protocol      requestProtocol;
    BspUicc_Lnk_ResetMgr_Protocol      responseProtocol;
    BspUicc_Lnk_ResetMgrClkConvFactor  requestClkConvFactor;
    BspUicc_Lnk_ResetMgrBaudConvFactor requestBaudConvFactor;
    BspUicc_Phy_SclkDiv                atrFreqVal = 0;
    SYS_UWORD16                        waitIValue;
    SYS_UWORD8                         i;

    /* initialize PPS variables */
    bspUicc_Lnk_resetMgrPpsDataRequest.ppss = 0;
    bspUicc_Lnk_resetMgrPpsDataRequest.pps0 = 0;
    bspUicc_Lnk_resetMgrPpsDataRequest.pps1 = 0;
    bspUicc_Lnk_resetMgrPpsDataRequest.pck  = 0;
    bspUicc_Lnk_resetMgrPpsDataResponse.ppss = 0;
    bspUicc_Lnk_resetMgrPpsDataResponse.pps0 = 0;
    bspUicc_Lnk_resetMgrPpsDataResponse.pps1 = 0;
    bspUicc_Lnk_resetMgrPpsDataResponse.pck  = 0;
    for(i=0; i<BSP_UICC_LNK_RESET_MGR_PPS_RESPONSE_SIZE; i++)
    {
        ppsResponse[i] = 0;
    }
     for(i=0; i<BSP_UICC_LNK_RESET_MGR_PPS_REQUEST_SIZE; i++)
    {
        ppsRequest[i] = 0;
    }

    /* Send a PPS request with the Protocol, F and D from the ATR */
#ifndef FEATURE_UICC_DISABLE_T1_PROTOCOL
        requestProtocol = protocol;
#else
        requestProtocol = BSP_UICC_LNK_RESET_MGR_T0_PROTOCOL;
#endif

#ifndef FEATURE_UICC_DISABLE_SPEED_ENHANCEMENT
    requestClkConvFactor = clkConvFactor;
    requestBaudConvFactor = baudConvFactor;
#else
    requestClkConvFactor = BSP_UICC_LNK_DATA_CONFIG_RESET_FI_VALUE;
    requestBaudConvFactor = BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE;
#endif

    status = bspUicc_Lnk_resetMgrSendPpsRequest( requestProtocol,
                                        requestClkConvFactor,
                                        requestBaudConvFactor );
	/*status contains the no. of bytes written to the card */
    if(status >0)
    {
        status = bspUicc_Lnk_resetMgrGetPpsResponse();
		/*status contains the no. of bytes read from the card */
        if(status >0)
        {
            responseProtocol = BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_PPS0_PROTOCOL,
                                                          ppsResponse );

            if( bspUicc_Lnk_resetMgrPpsDataRequest.ppss == bspUicc_Lnk_resetMgrPpsDataResponse.ppss )
            {
                if( requestProtocol == responseProtocol )
                {
                    bspUicc_Lnk_dataConfigSetProtocol( responseProtocol );
                }
                else
                {
                    bspUicc_Lnk_dataConfigSetProtocol( BSP_UICC_LNK_DATA_CONFIG_RESET_PROTOCOL );
                }

                if(( bspUicc_Lnk_resetMgrPpsDataRequest.pps1 > 0) &&
                   ( bspUicc_Lnk_resetMgrPpsDataRequest.pps1 ==  bspUicc_Lnk_resetMgrPpsDataResponse.pps1))
                {

                    bspUicc_Lnk_dataConfigSetFiValue(requestClkConvFactor);
                    bspUicc_Lnk_dataConfigSetDiValue(requestBaudConvFactor);
                    /* Max Freq for indicated Fi */
                    atrFreqVal = bspUicc_Lnk_resetMgrFreqValues[ requestClkConvFactor ];
                }
                else
                {
                    bspUicc_Lnk_dataConfigSetFiValue( BSP_UICC_LNK_DATA_CONFIG_RESET_FI_VALUE );
                    bspUicc_Lnk_dataConfigSetDiValue( BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE );
                    /* Max Freq for indicated Fi */
                    atrFreqVal = bspUicc_Lnk_resetMgrFreqValues[ BSP_UICC_LNK_DATA_CONFIG_RESET_FI_VALUE ];
                }

                bspUicc_Lnk_dataConfigSetSclkDiv( atrFreqVal );
                status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS;
            }
            else
            {
                status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
            }

        }
        else /* Specific Mode and Specific Protocol */
        {
             status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
        }
    }
    return status;
}
#endif

/*=============================================================================
 * Description:
 *   Function to Get the all the Atr characters
 *
 * Parameters
 *   resetDataPtr     -  Pointer to the ATR Data
 *
 * Returns
 *   status           -  PASS/FAILURE
 *
 */
static BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrGetAtrChars( BspUicc_Lnk_ResetData    *resetDataPtr )
{
    BspUicc_Lnk_ResetMgrReturnCode  status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS;
    BspUicc_Lnk_ResetData           *dataPtr;
    BspUicc_Phy_ReturnCode          bytesRead = 0;
    SYS_UWORD8                           checkBytePresenceFlag;

    bspUicc_Lnk_resetMgrAtrDecodedData.totalAtrSize = 0;
    checkBytePresenceFlag = 0;
    bspUicc_Lnk_resetMgrInitAtrData();

    dataPtr = resetDataPtr;

    bytesRead = bspUicc_Lnk_resetMgrGetTS( dataPtr );

    if( bytesRead > 0 )
    {
        /* Increment pointer by TS characters*/
        dataPtr += bytesRead;
        bspUicc_Lnk_resetMgrAtrDecodedData.totalAtrSize += bytesRead;


        bytesRead = bspUicc_Lnk_resetMgrGetT0Chars( dataPtr );
        bspUicc_Lnk_resetMgrAtrDecodedData.totalAtrSize += bytesRead;
        dataPtr += bytesRead;

        /* Check if a TD(i) character was present in the bytes read */
        if( bytesRead > 0 )
        {
            if( bspUicc_Lnk_resetMgrAtrData.td1 != 0 )
            {
                bytesRead = bspUicc_Lnk_resetMgrGetTD1Chars( dataPtr );
                dataPtr += bytesRead;
                bspUicc_Lnk_resetMgrAtrDecodedData.totalAtrSize += bytesRead;

                if( bspUicc_Lnk_resetMgrAtrData.td2 != 0 )
                {
	 	   //According to ISO7816-3, if protocol type T always equals 0, TCK will not be sent by UICC.
			if ( (bspUicc_Lnk_resetMgrAtrData.td2 & 0x0f) !=0)
				checkBytePresenceFlag = 1;

                    bytesRead = bspUicc_Lnk_resetMgrGetTD2Chars( dataPtr );
                    dataPtr += bytesRead;
                    bspUicc_Lnk_resetMgrAtrDecodedData.totalAtrSize += bytesRead;

                    if( bspUicc_Lnk_resetMgrAtrData.td3 != 0 )
                    {
	 	   //According to ISO7816-3, if protocol type T always equals 0, TCK will not be sent by UICC.
			if ( (bspUicc_Lnk_resetMgrAtrData.td3 & 0x0f) !=0)
				checkBytePresenceFlag = 1;

                        bytesRead = bspUicc_Lnk_resetMgrGetTD3Chars( dataPtr );
                        dataPtr += bytesRead;
                        bspUicc_Lnk_resetMgrAtrDecodedData.totalAtrSize += bytesRead;
                    }
                }
            }

            /* Get the Historic bytes */
            if( bspUicc_Lnk_resetMgrAtrDecodedData.numHistoricBytes > 0 )
            {
                    bytesRead = bspUicc_Lnk_resetMgrGetHistoricBytes( dataPtr );
                    dataPtr += bytesRead;
                    bspUicc_Lnk_resetMgrAtrDecodedData.totalAtrSize += bytesRead;
            }

            if( checkBytePresenceFlag )
            {
                bytesRead = bspUicc_Lnk_resetMgrGetCheckByte( dataPtr );

                dataPtr += bytesRead;
                bspUicc_Lnk_resetMgrAtrDecodedData.totalAtrSize += bytesRead;

                if( bspUicc_Lnk_resetMgrCheckAtrChars( resetDataPtr,
                                                       bspUicc_Lnk_resetMgrAtrDecodedData.totalAtrSize )
                    == BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE )
                {
                    status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
                }
            }
        }
    }
    else
    {
        /* The Physical layer read contains the failure code,
         return the failure code */
        status = bytesRead;
    }

    if (bspUicc_Lnk_resetMgrAtrData.tc1!=0 && bspUicc_Lnk_resetMgrAtrData.tc1!=255)
    	{
    	bspUicc_Lnk_resetMgrInitAtrData();
       status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE;
    	}

    return status;
}


/*=============================================================================
 *   Public Functions
 *============================================================================*/

BspUicc_Lnk_ResetMgrCardClass bspUicc_Lnk_currentCardClass;

/*=============================================================================
 * Description:
 *   Start ATR reception,
 */
BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrStartAtr( BspUicc_Lnk_ResetData      *resetDataPtr,
                              BspUicc_Lnk_ResetDataSize  *resetDataSizePtr,
                              BspUicc_Lnk_VoltageSelect  voltageSelect,
                              BspUicc_Lnk_ResetNewVoltClass  resetNewVoltClass,
                              BspUicc_Lnk_VoltClass       newVoltClass)

{
    BspUicc_Lnk_ResetMgrReturnCode  status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS;
    BspUicc_Lnk_ResetMgrReturnCode   ppsStatus =
                                     BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS;


    BspUicc_Lnk_DataConfig           configReq;
    SYS_UWORD8                            atrAttempts=0;
    SYS_UWORD8                            ppsAttempts=0;
    SYS_BOOL                             atrRetry = TRUE;
    SYS_BOOL                             ppsRetry = TRUE;
    SYS_UWORD8                        maxAtrAttempts=0;
    BspUicc_Lnk_ClkStopPreference    prevClkStopPref;
    BspUicc_Lnk_VoltClass            prevVoltClass;
    BspUicc_Lnk_ResetMgrClkConvFactor  requestClkConvFactor=1;
    BspUicc_Lnk_ResetMgrBaudConvFactor requestBaudConvFactor=1;
    BspUicc_Lnk_ResetMgr_Protocol      requestProtocol=0;

    if( voltageSelect ==  BSP_UICC_LNK_VOLTAGE_SELECT_REQUIRED)
    {
        if((resetNewVoltClass == TRUE) &&
           (newVoltClass == BSP_UICC_PHY_POWER_LEVEL_3))
        {
                bspUicc_Phy_powerOff();
                bspUicc_Phy_init();
                bspUicc_Power_increase();
                bspUicc_Lnk_currentCardClass= newVoltClass;

                /* Set the pending Config Data */
                bspUicc_Lnk_dataConfigSetPowerLevel(newVoltClass);

                maxAtrAttempts = BSP_UICC_LNK_RESET_MGR_ATR_ATTEMPTS_PER_POWERLEVEL;

        }
        else
        {
            bspUicc_Power_on();
            /* First attempt Atr for a 1.8V card */
            bspUicc_Lnk_currentCardClass = BSP_UICC_PHY_POWER_LEVEL_18;
            maxAtrAttempts = BSP_UICC_LNK_RESET_MGR_MAX_ATR_ATTEMPTS;
        }
    }

    else
	{

        if(bspUicc_Lnk_dataConfigGetPrevVoltClass() == BSP_UICC_PHY_POWER_LEVEL_18)
        {
            bspUicc_Power_on();
            /* First attempt Atr for a 1.8V card */
            bspUicc_Lnk_currentCardClass = BSP_UICC_PHY_POWER_LEVEL_18;
            bspUicc_Lnk_dataConfigSetPowerLevel(BSP_UICC_PHY_POWER_LEVEL_18);
            maxAtrAttempts = BSP_UICC_LNK_RESET_MGR_MAX_ATR_ATTEMPTS;
        }
        else if(bspUicc_Lnk_dataConfigGetPrevVoltClass() == BSP_UICC_PHY_POWER_LEVEL_3)
        {
            bspUicc_Phy_powerOff();
            bspUicc_Phy_init();
            bspUicc_Power_increase();
            bspUicc_Lnk_dataConfigSetPowerLevel(BSP_UICC_PHY_POWER_LEVEL_3);
            bspUicc_Lnk_currentCardClass = BSP_UICC_PHY_POWER_LEVEL_3;
            maxAtrAttempts = BSP_UICC_LNK_RESET_MGR_ATR_ATTEMPTS_PER_POWERLEVEL;
        }

    }

    for(atrAttempts = 0;
        ((atrAttempts < maxAtrAttempts )&&
         (atrRetry == TRUE));
         atrAttempts++)
    {
        bspUicc_Phy_init();

        /* Reset the Card */
        bspUicc_Phy_reset();

        status = bspUicc_Lnk_resetMgrGetAtrChars(resetDataPtr);
        bspUicc_Phy_setMode( BSP_UICC_PHY_MODE_PPS );


        if( status == BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS )
        {
            atrRetry = FALSE;
            *resetDataSizePtr =  bspUicc_Lnk_resetMgrAtrDecodedData.totalAtrSize;
            /* Voltage negotiation */
            if( ( ( bspUicc_Lnk_resetMgrAtrDecodedData.cardClass ==
                    BSP_UICC_PHY_POWER_LEVEL_3) ||
                  ( bspUicc_Lnk_resetMgrAtrDecodedData.cardClass ==
                    BSP_UICC_PHY_POWER_LEVEL_3_OR_5 ) ) &&
                  ( bspUicc_Lnk_currentCardClass ==
                    BSP_UICC_PHY_POWER_LEVEL_18 ) )
            {
                /* Card has responded for 1.8 V, but indicates that it is a 3V only card.
                   Increase the power to 3V and attempt an ATR */
                bspUicc_Phy_powerOff();

                bspUicc_Phy_init();
//                bspUicc_Power_on();
                bspUicc_Power_increase();

                /* Set the pending Config Data */
                bspUicc_Lnk_dataConfigSetPowerLevel( BSP_UICC_PHY_POWER_LEVEL_3 );

                bspUicc_Lnk_currentCardClass = BSP_UICC_PHY_POWER_LEVEL_3;

                /* Attempt ATR again */
                atrRetry = TRUE;
                atrAttempts = 0;
            }
        }
        else if( ( atrRetry == TRUE ) &&
                 ( atrAttempts == BSP_UICC_LNK_RESET_MGR_ATR_ATTEMPTS_PER_POWERLEVEL ) )
        {
            bspUicc_Phy_powerOff();
            bspUicc_Phy_init();
//            bspUicc_Power_on();
            bspUicc_Power_increase();

            bspUicc_Lnk_dataConfigSetPowerLevel( BSP_UICC_PHY_POWER_LEVEL_3 );
            bspUicc_Lnk_currentCardClass = BSP_UICC_PHY_POWER_LEVEL_3;
        }

    }


  if( atrRetry == TRUE )
  {
    /* ATR Fails. */
        bspUicc_Phy_powerOff();
        /*  Set the card as not inserted */
        if(  bspUicc_Phy_getCardInsertStatus() == BSP_UICC_PHY_LLIF_STATNOCARD_PRESENT )
        {
            bspUicc_Phy_toggleCardInsertStatus();
        }
    }
    else
    {
    /* ATR Succeeds.*/

    	/*Mark the card as present */
        if(  bspUicc_Phy_getCardInsertStatus() == BSP_UICC_PHY_LLIF_STATNOCARD_ABSENT )
        {
            bspUicc_Phy_toggleCardInsertStatus();
        }
#ifndef FEATURE_UICC_DISABLE_PPS

    /* If the Cards comes up in a Specific mode we need to try a warm reset */
    // atrRetry = TRUE;

    /* Continue with Warm Reset if ATR is sucessful and
       the ATR indicates that Card is in Specific mode.
       Warm Reset puts the card back into a negotiable mode
       that allows us to negotiate the speed and protocol.
       For the driver to be able to negotiate, the card should
       also indicate that changing modes is allowed. */


    if( (bspUicc_Lnk_resetMgrAtrDecodedData.atrMode ==
         BSP_UICC_LNK_RESET_MGR_ATR_MODE_SPECIFIC_MODE)
        && (status == BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS)
        && (bspUicc_Lnk_resetMgrAtrDecodedData.modeChangeable ==
            BSP_UICC_LNK_RESET_MGR_MODE_CHANGEABLE_TRUE) )
    {
        atrRetry = TRUE;

        for( atrAttempts = 0;
         ( (atrAttempts < BSP_UICC_LNK_RESET_MGR_WARM_RESET_ATTEMPTS) && (atrRetry == TRUE) );
         atrAttempts++ )
        {
            bspUicc_Phy_init();

            /* Perform a warm Reset */
            bspUicc_Phy_warmReset();

            status = bspUicc_Lnk_resetMgrGetAtrChars(resetDataPtr);

           if( bspUicc_Lnk_resetMgrAtrDecodedData.atrMode ==
                BSP_UICC_LNK_RESET_MGR_ATR_MODE_NEGOTIABLE_MODE )
            {
                atrRetry = FALSE;
            }
        }
     }

    if (bspUicc_Lnk_resetMgrAtrDecodedData.atrMode == BSP_UICC_LNK_RESET_MGR_ATR_MODE_NEGOTIABLE_MODE )
    {
	if ((bspUicc_Lnk_resetMgrAtrDecodedData.clkConvFactor==BSP_UICC_LNK_DATA_CONFIG_RESET_FI_VALUE) &&  (bspUicc_Lnk_resetMgrAtrDecodedData.baudConvFactor==BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE) )
	{
		bspUicc_Lnk_resetMgrAtrDecodedData.atrMode=BSP_UICC_LNK_RESET_MGR_ATR_MODE_SPECIFIC_MODE;
    	}
    }

    if( bspUicc_Lnk_resetMgrAtrDecodedData.atrMode ==
        BSP_UICC_LNK_RESET_MGR_ATR_MODE_NEGOTIABLE_MODE )
    {
            for(ppsAttempts = 0;
                    ((ppsAttempts <= BSP_UICC_LNK_RESET_MGR_MAX_PPS_ATTEMPTS )&&
                     (ppsRetry == TRUE));
                    ppsAttempts++)
            {


                requestClkConvFactor = bspUicc_Lnk_resetMgrAtrDecodedData.clkConvFactor;
                requestBaudConvFactor = bspUicc_Lnk_resetMgrAtrDecodedData.baudConvFactor;

                /* Send a PPS request with the Protocol, F and D from the ATR */
                if (bspUicc_Lnk_resetMgrAtrDecodedData.secondProtocol ==
                        BSP_UICC_LNK_RESET_MGR_T1_PROTOCOL)
                {
                    requestProtocol = bspUicc_Lnk_resetMgrAtrDecodedData.secondProtocol;
                }
                else
                {
                    requestProtocol = bspUicc_Lnk_resetMgrAtrDecodedData.firstProtocol;
                }


                if ((ppsAttempts == 0 ) || (ppsAttempts == 1))
                {
                    ppsStatus =  bspUicc_Lnk_resetMgrNegotiateMode( requestClkConvFactor,
                            requestBaudConvFactor,
                            requestProtocol );

                    if (ppsStatus ==  BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS)
                    {
                        ppsRetry = FALSE;
                    }
                }
                else if( ppsAttempts == 2)
                {
                    requestClkConvFactor =  1;
                	requestBaudConvFactor = 1;

                    ppsStatus = bspUicc_Lnk_resetMgrNegotiateMode(0x07,
                            0x07,
                            requestProtocol);
                    if (ppsStatus ==  BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS)
                    {

                        ppsRetry = FALSE;
                    }
                    else
                    {
                        /* No Action */
                    }
                }
                else if(ppsAttempts == 3)
                {

                    ppsRetry = FALSE;  /* No PPS is attempted if PPS fails 3 times at a stretch */
                    bspUicc_Lnk_dataConfigSetFiValue(
                            BSP_UICC_LNK_DATA_CONFIG_RESET_FI_VALUE);
                    bspUicc_Lnk_dataConfigSetDiValue(
                            BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE);

                }

                if(ppsRetry==TRUE)
                {
                    bspUicc_Phy_init();

                    /* Perform a warm Reset */
                    bspUicc_Phy_warmReset();

                    bspUicc_Lnk_resetMgrGetAtrChars(resetDataPtr);
                }
            }

    }
    else
    {
        /* The Card always comes up in specific mode */
        bspUicc_Lnk_dataConfigSetProtocol(bspUicc_Lnk_resetMgrAtrDecodedData.firstProtocol);

        bspUicc_Lnk_dataConfigSetFiValue( bspUicc_Lnk_resetMgrAtrDecodedData.clkConvFactor );

        bspUicc_Lnk_dataConfigSetDiValue( bspUicc_Lnk_resetMgrAtrDecodedData.baudConvFactor );
	ppsRetry = FALSE;
    }
#else
        ppsRetry = FALSE;
#endif
  }

    if (ppsRetry == TRUE) /* IF PPS isrequired and is not successful */
    {

        status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_INVALID_CARD;
    }



#ifndef FEATURE_UICC_DISABLE_SPEED_ENHANCEMENT
    if (bspUicc_Lnk_resetMgrAtrData.tc2 > 0)
    {
        bspUicc_Lnk_resetMgrAtrDecodedData.waitIValue =
            ( bspUicc_Lnk_resetMgrAtrData.tc2 *
              Di_Ta1_Mapping[requestBaudConvFactor]) - 1;
        bspUicc_Lnk_dataConfigSetWaitIValue( bspUicc_Lnk_resetMgrAtrDecodedData.waitIValue);
    }
    else
    {
        bspUicc_Lnk_resetMgrAtrDecodedData.waitIValue  = ( (BSP_UICC_LNK_DATA_CONFIG_RESET_WAITI_VALUE ) *
                Di_Ta1_Mapping[requestBaudConvFactor]) - 1;
        bspUicc_Lnk_dataConfigSetWaitIValue( bspUicc_Lnk_resetMgrAtrDecodedData.waitIValue );
    }
#else
    if (bspUicc_Lnk_resetMgrAtrData.tc2 > 0)
    {
        bspUicc_Lnk_resetMgrAtrDecodedData.waitIValue =
            ( bspUicc_Lnk_resetMgrAtrData.tc2
              *  Di_Ta1_Mapping[ BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE]) - 1;
        bspUicc_Lnk_dataConfigSetWaitIValue( bspUicc_Lnk_resetMgrAtrDecodedData.waitIValue);
    }
    else
    {
        bspUicc_Lnk_resetMgrAtrDecodedData.waitIValue  = ( (BSP_UICC_LNK_DATA_CONFIG_RESET_WAITI_VALUE ) *
                Di_Ta1_Mapping[ BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE]) - 1;
        bspUicc_Lnk_dataConfigSetWaitIValue( bspUicc_Lnk_resetMgrAtrDecodedData.waitIValue );
    }

#endif

    /* Configure only if ATR is succesful */
    if (status == BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS)
    {
#ifdef CLOCK_STOP_ENABLE
        if( voltageSelect ==  BSP_UICC_LNK_VOLTAGE_SELECT_OMIT )
        {
            prevClkStopPref = bspUicc_Lnk_dataConfigGetPrevSclkLev();
            bspUicc_Lnk_dataConfigSetClkStopInd(prevClkStopPref);
            prevVoltClass = bspUicc_Lnk_dataConfigGetPrevVoltClass();
            bspUicc_Lnk_dataConfigSetClkStopInd(prevVoltClass);
              bspUicc_Lnk_dataConfigCommitPending();
        }
        else
        {

             bspUicc_Lnk_dataConfigCommitPending();
             prevClkStopPref=bspUicc_Lnk_dataConfigGetClkStopInd();
             prevVoltClass=bspUicc_Lnk_dataConfigGetPowerLevel();
             bspUicc_Lnk_dataConfigSetPrevSclkLev(prevClkStopPref);
             bspUicc_Lnk_dataConfigSetPrevVoltClass(prevVoltClass);
        }
#else
        if( voltageSelect !=  BSP_UICC_LNK_VOLTAGE_SELECT_OMIT )
                     bspUicc_Lnk_dataConfigSetPrevVoltClass(prevVoltClass);
        bspUicc_Lnk_dataConfigCommitPending();
#endif
        bspUicc_Lnk_dataConfigGetPhyConfig( &configReq );

        configReq.configType = BSP_UICC_PHY_CONFIG_TYPE_READ_WRITE;

        bspUicc_Phy_configure( configReq.protocol,
                               configReq.sclkDiv,
                               configReq.etuPeriod,
                               configReq.cgt,
                               configReq.cwt,
                               configReq.bwt,
                               configReq.fiValue,
                               configReq.diValue,
                               configReq.waitIValue,
                               configReq.clkStopInd,
                               configReq.configType );


	bspUicc_Compute_RxTrigger();

        /* If card keeps coming up in specific mode, continue with default speeds */
    }
    else
    {
        if( bspUicc_Phy_getCardInsertStatus( ) == BSP_UICC_LNK_CARD_NOT_PRESENT_STATUS )
        {
            status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_CARD_NOT_INSERTED;
        }
        else
        {
            status = BSP_UICC_LNK_RESET_MGR_RETURN_CODE_INVALID_CARD;
        }
    }	
	
    return status;
}
