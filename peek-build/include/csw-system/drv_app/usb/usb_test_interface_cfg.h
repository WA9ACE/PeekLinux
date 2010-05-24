/**
 * @file	usb_test_interface_cfg.h
 *
 * Configuration definitions for the USB interfaces (e.g. modem,file storage,sync,etc.).
 * This file is used by both the USB as well as the swe that wants to
 * make use of this driver and has to provide the specified buffers
 *
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __USB_TEST_INTERFACE_CFG_H_
#define __USB_TEST_INTERFACE_CFG_H_


#include "rv/rv_general.h"		/* General Riviera definitions. */
#include "usb/usb_config.h"
#include "config/swconfig.cfg"
#include "config/chipset.cfg"
#include "rv/rv_defined_swe.h"


/* DO NOT MODIFY! THIS WILL DISABLE THE DRIVER ON THE HOST SIDE!*/
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
#define USB_DYN_CONST
#else
#define USB_DYN_CONST const
#endif

/*--------------------------------------------------------------------------------------*/
/* GENERAL DEFINITIONS																	*/
/*--------------------------------------------------------------------------------------*/
#define USB_TOT_SWE		1
#define USB_OUT			  0x00
#define USB_IN			  0x80

/*--------------------------------------------------------------------------------------*/
/* DEVICE specific defines																      */
/*--------------------------------------------------------------------------------------*/

#define USB_PRODUCT_ID		0x1234 //0x0702  Modified to add Philog driver VID/PID
#define USB_VENDOR_ID		0xFACE //0x2811  
#define USB_BCD_DEV				0x0001	/*DO NOT MODIFY!*/

#define USB_DEV_NUM_CFG		1		/*number of configurations curremtly supported by this decvice*/

#define USB_MAX_DIR_EP		15		  /*max nr of endpoint per direction*/
#define USB_MAX_EP			30

#define USB_EP_DESCR_LGNT	0x07    /*length of endpoint descriptor*/
#define USB_DEF_INT			0	    /*default interval for endpoint*/
#define USB_STND_INT		1		/*standard interval for Interrupt & ISO endpoint*/
/*--------------------------------------------------------------------------------------*/
/*configuration 1 defines																*/
/*--------------------------------------------------------------------------------------*/
#define USB_CFG1_VALUE			1	/*next configuration cannot have the same value*/
#define USB_CFG1_STR			conf1_descr_str
#define USB_CFG1_ATTRIBUTES		0xc0
#define USB_CFG1_MAX_POWER		50	/* current drawn from the usb bus (100ma)*/

//--> USB Multiple Configurations

#ifdef USB_DYNAMIC_CONFIG_SUPPORT
#define USB_IF_TOTAL_NR_EP 3
#endif

#if (TRACE_ON_USB == 1) &&  defined(RVM_USBFAX_SWE) &&  defined(RVM_USBMS_SWE) 
#define 	USB_TOTAL_DIFF_IF 3
#define USB_CFG1_NUM_INTERF 3
#define USB_IF_SERIAL_TRACE 0
#define USB_IF_CDC_SERIAL 1
#define USB_IF_MSTORAGE 2
#define NORMAL(x1, x2) x1 = x1

#elif (TRACE_ON_USB == 1) &&  defined(RVM_USBFAX_SWE)
#define 	USB_TOTAL_DIFF_IF 2
#define USB_CFG1_NUM_INTERF 2
#define USB_IF_SERIAL_TRACE 0
#define USB_IF_CDC_SERIAL 1
#define NORMAL(x1, x2) x1 = x1

#elif defined(RVM_USBFAX_SWE) && defined(RVM_USBMS_SWE) 
#define 	USB_TOTAL_DIFF_IF 2
#define USB_CFG1_NUM_INTERF 2
#define USB_IF_CDC_SERIAL 0
#define USB_IF_MSTORAGE 1
#define NORMAL(x1, x2) x1 = x1>>1;

#elif (TRACE_ON_USB == 1) &&  defined(RVM_USBMS_SWE)
#define 	USB_TOTAL_DIFF_IF 2
#define USB_CFG1_NUM_INTERF 2
#define USB_IF_SERIAL_TRACE 0
#define USB_IF_MSTORAGE 1
#define NORMAL(x1, x2) x1 = x1-x2

#elif (TRACE_ON_USB == 1) 
#define 	USB_TOTAL_DIFF_IF 1
#define USB_CFG1_NUM_INTERF 1
#define USB_IF_SERIAL_TRACE 0
#define USB_IF_CDC_SERIAL 1 //wupeng
#define NORMAL(x1, x2) x1 = x1

#elif defined(RVM_USBFAX_SWE)
#define 	USB_TOTAL_DIFF_IF 1
#define USB_CFG1_NUM_INTERF 1
#define USB_IF_CDC_SERIAL 0
#define NORMAL(x1, x2) x1 = x1

#elif defined(RVM_USBMS_SWE) 
#define 	USB_TOTAL_DIFF_IF 1
#define USB_CFG1_NUM_INTERF 1
#define USB_IF_MSTORAGE 0
#define NORMAL(x1, x2) x1 = x1

#else
#define 	USB_TOTAL_DIFF_IF 0
#define USB_CFG1_NUM_INTERF 0
#endif

#define DUMMY_USB_CONFIG 0
/*--------------------------------------------------------------------------------------*/
/*configuration 2 defines																*/
/*--------------------------------------------------------------------------------------*/
#define USB_CFG2_NUM_INTERF		5
#define USB_CFG2_VALUE			2	/*next configuration cannot have the same value*/
#define USB_CFG2_STR			conf2_descr_str
#define USB_CFG2_ATTRIBUTES		0xE0 /*see usb spec 1.1 chapter 9 for info on this value*/
#define USB_CFG2_MAX_POWER		60	/*current drawn from the usb bus (120ma) */

/*--------------------------------------------------------------------------------------*/
/*TEST interface 1 defines This interface is used in data transfer! */
/*--------------------------------------------------------------------------------------*/

#define USB_IF_TEST1				0

#define USB_IF1_ALT_SET				0		/*alternate setting*/
#define USB_IF1_TOTAL_NR_EP			3 		/*total of endpoints being used*/
#define USB_CDC_SERIAL_CLASS		0x02	/* vendor sepcific interface. */
#define USB_CDC_SERIAL_SUB_CLASS	0x02	/* we do not have a subclass. */
#define USB_CDC_SERIAL_PROTOCOL		0x00	/*usb protocol used for this interface*/
// #define USB_CDC_SERIAL_IF_STR		cdc_serial_emulation_if_str		

/*--------------------------------------------------------------------------------------*/
/*interface 2 defines																	*/
/*--------------------------------------------------------------------------------------*/
#define USB_IF_CDC_DATA				1
#define USB_IF2_ALT_SET				0		/*alternate setting*/
#define USB_IF2_TOTAL_NR_EP			3	
#define USB_CDC_DATA_CLASS			0x0A
#define USB_CDC_DATA_SUB_CLASS		0x00
#define USB_CDC_DATA_PROTOCOL		0x00
// #define USB_CDC_DATA_IF_STR			cdc_data_if_str

/*--------------------------------------------------------------------------------------*/
/*interface 3 defines																	*/
/*--------------------------------------------------------------------------------------*/
				
#define USB_IF3_ALT_SET			0	/*alternate setting*/
#define USB_IF3_TOTAL_NR_EP		3
#define USB_MSTORAGE_CLASS		0x08
#define USB_MSTORAGE_SUB_CLASS	0x06
#define USB_FILE_TRSF_PROTOCOL	0x50
// #define USB_MSTORAGE_IF_STR		mass_storage_if_str

/*--------------------------------------------------------------------------------------*/
/*interface 4 defines																	*/
/*--------------------------------------------------------------------------------------*/

#define USB_IF4_ALT_SET				0	/*alternate setting*/
#define USB_IF4_TOTAL_NR_EP			3
#define USB_SERIAL_TRACE_CLASS		0
#define USB_SERIAL_TRACE_SUB_CLASS	0
#define USB_SERIAL_TRACE_PROTOCOL	0
// #define USB_SERIAL_TRACE_IF_STR		trace_serial_if_str

/*--------------------------------------------------------------------------------------*/
/*interface 5 defines																	*/
/*--------------------------------------------------------------------------------------*/

#define USB_IF_DATA_TRACE			4
#define USB_IF5_ALT_SET				0	/*alternate setting*/
#define USB_IF5_TOTAL_NR_EP			2
#define USB_DATA_TRACE_CLASS		0
#define USB_DATA_TRACE_SUB_CLASS	0
#define USB_DATA_TRACE_PROTOCOL		0
// #define USB_DATA_SERIAL_TRACE_IF_STR	trace_data_if_str

/**
 *endpoint adress allocation
 */
/*
#define USB_RX_EP_1_START_ADDR		USB_ENDP_BASE_ADDR		
#define USB_RX_EP_2_START_ADDR		(USB_RX_EP_1_START_ADDR + (USB_EP_RX_1 / DEFAULT_SIZE))
#define USB_RX_EP_3_START_ADDR		(USB_RX_EP_2_START_ADDR	+ (USB_EP_RX_2 / DEFAULT_SIZE))
#define USB_RX_EP_4_START_ADDR		(USB_RX_EP_3_START_ADDR	+ (USB_EP_RX_3 / DEFAULT_SIZE))
#define USB_RX_EP_5_START_ADDR		(USB_RX_EP_4_START_ADDR	+ (USB_EP_RX_4 / DEFAULT_SIZE))
#define USB_RX_EP_6_START_ADDR		(USB_RX_EP_5_START_ADDR	+ (USB_EP_RX_5 / DEFAULT_SIZE))
#define USB_RX_EP_7_START_ADDR		(USB_RX_EP_6_START_ADDR	+ (USB_EP_RX_6 / DEFAULT_SIZE))
#define USB_RX_EP_8_START_ADDR		(USB_RX_EP_7_START_ADDR	+ (USB_EP_RX_7 / DEFAULT_SIZE))
#define USB_RX_EP_9_START_ADDR		(USB_RX_EP_8_START_ADDR	+ (USB_EP_RX_8 / DEFAULT_SIZE))
#define USB_RX_EP_10_START_ADDR		(USB_RX_EP_9_START_ADDR	+ (USB_EP_RX_9 / DEFAULT_SIZE))
#define USB_RX_EP_11_START_ADDR		(USB_RX_EP_10_START_ADDR + (USB_EP_RX_10 / DEFAULT_SIZE))
#define USB_RX_EP_12_START_ADDR		(USB_RX_EP_11_START_ADDR + (USB_EP_RX_11 / DEFAULT_SIZE))
#define USB_RX_EP_13_START_ADDR		(USB_RX_EP_12_START_ADDR + (USB_EP_RX_12 / DEFAULT_SIZE))
#define USB_RX_EP_14_START_ADDR		(USB_RX_EP_13_START_ADDR + (USB_EP_RX_13 / DEFAULT_SIZE))
#define USB_RX_EP_15_START_ADDR		(USB_RX_EP_14_START_ADDR + (USB_EP_RX_14 / DEFAULT_SIZE))

#define USB_TX_EP_1_START_ADDR		(USB_RX_EP_15_START_ADDR + (USB_EP_RX_15 / DEFAULT_SIZE))
#define USB_TX_EP_2_START_ADDR		(USB_TX_EP_1_START_ADDR	+ (USB_EP_TX_1 / DEFAULT_SIZE))
#define USB_TX_EP_3_START_ADDR		(USB_TX_EP_2_START_ADDR	+ (USB_EP_TX_2 / DEFAULT_SIZE))
#define USB_TX_EP_4_START_ADDR		(USB_TX_EP_3_START_ADDR	+ (USB_EP_TX_3 / DEFAULT_SIZE))
#define USB_TX_EP_5_START_ADDR		(USB_TX_EP_4_START_ADDR	+ (USB_EP_TX_4 / DEFAULT_SIZE))
#define USB_TX_EP_6_START_ADDR		(USB_TX_EP_5_START_ADDR	+ (USB_EP_TX_5 / DEFAULT_SIZE))
#define USB_TX_EP_7_START_ADDR		(USB_TX_EP_6_START_ADDR	+ (USB_EP_TX_6 / DEFAULT_SIZE))
#define USB_TX_EP_8_START_ADDR		(USB_TX_EP_7_START_ADDR	+ (USB_EP_TX_7 / DEFAULT_SIZE))
#define USB_TX_EP_9_START_ADDR		(USB_TX_EP_8_START_ADDR	+ (USB_EP_TX_8 / DEFAULT_SIZE))
#define USB_TX_EP_10_START_ADDR		(USB_TX_EP_9_START_ADDR	+ (USB_EP_TX_9 / DEFAULT_SIZE))
#define USB_TX_EP_11_START_ADDR		(USB_TX_EP_10_START_ADDR + (USB_EP_TX_10 / DEFAULT_SIZE))
#define USB_TX_EP_12_START_ADDR		(USB_TX_EP_11_START_ADDR + (USB_EP_TX_11 / DEFAULT_SIZE))
#define USB_TX_EP_13_START_ADDR		(USB_TX_EP_12_START_ADDR + (USB_EP_TX_12 / DEFAULT_SIZE))
#define USB_TX_EP_14_START_ADDR		(USB_TX_EP_13_START_ADDR + (USB_EP_TX_13 / DEFAULT_SIZE))
#define USB_TX_EP_15_START_ADDR		(USB_TX_EP_14_START_ADDR + (USB_EP_TX_14 / DEFAULT_SIZE))

*/

/**
 * @name String descriptor
 *
 * This type definition describes the data type that teh USB spec 1.1 forces
 * to use for communication with the USB HOST.
 */
/*@{*/
typedef struct
{
	UINT8	bLength;
	UINT8	bDescriptorType;
	UINT16	wSerialNumber1;
	UINT16	wSerialNumber2;	
	UINT16	wSerialNumber3;	
	UINT16	wSerialNumber4;
	UINT16	wSerialNumber5;
	UINT16	wSerialNumber6;	
	UINT16	wSerialNumber7;	
	UINT16	wSerialNumber8;
	UINT16	wSerialNumber9;
	UINT16	wSerialNumber10;	
	UINT16	wSerialNumber11;	
	UINT16	wSerialNumber12;
	UINT16	wSerialNumber13;	
}T_USB_STRING_DESCR;

/**
 * @name Interface descriptor
 *
 * This type definition describes the data type that teh USB spec 1.1 forces
 * to use for communication with the USB HOST.
 */
/*@{*/
typedef struct
{
	UINT8	bLength;
	UINT8	bDescriptorType;
	UINT8	bInterfaceNumber;
	UINT8	bAlternateSetting;
	UINT8	bNumEndpoints;
	UINT8	bInterfaceClass;
	UINT8	bInterfaceSubClass;
	UINT8	bInterfaceProtocol;
	UINT8	iInterface;
}T_USB_INTERFACE_DESCR;
/*@}*/

/**
 * @name Configuration descriptor
 *
 * This type definition describes the data type that teh USB spec 1.1 forces
 * to use for communication with the USB HOST.
 */
/*@{*/
typedef struct
{
	UINT8 	bLength;
	UINT8 	bDescriptorType;
	UINT16	wTotalLength;
	UINT8 	bNumInterfaces;
	UINT8 	bConfigurationValue;
	UINT8 	iConfiguration;
	UINT8 	bmAttributes;
	UINT8 	MaxPower;
}T_USB_CONFIG_DESCR;
/*@}*/

/**
 * @name Device descriptor
 *
 * This type definition describes the data type that teh USB spec 1.1 forces
 * to use for communication with the USB HOST.
 */
/*@{*/
typedef struct
{
	UINT8 	bLength;
	UINT8 	bDescriptorType;
	UINT16	bcdUSB;
	UINT8 	bDeviceClass;
	UINT8 	bDeviceSubClass;
	UINT8 	bDeviceProtocol;
	UINT8 	bMaxPacketSize0;
	UINT16	idVendor;
	UINT16	idProduct;
	UINT16	bcdDevice;
	UINT8 	iManufacturer;
	UINT8 	iProduct;
	UINT8 	iSerialNumber;
	UINT8 	bNumConfigurations;
}T_USB_DEVICE_DESCR;
/*@}*/

/**
 * @name T_USB_EP_DESCR
 *
 * this part describes all possible endpointdescriptors
 * 
 * there will be 32 of them. An interface can make more than one endpoint of a certain type
 * when is chosen for a C (control) rx endpoint, there must be a C tx endpoint as well.
 * 
 * I is used for intrupt transfers
 * S is used for isochronous
@{*/

/* EP_RX8 rx (OUT) Bulk endpoint not double buffered, with a fifo size of 8 bytes*/
/* EP_RX16 rx (OUT) Bulk endpoint not double buffered, with a fifo size of 16 bytes*/
/* EP_RX32 rx (OUT) Bulk endpoint not double buffered, with a fifo size of 32 bytes*/
/* EP_RX64 rx (OUT) Bulk endpoint not double buffered, with a fifo size of 64 bytes*/

/* EP_TX8 tx (IN) Bulk endpoint not double buffered, with a fifo size of 8 bytes*/
/* EP_TX16 tx (IN) Bulk endpoint not double buffered, with a fifo size of 16 bytes*/
/* EP_TX32 tx (IN) Bulk endpoint not double buffered, with a fifo size of 32 bytes*/
/* EP_TX64 tx (IN) Bulk endpoint not double buffered, with a fifo size of 64 bytes*/

/* EP_CRX8 rx (OUT) Control endpoint not double buffered, with a fifo size of 8 bytes*/
/* EP_CRX16 rx (OUT) Control endpoint not double buffered, with a fifo size of 16 bytes*/
/* EP_CRX32 rx (OUT) Control endpoint not double buffered, with a fifo size of 32 bytes*/
/* EP_CRX64 rx (OUT) Control endpoint not double buffered, with a fifo size of 64 bytes*/

/* EP_CTX8 tx (IN) Control endpoint not double buffered, with a fifo size of 8 bytes*/
/* EP_CTX16 tx (IN) Control endpoint not double buffered, with a fifo size of 16 bytes*/
/* EP_CTX32 tx (IN) Control endpoint not double buffered, with a fifo size of 32 bytes*/
/* EP_CTX64 tx (IN) Control endpoint not double buffered, with a fifo size of 64 bytes*/

/* EP_DRX8 rx (OUT) Bulk endpoint double buffered, with a fifo size of 2 * 8 bytes*/
/* EP_DRX16 rx (OUT) Bulk endpoint double buffered, with a fifo size of 2 * 16 bytes*/
/* EP_DRX32 rx (OUT) Bulk endpoint double buffered, with a fifo size of 2 * 32 bytes*/
/* EP_DRX64 rx (OUT) Bulk endpoint double buffered, with a fifo size of 2 * 64 bytes*/

/* EP_DTX8 tx (IN) Bulk endpoint double buffered, with a fifo size of 2 * 8 bytes*/
/* EP_DTX16 tx (IN) Bulk endpoint double buffered, with a fifo size of 2 * 16 bytes*/
/* EP_DTX32 tx (IN) Bulk endpoint double buffered, with a fifo size of 2 * 32 bytes*/
/* EP_DTX64 tx (IN) Bulk endpoint double buffered, with a fifo size of 2 * 64 bytes*/

/* EP_DCRX8 rx (OUT) Control endpoint double buffered, with a fifo size of 2 * 8 bytes*/
/* EP_DCRX16 rx (OUT) Control endpoint double buffered, with a fifo size of 2 * 16 bytes*/
/* EP_DCRX32 rx (OUT) Control endpoint double buffered, with a fifo size of 2 * 32 bytes*/
/* EP_DCRX64 rx (OUT) Control endpoint double buffered, with a fifo size of 2 * 64 bytes*/

/* EP_DCTX8 tx (IN) Control endpoint double buffered, with a fifo size of 2 * 8 bytes*/
/* EP_DCTX16 tx (IN) Control endpoint double buffered, with a fifo size of 2 * 16 bytes*/
/* EP_DCTX32 tx (IN) Control endpoint double buffered, with a fifo size of 2 * 32 bytes*/
/* EP_DCTX64 tx (IN) Control endpoint double buffered, with a fifo size of 2 * 64 bytes*/

typedef enum 
{
	EP_RX8 = 0,
	EP_RX16,
	EP_RX32,
	EP_RX64,
	EP_TX8,
	EP_TX16,
	EP_TX32,
	EP_TX64,
	EP_CRX8,
	EP_CRX16,
	EP_CRX32,
	EP_CRX64,
	EP_CTX8,
	EP_CTX16,
	EP_CTX32,
	EP_CTX64,
	EP_IRX8,
	EP_IRX16,
	EP_IRX32,
	EP_IRX64,
	EP_ITX8,
	EP_ITX16,
	EP_ITX32,
	EP_ITX64,
	EP_SRX8,
	EP_SRX16,
	EP_SRX32,
	EP_SRX64,
	EP_STX8,
	EP_STX16,
	EP_STX32,
	EP_STX64,
	EP_DRX8,
	EP_DRX16,
	EP_DRX32,
	EP_DRX64,
	EP_DTX8,
	EP_DTX16,
	EP_DTX32,
	EP_DTX64,
	EP_DCRX8,
	EP_DCRX16,
	EP_DCRX32,
	EP_DCRX64,
	EP_DCTX8,
	EP_DCTX16,
	EP_DCTX32,
	EP_DCTX64,
	EP_DIRX8,
	EP_DIRX16,
	EP_DIRX32,
	EP_DIRX64,
	EP_DITX8,
	EP_DITX16,
	EP_DITX32,
	EP_DITX64,
	EP_DSRX8,
	EP_DSRX16,
	EP_DSRX32,
	EP_DSRX64,
	EP_DSTX8,
	EP_DSTX16,
	EP_DSTX32,
	EP_DSTX64
}T_USB_EP_DESCR;
/*@}*/

/**
 * @name T_USB_FUNCTIONAL_HEADER_DESCR
 *
 * this part describes the type that is used for configuration cnfg
 * 
 * This type is used as the functional header descriptor for CDC 1.1 
 * class specific descriptors.
{*/
typedef struct
{
	UINT8	bLength;
	UINT8 	bDescriptorType;
	UINT8	bDescriptorSubType;
	UINT16	bCDC;
} T_USB_FUNCTIONAL_HEADER_DESCR;
/*@}*/

/**
 * @name T_USB_FUNCTIONAL_ACM_DESCR
 *
 * this part describes the type that is used for configuration cnfg
 * 
 * This type is used as the abstract control model functional descriptor 
 * for CDC 1.1 class specific de-scriptors.
@{*/
typedef struct
{
	UINT8	bLength;
	UINT8 	bDescriptorType;
	UINT8	bDescriptorSubType;
	UINT8	bmCapabilities;
} T_USB_FUNCTIONAL_ACM_DESCR;
/*@}*/

/**
 * @name T_USB_FUNCTIONAL_UNION_DESCR
 *
 * this part describes the type that is used for configuration cnfg
 * 
 * This type is used as the union functional descriptor for CDC 1.1 
 * class specific descriptors
@{*/
typedef struct
{
	UINT8	bLength;
	UINT8 	bDescriptorType;
	UINT8	bDescriptorSubType;
	UINT8	bMasterInterface;
	UINT8  bSlaveInterface0;
} T_USB_FUNCTIONAL_UNION_DESCR;
/*@}*/

/**
 * @name T_USB_CDC_DESCR
 *
 * this part describes the type that is used for configuration cnfg
 * 
 * This type is used to combine CDC functional descriptors to make a class 
 * specific descriptor for a communication device class implementation.
@{*/
typedef struct
{
	T_USB_FUNCTIONAL_HEADER_DESCR	header;
	T_USB_FUNCTIONAL_ACM_DESCR		acm_descriptor;
	T_USB_FUNCTIONAL_UNION_DESCR	interface_union;
} T_USB_CDC_DESCR;
/*@}*/

/**
 * @name T_USB_IF_CNFG
 *
 * this part describes the type that is used for interface cnfg
 * 
 * For every interface configuration a interface descriptor will be stored and
 * and a pointer to an array of T_USB_EP_DESCR. the value of index 0 in that
 * array is the ep type of the interfaces logical endpoint 0.
@{*/
typedef struct
{
	USB_DYN_CONST T_USB_EP_DESCR*			if_logical_ep_array_p;
	USB_DYN_CONST T_USB_CDC_DESCR*			if_cdc_descr_p;
	USB_DYN_CONST T_USB_INTERFACE_DESCR*	ifdescr_p;
}T_USB_IF_CNFG;
/*@}*/

/**
 * @name T_USB_IAD_DESCR
 *
 * this part describes the type that is used for configuration cnfg
 * 
 * Specification of interface association descriptor
@{*/
typedef struct
{
	UINT8	bLength;
	UINT8	bDescriptorType;
	UINT8	bFirstInterface;
	UINT8	bInterfaceCount;
	UINT8	bFunctionClass;
	UINT8	bFunctionSubClass;
	UINT8	bFunctionProtocol;
	UINT8	iFunction;
} T_USB_IAD_DESCR;
/*@}*/

/**
 * @name T_USB_CFG_CNFG
 *
 * this part describes the type that is used for configuration cnfg
 * 
 * For every config configuration a pointer to an configuration descriptor and a pointer to 
 * the interface configuration of that configuration is stored
@{*/
typedef struct
{
	USB_DYN_CONST T_USB_CONFIG_DESCR*		if_cfg_descr_p;
	USB_DYN_CONST T_USB_IAD_DESCR*			cfg_iad_descr_p;
	USB_DYN_CONST T_USB_IF_CNFG*			if_cnfg_p;			/* array of interfaces */

}T_USB_CFG_CNFG;
/*@}*/


/*--------------------------------------------------------------------------------------*/
/*external variable declarations														*/
/*--------------------------------------------------------------------------------------*/
extern USB_DYN_CONST T_USB_CFG_CNFG configurations_table[USB_DEV_NUM_CFG];
#ifdef USB_DYNAMIC_CONFIG_SUPPORT
extern T_USB_EP_DESCR endpoint_in_if1_array[USB_IF_TOTAL_NR_EP];
extern T_USB_EP_DESCR endpoint_in_if2_array[USB_IF_TOTAL_NR_EP];
extern T_USB_EP_DESCR endpoint_in_if3_array[USB_IF_TOTAL_NR_EP];
#endif
extern T_USB_CONFIG_DESCR usb_conf_descr_table[USB_DEV_NUM_CFG];

extern const T_RVF_ADDR_ID swe_tbl[USB_TOT_SWE];

#endif /* __USB_INTERFACE_CFG_H_ */
