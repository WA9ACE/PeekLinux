/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */


#include "sys_types.h"
#include "types.h"
#include "bspUicc_Phy.h"
#include "bspUicc_Phy_map.h"
#include "bspUicc_Phy_llif.h"
#include "bspUicc_Phy_intC.h"
#include "nucleus.h"
//#include "bspDelay.h"

/*=============================================================================
 *  File Description:
 *  Source File with code specific to accessing the Usim hardware block
 */

/*=============================================================================
 *  Static Data
 */

/*=============================================================================
 * Description:
 *   Static structure corressponding to the Usim hardware block
 */

extern BspUicc_Phy_Ctrl    bspUicc_Phy_ctrl;
extern BspUicc_Phy_RxCtrl  bspUicc_Phy_rxCtrl;
extern BspUicc_Phy_TxCtrl  bspUicc_Phy_txCtrl;

/*=============================================================================
 * Description:
 *   globals for CLOCKSTOP WA
 */

extern volatile SYS_BOOL waittingToSleep;
extern SYS_UWORD8 expectedDataCount;
extern SYS_UWORD8 dataCount;

extern BspUicc_clockStopWAstate clockStopWAstate;
extern SYS_UWORD8 rx_trigger_value;


/*=============================================================================
  *  Keeps the state of smartcard interface clock bspUicc_sClk_running
  *  set to FALSE on successsful CLOCK_STOP
  *  we reset it on RESET aswell
  *  This will also be used for powermgmt
  */
volatile SYS_BOOL bspUicc_sClk_running = TRUE;


//#define FEATURE_UICC_DISABLE_PARITY_WA

#ifndef FEATURE_UICC_DISABLE_PARITY_WA
extern Uint16 Fi_Ta1_Mapping[];
extern Uint8 Di_Ta1_Mapping[];
extern Uint8 bspUicc_Lnk_resetMgrMaxFreqValues[];
SYS_UWORD16 bspUicc_EtuValue=0,bspUicc_Phy_highSpeed=0;
Uint8 bspUicc_start_timer=FALSE;
Uint16 bspUicc_orig_cgt_value=0xd;
#define CHANGE_CGT
#endif


BspUicc_Phy_ReturnCode card_insert_status=BSP_UICC_PHY_LLIF_STATNOCARD_ABSENT;
/*=============================================================================
 * Private Functions
 *=============================================================================*/

/*=============================================================================
 * Public Functions
 *=============================================================================*/

#ifndef FEATURE_UICC_DISABLE_PARITY_WA
void uicc_timer_callback(void);
LOCAL void uicc_timer_callback2(void)
{

	/* reset fifo */
	BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_TX_RESET,
       	             BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
               	      BSP_UICC_PHY_LLIF_FIFO_TX_RESET_ENABLE );

   	BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_TX_RESET,
	                       BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
                           BSP_UICC_PHY_LLIF_FIFO_TX_RESET_DISABLE );
        Dtimer1_Register (BSP_UICC_PARITY_TIMER_ETU_VALUE*(bspUicc_EtuValue+bspUicc_Phy_highSpeed),TRUE,uicc_timer_callback);
 	 Dtimer1_Start (TRUE);

}


 void uicc_timer_callback(void)
{

 	/* parity error occured */
	if (BSP_UICC_PHY_LLIF_GET(BSP_UICC_PHY_LLIF_STATTXPAR,
	  	BSP_UICC_PHY_MAP_REG_USIM_STAT_PTR)==0)
			{
#ifdef CHANGE_CGT
			bspUicc_orig_cgt_value= BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_USIM_CGT,
                           BSP_UICC_PHY_MAP_REG_USIM_CGT_PTR);
		       BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_USIM_CGT,
                           BSP_UICC_PHY_MAP_REG_USIM_CGT_PTR, BSP_UICC_PARITY_CGT_VALUE);
#endif
 			BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_DTX,
                      BSP_UICC_PHY_MAP_REG_USIM_DTX_PTR,0);

	  	        Dtimer1_Register (BSP_UICC_FIFO_RESET_ETU_VALUE*bspUicc_EtuValue,FALSE,uicc_timer_callback2);
		        Dtimer1_Start (TRUE);
			}
	else if(BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_X_MODE,
                                  BSP_UICC_PHY_MAP_REG_USIM_STAT_PTR ) ==
           				BSP_UICC_PHY_LLIF_CONFTXNRX_RX )
           //switched to RX mode. No parity error. Parity error prevents switching to RX mode.
			{
			    Dtimer1_Start (FALSE); //stop parity timer
			}
}

#endif




/*=============================================================================
 * Description:
 *    Initialize the Usim Hardware block
 */
void
bspUicc_Phy_init( void )
{
    bspUicc_Phy_rxCtrl.outIndex = 0;
    bspUicc_Phy_rxCtrl.rxIndex  = 0;
    bspUicc_Phy_rxCtrl.expectedData = 0;
    bspUicc_Phy_rxCtrl.remainingData = 0;
    bspUicc_Phy_txCtrl.txIndex = 0;
    bspUicc_Phy_txCtrl.noTxBytes = 0;
    bspUicc_Phy_ctrl.error = 0;
}

/*=============================================================================
 * Description:
 *    Configure the Usim Hardware block
 */
BspUicc_Phy_ReturnCode
bspUicc_Phy_configure( BspUicc_Phy_Protocol   protocol,
                       BspUicc_Phy_SclkDiv    sclkDiv,
                       BspUicc_Phy_EtuPeriod  etuPeriod,
                       BspUicc_Phy_Cgt        cgt,
                       BspUicc_Phy_Cwt        cwt,
                       BspUicc_Phy_Bwt        bwt,
                       BspUicc_Phy_FiValue    fiValue,
                       BspUicc_Phy_DiValue    diValue,
                       BspUicc_Phy_WaitI      waitIValue,
                       BspUicc_Phy_SClkLev    sClkLev,
                       BspUicc_Phy_ConfigType configType )
{
    BspUicc_Phy_ReturnCode status;
    volatile SYS_UWORD16 tempReg = 0x0000;

    if( configType == BSP_UICC_PHY_CONFIG_TYPE_READ_WRITE )
    {
        /* Configure the physical layer with the properties
           indicated by the card in the ATR/PPS */

        bspUicc_Phy_ctrl.protocol = protocol;


        BSP_UICC_PHY_LLIF_SET_LOCAL( BSP_UICC_PHY_LLIF_CON_PROTOCOL,
                                     &tempReg,
                                     protocol );





        BSP_UICC_PHY_LLIF_SET_LOCAL( BSP_UICC_PHY_LLIF_CONFSCLKDIV,
                                     &tempReg,
                                     sclkDiv );

        BSP_UICC_PHY_LLIF_SET_LOCAL( BSP_UICC_PHY_LLIF_CONF_RESENT,
                                     &tempReg,
                                     BSP_UICC_PHY_LLIF_CONFRESENT_DEF_VAL );


        BSP_UICC_PHY_LLIF_SET_LOCAL( BSP_UICC_PHY_LLIF_CONFTXNRX,
                                     &tempReg,
                                     BSP_UICC_PHY_LLIF_CONFTXNRX_TX );

        BSP_UICC_PHY_LLIF_SET_LOCAL( BSP_UICC_PHY_LLIF_CONFCHKPAR,
                                     &tempReg,
                                     BSP_UICC_PHY_LLIF_CONFCHKPAR_ENABLE );


        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_CONF2,
                               BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR,
                               tempReg );


        tempReg = 0x0000;
        BSP_UICC_PHY_LLIF_SET_LOCAL( BSP_UICC_PHY_LLIF_CONF_WAITI,
                                     &tempReg,
                                     waitIValue );


         BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_CONF4_REG,
                                BSP_UICC_PHY_MAP_REG_USIM_CONF4_REG_PTR,
                                tempReg );



        tempReg = 0x0000;
        BSP_UICC_PHY_LLIF_SET_LOCAL( BSP_UICC_PHY_LLIF_CONF_FI_PROG,
                                     &tempReg,
                                     fiValue );

        BSP_UICC_PHY_LLIF_SET_LOCAL( BSP_UICC_PHY_LLIF_CONF_DI_PROG,
                                     &tempReg,
                                     diValue );

        BSP_UICC_PHY_LLIF_SET_LOCAL( BSP_UICC_PHY_LLIF_SOFT_NHARD_FIDI_PROG,
                                     &tempReg,
                                     BSP_UICC_PHY_LLIF_SOFT_NHARD_FIDI_PROG_HW );



        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_CONF5_REG,
                               BSP_UICC_PHY_MAP_REG_USIM_CONF5_REG_PTR,
                               tempReg );



        /* Configure Character guard time calculated using the ATR info*/

        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_USIM_CGT,
                               BSP_UICC_PHY_MAP_REG_USIM_CGT_PTR, cgt );


        /* Remember if Clock stop is supported or not
           using this clock stop config info */


        bspUicc_Phy_ctrl.sClkLev = sClkLev;

        if( sClkLev == BSP_UICC_PHY_SCLOCK_LEVEL_LOW )
        {
            BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFSCLKLEV,
                                   BSP_UICC_PHY_MAP_REG_USIM_CONF1_PTR,
                                   BSP_UICC_PHY_LLIF_CONFSCLKLEV_LOW );
        }
        else if( sClkLev == BSP_UICC_PHY_SCLOCK_LEVEL_HIGH )
        {
            BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFSCLKLEV,
                                   BSP_UICC_PHY_MAP_REG_USIM_CONF1_PTR,
                                   BSP_UICC_PHY_LLIF_CONFSCLKLEV_HIGH );

        }



        bspUicc_Phy_ctrl.mode = BSP_UICC_PHY_MODE_READ_WRITE;
    }
    /* Block wait time update, this is done while processing T= 1 blocks */
    else if( configType == BSP_UICC_PHY_CONFIG_TYPE_BWT_UPDATE )
    {
        /* bwt value is spread over 2 registers */
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_USIM_BWT_LSB,
                               BSP_UICC_PHY_MAP_REG_USIM_BWT_LSB_PTR,
                               (SYS_UWORD16) (bwt & 0x0000FFFF) );

        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_USIM_BWT_MSB,
                               BSP_UICC_PHY_MAP_REG_USIM_BWT_MSB_PTR,
                               ( bwt >> BSP_UICC_PHY_LLIF_USIM_BWT_LSB_WIDTH) );
    }

else if( configType == BSP_UICC_PHY_CONFIG_TYPE_CLKSTOP_UPDATE )
    {
         bspUicc_Phy_ctrl.sClkLev = sClkLev;

        if( sClkLev == BSP_UICC_PHY_SCLOCK_LEVEL_LOW )
        {
            BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFSCLKLEV,
                                   BSP_UICC_PHY_MAP_REG_USIM_CONF1_PTR,
                                   BSP_UICC_PHY_LLIF_CONFSCLKLEV_LOW );
        }
        else if( sClkLev == BSP_UICC_PHY_SCLOCK_LEVEL_HIGH )
        {
            BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFSCLKLEV,
                                   BSP_UICC_PHY_MAP_REG_USIM_CONF1_PTR,
                                   BSP_UICC_PHY_LLIF_CONFSCLKLEV_HIGH );

        }
    }

#ifndef FEATURE_UICC_DISABLE_PARITY_WA
	/* Check FI DI and set bspUicc_Phy_highSpeed */
	bspUicc_EtuValue=(Fi_Ta1_Mapping[fiValue]/
		(Di_Ta1_Mapping[diValue]*
		bspUicc_Lnk_resetMgrMaxFreqValues[fiValue]))	;
/*increasing timer timeout delay for hifh speed sims (otherwise on high speed sims ,too frequent interrupts causue system freeze */
	if(bspUicc_EtuValue<10)
		bspUicc_Phy_highSpeed = Di_Ta1_Mapping[diValue];
	else
		bspUicc_Phy_highSpeed = 0;

	bspUicc_orig_cgt_value= BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_USIM_CGT,
                           BSP_UICC_PHY_MAP_REG_USIM_CGT_PTR);

#endif

    status = BSP_UICC_PHY_RETURN_CODE_SUCCESS;

    return status;
}

/*=============================================================================
 * Description:
 *    Warm reset of the Usim Hardware block
 */
void bspUicc_Phy_warmReset( void )
{
    /* Set direction to reception */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFTXNRX,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR,
                           BSP_UICC_PHY_LLIF_CONFTXNRX_RX );

    while( BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_X_MODE,
                                  BSP_UICC_PHY_MAP_REG_USIM_STAT_PTR ) !=
           BSP_UICC_PHY_LLIF_CONFTXNRX_RX ){};

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_WARM_RESET_CMD,
                           BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                           BSP_UICC_PHY_LLIF_WARM_RESET_ENABLE );

    /* Wait till the Usim hardware switches its state */
    while( BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SM,
                                  BSP_UICC_PHY_MAP_REG_USIM_DEBUG_REG_PTR ) !=
           BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_WARM_RESET ){};

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_WARM_RESET_CMD,
                           BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                           BSP_UICC_PHY_LLIF_WARM_RESET_DISABLE );
	    bspUicc_sClk_running = TRUE;
}

#if  0

/*==============================================================================
 * Description:
 *   bspUicc_sleep()
 *   issue a STATUS command and handle the response as per spec.
 * . As soon as last byte is received RxInt Handler will set CLOCK_STOP_CMD and reset the flag  waittingToSleep.
 *   So we guarantee the command write before Tg expiry hence a proper synchronisation in UICC FSM.
 *
 */
SYS_UWORD16 bspUicc_sleep(SYS_UWORD16 Le)
 {
     SYS_UWORD8 InvalidCmd[5]={0xA0,0xF2,0x00,0x00,0x01};
     SYS_UWORD16 result;
	 volatile SYS_UWORD32        timeOut=0;

     if ((bspUicc_sClk_running == TRUE)
	 	 &&(bspUicc_Phy_ctrl.sClkLev != BSP_UICC_PHY_SCLOCK_NO_STOP))
     {
         if(sleepSequence != BSP_UICC_SLEEP_SEQ_NORMAL)
         {
             gsp_disable();
             BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_RX_TRIGGER,
                                    BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR, 0x1 );
             gsp_enable();
             waittingToSleep = TRUE;

            #ifdef SIM_DRIVER_CLOCKSTOP_DEBUG
             Rxindex =0;
            #endif
			 dataCount =0;
             /* comment_this_sandy_JUL10 */
			  InvalidCmd[4] = Le;
			 /* complete replica of the original status command */
      	     expectedDataCount = InvalidCmd[4];
			 clockStopWAstate = WAIT_FOR_ACK_NAK_SW_NULL;
		     bspUicc_Phy_write(5, InvalidCmd);

            while((waittingToSleep == TRUE)
                  && (timeOut < BSP_UICC_READ_TIMEOUT_VALUE)
                  && (bspUicc_Phy_ctrl.error == BSP_UICC_PHY_NO_ERROR) )
            {
                timeOut++;
                NU_Sleep(1);
            }
           /* switch to TX mode as we are not expecting more bytes */
            bspUicc_Phy_setDir( BSP_UICC_PHY_DIRECTION_TX );

		   /* we will ignore the error and continue with old sequence if invalid command write or read failed due to timeout or other errors */
           if((bspUicc_Phy_ctrl.error != BSP_UICC_PHY_NO_ERROR ) || (timeOut == BSP_UICC_READ_TIMEOUT_VALUE))
           {
			   waittingToSleep = FALSE;
               bspUicc_Phy_ctrl.error = 0; /*  clear the underflow error  */
               sleepSequence =	BSP_UICC_SLEEP_SEQ_NORMAL;
			   /*  This will have risk of FSM issue still better than avoiding deep sleep*/
			   BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_CLOCK_STOP_CMD,
									 BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
									 BSP_UICC_PHY_CLOCK_STOP_ENABLE);

           }

	    }
	    else if(sleepSequence == BSP_UICC_SLEEP_SEQ_NORMAL)
	    {
            /*  This will have risk of FSM issue still better than avoiding deep sleep*/
            BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_CLOCK_STOP_CMD,
		                          BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
			    				  BSP_UICC_PHY_CLOCK_STOP_ENABLE);
        }

        /*we have issued the clock stop command . Wait till the Usim hardware switches its state with a max wait of ~5s */
        timeOut =0;
        while ((BSP_UICC_PHY_LLIF_GET(BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SM,
		                 BSP_UICC_PHY_MAP_REG_USIM_DEBUG_REG_PTR) !=
                         BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_CLOCK_STOP)
                         #ifdef BSP_UICC_CLOCKSTOP_FSM_RECOVERY
		                 &&(timeOut < WAIT_100mS)
                         #endif
						 )
        {
           #ifdef BSP_UICC_CLOCKSTOP_FSM_RECOVERY
			timeOut++;
			NU_Sleep(1);
           #endif
        };

        if( timeOut == WAIT_100mS)
        {
            result = BSP_UICC_SLEEP_SEQ_FAIL;
        }
        else
        {
            result = sleepSequence;
            bspUicc_sClk_running = FALSE;
        }
     }

      #ifdef SIM_DRIVER_CLOCKSTOP_DEBUG
	   TRACE_FUNCTION_P1(" Response bytes = [%x]",Rxindex);
	   if(Rxindex >1)
	   TRACE_FUNCTION_P2(" SW1 = [%x] SW2 = [%x]", uiccDebugBuffer[Rxindex-2], uiccDebugBuffer[Rxindex-1]);
	   TRACE_FUNCTION_P1(" FSM timeOut = [%x]", timeOut);
      #endif

     return result;
 }
#endif

/*==============================================================================
 * Description:
 * Disable clock stop (includes workaround for missing resynchronization
 * between the register clock_stop command write and its input in the sequencer
 * causing warm reset in some cases.
 */
void
bspUicc_Phy_sleepModeDisable(void)
{
    volatile Uint16 sam1DivSave;
    volatile Uint16 fiDiModeSave;
    if ((bspUicc_Phy_ctrl.sClkLev != BSP_UICC_PHY_SCLOCK_NO_STOP)
		&&(bspUicc_sClk_running == FALSE))
    {
        /* before Tg expiry if we reach here  */
    if (BSP_UICC_PHY_LLIF_GET(BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SM,
                              BSP_UICC_PHY_MAP_REG_USIM_DEBUG_REG_PTR) !=
        BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_CLOCK_STOP)
    {
        return;
    }

    /* Store FIDI mode */
    fiDiModeSave =
        BSP_UICC_PHY_LLIF_GET(BSP_UICC_PHY_LLIF_SOFT_NHARD_FIDI_PROG,
                              BSP_UICC_PHY_MAP_REG_USIM_CONF5_REG_PTR);

    /* Set SW FIDI mode */
    BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_SOFT_NHARD_FIDI_PROG,
                          BSP_UICC_PHY_MAP_REG_USIM_CONF5_REG_PTR,
                          BSP_UICC_PHY_LLIF_SOFT_NHARD_FIDI_PROG_SW);

    /* Store SAM clock ratio */
    sam1DivSave =
        BSP_UICC_PHY_LLIF_GET(BSP_UICC_PHY_LLIF_SAM1_DIV,
                              BSP_UICC_PHY_MAP_REG_USIM_CONF_SAM1_DIV_PTR);

    /* Set SAM clock ratio to "zero" to cut off the clock */
    BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_SAM1_DIV,
                          BSP_UICC_PHY_MAP_REG_USIM_CONF_SAM1_DIV_PTR,
                          0x00);

	NU_Sleep(1);

    /* Disable Module Clock */
    BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_MODULE_CLK_EN,
                          BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                          BSP_UICC_PHY_LLIF_MODULE_CLK_DISABLE);


	NU_Sleep(1);

    /* Deactivate Clock Stop mode */
    BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_CLOCK_STOP_CMD,
                          BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                          BSP_UICC_PHY_CLOCK_STOP_DISABLE);

    /* Restore SAM clock ratio */
    BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_SAM1_DIV,
                          BSP_UICC_PHY_MAP_REG_USIM_CONF_SAM1_DIV_PTR,
                          sam1DivSave);

    /* Restor FIDI mode */
    BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_SOFT_NHARD_FIDI_PROG,
                          BSP_UICC_PHY_MAP_REG_USIM_CONF5_REG_PTR,
                          fiDiModeSave);

    /* Enable Module Clock */
    BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_MODULE_CLK_EN,
                          BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                          BSP_UICC_PHY_LLIF_MODULE_CLK_ENABLE);

    /* Wait till the Usim hardware switches its state */
    while (BSP_UICC_PHY_LLIF_GET(BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SM,
                                 BSP_UICC_PHY_MAP_REG_USIM_DEBUG_REG_PTR) !=
           BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_COMMUNICATION){};
        /* set SCLK running */
	    bspUicc_sClk_running = TRUE;
	}
}

/*=============================================================================
 * Description:
 *    Reset the Usim Hardware block
 */
BspUicc_Phy_ReturnCode
bspUicc_Phy_reset( void )
{
    BspUicc_Phy_ReturnCode status;
    /* Mask all to avoid int generation after clk enable */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_MASKIT,
                           BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                           BSP_UICC_PHY_LLIF_INTERRUPT_MASK_ALL );

    /* Clear command Register */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_CMD,
                           BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                           BSP_UICC_PHY_LLIF_REG_USIM_CMD_CLEAR_ALL );

    /* Enable Module Clock */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_MODULE_CLK_EN,
                           BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                           BSP_UICC_PHY_LLIF_MODULE_CLK_ENABLE );

    /* Set and clear Tx & Rx Reset Fifos bits */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_TX_RESET,
                           BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
                           BSP_UICC_PHY_LLIF_FIFO_TX_RESET_ENABLE );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_TX_RESET,
                           BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
                           BSP_UICC_PHY_LLIF_FIFO_TX_RESET_DISABLE );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_RX_RESET,
                           BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
                           BSP_UICC_PHY_LLIF_FIFO_RX_RESET_ENABLE );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_RX_RESET,
                           BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
                           BSP_UICC_PHY_LLIF_FIFO_RX_RESET_DISABLE );

    /* Soft Reset the UICC device */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CMDIFRST,
                           BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                           BSP_UICC_PHY_LLIF_CMDIFRST_ENABLE );

    /* Registers are not reset after soft reset.
       Explicitly reset all the registers        */
    /* Replace this by many Set locals & then Set the register */
	/* Changed from 0x02 to 0x00 as the CONFSIOLOW is only used when
	 * CONFBYPASS is set */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_CONF1,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF1_PTR, 0x00 );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_CONF2,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR, 0x06);

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_CONF3,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF3_PTR, 0x40 );

    /* Values Set To FIFORX_EMPTY and FIFOTX_EMPTY (0x4080) (Default)
     * Changed from FIFO_RX_FULL and FIFOTX_FULL (0x8100) */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_FIFOS,
                           BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR, 0x4080 );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_USIM_CGT,
                           BSP_UICC_PHY_MAP_REG_USIM_CGT_PTR, 0xD );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_USIM_CWT,
                           BSP_UICC_PHY_MAP_REG_USIM_CWT_PTR, 0x200B );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_USIM_BWT_LSB,
                           BSP_UICC_PHY_MAP_REG_USIM_BWT_LSB_PTR, 0x3C0B );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_USIM_BWT_MSB,
                           BSP_UICC_PHY_MAP_REG_USIM_BWT_MSB_PTR, 0x00 );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_SAM1_DIV,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF_SAM1_DIV_PTR, 0xB9 );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONF_WAITI,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF4_REG_PTR, 0x09 );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONF_ETU_DIV,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF_ETU_DIV_PTR, 0x5CF );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_CONF5_REG,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF5_REG_PTR, 0x101 );

    /* clear all ints */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_IT,
                        BSP_UICC_PHY_MAP_REG_USIM_IT_PTR,
                        BSP_UICC_PHY_LLIF_INTERRUPT_CLEAR_ALL );

    /* Disable module Clock  */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_MODULE_CLK_EN,
                           BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                           BSP_UICC_PHY_LLIF_MODULE_CLK_DISABLE );

    /* Unmask all ints */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_MASKIT,
                           BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                           0x00 );

    /* Mask Tx int as it is irrelevant for ATR */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_MASK_USIM_TX,
                           BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                           BSP_UICC_PHY_LLIF_INTERRUPT_MASK );

    /* Enable Fifo */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_ENABLE,
                           BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
                           BSP_UICC_PHY_LLIF_FIFO_ENABLE );

    /* Set Automatic Resend value */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONF_RESENT,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR,
                           BSP_UICC_PHY_LLIF_CONFRESENT_DEF_VAL );

    /* Set Rx trigger value to optimum value & Tx trigger to 3*/
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_RX_TRIGGER,
                           BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR, rx_trigger_value );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_TX_TRIGGER,
                           BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR, 0x3 );

    /* Set direction to reception */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFTXNRX,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR,
                           BSP_UICC_PHY_LLIF_CONFTXNRX_RX );

    bspUicc_Phy_ctrl.mode = BSP_UICC_PHY_MODE_ATR;
    bspUicc_Phy_ctrl.dirn = BSP_UICC_PHY_DIRECTION_RX;
    /*set it to running  */
	bspUicc_sClk_running = TRUE;
   /* Enable Module Clock */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_MODULE_CLK_EN,
                           BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                           BSP_UICC_PHY_LLIF_MODULE_CLK_ENABLE );


    while( BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_X_MODE,
                                  BSP_UICC_PHY_MAP_REG_USIM_STAT_PTR ) !=
           BSP_UICC_PHY_LLIF_CONFTXNRX_RX ){};

    /* Activate the Start sequence */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CMDSTART,
                           BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                           BSP_UICC_PHY_LLIF_CMDSTART_ENABLE );



    status = BSP_UICC_PHY_RETURN_CODE_SUCCESS;
    return status;
}



/*=============================================================================
 * Description:
 *    Write the specified number of bytes to the Usim Hardware block
 */
BspUicc_Phy_ReturnCode
bspUicc_Phy_write( BspUicc_Phy_DataSize   txDataSize,
                   BspUicc_Phy_TxData     *dataPtr )
{
    SYS_UWORD16                    i;
    BspUicc_Phy_ReturnCode   txStatus;
    BspUicc_Phy_DataSize     firstTxSize;
#ifndef FEATURE_UICC_DISABLE_PARITY_WA
    BspUicc_Phy_ReturnCode   waStatus;
#endif
    txStatus = BSP_UICC_PHY_RETURN_CODE_SUCCESS;

    bspUicc_Phy_ctrl.dirn = BSP_UICC_PHY_DIRECTION_TX;
    bspUicc_Phy_txCtrl.noTxBytes = txDataSize;
    bspUicc_Phy_txCtrl.txIndex = 0;

     /* Init Rx control data */
	            bspUicc_Phy_rxCtrl.outIndex = 0;
	            bspUicc_Phy_rxCtrl.rxIndex = 0;
	            bspUicc_Phy_rxCtrl.expectedData = 0;
	            bspUicc_Phy_rxCtrl.remainingData = 0;


    for(i =0; i<txDataSize; i++,dataPtr++)
    {
        bspUicc_Phy_txCtrl.txBuffer[i] = *dataPtr;
    }

    if( txDataSize < BSP_UICC_PHY_LLIF_FIFO_MAX_TX_SIZE )
    {
        firstTxSize = txDataSize;
    }
    else
    {
        firstTxSize = BSP_UICC_PHY_LLIF_FIFO_MAX_TX_SIZE;
    }

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_TX_TRIGGER,
                           BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
                           BSP_UICC_PHY_LLIF_FIFO_OPT_TX_TRIG_SIZE );

    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_RX_TRIGGER,
                           BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR, 0x1 );

    /* Set IO direction to Tx */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFTXNRX,
                           BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR,
                           BSP_UICC_PHY_LLIF_CONFTXNRX_TX );

    /* Poll on the X mode Status bit to check
       if the transmit has been effective     */
    while( BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_X_MODE,
                                  BSP_UICC_PHY_MAP_REG_USIM_STAT_PTR ) !=
           BSP_UICC_PHY_LLIF_CONFTXNRX_TX ){};


    gsp_disable();
    /* Fill the fifo */
    for( i=0; i<firstTxSize; i++ )
    {
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_DTX,
                               BSP_UICC_PHY_MAP_REG_USIM_DTX_PTR,
                               bspUicc_Phy_txCtrl.txBuffer[bspUicc_Phy_txCtrl.txIndex] );
        bspUicc_Phy_txCtrl.txIndex++;
        /* Remaining data will be txed by the int handler */
        txStatus = txDataSize;
    }

    /* Done transmitting. Set Tx trigger to 0 */
    if( bspUicc_Phy_txCtrl.txIndex == bspUicc_Phy_txCtrl.noTxBytes )
    {

        /* Set direction to Rx */
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFTXNRX,
                               BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR,
                               BSP_UICC_PHY_LLIF_CONFTXNRX_RX );



#ifdef FEATURE_UICC_DISABLE_PARITY_WA

        /* Mask Tx */
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_MASK_USIM_TX,
                               BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                               BSP_UICC_PHY_LLIF_INTERRUPT_MASK );
#else
        /* Set Trig to 0 */
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_TX_TRIGGER,
                               BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR, 0 );

		/* Clear all interrupts to ensure that the spurious interrupts generated while filling the fifo are cleared*/
		BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_IT,
							   BSP_UICC_PHY_MAP_REG_USIM_IT_PTR,
							   BSP_UICC_PHY_LLIF_INTERRUPT_CLEAR_ALL );


        /* unmask Tx */
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_MASK_USIM_TX,
                               BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                               BSP_UICC_PHY_LLIF_INTERRUPT_UNMASK );
	 bspUicc_start_timer=TRUE;
#endif
    }
    else
    {

		/* Clear all interrupts to ensure that the spurious interrupts generated while filling the fifo are cleared*/
		BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_IT,
						   BSP_UICC_PHY_MAP_REG_USIM_IT_PTR,
						   BSP_UICC_PHY_LLIF_INTERRUPT_CLEAR_ALL );


        /* unmask Tx */
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_MASK_USIM_TX,
                               BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                               BSP_UICC_PHY_LLIF_INTERRUPT_UNMASK );
#ifndef FEATURE_UICC_DISABLE_PARITY_WA
	 bspUicc_start_timer=TRUE;
#endif
    }

    gsp_enable();
    return txStatus;
}




/*=============================================================================
 * Description:
 *    Read the specified number of bytes to the Usim Hardware block
 */
BspUicc_Phy_ReturnCode
bspUicc_Phy_read( BspUicc_Phy_DataSize   rxDataSize,
                  BspUicc_Phy_RxData     *dataPtr )
{
    BspUicc_Phy_ReturnCode rxStatus = BSP_UICC_PHY_RETURN_CODE_FAILURE;
    volatile SYS_UWORD32        timeOut=0;
    volatile SYS_UWORD16         j;
    volatile SYS_UWORD16         rxCount = 0;
	SYS_UWORD32         required_data_size;
    bspUicc_Phy_rxCtrl.expectedData += rxDataSize;
	if(waittingToSleep ==FALSE)
      {
       gsp_disable();
         if( bspUicc_Phy_rxCtrl.expectedData > bspUicc_Phy_rxCtrl.rxIndex )
          {
				required_data_size = rxDataSize -(bspUicc_Phy_rxCtrl.rxIndex - bspUicc_Phy_rxCtrl.outIndex );
				if (required_data_size < rx_trigger_value)
					{

				        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_RX_TRIGGER,
			                       BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
			                       required_data_size);
					}
          }
	    gsp_enable();
      }

    /* Wait to read expected data, when there is no error */
    while ( ( (bspUicc_Phy_rxCtrl.rxIndex - bspUicc_Phy_rxCtrl.outIndex) < rxDataSize)
           && (timeOut < BSP_UICC_READ_TIMEOUT_VALUE)
           && (bspUicc_Phy_ctrl.error == BSP_UICC_PHY_NO_ERROR) )
    {
        timeOut++;
	 NU_Sleep(1);
    }

    /* Fill the given buffer with data in the rx buffer */
    if(( bspUicc_Phy_ctrl.error == BSP_UICC_PHY_NO_ERROR ) && (timeOut != BSP_UICC_READ_TIMEOUT_VALUE))
    {

        for( j=0; j< rxDataSize; j++)
        {
            *dataPtr =  bspUicc_Phy_rxCtrl.rxBuffer[bspUicc_Phy_rxCtrl.outIndex];
            dataPtr++;
            bspUicc_Phy_rxCtrl.outIndex++;
            rxCount++;
        }

        rxStatus = rxCount;
    }
    else if  (timeOut == BSP_UICC_READ_TIMEOUT_VALUE)
    {
        bspUicc_Phy_ctrl.error = BSP_UICC_PHY_READ_TIMEOUT_ERROR;
        rxStatus = bspUicc_Phy_ctrl.error;
    }
    else
    {
        rxStatus = bspUicc_Phy_ctrl.error;
    }
    return rxStatus;

}

/*=============================================================================
 * Description:
 *    Power off the Usim Hardware block
 */
BspUicc_Phy_ReturnCode
bspUicc_Phy_powerOff( void )
{
    BspUicc_Phy_ReturnCode status;
    /* Activate the Stop procedure for the card */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CMDSTOP,
                           BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                           BSP_UICC_PHY_LLIF_CMDSTOP_ENABLE );
    /* Stop the clock */
/*
BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CLOCK_STOP_CMD,
                           BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
                           BSP_UICC_PHY_CLOCK_STOP_ENABLE );
*/
    bspUicc_Power_off();

    status = BSP_UICC_PHY_RETURN_CODE_SUCCESS;
	bspUicc_sClk_running = FALSE;
    return status;

}

/*=============================================================================
 * Description:
 *    Check if the card has been inserted
 */
BspUicc_Phy_ReturnCode
bspUicc_Phy_getCardInsertStatus( void )
{
    return card_insert_status;
}

/*=============================================================================
 * Description:
 *    Toggle the card insert status
 */
void
bspUicc_Phy_toggleCardInsertStatus(void)
{
    if( card_insert_status == BSP_UICC_PHY_LLIF_STATNOCARD_ABSENT)
    {
        card_insert_status = BSP_UICC_PHY_LLIF_STATNOCARD_PRESENT;
    }
    else
    {
        card_insert_status = BSP_UICC_PHY_LLIF_STATNOCARD_ABSENT;
    }
}


/*=============================================================================
 * Description:
 *    Update the Physical Layer control data with the initialization functions
 *    insert and remove call back functions.
 */
void
 bspUicc_Phy_init_CardDetectFuncs( BspUicc_Phy_CardAbsentDetectHandler  *removeFunc)
{
    bspUicc_Phy_ctrl.removeFunc = *removeFunc;
}



/*=============================================================================
 * Description:
 *    Get the Error status
 */

BspUicc_Phy_Error bspUicc_Phy_getErrorStatus( void )
{
    return  bspUicc_Phy_ctrl.error;
}

/*=============================================================================
 * Description:
 *    Get the Current data Mode which could be ATR or PPS or READ-WRITE
 */
BspUicc_Phy_Mode bspUicc_Phy_getMode( void )
{

    return  bspUicc_Phy_ctrl.mode;
}

/*=============================================================================
 * Description:
 *    Set the Current data Mode which could be ATR or PPS or READ-WRITE
 */
void  bspUicc_Phy_setMode( BspUicc_Phy_Mode mode )
{

      bspUicc_Phy_ctrl.mode = mode ;
}

/*=============================================================================
 * Description:
 *    Set the direction to Transmit or Receive
 */
void  bspUicc_Phy_setDir( BspUicc_Phy_Direction phyDirection )
{
    if( phyDirection ==  BSP_UICC_PHY_DIRECTION_TX )
    {
        /* Set IO direction to Tx */
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFTXNRX,
                               BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR,
                               BSP_UICC_PHY_LLIF_CONFTXNRX_TX );
    }
    else
    {

        /* Set direction to reception */
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFTXNRX,
                               BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR,
                               BSP_UICC_PHY_LLIF_CONFTXNRX_RX );

    }
}

