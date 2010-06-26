/**
 * @file	usb_inth.c
 *
 * USB initialise and handle interrupt functions .
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
 *  14/01/2004  Pierre-Olivier POUX		Adding management class and vendor requests
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */
#include "usb/usb_inth.h"
#include "nucleus.h"
#include <string.h>
#include "sys_types.h"
#include "usb/usb_w2fc.h"
#include "usb_devstat.h"
#include "usb/usb_env.h"
#include "usb/usb_i.h"
#include "usb/usb_interface.h"
#include "inth/sys_inth.h"

#include "usb/usb_dma.h"
#include "usb/usb_pwr.h"


/****************************************************************************************
 |	DEFINITIONS																			|
 V**************************************************************************************V*/

// Size of the HISR stack associated to the RNG interrupt
#if (REMU==1)
#define USB_HISR_STACK_SIZE			(1024)
#else
#define USB_HISR_STACK_SIZE			(512)
#endif

#define USB_CNTRL_DIR					0x80	//direction of the setup control transfer

#define WITH_CMD_STALL					TRUE
#define WITHOUT_CMD_STALL				FALSE

//defines for get status request
#define USB_GET_STAT_REQ_DAT_LNTH		2		//length of data of USB GET STATUS request
#define USB_IF_RET_DATA					0x0000

//defines for get configuration standard request
#define USB_GET_CFG_REQ_TYPE			0x80
#define USB_GET_CONF_REQ_DAT_LNTH		1

//defines for get interface standard request
#define USB_GET_IF_REQ_TYPE				0x81

//defines for get_descriptor standard request
#define USB_CNFG_ALL					TRUE
#define USB_CNFG_SINGLE					FALSE

#define USB_DEV_REQ_TYPE				0x80
#define USB_IF_REQ_TYPE					0x81
#define USB_EP_REQ_TYPE					0x82

#define MAX_DMA_CHN						3		//DMA is not supported yet but when it is,
												//there will be a maximum of 3 channels available

/****************************************************************************************
 |	MACRO DEFINITIONS																	|
 V**************************************************************************************V*/

#define USB_REQUEST			usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->bRequest
#define USB_REQUEST_TYPE	usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->bmRequestType
#define USB_REQUEST_LENGTH	usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wLength
#define USB_REQUEST_VALUE	usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wValue
#define USB_REQUEST_INDEX	usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wIndex

#define USB_SETUP_OUT		usb_env_ctrl_blk_p->host_data.ep0_setup.setup_out
#define USB_SETUP_IN		usb_env_ctrl_blk_p->host_data.ep0_setup.setup_in

#define USB_RECIPIENT_DEVICE		0	
#define USB_RECIPIENT_INTERFACE		1
#define USB_RECIPIENT_ENDPOINT		2

#define USB_DEVICE_REMOTE_WAKEUP	1
#define USB_ENDPOINT_HALT			0

/****************************************************************************************
 |	FUNCTION PROTOTYPES																	|
 V**************************************************************************************V*/

static void usb_setup_hndlr(void);
static void parse_setup_cmd(T_USB_DEV_REQ* request_p);
static T_RV_RET get_setup_data_mem(UINT8** setup_p, UINT16 size);
static void clr_setup_mem(void);
T_RV_RET write_non_iso_tx_fifo_data(UINT16 nr_bytes_in_app_tx_buf, UINT16 ep_fifo_size, 
									   UINT16* nr_bytes_place_in_fifo, UINT8* buffer_p);
T_RV_RET read_non_iso_rx_fifo_data(UINT16 pep_nr, UINT16* nr_bytes_from_fifo, UINT8* buffer_p);
static T_RV_RET det_req_spec_action(void);

static void get_status(void);

static void set_descriptor(void);
static void get_descriptor(void);

static void set_configuration(void);
static void get_configuration(void);

static void set_interface(void);
static void get_interface(void);

static void set_feature(void);
static void clear_feature(void);

static void synch_frame(void);

static void dummy(void);
static void vendor_test_request(void);
static void ep0_tx_int_hndlr(void);
static void ep0_rx_int_hndlr(void);
static T_RV_RET epn_int_hndlr(void);
T_RV_RET non_iso_rx_hndlr(UINT8 endp_nb);
T_RV_RET non_iso_tx_hndlr(UINT8 endp_nb);
static void abort_setup_req(BOOL with_or_without_cmd_stall);
static void prepare_for_cntrl_read_stat_stage(UINT8 ep_nr, T_USB_RESP_TYPE ack_or_stall);
static void prepare_for_cntrl_write_stat_stage(UINT8 ep_nr,T_USB_RESP_TYPE ack_or_stall);
T_RV_RET stall_req_and_ep(UINT8 phys_ep_nr, BOOL direction);
static T_RV_RET send_conf_descr(UINT16* nr_bytes_in_buf, UINT8 descr_index, 
								UINT16* placed_in_fifo_p);
static void clear_reset_tx_ep(UINT8 ep_nr);
static void clear_reset_rx_ep(UINT8 ep_nr);
static void clear_setup_buf(void);
static void calc_no_trsf(void);
BOOL det_fifo_full_db(UINT16 pep_nr, UINT16* ep_buf_size, BOOL rx_tx);
T_RV_RET notify_rx_buffer_full(UINT32 pep_idx, UINT32 size, BOOL end_of_packet);
T_RV_RET notify_tx_buffer_empty(UINT32 pep_idx);
T_RV_RET read_non_iso_packet(UINT8 phys_ep_nr);
T_RV_RET read_non_iso_packet_protected(UINT8 phys_ep_nr);
T_RV_RET write_non_iso_packet(UINT8 phys_ep_nr);
T_RV_RET write_non_iso_packet_protected(UINT8 phys_ep_nr);
static void store_req(BOOL dummyfnc);

static T_RV_RET fill_cdc_descr(const T_USB_CDC_DESCR	*current_cdc_descritor, UINT8* cdc_descr_buffer);
T_RV_RET notify_ep0_tx_buffer_empty(UINT32 interface_id);
T_RV_RET notify_ep0_rx_buffer_full(UINT32 interface_id, UINT32 size, BOOL end_of_packet);

static void specific_ep0_tx_handler();
static BOOL specific_ep0_check_interface_validity();
static void specific_ep0_rx_handler();
static void specific_ep0_setup_device_to_host_handler();
static void send_empty_packet(UINT8 ep_nr);

static void store_specific_ep0_handler(T_SPECIFIC_EP_OPERATION ep0_operation);
/****************************************************************************************
 |	VARIABLE DECLARATIONS																|
 V**************************************************************************************V*/

static NU_HISR USB_Hisr;
#ifndef HISR_STACK_SHARING
static char USB_HisrStack[USB_HISR_STACK_SIZE];
#endif
static UINT16 inth_reg;

#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	extern BOOL callback_not_called;
#endif

/**
 * @name setup_stnd_fnc_table
 *
 * This table contains pointers to the standard usb setup functions
 *
 * This is used to determine the USB standard function that is using endpoint 0
 *
 * @return	NONE
 */
/*@{*/
const STND_FUNC setup_stnd_fnc_table[] = {
	get_status,		
	clear_feature,	
	dummy,			/* reserved for future use Chapter 9 of the USB Specification */
	set_feature,	
	dummy,			/* reserved for future use Chapter 9 of the USB Specification */
	dummy,			/* SET_ADDRESS managed by the W2FC */
	get_descriptor,
	set_descriptor,
	get_configuration,
	set_configuration,
	get_interface,
	set_interface,
	synch_frame
};
/*@}*/


/****************************************************************************************
 |	EXTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/

/**
*-----------------------------------------------------------------------*
 * @name USB_HisrEntry
 *
 * This function is called when an USB interrupt is received.
 *
 * @return	NONE
 *-----------------------------------------------------------------------* 
 */
/*@{*/
static void USB_HisrEntry(void)
{
	//check if driver is ready
	if(usb_env_ctrl_blk_p->driver_ready == TRUE) {
		 T_USB_INTERNAL_STATE prev_state;		//holds the previous state to which the driver will return 
												//after handling the interrupt
		prev_state = usb_env_ctrl_blk_p->state;
		change_internal_state(USB_HANDLE_INTERRUPT);
	
		//check the driver state
		if((usb_env_ctrl_blk_p->state == USB_IDLE) || (usb_env_ctrl_blk_p->state == USB_HANDLE_REQUEST)
			|| (usb_env_ctrl_blk_p->state == USB_HANDLE_INTERRUPT)) {
			if(prev_state == USB_HANDLE_INTERRUPT) {
				USB_SEND_TRACE("USB: interrupt occured during processing another interrupt", 
					RV_TRACE_LEVEL_DEBUG_LOW);
			}
			if((inth_reg & W2FC_IRQ_SRC_DS_CHG) == W2FC_IRQ_SRC_DS_CHG) {
				/*a device state changed interrupt has occurred*/
				USB_SEND_TRACE("USB: driver state changed interrupt", 
					RV_TRACE_LEVEL_DEBUG_LOW);
				/*call device state changed handler*/
				USB_TRACE_WARNING_PARAM("USB before : W2FC_IRQ_EN", W2FC_IRQ_EN);
				usb_state_chngd_hndlr();
				USB_TRACE_WARNING_PARAM("USB after  : W2FC_IRQ_EN", W2FC_IRQ_EN);
			}
			/*check the interrupt source*/
			if((inth_reg & W2FC_IRQ_SRC_SETUP) == W2FC_IRQ_SRC_SETUP) {
				/*setup handler*/
				USB_SEND_TRACE("USB: setup interrupt has occurred", 
					RV_TRACE_LEVEL_DEBUG_LOW);
				/*call USB interrupt setup handler*/
				usb_setup_hndlr();
			}
			if((inth_reg & W2FC_IRQ_SRC_EP0_RX) == W2FC_IRQ_SRC_EP0_RX) {
				/*a endpoint 0 RX transaction has occurred*/
					USB_SEND_TRACE("USB: endpoint 0 RX transaction has occurred", 
					RV_TRACE_LEVEL_DEBUG_LOW);
				ep0_rx_int_hndlr();  //call power mgmt api inside this.
			}
			if((inth_reg & W2FC_IRQ_SRC_EP0_TX) == W2FC_IRQ_SRC_EP0_TX) {
				/*a endpoint 0 TX transaction has occurred*/
				USB_SEND_TRACE("USB: endpoint 0 TX transaction has occurred", 
					RV_TRACE_LEVEL_DEBUG_LOW);
				ep0_tx_int_hndlr();  //call power mgmt api inside this.
			}
			/***NOT SUPPORTED FOR VERSION 3.0***/
			if((inth_reg & W2FC_IRQ_SRC_RXN_EOT) == W2FC_IRQ_SRC_RXN_EOT) {
				/*Non ISO RX DMA end of transfer handler*/
				USB_SEND_TRACE("USB: Non ISO RX DMA end of transfer interrupt has occurred", 
					RV_TRACE_LEVEL_DEBUG_LOW);
				usb_non_iso_rx_dma_eot_hndlr();  //call power mgmt api inside this.
			}
			/***NOT SUPPORTED FOR VERSION 3.0***/
			if((inth_reg & W2FC_IRQ_SRC_RXN_CNT) == W2FC_IRQ_SRC_RXN_CNT) {
				/*Non ISO RX DMA transactions count handler*/
				USB_SEND_TRACE("USB: Non ISO RX DMA transactions count handler interrupt has occurred", 
					RV_TRACE_LEVEL_DEBUG_LOW);
				usb_non_iso_rx_dma_transaction_count_hndlr();  //call power mgmt api inside this.
			}
			/***NOT SUPPORTED FOR VERSION 3.0***/
			if((inth_reg & W2FC_IRQ_SRC_TXN_DONE) == W2FC_IRQ_SRC_TXN_DONE) {
				/*Non ISO RX DMA transactions done handler*/
				USB_SEND_TRACE("USB: Non ISO TX DMA transactions done handler interrupt has occurred",
					RV_TRACE_LEVEL_DEBUG_LOW);
				usb_non_iso_tx_dma_done_hndlr();  //call power mgmt api inside this.
			}
			if((inth_reg & W2FC_IRQ_SRC_EPN_RX) == W2FC_IRQ_SRC_EPN_RX) {
				/*interrupt on RX endpoint x has occurred*/
				USB_SEND_TRACE("USB: RX endpoint x  interrupt has occurred", 
					RV_TRACE_LEVEL_DEBUG_LOW);  
				epn_int_hndlr();  //call power mgmt api inside this.
			}
			if((inth_reg & W2FC_IRQ_SRC_EPN_TX) == W2FC_IRQ_SRC_EPN_TX) {
				/*interrupt on TX endpoint x has occurred*/
				USB_SEND_TRACE("USB: TX endpoint x  interrupt has occurred", 
					RV_TRACE_LEVEL_DEBUG_LOW);
				epn_int_hndlr();  //call power mgmt api inside this.
			}
			/***NOT SUPPORTED FOR VERSION 3.0***/
			// If Start Of Frame interrupt is needed (isochronous mode),
			// then uncomment the following lines (if statement) : 
			if((inth_reg & W2FC_IRQ_SRC_SOF) == W2FC_IRQ_SRC_SOF)
			{
				W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_SOF);
			}
			/*set state to original state*/
			change_internal_state(prev_state);
		}
		else {
			/*driver is in wrong state*/
			USB_SEND_TRACE("USB: interrupt occured while usb driver was in wrong state", 
				RV_TRACE_LEVEL_DEBUG_LOW);
		}
	}
	else {
		/*driver is not ready*/
		USB_SEND_TRACE("USB: interrupt occured while usb driver wasn't ready", 
			RV_TRACE_LEVEL_DEBUG_LOW);
	}
	usb_int_clr();
}

/**
 *-----------------------------------------------------------------------*
 * @name usb_int_init
 *
 * This function is called to initialise the usb interrupt handler		 
 * mechanism															 
 *
 * @return	NONE
 *-----------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET usb_int_init()
{
	STATUS status_ret;
	
	// Fill the entire stack with the pattern 0xFE
    #ifndef HISR_STACK_SHARING
	memset (USB_HisrStack, 0xFE, sizeof(USB_HisrStack));
    #endif

	// Create the HISR which is called when an RNG interrupt is received.
	status_ret = NU_Create_HISR(&USB_Hisr, "USB_HISR", USB_HisrEntry, 2,
    		#ifndef HISR_STACK_SHARING
		       	USB_HisrStack, sizeof(USB_HisrStack)); // lowest prty
    		#else
			HISR_STACK_PRIO2,
			HISR_STACK_PRIO2_SIZE); // lowest prty
    		#endif
	
	if (status_ret != NU_SUCCESS) {
		USB_SEND_TRACE("USB: interrupt inittialisation failed", 
			RV_TRACE_LEVEL_ERROR);
		return RVM_INTERNAL_ERR;
	}
	return USB_OK;
}

/**
 *-----------------------------------------------------------------------*
 * @name usb_int_clr
 *
 *  This function is called to clear the interrupt and to (re)enable the
 *  interrupt again															 
 *
 * @return	NONE
 *-----------------------------------------------------------------------* 
 */
/*@{*/
void usb_int_clr(void)
{
	/*re enable interrupt*/
	F_INTH_ENABLE_ONE_IT(C_INTH_USB_IT);
}

/**
 *-----------------------------------------------------------------------*
 * @name Activate_USB_HISR
 *
 *  This function is called from the interrupt handler to activate        
 * the HISR associated to the USB External Interrupt.
 *
 * @return	NONE
 *-----------------------------------------------------------------------* 
 */
/*@{*/
SYS_BOOL Activate_USB_HISR(void)
{
	if(NU_SUCCESS != NU_Activate_HISR(&USB_Hisr)) {
		return 1;
	}
	return 0;
}

/**
 *-----------------------------------------------------------------------*
 * @name disable_interrupt_hndlr
 *
 * This function is called from the USB init routine to delete the       
 * previously created HISR associated to the USB External Interrupt.
 *
 * @return	NONE
 *-----------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET disable_interrupt_hndlr(void)
{
	if (NU_Delete_HISR (&USB_Hisr) != NU_SUCCESS) {
		return USB_INTERNAL_ERR;
	}
	else {
		return USB_OK;
	}
}

/**
 *-----------------------------------------------------------------------*
 * @name usb_int_handler
 *
 * This function is called when an interrupt occurs. It will first check 
 * if the USB module actually exist and if it is not uninitialised. When 
 * it exists and is initialised, the interrupt High interrupt service	 
 * routine is called that will in time call the USB_HisrEntry()   
 *
 * @return	NONE
 *-----------------------------------------------------------------------* 
 */
/*@{*/
void usb_int_handler(void)
{
	// Mask external interrupt (12)
	F_INTH_DISABLE_ONE_IT(C_INTH_USB_IT);

	// The external IRQ is mapped on the USB interrupt.
	// The associated HISR USB_Hisr is activated on reception on the external IRQ.
	// If usb is not started, return immediately 
	if ( (usb_env_ctrl_blk_p == 0) || (usb_env_ctrl_blk_p->state == USB_UNINITIALISED)) {
		usb_int_clr();
	}
	else {
		int i;
		inth_reg = W2FC_IRQ_SRC;
		if(Activate_USB_HISR()) {
			//handler NOT successfully executed 
			usb_int_clr();	
		}
	}	
}

/**
 *--------------------------------------------------------------------------------*
 * @name name fill_ep_descr
 *
 * This function creates an endpoint descriptor of a specified type (ep_type) and 
 * stores it in the provided temp_ep. Sometimes this function is called only for 
 * this descriptor, or it is called just to find out if the specified endpoint is
 * double buffered. Or for both reasons!
 *
 *	@param temp_ep:		pointer to location where endpoint descriptor can be stored
 *  @param ep_type:		type description of the endpoint descriptor to be created
 *
 *  @return	BOOL		Returns TRUE if endpoint is double buffered returns FALSE
 *						if endpoint is not Double buffered
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOL fill_ep_descr(T_USB_ENDPOINT_DESCR* temp_ep, T_USB_EP_DESCR ep_type)
{
	BOOL DB = FALSE;

	switch(ep_type)
	{
		case EP_DRX8:
			DB = TRUE; 	//falltrough
		case EP_RX8:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_BULK;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 8;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DRX16:	
			DB = TRUE; 	//falltrough
		case EP_RX16:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_BULK;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 16;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DRX32:	
			DB = TRUE; 	//falltrough
		case EP_RX32:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_BULK;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 32;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DRX64:	
			DB = TRUE; 	//falltrough
		case EP_RX64:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_BULK;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 64;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DTX8:	
			DB = TRUE; 	//falltrough
		case EP_TX8:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_BULK;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 8;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DTX16:
			DB = TRUE; 	//falltrough
		case EP_TX16:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_BULK;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 16;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DTX32:
			DB = TRUE; 	//falltrough
		case EP_TX32:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_BULK;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 32;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DTX64:	
			DB = TRUE; 	//falltrough
		case EP_TX64:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_BULK;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 64;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DCRX8:	
			DB = TRUE; 	//falltrough
		case EP_CRX8:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_CONTROL;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 8;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DCRX16:	
			DB = TRUE; 	//falltrough
		case EP_CRX16:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_CONTROL;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 16;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DCRX32:	
			DB = TRUE; 	//falltrough
		case EP_CRX32:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_CONTROL;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 32;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DCRX64:	
			DB = TRUE; 	//falltrough
		case EP_CRX64:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_CONTROL;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 64;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DCTX8:	
			DB = TRUE; 	//falltrough
		case EP_CTX8:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_CONTROL;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 8;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DCTX16:	
			DB = TRUE; 	//falltrough
		case EP_CTX16:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_CONTROL;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 16;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DCTX32:	
			DB = TRUE; 	//falltrough
		case EP_CTX32:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_CONTROL;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 32;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DCTX64:	
			DB = TRUE; 	//falltrough
		case EP_CTX64:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_CONTROL;
			temp_ep->iInterval = USB_DEF_INT;
			temp_ep->wMaxPacketSize = 64;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DIRX8:
			DB = TRUE; 	//falltrough
		case EP_IRX8:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_INTERRUPT;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 8;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DIRX16:
			DB = TRUE; 	//falltrough
		case EP_IRX16:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_INTERRUPT;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 16;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DIRX32:
			DB = TRUE; 	//falltrough
		case EP_IRX32:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_INTERRUPT;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 32;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DIRX64:
			DB = TRUE; 	//falltrough
		case EP_IRX64:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_INTERRUPT;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 64;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DITX8:
			DB = TRUE; 	//falltrough
		case EP_ITX8:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_INTERRUPT;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 8;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DITX16:
			DB = TRUE; 	//falltrough
		case EP_ITX16:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_INTERRUPT;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 16;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DITX32:
			DB = TRUE; 	//falltrough
		case EP_ITX32:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_INTERRUPT;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 32;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DITX64:
			DB = TRUE; 	//falltrough
		case EP_ITX64:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_INTERRUPT;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 64;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DSRX8:
			DB = TRUE; 	//falltrough
		case EP_SRX8:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_ISOCHRONOUS;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 8;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DSRX16:
			DB = TRUE; 	//falltrough
		case EP_SRX16:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_ISOCHRONOUS;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 16;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DSRX32:
			DB = TRUE; 	//falltrough
		case EP_SRX32:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_ISOCHRONOUS;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 32;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DSRX64:
			DB = TRUE; 	//falltrough
		case EP_SRX64:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_ISOCHRONOUS;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 64;
			temp_ep->bEndpointAddress = USB_OUT;
			break;
		case EP_DSTX8:
			DB = TRUE; 	//falltrough
		case EP_STX8:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_ISOCHRONOUS;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 8;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DSTX16:
			DB = TRUE; 	//falltrough
		case EP_STX16:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_ISOCHRONOUS;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 16;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DSTX32:
			DB = TRUE; 	//falltrough
		case EP_STX32:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_ISOCHRONOUS;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 32;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		case EP_DSTX64:
			DB = TRUE; 	//falltrough
		case EP_STX64:
			temp_ep->blength = USB_EP_DESCR_LGNT;
			temp_ep->bDescriptorType = USB_ENDPOINT;
			temp_ep->bmAttributes = USB_ISOCHRONOUS;
			temp_ep->iInterval = USB_STND_INT;
			temp_ep->wMaxPacketSize = 64;
			temp_ep->bEndpointAddress = USB_IN;
			break;
		default :
			/*there is no memory available to store setup data in*/
			USB_SEND_TRACE("USB: endpoint type is unrecognisable", RV_TRACE_LEVEL_ERROR);
			/*we don't want to wait untill memory is available, just quit*/
			abort_setup_req(WITH_CMD_STALL);
			break;
	}
	return(DB);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name name rmv_stall_cont_rx_trsfr
 *
 * This function is called from the API function set_rx_buffer. To make sure that 
 * the previous transfer is completed. If the transfer was already finished nothing
 * happens here. If there previous transfer is not completed yet, but was stalled
 * because the buffer was full, then remove the stall and continue transfer.
 *
 *	@param log_ep_nr:		logical endpoint number who's buffer is just provided
 *  @param if_nr:			interface number to which the logical endpoint belongs
 *
 *  @return	T_RV_RET 		Returns USB_OK when successfully executed this function
 *							returns USB_INTERNAL_ERR when failed
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET rmv_stall_cont_rx_trsfr(UINT8 log_ep_nr, UINT8 if_nr)
{
	UINT8 phys_ep_idx = 255;		//physical ep nr 255 does NOT exist
	UINT8 pep_nr;
	UINT16 remvove_stall;
	T_RV_RET ret = USB_OK;
	UINT8 i;

	static BOOL cpt = TRUE;

#ifdef  USB_DMA_USED
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
#endif

	F_INTH_DISABLE_ONE_IT(C_INTH_USB_IT);
	phys_ep_idx = LEP_2_PEP(log_ep_nr, if_nr, USB_RX);
	pep_nr = phys_ep_idx + 1;

	//check if endpoint was stalled
	if(phys_ep_idx == 255) {
		//apparently the endpoint was NOT found
		USB_SEND_TRACE("USB: logical endpoint_nr interface_nr doesn't exist", 
			RV_TRACE_LEVEL_ERROR);
		ret = USB_INTERNAL_ERR;
	} else {
		//we found the physical endpoint nr, now we can check if the endpoint is  stalled
		remvove_stall = usb_env_ctrl_blk_p->bm_rx_ep_buf_stall;
#ifndef  USB_DMA_USED
		if((remvove_stall & (1 << pep_nr)) == (1 << pep_nr))
		{
			//now a new buffer is available so remove stall en start read data
			//remove stall
			usb_env_ctrl_blk_p->bm_rx_ep_buf_stall &= ~(1 << pep_nr);
			
			W2FC_CTRL = W2FC_CTRL_CLR_HALT;
			USB_SEND_TRACE("USB: stall cleared",
				RV_TRACE_LEVEL_DEBUG_LOW);

			// Write EP_NUM register: //
			// EP_NUM.EP_Num = endp_nb //
			// EP_NUM.EP_Dir = 0       //
			// EP_NUM.EP_Sel = 1       //
			// EP_NUM.Setup_Sel = 0    //
			USB_EP_NUM_SEL_RX_EP(pep_nr);

			// RX Handler //
			if(read_non_iso_packet_protected(pep_nr) != USB_OK) {
				ret = USB_INTERNAL_ERR;
				USB_SEND_TRACE("USB: read_non_iso_packet failed",
					RV_TRACE_LEVEL_ERROR);
			}
			// Write EP_NUM register: //
			// EP_NUM.EP_Num = endp_nb //
			// EP_NUM.EP_Dir = 0       //
			// EP_NUM.EP_Sel = 0       //
			// EP_NUM.Setup_Sel = 0    //
			USB_EP_NUM_RX_RELEASE(pep_nr);

			// Flag Fifo not full and DB=1? //
			if(usb_env_ctrl_blk_p->host_data.fifo_not_full == TRUE) {
				// Write CTRL register: CTRL.Set_FIFO_En=1 //
				W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
			}
		}
		else
		{
			USB_EP_NUM_SEL_RX_EP(pep_nr);
			USB_EP_NUM_RX_RELEASE(pep_nr);
			W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
		}
#else
		//find physical ep
		endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl +
			(LEP_2_PEP(log_ep_nr, if_nr, USB_RX));

		/* Check if necessary to use DMA transfer */
		if(endpoint_vs_interface_entry_p->buf_p)
		{
			cpt = FALSE;
			if(endpoint_vs_interface_entry_p->buf_size >= DMA_RX_THRESHOLD) 
			{
				usb_prepare_dma_rx_transfer(if_nr, log_ep_nr);
			}
			else
			{
				if(if_nr == USB_IF_CDC_SERIAL)
				{
					/* Start the timer to track missing ZLP */
					usb_start_timer();
				}	
				/*
					User buffer is provided then enable endpoint fifo \
				*/
				USB_EP_NUM_SEL_RX_EP(pep_nr);
				W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
				USB_EP_NUM_RX_RELEASE(pep_nr);				

				USB_TRACE_WARNING_PARAM("USB: No initiate RX DMA transfer : size", endpoint_vs_interface_entry_p->buf_size);
			}
		}
#endif
	}
	F_INTH_ENABLE_ONE_IT(C_INTH_USB_IT);
	return(ret);
}

/**
 *--------------------------------------------------------------------------------*
 * @name name prepare_first_tx_trsfr
 *
 * This function is called from the API function set_tx_buffer. It copies the 
 * content of the provided buffer into the fifo of the specified endpoint. If the 
 * Buffer won't fit at once, the first ACKED TX interrupt on this endpoint will
 * continue coppying the rest of the buffer, untill it is finished
 * 
 *	@param log_ep_nr:		logical endpoint number who's buffer is just provided
 *  @param if_nr:			interface number to which the logical endpoint belongs
 *
 *  @return	T_RV_RET 		Returns USB_OK when successfully executed this function
 *							returns USB_INTERNAL_ERR when failed
 *---------------------------------------------------------------------------------* 
 */
T_RV_RET prepare_first_tx_trsfr(UINT8 log_ep_nr, UINT8 if_nr)
{
	UINT8 phys_ep_idx, phys_ep_nr;
	T_RV_RET ret = USB_OK;

#ifdef  USB_DMA_USED
	BOOL	found = FALSE;
	UINT8	i = 0;
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
#endif
	//watch out phys_ep_nr is an index in the xref table, must add 1 
	//to it to have the real physical ep_nr
	F_INTH_DISABLE_ONE_IT(C_INTH_USB_IT);
	phys_ep_idx = LEP_2_PEP(log_ep_nr, if_nr, USB_TX);
	phys_ep_nr = phys_ep_idx + 1;

	// Write EP_NUM register: //
	// EP_NUM.EP_Num = endp_nb //
	// EP_NUM.EP_Dir = 1       //
	// EP_NUM.EP_Sel = 1       //
	// EP_NUM.Setup_Sel = 0    // 
	USB_EP_NUM_SEL_TX_EP(phys_ep_nr);

	//since a buffer with valid data is provided enable endpoint!
	W2FC_CTRL = W2FC_CTRL_CLR_HALT;
	//update the endpoint stall bitmap remove stall condition for this ep

	usb_env_ctrl_blk_p->bm_tx_ep_buf_stall =   ((usb_env_ctrl_blk_p->bm_tx_ep_buf_stall)
											& (~(1U << phys_ep_nr )));

#ifndef  USB_DMA_USED
	// TX Handler //
	if(write_non_iso_packet_protected(phys_ep_idx) != USB_OK) {
		ret = USB_INTERNAL_ERR;
		USB_SEND_TRACE("USB: write_non_iso_packet failed",
					RV_TRACE_LEVEL_ERROR);
		USB_EP_NUM_SEL_TX_EP(phys_ep_nr);
		W2FC_CTRL = W2FC_CTRL_SET_HALT;
		USB_EP_NUM_TX_RELEASE(phys_ep_nr );
	}
	// Write EP_NUM register: //
	// EP_NUM.EP_Num = endp_nb //
	// EP_NUM.EP_Dir = 1       //
	// EP_NUM.EP_Sel = 0       //
	// EP_NUM.Setup_Sel = 0    //
	USB_EP_NUM_TX_RELEASE(phys_ep_nr );
	wait_ARM_cycles(convert_nanosec_to_cycles(2000));  /* 6 wait state so 134ns if 52Mhz clock*/
	F_INTH_ENABLE_ONE_IT(C_INTH_USB_IT);
	return(ret);
#else
	for(i = 0; i < DMA_TX_CHANNELS; i++)
	{
		if( (USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].interface_id == if_nr)
			&& (USB_DMA_CFG.dma_usb_endpoint_tx_tbl[i].endpoint_id == log_ep_nr) )
		{
			found = TRUE;
		}
	}

	//find physical ep
	endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl +
							(LEP_2_PEP(log_ep_nr, if_nr, USB_TX));

	/* Check if selected endpoint is managed by a DMA cannel */
	if((found == TRUE) && (endpoint_vs_interface_entry_p->buf_size >= DMA_TX_THRESHOLD))
	{
		USB_EP_NUM_TX_RELEASE(phys_ep_nr );

		ret = usb_prepare_dma_tx_transfer(if_nr, log_ep_nr);
	}
	else
	{
		// TX Handler //
		if(write_non_iso_packet(phys_ep_idx) != USB_OK) {
			ret = USB_INTERNAL_ERR;
			USB_SEND_TRACE("USB: write_non_iso_packet failed",
						RV_TRACE_LEVEL_ERROR);
			USB_EP_NUM_SEL_TX_EP(phys_ep_nr);
			W2FC_CTRL = W2FC_CTRL_SET_HALT;
			USB_EP_NUM_TX_RELEASE(phys_ep_nr );
		}
		// Write EP_NUM register: //
		// EP_NUM.EP_Num = endp_nb //
		// EP_NUM.EP_Dir = 1       //
		// EP_NUM.EP_Sel = 0       //
		// EP_NUM.Setup_Sel = 0    //
		USB_EP_NUM_TX_RELEASE(phys_ep_nr );
	}
	wait_ARM_cycles(convert_nanosec_to_cycles(2000));  /* 6 wait state so 134ns if 52Mhz clock*/
	F_INTH_ENABLE_ONE_IT(C_INTH_USB_IT);
	return(ret);
#endif
}

/****************************************************************************************
 |	INTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/

/**
 *--------------------------------------------------------------------------------*
 * @name name prepare_first_tx_trsfr
 *
 * This function handles setup interrupts initiated by the USB HOST
 *  
 *  @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void usb_setup_hndlr(void)
{
	UINT16 int_src_reg_backup;
	UINT8 usb_bmReqType;

	usb_env_ctrl_blk_p->host_data.ep0_setup.rx_or_tx_ep0_int = USB_SETUP_EP0_INT;

	/*cancel any ongoing transfer*/
	/*store previous interrupt enable register value*/
	int_src_reg_backup = W2FC_IRQ_EN;

	//application specific action to clear any ongoing transfer
	clear_reset_tx_ep(0);
	clear_reset_rx_ep(0);
	
	/*clear ep transactions and clear control flags*/
	//THIS IS A BUG see PR 190
	//	clr_ep_flgs();
	clr_setup_mem();
	
	/*parse command*/
	parse_setup_cmd(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p);

	//determine request type
	usb_bmReqType = ((USB_REQUEST_TYPE) >> 5) & 0x3;

	/*check if request is legal and supported*/
	if(((((USB_REQUEST != RESERVED) && (USB_REQUEST != RESERVED1)) && (USB_REQUEST <= SYNCH_FRAME)) && 
		(usb_bmReqType  == USB_STANDARD))	||  
		(usb_bmReqType == USB_VENDOR)		||
		(usb_bmReqType == USB_CLASS)		)
	{
		/*we have a valid request*/
		USB_SEND_TRACE("USB: legal HOST request", 
			RV_TRACE_LEVEL_DEBUG_LOW);
		/*determine request direction*/
		if((USB_REQUEST_TYPE & USB_CNTRL_DIR) == USB_CNTRL_DIR) {
			/*this is a device to host transfer (IN)*/
			USB_SEND_TRACE("USB: device to host transfer", 
				RV_TRACE_LEVEL_DEBUG_LOW);
			/*set control read flag*/
			USB_SETUP_IN = TRUE; 
			
			/* Current request is standard */
			if(usb_bmReqType  == USB_STANDARD)
			{
				//write ep_num reg:
				//EP_NUM.EP_NUM = 0
				//EP_NUM.EPDIR  = 1
				//EP_NUM.EP_SEL = 1
				//EP_NUM.EPSETUP_SEL = 0
				USB_EP_NUM_SEL_TX_EP(0);

				/*determine which request and take action*/
				if(det_req_spec_action() != USB_OK)  				{
					/*didn't succeed in responding as required to request*/
					USB_SEND_TRACE("USB: not responded to request as required", 
						RV_TRACE_LEVEL_DEBUG_LOW);
				}
				/*set fifo enable*/
				W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN; 

				/*finished writing in fifo so CLEAR this bit*/
				//write ep_num reg:
				//EP_NUM.EP_NUM = 0
				//EP_NUM.EPDIR  = 1
				//EP_NUM.EP_SEL = 0
				//EP_NUM.EPSETUP_SEL = 0
				USB_EP_NUM_TX_RELEASE(0);
			}
			else /* process a vendor or class request */
			{
				specific_ep0_setup_device_to_host_handler();
			}

		} else {
			/*this is a host to device transfer (OUT)*/
			USB_SEND_TRACE("USB: host to device transfer", 
				RV_TRACE_LEVEL_DEBUG_LOW);
			/*set control write flag*/
			USB_SETUP_OUT = TRUE; 
			
			/*check if there is data to be recieved*/
			if(USB_REQUEST_LENGTH > 0) {
				USB_SEND_TRACE("USB: setup request with additional data", 
					RV_TRACE_LEVEL_DEBUG_LOW);
				calc_no_trsf();
				/*check if driver is ready to recieve data*/
				if(get_setup_data_mem(&usb_env_ctrl_blk_p->host_data.ep0_data_p,
					USB_REQUEST_LENGTH) == USB_OK) {
					/*we reserved sufficient memory to store setup data in*/
					USB_SEND_TRACE("USB: memory to store data in is available", 
						RV_TRACE_LEVEL_DEBUG_LOW);
					
					// Write EP_NUM register: //
					// EP_NUM.EP_Num = 0      //
					// EP_NUM.EP_Dir = 0      //
					// EP_NUM.EP_Sel = 0      //
					// EP_NUM.Setup_Sel = 0   //
					USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);

					// Set CTRL.Set_FIFO_En to '1' //
					USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_SET_FIFO_EN);
				} else {
					/*there is no memory available to store setup data in*/
					USB_SEND_TRACE("USB: memory to store data in is NOT available, send NAK", 
						RV_TRACE_LEVEL_ERROR);
					/*we don't want to wait untill memory is available, just quit*/
					/*enable NAK interrupt*/
					W2FC_SYSCON1 |= W2FC_SYSCON1_NAK_EN;
				}
			} else {
				/*no data belongs with this setup request*/
				USB_SEND_TRACE("USB: setup request without additional data",
					RV_TRACE_LEVEL_DEBUG_LOW);
				//RX CONTROL transaction completion
				/* Current request is standard */
				if(usb_bmReqType  == USB_STANDARD)
				{
					/* prepare status request*/
					prepare_for_cntrl_write_stat_stage(USB_DEFAULT_EP, USB_ACK);
				}
				else
				{
					/* Notify to FMs,  reception of a buffer */
					/* Status request is made by FM */
					specific_ep0_rx_handler();
				}
			}
		}
	} else {
		/*request is not valid*/
		USB_SEND_TRACE("USB: Illegal Host request", 
		RV_TRACE_LEVEL_DEBUG_LOW);
		
		/*make sure next EP0 command is stalled*/
		W2FC_SYSCON2 |= W2FC_SYSCON2_STALL_CMD;
	}
	W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_SETUP);
	//Enable all interrupts again
	/*enable EPn_RX_IE*/
	W2FC_IRQ_EN |= W2FC_IRQ_EN_EPN_RX_IE;
	/*enable EPn_TX_IE*/
	W2FC_IRQ_EN |= W2FC_IRQ_EN_EPN_TX_IE;
	/*enable state changed interrupts*/
	W2FC_IRQ_EN |= W2FC_IRQ_EN_DS_CHG_IE;
	/*enable endpoint 0 interrupts*/
	W2FC_IRQ_EN |= W2FC_IRQ_EN_EP0_IE;
	// If Start Of Frame (SOF) interrupt is needed (isochronous mode),
        // then uncomment the next line :
//    W2FC_IRQ_EN |= W2FC_IRQ_EN_SOF_IE;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name parse_setup_cmd
 *
 * This function parses the request send by the USB HOST
 *
 * It reads only the request, not the data that comes with the 
 * specific request
 *
 * @param		request_p:		pointer to a request data type
 *								here the request will be stored in
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void parse_setup_cmd(T_USB_DEV_REQ* request_p)
{	
    UINT16 data_reg;	//holds the data read from the setup fifo before it is
						//copied to final destination
	/*parse fifo*/
	while((W2FC_IRQ_SRC & W2FC_IRQ_SRC_SETUP) == W2FC_IRQ_SRC_SETUP) 
	{
		/*here the irq_src.setup_bit will be cleared*/
			/*clear the interrupt source setup bit*/
		USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);

		/*select setup fifo this will clr int*/
		USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_EP_NUM_SETUP_SEL);
		
		/*read first request bytes from FIFO*/
		data_reg = W2FC_DATA;

		/*USB always does LSB before MSB*/
		request_p->bmRequestType = data_reg;	//LSB of FIFO contains request type
		request_p->bRequest = (T_USB_STANDARD_REQ) (data_reg>>8);	//MSB of FIFO contains request
		
		/*read next from FIFO*/
		data_reg = W2FC_DATA;
		
		/*assign FIFO value to wValue*/
		request_p->wValue = data_reg;

		/*read next from FIFO*/
		data_reg = W2FC_DATA;

		/*assign new fifo value to wIndex*/
		request_p->wIndex = data_reg;

		/*read next from FIFO*/
		data_reg = W2FC_DATA;

		/*assign new fifo value to wLength*/
		request_p->wLength = data_reg;

		/*unselect setup*/
		USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);
	}
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name name prepare_first_ep0_trsfr
 *
 * This function is called from the API function usb_set_tx_buffer with endpoint 0.
 * It stores interface ID in a field of the current setup request
 * vendor or class callback is initialized
 * 
 *
 *  @param if_nr:			interface number  *
 *  @return	T_RV_RET 		Returns always USB_OK
 *---------------------------------------------------------------------------------* 
 */
T_RV_RET prepare_first_ep0_trsfr(UINT8 interface_id)
{
	T_RV_RET ret = USB_OK;

	/* store interface ID in current setup structure */
	usb_env_ctrl_blk_p->host_data.ep0_setup.if_nbr = interface_id;

	/* Initialize TX callback */
	specific_ep0_tx_handler();

	return(ret);
}
/*@}*/


/**
 *--------------------------------------------------------------------------------*
 * @name name configure_ep0_interface
 *
 * This function is called from the API function usb_set_rx_buffer with endpoint 0.
 * If the previous request was host to device, its return to the host a IN packet
 * 
 *  @param if_nr:			interface number 
 *
 *  @return	T_RV_RET 		Returns always USB_OK
 *---------------------------------------------------------------------------------* 
 */
T_RV_RET configure_ep0_interface(UINT8 interface_id)
{
	T_RV_RET ret = USB_OK;

	if(USB_SETUP_OUT == TRUE)
	{
		/* Returns status of the previous Host to Device SETUP command */
		send_empty_packet(USB_DEFAULT_EP);
	}

	if(USB_SETUP_IN == TRUE)
	{
		prepare_for_cntrl_read_stat_stage(USB_DEFAULT_EP, USB_ACK);	
	}

	/* store interface ID for the next request */
	usb_env_ctrl_blk_p->host_data.ep0_setup.if_nbr = interface_id;

	return(ret);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name name specific_tx_handler
 *
 * This function is only used in case of Class or Vendor device to host requests
 * Callback on device to host transfer on endpoint 0.
 * 
 *  @param no parameter
 *
 *  @return	T_RV_RET 		Returns USB_OK when successfully executed this function
 *							returns USB_INTERNAL_ERR when failed
 *---------------------------------------------------------------------------------* 
 */
static void specific_ep0_tx_handler()
{
	BOOL	process = FALSE;
	UINT16	new_written_in_fifo;
	UINT8*	temp_p;

	static UINT16 nr_bytes_in_buf;
	static UINT16 nr_bytes_placed_in_fifo;

#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	UINT8 i;
#endif
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
	UINT8 if_nr = usb_env_ctrl_blk_p->host_data.ep0_setup.if_nbr;

	// Check validity of the interface
	if(specific_ep0_check_interface_validity() == FALSE)
	{
		//device behaviour not specified
		USB_SEND_TRACE("USB:  Interface is not valid", RV_TRACE_LEVEL_ERROR);
		abort_setup_req(WITH_CMD_STALL);
		process = FALSE;

		return;
	}

#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	for(i=0;i<USB_TOTAL_DIFF_IF;i++)
	{
		if(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_number == if_nr
			&& usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_enabled == TRUE)
			break;
	}
#endif
	if(USB_REQUEST_LENGTH == 0)
	{	// No need to send FM buffer
		USB_SEND_TRACE("USB: No need to send FM buffer", RV_TRACE_LEVEL_ERROR);
		notify_ep0_tx_buffer_empty(if_nr);
		return;
	}

	usb_env_ctrl_blk_p->host_data.ep0_data_p = NULL;

	endpoint_vs_interface_entry_p = 
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
				&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i]);
#else
				&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[if_nr]);
#endif

	if(usb_env_ctrl_blk_p->host_data.ep0_setup.rx_or_tx_ep0_int == USB_TX_EP0_INT) 
	{
		//for this request more IN transactions can occur so check it out
		USB_SEND_TRACE("USB: class standard request DATA stage is entered", 
			RV_TRACE_LEVEL_DEBUG_LOW);
		
		if(usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc >=
			usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req) 
		{
			//this is a legal last Data transaction stage. This is expected behaviour
			usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc++;
			
			//IN transaction on EP0 was ACKed 
			//see figure 17 pg 104 of usb w2fc rev 1.10
			//here wcount <=0 and no more data to be send
			// EP_NUM EP_Num = 0
			// EP_NUM EP_Dir = 1
			// EP_NUM EP_Sel = 0
			// EP_NUM EP_Setup_Sel = 0
			USB_EP_NUM_TX_RELEASE(0);
			//TX CONTROL transaction completion
			USB_SEND_TRACE("USB: FM buffer is sent", RV_TRACE_LEVEL_DEBUG_LOW);
			// prepare_for_cntrl_read_stat_stage(USB_DEFAULT_EP, USB_ACK);	
			notify_ep0_tx_buffer_empty(if_nr);
			process = FALSE;
		} 
		else 
		{
			USB_SEND_TRACE("USB: next data IN transaction for specific_ep0_tx_handler ", 
				RV_TRACE_LEVEL_DEBUG_LOW);
			// write non-iso TX FIFO data
			temp_p = endpoint_vs_interface_entry_p->buf_p;
			temp_p += nr_bytes_placed_in_fifo;
				write_non_iso_tx_fifo_data((nr_bytes_in_buf - nr_bytes_placed_in_fifo), 
										USB_ENDP0_SIZE, 
										&new_written_in_fifo, 
										temp_p);

			nr_bytes_placed_in_fifo += new_written_in_fifo;
			if((nr_bytes_in_buf - nr_bytes_placed_in_fifo == 0) ||
				(new_written_in_fifo == USB_ENDP0_SIZE)) 
			{
				// succesfully wrote data in fifo
				// write finished
				USB_SEND_TRACE("USB: apparently successfully written in fifo", 
					RV_TRACE_LEVEL_DEBUG_LOW);
			}
			
			//fifo enable
			W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
			
			// EP_NUM EP_Num = 0
			// EP_NUM EP_Dir = 1
			// EP_NUM EP_Sel = 0
			// EP_NUM EP_Setup_Sel = 0
			USB_EP_NUM_TX_RELEASE(0);
			process = FALSE;
		}
	} 
	else 
	{
		//this is the handling of the setup request 
		USB_SEND_TRACE("USB: class or vendor request setup stage is entered", 
			RV_TRACE_LEVEL_DEBUG_LOW);
		// store request that is currently being processed
		store_specific_ep0_handler(USB_CDC_EP0_TX); //no dummy
		//calculate nr of transactions required
		if((USB_REQUEST_LENGTH / USB_ENDP0_SIZE) < 1) 
		{
			//nr off processed tranfers is still 0 here
			usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc = 0;
			
			//save this transaction only 1 more IN transaction is needed
			usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req = 1;
		} 
		else 
		{
			calc_no_trsf();
		}

		//check if conditions for this request are right 
		if(((W2FC_DEVSTAT & W2FC_DEVSTAT_DEF) == W2FC_DEVSTAT_DEF) ||
			((W2FC_DEVSTAT & W2FC_DEVSTAT_ADD) == W2FC_DEVSTAT_ADD) ||
			((W2FC_DEVSTAT & W2FC_DEVSTAT_CFG) == W2FC_DEVSTAT_CFG)) 
		{
			USB_SEND_TRACE("USB: first data IN transaction for specific_ep0_tx_handler ", 
				RV_TRACE_LEVEL_DEBUG_LOW);
			process = TRUE;
		} else 
		{
			//device behaviour not specified
			USB_SEND_TRACE("USB:  request while hw state is undefined, behaviour not specified", 
				RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
			process = FALSE;
		}
	}

	if(process == TRUE) 
	{
		USB_EP_NUM_SEL_TX_EP(0);
		nr_bytes_in_buf	= endpoint_vs_interface_entry_p->buf_size;

		// write non-iso TX FIFO data
		write_non_iso_tx_fifo_data(	nr_bytes_in_buf, 
									USB_ENDP0_SIZE, 
									&nr_bytes_placed_in_fifo, 
									endpoint_vs_interface_entry_p->buf_p);

		USB_TRACE_WARNING_PARAM("USB: nr_bytes_in_buf :", nr_bytes_in_buf);
		USB_TRACE_WARNING_PARAM("USB: nr_bytes_placed_in_fifo:", nr_bytes_placed_in_fifo);

		//fifo enable
		W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
		// EP_NUM EP_Num = 0
		// EP_NUM EP_Dir = 1
		// EP_NUM EP_Sel = 0
		// EP_NUM EP_Setup_Sel = 0
		USB_EP_NUM_TX_RELEASE(0);

		if((nr_bytes_placed_in_fifo - nr_bytes_in_buf == 0) || 
							(nr_bytes_placed_in_fifo == USB_ENDP0_SIZE)) 
		{
			USB_SEND_TRACE("USB: apparently successfully written in fifo", 
				RV_TRACE_LEVEL_DEBUG_LOW);
			// succesfully wrote data in fifo
			// write finished
		} 
		else 
		{
			// data not succesfully written into Fifo
			USB_SEND_TRACE("USB: data not sucessfully written into fifo", 
				RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
		}
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name get_setup_data_mem
 *
 * This function reserves memory for the data that will be send 
 * to or from the device.
 *
 * @param		setup_pp:		pointer to a memory that will hold 
 *								the setup data
 * @param		size:			holds the size of the setup data buffer
 *
 * @return	USB_OK, RVM_MEMORY_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static T_RV_RET get_setup_data_mem(UINT8** setup_pp, UINT16 size)
{
	/* Memory bank status (red, yellow, green). */
	T_RVF_MB_STATUS mb_status;
	T_RV_RET ret = USB_OK;

	/* Create buffer to contain the endpoint 0 setup request additional data*/
	mb_status = rvf_get_buf(usb_env_ctrl_blk_p->prim_mb_id, 
								(sizeof(UINT8) * size),
								((T_RVF_BUFFER**) setup_pp));
	if (mb_status == RVF_RED) {
		/*
		 * The flag returned by rvf_get_buf is red, there is not enough
		 * memory to allocate the buffer.
		 * The environemnt will cancel the USB instance creation.
		 */
		USB_SEND_TRACE("USB: Error to get memory ",RV_TRACE_LEVEL_ERROR);
		ret = RVM_MEMORY_ERR;
	}
	else if (mb_status == RVF_YELLOW) {
		/*
		 * The flag is yellow, there will soon be not enough memory anymore.
		 */
		USB_SEND_TRACE("USB: Getting short on memory ", RV_TRACE_LEVEL_WARNING);
	}
	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name clr_setup_mem
 *
 * This function reserves memory for the setup request
 * and clears the setup control flags
 *
 * @return	USB_OK, RVM_MEMORY_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void clr_setup_mem(void)
{
	/* Memory bank status (red, yellow, green). */
	T_RVF_MB_STATUS mb_status;
	T_RV_RET ret = USB_OK;

	/*if the control flags are both false there is no setup request buffer yet*/
	if((USB_SETUP_OUT == TRUE) || (USB_SETUP_IN == TRUE)) {
		/*there was already a setup transaction, so delete the request buffer 
		before assigning a new one*/
		/*set control flags to FALSE this has for effect that the previous setup 
		action is interrupted and canceled*/
		USB_SETUP_OUT = FALSE;
		USB_SETUP_IN = FALSE;
	}
	clear_setup_buf();
}
/*@}*/

/**
 *------------------------------------------------------------------------------------------* 
 * @name write_non_iso_tx_fifo_data
 *
 * This function actually writes the data provided in the (FM /driver)'s buffer
 * in the fifo of the preselected endpoint.
 *
 * The caller of this function is responsible for selecting the appropriate endpoint
 * before calling this function. And to provide this function with the actual 
 * endpoints fifo size
 *
 * @param	nr_bytes_in_app_tx_buf: number of bytes that are waiting in the application
 *									buffer (FM / driver) to be written in the endpoints 
 *									fifo. This can be more as the endpoints fifo size
 * @param	ep_fifo_size:			size of the selected endpoints fifo	
 * @param	nr_bytes_place_in_fifo:	Pointer to variable containing the bytes actually 
 *									written in the fifo. This can be used by the application 
 *									to recalculate the buffer pointer and the size of data
 *									still to be written.
 * @param	buffer_p				pointer to the applications buffer
 * @return	USB_OK, RVM_MEMORY_ERR
 *-------------------------------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET write_non_iso_tx_fifo_data(UINT16 nr_bytes_in_app_tx_buf, UINT16 ep_fifo_size, 
									   UINT16* nr_bytes_place_in_fifo, UINT8* buffer_p)
{
	UINT16 loop_cntr;	
	UINT16 temp_fifo;
	T_RV_RET ret = USB_OK;
	
	/*check if nr_of_bytes to write is large as fifo*/
	if(nr_bytes_in_app_tx_buf > ep_fifo_size) {
		/*it is larger, set loop cntr to fifo size*/
		loop_cntr = ep_fifo_size;
	} else {
		/*it is smaller or just as large as fifo*/
		loop_cntr = nr_bytes_in_app_tx_buf;
	}
	/*nr of bytes to written in fifo*/
	*nr_bytes_place_in_fifo = loop_cntr;
	
	/*start filling fifo*/
	while(loop_cntr > 0) {
		/*check if we have to copy 16 bits or 8 bits*/
		if(loop_cntr < 2) {
			/*number of bytes to write is odd, for the last byte we must change to 8bits mode*/
			W2FC_DATA8 = *buffer_p;
			
			/*increment buffer pointer so we won't be reading same data twice*/
			buffer_p++;
			/* decrement loop counter */
			loop_cntr--;
			USB_SEND_TRACE("USB: odd buffer has completed writting into fifo", 
				RV_TRACE_LEVEL_DEBUG_LOW);
		} else {
			/*we can use the 16 bits acces to the fifo*/
			/*because LSB is always written first in fifo*/

			//buffer 
			/**********/
			/* byte 1 */
			/**********/
			/**********/
			/* byte 2 */
			/**********/
			/**********/
			/* byte 3 */
			/**********/
			/**********/
			/* byte 4 */
			/**********/

			//16bits fifo 1e transaction
			/**********//**********/
			/* byte 1 *//* byte 2 */ 
			/**********//**********/
			
			//16 bits fifo 2e transaction
			/**********//**********/
			/* byte 3 *//* byte 4 */ 
			/**********//**********/
			temp_fifo = *buffer_p;
			buffer_p++;

			temp_fifo |= ((UINT16)*buffer_p << 8);
			buffer_p++;

			/*fill 16 bits fifo*/
			W2FC_DATA = temp_fifo;
			
			/*decrement loop counter for 16 bits*/
			loop_cntr -= 2;
		}
	}
	return(ret);
}
/*@}*/

/**
 *------------------------------------------------------------------------------------------* 
 * @name read_non_iso_rx_fifo_data
 *
 * This function actually reads the data from the selected endpoint (pep_nr) which is placed
 * in there by the USB HOST
 *
 * The caller of this function is responsible for selecting the appropriate endpoint in the 
 * HArdware before calling this function. 
 *
 * @param	pep_nr:					physical endpoint number
 * @param	nr_bytes_from_fifo:		Pointer to variable containing the bytes actually 
 *									read from the fifo. This can be used by the application 
 *									to recalculate the buffer pointer and the size of data
 *									still to be read.
 * @param	buffer_p				pointer to the applications buffer
 * @return	USB_OK, RVM_MEMORY_ERR
 *-------------------------------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET read_non_iso_rx_fifo_data(UINT16 pep_nr, UINT16* nr_bytes_from_fifo, UINT8* buffer_p)
{
	UINT16 loop_cntr = 0;	
	UINT16 temp_fifo;
	UINT16 ep_buf_size;
	T_RV_RET ret = USB_OK;
	
	if((W2FC_STAT_FLG & W2FC_STAT_FLG_NON_ISO_FIFO_EMPTY) == 
		W2FC_STAT_FLG_NON_ISO_FIFO_EMPTY) {
		if(pep_nr == 0) {
			//endpoint 0 has NO double buffering
			usb_env_ctrl_blk_p->host_data.fifo_not_full = FALSE;
		} else {
			if(det_fifo_full_db(pep_nr, &ep_buf_size, USB_RX)) {
				usb_env_ctrl_blk_p->host_data.fifo_not_full = TRUE;
			} else {
				usb_env_ctrl_blk_p->host_data.fifo_not_full = FALSE;
			}
		}
	} else {
		if((W2FC_STAT_FLG & W2FC_STAT_FLG_NON_ISO_FIFO_FULL) == 
			W2FC_STAT_FLG_NON_ISO_FIFO_FULL) {
			//RXcounter -> buffersize
			if(pep_nr == 0) {
				loop_cntr = USB_ENDP0_SIZE;
			} else {
				det_fifo_full_db(pep_nr, &ep_buf_size, USB_RX);
				loop_cntr = ep_buf_size;
			}
		} else {
			//RXcounter -> RXFSTAT.RXF_count
			loop_cntr = W2FC_RXFSTAT;
			if(pep_nr != 0) {
				if(det_fifo_full_db(pep_nr, &ep_buf_size, USB_RX)) {
					usb_env_ctrl_blk_p->host_data.fifo_not_full = TRUE;
				} else {
					usb_env_ctrl_blk_p->host_data.fifo_not_full = FALSE;
				}
			} else {
				//endpoint 0 has NO double buffering
				usb_env_ctrl_blk_p->host_data.fifo_not_full = FALSE;
			}
		}
	}
	//this can differ from the calculated transfer size
	*nr_bytes_from_fifo = loop_cntr;
	
	/*start reading fifo*/
	while(loop_cntr > 0) {
		/*check if we have to copy 16 bits or 8 bits*/
		if(loop_cntr < 2) {
			//number of bytes to write is odd, 
			//for the last byte we must change to 8bits mode
			*buffer_p = W2FC_DATA8;

			/*increment buffer pointer so we won't be reading same data twice*/
			buffer_p++;
			/* decrement loop counter */
			loop_cntr--;
		} else {
			//read 16 bits fifo
			*(UINT16*)buffer_p = W2FC_DATA;
			buffer_p+=2;
			
			//decrement loop counter for 16 bits
			loop_cntr -= 2;
		}
	}
	return(ret);
}

/**
 *-----------------------------------------------------------------------*
 * @name det_req_spec_action
 *
 * This function finds out which request is done by the 
 * USB host and will take action accordingly.
 *
 * @return	USB_OK, RVM_MEMORY_ERR
 *-----------------------------------------------------------------------*
 */
/*@{*/
static T_RV_RET det_req_spec_action(void)
{
	T_RV_RET ret = USB_OK;
	UINT8 usb_bmReqType;

	usb_bmReqType = ((USB_REQUEST_TYPE) >> 5) & 0x3;
	
	switch (usb_bmReqType) {
		case USB_STANDARD:
			//execute requested action
			setup_stnd_fnc_table[USB_REQUEST]();	
			break;
		
		case USB_CLASS:
		case USB_VENDOR:
			if((USB_REQUEST_TYPE & USB_CNTRL_DIR) == USB_CNTRL_DIR)
			{	/* Host to device request */
				USB_SEND_TRACE("USB: Host to device request", RV_TRACE_LEVEL_DEBUG_LOW);
				specific_ep0_setup_device_to_host_handler();
			}
			else if((USB_REQUEST_TYPE & USB_CNTRL_DIR)  == 0)
			{	/* Device to host request */
				USB_SEND_TRACE("USB: Device to host request", RV_TRACE_LEVEL_DEBUG_LOW);
				specific_ep0_rx_handler();
			}
			else
			{
				USB_SEND_TRACE("USB: Nothing to do", RV_TRACE_LEVEL_ERROR);
			}
			break;
		case USB_RESERVED:
			break;
		default :
			break;
	}
	return(ret);
}
/*@}*/

/**
 *-----------------------------------------------------------------------------------*
 * @name get_descriptor
 *
 * This function request returns the descriptor of the specified descriptor type
 * This can be a configuration descriptor a device descriptor and a string descriptor
 *
 * THIS function will be called at least 2 times, on a Setup int and on an IN int
 *
 *-----------------------------------------------------------------------------------*
 */
/*@{*/

static void get_descriptor(void)
{
	T_RV_RET ret = USB_OK;
	BOOL process = FALSE;
	BOOL send_str_descr = FALSE;
	UINT8 descr_index;
	T_USB_DESCR_TYPE descr_type;
	T_RV_RET result;
	UINT8 i;
	UINT16 new_written_in_fifo;
	UINT8* temp_p;

	UINT32 product_id = 0;

	static UINT16 nr_bytes_in_buf;
	static UINT16 nr_bytes_placed_in_fifo;

	if(usb_env_ctrl_blk_p->host_data.ep0_setup.rx_or_tx_ep0_int == USB_TX_EP0_INT) {
		//for this request more IN transactions can occur so check it out
		USB_SEND_TRACE("USB: get_descriptor standard request DATA stage is entered", 
			RV_TRACE_LEVEL_DEBUG_LOW);
		if(usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc >=
			usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req) {
			//this is a legal last Data transaction stage. This is expected behaviour
			usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc++;
			
			//IN transaction on EP0 was ACKed 
			//see figure 17 pg 104 of usb w2fc rev 1.10
			//here wcount <=0 and no more data to be send
			// EP_NUM EP_Num = 0
			// EP_NUM EP_Dir = 1
			// EP_NUM EP_Sel = 0
			// EP_NUM EP_Setup_Sel = 0
			USB_EP_NUM_TX_RELEASE(0);
			//TX CONTROL transaction completion
			prepare_for_cntrl_read_stat_stage(USB_DEFAULT_EP, USB_ACK);		
				
			process = FALSE;
		} else {
			USB_SEND_TRACE("USB: next data IN transaction for get_descriptor ", 
				RV_TRACE_LEVEL_DEBUG_LOW);
			/*write non-iso TX FIFO data*/
			temp_p = usb_env_ctrl_blk_p->host_data.ep0_data_p;
			temp_p += nr_bytes_placed_in_fifo;
				write_non_iso_tx_fifo_data((nr_bytes_in_buf - nr_bytes_placed_in_fifo), 
										USB_ENDP0_SIZE, 
										&new_written_in_fifo, 
										temp_p);
			nr_bytes_placed_in_fifo += new_written_in_fifo;
			if((nr_bytes_in_buf - nr_bytes_placed_in_fifo == 0) ||
				(new_written_in_fifo == USB_ENDP0_SIZE)) {
				/*succesfully wrote data in fifo*/
				/*write finished*/
				USB_SEND_TRACE("USB: apparently successfully written in fifo", 
					RV_TRACE_LEVEL_DEBUG_LOW);
			}
						//fifo enable
			W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
			
			// EP_NUM EP_Num = 0
			// EP_NUM EP_Dir = 1
			// EP_NUM EP_Sel = 0
			// EP_NUM EP_Setup_Sel = 0
			USB_EP_NUM_TX_RELEASE(0);
			process = FALSE;
		}
	} else {
		//this is the handling of the setup request 
		USB_SEND_TRACE("USB: get_descriptor standard request setup stage is entered", 
			RV_TRACE_LEVEL_DEBUG_LOW);
		/*store request that is currently being processed*/
		store_req(FALSE); //no dummy
			//calculate nr of transactions required
		if((USB_REQUEST_LENGTH / USB_ENDP0_SIZE) <= 1) {
			//nr off processed tranfers is still 0 here
			usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc = 0;
			
			//save this transaction only 1 more IN transaction is needed
			usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req = 1;
		} else {
			calc_no_trsf();
		}
		//check if conditions for this request are right 
		if(((W2FC_DEVSTAT & W2FC_DEVSTAT_DEF) == W2FC_DEVSTAT_DEF) ||
			((W2FC_DEVSTAT & W2FC_DEVSTAT_ADD) == W2FC_DEVSTAT_ADD) ||
			((W2FC_DEVSTAT & W2FC_DEVSTAT_CFG) == W2FC_DEVSTAT_CFG)) {
			USB_SEND_TRACE("USB: first data IN transaction for get_descriptor ", 
				RV_TRACE_LEVEL_DEBUG_LOW);
			process = TRUE;
		} else {
			//device behaviour not specified
			USB_SEND_TRACE("USB:  request while hw state is undefined, behaviour not specified", 
				RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
			process = FALSE;
		}
	}	
	if(process == TRUE) {
		//request was valid so process the request
		//we don't have to create data buffers for this request 
		descr_index = usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wValue;
		descr_type = (T_USB_DESCR_TYPE)(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wValue >> 8);
		
		if(descr_type == USB_DEVICE) {
			//for get device descriptor wIndex should be Zero
			if(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wIndex == 0) {
				//check if the specified length is sufficient to store device descriptor in
				if(usb_device_descr.bLength <= 
					USB_REQUEST_LENGTH) {
					//this should fit
					//create buffer of required size 
					if(get_setup_data_mem(&usb_env_ctrl_blk_p->host_data.ep0_data_p, 
						usb_device_descr.bLength) == USB_OK) {
						/*fill it with data*/
						memcpy(usb_env_ctrl_blk_p->host_data.ep0_data_p, &usb_device_descr, 
								usb_device_descr.bLength); 
						#ifdef USB_DYNAMIC_CONFIG_SUPPORT
						/* Generate Different Product Id for each configuration */
					
						*((UINT16*)(usb_env_ctrl_blk_p->host_data.ep0_data_p +  (UINT32)&(((T_USB_DEVICE_DESCR*)0)->idProduct)))
							+=usb_env_ctrl_blk_p->enumInfo.info; 
												
						#endif
					
						/*place it in fifo*/
						nr_bytes_in_buf = usb_device_descr.bLength;
						
						/*write non-iso TX FIFO data*/
						write_non_iso_tx_fifo_data(nr_bytes_in_buf, 
													USB_ENDP0_SIZE, 
													&nr_bytes_placed_in_fifo, 
													usb_env_ctrl_blk_p->host_data.ep0_data_p);
						if((nr_bytes_in_buf - nr_bytes_placed_in_fifo == 0) || 
							(nr_bytes_placed_in_fifo == USB_ENDP0_SIZE)) {
							/*succesfully wrote data in fifo*/
							/*write finished*/
							USB_SEND_TRACE("USB: apparently successfully written in fifo", 
								RV_TRACE_LEVEL_DEBUG_LOW);
						} else {
							/*data not succesfully written into Fifo*/
							USB_SEND_TRACE("USB: data not sucessfully written into fifo", 
								RV_TRACE_LEVEL_ERROR);
							abort_setup_req(WITH_CMD_STALL);
						}
					} else {
						/*there is no memory available to store setup data in*/
						USB_SEND_TRACE("USB: memory NOT available, send STALL CMD", 
							RV_TRACE_LEVEL_ERROR);
						/*we don't want to wait untill memory is available, just quit*/
						abort_setup_req(WITH_CMD_STALL);
					}
				} else {
					//hey we need precice data length
					USB_SEND_TRACE("USB: get_descriptor size mismatch",
						RV_TRACE_LEVEL_ERROR);
					abort_setup_req(WITH_CMD_STALL);
				}
			} else {
				//hey we need a legal request
				USB_SEND_TRACE("USB:  wIndex not 0, behaviour not specified", 
					RV_TRACE_LEVEL_ERROR);
				abort_setup_req(WITH_CMD_STALL);
			}
		}//descr_type == USB_DEVICE)
		else {
			//check if it is a get configuration descriptor request
			if(descr_type == USB_CONFIGURATION) {
				//check if configuration really exists
				if(descr_index < USB_DEV_NUM_CFG) {
					//check get configuration descriptor wIndex should be Zero
					if(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wIndex == 0) {
						//ALL descriptors of the specified configuration
						if(send_conf_descr(&nr_bytes_in_buf, descr_index, 
							&nr_bytes_placed_in_fifo ) 
							!= USB_OK) {
							//not succeeded in sending configuration descriptor
							USB_SEND_TRACE("USB: get_descriptor all configuration failed", 
								RV_TRACE_LEVEL_ERROR);
							abort_setup_req(WITH_CMD_STALL);
						} else {
							USB_SEND_TRACE("USB: send get_descriptor all succeeded", 
								RV_TRACE_LEVEL_DEBUG_LOW);
						}
					} else {
						//hey we need a legal request
						USB_SEND_TRACE("USB: wIndex not 0,", 
							RV_TRACE_LEVEL_ERROR);
						abort_setup_req(WITH_CMD_STALL);
					}
				} else {
					//hey we need a legal configuration
					USB_SEND_TRACE("USB: configuration doesn't exist", 
						RV_TRACE_LEVEL_ERROR);
					abort_setup_req(WITH_CMD_STALL);
				}
			}//(descr_type == USB_CONFIGURATION)
			else {
				if(descr_type == USB_STRING) {
					USB_SEND_TRACE("USB: string descr not build yet", 
						RV_TRACE_LEVEL_ERROR);
//					abort_setup_req(WITH_CMD_STALL);
// PROCESS STRING DESC REQ:
					//create buffer of required size 
				if(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wIndex == 0){
//				if(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wIndex != 0){
#if 0							
						/*fill it with data*/
						memcpy(usb_env_ctrl_blk_p->host_data.ep0_data_p, &string_descr0, 
								string_descr.bLength);
#endif
					if(get_setup_data_mem(&usb_env_ctrl_blk_p->host_data.ep0_data_p, 
						string_descr_lang_id.bLength) == USB_OK) {
//						*(UINT32*)usb_env_ctrl_blk_p->host_data.ep0_data_p = 0x04090304;
						/*place it in fifo*/
							if(USB_REQUEST_LENGTH == 0xFF){
								nr_bytes_in_buf = 0x4;
								USB_REQUEST_LENGTH = 0x4;
								calc_no_trsf();
							}else
							{
								nr_bytes_in_buf = USB_REQUEST_LENGTH;								
							}
						/*fill it with data*/
						memcpy(usb_env_ctrl_blk_p->host_data.ep0_data_p, &string_descr_lang_id, 
								string_descr_lang_id.bLength);
						send_str_descr = TRUE;
						}else
						{
							/*data not succesfully written into Fifo*/
							USB_SEND_TRACE("USB: mem allocation for lang_id failed", 
								RV_TRACE_LEVEL_ERROR);
							abort_setup_req(WITH_CMD_STALL);
							send_str_descr = FALSE;
						}
						
				}else
				{
					/* 
					   NOTE:
					     OAMPS00075755: String descriptor handling is implemented
					   for supporting multi-LUN. Multiple LUN support requires 
					   serial number to be sent to the host. As protocol Serial number is 
					   part of string descriptor. The logic implemented to
					   support string descriptor deals only with serial number.
					   Other string descr like Manufacturer, product etc., are not
					   handled.
					*/
#if 1
					if(descr_index==USB_MANUF_STR)
					{
						if(get_setup_data_mem(&usb_env_ctrl_blk_p->host_data.ep0_data_p,
							string_descr_manufacturer.bLength) == USB_OK) {

							/*place it in fifo*/
							if(USB_REQUEST_LENGTH == 0xFF){
								nr_bytes_in_buf = string_descr_manufacturer.bLength;
								USB_REQUEST_LENGTH = string_descr_manufacturer.bLength;
								calc_no_trsf();
							}else
							{
								nr_bytes_in_buf = USB_REQUEST_LENGTH;
							}
							/*fill it with data*/
							memcpy(usb_env_ctrl_blk_p->host_data.ep0_data_p, &string_descr_manufacturer,
								string_descr_manufacturer.bLength);
							send_str_descr = TRUE;
						}else
						{
							/*data not succesfully written into Fifo*/
							USB_SEND_TRACE("USB: mem allocation for manufacturer name failed",
								RV_TRACE_LEVEL_ERROR);
							abort_setup_req(WITH_CMD_STALL);
							send_str_descr = FALSE;
						}
					}
					else if(descr_index==USB_PROD_STR)
					{
						if(get_setup_data_mem(&usb_env_ctrl_blk_p->host_data.ep0_data_p,
							string_descr_product .bLength) == USB_OK) {

							/*place it in fifo*/
							if(USB_REQUEST_LENGTH == 0xFF){
								nr_bytes_in_buf = string_descr_product .bLength;
								USB_REQUEST_LENGTH = string_descr_product .bLength;
								calc_no_trsf();
							}else
							{
								nr_bytes_in_buf = USB_REQUEST_LENGTH;
							}
							/*fill it with data*/
							memcpy(usb_env_ctrl_blk_p->host_data.ep0_data_p, &string_descr_product ,
								string_descr_product .bLength);
							send_str_descr = TRUE;
						}else
						{
							/*data not succesfully written into Fifo*/
							USB_SEND_TRACE("USB: mem allocation for product name failed",
								RV_TRACE_LEVEL_ERROR);
							abort_setup_req(WITH_CMD_STALL);
							send_str_descr = FALSE;
						}
					}
					else if(descr_index==USB_SERIAL_STR)
#else
					if(descr_index != 0xEE)
#endif
					{
						if(get_setup_data_mem(&usb_env_ctrl_blk_p->host_data.ep0_data_p, 
							string_descr_ser_num.bLength) == USB_OK) {				

							/*place it in fifo*/
							if(USB_REQUEST_LENGTH == 0xFF){
								nr_bytes_in_buf = string_descr_ser_num.bLength;
								USB_REQUEST_LENGTH = string_descr_ser_num.bLength;
								calc_no_trsf();
							}else
							{
								nr_bytes_in_buf = USB_REQUEST_LENGTH;							
							}
							/*fill it with data*/
							memcpy(usb_env_ctrl_blk_p->host_data.ep0_data_p, &string_descr_ser_num, 
								string_descr_ser_num.bLength);
							send_str_descr = TRUE;
						}else
						{
							/*data not succesfully written into Fifo*/
							USB_SEND_TRACE("USB: mem allocation for serial number failed", 
								RV_TRACE_LEVEL_ERROR);
							abort_setup_req(WITH_CMD_STALL);					
							send_str_descr = FALSE;
						}
					}
					else
					{
						send_str_descr = FALSE;
					}
				}
				if(TRUE == send_str_descr){		
					/*write non-iso TX FIFO data*/
					write_non_iso_tx_fifo_data(nr_bytes_in_buf, 
												USB_ENDP0_SIZE, 
												&nr_bytes_placed_in_fifo, 
												usb_env_ctrl_blk_p->host_data.ep0_data_p);
					if((nr_bytes_in_buf - nr_bytes_placed_in_fifo == 0) || 
						(nr_bytes_placed_in_fifo == USB_ENDP0_SIZE)) {
						/*succesfully wrote data in fifo*/
						/*write finished*/
						USB_SEND_TRACE("USB: apparently successfully written in fifo", 
							RV_TRACE_LEVEL_DEBUG_LOW);
					} else {
						/*data not succesfully written into Fifo*/
						USB_SEND_TRACE("USB: data not sucessfully written into fifo", 
							RV_TRACE_LEVEL_ERROR);
						abort_setup_req(WITH_CMD_STALL);
					}
				}
// PROCESS STRING DESC REQ;
				}//(descr_type == USB_STRING)
				else {
					//hey we need a legal descriptor type
					USB_SEND_TRACE("USB: get_descriptor descriptor type not legal", 
						RV_TRACE_LEVEL_ERROR);
					abort_setup_req(WITH_CMD_STALL);
				}
			}
		}
	}
}
/*@}*/

/**
 *-------------------------------------------------------------------------*
 * @name get_configuration
 *
 * This function returns to the host the current device configuration value
 *
 *-------------------------------------------------------------------------*
 */
/*@{*/
static void get_configuration(void)
{
	BOOL process = FALSE;
	UINT16 nr_bytes_placed_in_fifo;

	USB_SEND_TRACE("USB: Get Configuration standard request processing", RV_TRACE_LEVEL_DEBUG_LOW);

	if(usb_env_ctrl_blk_p->host_data.ep0_setup.rx_or_tx_ep0_int == USB_TX_EP0_INT) 
	{
		//for this request more IN transactions can occur so check it out
		USB_SEND_TRACE("USB: get_configuration standard request DATA stage is entered", RV_TRACE_LEVEL_DEBUG_LOW);
		usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc++;
			
		//IN transaction on EP0 was ACKed 
		//see figure 17 pg 104 of usb w2fc rev 1.10
		//here wcount <=0 and no more data to be send
		// EP_NUM EP_Num = 0
		// EP_NUM EP_Dir = 1
		// EP_NUM EP_Sel = 0
		// EP_NUM EP_Setup_Sel = 0
		USB_EP_NUM_TX_RELEASE(0);
		//TX CONTROL transaction completion
		prepare_for_cntrl_read_stat_stage(USB_DEFAULT_EP, USB_ACK);		
				
		process = FALSE;

	} 
	else 
	{
		//this is the handling of the setup request 
		USB_SEND_TRACE("USB: get_configuration standard request setup stage is entered", RV_TRACE_LEVEL_DEBUG_LOW);
		/*store request that is currently being processed*/
		store_req(FALSE); //no dummy
		
		//calculate nr of transactions required
		usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc = 0;
			
		//save this transaction only 1 more IN transaction is needed
		usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req = 1;
		//check if conditions for this request are right 
		if(((W2FC_DEVSTAT & W2FC_DEVSTAT_DEF) == W2FC_DEVSTAT_DEF) ||
			((W2FC_DEVSTAT & W2FC_DEVSTAT_ADD) == W2FC_DEVSTAT_ADD) ||
			((W2FC_DEVSTAT & W2FC_DEVSTAT_CFG) == W2FC_DEVSTAT_CFG)) 
		{
			USB_SEND_TRACE("USB: first data IN transaction for get_descriptor ", RV_TRACE_LEVEL_DEBUG_LOW);
			process = TRUE;
		} else 
		{
			//device behaviour not specified
			USB_SEND_TRACE("USB:  request while hw state is undefined, behaviour not specified", RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
			process = FALSE;
		}
	}

	if(process == TRUE) 
	{
		if(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wIndex == 0) 
		{
			if(get_setup_data_mem(&usb_env_ctrl_blk_p->host_data.ep0_data_p, 1) == USB_OK) 
			{
				if((USB_CNFGNR == USB_CONF_DEFAULT) || (USB_CNFGNR == USB_CONF_ADRESSED))
				{
					*usb_env_ctrl_blk_p->host_data.ep0_data_p = 0;
				}
				else
				{
					*usb_env_ctrl_blk_p->host_data.ep0_data_p = USB_CNFGNR;
				}
					
				/*write non-iso TX FIFO data*/
				write_non_iso_tx_fifo_data(1, 
											USB_ENDP0_SIZE, 
											&nr_bytes_placed_in_fifo, 
											usb_env_ctrl_blk_p->host_data.ep0_data_p);

				if(nr_bytes_placed_in_fifo == 1)
				{
					USB_SEND_TRACE("USB: apparently successfully written in fifo", RV_TRACE_LEVEL_DEBUG_LOW);
				} 
				else 
				{
					/*data not succesfully written into Fifo*/
					USB_SEND_TRACE("USB: data not sucessfully written into fifo", RV_TRACE_LEVEL_ERROR);
					abort_setup_req(WITH_CMD_STALL);
				}
			} 
			else 
			{
				//hey we need a legal request
				USB_SEND_TRACE("USB:  wIndex not 0, behaviour not specified", RV_TRACE_LEVEL_ERROR);
				abort_setup_req(WITH_CMD_STALL);
			}
		}
		else 
		{
			//hey we need a legal request
			USB_SEND_TRACE("USB:  wIndex not 0, behaviour not specified", RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
		}
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------*
 * @name set_interface
 *
 * This function request allows the host to select an alternative setting for
 * the specified interface
 *
 *---------------------------------------------------------------------------*
 */
/*@{*/
static void set_interface(void)
{
	const T_USB_INTERFACE_DESCR*	ifdescr_p = NULL;
	UINT8	cfg_idx = 0;
	
	USB_SEND_TRACE("USB: Set Interface standard request processing", RV_TRACE_LEVEL_DEBUG_LOW);

	if((USB_CNFGNR != USB_CONF_DEFAULT) && 
		(USB_CNFGNR != USB_CONF_ADRESSED))
	{
		cfg_idx = USB_CNFGNR - 1;

		if(USB_REQUEST_INDEX < 
			configurations_table[cfg_idx].if_cfg_descr_p->bNumInterfaces)
		{
			ifdescr_p = configurations_table[cfg_idx].if_cnfg_p[USB_REQUEST_INDEX].ifdescr_p;
			// ifdescr_p->bAlternateSetting = usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wValue;
		}
		else
		{
			/* interface is not valid */
			USB_SEND_TRACE("USB: invalid interface for the set_interface request", RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);

		}
	}
	else 
	{
		/* Configruartion is not yet selected */
		USB_SEND_TRACE("USB: wrong conf", RV_TRACE_LEVEL_ERROR);
		abort_setup_req(WITH_CMD_STALL);
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------*
 * @name set_descritpr
 *
 * host software cannot update existing descriptors
 * this standard request is stalled
 *
 *---------------------------------------------------------------------------*
 */
/*@{*/
static void set_descriptor(void)
{
	USB_SEND_TRACE("USB: Set Descriptor standard request processing", RV_TRACE_LEVEL_DEBUG_LOW);

	/* Configruartion is not yet selected */
	USB_SEND_TRACE("USB: can not update descriptors of the selected configuration", RV_TRACE_LEVEL_ERROR);
	abort_setup_req(WITH_CMD_STALL);
}
/*@}*/


/**
 *-------------------------------------------------------------------------*
 * @name get_interface
 *
 * This function returns to the host the current device configuration value
 *
 *-------------------------------------------------------------------------*
 */
/*@{*/
static void get_interface(void)
{
	BOOL process = FALSE;
	UINT16 nr_bytes_placed_in_fifo;

	UINT8	cfg_idx = 0;

	const T_USB_INTERFACE_DESCR*	ifdescr_p = NULL;

	USB_SEND_TRACE("USB: Get Interface standard request processing", RV_TRACE_LEVEL_DEBUG_LOW);

	if(usb_env_ctrl_blk_p->host_data.ep0_setup.rx_or_tx_ep0_int == USB_TX_EP0_INT) 
	{
		//for this request more IN transactions can occur so check it out
		USB_SEND_TRACE("USB: get_interface standard request DATA stage is entered", RV_TRACE_LEVEL_DEBUG_LOW);
		usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc++;
			
		//IN transaction on EP0 was ACKed 
		//see figure 17 pg 104 of usb w2fc rev 1.10
		//here wcount <=0 and no more data to be send
		// EP_NUM EP_Num = 0
		// EP_NUM EP_Dir = 1
		// EP_NUM EP_Sel = 0
		// EP_NUM EP_Setup_Sel = 0
		USB_EP_NUM_TX_RELEASE(0);
		//TX CONTROL transaction completion
		prepare_for_cntrl_read_stat_stage(USB_DEFAULT_EP, USB_ACK);		
				
		process = FALSE;

	} 
	else 
	{
		//this is the handling of the setup request 
		USB_SEND_TRACE("USB: get_interface standard request setup stage is entered", RV_TRACE_LEVEL_DEBUG_LOW);
		/*store request that is currently being processed*/
		store_req(FALSE); //no dummy
		
		//calculate nr of transactions required
		usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc = 0;
			
		//save this transaction only 1 more IN transaction is needed
		usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req = 1;
		//check if conditions for this request are right 
		if(((W2FC_DEVSTAT & W2FC_DEVSTAT_DEF) == W2FC_DEVSTAT_DEF) ||
			((W2FC_DEVSTAT & W2FC_DEVSTAT_ADD) == W2FC_DEVSTAT_ADD) ||
			((W2FC_DEVSTAT & W2FC_DEVSTAT_CFG) == W2FC_DEVSTAT_CFG)) 
		{
			USB_SEND_TRACE("USB: first data IN transaction for get_descriptor ", RV_TRACE_LEVEL_DEBUG_LOW);
			process = TRUE;
		} else 
		{
			//device behaviour not specified
			USB_SEND_TRACE("USB:  request while hw state is undefined, behaviour not specified", RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
			process = FALSE;
		}
	}

	if(process == TRUE) 
	{
		if(get_setup_data_mem(&usb_env_ctrl_blk_p->host_data.ep0_data_p, 1) == USB_OK) 
		{
			if((USB_CNFGNR != USB_CONF_DEFAULT) && 
				(USB_CNFGNR != USB_CONF_ADRESSED))
			{
				cfg_idx = USB_CNFGNR - 1;

				if(USB_REQUEST_INDEX < 
					configurations_table[cfg_idx].if_cfg_descr_p->bNumInterfaces)
				{
					ifdescr_p = configurations_table[cfg_idx].if_cnfg_p[USB_REQUEST_INDEX].ifdescr_p;
					*usb_env_ctrl_blk_p->host_data.ep0_data_p = ifdescr_p->bAlternateSetting;

					/*write non-iso TX FIFO data*/
					write_non_iso_tx_fifo_data(1, 
												USB_ENDP0_SIZE, 
												&nr_bytes_placed_in_fifo, 
												usb_env_ctrl_blk_p->host_data.ep0_data_p);

					if(nr_bytes_placed_in_fifo == 1)
					{
						USB_SEND_TRACE("USB: apparently successfully written in fifo", RV_TRACE_LEVEL_DEBUG_LOW);
					} 
					else 
					{
						/*data not succesfully written into Fifo*/
						USB_SEND_TRACE("USB: data not sucessfully written into fifo", RV_TRACE_LEVEL_ERROR);
						abort_setup_req(WITH_CMD_STALL);
					}
				}
				else
				{
					/* interface is not valid */
					USB_SEND_TRACE("USB: invalid interface for the get_interface request", RV_TRACE_LEVEL_ERROR);
					abort_setup_req(WITH_CMD_STALL);

				}

			}
			else 
			{
				/* Configruartion is not yet selected */
				USB_SEND_TRACE("USB: wrong conf", RV_TRACE_LEVEL_ERROR);
				abort_setup_req(WITH_CMD_STALL);
			}
		} 
		else 
		{
			/*there is no memory available to store setup data in*/
			USB_SEND_TRACE("USB: memory to store data in is NOT available, send STALL CMD", 
				RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
		}
	}
}
/*@}*/


/**
 *---------------------------------------------------------------*
 * @name set_configuration
 *
 * This function request sets the device configuration
 *
 *---------------------------------------------------------------*
 */
/*@{*/
static void set_configuration(void)
{
	USB_SEND_TRACE("USB: set_configuration standard request received", RV_TRACE_LEVEL_DEBUG_LOW);

	if(USB_CNFGNR == usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wValue)
	{
		USB_SEND_TRACE("USB: Configuration already set", RV_TRACE_LEVEL_DEBUG_LOW);
		USB_SEND_TRACE("USB: No need to set configuration again", RV_TRACE_LEVEL_DEBUG_LOW);
		W2FC_SYSCON2 = W2FC_SYSCON2_DEV_CFG;

		return;
	}

	//store configuration value
	USB_CNFGNR = 
		usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wValue;
	if(USB_CONF_ADRESSED == usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wValue)
      {
          W2FC_SYSCON2 =  W2FC_SYSCON2_CLR_CFG;
  	      notify_swe_connect(FALSE);
          return;
      }
	
	W2FC_SYSCON1 &= ~W2FC_SYSCON1_CFG_LOCK;
		USB_SEND_TRACE("USB: SET CONFIGURATION RECIEVED unlock current config", 
		RV_TRACE_LEVEL_ERROR);

	if(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wValue != 
		USB_CONF_DEFAULT)
	{
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
		if(EndpointConfiguration() != USB_OK) {
			USB_SEND_TRACE("USB: EndpointConfiguration failed", RV_TRACE_LEVEL_ERROR);
		} else {
			USB_SEND_TRACE("USB: EndpointConfiguration succeeded", RV_TRACE_LEVEL_DEBUG_LOW);
		}
	} else {
		//clear physical endpoint configuration
		init_physical_eps();
		USB_SEND_TRACE("USB: HW cfg changed back to USB_CONF_DEFAULT", RV_TRACE_LEVEL_DEBUG_LOW);
	}
	if( USB_CNFGNR >=1 ) {
			W2FC_SYSCON2 = W2FC_SYSCON2_DEV_CFG;
	} else {
		W2FC_SYSCON2 = W2FC_SYSCON2_CLR_CFG;
	}
}
/*@}*/

/**
 *---------------------------------------------------------------*
 * @name set_feature
 *
 * This function request sets the device configuration
 *
 *---------------------------------------------------------------*
 */
/*@{*/
void set_feature(void)
{
	UINT8	usb_bmReqFeature = 0;
	UINT8	pep_nr = 0;				/* Physical endpoint number */

	/* Get selected feature */
	usb_bmReqFeature = USB_REQUEST_TYPE & 0x3;

	USB_SEND_TRACE("USB: Set Feature standard request processing", RV_TRACE_LEVEL_DEBUG_LOW);

	switch(usb_bmReqFeature)
	{
		case USB_RECIPIENT_DEVICE :	/* Recipient is device */
			if(USB_REQUEST_VALUE == USB_DEVICE_REMOTE_WAKEUP)
			{
				/* remote wakeup is enabled by the USB HOST */
				usb_env_ctrl_blk_p->host_data.usb_rem_wake_enabled = TRUE;
			}
			else
			{
				/* interface is not valid */
				USB_SEND_TRACE("USB: invalid feature selector", RV_TRACE_LEVEL_ERROR);
				abort_setup_req(WITH_CMD_STALL);
			}
		break;

		case USB_RECIPIENT_INTERFACE :	/* Recipient is interface */
					/* nothing to do */
		break;

		case USB_RECIPIENT_ENDPOINT :	/* Recipient is an endpoint */
			if(USB_REQUEST_VALUE == USB_ENDPOINT_HALT)
			{
				/* Set stall on endpoint */
				pep_nr = USB_REQUEST_INDEX & 0x0F;
				
				if(USB_REQUEST_INDEX & USB_IN)	// IN endpoint
				{
					USB_EP_NUM_SEL_TX_EP(pep_nr);
					USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_SET_HALT);
					usb_env_ctrl_blk_p->bm_tx_ep_buf_stall |= (1 << pep_nr);
					USB_EP_NUM_TX_RELEASE(pep_nr);
				}
				else // OUT endpoint
				{
					USB_EP_NUM_SEL_RX_EP(pep_nr);
					USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_SET_HALT);
					usb_env_ctrl_blk_p->bm_rx_ep_buf_stall |= (1 << pep_nr);
					USB_EP_NUM_RX_RELEASE(pep_nr);
				}

				USB_EP_NUM_SEL_RX_EP(USB_DEFAULT_EP);
			}
			else
			{
				/* interface is not valid */
				USB_SEND_TRACE("USB: invalid feature selector", RV_TRACE_LEVEL_ERROR);
				abort_setup_req(WITH_CMD_STALL);
			}
		break;

		default :
			/* interface is not valid */
			USB_SEND_TRACE("USB: invalid feature", RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
		break;
	}
}
/*@}*/

/**
 *---------------------------------------------------------------*
 * @name clear_feature
 *
 * implement behaviour of the host to device request clear feature
 *
 *---------------------------------------------------------------*
 */
/*@{*/
void clear_feature(void)
{
	UINT8	usb_bmReqFeature = 0;
	UINT8	pep_nr = 0;				/* Physical endpoint number */

	/* Get selected feature */
	usb_bmReqFeature = USB_REQUEST_TYPE & 0x3;

	USB_SEND_TRACE("USB: Clear Feature standard request processing", RV_TRACE_LEVEL_DEBUG_LOW);

	switch(usb_bmReqFeature)
	{
		case USB_RECIPIENT_DEVICE :	/* Recipient is device */
			if(USB_REQUEST_VALUE == USB_DEVICE_REMOTE_WAKEUP)
			{
				/* remote wakeup is disabled by the USB HOST */
				usb_env_ctrl_blk_p->host_data.usb_rem_wake_enabled = FALSE;
			}
			else
			{
				/* interface is not valid */
				USB_SEND_TRACE("USB: invalid feature selector", RV_TRACE_LEVEL_ERROR);
				abort_setup_req(WITH_CMD_STALL);
			}
		break;

		case USB_RECIPIENT_INTERFACE :	/* Recipient is interface */
					/* nothing to do */
		break;

		case USB_RECIPIENT_ENDPOINT :	/* Recipient is an endpoint */
			if(USB_REQUEST_VALUE == USB_ENDPOINT_HALT)
			{
				/* Set stall on endpoint */
				pep_nr = USB_REQUEST_INDEX & 0x0F;
				
				if(USB_REQUEST_INDEX & USB_IN)	// IN endpoint
				{
					USB_EP_NUM_SEL_TX_EP(pep_nr);
					USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_CLR_HALT);	/* Clear halt condition */
					usb_env_ctrl_blk_p->bm_tx_ep_buf_stall &= ~(1 << pep_nr);
		//			usb_env_ctrl_blk_p->bm_tx_ep_buf_stall &= (~1 << pep_nr);
					USB_EP_NUM_TX_RELEASE(pep_nr);
				}
				else // OUT endpoint
				{
					USB_EP_NUM_SEL_RX_EP(pep_nr);
					USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_CLR_HALT);	/* Clear halt condition */
/*###==> USB-MS ANO 10-13-2004 by YL */					
					USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_CLR_EP);	/* reset endpoint fifo */
					if(pep_nr == 0)
					{
/*<==###*/						
						USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_SET_FIFO_EN);
					}
					//usb_env_ctrl_blk_p->bm_rx_ep_buf_stall &= (~1 << pep_nr);
/*###==> USB-MS ANO*/	
					USB_EP_NUM_RX_RELEASE(pep_nr);
				}
				
				USB_EP_NUM_SEL_RX_EP(USB_DEFAULT_EP);
			}
			else
			{
				/* interface is not valid */
				USB_SEND_TRACE("USB: invalid feature selector", RV_TRACE_LEVEL_ERROR);
				abort_setup_req(WITH_CMD_STALL);
			}
		break;

		default :
			/* interface is not valid */
			USB_SEND_TRACE("USB: invalid feature selector", RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
		break;
	}
}
/*@}*/

/**
 *---------------------------------------------------------------*
 * @name get_status
 *
 * This function returns status for the specified recipient
 *
 *---------------------------------------------------------------*
 */
/*@{*/
void get_status(void)
{
	BOOL process = FALSE;
	UINT16 nr_bytes_placed_in_fifo;

	UINT8	usb_bmReqFeature = 0;
	UINT8	pep_nr = 0;				/* Physical endpoint number */

	UINT16 remvove_stall = 0;

	UINT8	bmAttributes = 0;

	const T_USB_INTERFACE_DESCR*	ifdescr_p = NULL;

	USB_SEND_TRACE("USB: Get Status standard request processing", RV_TRACE_LEVEL_DEBUG_LOW);

	if(usb_env_ctrl_blk_p->host_data.ep0_setup.rx_or_tx_ep0_int == USB_TX_EP0_INT) 
	{
		//for this request more IN transactions can occur so check it out
		USB_SEND_TRACE("USB: get_interface standard request DATA stage is entered", RV_TRACE_LEVEL_DEBUG_LOW);
		usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc++;
			
		//IN transaction on EP0 was ACKed 
		//see figure 17 pg 104 of usb w2fc rev 1.10
		//here wcount <=0 and no more data to be send
		// EP_NUM EP_Num = 0
		// EP_NUM EP_Dir = 1
		// EP_NUM EP_Sel = 0
		// EP_NUM EP_Setup_Sel = 0
		USB_EP_NUM_TX_RELEASE(0);
		//TX CONTROL transaction completion
		prepare_for_cntrl_read_stat_stage(USB_DEFAULT_EP, USB_ACK);		
				
		process = FALSE;

	} 
	else 
	{
		//this is the handling of the setup request 
		USB_SEND_TRACE("USB: get_status standard request setup stage is entered", RV_TRACE_LEVEL_DEBUG_LOW);
		/*store request that is currently being processed*/
		store_req(FALSE); //no dummy
		
		//calculate nr of transactions required
		usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc = 0;
			
		//save this transaction only 1 more IN transaction is needed
		usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req = 1;
		//check if conditions for this request are right 
		if(((W2FC_DEVSTAT & W2FC_DEVSTAT_DEF) == W2FC_DEVSTAT_DEF) ||
			((W2FC_DEVSTAT & W2FC_DEVSTAT_ADD) == W2FC_DEVSTAT_ADD) ||
			((W2FC_DEVSTAT & W2FC_DEVSTAT_CFG) == W2FC_DEVSTAT_CFG)) 
		{
			USB_SEND_TRACE("USB: first data IN transaction for get_descriptor ", RV_TRACE_LEVEL_DEBUG_LOW);
			process = TRUE;
		} else 
		{
			//device behaviour not specified
			USB_SEND_TRACE("USB:  request while hw state is undefined, behaviour not specified", RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
			process = FALSE;
		}
	}

	if(process == TRUE) 
	{
		if(get_setup_data_mem(&usb_env_ctrl_blk_p->host_data.ep0_data_p, 2) == USB_OK) 
		{
			if((USB_CNFGNR != USB_CONF_DEFAULT) && 
				(USB_CNFGNR != USB_CONF_ADRESSED))
			{
				/* Get selected feature */
				usb_bmReqFeature = USB_REQUEST_TYPE & 0x3;

				switch(usb_bmReqFeature)
				{
					case USB_RECIPIENT_DEVICE :	/* Recipient is device */
						*usb_env_ctrl_blk_p->host_data.ep0_data_p		= 0;
						*(usb_env_ctrl_blk_p->host_data.ep0_data_p + 1) = 0;

						if(usb_env_ctrl_blk_p->host_data.usb_rem_wake_enabled == TRUE)
						{
							*usb_env_ctrl_blk_p->host_data.ep0_data_p |= 0x02;
						}
						
						bmAttributes = 
							configurations_table[USB_CNFGNR-1].if_cfg_descr_p->bmAttributes;
						
						if(bmAttributes & 0x40)
						{
							*usb_env_ctrl_blk_p->host_data.ep0_data_p |= 0x01;
						}

					break;

					case USB_RECIPIENT_INTERFACE :	/* Recipient is interface */
						*usb_env_ctrl_blk_p->host_data.ep0_data_p		= 0;
						*(usb_env_ctrl_blk_p->host_data.ep0_data_p + 1) = 0;
					break;

					case USB_RECIPIENT_ENDPOINT :	/* Recipient is an endpoint */
						/* Set stall on endpoint */
						pep_nr = USB_REQUEST_INDEX & 0x0F;
						
						if(USB_REQUEST_INDEX & USB_IN)	// IN endpoint
						{
							remvove_stall= usb_env_ctrl_blk_p->bm_tx_ep_buf_stall;
						}
						else // OUT endpoint
						{
							remvove_stall= usb_env_ctrl_blk_p->bm_rx_ep_buf_stall;
						}

						*usb_env_ctrl_blk_p->host_data.ep0_data_p		= 0;
						*(usb_env_ctrl_blk_p->host_data.ep0_data_p + 1) = 0;

						if((remvove_stall & (1 << pep_nr)) == (1 << pep_nr))
						{
							*usb_env_ctrl_blk_p->host_data.ep0_data_p = 0x01;
						}

					break;

					default :
						/* recipient is not valid */
						USB_SEND_TRACE("USB: invalid recipient", RV_TRACE_LEVEL_ERROR);
						abort_setup_req(WITH_CMD_STALL);
						return;
				}
				
				/*write non-iso TX FIFO data*/
				write_non_iso_tx_fifo_data(2, 
											USB_ENDP0_SIZE, 
											&nr_bytes_placed_in_fifo, 
											usb_env_ctrl_blk_p->host_data.ep0_data_p);

				if(nr_bytes_placed_in_fifo == 2)
				{
					USB_SEND_TRACE("USB: apparently successfully written in fifo", RV_TRACE_LEVEL_DEBUG_LOW);
				} 
				else 
				{
					/*data not succesfully written into Fifo*/
					USB_SEND_TRACE("USB: data not sucessfully written into fifo", RV_TRACE_LEVEL_ERROR);
					abort_setup_req(WITH_CMD_STALL);
				}
			}
			else 
			{
				/* Configruartion is not yet selected */
				USB_TRACE_WARNING_PARAM("USB: get_status wrong conf current_cnfg_nr:", USB_CNFGNR);
				abort_setup_req(WITH_CMD_STALL);
			}
		} 
		else 
		{
			/*there is no memory available to store setup data in*/
			USB_SEND_TRACE("USB: memory to store data in is NOT available, send STALL CMD", 
				RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
		}
	}
}
/*@}*/

/**
 *---------------------------------------------------------------*
 * @name synch_frame
 *
 * This function request sets the device configuration
 *
 *---------------------------------------------------------------*
 */
/*@{*/
void synch_frame(void)
{
	USB_SEND_TRACE("USB: Synch Frame standard request processing", RV_TRACE_LEVEL_DEBUG_LOW);

	/* USB stack does not support isochronous transfer */
	USB_SEND_TRACE("USB: USB stack does not support isochronous transfer", RV_TRACE_LEVEL_ERROR);
	abort_setup_req(WITH_CMD_STALL);
}
/*@}*/

/**
 *---------------------------------------------------------------*
 * @name dummy
 *
 * This function is used as a dummy and is called when the wrong 
 * request is provided
 *
 *---------------------------------------------------------------*
 */
/*@{*/
static void dummy(void)
{
	USB_SEND_TRACE("USB: dummy func called", 
		RV_TRACE_LEVEL_DEBUG_LOW);
}
/*@}*/

/**
 *---------------------------------------------------------------*
 * @name vendor_test_request
 *
 * This function request may be used by the example code driver 
 *
 *---------------------------------------------------------------*
 */
/*@{*/

/*
static void vendor_test_request(void)
{
	UINT8 i;

	//in the example driver a lot of DMA enabling and checking is done
	//we will not take any action here with the recieved 4 bytes
	USB_SEND_TRACE("USB: vendor_test_request", 
		RV_TRACE_LEVEL_DEBUG_LOW);
	if(USB_REQUEST_VALUE < MAX_DMA_CHN) {
		USB_SEND_TRACE("USB: vendor_test_request wants to use DMA channels, that are NOT supported", 
			RV_TRACE_LEVEL_DEBUG_LOW);
	}
}

*/

/*@}*/

/**
 *---------------------------------------------------------------*
 * @name epn_int_hndlr
 *
 * This function handles endpoint n rx and tx interrupts
 *
 *  W2FC Functional Specification Rev1.10: Figure 24 on Page 112
 *---------------------------------------------------------------*
 */
/*@{*/
static T_RV_RET epn_int_hndlr(void)
{
	UINT8 endp_nb;
	T_RV_RET ret = USB_OK;

	#if (L1_POWER_MGT != 0x00)
	Usb_Vote_DeepSleepStatus(); 
	/*to update power status.added by saumar*/
	#endif
	
	// IRQ_SRC.EPn_RX=1? //
	if((inth_reg & W2FC_IRQ_SRC_EPN_RX) == W2FC_IRQ_SRC_EPN_RX)	{
		// Read endp_nb value from EPN_STAT.EPn_RX_IT_src //
		endp_nb = W2FC_EPN_STAT >> 8;

		if(endp_nb == 0)
			return RV_OK;

		// write '1' to IRQ_SRC.EPn_RX to clear the IT //
		W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_EPN_RX);
		// Write EP_NUM register: //
		// EP_NUM.EP_Num = endp_nb //
		// EP_NUM.EP_Dir = 0       //
		// EP_NUM.EP_Sel = 1       //
		// EP_NUM.Setup_Sel = 0    //
		USB_EP_NUM_SEL_RX_EP(endp_nb);

		if(usb_env_ctrl_blk_p->host_data.usb_connected == TRUE)
		{
			// RX Handler //
			if(non_iso_rx_hndlr(endp_nb) != USB_OK) 
			{
				ret = USB_INTERNAL_ERR;
				USB_SEND_TRACE("USB: non_iso_rx_hndlr failed",
							RV_TRACE_LEVEL_ERROR);
			}
		}

		// Write EP_NUM register: //
		// EP_NUM.EP_Num = endp_nb //
		// EP_NUM.EP_Dir = 0       //
		// EP_NUM.EP_Sel = 0       //
		// EP_NUM.Setup_Sel = 0    //
		USB_EP_NUM_RX_RELEASE(endp_nb);
	
		// Flag Fifo not full and DB=1? //
		/*
		if(usb_env_ctrl_blk_p->host_data.fifo_not_full == TRUE) {
			// Write CTRL register: CTRL.Set_FIFO_En=1 //
			W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
		}
		*/
	} else {
	// Must be IRQ_SRC.EPn_TX //
		// Read endp_nb value from EPN_STAT.EPn_TX_IT_src //
		endp_nb = W2FC_EPN_STAT;

		if(endp_nb == 0)
			return RV_OK;	

		// write '1' to IRQ_SRC.EPn_TX to clear the IT //
		W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_EPN_TX);

		// Write EP_NUM register: //
		// EP_NUM.EP_Num = endp_nb //
		// EP_NUM.EP_Dir = 1       //
		// EP_NUM.EP_Sel = 1       //
		// EP_NUM.Setup_Sel = 0    //
		USB_EP_NUM_SEL_TX_EP(endp_nb);

		//for solving PR190
		USB_TRACE_WARNING_PARAM("USB: tx status flag W2FC_STAT_FLG before clr halt:", W2FC_STAT_FLG);
		USB_TRACE_WARNING_PARAM("USB: tx endpoint number:", W2FC_EP_NUM);

		// TX Handler //
		if(non_iso_tx_hndlr( endp_nb) != USB_OK) {
			ret = USB_INTERNAL_ERR;
			USB_SEND_TRACE("USB: non_iso_tx_hndlr failed",
				RV_TRACE_LEVEL_ERROR);
		}
		// Write EP_NUM register: //
		// EP_NUM.EP_Num = endp_nb //
		// EP_NUM.EP_Dir = 1       //
		// EP_NUM.EP_Sel = 0       //
		// EP_NUM.Setup_Sel = 0    //
		USB_EP_NUM_TX_RELEASE(endp_nb);
	}
	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name non_iso_rx_hndlr
 *
 * NonISO RX Handler 
 *
 * W2FC Functional Specification Rev1.10: Figure 25 on Page 113
 *
 * @param		endp_nb:		physical endpoint number on which the interrupt 
 *								occured
 *
 * @return	T_RV_RET			when function executed successfully USB_OK, when 
 *								failed RVM_MEMORY_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET non_iso_rx_hndlr(UINT8 endp_nb)
{
	T_RV_RET ret = USB_OK;

	USB_SEND_TRACE("USB: RX n interrupt RECIEVED",
		RV_TRACE_LEVEL_DEBUG_LOW);

	// STAT_FLG.ACK=1? (Data packet received?) //
	if((W2FC_STAT_FLG & W2FC_STAT_FLG_ACK) == W2FC_STAT_FLG_ACK) 
	{
		T_RV_RET ret_read;

		ret_read = read_non_iso_packet(endp_nb);

		// Application specific preaparation to receive endpoint data //
		if(ret_read != USB_OK) {
			ret = USB_INTERNAL_ERR;
			USB_SEND_TRACE("USB: read_non_iso_packet not succesfull",
				RV_TRACE_LEVEL_ERROR);
		} else {
			USB_SEND_TRACE("USB: read_non_iso_packet succesfull",
				RV_TRACE_LEVEL_DEBUG_LOW);
		}
	} else {// No. Must be STAT_FLG.STALL //
			// LH-Initiated Stall and can remove stall? -> Yes //
		USB_TRACE_WARNING_PARAM("USB : W2FC_STAT_FLG : ", W2FC_STAT_FLG);

		if((usb_env_ctrl_blk_p->bm_rx_ep_buf_stall & (1 << endp_nb)) != (1 << endp_nb)) {
			USB_SEND_TRACE("USB: stall can be removed halt bit wasn't set",
							RV_TRACE_LEVEL_DEBUG_LOW);

			//stall_flg is "no longer" set thus.. 
			// clear CTRL.Clr_Halt (remove halt condition) //
			W2FC_CTRL = W2FC_CTRL_CLR_HALT;
			W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
		} else {
			//check if new buffer is already provided
			if(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(endp_nb)].temp_buf_p != NULL) {
				USB_SEND_TRACE("USB: stall can be removed buffer is available",
							RV_TRACE_LEVEL_DEBUG_LOW);

				usb_env_ctrl_blk_p->bm_rx_ep_buf_stall &= ~(1U << endp_nb);
				//stall_flg is "no longer" set thus.. 
				// clear CTRL.Clr_Halt (remove halt condition) //
				W2FC_CTRL = W2FC_CTRL_CLR_HALT;
				W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
			} else {
				W2FC_CTRL = W2FC_CTRL_SET_HALT;
				ret = USB_INTERNAL_ERR;
				USB_SEND_TRACE("USB: Stall condition cannot yet be removed & force halt again",
						RV_TRACE_LEVEL_ERROR);
			}
		}
	}
	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name non_iso_tx_hndlr
 *
 * NonISO TX Handler 
 *
 * W2FC Functional Specification Rev1.10: Figure 27 on Page 115
 *
 * @param		endp_nb:		physical endpoint number on which the interrupt 
 *								occured
 *
 * @return	T_RV_RET			when function executed successfully USB_OK, when 
 *								failed RVM_MEMORY_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET non_iso_tx_hndlr(UINT8 endp_nb)
{
	T_RV_RET ret = USB_OK;

	// STAT_FLG.ACK=1? (Data packet received?) //
	if( W2FC_STAT_FLG & W2FC_STAT_FLG_ACK ) {
		// Retire the data that was just sent to the USB host from the Application's endpoint TX buffer //
		// Application specific activity because data successfully sent //
		if(write_non_iso_packet((endp_nb-1)) != USB_OK) {
			ret = USB_INTERNAL_ERR;
			USB_SEND_TRACE("USB: read_non_iso_packet not succesfull",
				RV_TRACE_LEVEL_ERROR);
		} else {
			USB_SEND_TRACE("USB: TXn int ACKED!!",
				RV_TRACE_LEVEL_DEBUG_LOW);
		}
	} else {
	// No. Must be STAT_FLG.STALL //
		// LH-Initiated Stall and can remove stall? -> Yes //
		if((usb_env_ctrl_blk_p->bm_tx_ep_buf_stall & (1 << endp_nb)) != (1 << endp_nb)) {
			//stall_flg is "no longer" set thus.. 
			// clear CTRL.Clr_Halt (remove halt condition) //
			W2FC_CTRL = W2FC_CTRL_CLR_HALT;
			W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
			USB_SEND_TRACE("USB: stall cleared",
				RV_TRACE_LEVEL_DEBUG_LOW);
		} else {
			//check if new buffer is already provided
			if(usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl[endp_nb].temp_buf_p != NULL) {
				usb_env_ctrl_blk_p->bm_tx_ep_buf_stall &= ~(1U << endp_nb);
				//stall_flg is "no longer" set thus.. 
				// clear CTRL.Clr_Halt (remove halt condition) //
				W2FC_CTRL = W2FC_CTRL_CLR_HALT;
				W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
				USB_SEND_TRACE("USB: stall cleared",
							RV_TRACE_LEVEL_DEBUG_LOW);
			} else {
				ret = USB_INTERNAL_ERR;
				USB_SEND_TRACE("USB: Stall condition cannot yet be removed",
						RV_TRACE_LEVEL_ERROR);
			}
		}
	}
	return(ret);
}

/**
 *---------------------------------------------------------------------------------* 
 * @name ep0_tx_int_hndlr
 *
 * NonISO EP0 TX Handler 
 *
 * W2FC Functional Specification Rev1.10: Figure 17 on Page 104
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void ep0_tx_int_hndlr(void)
{
	UINT16 stat_flg;
	UINT8 i = 0;

	usb_env_ctrl_blk_p->host_data.ep0_setup.rx_or_tx_ep0_int = USB_TX_EP0_INT;	//tx interrupt occurred
	//clear the interrupt
	W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_EP0_TX);
	
	// Write EP_NUM register: //
	// EP_NUM.EP_Num = 0      //
	// EP_NUM.EP_Dir = 1      //
	// EP_NUM.EP_Sel = 1      //
	// EP_NUM.Setup_Sel = 0   //
	USB_EP_NUM_SEL_TX_EP(0);

	#if (L1_POWER_MGT != 0x00)
	Usb_Vote_DeepSleepStatus(); 
	/*to update power status.added by saumar*/
	#endif

	//apparently the hardware needs some time to update its registers
	while(i < 5) i++;

	stat_flg = W2FC_STAT_FLG;

	if((stat_flg & W2FC_STAT_FLG_ACK) == W2FC_STAT_FLG_ACK) {
		USB_SEND_TRACE("USB:(IN) TX interrupt ACKED", 
			RV_TRACE_LEVEL_ERROR);
				
		/*since it is a IN transaction an OUT (empty) packet during this setup request 
		would indicate that the current request is handled*/
		if(USB_SETUP_OUT == TRUE) {
			
			if(usb_env_ctrl_blk_p->host_data.ep0_data_p != NULL) {
				rvf_free_buf(usb_env_ctrl_blk_p->host_data.ep0_data_p);
				usb_env_ctrl_blk_p->host_data.ep0_data_p = NULL;
			}

			/*set control flags to FALSE*/
			USB_SETUP_OUT = FALSE;
			USB_SETUP_IN = FALSE;

			usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req = 0;
			usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc = 0;
			
			//set fucntion pntr to dummy
			store_req(TRUE);
			
			//clear endpoint 0
			// EP_NUM EP_Num = 0
			// EP_NUM EP_Dir = 1
			// EP_NUM EP_Sel = 0
			// EP_NUM EP_Setup_Sel = 0
			USB_EP_NUM_TX_RELEASE(0);
		} else {
			if(usb_env_ctrl_blk_p->host_data.ep0_setup.current_setup_req != NULL) {
				//1 more transfer processed
				usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc++;
				/*executed requested action*/
				usb_env_ctrl_blk_p->host_data.ep0_setup.current_setup_req();
			} else {
				USB_SEND_TRACE("USB:no function pointer available to handle TX interrupt", 
					RV_TRACE_LEVEL_ERROR);
			}
		}
	} else {
		if((stat_flg & W2FC_STAT_FLG_STALL) == W2FC_STAT_FLG_STALL) {
			//IN transaction on EP0 was stalled
			USB_SEND_TRACE("USB: status stage responsed with STALL", 
				RV_TRACE_LEVEL_DEBUG_LOW);
			//set halt condition
			W2FC_CTRL =W2FC_CTRL_CLR_HALT;
			USB_EP_NUM_TX_RELEASE(USB_DEFAULT_EP);
		} else {
			//this is not probable for this function but maybe some other
			//conditions arise that allow a NAK to be checked here
			if((stat_flg & W2FC_STAT_FLG_NAK) == W2FC_STAT_FLG_NAK) {
				//IN transaction on EP0 was halted
				USB_SEND_TRACE("USB: status stage response with NAK", 
					RV_TRACE_LEVEL_DEBUG_LOW);
				//clear halt condition
				W2FC_CTRL = W2FC_CTRL_CLR_HALT;

				USB_EP_NUM_TX_RELEASE(USB_DEFAULT_EP);
			} else {
				//problem
				USB_SEND_TRACE("USB: status stage response with NAK", 
				RV_TRACE_LEVEL_DEBUG_LOW);
				//clear halt condition
				W2FC_CTRL = W2FC_CTRL_CLR_HALT;
				USB_EP_NUM_TX_RELEASE(USB_DEFAULT_EP);
			}
		}
		//unselect endpoint 0
		USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);
		USB_SEND_TRACE("USB: TX int was recieved but not ACKed", 
			RV_TRACE_LEVEL_ERROR);
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name ep0_rx_int_hndlr
 *
 * NonISO EP0 RX Handler 
 *
 * W2FC Functional Specification Rev1.10: Figure 15 on Page 102
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void ep0_rx_int_hndlr(void)
{
	UINT16 nr_bytes_in_buf;
	UINT16 nr_bytes_from_fifo;

	UINT8 usb_bmReqType;
	
	#if (L1_POWER_MGT != 0x00)
	Usb_Vote_DeepSleepStatus(); 
	/*to update power status.added by saumar*/
	#endif
	
	/* Get request type Standard, Class or Vendor */
	usb_bmReqType = ((USB_REQUEST_TYPE) >> 5) & 0x3;

	usb_env_ctrl_blk_p->host_data.ep0_setup.rx_or_tx_ep0_int = USB_RX_EP0_INT;	//rx interrupt occurred
	//clear the interrupt
	W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_EP0_RX);

	//select endpoint 0
	// EP_NUM EP_Num = 0
	// EP_NUM EP_Dir = 0
	// EP_NUM EP_Sel = 1
	// EP_NUM EP_Setup_Sel = 0
	USB_EP_NUM_SEL_RX_EP(0);
	
	//check status flag for handshake ACK
	if((W2FC_STAT_FLG & W2FC_STAT_FLG_ACK) == W2FC_STAT_FLG_ACK) {
		USB_SEND_TRACE("USB: RX is acked", 
			RV_TRACE_LEVEL_ERROR);
		
		/*since it is a IN transaction an OUT (empty) packet during this setup request 
		would indicate that the current request is handled*/
		if(USB_SETUP_IN == TRUE) {
			
			if(usb_env_ctrl_blk_p->host_data.ep0_data_p != NULL) {
				rvf_free_buf(usb_env_ctrl_blk_p->host_data.ep0_data_p);
				usb_env_ctrl_blk_p->host_data.ep0_data_p = NULL;
			}

			/* this setup transaction is finished completion status */	
			USB_SEND_TRACE("USB: ep0_rx_int_hndlr request status stage is entered", 
				RV_TRACE_LEVEL_DEBUG_LOW);
			
			/*set control flags to FALSE*/
			USB_SETUP_OUT = FALSE;
			USB_SETUP_IN = FALSE;

			usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req = 0;
			usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc = 0;
			
			//set fucntion pntr to dummy
			store_req(TRUE);
			
			//clear endpoint 0
			USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);
		} else {
			if(usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc <=
				usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req) {
				if(USB_REQUEST_LENGTH <= USB_ENDP0_SIZE) 
				{
					nr_bytes_in_buf = USB_REQUEST_LENGTH;
				} else {
					//are there still more transactions expected?
					if((usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc + 1) <
					usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req) {
						nr_bytes_in_buf = USB_ENDP0_SIZE;
					} else {
						nr_bytes_in_buf = (USB_REQUEST_LENGTH - (USB_ENDP0_SIZE * 
											usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc));
					}
				}
				read_non_iso_rx_fifo_data(USB_DEFAULT_EP, 
											&nr_bytes_from_fifo, 
											usb_env_ctrl_blk_p->host_data.ep0_data_p
											+ usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc * USB_ENDP0_SIZE);
				
				if((nr_bytes_from_fifo == USB_ENDP0_SIZE) || 
					(nr_bytes_from_fifo == (USB_REQUEST_LENGTH - (USB_ENDP0_SIZE * 
					usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc)))) {   
					//1 more transfer processed
					usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc++;
					USB_SEND_TRACE("USB: read data from fifo succesfull",
						RV_TRACE_LEVEL_DEBUG_LOW);
				} else {
					stall_req_and_ep(USB_DEFAULT_EP, USB_RX);
					USB_SEND_TRACE("USB: read data from fifo failed",
						RV_TRACE_LEVEL_ERROR);
				}

				//clear endpoint 0
				USB_REG16_ASSIGN(W2FC_EP_NUM, W2FC_16BIT_RESET);

				//check if all the data has been read now? ifso... take specific action
				if(usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc ==
				usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req) 
				{
					if(usb_bmReqType  == USB_STANDARD)
					{
						prepare_for_cntrl_write_stat_stage(USB_DEFAULT_EP, USB_ACK);
					}
					else /* Class or Vendor request */
					{
						specific_ep0_rx_handler();
					}
				} else {
					//figure 15 on page 102 of v 1.10 usb_w2fc spec suggests to test for
					//ready to recieve more data?. for the moment this test seems irrelevant
					
					//fifo enable
					USB_REG16_ASSIGN(W2FC_CTRL, W2FC_CTRL_SET_FIFO_EN);
				}
			} else {
				USB_SEND_TRACE("USB: to many data read from fifo actions",
					RV_TRACE_LEVEL_ERROR);
			}
		}
	} else {
		//RX not Acked
		USB_SEND_TRACE("USB: RX is NOT acked", 
			RV_TRACE_LEVEL_ERROR);
		//take action to remove nack
		W2FC_CTRL = W2FC_CTRL_CLR_HALT;
		//clear endpoint 0
		W2FC_EP_NUM = W2FC_16BIT_RESET;
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name abort_setup_req
 *
 * This function aborts the request we're currently processing
 * probably due to an internal error 
 *
 * W2FC Functional Specification Rev1.10: Figure 15 on Page 102
 *
 * @param	with_or_without_cmd_stall	Boolean indicating if the endpoint must be 
 *										stalled or not
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void abort_setup_req(BOOL with_or_without_cmd_stall)
{
	USB_SEND_TRACE("USB: abort setup request called", RV_TRACE_LEVEL_DEBUG_HIGH);
	/*clear the interrupt source setup bit*/
	W2FC_EP_NUM = W2FC_16BIT_RESET;
	/*select setup fifo this will clr int*/
	W2FC_EP_NUM |= W2FC_EP_NUM_SETUP_SEL;
	W2FC_EP_NUM = W2FC_16BIT_RESET;
	/*we don't want to wait untill memory is available, just quit*/
	/*enable NAK interrupt*/
	W2FC_SYSCON1 |= W2FC_SYSCON1_NAK_EN;

	/*clear flags*/
	USB_SETUP_IN = FALSE;
	USB_SETUP_OUT = FALSE;
	
	/*check if command needs to be stalled*/
	if(with_or_without_cmd_stall == TRUE) {
		/* setup request was unvalid so stall the cmd untill next setup */
		W2FC_SYSCON2 |= W2FC_SYSCON2_STALL_CMD;
		USB_SEND_TRACE("USB: abort setup request stalled", RV_TRACE_LEVEL_DEBUG_HIGH);
	}
	//clear request buffer
	clear_setup_buf();
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name prepare_for_cntrl_read_stat_stage
 *
 * This function prepares the device for the status stage that is comming
 * if the data stage was executed successfully it will respons with an ACK
 * else it will stall the request
 *
 * For TX CONTROL transactions
 *
 * @param	ack_or_stall	indicating if the endpoint must be stalled or ACKED
 *
 * @param   ep_nr			the endpoint that should be prepared
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void prepare_for_cntrl_read_stat_stage(UINT8 ep_nr, T_USB_RESP_TYPE ack_or_stall)
{
	//wanna respond with ACK??
	if(ack_or_stall == USB_ACK) {
		USB_SEND_TRACE("USB: status stage response with ACK", 
			RV_TRACE_LEVEL_DEBUG_LOW);
		//respond with ack

		// EP_NUM EP_Num = n
		// EP_NUM EP_Dir = 0
		// EP_NUM EP_Sel = 1
		// EP_NUM EP_Setup_Sel = 0
		USB_EP_NUM_SEL_RX_EP(ep_nr);

		//clear endpoint
		W2FC_CTRL = W2FC_CTRL_CLR_EP;
		//fifo enable
		W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;

		//reset EP_NUM reg
		// Write EP_NUM register: //
		// EP_NUM.EP_Num = 0      //
		// EP_NUM.EP_Dir = 0      //
		// EP_NUM.EP_Sel = 0      //
		// EP_NUM.Setup_Sel = 0   //
		W2FC_EP_NUM = W2FC_16BIT_RESET;
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name prepare_for_cntrl_write_stat_stage
 *
 * This function prepares the device for the status stage that is comming
 * if the data stage was executed successfully it will respons with an ACK
 * else it will stall the request
 *
 * For RX CONTROL transactions
 *
 * @param	ack_or_stall	indicating if the endpoint must be stalled or ACKED
 *
 * @param   ep_nr			the endpoint that should be prepared
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void prepare_for_cntrl_write_stat_stage(UINT8 ep_nr, T_USB_RESP_TYPE ack_or_stall)
{
	// Application specific actions to determine Control Write Action and Status result //

	// Want to repond with ACK? -> Yes //
	if(ack_or_stall == USB_ACK) {
		// Command specific actions //
		if(USB_REQUEST != INVALID_REQUEST)	// check if request is valid ?
		{
			if(det_req_spec_action() != USB_OK) {
				/*didn't succeed in responding as required to request*/
				USB_SEND_TRACE("USB: not responded to request as required", 
					RV_TRACE_LEVEL_DEBUG_LOW);
			}
		}

		// Write EP_NUM register: //
		// EP_NUM.EP_Num = n      //
		// EP_NUM.EP_Dir = 1      //
		// EP_NUM.EP_Sel = 1      //
		// EP_NUM.Setup_Sel = 0   //
		USB_EP_NUM_SEL_TX_EP(ep_nr);

		// Set CTRL.Clr_EP to '1', then set CTRL.Set_FIFO_En to '1' //
		W2FC_CTRL = W2FC_CTRL_CLR_EP;
		W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;

		// Write EP_NUM register: //
		// EP_NUM.EP_Num = n      //
		// EP_NUM.EP_Dir = 1      //
		// EP_NUM.EP_Sel = 0      //
		// EP_NUM.Setup_Sel = 0   //
		USB_EP_NUM_TX_RELEASE(ep_nr);
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name stall_req_and_ep
 *
 * This function prepares the device hardware for returning stall prelies to the USB
 * HOST on the next transaction on this endpoint.
 *
 * @param	direction       Boolean indicating to stall RX or TX endpoint
 *
 * @param   phys_ep_nr		The physical endpoint number of the endpoint that should 
 *							be prepared
 *
 * @return	T_RV_RET	
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET stall_req_and_ep(UINT8 phys_ep_nr, BOOL direction)
{
	if(direction == USB_RX) {
		USB_SEND_TRACE("USB: rx ep stalled ", 
			RV_TRACE_LEVEL_DEBUG_LOW);
		//set halt condition
		W2FC_CTRL =W2FC_CTRL_SET_HALT;
		USB_EP_NUM_RX_RELEASE(phys_ep_nr);
	}
	else {
		USB_SEND_TRACE("USB: tx ep stalled", 
			RV_TRACE_LEVEL_DEBUG_LOW);
		//sethalt condition
		W2FC_CTRL = W2FC_CTRL_SET_HALT;
		USB_EP_NUM_TX_RELEASE(phys_ep_nr);
	}
	return(USB_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name send_conf_descr
 *
 * This function sends configuration descriptor
 *
 * This function reserves memory, fills it with approprate data
 * copies it to the endpoints fifo and sends it to the USB host
 * For only a single configuration descriptor and for the whole
 * configuration with all therewith belonging descriptors
 *
 * @param	nr_bytes_in_buf number of bytes that the decriptor exists off that are
 *							in the buffer waiting to be send
 *
 * @param   descr_index			Index in descriptor table
 *
 * @param   placed_in_fifo_p    contains the number of bytes actually written in 
 *								the fifo
 *
 * @return	T_RV_RET		If successfully executed USB_OK else USB_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static T_RV_RET send_conf_descr(UINT16* nr_bytes_in_buf, 
								UINT8 descr_index, 
								UINT16* placed_in_fifo_p)
{
	T_RV_RET ret = USB_OK;
	UINT8*	support_p;	//used to store the temporary buffer pointer
	UINT8	if_ctr;
	UINT8	ep_ctr, i;
	T_USB_ENDPOINT_DESCR temp_ep;
	UINT8	ep_rx_cntr = 1;
	UINT8	ep_tx_cntr = 1;

	//all descriptors of this configuration are requested
	//create buffer of required size 
	USB_SEND_TRACE("USB: all descriptors are requested", 
		RV_TRACE_LEVEL_DEBUG_LOW);

	//check if wlength isn't larger as the total length for this descriptor
	if(USB_REQUEST_LENGTH >
		usb_conf_descr_table[descr_index].wTotalLength) {
		USB_REQUEST_LENGTH =
			usb_conf_descr_table[descr_index].wTotalLength;
	}
	USB_TRACE_WARNING_PARAM("USB_REQUEST_LENGTH", USB_REQUEST_LENGTH);
	
	//nr of transactions needed to send this descriptor
	calc_no_trsf();
	
	USB_TRACE_WARNING_PARAM("USB: usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req",
		usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req);
	
	if(get_setup_data_mem(&usb_env_ctrl_blk_p->host_data.ep0_data_p, 
		usb_conf_descr_table[descr_index].wTotalLength) == USB_OK) 	{
		USB_TRACE_WARNING_PARAM("USB: usb_conf_descr_table[descr_index].wTotalLength", 
			usb_conf_descr_table[descr_index].wTotalLength);
		//fill it with data
		support_p = usb_env_ctrl_blk_p->host_data.ep0_data_p;
	
		//step 1 configuration descriptor
		memcpy(support_p, configurations_table[descr_index].if_cfg_descr_p, 
				USB_CNF_DESCR_LNT);
	
		//add size to support pointer
		support_p += USB_CNF_DESCR_LNT;
		
		// Check if a IAD descriptor is associated to this configuration
		if(configurations_table[descr_index].cfg_iad_descr_p != NULL)
		{
			memcpy(support_p, configurations_table[descr_index].cfg_iad_descr_p, 
				USB_IAD_DESCR_LNT);
			
			//add size to support pointer
			support_p += USB_IAD_DESCR_LNT;
		}

		//step 2a interface descr 1
		//step 2b interface 1 enpoint descr 1 to n
		//step 3a interface descr 2
		//step 3b interface 2 enpoint descr 1 to n
		//etc.
		for(if_ctr = 0; 
			if_ctr < configurations_table[descr_index].if_cfg_descr_p->bNumInterfaces; 
			if_ctr++) {
			//copy interface descriptor into mem
			memcpy(support_p, configurations_table[descr_index].if_cnfg_p[if_ctr].ifdescr_p, 
					USB_IF_LNT);
			//add size to support pointer
			support_p += USB_IF_LNT;

			// Check if a CDC descriptor is present in this interface
			if(configurations_table[descr_index].if_cnfg_p[if_ctr].if_cdc_descr_p != NULL)
			{
				const T_USB_CDC_DESCR	*current_cdc_descritor = 
								configurations_table[descr_index].if_cnfg_p[if_ctr].if_cdc_descr_p;

				fill_cdc_descr(current_cdc_descritor, support_p);
				//add size to support pointer
				support_p += USB_CDC_DESCR_LNT;
			}

			//copy endpoint descriptors of this interface into mem
			for(ep_ctr = 0; 
				ep_ctr < configurations_table[descr_index].if_cnfg_p[if_ctr].ifdescr_p->bNumEndpoints; 
				ep_ctr++) { 
				//fill descriptor with data
				fill_ep_descr(&temp_ep, configurations_table[descr_index].
										if_cnfg_p[if_ctr].if_logical_ep_array_p[ep_ctr]);

				if(temp_ep.bEndpointAddress == USB_OUT) {
					//rx endpoint
					temp_ep.bEndpointAddress |= ep_rx_cntr;
					ep_rx_cntr++;
				} else {
					//tx endpoint
					temp_ep.bEndpointAddress |= ep_tx_cntr;
					ep_tx_cntr++;
				}
				//copy endpoint descriptor into mem
				memcpy(support_p, &temp_ep,	USB_EP_DESCR_LGNT);
				//add size to support_p
				support_p += USB_EP_DESCR_LGNT;
			}
		}

		*nr_bytes_in_buf = USB_REQUEST_LENGTH;
		
		//write non-iso TX FIFO data
		USB_SEND_TRACE("USB: start write data ",
			RV_TRACE_LEVEL_DEBUG_LOW);
		write_non_iso_tx_fifo_data(USB_REQUEST_LENGTH, 
									USB_ENDP0_SIZE, 
									placed_in_fifo_p, 
									usb_env_ctrl_blk_p->host_data.ep0_data_p);
		
		if((USB_REQUEST_LENGTH - 
			(*placed_in_fifo_p) == 0) || 
			((*placed_in_fifo_p) == USB_ENDP0_SIZE)) {
			//succesfully wrote data in fifo
			//write finished
			USB_SEND_TRACE("USB: apparently successfully written in fifo",
				RV_TRACE_LEVEL_DEBUG_LOW);
		} else {
			//data not succesfully written into fifo
			USB_SEND_TRACE("USB: data not sucessfully written into fifo", 
				RV_TRACE_LEVEL_ERROR);
			USB_SEND_TRACE("USB: get_descriptor data not successfully written", 
				RV_TRACE_LEVEL_ERROR);
			abort_setup_req(WITH_CMD_STALL);
		}
	} else {
		/*there is no memory available to store setup data in*/
		USB_SEND_TRACE("USB: memory to store data in is NOT available, send STALL CMD", 
			RV_TRACE_LEVEL_ERROR);
		/*we don't want to wait untill memory is available, just quit*/
		abort_setup_req(WITH_CMD_STALL);
	}
	return(ret);
} 
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name fill_cdc_descr
 *
 * This function fill a buffer given in parameter with the CDC configuration
 *
 * @param	cdc_descr	pointer to a buffer to fill
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static T_RV_RET fill_cdc_descr(const T_USB_CDC_DESCR	*current_cdc_descritor, UINT8* cdc_descr_buffer)
{
	T_RV_RET ret = USB_OK;

	/* Functional Header description */
	*cdc_descr_buffer = current_cdc_descritor->header.bLength;
	cdc_descr_buffer++;

	*cdc_descr_buffer = current_cdc_descritor->header.bDescriptorType;
	cdc_descr_buffer++;

	*cdc_descr_buffer = current_cdc_descritor->header.bDescriptorSubType;
	cdc_descr_buffer++;

	*cdc_descr_buffer = current_cdc_descritor->header.bCDC;
	cdc_descr_buffer += 1;

	*cdc_descr_buffer = (current_cdc_descritor->header.bCDC >> 8);
	cdc_descr_buffer += 1;
	/* End of filling header descriptor */

	/* Functional ACM Header description */
	*cdc_descr_buffer = current_cdc_descritor->acm_descriptor.bLength;
	cdc_descr_buffer++;

	*cdc_descr_buffer = current_cdc_descritor->acm_descriptor.bDescriptorType;
	cdc_descr_buffer++;

	*cdc_descr_buffer = current_cdc_descritor->acm_descriptor.bDescriptorSubType;
	cdc_descr_buffer++;

	*cdc_descr_buffer = current_cdc_descritor->acm_descriptor.bmCapabilities;
	cdc_descr_buffer++;
	/* End of filling ACM Header description */

	/* Functional ACM Header description */
	*cdc_descr_buffer = current_cdc_descritor->interface_union.bLength;
	cdc_descr_buffer++;

	*cdc_descr_buffer = current_cdc_descritor->interface_union.bDescriptorType;
	cdc_descr_buffer++;

	*cdc_descr_buffer = current_cdc_descritor->interface_union.bDescriptorSubType;
	cdc_descr_buffer++;

	*cdc_descr_buffer = current_cdc_descritor->interface_union.bMasterInterface;
	cdc_descr_buffer++;

	*cdc_descr_buffer = current_cdc_descritor->interface_union.bSlaveInterface0;
	cdc_descr_buffer++;
	/* End of filling ACM Header description */

	return(ret);
}

/**
 *---------------------------------------------------------------------------------* 
 * @name clear_reset_tx_ep
 *
 * This function resets the hardware TX endpoint
 *
 * @param	ep_nr		the physical endpoint that must be reset
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void clear_reset_tx_ep(UINT8 ep_nr)
{
	USB_EP_NUM_SEL_TX_EP(ep_nr);
	W2FC_CTRL = W2FC_CTRL_CLR_EP;
	W2FC_CTRL = W2FC_CTRL_RESET_EP;
	USB_EP_NUM_TX_RELEASE(ep_nr);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name clear_reset_rx_ep
 *
 * This function resets the hardware RX endpoint
 *
 * @param	ep_nr		the physical endpoint that must be reset
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void clear_reset_rx_ep(UINT8 ep_nr)
{
	USB_EP_NUM_SEL_RX_EP(ep_nr);
	W2FC_CTRL = W2FC_CTRL_CLR_EP;
	W2FC_CTRL = W2FC_CTRL_RESET_EP;
	USB_EP_NUM_RX_RELEASE(ep_nr);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name clear_setup
 *
 * This function reset the request pointer containing request data
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void clear_setup_buf(void)
{
	USB_SEND_TRACE("USB: clear setup buffer", RV_TRACE_LEVEL_DEBUG_LOW);
	//fill buffer with unvalid data 
	usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->bmRequestType = 0xFF;
	usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->bRequest = INVALID_REQUEST;  
	usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wValue = 0xFFFF;
	usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wIndex = 0xFFFF;
	usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wLength = 0xFFFF;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name store_req
 *
 * This function stores the current (vendor)request specific function that is 
 * currently being processed.
 *
 * @param	dummyfunc		Boolean that indicates if a serious function must be 
 *							stored, or a fake one, because no reuest is currently 
 *							being processed
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void store_req(BOOL dummyfnc)
{
	if(dummyfnc == TRUE) {
		usb_env_ctrl_blk_p->host_data.ep0_setup.current_setup_req = dummy;
	} else {
		usb_env_ctrl_blk_p->host_data.ep0_setup.current_setup_req = 
			setup_stnd_fnc_table[usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->bRequest];
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name store_specific_ep0_handler
 *
 * This function stores a callback on ep0 transfer tx or rx
 * in case of class or specific request
 *
 * @param	ep0_operation
 *							
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void store_specific_ep0_handler(T_SPECIFIC_EP_OPERATION ep0_operation)
{
	switch(ep0_operation)
	{
		case USB_CDC_EP0_TX :
			usb_env_ctrl_blk_p->host_data.ep0_setup.current_setup_req = specific_ep0_tx_handler;
		break;

		case USB_CDC_EP0_RX :
			usb_env_ctrl_blk_p->host_data.ep0_setup.current_setup_req = specific_ep0_rx_handler;
		break;

		case USB_CDC_EP0_NOOP :
			usb_env_ctrl_blk_p->host_data.ep0_setup.current_setup_req = dummy;
		break;
		
		default:
			usb_env_ctrl_blk_p->host_data.ep0_setup.current_setup_req = dummy;
		break;
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name calc_no_trsf
 *
 * This function is called at the beginning of a setup request, to see how many 
 * transfers are needed to complete the request.
 *
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void calc_no_trsf(void)
{
	//nr off processed tranfered is still 0 here
	usb_env_ctrl_blk_p->host_data.ep0_setup.nr_of_trsf_proc = 0;
	//it is larger so a transaction is needed to transmit the rest
	usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req = 
		(USB_REQUEST_LENGTH / USB_ENDP0_SIZE);
	//if dividing has a rest value Mod != 0 increment nunber of required requests
	// previous condition was 
	// if((USB_REQUEST_LENGTH % USB_ENDP0_SIZE) >= 0)
	// Changed to !=
	if((USB_REQUEST_LENGTH % USB_ENDP0_SIZE) != 0)
	{
		usb_env_ctrl_blk_p->host_data.ep0_setup.total_nr_trsf_req += 1;
	}
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name det_fifo_full_db
 *
 * This function determines if the specific endpoint is double buffered 
 *
 * @param	pep_nr			physical endpoint number (CAUTION not the physical 
 *							endpointnumber index in the xref table)
 *							being processed
 * @param   ep_buf_size     variable into which the endpoints buffer size will be 
 *							stored
 * @param	rx_tx			TRUE when dealing with and RX endpoint FALSE when 
 *							dealing with an TX endpoint
 * @return	BOOL			TRUE when endpoint is double buffered
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
BOOL det_fifo_full_db(UINT16 pep_nr, UINT16* ep_buf_size, BOOL rx_tx)
{
	T_USB_ENDPOINT_DESCR temp_ep;
	UINT8 phys_ep;
	UINT8 cur_if;
	UINT8 cur_lep;
	UINT16 cntr;
	T_USB_CROSS_REF* tmp_xref_tbl;
	UINT16 noep;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	UINT8 i;
#endif

	if(rx_tx == TRUE) {
		//searching the rx table
		tmp_xref_tbl = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl;
		noep = usb_env_ctrl_blk_p->cnfg.nof_rx_ep;
	} else {
		//searching the tx table
		tmp_xref_tbl = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl;
		noep = usb_env_ctrl_blk_p->cnfg.nof_tx_ep;
	}
	
	/* Add the endpoint 0 in the sum of endpoints */
	noep = noep + 1;

	USB_TRACE_WARNING_PARAM("USB: current_cnfg_nr:", 
			USB_CNFGNR);
	if((USB_CNFGNR != USB_CONF_DEFAULT) && 
		(USB_CNFGNR != USB_CONF_ADRESSED)) {
		USB_SEND_TRACE("USB: valid conf, so endpoint will be read", 
			RV_TRACE_LEVEL_DEBUG_LOW);
		//there is a valid configuration
		if((pep_nr < noep) && (pep_nr != USB_DEFAULT_EP)) {
			// EPn_RX.EPn_RX_Size or DB=1? //
			cur_lep = PEP_2_LEP(pep_nr, rx_tx);
			cur_if = tmp_xref_tbl[USB_PEP_INDX(pep_nr)].interface_data_p->interface_number;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
			for(i=0;i<USB_TOTAL_DIFF_IF;i++)
			{
				if(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_number == cur_if
					&& usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_enabled == TRUE)
					break;
			}			
#endif
			if(fill_ep_descr(&temp_ep, 
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
								configurations_table[USB_CNFGNR_2INDX].if_cnfg_p[i].
#else
								configurations_table[USB_CNFGNR_2INDX].if_cnfg_p[cur_if].
#endif
								if_logical_ep_array_p[USB_LEP_INDX(cur_lep)]
							) == TRUE) {
				//DB is set
				//return endpoint buffer size
				*ep_buf_size = temp_ep.wMaxPacketSize;
				return(TRUE);
			} else {
				//return endpoint buffer size
				*ep_buf_size = temp_ep.wMaxPacketSize;
				return(FALSE);
			}
		} else {
			USB_SEND_TRACE("USB: wrong conf, or wrong logical ep", 
				RV_TRACE_LEVEL_DEBUG_LOW);
			usb_env_ctrl_blk_p->host_data.fifo_not_full = FALSE;
		}
	} else {
		USB_SEND_TRACE("USB: no valid conf, so endpoint can't be read", 
			RV_TRACE_LEVEL_DEBUG_LOW);
		usb_env_ctrl_blk_p->host_data.fifo_not_full = FALSE;
	}

	return (RV_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name read_non_iso_packet_protected
 *
 * This function reads a non ISO packet send by the USB HOST never more than the
 * size of the fifo of the currently selected endpoint
 *
 * @param	phys_ep_nr		physical endpoint number (CAUTION not the physical
 *							endpointnumber index in the xref table)
 *
 * @return	T_RV_RET		USB_OK when successfully executed else USN_INTERNAL_ERR
 *---------------------------------------------------------------------------------*
 */
/*@{*/
T_RV_RET read_non_iso_packet_protected(UINT8 phys_ep_nr)
{
	UINT16 bytes_from_fifo;
	UINT16 size;
	T_RV_RET ret = USB_OK;

	F_INTH_DISABLE_ONE_IT(C_INTH_USB_IT);
	USB_EP_NUM_SEL_RX_EP(phys_ep_nr);
	if((W2FC_STAT_FLG & W2FC_STAT_FLG_NON_ISO_FIFO_FULL) != W2FC_STAT_FLG_NON_ISO_FIFO_FULL) {
		size = W2FC_RXFSTAT;
	} else {
		size = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].
			endpoint_data.endpo_descr_p->wMaxPacketSize;
	}
	USB_EP_NUM_RX_RELEASE(phys_ep_nr);

	//buffer integrity check
	if(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p == NULL) {
		//no valid buffer is provided yet
		// stall_req_and_ep(phys_ep_nr, USB_RX);
		//set endpoint stalled flag
		usb_env_ctrl_blk_p->bm_rx_ep_buf_stall |= (1 << phys_ep_nr);
		USB_SEND_TRACE("USB: no buffer to write data in",
			RV_TRACE_LEVEL_ERROR);
	} else {
		//check if temporary buffer pointer + new data size doesn't exceed
		//the original buffer pointer + total buffer size
		if((usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p + size) >
			(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p +
			usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_size)) {
			//buffer is to small to contain new data-> stall ep send buff full msg to FM
			//and wait untill new buffer is provided. (ITS BETTER TO PROVIDE BUFFERS
			//THAT ARE A MULTIPLE OF THE EP SIZE)
			//IN transaction on EP0 was stalled
			USB_EP_NUM_SEL_RX_EP(phys_ep_nr);
			stall_req_and_ep(phys_ep_nr, USB_RX);
			USB_EP_NUM_RX_RELEASE(phys_ep_nr);

			//set endpoint stalled flag
			usb_env_ctrl_blk_p->bm_rx_ep_buf_stall |= (1 << phys_ep_nr);
			//determine size that is written so far
			size = (UINT16)(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p -
						usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p);
			//send rx buff full message
			notify_rx_buffer_full(USB_PEP_INDX(phys_ep_nr), size, USB_NO_EOT);
			USB_SEND_TRACE("USB: buffer to write data in to small",
					RV_TRACE_LEVEL_ERROR);
		} else {
			//check if stall flag isn't set
			if((usb_env_ctrl_blk_p->bm_rx_ep_buf_stall &
				(1 << phys_ep_nr)) == (1 << phys_ep_nr)) {
				//no valid buffer is provided yet
				USB_EP_NUM_SEL_RX_EP(phys_ep_nr);
				stall_req_and_ep(phys_ep_nr, USB_RX);
				USB_EP_NUM_RX_RELEASE(phys_ep_nr);

				USB_SEND_TRACE("USB: stall flag for endpoint is still set",
					RV_TRACE_LEVEL_ERROR);
			} else {
				USB_EP_NUM_SEL_RX_EP(phys_ep_nr);
				// Read Non-ISO packet from RX FIFO data //
				if(read_non_iso_rx_fifo_data(phys_ep_nr,
											&bytes_from_fifo,
											usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].
											temp_buf_p) != USB_OK) {
					ret = USB_INTERNAL_ERR;
					USB_SEND_TRACE("USB: read_non_iso_tx_fifo_data failed",
						RV_TRACE_LEVEL_ERROR);
				}
				USB_EP_NUM_RX_RELEASE(phys_ep_nr);

				if(bytes_from_fifo == size) {
					//1 more transfer processed
					usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p += size;

					if((usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p) ==
						(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p +
						usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_size)) {
						//send rx buffer full msg
						notify_rx_buffer_full(USB_PEP_INDX(phys_ep_nr),
											usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_size,
											USB_NO_EOT);
					} else {
						if(size < usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].
							endpoint_data.endpo_descr_p->wMaxPacketSize) {
							//determine size that is written so far
							size = (UINT16)(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p -
									usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p);
							//send rx buffer full msg
							notify_rx_buffer_full(USB_PEP_INDX(phys_ep_nr), size, USB_EOT);
						}
					}

					USB_EP_NUM_SEL_RX_EP(phys_ep_nr);
					//if fifo not full or DB flag is set endpoint must be enabled later to enable both
					if(usb_env_ctrl_blk_p->host_data.fifo_not_full != TRUE) {
						// Ready to handle another RX Transaction and flag FIFO not full and DB=0? //
						// Write CTRL register: CTRL.Set_FIFO_En=1 //
						W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
					}
					USB_EP_NUM_RX_RELEASE(phys_ep_nr);

					USB_SEND_TRACE("USB: read rx data from fifo succesfull",
						RV_TRACE_LEVEL_DEBUG_LOW);
				} else {
					ret = USB_INTERNAL_ERR;
					USB_EP_NUM_SEL_RX_EP(phys_ep_nr);
					stall_req_and_ep(phys_ep_nr, USB_RX);
					USB_EP_NUM_RX_RELEASE(phys_ep_nr);

					USB_SEND_TRACE("USB: read data from fifo failed",
						RV_TRACE_LEVEL_ERROR);
				}
			}
		}
	}
	F_INTH_ENABLE_ONE_IT(C_INTH_USB_IT);
	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------*
 * @name read_non_iso_packet
 *
 * This function reads a non ISO packet send by the USB HOST never more than the 
 * size of the fifo of the currently selected endpoint
 *
 * @param	phys_ep_nr		physical endpoint number (CAUTION not the physical 
 *							endpointnumber index in the xref table)
 *							 
 * @return	T_RV_RET		USB_OK when successfully executed else USN_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET read_non_iso_packet(UINT8 phys_ep_nr)
{
	UINT16 bytes_from_fifo;
	UINT16 size;
	T_RV_RET ret = USB_OK;

	if((W2FC_STAT_FLG & W2FC_STAT_FLG_NON_ISO_FIFO_FULL) != W2FC_STAT_FLG_NON_ISO_FIFO_FULL) {
		size = W2FC_RXFSTAT;
	} else {
		size = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].
			endpoint_data.endpo_descr_p->wMaxPacketSize;
	}
	//buffer integrity check
	if(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p == NULL) {
		//no valid buffer is provided yet	
		// stall_req_and_ep(phys_ep_nr, USB_RX);
		//set endpoint stalled flag
		usb_env_ctrl_blk_p->bm_rx_ep_buf_stall |= (1 << phys_ep_nr);	
		USB_SEND_TRACE("USB: no buffer to write data in",
			RV_TRACE_LEVEL_ERROR);
	} else {
		//check if temporary buffer pointer + new data size doesn't exceed 
		//the original buffer pointer + total buffer size 
		if((usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p + size) > 
			(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p + 
			usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_size)) {
			//buffer is to small to contain new data-> stall ep send buff full msg to FM
			//and wait untill new buffer is provided. (ITS BETTER TO PROVIDE BUFFERS 
			//THAT ARE A MULTIPLE OF THE EP SIZE)
			//IN transaction on EP0 was stalled
			stall_req_and_ep(phys_ep_nr, USB_RX);
			//set endpoint stalled flag
			usb_env_ctrl_blk_p->bm_rx_ep_buf_stall |= (1 << phys_ep_nr);
			//determine size that is written so far
			size = (UINT16)(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p - 
						usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p);
			//send rx buff full message
			notify_rx_buffer_full(USB_PEP_INDX(phys_ep_nr), size, USB_NO_EOT);
			USB_SEND_TRACE("USB: buffer to write data in to small",
					RV_TRACE_LEVEL_ERROR);
		} else {
			//check if stall flag isn't set
			if((usb_env_ctrl_blk_p->bm_rx_ep_buf_stall & 
				(1 << phys_ep_nr)) == (1 << phys_ep_nr)) {
				//no valid buffer is provided yet	
				stall_req_and_ep(phys_ep_nr, USB_RX);
				USB_SEND_TRACE("USB: stall flag for endpoint is still set",
					RV_TRACE_LEVEL_ERROR);
			} else {
				// Read Non-ISO packet from RX FIFO data //
				if(read_non_iso_rx_fifo_data(phys_ep_nr,
											&bytes_from_fifo, 
											usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].
											temp_buf_p) != USB_OK) {
					ret = USB_INTERNAL_ERR;
					USB_SEND_TRACE("USB: read_non_iso_tx_fifo_data failed",
						RV_TRACE_LEVEL_ERROR);
				}
				if(bytes_from_fifo == size) {   
					//1 more transfer processed
					usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p += size;

					if((usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p) == 
						(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p + 
						usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_size)) {
						//send rx buffer full msg
						notify_rx_buffer_full(USB_PEP_INDX(phys_ep_nr), 
											usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_size, 
											USB_NO_EOT);
					} else {
						if(size < usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].
							endpoint_data.endpo_descr_p->wMaxPacketSize) {
							//determine size that is written so far
							size = (UINT16)(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p - 
									usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p);
							//send rx buffer full msg
							notify_rx_buffer_full(USB_PEP_INDX(phys_ep_nr), size, USB_EOT);
						}
						else
						{
							UINT32 maxPacketSize = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].endpoint_data.endpo_descr_p->wMaxPacketSize;
							if((usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p+maxPacketSize) > 
									(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p + 
									usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_size))
							{
								size = (UINT16)(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p - 
									usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p);
								//send rx buffer full msg
								notify_rx_buffer_full(USB_PEP_INDX(phys_ep_nr), size, USB_EOT);							
							}
							else
							{
								// Write CTRL register: CTRL.Set_FIFO_En=1 //
								W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
							}
					}
					}
					USB_SEND_TRACE("USB: read rx data from fifo succesfull",
						RV_TRACE_LEVEL_DEBUG_LOW);
				} else {
					ret = USB_INTERNAL_ERR;
					stall_req_and_ep(phys_ep_nr, USB_RX);
					USB_SEND_TRACE("USB: read data from fifo failed",
						RV_TRACE_LEVEL_ERROR);
				}
			}
		}
	}
	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name write_non_iso_packet
 *
 * This function writes a non ISO packet to the USB HOST never more than the 
 * size of the fifo of the currently selected endpoint
 *
 * @param	phys_ep_idx		physical endpoint index in TX xref table (CAUTION not 
 *							the physical endpointnumber itself)
 *							 
 * @return	T_RV_RET		USB_OK when successfully executed else USN_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/


T_RV_RET write_non_iso_packet(UINT8 phys_ep_idx)
{
	T_USB_CROSS_REF *temp_xref_p;
	UINT16 size;
	UINT16 bytes_from_fifo;
	T_RV_RET ret = USB_OK;
	BOOL transmit = FALSE;
	BOOL int_error = FALSE;
	UINT8 phys_ep_nr = phys_ep_idx + 1;

	//buffer integrity check
	temp_xref_p = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl;

	if(temp_xref_p[phys_ep_idx].temp_buf_p == NULL) 
	{
		//no valid buffer is provided yet	
		stall_req_and_ep(phys_ep_nr+1, USB_TX);
		//set endpoint stalled flag
		usb_env_ctrl_blk_p->bm_tx_ep_buf_stall |= (1 << phys_ep_nr);	
		USB_SEND_TRACE("USB: no buffer to read tx data from",
			RV_TRACE_LEVEL_ERROR);
	} 
	else 
	{
		//check if temporary buffer pointer doesn't exceed 
		//the original buffer pointer + total buffer size 
		if(temp_xref_p[phys_ep_idx].buf_size % 
			temp_xref_p[phys_ep_idx].endpoint_data.endpo_descr_p->wMaxPacketSize == 0) 
		{
			//the buffer size is equal to or a multiple of the physical endpoint buffer size
			if((temp_xref_p[phys_ep_idx].temp_buf_p) == 
				(temp_xref_p[phys_ep_idx].buf_p + temp_xref_p[phys_ep_idx].buf_size)) 
			{
				if(temp_xref_p[phys_ep_idx].shorter_transfer == TRUE) 
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : shorter_transfer", RV_TRACE_LEVEL_DEBUG_LOW);

					//All data from buffer is transmitted but an empty packet has to be send
					transmit = TRUE;
					//after we send the empty packet this value will be checked again and if 
					//it is still TRUE we will remain in a deadloop therefore --> FALSE
					temp_xref_p[phys_ep_idx].shorter_transfer = FALSE;
				} 
				else 
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : Notifify TX buffer empty (case 1)", RV_TRACE_LEVEL_DEBUG_LOW);

					//All data is send and we're finished send notify
					/* stall_req_and_ep(phys_ep_nr, USB_TX); */
					/* usb_env_ctrl_blk_p->bm_tx_ep_buf_stall |= (1 << phys_ep_nr); */

					notify_tx_buffer_empty(phys_ep_idx);
				}
			} 
			else 
			{
				if((temp_xref_p[phys_ep_idx].temp_buf_p) >
				(temp_xref_p[phys_ep_idx].buf_p + temp_xref_p[phys_ep_idx].buf_size)) 
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : buff size error (case 1)", RV_TRACE_LEVEL_DEBUG_LOW);
				
					//ERROR 
					int_error = TRUE;
				} 
				else 
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : ask to transmit (case 1)", RV_TRACE_LEVEL_DEBUG_LOW);

					//Data still has to be transmitted
					transmit = TRUE;
				}
			}
		} 
		else 
		{
			if((temp_xref_p[phys_ep_idx].temp_buf_p) == 
				(temp_xref_p[phys_ep_idx].buf_p + temp_xref_p[phys_ep_idx].buf_size)) 
			{
				USB_SEND_TRACE("USB::write_non_iso_packet : Notifify TX buffer empty (case 2)", 
					RV_TRACE_LEVEL_DEBUG_LOW);

				//All data is send and we're finished send notify
				/* stall_req_and_ep(phys_ep_nr, USB_TX); */
				notify_tx_buffer_empty(phys_ep_idx);
				/* usb_env_ctrl_blk_p->bm_tx_ep_buf_stall |= (1 << phys_ep_nr); */
			} 
			else 
			{
				if((temp_xref_p[phys_ep_idx].temp_buf_p) >
				(temp_xref_p[phys_ep_idx].buf_p + temp_xref_p[phys_ep_idx].buf_size)) 
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : buff size error (case 2)", RV_TRACE_LEVEL_DEBUG_LOW);

					//ERROR
					int_error = TRUE;
				} 
				else 
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : ask to transmit (case 2)", RV_TRACE_LEVEL_DEBUG_LOW);

					//Data still has to be transmitted
					transmit = TRUE;
				}
			}
		}
		if(int_error) 
		{
			stall_req_and_ep(phys_ep_nr, USB_TX);
			//set endpoint stalled flag
			usb_env_ctrl_blk_p->bm_rx_ep_buf_stall |= (1 << phys_ep_nr);
			//send tx buff empty message
			notify_tx_buffer_empty(phys_ep_idx);
			USB_SEND_TRACE("USB: trying to write data from invalid buffer",
					RV_TRACE_LEVEL_ERROR);
			ret = USB_INTERNAL_ERR;
		}

		if(transmit) 
		{
			USB_SEND_TRACE("USB::write_non_iso_packet : transmit", RV_TRACE_LEVEL_DEBUG_LOW);

			//check if stall flag isn't set
			if((usb_env_ctrl_blk_p->bm_tx_ep_buf_stall & 
				(1 << phys_ep_nr)) == (1 << phys_ep_nr)) 
			{
				//no valid buffer is provided yet	
				stall_req_and_ep(phys_ep_nr, USB_TX);
				USB_SEND_TRACE("USB: stall flag for tx endpoint is still set",
					RV_TRACE_LEVEL_ERROR);
			} else 
			{
				// WRITE Non-ISO packet to TX FIFO data //
				if(temp_xref_p[phys_ep_idx].temp_buf_p == 
					(temp_xref_p[phys_ep_idx].buf_p + temp_xref_p[phys_ep_idx].buf_size)) 
				{
					size = 0; //shorter_packet must been set to 1 here thus we have to send empty packet
							  //to notify End Of Transfer to HOST
				} else 
				{
					size = (temp_xref_p[phys_ep_idx].buf_size - 
						(UINT16)(temp_xref_p[phys_ep_idx].temp_buf_p - 
						temp_xref_p[phys_ep_idx].buf_p));
				}

				if(write_non_iso_tx_fifo_data(size,
											temp_xref_p[phys_ep_idx].endpoint_data.endpo_descr_p->wMaxPacketSize, 
											&bytes_from_fifo,
											temp_xref_p[phys_ep_idx].temp_buf_p) != USB_OK) 
				{
					ret = USB_INTERNAL_ERR;
					USB_SEND_TRACE("USB: write_non_iso_tx_fifo_data failed",
						RV_TRACE_LEVEL_ERROR);
				}
				if((bytes_from_fifo == size) || 
					(bytes_from_fifo == temp_xref_p[phys_ep_idx].endpoint_data.endpo_descr_p->wMaxPacketSize)) 
				{   
					//1 more transfer processed
					temp_xref_p[phys_ep_idx].temp_buf_p += bytes_from_fifo;
				
					USB_SEND_TRACE("USB: write tx data to fifo succesfull",
						RV_TRACE_LEVEL_DEBUG_LOW);
					USB_TRACE_WARNING_PARAM("USB: bytes_from_fifo :", bytes_from_fifo);

					// Set CTRL.Set_FIFO_En to '1' //
					W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;

				} 
				else 
				{
					ret = USB_INTERNAL_ERR;
					stall_req_and_ep(phys_ep_nr, USB_TX);
					USB_SEND_TRACE("USB: write data to fifo failed",
						RV_TRACE_LEVEL_ERROR);
				}
			}
		}
	}
	return(ret);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name write_non_iso_packet_protected
 *
 * This function writes a non ISO packet to the USB HOST never more than the
 * size of the fifo of the currently selected endpoint
 *
 * @param	phys_ep_idx		physical endpoint index in TX xref table (CAUTION not
 *							the physical endpointnumber itself)
 *
 * @return	T_RV_RET		USB_OK when successfully executed else USN_INTERNAL_ERR
 *---------------------------------------------------------------------------------*
 */
/*@{*/
T_RV_RET write_non_iso_packet_protected(UINT8 phys_ep_idx)
{
	T_USB_CROSS_REF *temp_xref_p;
	UINT16 size;
	UINT16 bytes_from_fifo;
	T_RV_RET ret = USB_OK;
	BOOL transmit = FALSE;
	BOOL int_error = FALSE;
	UINT8 phys_ep_nr = phys_ep_idx + 1;

	//buffer integrity check
	F_INTH_DISABLE_ONE_IT(C_INTH_USB_IT);
	temp_xref_p = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl;
	if(temp_xref_p[phys_ep_idx].temp_buf_p == NULL)
	{
		USB_EP_NUM_SEL_TX_EP(phys_ep_nr);
		//no valid buffer is provided yet
		stall_req_and_ep(phys_ep_nr, USB_TX);
		USB_EP_NUM_TX_RELEASE(phys_ep_nr );
		//set endpoint stalled flag
		usb_env_ctrl_blk_p->bm_tx_ep_buf_stall |= (1 << phys_ep_nr);
		USB_SEND_TRACE("USB: no buffer to read tx data from",
			RV_TRACE_LEVEL_ERROR);
	}
	else
	{
		//check if temporary buffer pointer doesn't exceed
		//the original buffer pointer + total buffer size
		if(temp_xref_p[phys_ep_idx].buf_size %
			temp_xref_p[phys_ep_idx].endpoint_data.endpo_descr_p->wMaxPacketSize == 0)
		{
			//the buffer size is equal to or a multiple of the physical endpoint buffer size
			if((temp_xref_p[phys_ep_idx].temp_buf_p) ==
				(temp_xref_p[phys_ep_idx].buf_p + temp_xref_p[phys_ep_idx].buf_size))
			{
				if(temp_xref_p[phys_ep_idx].shorter_transfer == TRUE)
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : shorter_transfer", RV_TRACE_LEVEL_DEBUG_LOW);

					//All data from buffer is transmitted but an empty packet has to be send
					transmit = TRUE;
					//after we send the empty packet this value will be checked again and if
					//it is still TRUE we will remain in a deadloop therefore --> FALSE
					temp_xref_p[phys_ep_idx].shorter_transfer = FALSE;
				}
				else
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : Notifify TX buffer empty (case 1)", RV_TRACE_LEVEL_DEBUG_LOW);

					//All data is send and we're finished send notify
					/* stall_req_and_ep(phys_ep_nr, USB_TX); */
					/* usb_env_ctrl_blk_p->bm_tx_ep_buf_stall |= (1 << phys_ep_nr); */
					notify_tx_buffer_empty(phys_ep_idx);
				}
			}
			else
			{
				if((temp_xref_p[phys_ep_idx].temp_buf_p) >
				(temp_xref_p[phys_ep_idx].buf_p + temp_xref_p[phys_ep_idx].buf_size))
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : buff size error (case 1)", RV_TRACE_LEVEL_DEBUG_LOW);
					//ERROR
					int_error = TRUE;
				}
				else
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : ask to transmit (case 1)", RV_TRACE_LEVEL_DEBUG_LOW);

					//Data still has to be transmitted
					transmit = TRUE;
				}
			}
		}
		else
		{
			if((temp_xref_p[phys_ep_idx].temp_buf_p) ==
				(temp_xref_p[phys_ep_idx].buf_p + temp_xref_p[phys_ep_idx].buf_size))
			{
				USB_SEND_TRACE("USB::write_non_iso_packet : Notifify TX buffer empty (case 2)",
					RV_TRACE_LEVEL_DEBUG_LOW);

				//All data is send and we're finished send notify
				/* stall_req_and_ep(phys_ep_nr, USB_TX); */
				notify_tx_buffer_empty(phys_ep_idx);
				/* usb_env_ctrl_blk_p->bm_tx_ep_buf_stall |= (1 << phys_ep_nr); */
			}
			else
			{
				if((temp_xref_p[phys_ep_idx].temp_buf_p) >
				(temp_xref_p[phys_ep_idx].buf_p + temp_xref_p[phys_ep_idx].buf_size))
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : buff size error (case 2)", RV_TRACE_LEVEL_DEBUG_LOW);

					//ERROR
					int_error = TRUE;
				}
				else
				{
					USB_SEND_TRACE("USB::write_non_iso_packet : ask to transmit (case 2)", RV_TRACE_LEVEL_DEBUG_LOW);

					//Data still has to be transmitted
					transmit = TRUE;
				}
			}
		}
		if(int_error)
		{
			USB_EP_NUM_SEL_TX_EP(phys_ep_nr);
			stall_req_and_ep(phys_ep_nr, USB_TX);
			USB_EP_NUM_TX_RELEASE(phys_ep_nr );

			//set endpoint stalled flag
			usb_env_ctrl_blk_p->bm_rx_ep_buf_stall |= (1 << phys_ep_nr);
			//send tx buff empty message
			notify_tx_buffer_empty(phys_ep_idx);
			USB_SEND_TRACE("USB: trying to write data from invalid buffer",
					RV_TRACE_LEVEL_ERROR);
			ret = USB_INTERNAL_ERR;
		}

		if(transmit)
		{
			USB_SEND_TRACE("USB:write_non_iso_packet : transmit", RV_TRACE_LEVEL_DEBUG_LOW);

			//check if stall flag isn't set
			if((usb_env_ctrl_blk_p->bm_tx_ep_buf_stall &
				(1 << phys_ep_nr)) == (1 << phys_ep_nr))
			{
				USB_EP_NUM_SEL_TX_EP(phys_ep_nr);
				//no valid buffer is provided yet
				stall_req_and_ep(phys_ep_nr, USB_TX);
				USB_EP_NUM_TX_RELEASE(phys_ep_nr );

				USB_SEND_TRACE("USB: stall flag for tx endpoint is still set",
					RV_TRACE_LEVEL_ERROR);
			} else
			{
				// WRITE Non-ISO packet to TX FIFO data //
				if(temp_xref_p[phys_ep_idx].temp_buf_p ==
					(temp_xref_p[phys_ep_idx].buf_p + temp_xref_p[phys_ep_idx].buf_size))
				{
					size = 0; //shorter_packet must been set to 1 here thus we have to send empty packet
							  //to notify End Of Transfer to HOST
				} else
				{
					size = (temp_xref_p[phys_ep_idx].buf_size -
						(UINT16)(temp_xref_p[phys_ep_idx].temp_buf_p -
						temp_xref_p[phys_ep_idx].buf_p));
				}

				USB_EP_NUM_SEL_TX_EP(phys_ep_nr);
				if(write_non_iso_tx_fifo_data(size,
											temp_xref_p[phys_ep_idx].endpoint_data.endpo_descr_p->wMaxPacketSize,
											&bytes_from_fifo,
											temp_xref_p[phys_ep_idx].temp_buf_p) != USB_OK)
				{
					ret = USB_INTERNAL_ERR;
					USB_SEND_TRACE("USB: write_non_iso_tx_fifo_data failed",
						RV_TRACE_LEVEL_ERROR);
				}
				if((bytes_from_fifo == size) ||
					(bytes_from_fifo == temp_xref_p[phys_ep_idx].endpoint_data.endpo_descr_p->wMaxPacketSize))
				{
					//1 more transfer processed
					temp_xref_p[phys_ep_idx].temp_buf_p += bytes_from_fifo;

					USB_SEND_TRACE("USB: write tx data to fifo succesfull",
						RV_TRACE_LEVEL_DEBUG_LOW);
					USB_TRACE_WARNING_PARAM("USB: bytes_from_fifo :", bytes_from_fifo);

					// Set CTRL.Set_FIFO_En to '1' //
					W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
				}
				else
				{
					ret = USB_INTERNAL_ERR;
					stall_req_and_ep(phys_ep_nr, USB_TX);
					USB_SEND_TRACE("USB: write data to fifo failed",
						RV_TRACE_LEVEL_ERROR);
				}
				USB_EP_NUM_TX_RELEASE(phys_ep_nr );
			}
		}
	}
	F_INTH_ENABLE_ONE_IT(C_INTH_USB_IT);
	return(ret);
}
/*@}*/


/**
 *---------------------------------------------------------------------------------*
 * @name notify_tx_buffer_empty
 *
 * This function is called to send the function manager to which the endpoint 
 * belongs a message indicating that the provided tx buffer was read out end send
 * to the USB HOST, The function manager can than provide a new buffer with data 
 * to transmit
 *
 * @param	pep_idx			physical endpoint index in TX xref table (CAUTION not 
 *							the physical endpointnumber itself)
 *							 
 * @return	T_RV_RET		USB_OK when successfully executed else USN_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/

extern T_RVF_MUTEX usb_tx_mutex;

T_RV_RET notify_tx_buffer_empty(UINT32 pep_idx)
{
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	T_USB_RX_BUFFER_FULL_MSG *msg_p;
	T_RV_RETURN *rp;

	/* sanity check */
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	if (pep_idx > cnfg->nof_tx_ep) {
#else
	if (pep_idx >= cnfg->nof_tx_ep) {
#endif
		return RV_INTERNAL_ERR;
	}
	/* create message */
	if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
						sizeof (T_USB_TX_BUFFER_EMPTY_MSG),
						USB_TX_BUFFER_EMPTY_MSG,
						((T_RVF_MSG**) &msg_p))) {
		USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
		return RVM_MEMORY_ERR;
	}
	endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl + pep_idx;
	msg_p->endpoint = endpoint_vs_interface_entry_p->endpoint_data.endpoint_number;
	msg_p->interface = endpoint_vs_interface_entry_p->interface_data_p->interface_number;
	endpoint_vs_interface_entry_p->buf_p = NULL;
	endpoint_vs_interface_entry_p->temp_buf_p = NULL;
	rp = &endpoint_vs_interface_entry_p->interface_data_p->swe_return_path;


	
	USB_TRACE_WARNING_PARAM("USB: notify_tx_buffer_empty: "
				"sending USB_TX_BUFFER_EMPTY_MSG to addr id ",
				rp->addr_id);
	/* follow return path */
	if (rp->callback_func) {
		rp->callback_func(msg_p);
		rvf_free_buf(msg_p);
	} else {


#if (TRACE_ON_USB == 1)
	if(endpoint_vs_interface_entry_p->interface_data_p->interface_number == USB_IF_SERIAL_TRACE
		&& endpoint_vs_interface_entry_p->endpoint_data.endpo_descr_p->bEndpointAddress == 0x81)
          rvf_unlock_mutex (&usb_tx_mutex);
#endif	
	   
	if (RVF_OK != rvf_send_msg (rp->addr_id, msg_p)) {
			USB_SEND_TRACE("USB: could not send notify msg", 
					RV_TRACE_LEVEL_ERROR);
			return RVM_MEMORY_ERR;
		}
	}
	return(USB_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name notify_rx_buffer_full
 *
 * This function is called to send the function manager to which the endpoint 
 * belongs a message indicating that the provided rx buffer is filled with data 
 * recieved from the USB HOST. The function manager can than provide a new buffer 
 * in which data can be written, this can be data that belongs to the same packet as
 * the previous buffer or data that belong to a complete new packet transfer.
 *
 * @param	pep_idx			physical endpoint index in TX xref table (CAUTION not 
 *							the physical endpointnumber itself)
 * @param   size			size of the data placed in the buffer
 * @param   end_of_packet	indicates if the data in the buffer is the last of a 
 *							packet, or if other data, that belongs with this packet, 
 *							is still comming
 *							 
 * @return	T_RV_RET		USB_OK when successfully executed else USN_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
extern UINT8 timer_started;
T_RV_RET notify_rx_buffer_full(UINT32 pep_idx, UINT32 size, BOOL end_of_packet)
{
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	T_USB_RX_BUFFER_FULL_MSG *msg_p;
	T_RV_RETURN *rp;

	/* sanity check */
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	if (pep_idx > cnfg->nof_rx_ep) {
#else
	if (pep_idx >= cnfg->nof_rx_ep) {
#endif
		return RV_INTERNAL_ERR;
	}
	/* create message */
	if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
						sizeof (T_USB_RX_BUFFER_FULL_MSG),
						USB_RX_BUFFER_FULL_MSG,
						((T_RVF_MSG**) &msg_p))) {
		USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
		return RVM_MEMORY_ERR;
	}
	endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl + pep_idx;
	msg_p->endpoint = endpoint_vs_interface_entry_p->endpoint_data.endpoint_number;
	msg_p->interface = endpoint_vs_interface_entry_p->interface_data_p->interface_number;
	msg_p->size = (UINT16)(endpoint_vs_interface_entry_p->temp_buf_p - endpoint_vs_interface_entry_p->buf_p);

	// Stop the timer used to handle multiple of 64 byte transfer.
	// As all host drivers are not sending ZLP in such cases.
	if(msg_p->interface == USB_IF_CDC_SERIAL && timer_started == 0x01)
	{
		msg_p->size = endpoint_vs_interface_entry_p->temp_buf_p - endpoint_vs_interface_entry_p->buf_p;
		usb_env_ctrl_blk_p->trackMissingZLP.pep_idx = 255;
		usb_env_ctrl_blk_p->trackMissingZLP.size = 0;
		usb_stop_timer();
	}
	
	endpoint_vs_interface_entry_p->buf_p = NULL;
	endpoint_vs_interface_entry_p->temp_buf_p = NULL;
	msg_p->end_of_packet = end_of_packet;
	rp = &endpoint_vs_interface_entry_p->interface_data_p->swe_return_path;
	USB_TRACE_WARNING_PARAM("USB: notify_rx_buffer_full: "
				"sending USB_RX_BUFFER_FULL_MSG to addr id ",
				rp->addr_id);
	USB_TRACE_WARNING_PARAM("USB: notify_rx_buffer_full: length ",
				size);
	/* follow return path */
	if (rp->callback_func) {
		rp->callback_func(msg_p);
		rvf_free_buf(msg_p);
	} else {
		if (RVF_OK != rvf_send_msg (rp->addr_id, msg_p)) {
			USB_SEND_TRACE("USB: could not send notify msg", 
					RV_TRACE_LEVEL_ERROR);
			return RVM_MEMORY_ERR;
		}
	}
	return(USB_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name notify_ep0_tx_buffer_empty
 *
 * This function is called to send the function manager suscribed to interface Id
 * a message indicating that the provided tx buffer was read out end send
 * to the USB HOST, The function manager can than provide a new buffer with data 
 * to transmit
 *
 * @param	interface_id	Interface to notify
 *							 
 * @return	T_RV_RET		USB_OK when successfully executed else USN_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/

T_RV_RET notify_ep0_tx_buffer_empty(UINT32 interface_id)
{
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	T_USB_RX_BUFFER_FULL_MSG *msg_p;
	T_RV_RETURN *rp;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	UINT8 i;
#endif

	/* sanity check */
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	if (interface_id > USB_TOTAL_DIFF_IF) 
#else
	if (interface_id >= cnfg->nof_if) 
#endif
	{
		return RV_INTERNAL_ERR;
	}
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	for(i=0;i<USB_TOTAL_DIFF_IF;i++)
	{
		if(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_number == interface_id
			&& usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_enabled == TRUE)
			break;
	}	
#endif
	/* create message */
	if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
						sizeof (T_USB_TX_BUFFER_EMPTY_MSG),
						USB_TX_BUFFER_EMPTY_MSG,
						((T_RVF_MSG**) &msg_p))) {
		USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
		return RVM_MEMORY_ERR;
	}

#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	endpoint_vs_interface_entry_p = &(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i]);
#else
	endpoint_vs_interface_entry_p = &(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[interface_id]);
#endif
	msg_p->endpoint = 0;
	msg_p->interface = endpoint_vs_interface_entry_p->interface_data_p->interface_number;
	endpoint_vs_interface_entry_p->buf_p = NULL;
	endpoint_vs_interface_entry_p->temp_buf_p = NULL;
	rp = &endpoint_vs_interface_entry_p->interface_data_p->swe_return_path;
	usb_env_ctrl_blk_p->host_data.ep0_setup.if_nbr = 0xFF;

	USB_TRACE_WARNING_PARAM("USB: notify_ep0_tx_buffer_empty: "
				"sending USB_TX_BUFFER_EMPTY_MSG to interface id ",
				rp->addr_id);
	
	/* follow return path */
	if (rp->callback_func) 
	{
		rp->callback_func(msg_p);
		rvf_free_buf(msg_p);
	} else 
	{

		if (RVF_OK != rvf_send_msg (rp->addr_id, msg_p)) 
		{
			USB_SEND_TRACE("USB: could not send notify msg", 
					RV_TRACE_LEVEL_ERROR);
			return RVM_MEMORY_ERR;
		}

	}

	return(USB_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name notify_ep0_rx_buffer_full
 *
 * This function is called to send the function manager suscribed to interface given
 * in parameter, a message indicating that the provided rx buffer is filled with data 
 * recieved from the USB HOST. The function manager can than provide a new buffer 
 * in which data can be written, this can be data that belongs to the same packet as
 * the previous buffer or data that belong to a complete new packet transfer.
 *
 * @param	interface_id	
 * @param   size			size of the data placed in the buffer
 * @param   end_of_packet	indicates if the data in the buffer is the last of a 
 *							packet, or if other data, that belongs with this packet, 
 *							is still comming
 *							 
 * @return	T_RV_RET		USB_OK when successfully executed else USN_INTERNAL_ERR
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
T_RV_RET notify_ep0_rx_buffer_full(UINT32 interface_id, UINT32 size, BOOL end_of_packet)
{
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	T_USB_RX_BUFFER_FULL_MSG *msg_p;
	T_RV_RETURN *rp;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	UINT8 i;
#endif

	/* sanity check */
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	if (interface_id > USB_TOTAL_DIFF_IF) {
#else
	if (interface_id >= cnfg->nof_rx_ep) {
#endif
		return RV_INTERNAL_ERR;
	}
	/* create message */
	if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
						sizeof (T_USB_RX_BUFFER_FULL_MSG),
						USB_RX_BUFFER_FULL_MSG,
						((T_RVF_MSG**) &msg_p))) {
		USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
		return RVM_MEMORY_ERR;
	}
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	for(i=0;i<USB_TOTAL_DIFF_IF;i++)
	{
		if(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_number == interface_id
			&& usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_enabled == TRUE)
			break;
	}
	endpoint_vs_interface_entry_p = &(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i]);
#else
	endpoint_vs_interface_entry_p = &(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[interface_id]);
#endif
	msg_p->endpoint = 0;
	msg_p->interface = endpoint_vs_interface_entry_p->interface_data_p->interface_number;
	msg_p->size = size;
	endpoint_vs_interface_entry_p->buf_p = NULL;
	endpoint_vs_interface_entry_p->temp_buf_p = NULL;
	msg_p->end_of_packet = end_of_packet;

	rp = &endpoint_vs_interface_entry_p->interface_data_p->swe_return_path;
	usb_env_ctrl_blk_p->host_data.ep0_setup.if_nbr = 0xFF;

	USB_TRACE_WARNING_PARAM("USB: notify_ep0_rx_buffer_full: "
				"sending USB_RX_BUFFER_FULL_MSG to interface id ",
				rp->addr_id);

	/* follow return path */
	if (rp->callback_func) 
	{
		rp->callback_func(msg_p);
		rvf_free_buf(msg_p);
	} 
	else 
	{
		if (RVF_OK != rvf_send_msg (rp->addr_id, msg_p)) 
		{
			USB_SEND_TRACE("USB: could not send notify msg", 
					RV_TRACE_LEVEL_ERROR);
			return RVM_MEMORY_ERR;
		}
	}

	return(USB_OK);
}
/*@}*/

/**
 *---------------------------------------------------------------------------------* 
 * @name send_empty_packet
 *
 * This function enables to send an empty packet to the host
 * It is used in case of Host to Device SETUP request
 *
 * @param   ep_nr			the endpoint that should be prepared
 *
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void send_empty_packet(UINT8 ep_nr)
{
	// Write EP_NUM register: //
	// EP_NUM.EP_Num = n      //
	// EP_NUM.EP_Dir = 1      //
	// EP_NUM.EP_Sel = 1      //
	// EP_NUM.Setup_Sel = 0   //
	USB_EP_NUM_SEL_TX_EP(ep_nr);

	// Set CTRL.Clr_EP to '1', then set CTRL.Set_FIFO_En to '1' //
	W2FC_CTRL = W2FC_CTRL_CLR_EP;
	W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;

	// Write EP_NUM register: //
	// EP_NUM.EP_Num = n      //
	// EP_NUM.EP_Dir = 1      //
	// EP_NUM.EP_Sel = 0      //
	// EP_NUM.Setup_Sel = 0   //
	USB_EP_NUM_TX_RELEASE(ep_nr);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name name specific_ep0_setup_device_to_host_handler
 *
 * This function is called in case of Class host to device request
 * FM buffer is filled with the Setup packet
 * 
 *  @param no parameter
 *
 *  @return	no returned value
 *							
 *---------------------------------------------------------------------------------* 
 */
static void specific_ep0_setup_device_to_host_handler()
{
	UINT16 setup_packet_size = 8;
	INT8 interface_id = USB_REQUEST_INDEX & 0x00FF;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	int i;
	INT8 lif;
#endif

	// Getting buffer from interface
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p =
							&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[interface_id]);
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	lif = pif_2_lif(usb_env_ctrl_blk_p->enumInfo.info, interface_id);
	if(lif != -1)
	{
		interface_id = lif;
	}else{
		return;
	}
	for(i=0;i<USB_TOTAL_DIFF_IF;i++)
	{
		if(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_number == interface_id
			&& usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_enabled == TRUE)
			break;
	}

	endpoint_vs_interface_entry_p =
							&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i]);
#endif

	if(specific_ep0_check_interface_validity() == FALSE)
	{
		//device behaviour not specified
		USB_SEND_TRACE("USB:  Interface is not valid", RV_TRACE_LEVEL_ERROR);
		abort_setup_req(WITH_CMD_STALL);

		return;
	}
	else
	{
		USB_SEND_TRACE("USB:  Validity of interface is OK", RV_TRACE_LEVEL_DEBUG_LOW);
	}


	if((endpoint_vs_interface_entry_p->buf_p != NULL)
		&& (endpoint_vs_interface_entry_p->buf_size >= 8))
	{
		USB_SEND_TRACE("USB:  Copy SETUP command", RV_TRACE_LEVEL_DEBUG_LOW);

		/* copy in user buffer the	received Setup Packet */
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_TYPE;
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST;

		/* Value field of the Setup request */
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_VALUE & 0xFF;
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_VALUE >> 8;

		/* Index field of the Setup request */
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_INDEX & 0xFF;
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_INDEX >> 8;

		/* Length field of the Setup request */
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_LENGTH & 0xFF;
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_LENGTH >> 8;

		USB_SEND_TRACE("USB:  No data in SETUP command", RV_TRACE_LEVEL_DEBUG_LOW);
		notify_ep0_rx_buffer_full(interface_id, setup_packet_size, USB_EOT);
	}
	else
	{
		USB_SEND_TRACE("USB:  No buffer valid to receive SETUP packet", 
		RV_TRACE_LEVEL_ERROR);
		abort_setup_req(WITH_CMD_STALL);
	}
}

/**
 *---------------------------------------------------------------------------------* 
 * @name specific_ep0_rx_handler
 *
 * This function is called on end of a class or vendor device to host transaction
 * FM buffer is filled with the Setup packet and with data of the transaction
 * a buffer full notification is sent to FM to indicate that buffer is available
 *
 * @param		no argument
 * @return	NONE
 *---------------------------------------------------------------------------------* 
 */
/*@{*/
static void specific_ep0_rx_handler()
{
	/* 8 bytes */
	UINT16 setup_packet_size = 8;
	INT8 interface_id = USB_REQUEST_INDEX & 0x00FF;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	int i;
	INT8 lif;
#endif

	// Getting buffer from interface
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p =
							&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[interface_id]);
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	lif = pif_2_lif(usb_env_ctrl_blk_p->enumInfo.info, interface_id);
	if(lif != -1)
	{
		interface_id = lif;
	}else{
		return;
	}
	
	for(i=0;i<USB_TOTAL_DIFF_IF;i++)
	{
		if(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_number == interface_id
			&& usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_enabled == TRUE)
			break;
	}

	endpoint_vs_interface_entry_p =
							&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i]);	
#endif

	if(specific_ep0_check_interface_validity() == FALSE)
	{
		//device behaviour not specified
		USB_SEND_TRACE("USB:  Interface is not valid", RV_TRACE_LEVEL_ERROR);
		abort_setup_req(WITH_CMD_STALL);

		return;
	}
	else
	{
		USB_SEND_TRACE("USB:  Validity of interface is OK", RV_TRACE_LEVEL_DEBUG_LOW);
	}


	if((endpoint_vs_interface_entry_p->buf_p != NULL)
		&& (endpoint_vs_interface_entry_p->buf_size >= 8))
	{
		USB_SEND_TRACE("USB:  Copy SETUP command", RV_TRACE_LEVEL_DEBUG_LOW);

		/* copy in user buffer the	received Setup Packet */
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_TYPE;
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST;

		/* Value field of the Setup request */
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_VALUE & 0xFF;
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_VALUE >> 8;

		/* Index field of the Setup request */
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_INDEX & 0xFF;
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_INDEX >> 8;

		/* Length field of the Setup request */
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_LENGTH & 0xFF;
		*(endpoint_vs_interface_entry_p->temp_buf_p++) = USB_REQUEST_LENGTH >> 8;
			
		if(USB_REQUEST_LENGTH == 0)
		{
			USB_SEND_TRACE("USB:  No data in SETUP command", RV_TRACE_LEVEL_DEBUG_LOW);
			notify_ep0_rx_buffer_full(interface_id, setup_packet_size, USB_EOT);
		}
		else /* Copy data of the SETUP packet in the FM buffer */
		{
			USB_SEND_TRACE("USB:  Copy data of SETUP command in FM buffer", RV_TRACE_LEVEL_DEBUG_LOW);
			
			if(usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wLength <= 
							(endpoint_vs_interface_entry_p->buf_size - setup_packet_size) )
			{
				/* FM enough is large enough to contain all data of the Setup command */
				memcpy(	endpoint_vs_interface_entry_p->temp_buf_p, 
						usb_env_ctrl_blk_p->host_data.ep0_data_p,
						usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wLength );

				endpoint_vs_interface_entry_p->temp_buf_p += usb_env_ctrl_blk_p->host_data.ep0_setup.request_p->wLength;

				notify_ep0_rx_buffer_full(	interface_id, 
											endpoint_vs_interface_entry_p->temp_buf_p 
															- endpoint_vs_interface_entry_p->buf_p, 
											USB_EOT);
			}
			else
			{
				UINT16	nr_byte_data_remaining =		/* Number of bytes remaining in FM buffer */
					endpoint_vs_interface_entry_p->buf_size - setup_packet_size;

				memcpy(	endpoint_vs_interface_entry_p->temp_buf_p, 
						usb_env_ctrl_blk_p->host_data.ep0_data_p,
						nr_byte_data_remaining );

				/* Notification is made to FM that his buffer is full */
				notify_ep0_rx_buffer_full(	interface_id,
											endpoint_vs_interface_entry_p->buf_size, 
											USB_NO_EOT);
			}
		}
	}
	else
	{
		USB_SEND_TRACE("USB:  Buffer too small to receive SETUP packet", 
		RV_TRACE_LEVEL_ERROR);
		abort_setup_req(WITH_CMD_STALL);
	}	
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name name specific_ep0_check_interface_validity
 *
 * This function is called by specific_ep0_tx_handler and specific_ep0_rx_handler
 * to check validity of interface in case of class or vendor request
 * 
 *  @param no parameter
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE 
 *---------------------------------------------------------------------------------* 
 */
static BOOL specific_ep0_check_interface_validity()
{
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	UINT8 interface_id = usb_env_ctrl_blk_p->host_data.ep0_setup.if_nbr;
	BOOL validity = FALSE;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	int i;
#endif

	USB_TRACE_WARNING_PARAM("USB : Checking interface :", interface_id);

	/* sanity check */
	if ((interface_id < USB_TOTAL_DIFF_IF) && (interface_id != 0xFF))/* is interface number valid */
	{
		T_USB_INTERFACE_DATA *usb_if_tbl_entry = 
									&(usb_env_ctrl_blk_p->usb_if_tbl[interface_id]);
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	for(i=0;i<USB_TOTAL_DIFF_IF;i++)
	{
		if(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_number == interface_id
			&& usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i].interface_data_p->interface_enabled == IF_ENABLED)
			break;
	}
#endif
		
		if (usb_if_tbl_entry->interface_enabled == IF_ENABLED)
		{
			T_USB_CROSS_REF *endpoint_vs_interface_entry_p =
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
									&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[i]);
#else
									&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[interface_id]);
#endif

			if(endpoint_vs_interface_entry_p->buf_p != NULL)
			{
				validity = TRUE;
			}
			else
			{
				USB_SEND_TRACE("USB: FM buffer is NULL", RV_TRACE_LEVEL_DEBUG_LOW);
			}
		}
		else
		{
			USB_SEND_TRACE("USB: Interface is not enabled", RV_TRACE_LEVEL_DEBUG_LOW);
		}
	}
	else
	{
		USB_SEND_TRACE("USB: Invalid interface", RV_TRACE_LEVEL_DEBUG_LOW);
	}

	return(validity);

}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name usb_non_iso_tx_dma_done_hndlr
 *
 *
 *  @param no parameter
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
// CHB
void usb_non_iso_tx_dma_done_handler( void );

static T_RV_RET usb_non_iso_tx_dma_done_hndlr()
{	
	T_RV_RET ret = USB_OK;
	
	/* Read the endpoint number */
	UINT8	endp_nb = (W2FC_DMAN_STAT & W2FC_DMAN_STAT_DMAN_TX_IT_SRC);

	/* clear the Tx Done interrupt */
	// CHB W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_TXN_DONE);

	#if (L1_POWER_MGT != 0x00)
	Usb_Vote_DeepSleepStatus(); 
	/*to update power status.added by saumar*/
	#endif

	// read progress counter
	USB_TRACE_WARNING_PARAM ("usb_non_iso_tx_dma_done_hndlr: f_dma_get_channel_counter=", f_dma_get_channel_counter (0));// 0 = a voir

	usb_non_iso_tx_dma_done_handler();

	/*
	 * Inform the application that the TX DMA transfer is completed
	 * argument of "notify_tx_buffer_empty" function is the physical index of
	 * array of endpoints
	 */
	// CHB notify_tx_buffer_empty(USB_PEP_INDX(endp_nb));

	return(ret);
}
/*@}*/

/*--------------------------------------------------------------------------------*
 * @name usb_timer_expiration
 *
 *
 *  @param
 *
 *
 *---------------------------------------------------------------------------------*
 */
	


void usb_timer_expiration (UNSIGNED id)
{
	/*
     * Timer has expired.
     */

	UINT16	CPC;

	UINT8	DMA_nb 	= 0;// 0 = a generaliser
	UINT8	DMA_ch 	= 4;// 4 = a generaliser	

	UINT16	bitmask_reset		= 0x0F;
	UINT16	bitmask_result		= 0x0F;

	UINT8	if_nr		= USB_DMA_CFG.dma_usb_endpoint_rx_tbl[id].interface_id;
	UINT8	log_ep_nr	= USB_DMA_CFG.dma_usb_endpoint_rx_tbl[id].endpoint_id;
	UINT8	dma_channel	= USB_DMA_CFG.dma_usb_endpoint_rx_tbl[id].dmg_channel;

    UINT8	phys_ep_nr	= LEP_2_PEP(log_ep_nr, if_nr,  USB_RX) + 1;
	T_RV_RET ret = RV_OK;

	UINT32	start_of_buffer = (((UINT32)usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p) & 0xffff);
	UINT32	end_of_buffer	= start_of_buffer +
		usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_size;

	UINT16	dma_channel_counter = f_dma_get_channel_counter(dma_channel);

#if 0
	CPC = dma_channel_counter
		- (((UINT32)usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p) & 0xffff) + 1;
#endif
	CPC = dma_channel_counter
		- (((UINT32)usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p) & 0xffff);

	if (CPC && (dma_channel_counter >= (UINT16)start_of_buffer)
			&& (dma_channel_counter < (UINT16)end_of_buffer))
	{

		if (CPC == USB_DMA_CFG.dma_usb_rx_memo_size[id])
		{
			// CPC : length used by DMA
                        // total_read_size : CPC + manual reading
#if 0                        
			UINT16 total_read_size = f_dma_get_channel_counter (dma_channel) -
                        (((UINT32)usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p) & 0xffff) + 1;
#endif 
			UINT16 total_read_size = f_dma_get_channel_counter (dma_channel) -
                        (((UINT32)usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p) & 0xffff);

			USB_SEND_TRACE("USB::usb_timer_expiration -> Notify buffer full", RV_TRACE_LEVEL_DEBUG_LOW);


			if(usb_get_dma_information(FALSE, phys_ep_nr, &DMA_ch, &DMA_nb) == RV_INTERNAL_ERR)
			{
				return;
			}

			if(TRUE == USB_DMA_CFG.dma_usb_endpoint_rx_tbl[id].channel_used)
			{
				ret = unsubscribe_dma_usb_channel(dma_channel);
				if(RV_OK == ret)
					USB_DMA_CFG.dma_usb_endpoint_rx_tbl[id].channel_used = FALSE;
			}	

			/*
				NOTE:
				Following line is added during ZLP timer implementation.
				To make data structure modification as similar as interrupt path the following code is added.
				'notify_rx_buffer_full' function calculates size by subtraction 'buf_p' from 'temp_buf_p'
			*/
			usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p += total_read_size;	

	
			notify_rx_buffer_full(	USB_PEP_INDX(phys_ep_nr),
								total_read_size,
								USB_NO_EOT);

			bitmask_reset = bitmask_reset << (4 * DMA_nb);
			bitmask_result = W2FC_RXDMA_CFG & ~bitmask_reset;
			W2FC_RXDMA_CFG = bitmask_result;			


			USB_DMA_CFG.dma_usb_rx_memo_size[id] = 0;
		}
		else
		{
			/*
			USB_SEND_TRACE("USB::timer Receiving data", RV_TRACE_LEVEL_DEBUG_LOW);
			*/

			USB_DMA_CFG.dma_usb_rx_memo_size[id] = CPC;

			(void) NU_Control_Timer (&(USB_DMA_CFG.dma_usb_rx_timer[id]),
					                 NU_DISABLE_TIMER);

			(void) NU_Reset_Timer (
                   &(USB_DMA_CFG.dma_usb_rx_timer[id]),
                   	usb_timer_expiration,
					WAKE_UP_TIME_IN_TDMA,
                   0, /* The timer expires once. */
                   NU_DISABLE_TIMER);

			(void) NU_Control_Timer (&(USB_DMA_CFG.dma_usb_rx_timer[id]),
									 NU_ENABLE_TIMER);
		}
	}
	else
	{
		(void) NU_Control_Timer (&(USB_DMA_CFG.dma_usb_rx_timer[id]),
					                 NU_DISABLE_TIMER);

		(void) NU_Reset_Timer (
                   &(USB_DMA_CFG.dma_usb_rx_timer[id]),
                   usb_timer_expiration,
				   WAKE_UP_TIME_IN_TDMA,
                   0, /* The timer expires once. */
                   NU_DISABLE_TIMER);

		(void) NU_Control_Timer (&(USB_DMA_CFG.dma_usb_rx_timer[id]),
								 NU_ENABLE_TIMER);
	}
}

/*@}*/

/*--------------------------------------------------------------------------------*
 * @name usb_non_iso_rx_dma_eot_hndlr
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */

static T_RV_RET usb_non_iso_rx_dma_eot_hndlr()
{
	T_RV_RET ret = USB_OK;
	UINT16 read_size;

	UINT8	DMA_nb 	= 0;// 0 = a generaliser
	UINT8	DMA_ch 	= 4;// 4 = a generaliser
	UINT32	sram_address_source = 0;

	/* Read endpoint number */
	UINT8	phys_ep_nr = (W2FC_DMAN_STAT & W2FC_DMAN_STAT_DMAN_RX_IT_SRC) >> 8;

	UINT8	i;

	/*
	 * Check if the core received an odd number
	 * of bytes during the last transaction
	 */
	BOOL	odd_number_of_bytes = ( (W2FC_DMAN_STAT & W2FC_DMAN_STAT_DMAN_RX_SB) != 0);

	UINT16	bitmask_reset		= 0x0F;
	UINT16	bitmask_result		= 0x0F;

	UINT32	sram_address = 0;
	UINT8*	p_sram_address = NULL;

	#if (L1_POWER_MGT != 0x00)
	Usb_Vote_DeepSleepStatus(); 
	/*to update power status.added by saumar*/
	#endif
	
	if(phys_ep_nr == 0)
	{
		return RV_OK;
	}

	// usb_env_ctrl_blk_p->bm_rx_ep_buf_stall |= (1 << phys_ep_nr);
	/* FALSE for receive channel */
	if(usb_get_dma_information(FALSE, phys_ep_nr, &DMA_ch, &DMA_nb) == RV_INTERNAL_ERR)
	{
		return RV_INTERNAL_ERR;
	}

#if 0
	(void) NU_Control_Timer (&(USB_DMA_CFG.dma_usb_rx_timer[DMA_nb]),
                             NU_DISABLE_TIMER);
#endif

	USB_DMA_CFG.dma_usb_rx_memo_size[DMA_nb] = 0;
	//- stop DMA transfer -//
	f_dma_channel_disable (DMA_ch);// 15/09/04 2 = a generaliser

	// read progress counter
	USB_TRACE_WARNING_PARAM ("usb_non_iso_rx_dma_eot_hndlr: f_dma_get_channel_counter=", f_dma_get_channel_counter (2));// 2 = a generaliser
	// length used in buff
    read_size = f_dma_get_channel_counter (DMA_ch) - C_DMA_CDSA_L_REG(DMA_ch) + 1;

	// unsubscribe_dma_usb_channel(DMA_ch);

	sram_address	=	C_DMA_CDSA_U_REG(DMA_ch);
	sram_address	<<= 16;
	sram_address	|=	C_DMA_CDSA_L_REG(DMA_ch);

	p_sram_address = (UINT8*)sram_address;

	if(p_sram_address != usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p)
	{
		memcpy(	usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p,
				p_sram_address,
				read_size);
	}

	if(TRUE == USB_DMA_CFG.dma_usb_endpoint_rx_tbl[DMA_nb].channel_used)
	{
		ret = unsubscribe_dma_usb_channel(DMA_ch);
		if(RV_OK == ret)
			USB_DMA_CFG.dma_usb_endpoint_rx_tbl[DMA_nb].channel_used = FALSE;
	}

	/*
		NOTE:
		Following changes done during ZLP timer implementation.
		To make data structure modification as similar as interrupt path the following code is added.
		<< will add mored comments on this!>>
	*/
	usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].temp_buf_p += read_size;

	// Inform the application that the RX DMA transfer n is completed
	notify_rx_buffer_full(	USB_PEP_INDX(phys_ep_nr),
		read_size,
		USB_EOT);

	bitmask_reset = bitmask_reset << (4 * DMA_nb);
	bitmask_result = W2FC_RXDMA_CFG & ~bitmask_reset;
	W2FC_RXDMA_CFG = bitmask_result;

	// wait 5 cycles, the hardware seems to need some time to update the registers
	for(i = 0; i < 5; i++);
	USB_TRACE_WARNING_PARAM("USB DMA in interrupt: W2FC_RXDMA_CFG", W2FC_RXDMA_CFG);

/*
	USB_EP_NUM_SEL_RX_EP(phys_ep_nr);
	W2FC_CTRL = W2FC_CTRL_SET_FIFO_EN;
	USB_EP_NUM_RX_RELEASE(phys_ep_nr);
*/

	switch(DMA_nb)
	{
		case 0 :
			W2FC_DMA_IRQ_EN &= ~W2FC_DMA_IRQ_EN_RX0_CNT_IE;
			W2FC_RXDMA0	= 0;
		break;

		case 1 :
			W2FC_DMA_IRQ_EN &= ~W2FC_DMA_IRQ_EN_RX1_CNT_IE;
			W2FC_RXDMA1	= 0;
		break;

		case 2 :
			W2FC_DMA_IRQ_EN &= ~W2FC_DMA_IRQ_EN_RX2_CNT_IE;
			W2FC_RXDMA2	= 0;
		break;

		default:
		break;
	}

	/* clear the EOT interrupt */
	W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_RXN_EOT);

	return(ret);
}
/*@}*/

/**
 *--------------------------------------------------------------------------------*
 * @name usb_non_iso_rx_dma_transaction_count_hndlr
 *
 *
 *  @param interface_id
 *
 *  @return	BOOL 		Returns TRUE when interface are corrects
 *						else FALSE
 *---------------------------------------------------------------------------------*
 */
static T_RV_RET usb_non_iso_rx_dma_transaction_count_hndlr()
{
	T_RV_RET ret = USB_OK;
	UINT16 read_size;
    UINT8	DMA_nb = 0;// 0 = a generaliser
	UINT8	DMA_ch = 4;// 4 = a generaliser

	/* Read endpoint number */
	UINT8	phys_ep_nr = (W2FC_DMAN_STAT & W2FC_DMAN_STAT_DMAN_RX_IT_SRC) >> 8;

	#if (L1_POWER_MGT != 0x00)
	Usb_Vote_DeepSleepStatus(); 
	/*to update power status.added by saumar*/
	#endif

	if(phys_ep_nr == 0)
	{
		return RV_OK;
	}

	/*
	 * Inform the application that the RX DMA transfer on channel n has sent
	 * RXDMAn.RXn_TC transsactions count without detecting an EOT
	 */

	//- stop DMA transfer -//
    if(usb_get_dma_information(FALSE, phys_ep_nr, &DMA_ch, &DMA_nb) != RV_INTERNAL_ERR)
	{
		f_dma_channel_disable (DMA_ch);// 15/09/04 2 = a generaliser
	}

	// read progress counter
	USB_TRACE_WARNING_PARAM ("usb_non_iso_rx_dma_transaction_count_hndlr: f_dma_get_channel_counter=", f_dma_get_channel_counter (2));// 2 = a generaliser
	//read_size = f_dma_get_channel_counter (2) - (((UINT32)usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep_nr)].buf_p) & 0xffff) + 1;

	/*
	 * Inform the application that the RX DMA transfer n is completed
	 */
	/*notify_rx_buffer_full(	USB_PEP_INDX(phys_ep_nr),
							read_size,
							USB_NO_EOT);*/

	/* clear the CNT interrupt */
	W2FC_IRQ_SRC_CLR_INT(W2FC_IRQ_SRC_RXN_CNT);

	switch(DMA_nb)
	{
		case 0 :
			W2FC_DMA_IRQ_EN &= ~W2FC_DMA_IRQ_EN_RX0_CNT_IE;
			W2FC_RXDMA0	= 0;
			USB_SEND_TRACE("USB: usb_non_iso_rx_dma_transaction_count_hndlr: channel 0", RV_TRACE_LEVEL_DEBUG_LOW);
		break;

		case 1 :
			W2FC_DMA_IRQ_EN &= ~W2FC_DMA_IRQ_EN_RX1_CNT_IE;
			W2FC_RXDMA1	= 0;
			USB_SEND_TRACE("USB: usb_non_iso_rx_dma_transaction_count_hndlr: channel 1", RV_TRACE_LEVEL_DEBUG_LOW);
		break;

		case 2 :
			W2FC_DMA_IRQ_EN &= ~W2FC_DMA_IRQ_EN_RX2_CNT_IE;
			W2FC_RXDMA2	= 0;
			USB_SEND_TRACE("USB: usb_non_iso_rx_dma_transaction_count_hndlr: channel 2", RV_TRACE_LEVEL_DEBUG_LOW);
		break;

		default:
		break;
	}

	//- start DMA transfer -//
	//f_dma_channel_enable (2);// 2 = a generaliser

	return(ret);
}
/*@}*/

