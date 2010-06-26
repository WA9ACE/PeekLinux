/**
 * @file	usb_devstat.c
 *
 * USB handle device state changed interrupt functions .
 *
 * @author	Anton van Breemen
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					     Modification
 *	-------------------------------------------------------------------
 *	26/09/2003	Anton van Breemen (ICT)
 *  14/01/2004  Pierre-Olivier POUX	(PHILOG) Adding suspend and resume notifications
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#include "usb_devstat.h"
#include "usb/usb_w2fc.h"
#include "usb/usb_env.h"
#include "usb/usb_i.h"
#include "usb/usb_interface.h"
#include "usb/usb_inth.h"

#include "usb/usb_dma.h"
#include "inth/sys_inth.h"

/****************************************************************************************
 |	DEFINITIONS|
 V**************************************************************************************V*/

#define USB_HOST				TRUE


//for debug purposes only
UINT16 int_mask_reg2;
UINT16 int_level_reg;
UINT16 usb_devstat_reg;
UINT16 syscon1_reg;
UINT16 irq_en_reg;
UINT16 conf_core_reg;

extern UINT8 timer_started;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	extern BOOL callback_not_called;
#endif

T_RV_RET free_all_on_discon(void);

/****************************************************************************************
 |	EXTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/

/**
 *---------------------------------------------------------------*
 * @name force_to_default_configuration
 *
 * This function request sets the device configuration to 1
 *
 *---------------------------------------------------------------*
 */
/*@{*/
void force_to_default_configuration(void)
{
	USB_SEND_TRACE("USB: Force to default configuration", RV_TRACE_LEVEL_ERROR);

	//store configuration value
	USB_CNFGNR = 1;

	W2FC_SYSCON1 &= ~W2FC_SYSCON1_CFG_LOCK;
	USB_SEND_TRACE("USB: SET CONFIGURATION RECIEVED unlock current config", RV_TRACE_LEVEL_ERROR);

	//first fill the data structure with chosen configuration data.
	conf_if_xrefs(USB_CNFGNR); //configuration selected by USB HOST
	USB_TRACE_WARNING_PARAM("USB: xref table configured with config nr:",
		usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wValue);

	//send connect message to involved interfaces
	notify_swe_connect(TRUE);
      usb_env_ctrl_blk_p->bm_rx_ep_buf_stall = USB_ZERO_BMP;
      usb_env_ctrl_blk_p->bm_tx_ep_buf_stall = USB_ZERO_BMP; 

#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	if(callback_not_called == TRUE)
	{
		callback_not_called = FALSE;
		if(usb_env_ctrl_blk_p->ntfcCallback) {
			(*(usb_env_ctrl_blk_p->ntfcCallback))(USB_ENUM_SUCCESS_EVENT,usb_env_ctrl_blk_p->ntfcCtx);
		}
	}
#endif

	USB_SEND_TRACE("USB: connect message send",
		RV_TRACE_LEVEL_DEBUG_LOW);

	//set physical endpoints
	if(EndpointConfiguration() != USB_OK)
	{
		USB_SEND_TRACE("USB: EndpointConfiguration failed", RV_TRACE_LEVEL_ERROR);
	} else
	{
		USB_SEND_TRACE("USB: EndpointConfiguration succeeded", RV_TRACE_LEVEL_DEBUG_LOW);
	}
}
/*@}*/


 /**
 * @name usb_state_chngd_hndlr
 *
 * This function checks if the device state has changed and takes the appropriate action
 *
 * @return	NONE
 */
/*@{*/
void usb_state_chngd_hndlr(void)
{
	BOOL unplugged = FALSE;

	USB_SEND_TRACE("USB: usb_state_chngd_hndlr",
			RV_TRACE_LEVEL_DEBUG_LOW);

    USB_TRACE_WARNING_PARAM("@U@", W2FC_DEVSTAT);



	/*see if devstat register has changed*/
	if((W2FC_DEVSTAT & W2FC_DEVSTAT_ATT) != (usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_ATT))
	{
		/*the ATT bit has changed in current devstat register since the previous devstat*/
		/*There was an USB cable attachement or removal*/
		usb_att_unatt_hndlr();
		if(usb_env_ctrl_blk_p->host_data.hw_state == USB_HW_UNATTACHED)
		{
			unplugged = TRUE;
		}

	}
	/*see if devstat register has changed*/
	if(
		((W2FC_DEVSTAT & W2FC_DEVSTAT_USB_RESET) != (usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_USB_RESET)) ||
		((W2FC_DEVSTAT & W2FC_DEVSTAT_DEF) != (usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_DEF))
	  )
	{
		/*the reset bit has changed in current devstat register since the previous devstat*/
		/*or the device has been reset and is in default state now (end of reset)*/
		usb_reset_hndlr();
		usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem = W2FC_DEVSTAT;
	}
	/*see if devstat register has changed*/
	if((W2FC_DEVSTAT & W2FC_DEVSTAT_SUS) != (usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_SUS))
	{
		/*the bus didn't have any activity for 5 ms so suspend, or resume after suspension*/
		usb_suspend_resume(USB_HOST);
	}
	/*see if devstat register has changed*/
	if((W2FC_DEVSTAT & W2FC_DEVSTAT_R_WK_OK) != (usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_R_WK_OK))
	{
		/*something changed the remote wake up bit*/
		if((W2FC_DEVSTAT & W2FC_DEVSTAT_R_WK_OK) == W2FC_DEVSTAT_R_WK_OK)
		{
			/*remote wakeup is enabled by the USB HOST*/
			usb_env_ctrl_blk_p->host_data.usb_rem_wake_enabled = TRUE;
			USB_SEND_TRACE("USB: remote wake up enabled", RV_TRACE_LEVEL_DEBUG_LOW);

		}
		else
		{
			/*remote wakeup is disabled by the USB HOST*/
			usb_env_ctrl_blk_p->host_data.usb_rem_wake_enabled = FALSE;
			USB_SEND_TRACE("USB: remote wake up disabled", RV_TRACE_LEVEL_DEBUG_LOW);
		}
	}
	/*see if devstat register has changed*/
	if((W2FC_DEVSTAT & W2FC_DEVSTAT_CFG) !=
		(usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_CFG))
	{
		/*configuration changed*/
		usb_cnfg_changed();
	}
	/*see if devstat register has changed*/
	if((W2FC_DEVSTAT & W2FC_DEVSTAT_ADD) !=
		(usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_ADD))
	{
		USB_TRACE_WARNING_PARAM("USB Test: W2FC_DEVSTAT", W2FC_DEVSTAT);
		/*device was addressed or defaulted again*/
		if((W2FC_DEVSTAT & W2FC_DEVSTAT_ADD) == W2FC_DEVSTAT_ADD)
		{
			/*handle transition from default state to adressed state*/
			change_usb_hw_state(USB_HW_ADRESSED);
			USB_SEND_TRACE("USB: hw state changed to USB_HW_ADRESSED",
				RV_TRACE_LEVEL_DEBUG_LOW);
			if(USB_CNFGNR != USB_CONF_DEFAULT)
			{
				/*the device was in the default state and changed to adressed but the
				configuration number should be USB_CONF_DEFAULT but it isn't. This
				requires at least some explanation*/
				USB_SEND_TRACE("USB: conf nr is supposed to be USB_CONF_DEFAULT at this stage but it isn't",
					RV_TRACE_LEVEL_DEBUG_LOW);
			}
			USB_CNFGNR = USB_CONF_ADRESSED;
			USB_SEND_TRACE("USB: conf nr was USB_CONF_DEFAULT, GOOD!!",
				RV_TRACE_LEVEL_DEBUG_LOW);

			force_to_default_configuration();
		}
		else
		{
			/*handle transition from adressed state to default state*/
			change_usb_hw_state(USB_HW_DEFAULT);
			if(USB_CNFGNR != USB_CONF_ADRESSED)
			{
				/*the device was in the adressed state and changed to default but the
				configuration number should be USB_CONF_ADRESSED still, but it isn't. This
				requires at least some explanation*/
				USB_SEND_TRACE("USB: conf nr supoposed to be USB_CONF_ADRESSED at this stage but isn't", RV_TRACE_LEVEL_DEBUG_LOW);
			}
			USB_SEND_TRACE("USB: hw state changed to USB_HW_DEFAULT", RV_TRACE_LEVEL_DEBUG_LOW);
			USB_CNFGNR = USB_CONF_DEFAULT;
		}
	}

	if(TRUE == unplugged)
	{
		usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem &= (~W2FC_DEVSTAT_ATT);
		usb_env_ctrl_blk_p->host_data.hw_state = USB_HW_UNATTACHED;
	}
	/*store W2FC_DEVSTAT value to refer with the next state changed interrupt*/
	usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem = W2FC_DEVSTAT;

	/*clear the devstat interrupt*/
	W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_DS_CHG);
	USB_SEND_TRACE("USB: state changed interrupt cleared", RV_TRACE_LEVEL_DEBUG_LOW);
}
/*@}*/


/**
 * @name usb_attached_unattached_hndlr
 *
 * This function handles the attached unattached behaviour
 *
 * @return	NONE
 */
/*@{*/
T_RV_RET usb_att_unatt_hndlr(void)
{
	UINT8 cntr;
	UINT8 list_cntr;
	BOOL exist = FALSE;
	BOOL attached =  FALSE;
	T_USB_BUS_CONNECTED_MSG* usb_connected_msg_p;
	T_USB_BUS_DISCONNECTED_MSG* usb_disconnected_msg_p;

	USB_SEND_TRACE("USB: usb_att_unatt_hndlr",
			RV_TRACE_LEVEL_DEBUG_LOW);

	//here don't look at the delta as in usb_state_chngd_hndlr function
	//because we need to know if it is attached or detached
	if((W2FC_DEVSTAT & W2FC_DEVSTAT_ATT) == W2FC_DEVSTAT_ATT) {
		/*the device is connected to the USB bus*/
		/*set local attached variable to TRUE*/

		/*set the usb connected variable to true*/
		usb_env_ctrl_blk_p->host_data.usb_connected = TRUE;
		attached = TRUE;
		change_usb_hw_state(USB_HW_ATTACHED);
		USB_SEND_TRACE("USB : state chng to USB_HW_ATTACHED", RV_TRACE_LEVEL_DEBUG_LOW);

		//pullup enable
		W2FC_SYSCON1 |= W2FC_SYSCON1_PULLUP_EN;

		// moved to Set Configuration request  usb_inth.c
		//notify_swe_connect(attached);
		/*bitmaps for every endpoint buffer to default (empty)*/
		usb_env_ctrl_blk_p->bm_rx_ep_buf_stall = USB_ZERO_BMP;
		usb_env_ctrl_blk_p->bm_tx_ep_buf_stall = USB_ZERO_BMP;
	}
	else {
		//the device is disconnected from the USB bus
		//set local attached variable to FALSE


		//set the usb connected variable to true
		usb_env_ctrl_blk_p->host_data.usb_connected = FALSE;
		attached = FALSE;
		USB_SEND_TRACE("USB : state chng to USB_HW_UNATTACHED", RV_TRACE_LEVEL_DEBUG_LOW);
		change_usb_hw_state(USB_HW_UNATTACHED);
		W2FC_SYSCON1 &=(~W2FC_SYSCON1_SOFF_DIS);

		if(timer_started == 0x1)
		{
			usb_stop_timer();
		}
		//take appropriate action to clear allocated buffers and clear enpoints etc
		notify_swe_connect(attached);

		USB_CNFGNR = USB_CONF_DEFAULT;
		reset_registers();
		init_physical_eps();
		free_all_dma_channels_on_discon();
		free_all_on_discon();

		/*bitmaps for every endpoint buffer to default (empty)*/
		usb_env_ctrl_blk_p->bm_rx_ep_buf_stall = USB_ZERO_BMP;
		usb_env_ctrl_blk_p->bm_tx_ep_buf_stall = USB_ZERO_BMP;
	}
	return USB_OK;
}
/*@}*/

/**
 * @name usb_reset_hndlr
 *
 * This function handles USB HOST initiated reset behaviour
 *
 * @return	NONE
 */
/*@{*/
T_RV_RET usb_reset_hndlr(void)
{

	T_RV_RET ret = USB_OK;
	UINT16 ep_cntr;
	UINT8 i;

	USB_SEND_TRACE("USB: usb_reset_hndlr",
			RV_TRACE_LEVEL_DEBUG_LOW);

	//look at the delta! is the reset toggled it means another reset cmd else
	//the default state must have been changed
	if((W2FC_DEVSTAT & W2FC_DEVSTAT_USB_RESET) !=
		(usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_USB_RESET))
	{
		/*a reset is initiated by the USB host*/
		if(usb_env_ctrl_blk_p->host_data.hw_state == USB_HW_UNATTACHED)
		{
			/*a reset from the USB HOST can only occur if the cable is attached*/
			USB_SEND_TRACE("USB: a USB HOST reset can only occur when the cable is attached",
				RV_TRACE_LEVEL_DEBUG_LOW);
			ret = USB_INTERNAL_ERR;
		}
		else
		{

			    USB_SEND_TRACE("USB :A USB HOST initiated reset has occurred",
				    RV_TRACE_LEVEL_DEBUG_LOW);

			    /*clear ep transactions*/
			    clr_ep_flgs();

			    /*application specific action to clear configuration and alternative interface settings*/
			    USB_CNFGNR = USB_CONF_DEFAULT;	/*set to -1 because it is
																				    in default state when it
																				    becomes adressed it will
																				    be 0 and > 0 when it is
																				    configured*/
			    /*appl specific actions to mark all endpoints unhalted*/
			    /*clear all rx halted ep's*/
			    for(ep_cntr = 1; ep_cntr <= 15; ep_cntr++)
			    {
				    USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);
				    /*EP_Dir can stay 0 for it is an out (rx) endpoint*/
				    USB_EP_NUM_RX_RELEASE((ep_cntr & 0x000F)); /*select the endpoint*/
				    /*the correct ep is selected now clear the selected halted endpoint*/
				    USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_CLR_HALT);
			    }
			    /*clear all halted tx ep's*/
			    for(ep_cntr = 1; ep_cntr <= 15; ep_cntr++)
			    {
				    USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);
				    /*EP_Dir must be 1 for it is an in (tx) endpoint*/
				    USB_EP_NUM_TX_RELEASE(ep_cntr & 0x000F); /*select the endpoint*/
				    /*the correct ep is selected now clear the selected halted endpoint*/
				    USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_CLR_HALT);
			    }

			    /*appl spec action to mark device not remote wakeup enabled*/
			    usb_env_ctrl_blk_p->host_data.usb_rem_wake_enabled = FALSE; /*disable remote wake_up*/

			    /*appl spec action to mark device not in suspend mode*/
			    change_usb_hw_state(USB_HW_DEFAULT);
			    W2FC_SYSCON1 |= W2FC_SYSCON1_SOFF_DIS;

			    /*appl spec actions to clear local copy of frame nr*/
			    /***NOT SUPPORTED FOR VERSION 3.0***/
			}
		// Enable all interrupts again (but SOF).

		// If Start Of Frame (SOF) interrupt is needed (isochronous mode),
		// just 'or' the W2FC_IRQ_EN_SOF_IE define to the next statement :
		USB_REG16_ASSIGN(W2FC_IRQ_EN, (W2FC_IRQ_EN_EPN_RX_IE | W2FC_IRQ_EN_EPN_TX_IE |
                                               W2FC_IRQ_EN_DS_CHG_IE | W2FC_IRQ_EN_EP0_IE /*| W2FC_IRQ_EN_SOF_IE*/));

		/*We get reset to come-out of resume so --Sidhu*/
		notify_swe_connect(FALSE);

		// wait 5 cycles, the hardware seems to need some time to update the registers
		//for(i = 0; i < 5; i++);
		wait_ARM_cycles(convert_nanosec_to_cycles(2500));  /* 6 wait state so 134ns */

	}
	/*it must be ready reseting than (set state to default)*/
	if((W2FC_DEVSTAT & W2FC_DEVSTAT_DEF) !=
		(usb_env_ctrl_blk_p->host_data.usb_ds_chg_mem & W2FC_DEVSTAT_DEF))
	{
		change_usb_hw_state(USB_HW_DEFAULT);
		USB_SEND_TRACE("USB : hw state changed to USB_HW_DEFAULT",
			RV_TRACE_LEVEL_DEBUG_LOW);
	}
	return ret;
}
/*@}*/


/**
 * @name usb_suspend_resume
 *
 * This function handles suspend resume behaviour
 *
 *@param	lh_or_usbh:	this boolean indicates whether the USB HOST requests a resume (TRUE) or
 *						if the local host request the resume (FALSE)
 *
 *@return	USB_OK, USB_INTERNAL_ERR
 */
/*@{*/
T_RV_RET usb_suspend_resume(BOOL lh_or_usbh)
{
	T_RV_RET ret = USB_OK;

	USB_SEND_TRACE("USB: usb_suspend_resume",
			RV_TRACE_LEVEL_DEBUG_LOW);
	if(lh_or_usbh == TRUE)
	{
		/*suspend or resume requested by the USB HOST*/
		if((W2FC_DEVSTAT & W2FC_DEVSTAT_SUS) == W2FC_DEVSTAT_SUS)
		{
			/*USB HOST has suspended the Device*/

			/*store the previous state so we can return to the previouis state
			at resume time*/
			usb_env_ctrl_blk_p->host_data.state_prior_to_sus = usb_env_ctrl_blk_p->host_data.hw_state;
			change_usb_hw_state(USB_HW_SUSPENDED);
			W2FC_SYSCON1 &=(~W2FC_SYSCON1_SOFF_DIS);

			/* Notify suspend notification */
			notify_swe_suspend_state(TRUE);
			USB_SEND_TRACE("USB : hw state changed to USB_HW_SUSPENDED", RV_TRACE_LEVEL_DEBUG_LOW);
		}
		else
		{
			/*USB HOST has resumed the Device*/

			/*restore to the previous state*/
			change_usb_hw_state(usb_env_ctrl_blk_p->host_data.state_prior_to_sus);
			ret = USB_OK;
			W2FC_SYSCON1 |= W2FC_SYSCON1_SOFF_DIS;

			/* Notify resume notification */
			notify_swe_suspend_state(FALSE);
			USB_SEND_TRACE("USB: USB RESUMED suspension is released, hw_state returns to previous state", RV_TRACE_LEVEL_DEBUG_LOW);
		}
	}
	else
	{
		/*resume requested by the Local Host (usb driver itself)*/
		if((W2FC_DEVSTAT & W2FC_DEVSTAT_R_WK_OK) == W2FC_DEVSTAT_R_WK_OK)
		{
			/*the remote wake up functionality is enabled*/
			if(usb_env_ctrl_blk_p->host_data.usb_rem_wake_enabled)
			{
				/*the USB_HOST has allowed remote wake up this is done in some
				"set feature" default command*/

				/*initiate resume*/
				USB_REG16_ASSIGN(W2FC_SYSCON2, W2FC_SYSCON2_RMT_WKP);
				ret = USB_OK;
				W2FC_SYSCON1 |= W2FC_SYSCON1_SOFF_DIS;
				USB_SEND_TRACE("USB: driver hw resumed on LH request", RV_TRACE_LEVEL_DEBUG_LOW);
			}
			else
			{
				/*resume requested but not allowed*/
				ret = USB_INTERNAL_ERR;
				USB_SEND_TRACE("USB: LH has requested a resume but this is not allowed", RV_TRACE_LEVEL_DEBUG_LOW);
			}
		}
		else
		{
			/*resume request not allowed because W2FC_DEVSTAT_R_WK_OK bit is not set*/
				ret = USB_INTERNAL_ERR;
				USB_SEND_TRACE("USB: LH has requested a resume but this is not allowed", RV_TRACE_LEVEL_DEBUG_LOW);
		}
	}
	return ret;
}
/*@}*/

/**
 * @name change_usb_hw_state
 *
 * This function handles changes the usb driver hardware state
 * @return	NONE
 */
/*@{*/
void change_usb_hw_state(T_USB_HW_STATE hw_state)
{
	 usb_env_ctrl_blk_p->host_data.hw_state = hw_state;
}
/*@}*/

/**
 * @name usb_cnfg_changed
 *
 * This function handles configuration changes initiated by the USB HOST
 *
 * @return	T_RV_RET:	USB_OK USB_INTERNAL_ERR
 */
/*@{*/
void usb_cnfg_changed(void)
{
	USB_SEND_TRACE("USB: usb_cnfg_changed function",
			RV_TRACE_LEVEL_DEBUG_LOW);
	USB_TRACE_WARNING_PARAM("USB Test: W2FC_DEVSTAT", W2FC_DEVSTAT);
	if((W2FC_DEVSTAT & W2FC_DEVSTAT_CFG) == W2FC_DEVSTAT_CFG)
	{
		/*handle transition from adressed state to configured state*/
		change_usb_hw_state(USB_HW_CONFIGURED);
		USB_SEND_TRACE("USB: hw state changed to USB_HW_CONFIGURED",
			RV_TRACE_LEVEL_DEBUG_LOW);

	}
	else
	{
		//the current config must be removed
/*		reset_registers();
		init_physical_eps();
		W2FC_SYSCON1 &= ~W2FC_SYSCON1_CFG_LOCK;*/
		if((W2FC_DEVSTAT & W2FC_DEVSTAT_ADD) == W2FC_DEVSTAT_ADD)
		{
			init_dma_usb_tbl();
			/*handle transition from configured state to adressed state*/
			change_usb_hw_state(USB_HW_ADRESSED);
			USB_SEND_TRACE("USB: hw state changed to USB_HW_ADRESSED",
				RV_TRACE_LEVEL_DEBUG_LOW);
			USB_CNFGNR = USB_CONF_ADRESSED;

		}
		else
		{
			W2FC_SYSCON1 &= ~W2FC_SYSCON1_CFG_LOCK;
			reset_registers();
			init_physical_eps();
		
			/*handle transition from configured state to default state*/
			change_usb_hw_state(USB_HW_DEFAULT);
			USB_SEND_TRACE("USB: hw state changed to USB_HW_DEFAULT",
				RV_TRACE_LEVEL_DEBUG_LOW);
			USB_CNFGNR = USB_CONF_DEFAULT;

		}
	}
}
/*@}*/

/**
 * @name clr_ep_flgs
 *
 * This function cancels all ongoing endpoint transactions and clears all
 * endpoint flags. It empties the endpoint buffers provided by the FM's
 *
 * @return	NONE
 */
/*@{*/
void clr_ep_flgs(void)
{
	UINT16 ep_cntr;


	USB_SEND_TRACE("USB: clr_ep_flags function",
			RV_TRACE_LEVEL_DEBUG_LOW);
	/*clear endpoints transactions and control transfer flags*/
	/*-Here we only use the CTRL.Clr_EP bit. With a specific clear_feature (halt)
	request from the Host the CTRL.Reset_EP will be used (see USB_W2FC 1.10 documentation)-*/
	/*clear all rx ep's*/
	for(ep_cntr = 1; ep_cntr <= 15; ep_cntr++)
	{
		USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);
		/*EP_Dir can stay 0 for it is an out (rx) endpoint*/
		USB_EP_NUM_RX_RELEASE(ep_cntr & 0x000F); /*select the endpoint*/
		/*the correct ep is selected now clear the selected endpoint*/
		USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_CLR_EP);
	}
	/*clear all tx ep's*/
	for(ep_cntr = 1; ep_cntr <= 15; ep_cntr++)
	{
		USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);
		/*EP_Dir must be 1 for it is an in (tx) endpoint*/
		USB_EP_NUM_TX_RELEASE(ep_cntr & 0x000F);

		USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_CLR_EP);
	}
}
/*@}*/

T_RV_RET free_all_on_discon(void)
{
	UINT16 cntr;

	USB_SEND_TRACE("USB: free_all_on_discon",
			RV_TRACE_LEVEL_DEBUG_LOW);

	/**
	 *free interface variables
	 */
	if (usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl != NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl);
		usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl= NULL;
	}
	if (usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl != NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl);
		usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl = NULL;
	}
	if (usb_env_ctrl_blk_p->endpoint_rx_descr_tbl != NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->endpoint_rx_descr_tbl);
		usb_env_ctrl_blk_p->endpoint_rx_descr_tbl = NULL;
	}
	if (usb_env_ctrl_blk_p->endpoint_tx_descr_tbl != NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->endpoint_tx_descr_tbl);
		usb_env_ctrl_blk_p->endpoint_tx_descr_tbl = NULL;
	}

	/*we delete the buffer here also, just to make sure, */
/*	if(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p != NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p);
		usb_env_ctrl_blk_p->host_data.ep0_setup.request_p = NULL;
	}*/
	if(usb_env_ctrl_blk_p->host_data.ep0_data_p != NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->host_data.ep0_data_p);
		usb_env_ctrl_blk_p->host_data.ep0_data_p = NULL;
	}
	return(USB_OK);
}
