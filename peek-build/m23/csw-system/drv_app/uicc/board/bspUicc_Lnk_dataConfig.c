/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */

#include "sys_types.h"
#include "types.h"
#include "bspUicc_Phy.h"
#include "bspUicc_Lnk.h"
#include "bspUicc_Lnk_resetMgr.h"
#include "bspUicc_Lnk_dataConfig.h"
#include "bspUicc_Lnk_msgUtil.h"

/*=============================================================================
 *  File Description:
 *  Source File with code for Link layer Data configuration file
 */

/*=============================================================================
 * Description:
 *   Global variable to hold modified physical layer configuration
 * When configuration parameters are modified, they will be
 * changed in the pending data structure. Performing a commit will
 * update the pending data. This is done to allow discarding of
 * updated configuration.
 */
BspUicc_Lnk_DataConfig bspUicc_Lnk_dataConfigPending;

/*=============================================================================
 * Description:
 *   Global variable to hold current physical layer configuration
 */
BspUicc_Lnk_DataConfig bspUicc_Lnk_dataConfigCurrent;

BspUicc_Lnk_PrevDataConfig bspUicc_Lnk_prevDataConfig;

/*=============================================================================
 *   Public Functions
 */
 
/*=============================================================================
 * Description:
 *   Set the configuration structure to the reset default values
 */
void bspUicc_Lnk_dataConfigSetPhyResetConfig( void )
{
    bspUicc_Lnk_dataConfigCurrent.protocol    = BSP_UICC_LNK_DATA_CONFIG_RESET_PROTOCOL;
    bspUicc_Lnk_dataConfigCurrent.sclkDiv     = BSP_UICC_LNK_DATA_CONFIG_RESET_SCLK_DIV;
    bspUicc_Lnk_dataConfigCurrent.etuPeriod   = BSP_UICC_LNK_DATA_CONFIG_RESET_ETU_PERIOD;
    bspUicc_Lnk_dataConfigCurrent.cgt         = BSP_UICC_LNK_DATA_CONFIG_RESET_CGT;
    bspUicc_Lnk_dataConfigCurrent.cwt         = BSP_UICC_LNK_DATA_CONFIG_RESET_CWT;
    bspUicc_Lnk_dataConfigCurrent.bwt         = BSP_UICC_LNK_DATA_CONFIG_RESET_BWT_LSB |
        (BSP_UICC_LNK_DATA_CONFIG_RESET_BWT_MSB << 16);
    bspUicc_Lnk_dataConfigCurrent.fiValue     = BSP_UICC_LNK_DATA_CONFIG_RESET_FI_VALUE;
    bspUicc_Lnk_dataConfigCurrent.diValue     = BSP_UICC_LNK_DATA_CONFIG_RESET_DI_VALUE;
    bspUicc_Lnk_dataConfigCurrent.waitIValue  = BSP_UICC_LNK_DATA_CONFIG_RESET_WAITI_VALUE;
    bspUicc_Lnk_dataConfigCurrent.convention  = BSP_UICC_PHY_CONVENTION_DIRECT;
    bspUicc_Lnk_dataConfigCurrent.powerLevel  = BSP_UICC_LNK_DATA_CONFIG_RESET_POWER_LEVEL;
    bspUicc_Lnk_dataConfigCurrent.blockSize   = BSP_UICC_LNK_DATA_CONFIG_RESET_BLOCK_SIZE_VALUE;
    bspUicc_Lnk_dataConfigCurrent.clkStopInd  = BSP_UICC_PHY_SCLOCK_NO_STOP;

    /* Initialize the pending data structure also to Current values.
       When a select few parameters are changed, and a commit is
       performed, it will not reset the current data */
    bspUicc_Lnk_dataConfigPending = bspUicc_Lnk_dataConfigCurrent;
}
 
/*=============================================================================
 * Description:
 *   Get the current physical layer configuration
 */    
void bspUicc_Lnk_dataConfigGetPhyConfig( BspUicc_Lnk_DataConfig* configReq )
{
    configReq->protocol    = bspUicc_Lnk_dataConfigCurrent.protocol;
    configReq->sclkDiv     = bspUicc_Lnk_dataConfigCurrent.sclkDiv;
    configReq->etuPeriod   = bspUicc_Lnk_dataConfigCurrent.etuPeriod;
    configReq->cgt         = bspUicc_Lnk_dataConfigCurrent.cgt;
    configReq->cwt         = bspUicc_Lnk_dataConfigCurrent.cwt;
    configReq->bwt         = bspUicc_Lnk_dataConfigCurrent.bwt;
    configReq->fiValue     = bspUicc_Lnk_dataConfigCurrent.fiValue;
    configReq->diValue     = bspUicc_Lnk_dataConfigCurrent.diValue;
    configReq->waitIValue  = bspUicc_Lnk_dataConfigCurrent.waitIValue;
    configReq->convention  = bspUicc_Lnk_dataConfigCurrent.convention;
    configReq->powerLevel  = bspUicc_Lnk_dataConfigCurrent.powerLevel;
    configReq->blockSize   = bspUicc_Lnk_dataConfigCurrent.blockSize;
    configReq->clkStopInd  = bspUicc_Lnk_dataConfigCurrent.clkStopInd;
}
 
/*=============================================================================
 * Description:
 *   Set the current physical layer configuration
 */    
void bspUicc_Lnk_dataConfigSetPhyConfig( BspUicc_Lnk_DataConfig* configReq )
{
    bspUicc_Lnk_dataConfigCurrent.protocol    = configReq->protocol;
    bspUicc_Lnk_dataConfigCurrent.sclkDiv     = configReq->sclkDiv;
    bspUicc_Lnk_dataConfigCurrent.etuPeriod   = configReq->etuPeriod;
    bspUicc_Lnk_dataConfigCurrent.cgt         = configReq->cgt;
    bspUicc_Lnk_dataConfigCurrent.cwt         = configReq->cwt;
    bspUicc_Lnk_dataConfigCurrent.bwt         = configReq->bwt;
    bspUicc_Lnk_dataConfigCurrent.fiValue     = configReq->fiValue;
    bspUicc_Lnk_dataConfigCurrent.diValue     = configReq->diValue;
    bspUicc_Lnk_dataConfigCurrent.waitIValue  = configReq->waitIValue;
    bspUicc_Lnk_dataConfigCurrent.powerLevel  = configReq->powerLevel;
    bspUicc_Lnk_dataConfigCurrent.blockSize   = configReq->blockSize;
    bspUicc_Lnk_dataConfigCurrent.clkStopInd  = configReq->clkStopInd;
}


/*=============================================================================
 * Description:
 *   Perform the  physical layer configuration
 */    
void bspUicc_Lnk_dataConfigDoPhyConfig( void )
{
   
    bspUicc_Phy_configure(  bspUicc_Lnk_dataConfigCurrent.protocol,
                            bspUicc_Lnk_dataConfigCurrent.sclkDiv,
                            bspUicc_Lnk_dataConfigCurrent.etuPeriod,
                            bspUicc_Lnk_dataConfigCurrent.cgt,
                            bspUicc_Lnk_dataConfigCurrent.cwt,
                            bspUicc_Lnk_dataConfigCurrent.bwt,
                            bspUicc_Lnk_dataConfigCurrent.fiValue,
                            bspUicc_Lnk_dataConfigCurrent.diValue,
                            bspUicc_Lnk_dataConfigCurrent.waitIValue,
                            bspUicc_Lnk_dataConfigCurrent.clkStopInd,
                            bspUicc_Lnk_dataConfigCurrent.configType );
}

/*=============================================================================
 * Description:
 *   Perform the  physical layer configuration for clock stop prefernce
 */    
void bspUicc_Lnk_dataConfigDoPhyConfigClkStop(  BspUicc_Phy_SClkLev sclkLev )
{
    BspUicc_Phy_ConfigType configType;
    bspUicc_Lnk_dataConfigSetClkStopInd( sclkLev );
    configType =  BSP_UICC_PHY_CONFIG_TYPE_CLKSTOP_UPDATE;
    bspUicc_Lnk_dataConfigSetConfigType( configType );
    bspUicc_Lnk_dataConfigCommitPending();
    
    bspUicc_Phy_configure(  bspUicc_Lnk_dataConfigCurrent.protocol,
                            bspUicc_Lnk_dataConfigCurrent.sclkDiv,
                            bspUicc_Lnk_dataConfigCurrent.etuPeriod,
                            bspUicc_Lnk_dataConfigCurrent.cgt,
                            bspUicc_Lnk_dataConfigCurrent.cwt,
                            bspUicc_Lnk_dataConfigCurrent.bwt,
                            bspUicc_Lnk_dataConfigCurrent.fiValue,
                            bspUicc_Lnk_dataConfigCurrent.diValue,
                            bspUicc_Lnk_dataConfigCurrent.waitIValue,
                            bspUicc_Lnk_dataConfigCurrent.clkStopInd,
                            bspUicc_Lnk_dataConfigCurrent.configType );
}

/*=============================================================================
 * Description:
 *   Get interface for all the config data
 */

/*=============================================================================*/
BspUicc_Phy_Protocol bspUicc_Lnk_dataConfigGetProtocol( void )
{
    return bspUicc_Lnk_dataConfigCurrent.protocol;
}

/*=============================================================================*/
BspUicc_Phy_SclkDiv bspUicc_Lnk_dataConfigGetSclkDiv( void )
{
    return bspUicc_Lnk_dataConfigCurrent.sclkDiv;
}

/*=============================================================================*/
BspUicc_Phy_EtuPeriod bspUicc_Lnk_dataConfigGetEtuPeriod( void )
{
    return bspUicc_Lnk_dataConfigCurrent.etuPeriod;
}

/*=============================================================================*/
BspUicc_Phy_Cgt bspUicc_Lnk_dataConfigGetCgt( void )
{
    return bspUicc_Lnk_dataConfigCurrent.cgt;
}

/*=============================================================================*/
BspUicc_Phy_Cwt bspUicc_Lnk_dataConfigGetCwt( void )
{
    return bspUicc_Lnk_dataConfigCurrent.cwt;
}

/*=============================================================================*/
BspUicc_Phy_Bwt bspUicc_Lnk_dataConfigGetBwt( void )
{
    return bspUicc_Lnk_dataConfigCurrent.bwt;
}

/*=============================================================================*/
BspUicc_Phy_FiValue bspUicc_Lnk_dataConfigGetFiValue( void )
{
    return bspUicc_Lnk_dataConfigCurrent.fiValue;
}

/*=============================================================================*/
BspUicc_Phy_DiValue bspUicc_Lnk_dataConfigGetDiValue( void )
{
    return bspUicc_Lnk_dataConfigCurrent.diValue;
}

/*=============================================================================*/
BspUicc_Phy_WaitI bspUicc_Lnk_dataConfigGetWaitIValue( void )
{
    return bspUicc_Lnk_dataConfigCurrent.waitIValue;
}

/*=============================================================================*/
BspUicc_Phy_Convention bspUicc_Lnk_dataConfigGetConvention( void )
{
    return bspUicc_Lnk_dataConfigCurrent.convention;
}

/*=============================================================================*/
BspUicc_Phy_PowerLevel bspUicc_Lnk_dataConfigGetPowerLevel( void )
{
    return bspUicc_Lnk_dataConfigCurrent.powerLevel;  
}
/*=============================================================================*/
BspUicc_Phy_BlockSize bspUicc_Lnk_dataConfigGetBlockSize( void )
{
    return bspUicc_Lnk_dataConfigCurrent.blockSize;  
}

/*=============================================================================*/
BspUicc_Phy_ClockStop  bspUicc_Lnk_dataConfigGetClkStopInd( void )
{
    return bspUicc_Lnk_dataConfigCurrent.clkStopInd;  
}

/*=============================================================================
 * Description:
 *   Set interface for each field of  the config data
 */

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetProtocol( BspUicc_Phy_Protocol protocol )
{
    bspUicc_Lnk_dataConfigPending.protocol = protocol;
}

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetSclkDiv( BspUicc_Phy_SclkDiv sclkDiv )
{
    bspUicc_Lnk_dataConfigPending.sclkDiv = sclkDiv;
}

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetEtuPeriod( BspUicc_Phy_EtuPeriod etuPeriod )
{
    bspUicc_Lnk_dataConfigPending.etuPeriod = etuPeriod;
}

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetCgt( BspUicc_Phy_Cgt cgt )
{
    bspUicc_Lnk_dataConfigPending.cgt = cgt;
}

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetCwt( BspUicc_Phy_Cwt cwt )
{
    bspUicc_Lnk_dataConfigPending.cwt = cwt;
}

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetBwt( BspUicc_Phy_Bwt bwt )
{
    bspUicc_Lnk_dataConfigPending.bwt = bwt;
}

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetFiValue( BspUicc_Phy_FiValue fiValue )
{
    bspUicc_Lnk_dataConfigPending.fiValue = fiValue;
}

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetDiValue( BspUicc_Phy_DiValue diValue )
{
    bspUicc_Lnk_dataConfigPending.diValue = diValue;
}

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetWaitIValue( BspUicc_Phy_WaitI waitIValue )
{
    bspUicc_Lnk_dataConfigPending.waitIValue = waitIValue;
}

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetConvention( BspUicc_Phy_Convention convention )
{
    bspUicc_Lnk_dataConfigPending.convention = convention;
}

/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetPowerLevel( BspUicc_Phy_PowerLevel powerLevel )
{
    bspUicc_Lnk_dataConfigPending.powerLevel  = powerLevel;
}
/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetBlockSize( BspUicc_Phy_BlockSize blockSize )
{
    bspUicc_Lnk_dataConfigPending.blockSize  = blockSize;
}
/*=============================================================================*/
void bspUicc_Lnk_dataConfigSetClkStopInd( BspUicc_Phy_ClockStop clkStopInd )
{
    bspUicc_Lnk_dataConfigPending.clkStopInd  = clkStopInd;
}

/*=============================================================================*/
void
bspUicc_Lnk_dataConfigSetConfigType(BspUicc_Phy_ConfigType configType)
{
    
    bspUicc_Lnk_dataConfigPending.configType  = configType;
    
    return;
    
}

/*=============================================================================
 * Description:
 *   Convert the Pending Configuration into the current configuration.
 */
void bspUicc_Lnk_dataConfigCommitPending( void )
{
    bspUicc_Lnk_dataConfigCurrent = bspUicc_Lnk_dataConfigPending;
}

BspUicc_Phy_PowerLevel
bspUicc_Lnk_dataConfigGetPrevVoltClass(void)
{

    return  bspUicc_Lnk_prevDataConfig.powerLevel;

}
void
bspUicc_Lnk_dataConfigSetPrevVoltClass(  BspUicc_Phy_PowerLevel  powerLevel )
{
    bspUicc_Lnk_prevDataConfig.powerLevel = powerLevel;

    return;

}

void
bspUicc_Lnk_dataConfigSetPrevSclkLev(BspUicc_Phy_SClkLev sclkLev  )
{

    bspUicc_Lnk_prevDataConfig.clkStopInd = sclkLev;
}

BspUicc_Phy_SClkLev
bspUicc_Lnk_dataConfigGetPrevSclkLev(void)
{

    return bspUicc_Lnk_prevDataConfig.clkStopInd;
}

