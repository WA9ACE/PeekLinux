/**
 * @file	usb_task.c
 *
 *	TASK for USB SWE.
 *
 * @author	Anton van Breemen
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	26/09/2003	Anton van Breemen (ICT)		
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

/*
 * Note: This module will be build for target environment.
 */

#include "usb/usb_i.h"
#include "usb/usb_api.h"
#include "usb/usb_w2fc.h"
#include "abb/abb.h"
#include "usb/usb_inth.h"
//#include "spi/spi_drv.h"
#include "inth/iq.h"
#include "inth/sys_inth.h"
#include "chipset.cfg"

 /**
 *NON ISO endpoints can have a FIFO size of maximum 64 bytes  
 *00110000:00000000 for 64 bytes
 *00100000:00000000 for 32 bytes
 *00010000:00000000 for 16 bytes
 *00000000:00000000 for 8 bytes
 */
#define USB_DETERM_EP_SIZE(a)	((a == 64) ? 0x3000 : ((a == 32) ? 0x2000 : \
								((a == 16) ? 0x1000 : ((a > 64) ? 0xFFFF : 0x0000))))
#define USB_ADD_EP_PNTR(a)		(a & 0x0FFF)

#define USB_EP_NUM_PREP_EP(a)	(W2FC_EP_NUM = a)

#define INTH_IT_SPI			13

#if (CHIPSET == 15)
#define Mask_USBPRES STS_HW_CONDITIONS_STS_USB_BIT
#else
#define Mask_USBPRES 0x0080
#endif
#define Mask_USB_IT  0x0010

#define W2FC_EPSIZE_8		(0x0 << 12)
#define W2FC_EPSIZE_16		(0x1 << 12)
#define W2FC_EPSIZE_32		(0x2 << 12)
#define W2FC_EPSIZE_64		(0x3 << 12)
#define W2FC_EPSIZE_128		(0x4 << 12)
#define W2FC_EPSIZE_256		(0x5 << 12)
#define W2FC_EPSIZE_512		(0x6 << 12)
#define W2FC_EPSIZE_1023	(0x7 << 12)

#define W2FC_SIZE2PARAM( size, param ) switch( size ) { \
														case 8:		param=W2FC_EPSIZE_8;	break;\
														case 16:	param=W2FC_EPSIZE_16;	break;\
														case 32:	param=W2FC_EPSIZE_32;	break;\
														case 64:	param=W2FC_EPSIZE_64;	break;\
														case 128:	param=W2FC_EPSIZE_128;	break;\
														case 256:	param=W2FC_EPSIZE_256;	break;\
														case 512:	param=W2FC_EPSIZE_512;	break;\
														case 1023:	param=W2FC_EPSIZE_1023;	break;\
														}
 /**
 * @name init_hw_ep
 *
 * this function is called by the usb_init function
 *
 * This function places a default value in the register of the endpoint.
 * as well as halts all endpoints
 *
 *@{*/
T_RV_RET init_physical_eps(void)
{
	UINT16 ep_cntr;
	
	/**
	 *CONTROL endpoint0 register configuration
	 */
	
	/*configure endpoint 0 register*/
	USB_REG16_ASSIGN(W2FC_EP0, W2FC_16BIT_RESET); //write 0x0000 in it
	/*set fifo 0 size*/
	USB_REG16_ASSIGN(W2FC_EP0, (USB_DETERM_EP_SIZE(USB_ENDP0_SIZE) | 
		USB_ADD_EP_PNTR(USB_ENDP0_ADDR)));
	
	/**
	 *init rx endpoints register
	 */
	USB_REG16_ASSIGN(W2FC_EP1_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP2_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP3_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP4_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP5_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP6_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP7_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP8_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP9_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP10_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP11_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP12_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP13_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP14_RX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP15_RX, W2FC_16BIT_RESET);
	
	/**
	 *init endpoints register
	 */
	USB_REG16_ASSIGN(W2FC_EP1_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP2_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP3_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP4_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP5_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP6_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP7_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP8_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP9_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP10_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP11_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP12_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP13_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP14_TX, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_EP15_TX, W2FC_16BIT_RESET);

	/**
	 *halt all rx and tx endpoints 
	 *when the usb host is attached a reset will occur for all endpoints 
	 *then the halt condition will be cleared. 
	 */
	/*halt rx ep's*/
	USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);
	for(ep_cntr = 1; ep_cntr <= 15; ep_cntr++) {
		// Write EP_NUM register: //
		// EP_NUM.EP_Num = endp_nb //
		// EP_NUM.EP_Dir = 0       //
		// EP_NUM.EP_Sel = 0       //
		// EP_NUM.Setup_Sel = 0    //
		USB_EP_NUM_RX_RELEASE(ep_cntr);
		
                USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_SET_HALT);
		//update the endpoint stall bitmap
		usb_env_ctrl_blk_p->bm_rx_ep_buf_stall |= (1 << ep_cntr);	
	}
	/*halt tx ep's*/
	for(ep_cntr = 1; ep_cntr <= 15; ep_cntr++) {
		// Write EP_NUM register: //
		// EP_NUM.EP_Num = endp_nb //
		// EP_NUM.EP_Dir = 1       //
		// EP_NUM.EP_Sel = 0       //
		// EP_NUM.Setup_Sel = 0    //
		USB_EP_NUM_TX_RELEASE(ep_cntr);
		USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_SET_HALT);
		usb_env_ctrl_blk_p->bm_tx_ep_buf_stall |= (1 << ep_cntr);
	}
	/*clear W2FC_EP_NUM */
	USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);

	/**
	 *configure W2FC_IRQ_EN
	 */
	USB_REG16_ASSIGN(W2FC_IRQ_EN, W2FC_16BIT_RESET); //write 0x0000 in it
	/*enable state changed interrupts*/
	USB_REG16_ASSIGN(W2FC_IRQ_EN, (W2FC_IRQ_EN_DS_CHG_IE | W2FC_IRQ_EN_EP0_IE));
	
	/*this is the place were the bus communication is enabled*/
	/*because the driver is ready and the state is correct*/
	//USB_REG16_ASSIGN(W2FC_SYSCON1, (W2FC_SYSCON1_PULLUP_EN | W2FC_SYSCON1_SOFF_DIS | W2FC_SYSCON1_CFG_LOCK)); 
	USB_REG16_ASSIGN(W2FC_SYSCON1, (W2FC_SYSCON1_SOFF_DIS | W2FC_SYSCON1_CFG_LOCK)); 
	/*the pullup enable is only enabled after a device state changed interrupt*/

	return USB_OK;
}


void reset_registers(void)
{
	/**
	 *configure W2FC_IRQ_EN
	 */
	USB_REG16_ASSIGN(W2FC_IRQ_EN, W2FC_16BIT_RESET); //write 0x0000 in it

	/*enable state changed interrupts*/
	USB_REG16_ASSIGN(W2FC_IRQ_EN, (W2FC_IRQ_EN_DS_CHG_IE | W2FC_IRQ_EN_EP0_IE));

	//W2FC_IRQ_EN |= W2FC_IRQ_EN_SOF_IE;
	//USB_REG16_ASSIGN(W2FC_SYSCON1, (W2FC_SYSCON1_PULLUP_EN | W2FC_SYSCON1_SOFF_DIS | W2FC_SYSCON1_CFG_LOCK));
	USB_REG16_ASSIGN(W2FC_SYSCON1, (W2FC_SYSCON1_SOFF_DIS | W2FC_SYSCON1_CFG_LOCK)); 
	////
	/*syscon2*/
	USB_REG16_ASSIGN(W2FC_SYSCON2, W2FC_16BIT_RESET);
	/*dma irq enable*/
	USB_REG16_ASSIGN(W2FC_DMA_IRQ_EN, W2FC_16BIT_RESET);

	/*disable DMA endpoints*/
	USB_REG16_ASSIGN(W2FC_RXDMA_CFG, W2FC_16BIT_RESET); //write 0x0000 in it
	USB_REG16_ASSIGN(W2FC_TXDMA_CFG, W2FC_16BIT_RESET); //write 0x0000 in it
	USB_REG16_ASSIGN(W2FC_RXDMA0, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_RXDMA1, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_RXDMA2, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_TXDMA0, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_TXDMA1, W2FC_16BIT_RESET);
	USB_REG16_ASSIGN(W2FC_TXDMA2, W2FC_16BIT_RESET);

	//F_INTH_DISABLE_ONE_IT(C_INTH_USB_IT);
}

//**********************************************************************
//  EndpointConfiguration
//
//    Endpoint Configuration Routine
//
//    Input:  None
//    Output: None
//    return: None
//
//  W2FC Functional Specification Rev1.8: Figure 9 on Page 92
//
//**********************************************************************
T_RV_RET EndpointConfiguration( void )
{
	UINT8 Ptr_flag;
	UINT16 size = 0;
	UINT16 i;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	UINT16 j;
	INT8 pif;
#endif
	UINT16 EPN;
	UINT16 nof_active_ep;
	UINT16 reg_val;
	T_USB_ENDPOINT_DESCR temp_ep;
	INT8 tmp_if_nr = 0;
	UINT16 tmp_log_ep_nr = 0;
	T_RV_RET ret = USB_OK;

	// Set Ptr_Flag to base address: for endpoint 1(rx) this is setupbuf size(8) + endpoint 0 bufsize(64)
	//--> 72 / 8 = 9
	Ptr_flag = ((USB_EP_BASE_SIZE + USB_ENDP0_SIZE) / 8);

	//first reset all the endpoints and halt them
	/*configure endpoint 0 register*/
	USB_REG16_ASSIGN(W2FC_EP0, W2FC_16BIT_RESET); //write 0x0000 in it
	/*set fifo 0 size*/
	USB_REG16_ASSIGN(W2FC_EP0, (USB_DETERM_EP_SIZE(USB_ENDP0_SIZE) | 
		USB_ADD_EP_PNTR(USB_ENDP0_ADDR)));
	
	//- Set RX Endpoints 1-15 -//
	if(usb_env_ctrl_blk_p->cnfg.nof_rx_ep < USB_MAX_DIR_EP)	{	
		nof_active_ep = usb_env_ctrl_blk_p->cnfg.nof_rx_ep;
	}
	else {
		nof_active_ep = USB_MAX_DIR_EP;
	}
	//FOREACH active rx endpoint
	for( i=1; i<=nof_active_ep ; i++ ) {
		//- Read EPn size -//
		reg_val = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[i-1].
					endpoint_data.endpo_descr_p->wMaxPacketSize;
		W2FC_SIZE2PARAM(reg_val, size);

		//- Prepare EPn parameter -//
		//- EP Type Non-ISO -//
		// Fill EPn register with:     //
		// EPn_Valid = 1               //
		// EPn_Db    = 0               //
		// EPn_Size  = EP Size (2bits) //
		// EPn_Iso   = 0               //
		// EPn_ptr   = Ptr_flag        //
		EPN = (W2FC_EPn_EPn_VALID | size | (Ptr_flag & 0x3FF));
	
		// Ptr_flag = Ptr_flag + (EPn_Size / 8)//
		Ptr_flag += (reg_val / 8);
	
		tmp_if_nr = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[i-1].
			interface_data_p->interface_number;
		tmp_log_ep_nr = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[i-1].
			endpoint_data.endpoint_number;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT

		pif = lif_2_pif(usb_env_ctrl_blk_p->enumInfo.info, tmp_if_nr);
		if(pif != -1)
		{
			tmp_if_nr = pif;
		}else{
			return USB_INTERNAL_ERR;
		}

		for(j=0;j<USB_TOTAL_DIFF_IF;j++)
		{
			if(configurations_table[USB_CNFGNR_2INDX].if_cnfg_p[j].ifdescr_p->bInterfaceNumber== tmp_if_nr)
				break;
		}
		if(fill_ep_descr(&temp_ep, configurations_table[USB_CNFGNR_2INDX].if_cnfg_p[j].
#else
		if(fill_ep_descr(&temp_ep, configurations_table[USB_CNFGNR_2INDX].if_cnfg_p[tmp_if_nr].
#endif
								if_logical_ep_array_p[(tmp_log_ep_nr - 1)]) == TRUE) {
			//this ep is double buffered
			EPN |= W2FC_EPn_EPn_DB;
			// Ptr_flag = Ptr_flag + (EPn_Size / 8)//
			Ptr_flag += (reg_val / 8);
		}
		// Fill EPn register //
		W2FC_EPn_RX(i) = EPN;

#ifndef  USB_DMA_USED
		//enable endpoint
		prepare_rx_trsfr(i);
#endif

		USB_TRACE_WARNING_PARAM("USB: W2FC_EPn_RX(i)", W2FC_EPn_RX(i));
	}
	//- Set TX Endpoints 1-15 -//
	if(usb_env_ctrl_blk_p->cnfg.nof_tx_ep < USB_MAX_DIR_EP)	{
		nof_active_ep = usb_env_ctrl_blk_p->cnfg.nof_tx_ep;
	}
	else {
		nof_active_ep = USB_MAX_DIR_EP;
	}
	//FOREACH active tx endpoint
	for( i=1; i<=nof_active_ep ; i++ ) {
		//- Read EPn size -//
		reg_val = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl[i-1].
					endpoint_data.endpo_descr_p->wMaxPacketSize;
		W2FC_SIZE2PARAM(reg_val, size);
		//- Prepare EPn parameter -//
		//- EP Type Non-ISO -//
		// Fill EPn register with:     //
		// EPn_Valid = 1               //
		// EPn_Db    = 0               //
		// EPn_Size  = EP Size (2bits) //
		// EPn_Iso   = 0               //
		// EPn_ptr   = Ptr_flag        //
		EPN = (W2FC_EPn_EPn_VALID | size | (Ptr_flag & 0x3FF));
		// Ptr_flag = Ptr_flag + (EPn_Size / 8)//
		Ptr_flag += (reg_val / 8);

		// EPn_RX.EPn_RX_Size or DB=1? //
		tmp_if_nr = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl[i-1].
			interface_data_p->interface_number;
		tmp_log_ep_nr = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl[i-1].
			endpoint_data.endpoint_number;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT

		pif = lif_2_pif(usb_env_ctrl_blk_p->enumInfo.info, tmp_if_nr);
		if(pif != -1)
		{
			tmp_if_nr = pif;
		}else{
			return USB_INTERNAL_ERR;
		}

		for(j=0;j<USB_TOTAL_DIFF_IF;j++)
		{
			if(configurations_table[USB_CNFGNR_2INDX].if_cnfg_p[j].ifdescr_p->bInterfaceNumber== tmp_if_nr)
				break;
		}		
		if(fill_ep_descr(&temp_ep, configurations_table[USB_CNFGNR_2INDX].if_cnfg_p[j].
#else
		if(fill_ep_descr(&temp_ep, configurations_table[USB_CNFGNR_2INDX].if_cnfg_p[tmp_if_nr].
#endif
								if_logical_ep_array_p[(tmp_log_ep_nr - 1)]) == TRUE) {
			//this ep is double buffered
			EPN |= W2FC_EPn_EPn_DB;
			// Ptr_flag = Ptr_flag + (EPn_Size / 8)//
			Ptr_flag += (reg_val / 8);
		}

		// Fill EPn register //
		W2FC_EPn_TX(i) = EPN;
	}
	
	/*enable state changed interrupts*/
	W2FC_IRQ_EN |= W2FC_IRQ_EN_DS_CHG_IE;
	/*enable endpoint 0 interrupts*/
	W2FC_IRQ_EN |= W2FC_IRQ_EN_EP0_IE;
	//enable EPn TX and EPn RX interrupts
	W2FC_IRQ_EN |= W2FC_IRQ_EN_EPN_RX_IE;
	W2FC_IRQ_EN |= W2FC_IRQ_EN_EPN_TX_IE;
	USB_TRACE_WARNING_PARAM("USB: W2FC_IRQ_EN", W2FC_IRQ_EN);
	

	//CAUTION WATCH OUT if W2FC_SYSCON1 = W2FC_SYSCON1_CFG_LOCK  is used the 
	//pullup_enable will be removed causing the device to be unconifured again
	//so use |= instead
	// Set SYSCON1.Cfg_Lock to '1' //
	W2FC_SYSCON1 |= W2FC_SYSCON1_CFG_LOCK;

	//FOR PR 190
	//	W2FC_SYSCON1 |= W2FC_SYSCON1_NAK_EN ; //Enable NAK bit for Debug purpose
	USB_TRACE_WARNING_PARAM("USB: W2FC_SYSCON1 ", W2FC_SYSCON1);

	return(ret);
}

T_RV_RET prepare_rx_trsfr(UINT8 pep)
{
	T_RV_RET ret = USB_OK;

	//update the endpoint stall bitmap
	usb_env_ctrl_blk_p->bm_rx_ep_buf_stall &= ~(1U << pep);	
	
	//select endpoint
	// EP_NUM EP_Num = n
	// EP_NUM EP_Dir = 0
	// EP_NUM EP_Sel = 0
	// EP_NUM EP_Setup_Sel = 0
	USB_EP_NUM_PREP_EP(pep);

	// USB_TRACE_WARNING_PARAM("USB: W2FC_EP_NUM", W2FC_EP_NUM);

	//clear endpointhalt condition
	USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_CLR_HALT);
			
	// Set CTRL.Set_FIFO_En //
	// This will enable both FIFOs if double-buffering is used. //
	USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_SET_FIFO_EN);

	return(ret);
}
