/*/ * @file	usb_cfg_dat.c
 *
 * USB hardware / data handling functions, 
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

#include "usb/usb_i.h"
#include "usb/usb_inth.h"
#include "usb/usb_env.h"
#include "usb/usb_config.h"
#include "usb/usb_inth.h"
/****************************************************************************************
 |	DEFINITIONS																			|
 V**************************************************************************************V*/

 /****************************************************************************************
 |	VARIABLE DECLARATIONS																|
 V**************************************************************************************V*/


/****************************************************************************************
 |	FUNCTION PROTOTYPES																	|
 V**************************************************************************************V*/

T_RV_RET fill_ep_descr_tbl(T_USB_ENDPOINT_DESCR *ep_p, T_USB_EP_DESCR lep, UINT32 pep_nr);

 /*
T_RV_RET find_endpoint(UINT8 rx_tx, UINT8 if_nr, UINT8 ep_type, UINT8 ep_nr,\
					   T_USB_ENDPOINT_DATA* out_ep_pointer);

*/
/**
 * @name USB device descriptor
 *
 *There is only one device descriptor which is defined here!
 */
/*@{*/
const T_USB_DEVICE_DESCR usb_device_descr = 
{	
	USB_DEV_DESCR_LNT,
	USB_DEVICE_TYPE,
	USB_BCD_CODE,
	USB_DEV_CLASS,
	USB_DEV_SUB_CLASS,
	USB_DEV_PROTOCOL,
	USB_ENDP0_SIZE,
	USB_VENDOR_ID,
	USB_PRODUCT_ID,
	USB_BCD_DEV,
	(UINT8)USB_MANUF_STR,
	(UINT8)USB_PROD_STR,
	(UINT8)USB_SERIAL_STR,
	//0x00,	//no string
	//0x00,	//no string
	//0x00,	//no string
	USB_DEV_NUM_CFG
	//0x01
};

/****************************************************************************************
 |	EXTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/

T_RV_RET create_endpoint_tables(void)
{
	return USB_OK;
}

#ifdef USB_DYNAMIC_CONFIG_SUPPORT
T_RV_RET create_configuration_descriptor(UINT8 enumInfo)
{
	UINT8 config = enumInfo;
	UINT8 bit_shifted;
	UINT8 ifaceCount=0;
	UINT8 ifaceArrayIndex=0;// Holds interface value used by FMs
	UINT8 configTrue=0;
	UINT8 enummask=0x1;// Used to mask uncompiled interfaces
	UINT8 i;

	configurations_table[0].if_cfg_descr_p = &usb_conf_descr_table[0];

	for (i = 0; i < USB_TOTAL_DIFF_IF; i++) {
		if_descr_table[i].bInterfaceNumber = 0xff;
	}

	while((bit_shifted = config>>ifaceArrayIndex)){
		if(bit_shifted&0x1){
			if(((config&(UINT8)USB_TRACE) == USB_TRACE) && ifaceArrayIndex == 0){
				#if (TRACE_ON_USB==1)
				if_descr_table[USB_IF_SERIAL_TRACE].bInterfaceNumber = ifaceCount;
				configurations_table[0].if_cnfg_p[ifaceCount].ifdescr_p = &if_descr_table[USB_IF_SERIAL_TRACE];
				configurations_table[0].if_cnfg_p[ifaceCount].if_cdc_descr_p = NULL;
				configurations_table[0].if_cnfg_p[ifaceCount].if_logical_ep_array_p = &endpoint_in_if3_array[0];
				configTrue =1;
				#else
				configTrue=0;
				#endif
			}
			else if(((config&(UINT8)USB_AT) == USB_AT) && ifaceArrayIndex == 1){
				#if defined(RVM_USBFAX_SWE)
				if_descr_table[USB_IF_CDC_SERIAL].bInterfaceNumber = ifaceCount;
				configurations_table[0].if_cnfg_p[ifaceCount].ifdescr_p = &if_descr_table[USB_IF_CDC_SERIAL];
				configurations_table[0].if_cnfg_p[ifaceCount].if_cdc_descr_p = NULL;
				configurations_table[0].if_cnfg_p[ifaceCount].if_logical_ep_array_p = &endpoint_in_if1_array[0];
				configTrue=1;
				#else
				configTrue=0;
				#endif
			}
			else if(((config&(UINT8)USB_MS)== USB_MS) && ifaceArrayIndex == 2){
				#if defined(RVM_USBMS_SWE)
				if_descr_table[USB_IF_MSTORAGE].bInterfaceNumber = ifaceCount;
				configurations_table[0].if_cnfg_p[ifaceCount].ifdescr_p = &if_descr_table[USB_IF_MSTORAGE];
				configurations_table[0].if_cnfg_p[ifaceCount].if_cdc_descr_p = NULL;
				configurations_table[0].if_cnfg_p[ifaceCount].if_logical_ep_array_p = &endpoint_in_if2_array[0];
				configTrue=1;
				#else
				configTrue=0;
				#endif
			}
			if(configTrue)
			{
#ifdef  USB_DMA_USED
				subscribe_dma_usb_interface(configurations_table[0].if_cnfg_p[ifaceCount].ifdescr_p->bInterfaceNumber);
#endif
			
				ifaceCount++;
			}
			else
			{
				enumInfo = enumInfo & (~(enummask<<ifaceArrayIndex));
			}
		}
		ifaceArrayIndex++;
	}
	
	usb_env_ctrl_blk_p->enumInfo.info = enumInfo;
	configurations_table[0].if_cfg_descr_p->bNumInterfaces = ifaceCount;
	
	return USB_OK;
}
#endif
/**
 * @name conf_if_variables
 *
 * This function gives every interface data variable an initial value
 *
 * @return	T_RV_RET:	USB_OK, USB_NOT_READY, USB_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET conf_if_variables(void)
{
	UINT8 cntr = 0;
	T_RV_RET ret = USB_OK;
	
	for(cntr = 0; cntr < USB_TOTAL_DIFF_IF; cntr++)
	{
		/*the table of pointers to endpoint descriptors will be created as soon as a specific 
		conficuration in which this interface fits is chosen*/
		usb_env_ctrl_blk_p->usb_if_tbl[cntr].interface_enabled = IF_DISABLED;
		/*the return path will be assigned during subscription of the FM -to the if*/
		usb_env_ctrl_blk_p->usb_if_tbl[cntr].swe_return_path.addr_id = 0;
		usb_env_ctrl_blk_p->usb_if_tbl[cntr].swe_return_path.callback_func = NULL;
		/*bitmaps for every endpoint buffer to default (empty)*/
		usb_env_ctrl_blk_p->bm_rx_ep_buf_stall = USB_ZERO_BMP;
		usb_env_ctrl_blk_p->bm_tx_ep_buf_stall = USB_ZERO_BMP;
		/*set interface number*/
		/*the interface numbering starts with 1 while the array index starts with 0 so add 1*/
		// usb_env_ctrl_blk_p->usb_if_tbl[cntr].interface_number = (cntr + 1);
		usb_env_ctrl_blk_p->usb_if_tbl[cntr].interface_number = cntr;
//			if_descr_table[cntr].bInterfaceNumber;
	}//all interface variables are initialised now
	return ret;
}
/*@}*/


static BOOL ep_is_rx(T_USB_EP_DESCR e) 
{ 
	T_USB_ENDPOINT_DESCR ep;
	fill_ep_descr(&ep, e);
	return  ep.bEndpointAddress == USB_OUT;
}

T_RV_RET fill_ep_descr_tbl(T_USB_ENDPOINT_DESCR *ep_p, T_USB_EP_DESCR lep, UINT32 pep_nr)
{
	T_USB_ENDPOINT_DESCR ep;
	
	fill_ep_descr(&ep, lep);

	ep_p->blength = ep.blength;
	//USB_TRACE_WARNING_PARAM("USB: ep_p->blength",ep_p->blength);
	ep_p->bDescriptorType = ep.bDescriptorType;
	//USB_TRACE_WARNING_PARAM("USB: ep_p->bDescriptorType",ep_p->bDescriptorType );
	ep_p->bmAttributes = ep.bmAttributes;
	//USB_TRACE_WARNING_PARAM("USB: ep_p->bmAttributes ",ep_p->bmAttributes );
	ep_p->iInterval = ep.iInterval;
	//USB_TRACE_WARNING_PARAM("USB: ep_p->iInterval ",ep_p->iInterval );
	ep_p->wMaxPacketSize = ep.wMaxPacketSize;
	//USB_TRACE_WARNING_PARAM("USB: ep_p->wMaxPacketSize",ep_p->wMaxPacketSize);
	
	if(ep.bEndpointAddress == USB_OUT) {
		ep_p->bEndpointAddress = (UINT8)pep_nr;
	}
	else {
		ep_p->bEndpointAddress = ((UINT8)pep_nr | ep.bEndpointAddress);
	}
	//USB_TRACE_WARNING_PARAM("ep_p->bEndpointAddress ",ep_p->bEndpointAddress);

	return(USB_OK);
}
/**
 * @name conf_if_xrefs
 *
 * This function gives every interface data variable an initial value
 *
 * @return	T_RV_RET:	USB_OK, USB_NOT_READY, USB_INVALID_PARAMETER
 */
/*@{*/
T_RV_RET conf_if_xrefs(UINT32 cfg_idx)
{
	const T_USB_IF_CNFG* if_cnfg_p;
	UINT32 active_if_idx; /* for looping through IFs in current config */
	UINT32 static_if_idx; /* in config file */
	UINT32 ep_idx;
	UINT32 logical_ep_num = 0;
	UINT32 physical_ep_idx = 0;
	UINT32 physical_rx_ep_idx = 0;
	UINT32 physical_tx_ep_idx = 0;
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;
	T_USB_ENDPOINT_DESCR *endpoint_descr_tbl_entry_p = NULL;
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	UINT32 pep_nr;


	if (cfg_idx == 0) {
		return RVM_OK;
	}
	if (cfg_idx > USB_DEV_NUM_CFG) {
		USB_SEND_TRACE("USB: conf_if_xrefs: invalid config number", RV_TRACE_LEVEL_ERROR);
		return RVM_INTERNAL_ERR;
	}
	cfg_idx--;
	// usb_lock();
	if_cnfg_p = configurations_table[cfg_idx].if_cnfg_p;
	/* count physical RX and TX endpoints */
	/* FOREACH if in chosen cnfg */
	cnfg->nof_if = configurations_table[cfg_idx].if_cfg_descr_p->bNumInterfaces;
	for (active_if_idx = 0; active_if_idx < cnfg->nof_if; active_if_idx++) {
		const T_USB_EP_DESCR* if_logical_ep_array_p = if_cnfg_p[active_if_idx].if_logical_ep_array_p;
		/* FOREACH ep in this if */
		for (ep_idx = 0; ep_idx < if_cnfg_p[active_if_idx].ifdescr_p->bNumEndpoints; ep_idx++) {
			if (ep_is_rx(if_logical_ep_array_p[ep_idx])) {
				physical_rx_ep_idx++;
			} else {
				physical_tx_ep_idx++;
			}
		} /* END ep_idx loop */
	} /* END active_if_idx loop */
	cnfg->nof_rx_ep = physical_rx_ep_idx;
	cnfg->nof_tx_ep = physical_tx_ep_idx;

	/* clean up old cross ref tables, if present */
	/* TODO: a single NULL test should work as well */
	if (usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl != NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl);
		usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl = NULL;
	}
	if (usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl != NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl);
		usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl = NULL;
	}
	if (usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl != NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl);
		usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl = NULL;
	}
	if (usb_env_ctrl_blk_p->endpoint_rx_descr_tbl!= NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->endpoint_rx_descr_tbl);
		usb_env_ctrl_blk_p->endpoint_rx_descr_tbl = NULL;	
	}
	if (usb_env_ctrl_blk_p->endpoint_tx_descr_tbl != NULL) {
		rvf_free_buf(usb_env_ctrl_blk_p->endpoint_tx_descr_tbl);
		usb_env_ctrl_blk_p->endpoint_tx_descr_tbl = NULL;
	}

	/* allocate cross ref tables */
	if (RVF_RED == rvf_get_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				   cnfg->nof_rx_ep * sizeof (T_USB_CROSS_REF),
				   ((T_RVF_BUFFER**) &usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl))
			||
	    RVF_RED == rvf_get_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				   cnfg->nof_tx_ep * sizeof (T_USB_CROSS_REF),
				   ((T_RVF_BUFFER**) &usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl))
			||
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
		RVF_RED == rvf_get_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				   USB_CFG1_NUM_INTERF * sizeof (T_USB_CROSS_REF),
				   ((T_RVF_BUFFER**) &usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl))
#else
		RVF_RED == rvf_get_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				   cnfg->nof_if * sizeof (T_USB_CROSS_REF),
				   ((T_RVF_BUFFER**) &usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl))
#endif
			||
	    RVF_RED == rvf_get_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				   cnfg->nof_rx_ep * sizeof (T_USB_ENDPOINT_DESCR),
				   ((T_RVF_BUFFER**) &usb_env_ctrl_blk_p->endpoint_rx_descr_tbl))
			||
	    RVF_RED == rvf_get_buf(usb_env_ctrl_blk_p->prim_mb_id, 
				   cnfg->nof_tx_ep * sizeof (T_USB_ENDPOINT_DESCR),
				   ((T_RVF_BUFFER**) &usb_env_ctrl_blk_p->endpoint_tx_descr_tbl)))
	{
		USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
		return RVM_MEMORY_ERR;
	}

	physical_rx_ep_idx = 0;
	physical_tx_ep_idx = 0;

	/* FOREACH if in chosen cnfg */
	for (active_if_idx = 0; active_if_idx < cnfg->nof_if; active_if_idx++) {
		T_USB_INTERFACE_DATA *usb_if_tbl_entry = NULL;
		
		/* map from active if idx -> static if idx (in definition) */
		/* FOREACH static if definition */
		for (static_if_idx = 0; static_if_idx < USB_TOTAL_DIFF_IF; static_if_idx++) {
			if (if_cnfg_p[active_if_idx].ifdescr_p->bInterfaceNumber == 
					if_descr_table[static_if_idx].bInterfaceNumber) {
				usb_if_tbl_entry = usb_env_ctrl_blk_p->usb_if_tbl + static_if_idx;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
				usb_if_tbl_entry->interface_enabled = IF_ENABLED;
#endif
			}
		}
		if (usb_if_tbl_entry == NULL) {
			USB_SEND_TRACE("USB: cannot map from active if idx to static if idx", RV_TRACE_LEVEL_ERROR);
			usb_unlock();
			return RVM_INTERNAL_ERR;
		}
		/* Fill ep0 descriptor table */
		/* and interface table of endpoint 0 */
		endpoint_vs_interface_entry_p = 
					&(usb_env_ctrl_blk_p->ep0_endpoint_vs_interface_tbl[active_if_idx]);

		/* link back to interface */
		endpoint_vs_interface_entry_p->interface_data_p = usb_if_tbl_entry;
		/* link to endpoint descr table */
		endpoint_vs_interface_entry_p->endpoint_data.endpo_descr_p 
										= &(endpoint_descr_tbl_entry_p[active_if_idx]);

		endpoint_vs_interface_entry_p->endpoint_data.endpoint_number = 0;;
		endpoint_vs_interface_entry_p->buf_p		= NULL;
		endpoint_vs_interface_entry_p->temp_buf_p	= NULL;
		endpoint_vs_interface_entry_p->buf_size		= 0;
		/* FOREACH ep in this if */
		for (ep_idx = 0; ep_idx < if_cnfg_p[active_if_idx].ifdescr_p->bNumEndpoints; ep_idx++) {
			const T_USB_EP_DESCR* if_logical_ep_array_p = if_cnfg_p[active_if_idx].if_logical_ep_array_p;
			if (ep_is_rx(if_logical_ep_array_p[ep_idx])) {
				endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl + 
					physical_rx_ep_idx;
				endpoint_descr_tbl_entry_p = usb_env_ctrl_blk_p->endpoint_rx_descr_tbl + physical_rx_ep_idx++;
				pep_nr = physical_rx_ep_idx;
			} else {
				endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl + 
					physical_tx_ep_idx;
				endpoint_descr_tbl_entry_p = usb_env_ctrl_blk_p->endpoint_tx_descr_tbl + physical_tx_ep_idx++;
				pep_nr = physical_tx_ep_idx;
			}
			//really fill the endpoint descr table
			fill_ep_descr_tbl(endpoint_descr_tbl_entry_p, if_logical_ep_array_p[ep_idx], pep_nr);

			/* link back to interface */
			endpoint_vs_interface_entry_p->interface_data_p = usb_if_tbl_entry;
			/* link to endpoint descr table */
			endpoint_vs_interface_entry_p->endpoint_data.endpo_descr_p = endpoint_descr_tbl_entry_p;
			/* assign logical endpoint number */
			endpoint_vs_interface_entry_p->endpoint_data.endpoint_number = ep_idx + 1;
			/* defaults for endpoint buffer */
			endpoint_vs_interface_entry_p->buf_p = NULL;
			endpoint_vs_interface_entry_p->temp_buf_p = NULL;
			endpoint_vs_interface_entry_p->buf_size = 0;
		} /* END ep_idx loop */		
	} /* END active_if_idx loop */
	// usb_unlock();
	return RV_OK;
}
/*@}*/

/**
 * @name notify_swe_connect
 *
 * This function sends a T_USB_BUS_(DIS)CONNECTED_MSG to all SWEs that are active
 * in the current configuration
 *
 * @param	BOOL connect	TRUE = connect, FALSE is disconnect
 * @return	T_RV_RET:	RVM_MEMORY_ERR, RV_OK
 */
/*@{*/
T_RV_RET notify_swe_connect(BOOL connect)
{
#if (REMU==0)
	extern const T_RVM_CONST_SWE_INFO RVM_SWE_GET_INFO_ARRAY[];
       const T_RVM_CONST_SWE_INFO *swe_get_info_ptr = RVM_SWE_GET_INFO_ARRAY;		
#endif	   
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	UINT32 i,j;
	T_RV_RETURN_PATH *swe_connected[USB_TOTAL_DIFF_IF];
	UINT32 nof_connected_swe = 0;
	UINT32 if_idx;

	USB_SEND_TRACE("USB: notify_swe_connect ENTER", RV_TRACE_LEVEL_WARNING);

	// usb_lock();
	/* find and store return paths of all subscribers */
	/* FOREACH active interface */
	for (if_idx = 0; if_idx < USB_TOTAL_DIFF_IF; if_idx++) 
	{
		T_USB_INTERFACE_DATA *usb_if_tbl_entry = usb_env_ctrl_blk_p->usb_if_tbl + if_idx;
		swe_connected[if_idx] = NULL;
		if (usb_if_tbl_entry->interface_enabled == IF_ENABLED) {
			USB_TRACE_WARNING_PARAM("USB: notify_swe_connect: found subscriptor "
						"for interface ",
						usb_if_tbl_entry->interface_number);
			swe_connected[nof_connected_swe++] = &usb_if_tbl_entry->swe_return_path;
		}
	} /* END if_idx loop */
	/* FOREACH connected SWE */
	for (i=0;i<nof_connected_swe;i++) {
		/* skip duplicate return paths - a single SWE may have more than */
		/* one active interface */
		BOOL duplicate = FALSE;
		/* FOREACH connected SWE before this one (i) */
		//(j always < i) always < nof_connected_swe (for pclint lovers)
		for (j=0;j<i;j++) {
			if ((swe_connected[i] != NULL) && (swe_connected[j] != NULL) )
			{
				if(swe_connected[j]->addr_id == swe_connected[i]->addr_id)
				{
					duplicate = TRUE;
					break;
				}
			}
		}
		if (!duplicate && (swe_connected[i] != NULL)) {
			/* allocate '(dis)connected' msg */
			T_USB_BUS_CONNECTED_MSG *msg_p; /* or disconnect, same struct */
			if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
						sizeof (T_USB_BUS_CONNECTED_MSG),
						connect ? USB_BUS_CONNECTED_MSG: USB_BUS_DISCONNECTED_MSG,
						((T_RVF_MSG**) &msg_p))) {
				USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
				return RVM_MEMORY_ERR;
			}
			USB_TRACE_WARNING_PARAM("USB: notify_swe_connect: "
						"sending USB_BUS_(DIS)CONNECTED_MSG to addr id ",
						swe_connected[i]->addr_id);
			/* follow return path to send 'connected' msg */
			if (swe_connected[i]->callback_func) {
				swe_connected[i]->callback_func(msg_p);
				rvf_free_buf(msg_p);
			} else {
				if (RVF_OK != rvf_send_msg (swe_connected[i]->addr_id, msg_p)) {
					USB_SEND_TRACE("USB: could not send (dis)connect notify msg", 
							RV_TRACE_LEVEL_ERROR);
					return RVM_MEMORY_ERR;
				}
			}
		}
	}
	// usb_unlock();
	return RV_OK;
}
/*@}*/

/**
 * @name notify_swe_suspend_state
 *
 * This function sends a T_USB_BUS_SUSPEND_MSG to all SWEs that are active
 * in the current configuration
 *
 * @param	BOOL connect	TRUE = suspend, FALSE is resume
 * @return	T_RV_RET:	RVM_MEMORY_ERR, RV_OK
 */
/*@{*/
T_RV_RET notify_swe_suspend_state(BOOL suspend)
{
#if (REMU==0)
	extern	const T_RVM_CONST_SWE_INFO RVM_SWE_GET_INFO_ARRAY[];
	const T_RVM_CONST_SWE_INFO *swe_get_info_ptr = RVM_SWE_GET_INFO_ARRAY;
#endif	
	T_USB_ACTIVE_CNFG *cnfg = &usb_env_ctrl_blk_p->cnfg;
	UINT32 i,j;
	T_RV_RETURN_PATH *swe_connected[USB_TOTAL_DIFF_IF];

	
	UINT32 nof_connected_swe = 0;
	UINT32 if_idx;

	USB_SEND_TRACE("USB: notify_swe_suspend_state ENTER", RV_TRACE_LEVEL_WARNING);

	// usb_lock();
	/* find and store return paths of all subscribers */
	/* FOREACH active interface */
	for (if_idx = 0; if_idx < USB_TOTAL_DIFF_IF; if_idx++) 
	{
		T_USB_INTERFACE_DATA *usb_if_tbl_entry = usb_env_ctrl_blk_p->usb_if_tbl + if_idx;
		swe_connected[if_idx] = NULL;
		if (usb_if_tbl_entry->interface_enabled == IF_ENABLED) 
		{
			USB_TRACE_WARNING_PARAM("USB: notify_swe_suspend_state: found subscriptor "
						"for interface ",
						usb_if_tbl_entry->interface_number);

			swe_connected[nof_connected_swe++] = &usb_if_tbl_entry->swe_return_path;
		}
	} /* END if_idx loop */

	/* FOREACH connected SWE */
	for (i=0;i<nof_connected_swe;i++) 
	{
		/* skip duplicate return paths - a single SWE may have more than */
		/* one active interface */
		BOOL duplicate = FALSE;

		/* FOREACH connected SWE before this one (i) */
		//(j always < i) always < nof_connected_swe (for pclint lovers)
		for (j=0; j<i; j++) 
		{
			if ( (swe_connected[i] != NULL) && (swe_connected[j] != NULL) )
				
			{
				if (swe_connected[j] == swe_connected[i])
				{
					duplicate = TRUE;
					break;
				}
			}
		}

		if (!duplicate && swe_connected[i] != NULL) 
		{
			/* allocate '(dis)connected' msg */
			T_USB_BUS_SUSPEND_MSG *msg_p; /* or disconnect, same struct */

			if (RVF_RED == rvf_get_msg_buf(usb_env_ctrl_blk_p->prim_mb_id, 
						sizeof (T_USB_BUS_SUSPEND_MSG),
						suspend ? USB_BUS_SUSPEND_MSG: USB_BUS_RESUME_MSG,
						((T_RVF_MSG**) &msg_p))) 
			{
				USB_SEND_TRACE("USB: out of memory", RV_TRACE_LEVEL_ERROR);
				return RVM_MEMORY_ERR;
			}

			if(suspend == TRUE)
			{
				USB_TRACE_WARNING_PARAM("USB: notify_swe_suspend_state: "
							"sending USB_BUS_SUSPEND_MSG to addr id ",
							swe_connected[i]->addr_id);
			}
			else
			{
				USB_TRACE_WARNING_PARAM("USB: notify_swe_suspend_state: "
							"sending USB_BUS_RESUME_MSG to addr id ",
							swe_connected[i]->addr_id);
			}

			/* follow return path to send 'connected' msg */
			if (swe_connected[i]->callback_func) 
			{
				swe_connected[i]->callback_func(msg_p);
				rvf_free_buf(msg_p);
			} 
			else 
			{
				if (RVF_OK != rvf_send_msg (swe_connected[i]->addr_id, msg_p)) 
				{
					USB_SEND_TRACE("USB: could not send (dis)connect notify msg", 
							RV_TRACE_LEVEL_ERROR);

					return RVM_MEMORY_ERR;
				}
			}
		}
	}

	// usb_unlock();
	return RV_OK;
}
/*@}*/
/****************************************************************************************
 |	INTERNAL FUNCTIONS																	|
 V**************************************************************************************V*/
UINT8 lep_2_pep(UINT8 lep, UINT8 if_nr, BOOL rx_tx)
{
	UINT8 i = 255; 
	UINT8 nr_eps;
	T_USB_CROSS_REF *endpoint_vs_interface_entry_p;

	if((USB_CNFGNR != USB_CONF_DEFAULT) &&
		(USB_CNFGNR != USB_CONF_ADRESSED)) {

		if(rx_tx) {
			
		if(usb_env_ctrl_blk_p->cnfg.nof_rx_ep < USB_MAX_DIR_EP) {
			nr_eps = usb_env_ctrl_blk_p->cnfg.nof_rx_ep;
		}
		else {
			nr_eps = USB_MAX_DIR_EP;
		}
			
			endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl;
		}
		else {

			if(usb_env_ctrl_blk_p->cnfg.nof_tx_ep < USB_MAX_DIR_EP) {
				nr_eps = usb_env_ctrl_blk_p->cnfg.nof_tx_ep;
			}
			else {
				nr_eps = USB_MAX_DIR_EP;
			}
			
			endpoint_vs_interface_entry_p = usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl;
		}
		for(i = 0; i < nr_eps; i++) { 
			if(endpoint_vs_interface_entry_p[i].interface_data_p->interface_number == if_nr) { 
				if(endpoint_vs_interface_entry_p[i].endpoint_data.endpoint_number == lep) {
					break;
				}
			}
		}
	}
	else {
		USB_SEND_TRACE("USB: cannot determine PEP for invalid configuration",
		RV_TRACE_LEVEL_ERROR);	
	}
	return(i);
}

/*
	Input: 	Product id
			Logical interface id
	Output:	Physical interface id
*/
INT8 lif_2_pif(INT16 prod_id, INT8 lif)
{
	INT8 pif = -1;
	UINT32 nof_if = usb_env_ctrl_blk_p->cnfg.nof_if;

	UINT8 config;
	UINT8 bit_shifted;
	UINT8 ifCnt=0;
	UINT8 ifaceArrayIndex=0;
	UINT8 enummask=0x1;

#if 0	
	if(nof_if < 3)
	{
		config = prod_id;
		prod_id = 0;
		ifaceArrayIndex = 0;
		while((bit_shifted = config>>ifaceArrayIndex)){
			if(bit_shifted&0x1)
			{
				prod_id |= enummask<<ifCnt;
				ifCnt++;
			}
			ifaceArrayIndex++;
		}
	}
#endif	
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	NORMAL(prod_id, nof_if);
	switch(prod_id)
	{
/* Prod ID corresponds to three interface */	
//		case 0x1234:
		case 7:
			pif = lif;
			break;
/* !Prod ID corresponds to three interface */			
/* Prod ID corresponds to one interface */			
		case 1:
			if(lif == 0)
			{
				pif = 0;
			}else{
				pif = -1;
			}
			break;
		case 2:
			if(lif == 1)
			{
				pif = 0;
			}else{
				pif = -1;
			}
			break;
		case 4:
			if(lif == 2)
			{
				pif = 0;
			}else{
				pif = -1;
			}
			break;
/* !Prod ID corresponds to one interface */
/* Prod ID corresponds to two interface */
		case 3:
			if(lif == 0)
			{
				pif = 0;
			}else if(lif == 1){
				pif = 1;
			}else{
				pif = -1;
			}
			break;
		case 5:
			if(lif == 0)
			{
				pif = 0;
			}else if(lif == 2){
				pif = 1;
			}else{
				pif = -1;
			}
			break;
		case 6:
			if(lif == 1)
			{
				pif = 0;
			}else if(lif == 2){
				pif = 1;
			}else{
				pif = -1;
			}
			break;
/* !Prod ID corresponds to two interface */
		default:
			pif = -1;
			break;
	}
#else
	pif = lif;
#endif
	return pif;
}
/*
	Input: 	Product id
			Logical interface id
	Output:	Physical interface id
*/
INT8 pif_2_lif(INT16 prod_id, INT8 pif)
{
	INT8 lif = -1;
	UINT32 nof_if = usb_env_ctrl_blk_p->cnfg.nof_if;

	UINT8 config;
	UINT8 bit_shifted;
	UINT8 ifaceArrayIndex=0;
	UINT8 ifCnt = 0;
	UINT8 enummask=0x1;

#if 0	
	if(nof_if < 3)
	{
		config = prod_id;
		prod_id = 0;
		ifaceArrayIndex = 0;
		while((bit_shifted = config>>ifaceArrayIndex)){
			if(bit_shifted&0x1)
			{
				prod_id |= enummask<<ifCnt;
				ifCnt++;
			}
			ifaceArrayIndex++;
		}
	}	
#endif	
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
	NORMAL(prod_id, nof_if);
	switch(prod_id)
	{
/* Prod ID corresponds to three interface */	
//		case 0x1234:
		case 7:
			lif = pif;
			break;
/* !Prod ID corresponds to three interface */			
/* Prod ID corresponds to one interface */			
		case 1:
			if(pif == 0)
			{
				lif = 0;
			}else{
				lif = -1;
			}
			break;
		case 2:
			if(pif == 0)
			{
				lif = 1;
			}else{
				lif = -1;
			}
			break;
		case 4:
			if(pif == 0)
			{
				lif = 2;
			}else{
				lif = -1;
			}
			break;
/* !Prod ID corresponds to one interface */
/* Prod ID corresponds to two interface */
		case 3:
			if(pif == 0)
			{
				lif = 0;
			}else if(pif == 1){
				lif = 1;
			}else{
				lif = -1;
			}
			break;
		case 5:
			if(pif == 0)
			{
				lif = 0;
			}else if(pif == 1){
				lif = 2;
			}else{
				lif = -1;
			}
			break;
		case 6:
			if(pif == 0)
			{
				lif = 1;
			}else if(pif == 1){
				lif = 2;
			}else{
				lif = -1;
			}
			break;
/* !Prod ID corresponds to two interface */
		default:
			lif = -1;
			break;
	}
#else
	lif = pif;
#endif
	return lif;
}
