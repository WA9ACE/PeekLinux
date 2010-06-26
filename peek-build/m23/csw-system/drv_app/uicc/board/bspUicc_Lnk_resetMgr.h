/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_LNK_RESET_MGR_HEADER
#define BSP_UICC_LNK_RESET_MGR_HEADER

/*===========================================================================
 * Component Description:
 */
/*!  
 * @header bspUicc_Lnk_resetMgr.h
 *  This component manages the answer to reset characters of a card
 */

/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_ResetMgrReturnCode
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This type specifies the return code/status. It has the size information
 *  when a function is sending or receiveing data.
 */
enum
{    
     BSP_UICC_LNK_RESET_MGR_RETURN_CODE_FAILURE                    = (-1),
     BSP_UICC_LNK_RESET_MGR_RETURN_CODE_SUCCESS                    = 0,
     BSP_UICC_LNK_RESET_MGR_RETURN_CODE_CARD_NOT_INSERTED          = 1,
     BSP_UICC_LNK_RESET_MGR_RETURN_CODE_INVALID_CARD               = 2,
     BSP_UICC_LNK_RESET_MGR_RETURN_CODE_ME_FAILURE                 = 3
};
typedef SYS_WORD16  BspUicc_Lnk_ResetMgrReturnCode;


/*=============================================================================*/
/*!
 * @typedef BspUicc_Lnk_ResetMgrProtocol
 * 
 * @discussion
 * <b> Description  </b><br>
 *   Specifies the type for the Protocol used (Char or Block).
 */
enum
{
    BSP_UICC_LNK_RESET_MGR_T0_PROTOCOL               = 0,
    BSP_UICC_LNK_RESET_MGR_T1_PROTOCOL               = 1
};
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgr_Protocol;



/*============================================================================*/
/*!
 * @function bspUicc_Lnk_resetMgrStartATR
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function sets the Physical layer  config to the default ATR configuration
 *  to start the ATR.
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer
 *
 * <b> Returns </b><br>
 *    BspUicc_Lnk_ResetMgrReturnCode
 *
 * @param
 *    resetDataPtr
 *    Pointer to the ATR data
 *
 * @param
 *    resetDataSize
 *    size of the reset data Buffer
 *
 */
BspUicc_Lnk_ResetMgrReturnCode
bspUicc_Lnk_resetMgrStartAtr( BspUicc_Lnk_ResetData *resetDataPtr,
                              BspUicc_Lnk_ResetDataSize *resetDataSizePtr,
                              BspUicc_Lnk_VoltageSelect voltageSelect,
                              BspUicc_Lnk_ResetNewVoltClass  resetNewVoltClass,
                              BspUicc_Lnk_VoltClass       newVoltClass);


                            

/*=============================================================================*/
/*!
 *  @typedef BspUicc_AtrSize
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the size of the ATR data rturned by the card
 */

typedef SYS_UWORD8  BspUicc_Lnk_ResetMgr_AtrSize;

/*=============================================================================*/
/*!
 *  @typedef BspUicc_AtrData
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies ATR data returned by the card
 */


typedef SYS_UWORD8  BspUicc_Lnk_ResetMgr_AtrData;


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
     BspUicc_Lnk_ResetDataSize   atrSize;
     BspUicc_Lnk_ResetData       *atrDataPtr;
    
}BspUicc_Lnk_ResetMgr_AtrDataInfo;


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
    BSP_UICC_LNK_RESET_MGR_OMIT_CONFIG_CHARS                 = 0x0,
    BSP_UICC_LNK_RESET_REQUEST_CONFIG_CHARS                  = 0x1
    };
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgr_ConfigReq;


/*=============================================================================*/
/*!
 *  @typedef BspUicc_ClkStopPreference
 *
 * @discussion
 * <b> Description  </b><br>
 * This type specifies the clock stop prefernce of the MF
 */

typedef SYS_UWORD8  BspUicc_Lnk_ResetMgr_ClkStopPreference;


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
    BSP_UICC_LNK_RESET_MGR_CLOCK_STOP_NOT_SUPPORTED                    = 0x00
};
typedef SYS_UWORD8 BspUicc_Lnk_ResetMgr_ClockStopSuported;


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
     BspUicc_Lnk_ResetMgr_ClkStopPreference      clkStopPreference;
}BspUicc_Lnk_ResetMgr_ConfigChars;    


/*============================================================================*/
/*!
 * @typedef BspUicc_Lnk_ResetMgr_XiUiBytePresent
 *
 * @discussion
 * Description
 * This type specifies the if the TA(i) byte after the first occurence of T=15
 * is present. refernce ISO/IEC 7816-3 section 6.5.1
 */
typedef Bool BspUicc_Lnk_ResetMgr_XiUiBytePresent;

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_XiUiBytePresent
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This function chceks if the Xi/Ui byte TA(i) present in the ATR.
 *
 * <b> Context </b><br>
 *     Called from BSP_UICC link Layer.
 *
 * <b> Returns </b><br>
 *     TRUE if the Xi/Ui byte TA(i) present in the ATR.
 *
 * @param None
 *
 */
Bool
bspUicc_Lnk_ResetMgr_xiUiBytePresent(void);
#endif
