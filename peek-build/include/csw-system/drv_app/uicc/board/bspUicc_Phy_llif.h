/*===================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_PHY_LLIF_HEADER
#define BSP_UICC_PHY_LLIF_HEADER

#include "bspUtil_BitUtil.h"
//#include "bspUtil_MemUtil.h"

/*=============================================================================
 * Component Description:
 *   Defines coponents and macros for the BSP_UICC Device Driver Block.
 */

/*=============================================================================
 * Constants
 */

/****** Definitions for USIM_CMD Register *******/
#define    BSP_UICC_PHY_LLIF_CMDIFRST_OFFSET          0
#define    BSP_UICC_PHY_LLIF_CMDIFRST_WIDTH           1

#define    BSP_UICC_PHY_LLIF_CMDSTOP_OFFSET           1
#define    BSP_UICC_PHY_LLIF_CMDSTOP_WIDTH            1

#define    BSP_UICC_PHY_LLIF_CMDSTART_OFFSET          2
#define    BSP_UICC_PHY_LLIF_CMDSTART_WIDTH           1

#define    BSP_UICC_PHY_LLIF_MODULE_CLK_EN_OFFSET     3
#define    BSP_UICC_PHY_LLIF_MODULE_CLK_EN_WIDTH      1

#define    BSP_UICC_PHY_LLIF_WARM_RESET_CMD_OFFSET    4
#define    BSP_UICC_PHY_LLIF_WARM_RESET_CMD_WIDTH     1

#define    BSP_UICC_PHY_LLIF_CLOCK_STOP_CMD_OFFSET    5
#define    BSP_UICC_PHY_LLIF_CLOCK_STOP_CMD_WIDTH     1

#define    BSP_UICC_PHY_LLIF_STP_EMV_ATR_TIMER_OFFSET 6
#define    BSP_UICC_PHY_LLIF_STP_EMV_ATR_TIMER_WIDTH  1

#define    BSP_UICC_PHY_LLIF_REG_USIM_CMD_OFFSET      0
#define    BSP_UICC_PHY_LLIF_REG_USIM_CMD_WIDTH      16

#define    BSP_UICC_PHY_LLIF_REG_USIM_CMD_CLEAR_ALL   0

enum
{
    BSP_UICC_PHY_LLIF_CMDIFRST_DISABLE   = 0,
    BSP_UICC_PHY_LLIF_CMDIFRST_ENABLE    = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_CmdIfReset;


enum
{
    BSP_UICC_PHY_LLIF_CMDSTOP_DISABLE    = 0,
    BSP_UICC_PHY_LLIF_CMDSTOP_ENABLE     = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_CmdStop;

enum
{
    BSP_UICC_PHY_LLIF_CMDSTART_DISABLE   = 0,
    BSP_UICC_PHY_LLIF_CMDSTART_ENABLE    = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_CmdStart;

enum
{
    BSP_UICC_PHY_LLIF_MODULE_CLK_DISABLE = 0,
    BSP_UICC_PHY_LLIF_MODULE_CLK_ENABLE  = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ModuleClk;

enum
{
    BSP_UICC_PHY_LLIF_WARM_RESET_DISABLE = 0,
    BSP_UICC_PHY_LLIF_WARM_RESET_ENABLE  = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_WarmReset;

    
    

/****** Definitions for USIM_STAT Register *******/
#define    BSP_UICC_PHY_LLIF_STATNOCARD_OFFSET        0
#define    BSP_UICC_PHY_LLIF_STATNOCARD_WIDTH         1

#define    BSP_UICC_PHY_LLIF_STATTXPAR_OFFSET         1
#define    BSP_UICC_PHY_LLIF_STATTXPAR_WIDTH          1

#define    BSP_UICC_PHY_LLIF_STATLRC_OFFSET           2
#define    BSP_UICC_PHY_LLIF_STATLRC_WIDTH            1

#define    BSP_UICC_PHY_LLIF_CONFCODCONV_OFFSET       3
#define    BSP_UICC_PHY_LLIF_CONFCODCONV_WIDTH        1

#define    BSP_UICC_PHY_LLIF_X_MODE_OFFSET            4
#define    BSP_UICC_PHY_LLIF_X_MODE_WIDTH             1

#define    BSP_UICC_PHY_LLIF_REG_USIM_STAT_OFFSET     0
#define    BSP_UICC_PHY_LLIF_REG_USIM_STAT_WIDTH     16

enum
{
    BSP_UICC_PHY_LLIF_STATNOCARD_ABSENT  = 0,
    BSP_UICC_PHY_LLIF_STATNOCARD_PRESENT = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_StatNoCard;

enum
{
    BSP_UICC_PHY_LLIF_STATTXPAR_ERROR    = 0,
    BSP_UICC_PHY_LLIF_STATTXPAR_OK       = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_StatTxPar;

enum
{
    BSP_UICC_PHY_LLIF_STATLRC_OK         = 0,
    BSP_UICC_PHY_LLIF_STATLRC_ERROR      = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_StatLRC;

enum
{
    BSP_UICC_PHY_LLIF_CONFCODCONV_DIRECT = 0,
    BSP_UICC_PHY_LLIF_CONFCODCONV_INVERSE= 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfCodConv;

/****** Definitions for USIM_CONF1 Register *******/
#define    BSP_UICC_PHY_LLIF_CONFSCLKLEV_OFFSET       0
#define    BSP_UICC_PHY_LLIF_CONFSCLKLEV_WIDTH        1

#define    BSP_UICC_PHY_LLIF_CONFSIOLOW_OFFSET        1
#define    BSP_UICC_PHY_LLIF_CONFSIOLOW_WIDTH         1

#define    BSP_UICC_PHY_LLIF_CONFBYPASS_OFFSET        2
#define    BSP_UICC_PHY_LLIF_CONFBYPASS_WIDTH         1

#define    BSP_UICC_PHY_LLIF_CONFSVCCLEV_OFFSET       3
#define    BSP_UICC_PHY_LLIF_CONFSVCCLEV_WIDTH        1

#define    BSP_UICC_PHY_LLIF_CONFSRSTLEV_OFFSET       4
#define    BSP_UICC_PHY_LLIF_CONFSRSTLEV_WIDTH        1

#define    BSP_UICC_PHY_LLIF_CONF_SCLK_EN_OFFSET      5
#define    BSP_UICC_PHY_LLIF_CONF_SCLK_EN_WIDTH       1

#define    BSP_UICC_PHY_LLIF_EMV_CONF_OFFSET          6
#define    BSP_UICC_PHY_LLIF_EMV_CONF_WIDTH           1

#define    BSP_UICC_PHY_LLIF_REG_USIM_CONF1_OFFSET    0
#define    BSP_UICC_PHY_LLIF_REG_USIM_CONF1_WIDTH    16

enum
{
    BSP_UICC_PHY_LLIF_CONFSCLKLEV_LOW    = 0,
    BSP_UICC_PHY_LLIF_CONFSCLKLEV_HIGH   = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfSclkLev;

enum
{
    BSP_UICC_PHY_LLIF_CONFSIOLOW_DISABLE = 0,
    BSP_UICC_PHY_LLIF_CONFSIOLOW_ENABLE  = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfSioLow;
enum
{
    BSP_UICC_PHY_LLIF_CONFBYPASS_DISABLE = 0,
    BSP_UICC_PHY_LLIF_CONFBYPASS_ENABLE  = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfBypass;
enum
{
    BSP_UICC_PHY_LLIF_CONFSVCCLEV_LOW    = 0,
    BSP_UICC_PHY_LLIF_CONFSVCCLEV_HIGH   = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfSvccLev;

enum
{
    BSP_UICC_PHY_LLIF_CONFSRST_LOW       = 0,
    BSP_UICC_PHY_LLIF_CONFSRST_HIGH      = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfSrst;

enum
{
    BSP_UICC_PHY_LLIF_CONFSCLK_DISABLE   = 0,
    BSP_UICC_PHY_LLIF_CONFSCLK_ENABLE    = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfSclk;


/****** Definitions for USIM_CONF2 Register *******/
#define    BSP_UICC_PHY_LLIF_CONFCHKPAR_OFFSET        0
#define    BSP_UICC_PHY_LLIF_CONFCHKPAR_WIDTH         1

#define    BSP_UICC_PHY_LLIF_CONFTXNRX_OFFSET         1
#define    BSP_UICC_PHY_LLIF_CONFTXNRX_WIDTH          1

#define    BSP_UICC_PHY_LLIF_CONFSCLKDIV_OFFSET       2
#define    BSP_UICC_PHY_LLIF_CONFSCLKDIV_WIDTH        2

#define    BSP_UICC_PHY_LLIF_ATR_ASYN_BYPASS_OFFSET   4
#define    BSP_UICC_PHY_LLIF_ATR_ASYN_BYPASS_WIDTH    1

#define    BSP_UICC_PHY_LLIF_CON_PROTOCOL_OFFSET      5
#define    BSP_UICC_PHY_LLIF_CON_PROTOCOL_WIDTH       1

#define    BSP_UICC_PHY_LLIF_CONF_EDC_OFFSET          6
#define    BSP_UICC_PHY_LLIF_CONF_EDC_WIDTH           1

#define    BSP_UICC_PHY_LLIF_CONFLRCCHECK_OFFSET      7
#define    BSP_UICC_PHY_LLIF_CONFLRCCHECK_WIDTH       1

#define    BSP_UICC_PHY_LLIF_CONF_RESENT_OFFSET       8
#define    BSP_UICC_PHY_LLIF_CONF_RESENT_WIDTH        3

#define    BSP_UICC_PHY_LLIF_REG_USIM_CONF2_OFFSET    0
#define    BSP_UICC_PHY_LLIF_REG_USIM_CONF2_WIDTH    16

enum
{
    BSP_UICC_PHY_LLIF_CONFCHKPAR_DISABLE    = 0,
    BSP_UICC_PHY_LLIF_CONFCHKPAR_ENABLE     = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfChkPar;

enum
{
    BSP_UICC_PHY_LLIF_CONFTXNRX_RX          = 0,
    BSP_UICC_PHY_LLIF_CONFTXNRX_TX          = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfTxnRx;

enum
{
    BSP_UICC_PHY_LLIF_CONFETUPERIOD_8       = 0,  /* for 8 times 1/F sclk */
    BSP_UICC_PHY_LLIF_CONFETUPERIOD_372     = 1,
    BSP_UICC_PHY_LLIF_CONFETUPERIOD_512_8   = 2,
    BSP_UICC_PHY_LLIF_CONFETUPERIOD_512_16  = 3
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfEtuPeriod;

enum
{
    BSP_UICC_PHY_LLIF_SCLKDIV_13_4          = 0, /* for 13/4 MHz */
    BSP_UICC_PHY_LLIF_SCLKDIV_13_8          = 1  /* for 13/8 MHz */
};
typedef SYS_UWORD8 BspUicc_Usim_llif_SclkDiv;
enum
{
    BSP_UICC_PHY_LLIF_CONFPROTOCOL_T0       = 0,
    BSP_UICC_PHY_LLIF_CONFPROTOCOL_T1       = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfProtocol;
enum
{
    BSP_UICC_PHY_LLIF_CONFEDC_LRC            = 0,
    BSP_UICC_PHY_LLIF_CONFEDC_CRC            = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfEdc;
enum
{
    BSP_UICC_PHY_LLIF_CONFLRCCHECK_DISABLE   = 0,
    BSP_UICC_PHY_LLIF_CONFLRCCHECK_ENABLE    = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfLrcCheck;

enum
{
    BSP_UICC_PHY_LLIF_CONFRESENT_DEF_VAL   = 0x7
};
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfResent;

enum
{
    BSP_UICC_PHY_LLIF_ATR_ASYN_BYPASS_DISABLE= 0,
    BSP_UICC_PHY_LLIF_ATR_ASYN_BYPASS_ENABLE = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_AtrAsynBypass;

/****** Definitions for USIM_CONF3 Register ********/
#define    BSP_UICC_PHY_LLIF_CONFTFUSIM_OFFSET             0
#define    BSP_UICC_PHY_LLIF_CONFTFUSIM_WIDTH              4

#define    BSP_UICC_PHY_LLIF_CONFTDUSIM_OFFSET             4
#define    BSP_UICC_PHY_LLIF_CONFTDUSIM_WIDTH              4

#define    BSP_UICC_PHY_LLIF_REG_USIM_CONF3_OFFSET         0
#define    BSP_UICC_PHY_LLIF_REG_USIM_CONF3_WIDTH         16

typedef SYS_UWORD8 BspUicc_Usim_llif_ConfWaiti;
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfTdUsim;
typedef SYS_UWORD8 BspUicc_Usim_llif_ConfTfUsim;

/****** Definitions for USIM_IT Register ***********/
#define    BSP_UICC_PHY_LLIF_USIM_NATR_OFFSET              0
#define    BSP_UICC_PHY_LLIF_USIM_NATR_WIDTH               1

#define    BSP_UICC_PHY_LLIF_USIM_WT_OFFSET                1
#define    BSP_UICC_PHY_LLIF_USIM_WT_WIDTH                 1

#define    BSP_UICC_PHY_LLIF_USIM_OV_OFFSET                2
#define    BSP_UICC_PHY_LLIF_USIM_OV_WIDTH                 1

#define    BSP_UICC_PHY_LLIF_USIM_TX_OFFSET                3
#define    BSP_UICC_PHY_LLIF_USIM_TX_WIDTH                 1

#define    BSP_UICC_PHY_LLIF_USIM_RX_OFFSET                4
#define    BSP_UICC_PHY_LLIF_USIM_RX_WIDTH                 1

#define    BSP_UICC_PHY_LLIF_USIM_CD_OFFSET                5
#define    BSP_UICC_PHY_LLIF_USIM_CD_WIDTH                 1

#define    BSP_UICC_PHY_LLIF_USIM_EOB_OFFSET               6
#define    BSP_UICC_PHY_LLIF_USIM_EOB_WIDTH                1

#define    BSP_UICC_PHY_LLIF_USIM_TOC_OFFSET               7
#define    BSP_UICC_PHY_LLIF_USIM_TOC_WIDTH                1

#define    BSP_UICC_PHY_LLIF_USIM_TOB_OFFSET               8
#define    BSP_UICC_PHY_LLIF_USIM_TOB_WIDTH                1

#define    BSP_UICC_PHY_LLIF_USIM_RESENT_OFFSET            9
#define    BSP_UICC_PHY_LLIF_USIM_RESENT_WIDTH             1

#define    BSP_UICC_PHY_LLIF_USIM_TS_ERROR_OFFSET         10
#define    BSP_UICC_PHY_LLIF_USIM_TS_ERROR_WIDTH           1

#define    BSP_UICC_PHY_LLIF_EMV_ATR_LEN_TO_OFFSET        11
#define    BSP_UICC_PHY_LLIF_EMV_ATR_LEN_TO_WIDTH          1

#define    BSP_UICC_PHY_LLIF_REG_USIM_IT_OFFSET            0
#define    BSP_UICC_PHY_LLIF_REG_USIM_IT_WIDTH            16

#define    BSP_UICC_PHY_LLIF_INTERRUPT_CLEAR_ALL      0xFFFF
#define    BSP_UICC_PHY_LLIF_INTERRUPT_CLEAR               0

/****** Definitions for USIM_MASKIT Register *******/
#define    BSP_UICC_PHY_LLIF_MASK_USIM_NATR_OFFSET         0
#define    BSP_UICC_PHY_LLIF_MASK_USIM_NATR_WIDTH          1

#define    BSP_UICC_PHY_LLIF_MASK_USIM_WT_OFFSET           1
#define    BSP_UICC_PHY_LLIF_MASK_USIM_WT_WIDTH            1

#define    BSP_UICC_PHY_LLIF_MASK_USIM_OV_OFFSET           2
#define    BSP_UICC_PHY_LLIF_MASK_USIM_OV_WIDTH            1

#define    BSP_UICC_PHY_LLIF_MASK_USIM_TX_OFFSET           3
#define    BSP_UICC_PHY_LLIF_MASK_USIM_TX_WIDTH            1

#define    BSP_UICC_PHY_LLIF_MASK_USIM_RX_OFFSET           4
#define    BSP_UICC_PHY_LLIF_MASK_USIM_RX_WIDTH            1

#define    BSP_UICC_PHY_LLIF_MASK_USIM_CD_OFFSET           5
#define    BSP_UICC_PHY_LLIF_MASK_USIM_CD_WIDTH            1

#define    BSP_UICC_PHY_LLIF_MASK_USIM_EOB_OFFSET          6
#define    BSP_UICC_PHY_LLIF_MASK_USIM_EOB_WIDTH           1

#define    BSP_UICC_PHY_LLIF_MASK_USIM_TOC_OFFSET          7
#define    BSP_UICC_PHY_LLIF_MASK_USIM_TOC_WIDTH           1

#define    BSP_UICC_PHY_LLIF_MASK_USIM_TOB_OFFSET          8
#define    BSP_UICC_PHY_LLIF_MASK_USIM_TOB_WIDTH           1

#define    BSP_UICC_PHY_LLIF_MASK_USIM_RESENT_OFFSET       9
#define    BSP_UICC_PHY_LLIF_MASK_USIM_RESENT_WIDTH        1

#define    BSP_UICC_PHY_LLIF_MASK_USIM_TS_ERROR_OFFSET    10
#define    BSP_UICC_PHY_LLIF_MASK_USIM_TS_ERROR_WIDTH      1

#define    BSP_UICC_PHY_LLIF_MASK_EMV_ATR_LEN_TO_OFFSET   11
#define    BSP_UICC_PHY_LLIF_MASK_EMV_ATR_LEN_TO_WIDTH     1

#define    BSP_UICC_PHY_LLIF_REG_USIM_MASKIT_OFFSET        0
#define    BSP_UICC_PHY_LLIF_REG_USIM_MASKIT_WIDTH        16

#define    BSP_UICC_PHY_LLIF_INTERRUPT_MASK_ALL        0xFFF
#define    BSP_UICC_PHY_LLIF_INTERRUPT_MASK                1
#define    BSP_UICC_PHY_LLIF_INTERRUPT_UNMASK              0

/****** Definitions for USIM_DRX Register **********/
#define    BSP_UICC_PHY_LLIF_USIM_DRX_OFFSET               0
#define    BSP_UICC_PHY_LLIF_USIM_DRX_WIDTH                8

#define    BSP_UICC_PHY_LLIF_USIM_STATRXPAR_OFFSET         8
#define    BSP_UICC_PHY_LLIF_USIM_STATRXPAR_WIDTH          1

#define    BSP_UICC_PHY_LLIF_REG_USIM_DRX_OFFSET           0
#define    BSP_UICC_PHY_LLIF_REG_USIM_DRX_WIDTH            9

/****** Definitions for USIM_DTX Register **********/
#define    BSP_UICC_PHY_LLIF_REG_USIM_DTX_OFFSET           0
#define    BSP_UICC_PHY_LLIF_REG_USIM_DTX_WIDTH            8

/****** Definitions for USIM_FIFOS Register ********/
#define    BSP_UICC_PHY_LLIF_FIFO_DMA_MODE_OFFSET          0
#define    BSP_UICC_PHY_LLIF_FIFO_DMA_MODE_WIDTH           1

#define    BSP_UICC_PHY_LLIF_FIFO_ENABLE_OFFSET            1
#define    BSP_UICC_PHY_LLIF_FIFO_ENABLE_WIDTH             1

#define    BSP_UICC_PHY_LLIF_FIFO_TX_TRIGGER_OFFSET        2
#define    BSP_UICC_PHY_LLIF_FIFO_TX_TRIGGER_WIDTH         4

#define    BSP_UICC_PHY_LLIF_FIFO_TX_RESET_OFFSET          6
#define    BSP_UICC_PHY_LLIF_FIFO_TX_RESET_WIDTH           1

#define    BSP_UICC_PHY_LLIF_FIFO_TX_EMPTY_OFFSET          7
#define    BSP_UICC_PHY_LLIF_FIFO_TX_EMPTY_WIDTH           1

#define    BSP_UICC_PHY_LLIF_FIFO_TX_FULL_OFFSET           8
#define    BSP_UICC_PHY_LLIF_FIFO_TX_FULL_WIDTH            1

#define    BSP_UICC_PHY_LLIF_FIFO_RX_TRIGGER_OFFSET        9
#define    BSP_UICC_PHY_LLIF_FIFO_RX_TRIGGER_WIDTH         4

#define    BSP_UICC_PHY_LLIF_FIFO_RX_RESET_OFFSET          13
#define    BSP_UICC_PHY_LLIF_FIFO_RX_RESET_WIDTH           1

#define    BSP_UICC_PHY_LLIF_FIFO_RX_EMPTY_OFFSET          14
#define    BSP_UICC_PHY_LLIF_FIFO_RX_EMPTY_WIDTH           1

#define    BSP_UICC_PHY_LLIF_FIFO_RX_FULL_OFFSET           15
#define    BSP_UICC_PHY_LLIF_FIFO_RX_FULL_WIDTH            1

#define    BSP_UICC_PHY_LLIF_REG_USIM_FIFOS_OFFSET         0
#define    BSP_UICC_PHY_LLIF_REG_USIM_FIFOS_WIDTH          16

enum
{
    BSP_UICC_PHY_LLIF_FIFO_DISABLE = 0,
    BSP_UICC_PHY_LLIF_FIFO_ENABLE  = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_FifoEnable;

enum
{
    BSP_UICC_PHY_LLIF_FIFO_OPT_TX_TRIG_SIZE   = 3,
    BSP_UICC_PHY_LLIF_FIFO_MAX_TX_SIZE        = 0x9
};
typedef SYS_UWORD8 BspUicc_Usim_llif_FifoTxTriggerSize;

enum
{
    BSP_UICC_PHY_LLIF_FIFO_MIN_RX_TRIG_SIZE  = 1,
    BSP_UICC_PHY_LLIF_FIFO_OPT_RX_TRIG_SIZE  = 0xD

};
typedef SYS_UWORD8 BspUicc_Usim_llif_FifoRxTriggerSize;
enum
{
    BSP_UICC_PHY_LLIF_FIFO_TX_RESET_DISABLE = 0,
    BSP_UICC_PHY_LLIF_FIFO_TX_RESET_ENABLE  = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_FifoTxReset;
enum
{
    BSP_UICC_PHY_LLIF_FIFO_TX_EMPTY_DISABLED= 0,
    BSP_UICC_PHY_LLIF_FIFO_TX_EMPTY_ENABLED = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_FifoTxEmpty;
enum
{
    BSP_UICC_PHY_LLIF_FIFO_TX_FULL_DISABLED = 0,
    BSP_UICC_PHY_LLIF_FIFO_TX_FULL_ENABLED  = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_FifoTxFull;
enum
{
    BSP_UICC_PHY_LLIF_FIFO_RX_RESET_DISABLE = 0,
    BSP_UICC_PHY_LLIF_FIFO_RX_RESET_ENABLE  = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_FifoRxReset;
enum
{
    BSP_UICC_PHY_LLIF_FIFO_RX_EMPTY_DISABLED = 0,
    BSP_UICC_PHY_LLIF_FIFO_RX_EMPTY_ENABLED  = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_FifoRxEmpty;
enum
{
    BSP_UICC_PHY_LLIF_FIFO_RX_FULL_DISABLED  = 0,
    BSP_UICC_PHY_LLIF_FIFO_RX_FULL_ENABLED   = 1
};
typedef SYS_UWORD8 BspUicc_Usim_llif_FifoRxFull;

/****** Definitions for USIM_CGT Register ********/
#define    BSP_UICC_PHY_LLIF_USIM_CGT_OFFSET                   0
#define    BSP_UICC_PHY_LLIF_USIM_CGT_WIDTH                    8

typedef SYS_UWORD8 BspUicc_Usim_llif_UsimCgt;

/****** Definitions for USIM_CWT Register ********/
#define    BSP_UICC_PHY_LLIF_USIM_CWT_OFFSET                   0
#define    BSP_UICC_PHY_LLIF_USIM_CWT_WIDTH                    16

typedef SYS_UWORD16 BspUicc_Usim_llif_UsimCwt;

/****** Definitions for USIM_BWT_LSB Register ********/
#define    BSP_UICC_PHY_LLIF_USIM_BWT_LSB_OFFSET               0
#define    BSP_UICC_PHY_LLIF_USIM_BWT_LSB_WIDTH                16

typedef SYS_UWORD16 BspUicc_Usim_llif_UsimBwtLsb;

/****** Definitions for USIM_BWT_MSB Register ********/
#define    BSP_UICC_PHY_LLIF_USIM_BWT_MSB_OFFSET               0
#define    BSP_UICC_PHY_LLIF_USIM_BWT_MSB_WIDTH                7


/****** Definitions for DEBUG_REG Register ********/
#define    BSP_UICC_PHY_LLIF_DEBUG_RX_SM_OFFSET                6
#define    BSP_UICC_PHY_LLIF_DEBUG_RX_SM_WIDTH                 2

#define    BSP_UICC_PHY_LLIF_DEBUG_TX_SM_OFFSET                4
#define    BSP_UICC_PHY_LLIF_DEBUG_TX_SM_WIDTH                 2

#define    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SM_OFFSET        0
#define    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SM_WIDTH         4

#define    BSP_UICC_PHY_LLIF_DEBUG_REG_OFFSET                  0
#define    BSP_UICC_PHY_LLIF_DEBUG_REG_WIDTH                   16

enum
{
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_NO_CONNECT      = 0,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SVCC_ON         = 1,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SIO_RX          = 2,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SCLK_ON         = 3,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_WAIT_ATR_INT    = 4,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SRST_ON         = 5,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_WAIT_ATR_EXT    = 6,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_CLOCK_STOP      = 7,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_CLOCK_RESTART   = 8,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_WARM_RESET      = 9,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_DECODE_TS       = 10,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_COMMUNICATION   = 11,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SRST_OFF        = 12,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SCLK_OFF        = 13,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SIO_TX          = 14,
    BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_TDSIM           = 15
};

typedef SYS_UWORD16 BspUicc_Usim_llif_DebugMainState;

/****** Definitions for CONF_SAM1_DIV Register ********/
#define    BSP_UICC_PHY_LLIF_SAM1_DIV_OFFSET                    0
#define    BSP_UICC_PHY_LLIF_SAM1_DIV_WIDTH                    12


/****** Definitions for CONF4_REG Register ********/
#define    BSP_UICC_PHY_LLIF_CONF_WAITI_OFFSET                  0
#define    BSP_UICC_PHY_LLIF_CONF_WAITI_WIDTH                  13

#define    BSP_UICC_PHY_LLIF_REG_CONF4_REG_OFFSET               0
#define    BSP_UICC_PHY_LLIF_REG_CONF4_REG_WIDTH                16

/****** Definitions for ATR_CLK_PRD_NBS Register ********/
#define    BSP_UICC_PHY_LLIF_CLK_NBR_B4_ATR_OFFSET              0
#define    BSP_UICC_PHY_LLIF_CLK_NBR_B4_ATR_WIDTH              16

/****** Definitions for CONF_ETU_DIV Register ********/
#define    BSP_UICC_PHY_LLIF_CONF_ETU_DIV_OFFSET                0
#define    BSP_UICC_PHY_LLIF_CONF_ETU_DIV_WIDTH                16

#define    BSP_UICC_PHY_LLIF_CONF_ETU_DIV_RESET_VAL         0x5CF

/****** Definitions for CONF5_REG Register ********/
#define    BSP_UICC_PHY_LLIF_SOFT_NHARD_FIDI_PROG_OFFSET        8
#define    BSP_UICC_PHY_LLIF_SOFT_NHARD_FIDI_PROG_WIDTH         1

#define    BSP_UICC_PHY_LLIF_CONF_FI_PROG_OFFSET                4
#define    BSP_UICC_PHY_LLIF_CONF_FI_PROG_WIDTH                 4

#define    BSP_UICC_PHY_LLIF_CONF_DI_PROG_OFFSET                0
#define    BSP_UICC_PHY_LLIF_CONF_DI_PROG_WIDTH                 4

#define    BSP_UICC_PHY_LLIF_REG_CONF5_REG_OFFSET               0
#define    BSP_UICC_PHY_LLIF_REG_CONF5_REG_WIDTH                16

#define    BSP_UICC_PHY_LLIF_SOFT_NHARD_FIDI_PROG_HW            0
#define    BSP_UICC_PHY_LLIF_SOFT_NHARD_FIDI_PROG_SW            1



/*=============================================================================
 * Types
 */
/*=============================================================================
 * Description:
 * Type for a 16 bit Register
 */
typedef SYS_UWORD16 BspUicc_Usim_llif_Reg;


/*=============================================================================
 * Utility Macros
 */
/*=============================================================================
 * Description:
 *    Gets a value from a local (shadowed or in local memory) register.
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 */
#define BSP_UICC_PHY_LLIF_GET_LOCAL( _name,                      \
                                     _ptr )                      \
(                                                                \
    BSPUTIL_BITUTIL_BIT_FIELD_GET( (_ptr),                       \
                                    BSPUTIL_BITUTIL_DATAUNIT_16, \
                                    _name ##_OFFSET,             \
                                    _name ##_WIDTH )             \
)


/*=============================================================================
 * Description:
 *    Sets a value into a  local (shadowed or in local memory) register field.
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 *    _newValue - new value for the field.
 */
#define BSP_UICC_PHY_LLIF_SET_LOCAL( _name,                     \
                                     _ptr,                      \
                                     _newValue )                \
{                                                               \
    BSPUTIL_BITUTIL_BIT_FIELD_SET( (_ptr),                      \
                                   (_newValue),                 \
                                   BSPUTIL_BITUTIL_DATAUNIT_16, \
                                   _name ##_OFFSET,             \
                                   _name ##_WIDTH );            \
}

/*=============================================================================
 * Description:
 *    Gets a value from a register field. (XXXNote, don't use this for
 *    registers that must also be shadowed).
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 */

#define BSP_UICC_PHY_LLIF_GET( _name,                           \
                               _ptr )                           \
(                                                               \
    BSPUTIL_BITUTIL_BIT_FIELD_GET( (_ptr),                               \
                          BSPUTIL_BITUTIL_DATAUNIT_16,          \
                          _name ##_OFFSET,                      \
                          _name ##_WIDTH )                      \
)


/*=============================================================================
 * Description:
 *    Sets a value into a register field. (XXXNote, don't use this for
 *    registers that must also be shadowed).
 *
 *    _name - the name of a field, without the trailing _OFFSET or _WIDTH
 *    _ptr - pointer to the register that the field is in.
 *    _newValue - new value for the field.
 */

#define BSP_UICC_PHY_LLIF_SET( _name,                           \
                               _ptr,                            \
                               _newValue )                      \
{                                                               \
    BSPUTIL_BITUTIL_BIT_FIELD_SET( (_ptr),                              \
                           (_newValue),                         \
                           BSPUTIL_BITUTIL_DATAUNIT_16,         \
                           _name ##_OFFSET,                     \
                           _name ##_WIDTH );                    \
}

#endif














