/**
 * @file	usb_i.h
 *
 * Internal definitions for USB.
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

#ifndef __USB_INST_I_H_
#define __USB_INST_I_H_



#include "config/swconfig.cfg"
#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"
#include "dar/dar_gen.h" 
#include "dar/dar_api.h"

#include "usb/usb_config.h"
#include "usb/usb_api.h"
#include "usb/usb_message.h"

#include "usb/usb_state_i.h"

#include "usb/usb_interface.h"
#include "chipset.cfg"
#include "nucleus.h"

#if (CHIPSET==15)
#include "dma/dma_api.h"
#else
#include "dmg/dmg_api.h"
#endif

/****************************************************************************************
 |	DEFINITIONS																			|
 V**************************************************************************************V*/

#define USB_RX				TRUE
#define USB_TX				FALSE

#define USB_NON_VALID_SWE	0

#define USB_RX_EP0_INT		0
#define USB_TX_EP0_INT		1
#define USB_SETUP_EP0_INT	2

#define USB_ZERO_BMP		0x0000
#define IF_DISABLED			FALSE
#define	IF_ENABLED			TRUE

#ifdef SYS_DMA
#define	USB_DMA_USED
#endif

#define	DMA_RX_CHANNELS		3	/* Maximum number of DMA channels for receiption	*/
#define	DMA_TX_CHANNELS		3	/* Maximum number of DMA channels for transmission	*/

#define DMA_INVALID_ENDPOINT	255		/* endpoint 255 is invalid in case of DMA transfers	*/

#define WAKE_UP_TIME_IN_TDMA  10		/* timer reception for a DMA channel 23 ms */

#define	DMA_TX_THRESHOLD	512
#define	DMA_RX_THRESHOLD	512		/* All receiption must be made in DMA mode */
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
#endif

/****************************************************************************************
 |	MACRO DEFINITIONS																	|
 V**************************************************************************************V*/

#if (REMU==1)
	#define USB_ENABLE_TRACE	0
#else
	#define USB_ENABLE_TRACE	0
#endif

#if ((TRACE_ON_USB == 0) && (USB_ENABLE_TRACE == 1))
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x
	#define USB_SEND_TRACE(string, trace_level) 	rvf_send_## trace_level(string)
	#define USB_TRACE_WARNING_PARAM(string, param) { \
		rvf_send_RV_TRACE_LEVEL_WARNING(string ## "0x%X", param); }
#else
#define USB_SEND_TRACE(string, trace_level) \
		rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, USB_USE_ID)
	#define USB_TRACE_WARNING_PARAM(string, param) { \
		rvf_send_trace((string), sizeof(string) - 1, param, RV_TRACE_LEVEL_WARNING, USB_USE_ID); }
#endif
#else
#define USB_SEND_TRACE(string, trace_level)
#define USB_TRACE_WARNING_PARAM(string, param)
#endif
	
#define usb_lock()  rvf_lock_mutex (&usb_env_ctrl_blk_p->mutex)
#define usb_unlock()  rvf_unlock_mutex (&usb_env_ctrl_blk_p->mutex)

#define usb_dma_lock()		rvf_lock_mutex		(&usb_env_ctrl_blk_p->dma_mutex)
#define usb_dma_unlock()	rvf_unlock_mutex	(&usb_env_ctrl_blk_p->dma_mutex)
	
#define USB_CNFGNR			usb_env_ctrl_blk_p->host_data.current_cnfg_nr
#define USB_CNFGNR_2INDX	(USB_CNFGNR - 1)

#define USB_PEP_INDX(pep)	(pep - 1)
#define USB_LEP_INDX(lep)	(lep - 1)

#define USB_CURRENT_CNFG_NR_IF	configurations_table[USB_CNFGNR_2INDX].\
									if_cfg_descr_p[USB_CNFGNR_2INDX].\
									bNumInterfaces

#define LEP_2_PEP(lep, ifnr, rx_tx) lep_2_pep(lep, ifnr, rx_tx)
#define PEP_2_LEP(phys_ep, rx_tx)	(rx_tx == TRUE ? usb_env_ctrl_blk_p->rx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep)].\
													endpoint_data.endpoint_number : \
													usb_env_ctrl_blk_p->tx_endpoint_vs_interface_tbl[USB_PEP_INDX(phys_ep)].\
													endpoint_data.endpoint_number)

#define USB_TESTREG(A)	(*(volatile UINT16*)(A))	/* Let op, als je een 16 bit addressering hebt.*/

#define USB_DMA_CFG			usb_env_ctrl_blk_p->dma_cfg

/****************************************************************************************
 |	FUNCTIONS	DECLARATIONS															|
 V**************************************************************************************V*/

T_RV_RET EndpointConfiguration( void );
void reset_registers(void);
T_RV_RET prepare_rx_trsfr(UINT8 pep);
T_RV_RET create_usb_msg(UINT32 message_size, T_RVF_MSG_ID msg_id, T_RVF_MSG ** pp_msg);
UINT8 w2fc_rev(void);
T_RV_RET enable_if_hw_ep(T_USB_FM_SUBSCRIBE_MSG* msg_p);
T_RV_RET disable_if_hw_ep(T_USB_FM_UNSUBSCRIBE_MSG* msg_p);
T_RV_RET create_endpoint_tables(void);
T_RV_RET init_physical_eps(void);

UINT8 lep_2_pep(UINT8 lep, UINT8 if_nr, BOOL rx_tx);

/****************************************************************************************
 |	TYPE DEFINITIONS																	|
 V**************************************************************************************V*/


/**
 * @name Endpoint descriptor
 *
 * This type definition describes the data type that teh USB spec 1.1 forces
 * to use for communication with the USB HOST.
 */
/*@{*/
typedef struct
{
	UINT8	blength;
	UINT8	bDescriptorType;
	UINT8	bEndpointAddress;
	UINT8	bmAttributes;
	UINT16	wMaxPacketSize;
	UINT8	iInterval;
}T_USB_ENDPOINT_DESCR;
/*@}*/
 
 /**
 * @name Endpoint data
 *
 * This type definition describes a data type holds all endpoint specific data
 * such as the interface that belongs with the endpoint 
 */
/*@{*/
typedef struct
{
	UINT8					endpoint_number;
	T_USB_ENDPOINT_DESCR*	endpo_descr_p;
}T_USB_ENDPOINT_DATA;
/*@}*/

/**
 * @name Interface data
 *
 * This type definition describes a data type holds all interface specific data
 * such as te endpoints that belong with the interface and whether the interface is 
 * initialised or not
 */
/*@{*/
typedef struct
{
	UINT8					interface_number;
	T_USB_ENDPOINT_DATA*	pntr_to_if_ep_descr_p;			/*pointer to an array of pointers to ep descriptors 
															for every ep of this specific interface*/
	UINT8					interface_enabled;
	T_RV_RETURN_PATH		swe_return_path;				/*when this SWE is not riviera complient the
															  function is used for callback, else the swe id 
															  will be used*/
}T_USB_INTERFACE_DATA;
/*@}*/

/**
 * @name cross reference table type
 *
 * This type definition links the endpoint to the correct interface
 */
/*@{*/
typedef struct
{
	T_USB_ENDPOINT_DATA		endpoint_data;
	T_USB_INTERFACE_DATA*	interface_data_p;
	UINT8 *buf_p;
	UINT8 *temp_buf_p;		//pointer to the last position in the provided 
							//buffer where next data can be stored
	UINT16 buf_size;
	BOOL shorter_transfer;
}T_USB_CROSS_REF;
/*@}*/

/**
 * @name T_USB_STANDARD_REQ
 *
 * This type contains all possible standard requests
 */
/*@{*/
typedef enum
{
	GET_STATUS =  0,
	CLEAR_FEATURE,
	RESERVED,
	SET_FEATURE,
	RESERVED1,
	SET_ADRESS,
	GET_DESCRIPTOR,
	SET_DESCRIPTOR,
	GET_CONFIGURATION,
	SET_CONFIGURATION,
	GET_INTERFACE,
	SET_INTERFACE,
	SYNCH_FRAME,
	INVALID_REQUEST = 0xFF	
}T_USB_STANDARD_REQ;
/*@}*/

/**
 * @name T_USB_VENDOR_REQ
 *
 * Vendor requests are managed by FM
 */
/*@{*/
typedef enum
{
	VENDOR_HOST_TO_DEVICE = 0,
	VENDOR_DEVICE_TO_HOST
	
}T_USB_VENDOR_REQ;
/*@}*/

/**
 * @name T_USB_CLASS_REQ
 *
 * Class requests are managed by FM
 */
/*@{*/
typedef enum
{
	CLASS_HOST_TO_DEVICE = 0,
	CLASS_DEVICE_TO_HOST
}T_USB_CLASS_REQ;
/*@}*/


/**
 * @name T_USB_REQ_TYPE
 *
 * This type contains all possible USB request types
 */
/*@{*/
typedef enum
{
	USB_STANDARD = 0,
	USB_CLASS,
	USB_VENDOR,
	USB_RESERVED
}T_USB_REQ_TYPE;
/*@}*/

/**
 * @name T_USB_REQ_RECIPIENT
 *
 * This type contains all possible USB request recipent
 */
/*@{*/
typedef enum
{
	USB_REC_DEVICE		= 0,
	USB_REC_INTERFACE	= 1,
	USB_REC_ENDPOINT	= 2,
	USB_REC_OTHER		= 3
}T_USB_RECIPIENT_TYPE;
/*@}*/

/**
 * @name T_SPECIFIC_EP_OPERATION
 *
 * This type contains all possible USB request recipent
 */
/*@{*/
typedef enum
{
	USB_CDC_EP0_NOOP	= 0,
	USB_CDC_EP0_TX		= 1,
	USB_CDC_EP0_RX		= 2
}T_SPECIFIC_EP_OPERATION;
/*@}*/


/**
 * @name T_USB_DEV_REQ
 *
 * This type definition describes what the USB device requests are like
 * and should be used to store request's
 */
/*@{*/
typedef struct
{
	UINT8				bmRequestType;			//identifies the characteristics of the specific request
	T_USB_STANDARD_REQ	bRequest;				//specifies the particular request
	UINT16				wValue;					//contents vary according to the request
	UINT16				wIndex;					//contents vary acoording to the request (passing params)
	UINT16				wLength;				//specifies the length of data transferred
}T_USB_DEV_REQ;
/*@}*/

/**
 * @name T_USB_SETUP
 *
 * This type is used to create setup and EP0 interrupt handle functions
 */
/*@{*/
typedef void (*STND_FUNC)(void);
/*@}*/

/**
 * @name T_USB_SETUP
 *
 * This type contains all endpoint 0 setup related data
 */
/*@{*/
typedef struct 
{
	T_USB_DEV_REQ*	request_p;			/*pointer to setup request buffer*/
	BOOL			setup_out;			/*control flag indicating if a setup OUT 
										transaction is ongoing*/
	BOOL			setup_in;			/*control flag indicating if a setup IN
										transaction is ongoing*/
	STND_FUNC		current_setup_req;	//setup request curently being processed
	UINT16			total_nr_trsf_req;	//total number of transactions to complete 
										//this setup request
	UINT16			nr_of_trsf_proc;	//number of transactions allready processed
										//for this setup request
	UINT8			rx_or_tx_ep0_int;	//0 when RX ep0 interrupt occured
										//1 when TX ep0 interrupt occured
										//2 when Setup ep0 interrupt occurred
	UINT8			if_nbr;				//interface number in charge of the Setup Request
}T_USB_SETUP;
/*@}*/

typedef struct {
	UINT32 nof_rx_ep;
	UINT32 nof_tx_ep;
	UINT32 nof_if;
} T_USB_ACTIVE_CNFG;

/**
 * @name T_USB_DMA_REQUEST_INFORMATION
 *
 * Information about a USB DMA transsaction
 */
/*@{*/
typedef struct
{
	UINT16	W2FC_DMA_Frame0;
	UINT16	W2FC_DMA_Element0;
	UINT8	W2FC_TXDMA_EOTB0;
	UINT16	W2FC_TXDMA_XSWL0;
	UINT16	W2FC_TXDMA_FBT0;
	UINT8	W2FC_TXDMA_fEOT0;
	UINT8	*CHB_buffer_p;
} T_USB_DMA_REQUEST_INFORMATION;
/*@}*/

/**
 * @name T_USB_ENDPOINT
 *
 * Endpoint identifier
 */
/*@{*/
typedef struct
{
	UINT8							interface_id;
	UINT8							endpoint_id;
	UINT8							subscription_order;
#if (CHIPSET==15)
	T_DMA_CHANNEL					dmg_channel;
#else
	T_DMG_CHANNEL					dmg_channel;
#endif
	T_USB_DMA_REQUEST_INFORMATION	usb_dma_transfer_info;
	UINT8 							channel_used;
} T_USB_DMA_ENDPOINT;
/*@}*/

/**
 * @name T_DMA_USB_CFG
 *
 * Cross reference table between endpoint and DMA channels
 */
/*@{*/
typedef struct
{
	T_USB_DMA_ENDPOINT	dma_usb_endpoint_rx_tbl	[DMA_RX_CHANNELS];
	T_USB_DMA_ENDPOINT	dma_usb_endpoint_tx_tbl	[DMA_TX_CHANNELS];

	NU_TIMER			dma_usb_rx_timer		[DMA_RX_CHANNELS];
	UINT16				dma_usb_rx_memo_size	[DMA_RX_CHANNELS];
} T_DMA_USB_CFG;
/*@}*/


/**
 * @name T_USB_HOST_DATA
 *
 * This type definition contains USB host specific data
 */
/*@{*/
typedef struct
{
	T_USB_HW_STATE			state_prior_to_sus;		/*contans the state before the driver 
													was suspended*/
	BOOL					usb_connected;			/*indicates whether the usb  bus is 
													actually connected*/
	BOOL					usb_rem_wake_enabled;	/*indicates whether remote wakeup is 
													allowed by the 
													USB host or not*/
	UINT8					current_cnfg_nr;		/*current configuration number starts 
													with 1, 0 means that device is in its 
													addressed state (default is -1)*/	
	UINT16					usb_ds_chg_mem;			/*variable that holds the last value 
													of the devstat register*/
	T_USB_HW_STATE			hw_state;				/*current hardware state of the USB*/
	UINT8*					ep0_data_p;				/*pointer to endpoint 0 buffer*/
	T_USB_SETUP				ep0_setup;				/*variable containing setup related data*/
	BOOL					fifo_not_full;			//if DB is set or fifo not full	
}T_USB_HOST_DATA;
/*@}*/

/**
 * @name T_USB_TRACK_MISSING_ZLP
 *
 * This type definition contains USB host specific data
 */
/*@{*/
typedef struct
{
	UINT32 pep_idx;
	UINT32 size;
	BOOL end_of_packet;
}T_USB_TRACK_MISSING_ZLP;
/*@}*/


/**
 * The Control Block buffer of USB, which gathers all 'Global variables'
 * used by USB instance.
 *
 * A structure should gather all the 'global variables' of USB instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_USB_ENV_CTRL_BLK buffer is allocated when creating USB instance and is 
 * then always refered by USB instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
	/** Store the current state of the USB instance */
	T_USB_INTERNAL_STATE state;

	/**return path for other SWE to the USB*/
	T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name,	
							 T_RVM_RETURN error_cause,
							 T_RVM_ERROR_TYPE error_type,
							 T_RVM_STRING error_msg);
	/** Mem bank id. */
	T_RVF_MB_ID				prim_mb_id;

	T_RVF_ADDR_ID			addr_id;

	T_USB_HOST_DATA			host_data;		/*contain USB HOST specific data*/

	T_RV_RETURN_PATH		usb_return_path;
	BOOL					driver_ready;	/*indicates whether the driver is fully initialised
											  and configured*/

	T_USB_CROSS_REF*		rx_endpoint_vs_interface_tbl; /*table containing the link between the 
															logical and physical rx endpoints*/

	T_USB_CROSS_REF*		tx_endpoint_vs_interface_tbl; /*table containing the link between the 
															logical and physical tx endpoints*/
	T_USB_CROSS_REF*		ep0_endpoint_vs_interface_tbl; /*table containing the link between the 
															logical and physical endpoint 0*/

	T_USB_ENDPOINT_DESCR *endpoint_rx_descr_tbl;
	T_USB_ENDPOINT_DESCR *endpoint_tx_descr_tbl;
	T_USB_INTERFACE_DATA*	usb_if_tbl;		/*table of interface data dynamically created depending 
											on the chosen configuration*/
	T_USB_ACTIVE_CNFG		cnfg;
	T_RVF_MUTEX mutex;
	T_RVF_MUTEX dma_mutex;
	UINT16					bm_rx_ep_buf_stall;	/*indicates a physical endpoint buffer stall, starts with bit 1*/
	UINT16					bm_tx_ep_buf_stall;	/*indicates a physical endpoint buffer stall, starts with bit 1*/
	
	T_DMA_USB_CFG			dma_cfg;				/* defines cross ref table between endpoints and DMA channel */
	T_RVF_TIMER_ID						timer;	
	T_USB_TRACK_MISSING_ZLP	trackMissingZLP;
#ifdef USB_DYNAMIC_CONFIG_SUPPORT	
	T_USB_EVENT_CALLBACK                     ntfcCallback;       /* Callback to notify  events to Application */
	void									*ntfcCtx;			/* Context Information */
	T_USB_ENUM_INFO                          enumInfo;			/* Enum Info Provided by Application */
#endif	
} T_USB_ENV_CTRL_BLK;



/**
 * @name T_USB_EP_BUFFERS
 *
 * This type definition describes a data type holds all endpoint specific data
 * buffers
 */
/*@{*/
typedef struct
{
	UINT8**		rx_ep_array_pp;
	UINT8**		ctrl_rx_ep_array_pp;
	UINT8**		tx_ep_array_pp;
	UINT8**		ctrl_tx_ep_array_pp;
}T_USB_EP_BUFFERS;
/*@}*/

/**
 * @name T_USB_RESP_TYPE
 *
 * This type definition specifies all responses possibly given or recieved to 
 * or from the USB HOST
 * 
 */
/*@{*/
typedef enum
{
	USB_ACK,
	USB_STALL,
	USB_NAK
}T_USB_RESP_TYPE;
/*@}*/

/**
 * @name T_USB_DESCR_TYPE
 *
 * This type definition specifies all descriptors defined by the USB
 * 
 */
/*@{*/
typedef enum
{
	USB_DEVICE = 1,
	USB_CONFIGURATION,
	USB_STRING,
	USB_INTERFACE,
	USB_ENDPOINT
}T_USB_DESCR_TYPE;
/*@}*/

/**
 * @name T_USB_RETURN_PATH
 *
 * This type definition describes the data type to be used as a return path
 */
/*@{*/
typedef struct
{
	T_RVF_ADDR_ID	addr_id;
	void			(*callback_func)(T_RV_HDR *);
} T_USB_RETURN_PATH;
/*@}*/

/**
 * @name T_USB_NR_ACTIVE_FM
 *
 * This type definition describes number of interface that are controlled
 * by a single FM
 */
/*@{*/
typedef struct
{
	T_RVF_ADDR_ID	addr_id;
	UINT8			nr_of_fm;
} T_USB_NR_ACTIVE_FM;
/*@}*/


/**
 * @name T_USB_IF_EP_NR
 *
 * This type definition describes the data type that is used 
 * to combine ep and if info
 */
/*@{*/
typedef struct
{
	T_USB_RETURN_PATH	interface_return_path;//sort of a long term relationship with an interface
	UINT8				nr_of_rx_ep;		//every interface has a unique number of rx ep's
	UINT8				nr_of_rx_ctrl_ep;	//every interface has a unique number of control rx ep's
	UINT8				nr_of_tx_ep;		//every interface has a unique number of tx ep's
	UINT8				nr_of_tx_ctrl_ep;	//every interface ahs a unique number of control ep's
}T_USB_IF_EP_NR;
/*@}*/

/**
 * @name T_USB_SWE_IF_T
 *
 * This type definition is used to link interface numbers
 * to SWEs, so that relevant SWEs can be informed of cable connects
 */
/*@{*/
typedef struct {
	T_RVM_USE_ID use_id;
	UINT8 if_num;
} T_USB_SWE_IF_T;
/*@}*/

/****************************************************************************************
 |	VARIABLE DEFINITIONS																|
 V**************************************************************************************V*/


extern const T_USB_ENDPOINT_DESCR endpoint_rx_descr_tbl[];

extern const T_USB_ENDPOINT_DESCR endpoint_tx_descr_tbl[];

/** External ref "global variables" structure. */
extern T_USB_ENV_CTRL_BLK	*usb_env_ctrl_blk_p;

extern const T_USB_CROSS_REF rx_endpoint_vs_interface_tbl[];

extern const T_USB_CROSS_REF tx_endpoint_vs_interface_tbl[];

extern const T_USB_CROSS_REF ep0_endpoint_vs_interface_tbl[];

extern const T_USB_DEVICE_DESCR usb_device_descr;

extern USB_DYN_CONST T_USB_INTERFACE_DESCR if_descr_table[];
extern T_USB_STRING_DESCR string_descr_lang_id;
extern T_USB_STRING_DESCR string_descr_manufacturer;
extern T_USB_STRING_DESCR string_descr_product;
extern T_USB_STRING_DESCR string_descr_ser_num;

extern UINT16 usb_ds_chg_mem;

extern const T_USB_SWE_IF_T usb_swe_tbl[];


extern T_RV_RET notify_swe_connect		(BOOL connect);
extern T_RV_RET notify_swe_suspend_state(BOOL suspend);

extern T_RV_RET conf_if_xrefs(UINT32 cfg_idx);
#endif /* __USB_INST_I_H_ */




