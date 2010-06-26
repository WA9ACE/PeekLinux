/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */
#ifndef BSP_UICC_PHY_INTC_HEADER
#define BSP_UICC_PHY_INTC_HEADER
#include "bspUicc_Phy_llif.h"


/*=============================================================================
 * Component Description:
 *    Interrupt Service Routine (ISR) component for BSP_UICC.
 */

/*===========================================================================*/
/*!
 * @typedef BspUicc_Phy_IntC_SourceId
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type used for interrupt sources. Each interrupt controller
 *   implementation must define the valid list of source ids.
 */
#define BSP_UICC_PHY_INTC_SOURCE_ID_NATR           ( BSP_UICC_PHY_LLIF_USIM_NATR_OFFSET )
#define BSP_UICC_PHY_INTC_SOURCE_ID_WT             ( BSP_UICC_PHY_LLIF_USIM_WT_OFFSET )
#define BSP_UICC_PHY_INTC_SOURCE_ID_OV             ( BSP_UICC_PHY_LLIF_USIM_OV_OFFSET )
#define BSP_UICC_PHY_INTC_SOURCE_ID_TX             ( BSP_UICC_PHY_LLIF_USIM_TX_OFFSET )
#define BSP_UICC_PHY_INTC_SOURCE_ID_RX             ( BSP_UICC_PHY_LLIF_USIM_RX_OFFSET )
#define BSP_UICC_PHY_INTC_SOURCE_ID_CD             ( BSP_UICC_PHY_LLIF_USIM_CD_OFFSET )
#define BSP_UICC_PHY_INTC_SOURCE_ID_EOB            ( BSP_UICC_PHY_LLIF_USIM_EOB_OFFSET )
#define BSP_UICC_PHY_INTC_SOURCE_ID_TOC            ( BSP_UICC_PHY_LLIF_USIM_TOC_OFFSET )
#define BSP_UICC_PHY_INTC_SOURCE_ID_TOB            ( BSP_UICC_PHY_LLIF_USIM_TOB_OFFSET )
#define BSP_UICC_PHY_INTC_SOURCE_ID_RESENT         ( BSP_UICC_PHY_LLIF_USIM_RESENT_OFFSET )
#define BSP_UICC_PHY_INTC_SOURCE_ID_TS_ERROR       ( BSP_UICC_PHY_LLIF_USIM_TS_ERROR_OFFSET )

#define BSP_UICC_PHY_INTC_SOURCE_ID_NUM_SOURCES      (11)
#define BSP_UICC_PHY_INTC_SOURCE_ID_TO_MASK(_id)     ( 0x01 << (_id) )

typedef SYS_UWORD16 BspUicc_Phy_IntC_SourceId;

/*===========================================================================*/
/*!
 * @typedef BspUicc_Phy_IntC_Priority
 * 
 * @discussion
 * <b> Description  </b><br>
 *   Priority for each interrupt source
 */
typedef SYS_UWORD32 BspUicc_Phy_IntC_Priority;


/*===========================================================================*/
/*!
 * @typedef BspUicc_Phy_IntC_ReturnCode
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the data type for return codes for the Interrupt controller
 *   functions
 */
#define BSP_UICC_PHY_INTC_RETURN_CODE_ENABLED       (1)
#define BSP_UICC_PHY_INTC_RETURN_CODE_NOT_ENABLED   (0)
#define BSP_UICC_PHY_INTC_RETURN_CODE_SUCCESS       (0)
#define BSP_UICC_PHY_INTC_RETURN_CODE_FAILURE       (-1)

#define BSP_UICC_FIFO_RESET_ETU_VALUE 6
#define BSP_UICC_PARITY_TIMER_ETU_VALUE 15
#define BSP_UICC_PARITY_CGT_VALUE (BSP_UICC_PARITY_TIMER_ETU_VALUE+25)


typedef SYS_WORD32 BspUicc_Phy_IntC_ReturnCode;

/*===========================================================================*/
/*!
 * @typedef BspUicc_Phy_IntC_Handler
 * 
 * @discussion
 * <b> Description  </b><br>
 *   This is the type used for an interrupt service routine handler. It is
 *   a function that returns nothing and takes the source Id of the interrupt
 *   as a parameter.
 */
typedef void (*BspUicc_Phy_IntC_Handler)( BspUicc_Phy_IntC_SourceId sourceId );

/*=============================================================================
 * Creators
 */

/*=============================================================================
 */
/*!
 * @function bspUicc_Phy_intCInit
 *
 * @discussion
 * <b> Description </b><br>
 *    Used to initialize the BSP_UICC Interrupt Controller.
 *
 * <b> Context </b><br>
 *    It must be called by the application. It must be called before any other
 *    calls to the physical layer of BSP_UICC device driver.
 * 
 *    <b>Not Interrupt-Safe</b>
 *    <b>Not Thread-Safe</b>
 *
 *  @result <br>
 *    The interrupt controller internal data is initialized ,all sources are
 *    disabled, the initial device specific configuration for each of the
 *    sources is set to the passed in configuration.
 *    SC_TISD1_INTC_RETURN_CODE_SUCCESS returned if there were no errors,
 *    otherwise SC_TISD1_INTC_RETURN_CODE_FAILURE is returned.
 */
BspUicc_Phy_IntC_ReturnCode bspUicc_Phy_intCInit( void );

/*=============================================================================
 */
/*!
 * @function bspUicc_Phy_intCSetHandler
 *
 * @discussion
 * <b> Description </b><br>
 *    This function maps (or unmaps with NULL) a handler to a specific system 
 *    controller interrupt source.
 *
 * <b> Context </b><br>
 *    It must be called by the application to set a handler ISR for each source
 *    This function may be called after initialization.
 * 
 *    Interrupt-Safe</br>
 *    Not Thread-Safe - The interrupt source being modified must be disabled.
 *
 *  @param  sourceId
 *              This is the interrupt source being registered.
 * 
 *  @param  handler
 *              This is a pointer to the function to be called when the
 *              specified interrupt occurs. Use NULL to remove a handler.
 *
 *  @result <br>
 *    INTC_RETURN_CODE_SUCCESS if successful, INTC_RETURN_CODE_FAILURE otherwise.
 */
BspUicc_Phy_IntC_ReturnCode
bspUicc_Phy_intCSetHandler( BspUicc_Phy_IntC_SourceId  sourceId,
                          BspUicc_Phy_IntC_Handler   handler );

/*=============================================================================
 */
/*!
 * @function intC_dispatchInterrupt
 *
 * @discussion
 * <b> Description </b><br>
 *    Call the handler functions that corresponds to an active interrupt
 *    sources.
 *
 * <b> Context </b><br>
 *    This function is typically used only in within the interrupt controller
 *    device drive, but it could also be used externally for debugging purposes
 * 
 *    Interrupt-Safe</br>
 *    Thread-Safe
 *
 *  @result <br>
 *    INTC_RETURN_CODE_SUCCESS is returned if the source is enabled, and the
 *    corresponding handler exists and can be called. INTC_RETURN_CODE_FAILURE
 *    is returned otherwise. The interrupt source is cleared at the interrupt
 *    controller when this function is called.
 */
BspUicc_Phy_IntC_ReturnCode bspUicc_Phy_intCDispatchInterrupt( void );

#endif
