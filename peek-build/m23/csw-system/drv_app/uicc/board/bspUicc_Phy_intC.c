/*=============================================================================
 *    Copyright 2001-2005 Texas Instruments Inc. All rights reserved.
 */


#include "sys_types.h"
#include "types.h"
#include "general.h"
#include "stdlib.h"
#include "sys_inth.h"

#include "bspUicc_Phy.h"
#include "bspUicc.h"
#include "bspUicc_Phy_llif.h"
#include "bspUicc_Phy_map.h"
#include "bspUicc_Phy_intC.h"
#include "bspUtil_Assert.h"

#include "bspTwl3029.h"
#include "bspTwl3029_IntC.h"
#include "nucleus.h"
/*=============================================================================
 * File Contents:
 *    This file contains the functions and defines needed to register the BSP_UICC
 *    interrupt with the main interrupt controller.
 */

/*============================================================================
 * BSP_UICC driver does not support prioritization of its interrupts right now
 */
#define BSP_UICC_PHY_INTC_EMPTY_SLOT { NULL, NULL }


/*=============================================================================
 *   Prototypes for semi-static functions (they are not exported, but they are
 *   officially public just incase they are to be written in assembly)
 *============================================================================*/
void bspUicc_Phy_intCHandler( void );


/*=============================================================================
 *   Global Data
 *============================================================================*/
/*=============================================================================
 * Description:
 *   Control data used by Physical layer
 */
BspUicc_Phy_Ctrl    bspUicc_Phy_ctrl;
extern BspUicc_CardPresentDetectHandler bspUicc_insert;

/*=============================================================================
 * Description:
 *   Control data used by Physical layer Receive path
 */
BspUicc_Phy_RxCtrl  bspUicc_Phy_rxCtrl;

/*=============================================================================
 * Description:
 *   Control data used by Physical layer Transmit path
 */
BspUicc_Phy_TxCtrl  bspUicc_Phy_txCtrl;

/*=============================================================================
 * Description:
 *   Global variables that make debug information available to the ISR
 *   call-back function.
 */
BspUicc_Phy_IntC_SourceId bspUicc_Phy_intCSourceId;

/*=============================================================================
 * Description:
 *   This is the type for the internal structure holding the software
 *   configuration of the interrupt controller.
 *   
 *   Note: Changing the size or the order of the parameters in this type may
 *         effect the dispatchInterrupt function if it is implemented in
 *         assembly.
 */
typedef struct
{
    BspUicc_Phy_IntC_SourceId            sourceId;
    BspUicc_Phy_IntC_Handler             handler;
}BspUicc_Phy_IntTableEntry;



/*=============================================================================
 * Each entry in this table is a function pointer that corresponds to a bit in
 * the status register.  Bit zero's function pointer is in location zero.  Bit
 * one's function pointer is in location one, etc.
 */
BspUicc_Phy_IntTableEntry bspUicc_Phy_intCIntTable[] = 
{
    BSP_UICC_PHY_INTC_EMPTY_SLOT, BSP_UICC_PHY_INTC_EMPTY_SLOT,
    BSP_UICC_PHY_INTC_EMPTY_SLOT, BSP_UICC_PHY_INTC_EMPTY_SLOT,
    BSP_UICC_PHY_INTC_EMPTY_SLOT, BSP_UICC_PHY_INTC_EMPTY_SLOT,
    BSP_UICC_PHY_INTC_EMPTY_SLOT, BSP_UICC_PHY_INTC_EMPTY_SLOT,
    BSP_UICC_PHY_INTC_EMPTY_SLOT, BSP_UICC_PHY_INTC_EMPTY_SLOT,
    BSP_UICC_PHY_INTC_EMPTY_SLOT
};
//#define FEATURE_UICC_DISABLE_PARITY_WA

#ifndef FEATURE_UICC_DISABLE_PARITY_WA			
extern SYS_UWORD16 bspUicc_EtuValue,bspUicc_Phy_highSpeed;
extern Uint16 bspUicc_orig_cgt_value;
extern Uint8 bspUicc_start_timer;
extern SYS_UWORD8 rx_trigger_value;
void uicc_timer_callback(void);
#endif
volatile SYS_BOOL clockstopDone = FALSE;

/*=============================================================================
  *  set before writing the trick STATUS command and reset upon getting the expected lastbyte  
  *
  */  
 volatile SYS_BOOL waittingToSleep = FALSE; 
/*=============================================================================
  *  expectedDataCount  is set to Le 
  *
  */  
 SYS_UWORD8 expectedDataCount;
/*=============================================================================
   *  expectedDataCount  is set to Le 
   *
   */  
 SYS_UWORD8 dataCount;
/*=============================================================================
   *  clockStopWAstate  keeps the state of the decoding STATEMACHINE 
   *
   */  
 BspUicc_clockStopWAstate clockStopWAstate = WAIT_FOR_ACK_NAK_SW_NULL;


/*=============================================================================
 * Description:
 *    Interrupt Handler for No Answer To Reset
 */
static void
bspUicc_Phy_noAnswerToResetHandler( BspUicc_Phy_IntC_SourceId sourceId )
{
    /* There has been no Answer to Reset */
    bspUicc_Phy_ctrl.error = BSP_UICC_PHY_NATR_ERROR;
}

/*=============================================================================
 * Description:
 *    Interrupt Handler for Character Underflow
 */
static void
bspUicc_Phy_charUnderFlowHandler( BspUicc_Phy_IntC_SourceId sourceId )
{
    bspUicc_Phy_ctrl.error = BSP_UICC_PHY_CHAR_UNDERFLOW_ERROR;
}

/*=============================================================================
 * Description:
 *    Interrupt Handler for Receive Overflow
 */
static void
bspUicc_Phy_charOverFlowHandler( BspUicc_Phy_IntC_SourceId sourceId )
{
    bspUicc_Phy_ctrl.error = BSP_UICC_PHY_CHAR_OVERFLOW_ERROR;
}

/*=============================================================================
 * Description:
 *    Interrupt Handler for Waiting for Characters to Transmit
 */

static void
bspUicc_Phy_waitingTxHandler( BspUicc_Phy_IntC_SourceId sourceId )
{
    SYS_UWORD8 i;
    
    if( ( bspUicc_Phy_txCtrl.txIndex < bspUicc_Phy_txCtrl.noTxBytes ) &&
        ( bspUicc_Phy_ctrl.dirn == BSP_UICC_PHY_DIRECTION_TX ) )
    {
        for( i=0;(( bspUicc_Phy_txCtrl.txIndex<bspUicc_Phy_txCtrl.noTxBytes) && (i<=BSP_UICC_PHY_LLIF_FIFO_MAX_TX_SIZE)) ; i++ )
         {
            /* Transmit the remaining bytes */
            BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_DTX,
                                   BSP_UICC_PHY_MAP_REG_USIM_DTX_PTR,
                                   bspUicc_Phy_txCtrl.txBuffer[bspUicc_Phy_txCtrl.txIndex] );
            bspUicc_Phy_txCtrl.txIndex++;   
         }      
         
        if( bspUicc_Phy_txCtrl.txIndex == bspUicc_Phy_txCtrl.noTxBytes )
        {          

            /* Return Direction */
            BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_CONFTXNRX,
                                   BSP_UICC_PHY_MAP_REG_USIM_CONF2_PTR,
                                   BSP_UICC_PHY_LLIF_CONFTXNRX_RX );
			

#ifdef FEATURE_UICC_DISABLE_PARITY_WA			
            /* mask Tx */
            BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_MASK_USIM_TX,
                                  BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                                  1);            
#endif
           

            /* Set Tx trig to 0 */
            BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_TX_TRIGGER,
                                   BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR, 0 );
			
        }
		
		/* Clear all interrupts to ensure that the spurious interrupts generated while filling the fifo are cleared*/
		BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_IT,
							   BSP_UICC_PHY_MAP_REG_USIM_IT_PTR,
							   BSP_UICC_PHY_LLIF_INTERRUPT_CLEAR_ALL );		
    }

#ifndef FEATURE_UICC_DISABLE_PARITY_WA
	else
	{
        /* mask Tx */
        BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_MASK_USIM_TX,
                        BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                                  1);            
	if (bspUicc_start_timer==TRUE)
		{
		 bspUicc_start_timer=FALSE;
#if (!(defined(WCP_PROF) && (WCP_PROF==1)))		 //start time 1 only in non wcp build. WCP uses Timer 1 
	        Dtimer1_Register ((BSP_UICC_PARITY_TIMER_ETU_VALUE*(bspUicc_EtuValue+bspUicc_Phy_highSpeed)),TRUE,uicc_timer_callback);	
	 	 Dtimer1_Start (TRUE);
#endif
		}
	}
#endif
	
}

/*=============================================================================
 * Description:
 *    Interrupt Handler for Waiting for Characters to be read
 */
#define  CHANGE_CGT

/* uiccDebugBuffer will hold the Rx data from card after we issue a STATUS command during sleep sequence */
#ifdef SIM_DRIVER_CLOCKSTOP_DEBUG
volatile SYS_UWORD8 uiccDebugBuffer[40];
volatile SYS_UWORD8 Rxindex =0;
#endif

static void
bspUicc_Phy_waitingRxHandler( BspUicc_Phy_IntC_SourceId sourceId )
{
    volatile SYS_UWORD16 regValue;
    volatile SYS_UWORD16 value;

    volatile SYS_UWORD8 initialIndex=0;
        
    initialIndex = bspUicc_Phy_rxCtrl.rxIndex;
#ifndef FEATURE_UICC_DISABLE_PARITY_WA
#if (!(defined(WCP_PROF) && (WCP_PROF==1)))	
    Dtimer1_Start (FALSE); //stop parity timer
#endif
#ifdef CHANGE_CGT
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_USIM_CGT,
                           BSP_UICC_PHY_MAP_REG_USIM_CGT_PTR, bspUicc_orig_cgt_value);
#endif
#endif
    /* Read the fifo till it is empty */
    while( BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_FIFO_RX_EMPTY,
                                  BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR ) == 0 )     
    {
        regValue = *( BSP_UICC_PHY_MAP_REG_USIM_DRX_PTR );

        value = BSP_UICC_PHY_LLIF_GET_LOCAL( BSP_UICC_PHY_LLIF_USIM_DRX, &regValue);

        bspUicc_Phy_rxCtrl.rxBuffer[bspUicc_Phy_rxCtrl.rxIndex] = value;

        if( bspUicc_Phy_ctrl.protocol == BSP_UICC_PHY_T0_PROTOCOL )
        {
              if (  BSP_UICC_PHY_LLIF_GET_LOCAL( BSP_UICC_PHY_LLIF_USIM_STATRXPAR,
                                           &regValue ) == 1 )  
              {
                /*Increment index only on correct parity
                  If a parity error occurs the read byte will be
                  replaced by the retransmitted byte */
                  bspUicc_Phy_rxCtrl.rxIndex++;
               if(waittingToSleep == TRUE)  /* status command waiting to sleep */
               {
				switch(clockStopWAstate)
				 {
				  case WAIT_FOR_ACK_NAK_SW_NULL: /* NULL bytes will be ignored here*/
					{
					  if ((value & 0x00FF)== 0x00F2)
					   clockStopWAstate = ACK_RECEIVED;
					  else if ((value & 0x00FF)== 0x000D)
					   clockStopWAstate = NAK_RECEIVED;
					  else if (((value & 0x00FF)!= 0x0060)
							  &&(((value & 0x00F0)== 0x0060)
							  ||((value & 0x00F0)== 0x0090)))
					   clockStopWAstate = SW1_RECEIVED;
					  break;
					 }
				  case NAK_RECEIVED:
					 {
						 clockStopWAstate = WAIT_FOR_ACK_NAK_SW_NULL;
					 }
				  case ACK_RECEIVED:
					 {
					   dataCount++;
					   if(dataCount == expectedDataCount)
					   clockStopWAstate = DATA_RECEIVED;
					   break;
					 }
				  case DATA_RECEIVED: /* NULL bytes will be ignored here*/
					 {
						if (((value & 0x00FF)!= 0x0060)
							&&(((value & 0x00F0)== 0x0060)
							||((value & 0x00F0)== 0x0090)))
						   clockStopWAstate = SW1_RECEIVED;
						break;
					 }
				  case SW1_RECEIVED:
					 {
					   /* we received a byte after SW1 so this was the last response byte. Hence do CLOCK STOP*/
						waittingToSleep = FALSE;
						clockStopWAstate = WAIT_FOR_ACK_NAK_SW_NULL;
						BSP_UICC_PHY_LLIF_SET(BSP_UICC_PHY_LLIF_CLOCK_STOP_CMD,
												BSP_UICC_PHY_MAP_REG_USIM_CMD_PTR,
												BSP_UICC_PHY_CLOCK_STOP_ENABLE);
                        clockstopDone =TRUE;   
					   break;
					 }
				  /* STATE MACHINE confirming ISO7916 Table-12 end : */
 				 }
/*end CS patch stop */
               	}  

              }

              /* skip parity check for the first byte in ATR */
              else if(( bspUicc_Phy_ctrl.mode == BSP_UICC_PHY_MODE_ATR ) &&
                      ( bspUicc_Phy_rxCtrl.rxIndex == 0 ))   
              {
                  bspUicc_Phy_rxCtrl.rxIndex++;
              }   
        }
        else
        {
              /* Parity Check not needed for T= 1 protocol */
              bspUicc_Phy_rxCtrl.rxIndex++;
             
        }
    }
    
   if(waittingToSleep ==FALSE)
{
        if( bspUicc_Phy_rxCtrl.expectedData > bspUicc_Phy_rxCtrl.rxIndex )
        {
	    bspUicc_Phy_rxCtrl.remainingData = bspUicc_Phy_rxCtrl.expectedData -  bspUicc_Phy_rxCtrl.rxIndex ;

    	if(bspUicc_Phy_rxCtrl.remainingData > rx_trigger_value)
    	{
        	BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_RX_TRIGGER,
                               BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
                               rx_trigger_value );
	    }
    	else
            {
                BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_RX_TRIGGER,
                                       BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
                                       bspUicc_Phy_rxCtrl.remainingData );
            }
        }
            else
            {
                BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_RX_TRIGGER,
                                       BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
							   rx_trigger_value );
            }

   	}    
}

/*=============================================================================
 * Description:
 *    Interrupt Handler for Card Detect, both insertion and extraction
 */
static void
bspUicc_Phy_cardDetectHandler( BspTwl3029_IntC_SourceId sourceId )
{
    /*   if card has been inserted */
    bspUicc_Phy_toggleCardInsertStatus();
    if(  bspUicc_Phy_getCardInsertStatus() == BSP_UICC_PHY_LLIF_STATNOCARD_ABSENT ) 
    {
         bspUicc_Phy_ctrl.error = BSP_UICC_PHY_NO_CARD_PRESENT_ERROR;
         if(( bspUicc_Phy_ctrl.removeFunc != NULL) )
         {
            Dtimer1_Start (FALSE);
            bspUicc_Phy_ctrl.removeFunc();
         }
    }
    else if(bspUicc_insert != NULL )
    {
         bspUicc_Phy_ctrl.error = BSP_UICC_PHY_NO_ERROR;
         bspUicc_insert(NULL,BSP_UICC_OMIT_CONFIG_CHARACTERISTICS,NULL);
    }
}

/*=============================================================================
 * Description:
 *    Interrupt Handler for End of Block received
 * Currently End of block is calculated by the Link layer.
 */
static void
bspUicc_Phy_endOfBlockHandler( BspUicc_Phy_IntC_SourceId sourceId )
{
    /* Not using the End of block interrupt
       Block size is determined by the prologue/block header */
    
    /* Clear the Rx fifo of any junk data
       Have no clue why this is needed   */ 
    while( BSP_UICC_PHY_LLIF_GET
           ( BSP_UICC_PHY_LLIF_FIFO_RX_EMPTY,
           
             BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR ) == 0 )
    {
        volatile SYS_UWORD8 junk;
        junk = *( BSP_UICC_PHY_MAP_REG_USIM_DRX_PTR );
    }
}

/*=============================================================================
 * Description:
 *    Interrupt Handler for Time Out Characer
 */
static void
bspUicc_Phy_charTimeOutHandler( BspUicc_Phy_IntC_SourceId sourceId )
{
    bspUicc_Phy_ctrl.error = BSP_UICC_PHY_CHAR_TIMEOUT_ERROR;
}

/*=============================================================================
 * Description:
 *    Interrupt Handler for Time Out Block
 */
static void
bspUicc_Phy_blockTimeOutHandler( BspUicc_Phy_IntC_SourceId sourceId )
{
    bspUicc_Phy_ctrl.error = BSP_UICC_PHY_BLOCK_TIMEOUT_ERROR;
}

/*=============================================================================
 * Description:
 *    Interrupt Handler for Resent Reach Interrupt
 */
static void
bspUicc_Phy_maxResentHandler( BspUicc_Phy_IntC_SourceId sourceId )
{
    bspUicc_Phy_ctrl.error = BSP_UICC_PHY_MAX_RESENT_ERROR; 
}

/*=============================================================================
 * Description:
 *    Interrupt Handler for error in Interpreting TS character interrupt
 */
static void
bspUicc_Phy_tsDecodeErrorHandler( BspUicc_Phy_IntC_SourceId sourceId )
{
    bspUicc_Phy_ctrl.error = BSP_UICC_PHY_TS_DECODE_ERROR;
}

/*=============================================================================
 * Description:
 *    Register all the handlers for the Usim interrupts.
 */
static void
bspUicc_Phy_registerIntHandlers( void )
{
    SYS_UWORD16 mask,i;

    BspUicc_Phy_IntTableEntry bspUicc_Phy_intTable[] =
    {
        { BSP_UICC_PHY_INTC_SOURCE_ID_NATR,     bspUicc_Phy_noAnswerToResetHandler },
        { BSP_UICC_PHY_INTC_SOURCE_ID_WT,       bspUicc_Phy_charUnderFlowHandler   },
        { BSP_UICC_PHY_INTC_SOURCE_ID_OV,       bspUicc_Phy_charOverFlowHandler    },
        { BSP_UICC_PHY_INTC_SOURCE_ID_TX,       bspUicc_Phy_waitingTxHandler       },
        { BSP_UICC_PHY_INTC_SOURCE_ID_RX,       bspUicc_Phy_waitingRxHandler       },
        { BSP_UICC_PHY_INTC_SOURCE_ID_CD,       (BspUicc_Phy_IntC_Handler) bspUicc_Phy_cardDetectHandler      },
        { BSP_UICC_PHY_INTC_SOURCE_ID_EOB,      bspUicc_Phy_endOfBlockHandler      },
        { BSP_UICC_PHY_INTC_SOURCE_ID_TOC,      bspUicc_Phy_charTimeOutHandler     },
        { BSP_UICC_PHY_INTC_SOURCE_ID_TOB,      bspUicc_Phy_blockTimeOutHandler    },
        { BSP_UICC_PHY_INTC_SOURCE_ID_RESENT,   bspUicc_Phy_maxResentHandler       },
        { BSP_UICC_PHY_INTC_SOURCE_ID_TS_ERROR, bspUicc_Phy_tsDecodeErrorHandler   }
    };

    for( i=0; i<BSP_UICC_PHY_INTC_SOURCE_ID_NUM_SOURCES; i++)
    {
        bspUicc_Phy_intCSetHandler( bspUicc_Phy_intTable[i].sourceId,
                                    bspUicc_Phy_intTable[i].handler );
    }
/* mask USIM TX interrupt */	
        mask=BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_REG_USIM_MASKIT,
                            BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR);
	 mask=mask | (1<<BSP_UICC_PHY_INTC_SOURCE_ID_TX);
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_MASKIT,
                            BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                            mask);

}

/*=============================================================================
 *   Local Functions
 *============================================================================*/
/*=============================================================================
 * Description:
 *   This is the ISR that is registered with the main interrupt controller
 */
void
bspUicc_Phy_intCHandler( void )
{
    register SYS_UWORD32 sourceId;
    register SYS_UWORD32 status;
    register Uint32 mask;

    /*
     * The sourceType is actually the address of the status register
     */
    status = BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_REG_USIM_IT,
                                    BSP_UICC_PHY_MAP_REG_USIM_IT_PTR );

    /* Clear all interrupts */
    BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_IT,
                           BSP_UICC_PHY_MAP_REG_USIM_IT_PTR,
                           BSP_UICC_PHY_LLIF_INTERRUPT_CLEAR_ALL );


    /* Clear Interrupts We are not Interested in */
    mask = BSP_UICC_PHY_LLIF_GET(BSP_UICC_PHY_LLIF_REG_USIM_MASKIT,
                                    BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR);
    status = status & ~(mask);
    
    /* Loop through every bit and check if an interrupt is pending.  If it
     * is, then service it.
     */
    for( sourceId = 0;
         (status > 0);
         sourceId++, status >>= 1 )
    {
        if( (status & 0x1) == 0x1 && (sourceId!=BSP_UICC_PHY_INTC_SOURCE_ID_CD))
        {
            if(( bspUicc_Phy_intCIntTable[ sourceId ].handler != NULL) )
            {
                bspUicc_Phy_intCSourceId = sourceId;
                bspUicc_Phy_intCIntTable[ sourceId ].handler( sourceId );
            }
            else
            {
                BSPUTIL_ASSERT( FALSE ); /* Handler not installed */
            }
        }
    }
    return;
}


/*=============================================================================
 *   Public Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 *   Initialization code for the BSP_UICC interrupts.
 */
BspUicc_Phy_IntC_ReturnCode
bspUicc_Phy_intCInit( void )
{

    bspUicc_Phy_registerIntHandlers();

    /* Initialize the card remove call back functions */
    bspUicc_Phy_ctrl.removeFunc = NULL;
    
    /* unmask USIM interrupt */
    F_INTH_ENABLE_ONE_IT(C_INTH_USIM_IT);	



    /*Register Triton Interrupt Handelers*/
    bspTwl3029_IntC_setHandler(BSP_TWL3029_INTC_SOURCE_ID_SIM,
                        		bspUicc_Phy_cardDetectHandler);

    return( BSP_UICC_PHY_INTC_RETURN_CODE_SUCCESS );
}



/*=============================================================================
 * Description:
 *   Registers an BSP_UICC ISR.
 */
BspUicc_Phy_IntC_ReturnCode
bspUicc_Phy_intCSetHandler( BspUicc_Phy_IntC_SourceId  sourceId,
                          BspUicc_Phy_IntC_Handler   handler )
{
    BspUicc_Phy_IntC_ReturnCode status = BSP_UICC_PHY_INTC_RETURN_CODE_FAILURE;
    
    if( sourceId < BSP_UICC_PHY_INTC_SOURCE_ID_NUM_SOURCES )
    {
        bspUicc_Phy_intCIntTable[ sourceId ].handler = handler;
        
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_MASKIT,
                            BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                            0 );

        status = BSP_UICC_PHY_INTC_RETURN_CODE_SUCCESS;
    }
    
    return( status );
}

/*=============================================================================
 * Description:
 *   Disable an interrupt source
 */
BspUicc_Phy_IntC_ReturnCode
bspUicc_Phy_intCDisableSource( BspUicc_Phy_IntC_SourceId  sourceId )
{
    Uint8                       oldMask;
    BspUicc_Phy_IntC_ReturnCode   status = BSP_UICC_PHY_INTC_RETURN_CODE_FAILURE;
    
    oldMask = BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_REG_USIM_MASKIT,
                                  BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR );
	if(sourceId == BSP_UICC_PHY_INTC_SOURCE_ID_CD)
    {
        bspTwl3029_IntC_disableSource(BSP_TWL3029_INTC_SOURCE_ID_SIM);
        status = BSP_UICC_PHY_INTC_RETURN_CODE_SUCCESS;	
    }

    else if( sourceId < BSP_UICC_PHY_INTC_SOURCE_ID_NUM_SOURCES )
    {
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_MASKIT,
                            BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                            ( oldMask | BSP_UICC_PHY_INTC_SOURCE_ID_TO_MASK( sourceId ) ) );
        
        status = BSP_UICC_PHY_INTC_RETURN_CODE_SUCCESS;
    }
    
    return( status );
}

/*=============================================================================
 * Description:
 *   Enable an interrupt source
 */
BspUicc_Phy_IntC_ReturnCode
bspUicc_Phy_intCEnableSource( BspUicc_Phy_IntC_SourceId  sourceId )
{
    Uint16                       oldMask;
    Uint16                       newMask;
    BspUicc_Phy_IntC_ReturnCode   status = BSP_UICC_PHY_INTC_RETURN_CODE_FAILURE;

    oldMask = BSP_UICC_PHY_LLIF_GET( BSP_UICC_PHY_LLIF_REG_USIM_MASKIT,
                                  BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR );

    newMask =  ~((Uint16)BSP_UICC_PHY_INTC_SOURCE_ID_TO_MASK( sourceId ) );
	if(sourceId == BSP_UICC_PHY_INTC_SOURCE_ID_CD)
    {

        bspTwl3029_IntC_enableSource( BSP_TWL3029_INTC_SOURCE_ID_SIM);
        status = BSP_UICC_PHY_INTC_RETURN_CODE_SUCCESS;	
    }

    else if( sourceId < BSP_UICC_PHY_INTC_SOURCE_ID_NUM_SOURCES )
    {
        BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_REG_USIM_MASKIT,
                            BSP_UICC_PHY_MAP_REG_USIM_MASKIT_PTR,
                            ( oldMask & newMask ) );

        status = BSP_UICC_PHY_INTC_RETURN_CODE_SUCCESS;
    }
    
    return( status );
}


