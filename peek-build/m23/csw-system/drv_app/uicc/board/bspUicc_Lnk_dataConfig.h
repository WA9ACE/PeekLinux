/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_DATA_CONFIG_HEADER
#define BSP_UICC_DATA_CONFIG_HEADER


/*=============================================================================
 * Component Description:
 *    BSP_UICC Link layer Data configurattion file
 */
/*!  
 * @header bspUicc_Lnk_dataConfig.h
 *  Header file for the BSP_UICC Link layer data configuration. this file has functions
 * to store default config parameters and current parameters, methods to get and set
 * the parameters.
 */

/*=============================================================================
 * Description:
 *   Default Values for the Data Configuration data structure
 */
#define BSP_UICC_LNK_DATA_CONFIG_RESET_PROTOCOL            BSP_UICC_PHY_T0_PROTOCOL
#define BSP_UICC_LNK_DATA_CONFIG_RESET_SCLK_DIV            BSP_UICC_PHY_SCLKDIV_13_4
#define BSP_UICC_LNK_DATA_CONFIG_RESET_ETU_PERIOD          BSP_UICC_PHY_ETU_PERIOD_372 
#define BSP_UICC_LNK_DATA_CONFIG_RESET_CGT                 0xD
#define BSP_UICC_LNK_DATA_CONFIG_RESET_CWT                 0x200B
#define BSP_UICC_LNK_DATA_CONFIG_RESET_BWT_LSB             0x3C0B
#define BSP_UICC_LNK_DATA_CONFIG_RESET_BWT_MSB             0
#define BSP_UICC_LNK_DATA_CONFIG_RESET_FI_VALUE            1 /* Clk Conv Factor 372 */
#define BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE            1 /* Baud Conv Factor 1  */
#define BSP_UICC_LNK_DATA_CONFIG_RESET_POWER_LEVEL         BSP_UICC_PHY_POWER_LEVEL_18;
#define BSP_UICC_LNK_DATA_CONFIG_RESET_WAITI_VALUE         0x14 /* default is 10 but programmed as (9+1) */
#define BSP_UICC_LNK_DATA_CONFIG_RESET_BLOCK_SIZE_VALUE    32

/*============================================================================= */
/*!
 * @typedef BspUicc_Lnk_NumHistoricBytes
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for the Number of Historic Bytes in the ATR
 */
typedef SYS_UWORD8 BspUicc_Lnk_NumHistoricBytes;

/*============================================================================= */
/*!
 * @typedef BspUicc_Lnk_DataConfig
 *
 * @discussion
 * <b> Description  </b><br>
 *  Specifies the type for holding the Link layer configuration data
 */
typedef struct 
{
    BspUicc_Phy_Protocol             protocol;
    BspUicc_Phy_SclkDiv              sclkDiv;
    BspUicc_Phy_EtuPeriod            etuPeriod;
    BspUicc_Phy_Cgt                  cgt;
    BspUicc_Phy_Cwt                  cwt;
    BspUicc_Phy_Bwt                  bwt;
    BspUicc_Phy_FiValue              fiValue;
    BspUicc_Phy_DiValue              diValue;
    BspUicc_Phy_WaitI                waitIValue;
    BspUicc_Phy_Convention           convention;
    BspUicc_Phy_PowerLevel           powerLevel;
    BspUicc_Phy_BlockSize            blockSize;
    BspUicc_Phy_SClkLev              clkStopInd;
    BspUicc_Phy_ConfigType           configType;
}
BspUicc_Lnk_DataConfig;

/*============================================================================== */
/*!
 * @typedef BspUicc_Lnk_PrevDataConfig
 *
 * @discussion
 * <b> Description  </b><br>
 *     Specifies the type for holding the previously used
 *     Link layer configuration data.
 */
typedef struct 
{
    BspUicc_Phy_PowerLevel           powerLevel;
    BspUicc_Phy_SClkLev              clkStopInd;
}
BspUicc_Lnk_PrevDataConfig;


/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigSetPhyResetConfig
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function sets the physical layer configuration to the reset values
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer 
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param
 *    None
 */
void bspUicc_Lnk_dataConfigSetPhyResetConfig( void );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigGetPhyConfig
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function gets the current physical layer configuration
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer 
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param
 *    configReq
 *   Pointer to the physical layer configuration structure
 */
void bspUicc_Lnk_dataConfigGetPhyConfig( BspUicc_Lnk_DataConfig* configReq );


/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigSetPhyConfig
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function sets the current physical layer configuration 
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer 
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param
 *    configReq
 *   Pointer to the physical layer configuration structure
 */
void bspUicc_Lnk_dataConfigSetPhyConfig( BspUicc_Lnk_DataConfig* configReq );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigDoPhyConfig
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function calls the physical layer configuration
 * function to configure the hardware block.
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer 
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param
 *    None
*/
void  bspUicc_Lnk_dataConfigDoPhyConfig ( void );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigDoPhyConfigClkStop
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function calls the physical layer configuration
 * function to configure the hardware block for clock stop.
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer 
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param
 *    None
*/
void  bspUicc_Lnk_dataConfigDoPhyConfigClkStop (  BspUicc_Phy_SClkLev   sclkLev  );


/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigCommitPending
 * 
 * @discussion
 * <b> Description  </b><br>
 *  This function overwrites the current physical layer configuration with the
 *  Pending configuration. 
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer 
 *
 * <b> Returns </b><br>
 *    None
 *
 * @param
 *    configReq
 *   Pointer to the physical layer configuration structure
 */
void bspUicc_Lnk_dataConfigCommitPending( void );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigGetField
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function gets an individual field from the config Data
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer 
 *
 * <b> Returns </b><br>
 *    The field type
 *
 * @param
 *    None
 */
BspUicc_Phy_Protocol bspUicc_Lnk_dataConfigGetProtocol( void );

/*============================================================================*/
BspUicc_Phy_SclkDiv bspUicc_Lnk_dataConfigGetSclkDiv( void );

/*============================================================================*/
BspUicc_Phy_EtuPeriod bspUicc_Lnk_dataConfigGetEtuPeriod( void );

/*============================================================================*/
BspUicc_Phy_Cgt bspUicc_Lnk_dataConfigGetCgt( void );

/*============================================================================*/
BspUicc_Phy_Cwt bspUicc_Lnk_dataConfigGetCwt( void );

/*=============================================================================*/
BspUicc_Phy_Bwt bspUicc_Lnk_dataConfigGetBwt( void );

/*============================================================================*/
BspUicc_Phy_FiValue bspUicc_Lnk_dataConfigGetFiValue( void );

/*============================================================================*/
BspUicc_Phy_DiValue bspUicc_Lnk_dataConfigGetDiValue( void );

/*============================================================================*/
BspUicc_Phy_WaitI bspUicc_Lnk_dataConfigGetWaitIValue( void );

/*============================================================================*/
BspUicc_Phy_Convention bspUicc_Lnk_dataConfigGetConvention( void );

/*============================================================================*/
BspUicc_Phy_PowerLevel bspUicc_Lnk_dataConfigGetPowerLevel( void );

/*============================================================================*/
BspUicc_Phy_BlockSize bspUicc_Lnk_dataConfigGetBlockSize( void );

/*============================================================================*/
BspUicc_Phy_SClkLev bspUicc_Lnk_dataConfigGetClkStopInd( void );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigSetField
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This function Sets an individual field in the config Data
 *
 * <b> Context </b><br>
 *    Called from BSP_UICC Link Layer 
 *
 * <b> Returns </b><br>
 *    The field type
 *
 * @param
 *    None
 */
void bspUicc_Lnk_dataConfigSetProtocol( BspUicc_Phy_Protocol protocol );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigSetPrevVoltClass
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This function Sets an individual field in the config Data.
 *
 * <b> Context </b><br>
 *     Called from BSP_UICC Link Layer. 
 *
 * <b> Returns </b><br>
 *     None
 *
 * @param clockStop
 *        The field type - BspUicc_Phy_PowerLevel.
 */
void bspUicc_Lnk_dataConfigSetPrevVoltClass( BspUicc_Phy_PowerLevel  powerLevel );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigGetPrevVoltClass
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This type get the previously used  setting for voltage class Indicator.
 *
 * <b> Context </b><br>
 *     Called from BSP_UICC Link Layer. 
 *
 * <b> Returns </b><br>
 *     The field type - BspUicc_Phy_Powerlevel.
 *
 * @param None
 *    
 */

 BspUicc_Phy_PowerLevel bspUicc_Lnk_dataConfigGetPrevVoltClass( void);

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigGetPrevSclkLev
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This type get the previously used  setting for teh Clock Stop Indicator.
 *
 * <b> Context </b><br>
 *     Called from BSP_UICC Link Layer. 
 *
 * <b> Returns </b><br>
 *     The field type - BspUicc_Phy_SclkLev.
 *
 * @param None
 *    
 */
 BspUicc_Phy_SClkLev  bspUicc_Lnk_dataConfigGetPrevSclkLev(void );

/*============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigSetPrevVoltSclklev
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This function Sets an individual field in the config Data.
 *
 * <b> Context </b><br>
 *     Called from BSP_UICC Link Layer. 
 *
 * <b> Returns </b><br>
 *     None
 *
 * @param clockStop
 *        The field type - BspUicc_Phy_SClkLev.
 */
void bspUicc_Lnk_dataConfigSetPrevSclkLev(BspUicc_Phy_SClkLev sclkLev);

/*=============================================================================*/
/*!
 * @function bspUicc_Lnk_dataConfigSetConfigType
 * 
 * @discussion
 * <b> Description  </b><br>
 *     This function Sets the type of physical configuration to be done.
 *
 * <b> Context </b><br>
 *     Called from BSP_UICC Link Layer. 
 *
 * <b> Returns </b><br>
 *     None
 *
 * @param clockStop
 *        The field type - BspUicc_Phy_ConfigType.
 */
void bspUicc_Lnk_dataConfigSetConfigType( BspUicc_Phy_ConfigType configType );
/*============================================================================*/
void bspUicc_Lnk_dataConfigSetSclkDiv( BspUicc_Phy_SclkDiv sclkDiv );

/*============================================================================*/
void bspUicc_Lnk_dataConfigSetEtuPeriod( BspUicc_Phy_EtuPeriod etuPeriod );

/*============================================================================*/
void bspUicc_Lnk_dataConfigSetCgt( BspUicc_Phy_Cgt cgt );

/*============================================================================*/
void bspUicc_Lnk_dataConfigSetCwt( BspUicc_Phy_Cwt cwt );

/*============================================================================*/
void bspUicc_Lnk_dataConfigSetBwt( BspUicc_Phy_Bwt bwt );

/*============================================================================*/
void bspUicc_Lnk_dataConfigSetFiValue( BspUicc_Phy_FiValue fiValue );

/*============================================================================*/
void bspUicc_Lnk_dataConfigSetDiValue( BspUicc_Phy_DiValue diValue );

/*============================================================================*/
void bspUicc_Lnk_dataConfigSetWaitIValue( BspUicc_Phy_WaitI waitIValue );

/*============================================================================*/
void bspUicc_Lnk_dataConfigSetConvention( BspUicc_Phy_Convention convention );

/*============================================================================*/
void bspUicc_Lnk_dataConfigSetPowerLevel( BspUicc_Phy_PowerLevel powerLevel );

/*============================================================================*/
void bspUicc_Lnk_dataConfigSetBlockSize( BspUicc_Phy_BlockSize blockSize );
/*============================================================================*/
void  bspUicc_Lnk_dataConfigSetClkStopInd( BspUicc_Phy_ClockStop );



#endif
