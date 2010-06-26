/*/ * @file	usb_cfg.c
 *
 * USB configuration specific actions and functions
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

#include "usb/usb_interface.h"
#include "usb/usb_config.h"
#include "usb/usb_interface.h"
#include "usb/usb_i.h"
#include "rv/rv_defined_swe.h"

/* has to do with build process, 'activate' when NOT in test mode
 */
#ifndef USB_TEST_MODE

#define USB_CFG_TOT_DESCR_LNT	0 //Is determined runtime

/*--------------------------------------------------------------------------------------*/
/*global strings																		*/
/*--------------------------------------------------------------------------------------*/

UINT8 conf1_descr_str[] = "Default configuration";
UINT8 conf2_descr_str[] = "Trace   configuration";

UINT8 cdc_serial_emulation_if_str[]     = "CDC serial emulation Interface";
UINT8 cdc_data_if_str[]					= "CDC data Interface";
UINT8 mass_storage_if_str[]				= "Mass storage Interface";
UINT8 trace_serial_if_str[]				= "CDC trace serial Interface";
UINT8 trace_data_if_str[]				= "CDC trace data Interface";

UINT8 product_str[]      = "";
UINT8 manufacturer_str[] = "";
UINT8 serialnr_str[]     = "";

/*--------------------------------------------------------------------------------------*/
/* CDC constants																		*/
/*																						*/
/* USB Class Definitions for Communication Devices										*/
/*--------------------------------------------------------------------------------------*/
#define USB_CDC_HEADER_LENGTH		0x05	/* size of  structure */
#define USB_CS_INTERFACE_TYPE		0x24	/* CS_INTERFACE in Table 24 */
#define USB_CDC_HEADER_SUBTYPE		0x00	/* Header. in Table 25 */
#define	USB_CDC_REVISION			0x0110	/* version 1.1 in BCD */

#define USB_CDC_ACM_LENGTH			0x04	/* size of  structure */
#define USB_CS_INTERFACE_TYPE		0x24	/* CS_INTERFACE in Table 24 */
#define USB_CDC_ACM_SUBTYPE			0x02	/* Abstract Control Management Functional Descriptor. in Table 25 */
#define USB_CDC_ACM_CAPABILITIES	0x02	/* only support Set(Get)_Line_Coding, Set(Get)_Line_State in Table 28 */

#define USB_CDC_UNION_LENGTH		0x05	/* size of structure */
#define USB_CS_INTERFACE_TYPE		0x24	/* CS_INTERFACE in Table 24 */
#define USB_CDC_UNION_SUBTYPE		0x06	/* Union. in Table 25 */
#define USB_CDC_MASTER_IF			0x00	/* COM serial emulation is managed on Interface 0 */
#define USB_CDC_SLAVE_IF			0x00	/* Data are transmitted on Interface 0 */

/*--------------------------------------------------------------------------------------*/
/* IAD constants																		*/
/*																						*/
/* USB Class Definitions for Communication Devices										*/
/*--------------------------------------------------------------------------------------*/
#define USB_IAD_HEADER_LENGTH		0x08	/* size of  structure */
#define USB_IAD_INTERFACE_TYPE		0x0B	/*  */
#define USB_IAD_FIRST_INTERFACE		0x00	/* Select interface 0 */
#define	USB_IAD_INTERFACE_COUNT		0x02	/* interface count for this IAD */
#define USB_IAD_FUNCTION_CLASS		USB_CDC_SERIAL_CLASS
#define USB_IAD_FUNCTION_SUB_CLASS	USB_CDC_SERIAL_SUB_CLASS
#define USB_IAD_FUNCTION_PROTOCOL	USB_CDC_SERIAL_PROTOCOL
#define USB_IAD_FUNCTION			0x00	/* default value */


/*--------------------------------------------------------------------------------------*/
/* table of SWEs that will receive a USB_BUS_CONNECT_MSG if an interface is active
*--------------------------------------------------------------------------------------*/

/**
 * @name activate_swe_table
 *
 * table that contains a list of SWE's that will be signalled when the USB driver is 
 * started, so the have a change to subscribe to an interface if they like;
 */
/*@{*/
const T_USB_SWE_IF_T usb_swe_tbl[USB_TOT_SWE] = 
{
	{RTEST_USE_ID, USB_IF_CDC_SERIAL}
};

/*@}*/

/*--------------------------------------------------------------------------------------*/
/* table of strings that for the various USB descriptors								*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name usb_descr_strings
 *
 * Table that contains all the strings that are used to describe an USB device or 
 * configuration etc, this could be extended to multiple languages etc. The order 
 * in which this table is filled is forced by the T_USB_STR_IDX enum in usb_test_cfg.h
 */
/*@{*/
const UINT8* usb_descr_strings[] =
{
	product_str,
	manufacturer_str,
	serialnr_str,
	conf1_descr_str,
	conf2_descr_str,
};
/*@}*/
/*--------------------------------------------------------------------------------------*/
/*interface descriptors																	*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name USB interface descriptor
 *
 *There are more interface descriptors 
 * Here the interface descriptors are created, see USB 1.1 spec for more info on the 
 * fields that are required to fill in
 */
/*@{*/
const T_USB_INTERFACE_DESCR if_descr_table[USB_TOTAL_DIFF_IF] =
{
	{
		USB_IF_LNT,					  /*always use this define*/
		USB_INTERFACE_TYPE,	  /*always use this define*/
		USB_IF_CDC_SERIAL,			
		USB_IF1_ALT_SET,	
		USB_IF1_TOTAL_NR_EP,
		USB_CDC_SERIAL_CLASS,		
		USB_CDC_SERIAL_SUB_CLASS,
		USB_CDC_SERIAL_PROTOCOL,	
		0                     /* no interface description string index USB_TEST1_IF_STR */
	},
	{
		USB_IF_LNT,					  /*always use this define*/
		USB_INTERFACE_TYPE,	  /*always use this define*/
		USB_IF_MSTORAGE,			
		USB_IF3_ALT_SET,	
		USB_IF3_TOTAL_NR_EP,
		USB_MSTORAGE_CLASS,		
		USB_MSTORAGE_SUB_CLASS,
		USB_FILE_TRSF_PROTOCOL,	
		0                     /* no interface description string index USB_TEST1_IF_STR */
	},
	{
		USB_IF_LNT,					  /*always use this define*/
		USB_INTERFACE_TYPE,	  /*always use this define*/
		USB_IF_SERIAL_TRACE,
		USB_IF4_ALT_SET,
		USB_IF4_TOTAL_NR_EP,
		USB_CDC_SERIAL_CLASS,
		USB_CDC_SERIAL_SUB_CLASS,
		USB_CDC_SERIAL_PROTOCOL,
		0                     /* no interface description string index USB_TEST1_IF_STR */
	}
};

/*@}*/

/*--------------------------------------------------------------------------------------*/
/*configuration descriptor															*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name USB configuration descriptor
 *
 *There can be more configuration descriptors
 */
/*@{*/
T_USB_CONFIG_DESCR usb_conf_descr_table[USB_DEV_NUM_CFG] =
{
	{
		USB_CNF_DESCR_LNT,						/*always use this define*/
		USB_CONFIGURATION_TYPE,					/*always use this define*/	
		USB_CFG_TOT_DESCR_LNT,					/*always use this define*/
        USB_CFG1_NUM_INTERF, 				/*define your own*/
		USB_CFG1_VALUE,							/*define your own*/	
		0,							              /*no string index for descriptive string */
		USB_CFG1_ATTRIBUTES,					/*define your own*/		
		USB_CFG1_MAX_POWER,						/*define your own*/	
		//0x32,	//example code
	}/*,
	{
		USB_CNF_DESCR_LNT,
		USB_CONFIGURATION_TYPE,
		USB_CFG_TOT_DESCR_LNT,
		USB_CFG2_NUM_INTERF,
		USB_CFG2_VALUE,
		0,
		USB_CFG2_ATTRIBUTES,
		USB_CFG2_MAX_POWER
	}*/
};
/*@}*/

/*--------------------------------------------------------------------------------------*/
/* interface 0 endpoints  -  Serial Emulation																*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name endpoint_in_if1_array
 *
 * This table contains all enpoints for interface 0 index 0 contains the endpoint
 * information of interface 1 first logical endpoint, index 1 contains info of the second 
 * logical endpoint, etc.
 * This interface is used by USB mass storage class driver
 */
/*@{*/

const T_USB_EP_DESCR endpoint_in_if1_array[USB_IF1_TOTAL_NR_EP] =
{
	EP_TX64,	/*logical endpoint 1 of this interface*/
				/*Bulk IN & 64 bytes*/

	EP_RX64,	/*logical endpoint 2 of this interface*/
				/*Bulk OUT & 64 bytes*/
	
	EP_ITX16	/*logical endpoint 3 of this interface*/
				/*Interrupt IN & 16 bytes*/
};
/*@}*/

/*--------------------------------------------------------------------------------------*/
/* interface 1 endpoints - Mass Storage																*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name endpoint_in_if2_array
 *
 * This table contains all enpoints for interface 1 index 0 contains the endpoint
 * information of interface 1 first logical endpoint, index 1 contains info of the second
 * logical endpoint, etc.
 * This interface is used by USB serial emulation class driver
 */
/*@{*/

const T_USB_EP_DESCR endpoint_in_if2_array[USB_IF2_TOTAL_NR_EP] =
{
	EP_IRX16,	/*logical endpoint 3 of this interface*/
				/*Interrupt OUT & 16 bytes*/

	EP_TX64,	/*logical endpoint 1 of this interface*/
				/*Bulk IN & 64 bytes*/

	EP_RX64		/*logical endpoint 2 of this interface*/
				/*Bulk OUT & 64 bytes*/
};
/*@}*/


/*--------------------------------------------------------------------------------------*/
/* interface 2 endpoints  -  Serial Emulation																*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name endpoint_in_if3_array
 *
 * This table contains all enpoints for interface 2 index 0 contains the endpoint
 * information of interface 2 first logical endpoint, index 1 contains info of the second
 * logical endpoint, etc.
 * This interface is used by USB serial emulation class driver
 */
/*@{*/

const T_USB_EP_DESCR endpoint_in_if3_array[USB_IF3_TOTAL_NR_EP] =
{
	EP_TX64,	/*logical endpoint 1 of this interface*/
				/*Bulk IN & 64 bytes*/

	EP_RX64,	/*logical endpoint 2 of this interface*/
				/*Bulk OUT & 64 bytes*/
	
	EP_ITX16	/*logical endpoint 3 of this interface*/
				/*Interrupt IN & 16 bytes*/
};
/*@}*/

/*--------------------------------------------------------------------------------------*/
/* interface 3 endpoint	-  Not used															*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name endpoint_in_if4_array
 *
 * This table contains all enpoints for interface 3 index 0 contains the endpoint
 * information of interface 3 first logical endpoint, index 1 contains info of the second
 * logical endpoint, etc.
 */
/*@{*/

const T_USB_EP_DESCR endpoint_in_if4_array[USB_IF4_TOTAL_NR_EP] =
{
	EP_ITX16		/*logical endpoint 1 of this interface*/
					/*Interrupt IN & 16 bytes*/
};
/*@}*/

/*--------------------------------------------------------------------------------------*/
/* interface 4 endpoints  -  Not used															*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name endpoint_in_if5_array
 *
 * This table contains all enpoints for interface 4 index 0 contains the endpoint
 * information of interface 4 first logical endpoint, index 1 contains info of the second
 * logical endpoint, etc.
 */
/*@{*/

const T_USB_EP_DESCR endpoint_in_if5_array[USB_IF5_TOTAL_NR_EP] =
{
	EP_TX64,	/*logical endpoint 1 of this interface*/
				/*Bulk IN & 64 bytes*/

	EP_RX64		/*logical endpoint 2 of this interface*/	
				/*Bulk OUT & 64 bytes*/
};
/*@}*/

/*--------------------------------------------------------------------------------------*/
/*Class descriptor for Communication device 											*/
/*--------------------------------------------------------------------------------------*/
const T_USB_IAD_DESCR cnfg1_iad_descr =
{
	USB_IAD_HEADER_LENGTH,
	USB_IAD_INTERFACE_TYPE,
	USB_IAD_FIRST_INTERFACE,
	USB_IAD_INTERFACE_COUNT,
	USB_IAD_FUNCTION_CLASS,
	USB_IAD_FUNCTION_SUB_CLASS,
	USB_IAD_FUNCTION_PROTOCOL,
	USB_IAD_FUNCTION
};


/*--------------------------------------------------------------------------------------*/
/*Class descriptor for Communication device 											*/
/*--------------------------------------------------------------------------------------*/
const T_USB_CDC_DESCR cnfg1_cdc_descr =
{
	/*HEADER functional descriptor*/
	{
		USB_CDC_HEADER_LENGTH,		/*always use this define */
		USB_CS_INTERFACE_TYPE,		/*always use this define */
		USB_CDC_HEADER_SUBTYPE,		/*always use this define */
		USB_CDC_REVISION			/*always use this define */
	},
	/*Abstract control Model functional descriptor*/
	{
		USB_CDC_ACM_LENGTH,			/*always use this define */
		USB_CS_INTERFACE_TYPE,		/*always use this define */
		USB_CDC_ACM_SUBTYPE,		/*always use this define */
		USB_CDC_ACM_CAPABILITIES	/*bitmap defining the supported class requests*/
	},
	/*Union control Model functional descriptor*/
	{
		USB_CDC_UNION_LENGTH,		/*always use this define */
		USB_CS_INTERFACE_TYPE,		/*always use this define */
		USB_CDC_UNION_SUBTYPE,		/*always use this define */
		USB_CDC_MASTER_IF,			/*must match the communication Interface number*/
		USB_CDC_SLAVE_IF			/*must match the data Interface number*/
	}
};


/*--------------------------------------------------------------------------------------*/
/* configuration 1 interfaces															*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name cnfg1_if_table
 *
 * This table contains all interfaces that are part of configuration 1
 */
/*@{*/
#if (TRACE_ON_USB == 0)
const T_USB_IF_CNFG cnfg1_if_table[USB_CFG1_NUM_INTERF]=
{

	{
		&endpoint_in_if1_array[0],	/*interface 1 endpoints table*/
		NULL,//&cnfg1_cdc_descr,
		&if_descr_table[0]			/*interface descriptor of interface 1*/
	}
/* FIXME: Once Mass storage is integrated remove the following checking */	
#ifdef RVM_USBMS_SWE
	,{
		&endpoint_in_if2_array[0],	/*interface 2 endpoints table*/
		NULL,						/*no CDC associated to this interface*/
		&if_descr_table[1]			/*interface descriptor of interface 2*/
	}//,
#endif
//	{
//		&endpoint_in_if3_array[0],	/*interface 2 endpoints table*/
//		NULL,						/*no CDC associated to this interface*/
//		&if_descr_table[2]			/*interface descriptor of interface 2*/
//	}
};
#else
const T_USB_IF_CNFG cnfg1_if_table[USB_CFG1_NUM_INTERF]=
{

	{
		&endpoint_in_if1_array[0],	/*interface 1 endpoints table*/
		NULL,//&cnfg1_cdc_descr,
		&if_descr_table[0]			    /*interface descriptor of interface 1*/
	}//,
//	{
//		&endpoint_in_if2_array[0],	/*interface 2 endpoints table*/
//		NULL,						/*no CDC associated to this interface*/
//		&if_descr_table[1]			/*interface descriptor of interface 2*/
//	}//,
//	{
//		&endpoint_in_if3_array[0],	/*interface 2 endpoints table*/
//		NULL,						/*no CDC associated to this interface*/
//		&if_descr_table[2]			/*interface descriptor of interface 2*/
//	}
};
#endif

/*@}*/

/*--------------------------------------------------------------------------------------*/
/* configuration 2 interfaces															*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name cnfg2_if_table
 *
 * This table contains all interfaces that are part of configuration 2
 */
/*@{*/
const T_USB_IF_CNFG cnfg2_if_table[USB_CFG2_NUM_INTERF]=
{
	{
		&endpoint_in_if1_array[0],	/*interface 1 endpoints table*/
		NULL,
		&if_descr_table[0]			/*interface descriptor of interface 1*/
	},
	{
		&endpoint_in_if2_array[0],	/*interface 2 endpoints table*/
		NULL,						/*no CDC associated to this interface*/
		&if_descr_table[1]			/*interface descriptor of interface 2*/
	},
	{
		&endpoint_in_if3_array[0],	/*interface 3 endpoints table*/
		NULL,						/*no CDC associated to this interface*/
		&if_descr_table[2]			/*interface descriptor of interface 3*/
	},
	{
		&endpoint_in_if4_array[0],	/*interface 4 endpoints table*/
		NULL,						/*no CDC associated to this interface*/
		&if_descr_table[3]			/*interface descriptor of interface 4*/
	},
	{
		&endpoint_in_if5_array[0],	/*interface 4 endpoints table*/
		NULL,						/*no CDC associated to this interface*/
		&if_descr_table[4]			/*interface descriptor of interface 4*/
	}
};
/*@}*/


/*--------------------------------------------------------------------------------------*/
/*configuration table																	*/
/*--------------------------------------------------------------------------------------*/

/**
 * @name configurations_table
 *
 * This table contains all available USB configurations
 */
/*@{*/
const T_USB_CFG_CNFG configurations_table[USB_DEV_NUM_CFG]=
{
	{
		&usb_conf_descr_table[0],
		NULL,
		&cnfg1_if_table[0]
	}/*,
	{
		&usb_conf_descr_table[1],
		NULL,
		&cnfg2_if_table[0]
	}*/
};

#endif // USB_TEST_MODE

/*@}*/
