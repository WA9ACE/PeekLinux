/**
 * @file    usbms_i.h
 *
 * Internal definitions for USBMS.
 *
 * @author  Your name here (your_email_here)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author                  Modification
 *  -------------------------------------------------------------------
 *  3/19/2004   Virgile COULANGE        Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */

#ifndef __USBMS_INST_I_H_
#define __USBMS_INST_I_H_


#include "config/swconfig.cfg"
#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "usbms/usbms_cfg.h"
#include "usbms/usbms_api.h"
#include "usbms/usbms_state_i.h"
#include "usbms/usbms_message.h"

#include "gbi/gbi_api.h"
#include "gbi/gbi_message.h"

#include "usb/usb_test_interface_cfg.h"
#include "usb/usb_api.h"
#include "usb/usb_message.h"
#include "rv_swe.h"


/****************************************************************************************
 |  DEBUG                                                                               |
 V**************************************************************************************V*/
//#define TEST_MODE
#define USBMS_ENABLE_TRACE      0

/****************************************************************************************
 |  MACROS DEFINITIONS                                                                          |
 V**************************************************************************************V*/

#if ((TRACE_ON_USB == 0) && (USBMS_ENABLE_TRACE == 1))
    #define USBMS_SEND_TRACE(string, trace_level) \
        rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, USBMS_USE_ID)
    #define USBMS_TRACE_PARAM(string, param) { \
        rvf_send_trace((string), sizeof(string) - 1, param, RV_TRACE_LEVEL_WARNING, USBMS_USE_ID); }
#else
	#define USBMS_SEND_TRACE(string, trace_level) do {\
														extern char ser_cfg_info[]; \
														if(ser_cfg_info[0]=='R' && USBMS_ENABLE_TRACE) \
														{\
															rvf_send_trace(string, (sizeof(string) - 1), NULL_PARAM, trace_level, USBMS_USE_ID);\
														}\
													  }while(0)
	#define USBMS_TRACE_PARAM(string, param) do {\
														extern char ser_cfg_info[]; \
														if(ser_cfg_info[0]=='R' && USBMS_ENABLE_TRACE) \
														{\
															rvf_send_trace((string), sizeof(string) - 1, param, RV_TRACE_LEVEL_WARNING, USBMS_USE_ID);\
														}\
													  }while(0)
#endif




/****************************************************************************************
 |  MACROS                                                                          |
 V**************************************************************************************V*/
#define VALUE_16(p1,p2) (   (p1)                    \
                            |(p2<<8)    )

#define VALUE_32(p1,p2,p3,p4)   (   (p1)            \
                                    |(p2<<8)        \
                                    |(p3<<16)       \
                                    |(p4<<24)   )



/****************************************************************************************
 |  DEFINITIONS                                                                         |
 V**************************************************************************************V*/

/**
 *
 *  Definition of each endpoint
 *
 */
#define EP_CTRL                     0                       /* CONTROL Endpoint     */
#define EP_RX                       3//2                        /* BULK OUT Endpoint    */
#define EP_TX                       2//1                        /* BULK IN Endpoint     */



/**
 *
 *  various definitions
 *
 */
#define PARTITION_NMB           0                       /* Partition number         */
#define MAX_BYTE_PER_XFER       (32 * 512)              /* Max number of block      */
#define MAX_PACKET_SIZE         64                      /* USB Max Packet size      */
#define MAX_TRANSFER_SIZE       (256 * MAX_PACKET_SIZE)  /* Maximum transfer size between  USB and Mass Storage  */
#define MAX_COMMAND_SIZE        32						/* Maximum transfer size of CBW and RBC COmmands */

#if ((DATALIGHT_NOR == 1) && (DATALIGHT_NAND == 1))

#ifdef RVM_MC_SWE
	#define MAX_LUN_NUM             3                 /* Logical Unit Number  !!!beware!!! the stack pool size depends on it*/
#else
	#define MAX_LUN_NUM             2
#endif

#else
	#define MAX_LUN_NUM             1
#endif

#define USBMS_INTERFACE_ID      USB_IF_MSTORAGE



/**
 *
 *  Class Request specific definitions
 *
 */
#define MAX_CLASS_REQUEST_SIZE  10                      /** Max class request buffer size   */

/** Mass Storage reset class request    */
#define RESET_REQUEST_TYPE      0x21                    /** "request type" value for the mass storage reset command */
#define RESET_REQUEST           0xFF                    /** "request" value for the mass storage reset command */
#define RESET_VALUE             0x00                    /** "value" value for the mass storage reset command */
#define RESET_INDEX             USBMS_INTERFACE_ID      /** "index" value for the mass storage reset command */
#define RESET_LENGTH            0x00                    /** "length" value for the mass storage reset command */

/** Get Max Lun class request   */
#define MAX_LUN_REQUEST_TYPE    0xA1                    /** "request type" value for the get max lun command */
#define MAX_LUN_REQUEST         0xFE                    /** "request" value for the get max lun command */
#define MAX_LUN_VALUE           0x00                    /** "value" value for the get max lun command */
#define MAX_LUN_INDEX           USBMS_INTERFACE_ID      /** "index" type value for the get max lun command */
#define MAX_LUN_LENGTH          0x01                    /** "length" type value for the get max lun command */



/**
 *
 *  RBC specific definitions
 *
 */
#define INQUIRY                 0x12
#define MODE_SELECT             0x15
#define MODE_SENSE              0x1A
#define PREVENT_ALLOW_REMOVAL   0x1E
#define START_STOP_UNIT         0x1B
#define READ_CAPACITY           0x25
#define TEST_UNIT_READY         0x00
#define READ_10                 0x28
#define WRITE_10                0x2A
#define VERIFY                  0x2F
#define REQUEST_SENSE           0x03


/**
 *
 *  BOT specific definitions
 *
 */
#define CBW_SIGNATURE           0x43425355
#define CSW_SIGNATURE           0x53425355

/**
 * Double buffer specific definitions
 */
#define NUM_BUFFERS 2
#define MAX_BYTE_PER_READ_XFER (MAX_BYTE_PER_XFER)

/****************************************************************************************
 |  STRUCTURES DEFINITIONS                                                              |
 V**************************************************************************************V*/


/**
 * @name T_SENSE_KEY
 *
 *  Enum defining the SENSE KEY error cases
 *
 */
/*@{*/
typedef enum _T_SENSE_KEY
{
    NO_SENSE,
    RECOVERED_ERROR,
    NOT_READY,
    MEDIUM_ERROR,
    HARDWARE_ERROR,
    ILLEGAL_REQUEST,
    UNIT_ATTENTION,
    DATA_PROTECT,
    BLANK_CHECK,
    VENDOR_SPECIFIC,
    COPY_ABORTED,
    ABORTED_COMMAND
}T_SENSE_KEY;
/*@}*/


/**
 * @name T_COMMAND_BLOCK_STATUS
 *
 *  Enum defining the possible status to be returned with the T_CSW structure
 *
 */
/*@{*/
typedef enum
{
    GOOD_STATUS,                                /* BOT Good status  */
    COMMAND_FAILED,                             /* BOT Command failure status   */
    PHASE_ERROR                                 /* BOT Phase error status   */
}T_COMMAND_BLOCK_STATUS;
/*@}*/

/**
 * @name T_BUF_STATUS
 *
 *  Enum defining the possible status of read buffer status
 *
 */
/*@{*/
typedef enum
{
    AVAILABLE,              /* Buffer available */
    USB_OP,                 /* USB Operation */
    GBI_OP                  /* GBI Operation */
}T_BUF_STATUS;
/*@}*/

/**
 * @name T_USBMS_READ_XFER_BUFFER
 *
 * Data structure holding information for read transfer
 *
 */
/*@{*/
typedef struct
{
    UINT8           buffer     [NUM_BUFFERS][MAX_BYTE_PER_READ_XFER];
    T_BUF_STATUS    buffer_status   [NUM_BUFFERS];
    UINT32          usb_tx_count;
    UINT32          gbi_tx_count;
    UINT32          requested_tx_count;
    UINT8           usb_op_buf_index;
    UINT8           gbi_op_buf_index;

} T_USBMS_READ_XFER_BUFFER;
/*@}*/

/**
 * @name T_USBMS_XFER
 *
 * Data structure describing the transfer to execute
 *
 */
/*@{*/
typedef struct
{
    UINT32                      first_block_nmb;
    UINT32                      number_of_blocks;
    UINT16                      current_block;                          /* Number of the bloc currently transferred */
    UINT8                       *data_buff;                             /* Buffer containing the data to transferred    */
    T_USBMS_READ_XFER_BUFFER    *data_read_buff;                         /* Buffer for read operation from host*/
    UINT16                      buffer_size;                            /* Size of the data to transfer */
    UINT32                      command_size;                           /* Size of the command  */
    UINT32                      transferred_size;                       /* size of the data actually transferred at this point  */
}T_USBMS_XFER;
/*@}*/



/**
 * @name T_USB_ENDPOINT_CONTEXT
 *
 * Stores the line signals sets by the USB Host
 *
 */
/*@{*/
typedef struct
{
    UINT8   interface_id;                               /* Number of the interface owning the endpoint */
    UINT8   endpoint_id;                                /* Number of the endpoint */
    UINT8   max_packet_size;                            /* Max packet size of the endpoint*/
} T_USB_ENDPOINT_CONTEXT;
/*@}*/




/**
 * @name T_USBMS_TEMPORARY_BUFFER
 *
 * Temporary buffer for transferring data and command from the USB SWE and the USBMS SWE
 *
 */
/*@{*/
typedef struct
{
    UINT8           *buff;                          /* Pointer to the current segment */
    UINT16          size;                           /* Size of received or transmitted segment */
} T_USBMS_TEMPORARY_BUFFER;
/*@}*/



/****************************************************************************************
 |  RBC SPECIFIC
 V**************************************************************************************V*/




/**
 * @name T_USBMS_GENERIC_RBC
 *
 * Structure representing the generic RBC command
 *
 */
/*@{*/
typedef struct
{
    UINT8           operation_code;                     /* operation code of the rbc request */
    UINT8           reserved[8];                        /* Reserved; must be set to 0 */
    UINT8           control;                            /* control code; must be set to zero */
} T_USBMS_GENERIC_RBC;
/*@}*/



/**
 * @name T_USBMS_READ_10
 *
 * This structure represents the command for a read operation
 *
 *
 */
/*@{*/
typedef struct
{
    UINT8           operation_code;                     /* Operation code : 0x28    */
    UINT8           reserved_1;                         /* reserved; must not be used   */
    UINT8           block_address_3;                    /* byte 3 of the start block address    */
    UINT8           block_address_2;                    /* byte 2 of the start block address    */
    UINT8           block_address_1;                    /* byte 1 of the start block address    */
    UINT8           block_address_0;                    /* byte 0 of the start block address    */
    UINT8           reserved_2;                         /* reserved; must not be used   */
    UINT8           xfer_len_1;                         /* byte 1 of the data length    */
    UINT8           xfer_len_0;                         /* byte 0 of the data length    */
    UINT8           control;                            /* control code must be set to 0    */
} T_USBMS_READ_10;
//typedef struct
//{
//  UINT8           operation_code;                     /* Operation code : 0x28            */
//  UINT8           reserved_1;                         /* reserved; must not be used       */
//  UINT32          block_address;                      /* the start block address          */
//  UINT8           reserved_2;                         /* reserved; must not be used       */
//  UINT16          xfer_len;                           /* the data length                  */
//  UINT8           control;                            /* control code must be set to 0    */
//} T_USBMS_READ_10;
/*@}*/



/**
 * @name T_USBMS_READ_CAPACITY_DATA
 *
 * Structure containing the USB-MS current capacity
 *
 */
/*@{*/
typedef struct
{
    UINT8       block_address_3;                        /* byte 3 of the start addres of the last logical block */
    UINT8       block_address_2;                        /* byte 2 of the start addres of the last logical block */
    UINT8       block_address_1;                        /* byte 1 of the start addres of the last logical block */
    UINT8       block_address_0;                        /* byte 0 of the start addres of the last logical block */
    UINT8       block_len_3;                            /* byte 3 of the length of the last logical block   */
    UINT8       block_len_2;                            /* byte 2 of the length of the last logical block   */
    UINT8       block_len_1;                            /* byte 1 of the length of the last logical block   */
    UINT8       block_len_0;                            /* byte 0 of the length of the last logical block   */
} T_USBMS_READ_CAPACITY_DATA;
/*@}*/




/**
 * @name T_USBMS_READ_CAPACITY
 *
 *  Structure containing the READ_CAPACITY command
 *
 */
/*@{*/
typedef struct
{
    UINT8       operation_code;                         /* Operation code : 0x25*/
    UINT8       reserved[8];                            /* Must not be used */
    UINT8       control;                                /* Control code, set to zero    */
} T_USBMS_READ_CAPACITY;
/*@}*/



/**
 * @name T_USBMS_START_STOP_UNIT
 *
 *  Structure containing the Start/Stop command
 *
 */
/*@{*/
#define PWR_CONDITION   0xF0
#define START           0x01
typedef struct
{
    UINT8       operation_code;                         /** Operation code : 0x1b*/
    //UINT8     immediate:1;                            /** indicates if the status must be returned as soon as the request is validated or when the request is fully completed */
    //UINT8     reserved_1:7;                           /** Must not be used    */
    UINT8       immediate;                              /** Only bit 0 is used, the other bits are reserved */
    UINT8       reserved_2[2];                          /** Must not be used    */

//  union T_START_STOP_FLAGS
//  {
//      struct _bits_0
//      {
//          UINT8   start:1;                            /* Start/stop device    */
//          UINT8   load_eject:1;                       /* */
//          UINT8   reserved_3:2;                       /* Must not be used */
//          UINT8   power_conditions:4;                 /* If this field is different from 0, start and load_eject field are ignored    */
//      } bits_0;
//      struct _bits_1
//      {
//          UINT8   medium_state:2;
//          UINT8   reserved_3:2;
//          UINT8   power_conditions:4;                 /* If this field is different from 0, start and load_eject field are ignored    */
//      } bits_1;
//  }flags;
    UINT8       flags;
    UINT8       control;                                /* Control code, set to zero    */
} T_USBMS_START_STOP_UNIT;
/*@}*/



/**
 * @name T_USBMS_WRITE_10
 *
 *  Structure containing the RBC write command
 *
 */
/*@{*/
typedef struct
{
    UINT8       operation_code;                         /* Operation code : 0x35*/
    UINT8       reserved_1;                             /* Must not be used */
    UINT8       block_address_3;                        /* byte 3 of the address of the first block to be written   */
    UINT8       block_address_2;                        /* byte 2 of the address of the first block to be written   */
    UINT8       block_address_1;                        /* byte 1 of the address of the first block to be written   */
    UINT8       block_address_0;                        /* byte 0 of the address of the first block to be written   */
    UINT8       reserved_2;                             /* Must not be used */
    UINT8       xfer_len_1;                             /* byte 1 of the number of block to transfer    */
    UINT8       xfer_len_0;                             /* byte 0 of the number of block to transfer    */
    UINT8       control;                                /* Must be set to 0 */
} T_USBMS_WRITE_10;
/*@}*/




/**
 * @name T_USBMS_VERIFY
 *
 *  Structure containing the RBC verify command
 *
 */
/*@{*/
typedef struct
{
    UINT8       operation_code;                         /* Operation code : 0x2F*/
    UINT8       reserved_0;                             /* Must not be used */
    UINT8       block_address_3;                        /* byte 3 of the address of the first block to be verified  */
    UINT8       block_address_2;                        /* byte 2 of the address of the first block to be verified  */
    UINT8       block_address_1;                        /* byte 1 of the address of the first block to be verified  */
    UINT8       block_address_0;                        /* byte 0 of the address of the first block to be verified  */
    UINT8       reserved_1;                             /* Must not be used */
    UINT8       xfer_len_1;                             /* byte 1 of the number of block to verify  */
    UINT8       xfer_len_0;                             /* byte 0 of the number of block to verify  */
    UINT8       control;                                /* Must be set to 0 */
} T_USBMS_VERIFY;
/*@}*/



/**
 * @name T_USBMS_INQUIRY
 *
 *  Structure containing the RBC inquiry command
 *
 */
/*@{*/
//
//typedef struct
//{
//  UINT8       operation_code;                         /* Operation code : 0x12*/
//  //UINT8     enable_vpd:1;                           /* if set to 0, USB-MS shall return the standard inquiry data   */
//  //UINT8     reserved_0:7;                           /* Must not be used */
//  UINT8       enable_vpd;                             /** Only the bits 0 is used*/
//  UINT8       page_code;                              /* specifies which vital product data shall be returned */
//  UINT8       length_1;                               /* byte 1 of the address of the first block to be verified  */
//  UINT8       length_0;                               /* byte 0 of the address of the first block to be verified  */
//  UINT8       reserved_1;                             /* Must not be used */
//  UINT8       xfer_len_1;                             /* byte 1 of the allocation length  */
//  UINT8       xfer_len_0;                             /* byte 0 of the allocation length  */
//  UINT8       control;                                /* Must be set to 0 */
//} T_USBMS_INQUIRY;
#define ENAVLE_VPD  0x01
typedef struct
{
    UINT8       operation_code;                         /* Operation code : 0x12*/
    UINT8       enable_vpd;                             /** Only the bits 0 is used*/
    UINT8       page_code;                              /* specifies which vital product data shall be returned */
    UINT16      length;                                 /*  */
    UINT8       control;                                /* Must be set to 0 */
} T_USBMS_INQUIRY;
/*@}*/


/**
 * @name T_USBMS_INQUIRY_DATA
 *
 *  Structure containing the response to the RBC inquiry command for the standard inquiry data
 *
 */
/*@{*/
#define DEVICE_TYPE             0x1f
#define QUALIFIER               0xE0
#define REMOVABLE               0x01
#define RESPONSE_DATA_FORMAT    0x0F
#define HI_SUP                  0x10
#define NACA                    0x20

#define PROTECT                 0x01
#define T_P_C                   0x08
#define ALUA                    0x30
#define ACC                     0x40
#define SCCS                    0x80

#define MCHNGR                  0x08
#define MULTI_P                 0x10
#define VS_0                    0x20
#define ENC_SERV                0x40
#define B_QUE                   0x80

#define VS_1                    0x01
#define CMD_QUE                 0x02
#define LINKED                  0x08

typedef struct
{
    UINT8       device;                                 /** bits 0->5:device_type (0x0e), bits 6->8:qualifier*/
    UINT8       removable;                              /** only the bit 0 is used*/
    UINT8       version;                                /* Must be set to 0, because this device does not claim conformance to any standard */
    UINT8       rdf_hisup_naca;                         /** bits 0->3: response data format; */
    UINT8       additionnal_length;                     /* Must be set to : sizeof(T_INQUIRY_DATA)-5    */
    UINT8       protect_tpc_alua_acc_sccs;              /** bits 0: protect ;   */
                                                        /** bits 3: tpc ;   */
                                                        /** bits 4->5: alua ;   */
                                                        /** bits 6: acc ;   */
                                                        /** bits 7: sccs ;  */
    UINT8       mchngr_multip_vs_es_bq;                 /** bits 3: mchngr ;    */
                                                        /** bits 4: multi_p ;   */
                                                        /** bits 5: VS ;    */
                                                        /** bits 6: enc_serv ;  */
                                                        /** bits 7: b_que ; */
    UINT8       vs_cmdque_linked;                       /** bits 0: VS ;    */
                                                        /** bits 1: CmdQue ;    */
                                                        /** bits 3: Linked ;    */
    UINT8       vendor_id[9];                         /*  8 + 1 for \0*/
    UINT8       product_id[17];                         /* 16 + 1 for \0*/
    UINT8       product_revision_level[5];              /* 4 + 1 for \0*/


    //UINT8     vendor_specific[20];                    /* */
    //UINT8     reserved_8;
    //UINT8     reserved_9;                             /* Must not be used     */
    //UINT16        version_descriptor[8];                  /* */
    //UINT8     reserved_10;                            /* Must not be used     */
    //UINT8     reserved_11;                            /* Must not be used     */
    //UINT8     reserved_12[22];                        /* Must not be used     */
} T_USBMS_INQUIRY_DATA;
/*@}*/


/**
 * @name T_USBMS_SERIAL_PAGE
 *
 *  Structure defining the vital product data unit serial number
 *
 */
/*@{*/
typedef struct
{
    //UINT8     device_type:5;                          /* device type : 0x0E   */
    //UINT8     qualifier:3;                            /* Qualifier; must be set to 0  */
    UINT8       device_type;                            /** bits 0->4:device type   */
                                                        /** bits 5->7:qualifier */
    UINT8       page_code;                              /* Miust be set to 0x80 */
    UINT8       reserved;                               /* Must not be used */
    UINT8       page_length;                            /* must be set to sizeof(T_SERIAL_PAGE)-4   */
    UINT8       serial_number[24];                      /* */
}T_USBMS_SERIAL_PAGE;
/*@}*/


/**
 * @name T_USBMS_ID_DESCRIPTOR
 *
 *  Structure containing the ID descriptor
 *
 */
/*@{*/
typedef struct
{
    //UINT8     code_set:4;                             /* code set : 0x02  (ascii_id contains ascii caracters) */
    //UINT8     protocol_identifier:4;                  /* */
    UINT8       code_protocol;                          /** bits 0->3:Code Set  */
                                                        /** bits 4->7:Protocol Identifier   */
    //UINT8     identifier_type:4;                      /* */
    //UINT8     association:2;                          /* */
    //UINT8     reserved_0:1;                           /* Must not be used */
    //UINT8     piv:1;                                  /* */
    UINT8       IDtype_assoc_piv;                       /** bits 0->3:Identifier type   */
                                                        /** bits 4->5:association   */
                                                        /** bits 7:PIV  */
    UINT8       reserved_1;                             /* Must not be used */
    UINT8       identifier_length;                      /* Must be set to sizeof(T_USBMS_ID_DESCRIPTOR)-4   */
    UINT8       ascii_id[32];
}T_USBMS_ID_DESCRIPTOR;
/*@}*/



/**
 * @name T_USBMS_DEVICE_ID_PAGE
 *
 *  Structure defining the vital product data device identification
 *
 */
/*@{*/
typedef struct
{
    //UINT8     device_type:5;                          /* device type : 0x0E   */
    //UINT8     qualifier:3;                            /* Qualifier; must be set to 0  */
    UINT8       device_type;                            /** bits 0->4:device type   */
                                                        /** bits 5->7:qualifier */
    UINT8       page_code;                              /* Must be set to 0x83  */
    UINT8       page_length_1;                          /* must be set to 0 */
    UINT8       page_length_0;                          /* must be set to sizeof(T_USBMS_DEVICE_ID_PAGE)-4  */
    T_USBMS_ID_DESCRIPTOR   ascii_id_descriptor[1];     /* */
}T_USBMS_DEVICE_ID_PAGE;
/*@}*/




/**
 * @name T_USBMS_MODE_SELECT
 *
 *  Structure containing the MODE_SELECT command
 *
 */
/*@{*/
typedef struct
{
    UINT8       operation_code;                         /* Operation code : 0x15    */
    //UINT8     sp:1;                                   /* not supported    */
    //UINT8     reserved_0:3;                           /* Must not be used */
    //UINT8     pf;                                     /* page format : 0x01   */
    //UINT8     reserved_1:3;                           /* Must not be used */
    UINT8       sp_pf;                                  /** bit 0:SP    */
                                                        /** bit 4:PF    */
    UINT8       reserved_2;                             /* Must not be used */
    UINT8       reserved_3;                             /* Must not be used */
    UINT8       parameter_list_length;                  /* */
    UINT8       reserved_4;                             /* Must not be used */
    UINT8       control;                                /* Control code : 0x00  */
}T_USBMS_MODE_SELECT;
/*@}*/



/**
 * @name T_USBMS_MODE_SENSE
 *
 *  Structure containing the MODE_SENSE command
 *
 */
/*@{*/
typedef struct
{
    UINT8       operation_code;                         /* Operation code : 0x1A    */
    //UINT8     reserved_0:3;                           /* Must not be used */
    //UINT8     dbd:1;                                  /* Disable block descriptors : 0x01 */
    //UINT8     reserved:4;                             /* Must not be used */
    UINT8       dbd;                                    /** bit 3 : DBD */
    //UINT8     page_code:6;                            /* page code    */
    //UINT8     pc:2;                                   /* page control */
    UINT8       code_control;                           /** bits 0->5: Page Code    */
                                                        /** bits 6->7: Page Control */
    UINT8       subpage_code;                           /* subpage code */
    UINT8       allocation_length;                      /* allocatrion length   */
    UINT8       control;                                /* control code : 0x00  */
}T_USBMS_MODE_SENSE;
/*@}*/


/**
 * @name T_USBMS_MODE_PARAMETER_HEAD
 *
 *  Structure containing the mode parameter header data
 *
 */
/*@{*/
typedef struct
{
    UINT8       mode_data_length;                       /* if used with MODE_SENSE this field contains the size of the data to transfer excluding this field    */
    UINT8       medium_type;                            /* medium type : 0x00   */
    UINT8       device_specific_parameter;              /* device specific parameter : 0x00 */
    UINT8       block_descriptor_length;                /* set to 0 */
}T_USBMS_MODE_PARAMETER_HEAD;
/*@}*/



/**
 * @name T_USBMS_MEDIA_REMOVAL
 *
 *  Structure containing the prevent/allow media removal command
 *
 */
/*@{*/
typedef struct
{
    UINT8       operation_code;                         /* Operation code : 0x1E    */
    UINT8       reserved_0;                             /* Must not be used */
    UINT8       reserved_1;                             /* Must not be used */
    UINT8       reserved_2;                             /* Must not be used */
    //UINT8     prevent:2;                              /* specifies if removal is allowed or not   */
    //UINT8     reserved_3:6;                           /* Must not be used */
    UINT8       prevent;                                /** bits 0->1: Prevent  */
    UINT8       control;                                /* Must be set to 0 */
}T_USBMS_MEDIA_REMOVAL;
/*@}*/



/**
 * @name T_USBMS_REQUEST_SENSE
 *
 *  Structure containing the REQUEST_SENSE command
 *
 */
/*@{*/
typedef struct
{
    UINT8       operation_code;                         /* Operation code : 0x03    */
    //UINT8     desc:1;                                 /* */
    //UINT8     reserved_0:7;                           /* Must not be used */
    UINT8       desc;                                   /** bit 0:Desc  */
    UINT8       reserved_1;                             /* Must not be used */
    UINT8       reserved_2;                             /* Must not be used */
    UINT8       allocation_length;                      /*  */
    UINT8       control;                                /* Must be set to 0x00  */
}T_USBMS_REQUEST_SENSE;
/*@}*/






/**
 * @name T_USBMS_REQUEST_SENSE_DATA
 *
 *  Structure defining the fixed format sense data
 *
 */
/*@{*/
typedef struct
{
    //UINT8     response_code:7;                        /* response code : set to 0x70  */
    //UINT8     valid:1;                                /* set to one; specifies the infirmation fields contains valid informations as defined in the scp3 standard or a command set standard*/
    UINT8       response_code_valid;                    /** bits 0->6:Response code */
                                                        /** bits 7:Valid    */
    UINT8       obsolete;                               /* obsolete must not be used    */
    //UINT8     sense_key:4;                            /* generic error specified in enum T_SENSE_KEY  */
    //UINT8     reserved:1;                             /* Must not be used */
    //UINT8     ili:1;                                  /* */
    //UINT8     eom:1;                                  /* */
    //UINT8     file_mark:1;                            /* */
    UINT8       sense_key;                              /** bits 0->3: sense key    */
                                                        /** bits 5: ili */
                                                        /** bits 6: eom */
                                                        /** bits 7: file mark   */
    UINT8       information_3;                          /* */
    UINT8       information_2;                          /* */
    UINT8       information_1;                          /* */
    UINT8       information_0;                          /* */
    UINT8       additional_sense_length;                /* Must be set to sizeof(T_REQUEST_SENSE_DATA)-8    */
    UINT8       command_specific_information_3;         /* */
    UINT8       command_specific_information_2;         /* */
    UINT8       command_specific_information_1;         /* */
    UINT8       command_specific_information_0;         /* */
    UINT8       additional_sense_code;                  /* */
    UINT8       additional_sense_code_qualifier;        /* */
    UINT8       field_replaceable_unit_code;            /* */
    //UINT8     sense_key_specific_2:7;                 /* */
    //UINT8     sksv:1;                                 /* sense key specific valid; set to one */
    UINT8       sense_key_specifix_2;                   /** bits 0->6:sense key specific    */
                                                        /** bits 7:sksv */
    UINT8       sense_key_specifix_1;                   /* for an accurate definition of the content    */
    UINT8       sense_key_specifix_0;                   /* of this fields please refer to spc3r17, chapter 4.5.2.4  */
}T_USBMS_REQUEST_SENSE_DATA;
/*@}*/



/**
 * @name T_USBMS_TEST_UNIT_READY
 *
 *  Structure defining the TEST UNIT READY command
 *
 */
/*@{*/
typedef struct
{
    UINT8   operation_code;                 /** OPeration code must be 0x00 */
    UINT8   reserved_0;
    UINT8   reserved_1;
    UINT8   reserved_2;
    UINT8   reserved_3;
    UINT8   control;
}T_USBMS_TEST_UNIT_READY;
/*@}*/




/**
 * @name T_USBMS_BOT_CDB
 *
 *  Structure defining the fixed format sense data
 *
 */
/*@{*/
typedef union
{
    T_USBMS_GENERIC_RBC     generic_rbc;
    T_USBMS_READ_10         read;
    T_USBMS_READ_CAPACITY   read_capacity;
    T_USBMS_START_STOP_UNIT start_stop;
    T_USBMS_VERIFY          verify;
    T_USBMS_WRITE_10        write;
    T_USBMS_INQUIRY         inquiry;
    T_USBMS_MODE_SELECT     mode_select;
    T_USBMS_MODE_SENSE      mode_sense;
    T_USBMS_MEDIA_REMOVAL   media_removal;
    T_USBMS_REQUEST_SENSE   request_sense;
    T_USBMS_TEST_UNIT_READY test_unit_ready;
}T_USBMS_BOT_CDB;
/*@}*/



/**
 * @name T_USBMS_CBW
 *
 *  Structure defining the command block wrapper
 *
 */
/*@{*/
//typedef struct
//{
//  UINT32      dCBWSignature;                          /** Must be 0x43425355  */
//  UINT32      dCBWTag;                                /** Must be the same as dCSWTag */
//  UINT32      dCBWDataTransferLength;                 /** Number of bytes transfer    */
//  UINT8       bmCBWFlags;                             /** indicates the directin of the transfer : 0x80:IN (device to host) ; 0x00:OUT (host to device)   */
//  UINT8       bCBWLUN;                                /** bits 0->3: bCBWLUN  */
//  UINT8       bCBWCBLength;                           /** bits 0->4:bCBWCBLength  */
//  UINT8       CBWCB[16];                              /** the command bmock to be executed by the device  */
//}T_USBMS_CBW;
typedef struct
{
    UINT8       dCBWSignature_0;                        /** Must be 0x43425355  */
    UINT8       dCBWSignature_1;
    UINT8       dCBWSignature_2;
    UINT8       dCBWSignature_3;
    UINT8       dCBWTag_0;                              /** Must be the same as dCSWTag */
    UINT8       dCBWTag_1;
    UINT8       dCBWTag_2;
    UINT8       dCBWTag_3;
    UINT8       dCBWDataTransferLength_0;               /** Number of bytes transfer    */
    UINT8       dCBWDataTransferLength_1;
    UINT8       dCBWDataTransferLength_2;
    UINT8       dCBWDataTransferLength_3;
    UINT8       bmCBWFlags;                             /** indicates the directin of the transfer : 0x80:IN (device to host) ; 0x00:OUT (host to device)   */
    UINT8       bCBWLUN;                                /** bits 0->3: bCBWLUN  */
    UINT8       bCBWCBLength;                           /** bits 0->4:bCBWCBLength  */
    UINT8       CBWCB[16];                              /** the command bmock to be executed by the device  */
}T_USBMS_CBW;
/*@}*/


/**
 * @name T_USBMS_CSW
 *
 *  Structure defining the command status wrapper
 *
 */
/*@{*/
//typedef struct
//{
//  UINT32      dCSWSignature;                          /* Must be 0x53425355   */
//  UINT32      dCSWTag;                                /* Must be the same as dCBWTag  */
//  UINT32      dCSWDataResidue;                        /* set to the difference between the amount of data expected    */
//                                                      /* to be processed and trhe amount of data actually processed   */
//  UINT8       bCSWStatus;                             /* Status odf the the operation, defined by thje T_COMMAND_BLOCK_STATUS enum*/
//}T_USBMS_CSW;
typedef struct
{
    UINT8       dCSWSignature_0;                            /* Must be 0x53425355   */
    UINT8       dCSWSignature_1;
    UINT8       dCSWSignature_2;
    UINT8       dCSWSignature_3;
    UINT8       dCSWTag_0;                              /* Must be the same as dCBWTag  */
    UINT8       dCSWTag_1;
    UINT8       dCSWTag_2;
    UINT8       dCSWTag_3;
    UINT8       dCSWDataResidue_0;                      /* set to the difference between the amount of data expected    */
    UINT8       dCSWDataResidue_1;                      /* to be processed and trhe amount of data actually processed   */
    UINT8       dCSWDataResidue_2;
    UINT8       dCSWDataResidue_3;
    UINT8       bCSWStatus;                             /* Status odf the the operation, defined by thje T_COMMAND_BLOCK_STATUS enum*/
}T_USBMS_CSW;
/*@}*/





/****************************************************************************************
 |  USB CLASS REQUEST                                                                   |
 V**************************************************************************************V*/


/**
 * @name T_USBMS_CLASS_REQUEST
 *
 *
 * Structure for mapping the class rerquest buffer
 *
 */
/*@{*/
typedef struct
{
    UINT8       bmRequestType;                                          /* Request type     */
    UINT8       bRequest;                                               /* Request          */
    UINT16      wValue;                                                 /* Value ; always 0 */
    UINT16      wIndex;                                                 /* index : Interface number */
    UINT16      wLength;                                                /* data length      */
    UINT8       data;                                                   /* data buffer      */
} T_USBMS_CLASS_REQUEST;
/*@}*/



/**
 * @name T_USB_LUN
 *
 * Structure specific to each Logical Unit Number.
 */
/*@{*/
typedef struct
{
    T_GBI_MEDIA_INFO                    media_info;         /** Media info                          */
    T_GBI_PARTITION_INFO                partition_info;     /** Partition info                      */
    UINT8                               fail_reason;        /** variable for storing the ID of the cause of the data transfer failure   */
    UINT8                               media_state;        /** Variable for storing the media state                */
    BOOLEAN                             fail_request;       /** Informs state machine to skip data until the state changes  */
    T_USBMS_XFER                        write_request;      /** Data structure describing the write transfer to process */
    T_USBMS_XFER                        read_request;       /** Data structure describing the read transfer to execute  */
} T_USB_LUN;
/*@}*/


/**
 *
 * Data structure specific to the USBMS SWE
 *
 *
 */
/*@{*/
typedef struct
{
    UINT8                       media_number;       /** Number of media                                     */
    T_USB_LUN                   usb_lun[MAX_LUN_NUM];
    // T_MODE_RBC_DEVICE_PARAMETER_PAGE device_page;/** RBC device page                                     */
    T_USBMS_INQUIRY_DATA        inquiry_data;       /** structure containing the standard inquiry data      */
    // T_USBMS_DEVICE_ID_PAGE       device_id_page;     /** Device identification page for the Inquiry command  */
    // T_USBMS_SERIAL_PAGE          serial_number_page; /** Unit serial number for the Inquiry command          */
    T_USBMS_CLASS_REQUEST       class_request;      /** Class request buffer for EP_CTRL*/
    //T_USBMS_TEMPORARY_BUFFER  buffer_tx_temp;     /** TX temporary buffer                                 */
    T_USBMS_TEMPORARY_BUFFER    buffer_rx_temp;     /** RX temporary buffer                                 */
    T_USBMS_CSW                 csw;                /** CSW structure for the bot stats phase               */
    UINT8                       current_lun;        /** number of the currently used lun                    */
    T_USBMS_MODE_PARAMETER_HEAD mode_parameter_head;/** Structure returned for a MODE_SENSE command         */
    T_USBMS_READ_CAPACITY_DATA  read_capacity_data; /** Structure containing the read capacity of the lun   */
} T_USBMS_SPECIFIC;
/*@}*/


/**
 *
 * Data structure containing buffer used for DMA transfer
 *
 */
/*@{*/
typedef struct
{

    T_USBMS_READ_XFER_BUFFER    data_xfer_buff;
    UINT8       temp_buff       [MAX_COMMAND_SIZE];

} T_USBMS_XFER_BUFFER;
/*@}*/

/**
 * The Control Block buffer of USBMS, which gathers all 'Global variables'
 * used by USBMS instance.
 *
 * A structure should gathers all the 'global variables' of USBMS instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_USBMS_ENV_CTRL_BLK buffer is allocated when creating USBMS instance and is
 * then always refered by USBMS instance when access to 'global variable'
 * is necessary.
 */
typedef struct
{
    T_USBMS_INTERNAL_STATE  state;                              /** Store the current state of the USBMS instance */
    T_USBMS_INTERNAL_STATE  previous_state;                     /** Store the previous state of the USBMS instance */
    T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name,               /** Pointer to the error function */
                             T_RVM_RETURN error_cause,
                             T_RVM_ERROR_TYPE error_type,
                             T_RVM_STRING error_msg);
    T_RVF_MB_ID             prim_mb_id;                         /** Mem bank id. */
    T_RVF_MB_ID             sec_mb_id;
    T_RVF_ADDR_ID           addr_id;

    T_USBMS_SPECIFIC        usbms_specific;
    T_RV_RETURN_PATH        return_path;
    UINT8                   interface_id;                       /** Interface id of the mass storage    */
    UINT8					media_change_info;

} T_USBMS_ENV_CTRL_BLK;

#define USBMS_MEDIA_INSERTED    0x80
#define USBMS_MEDIA_REMOVED     0x40

/** External ref "global variables" structure. */
extern T_USBMS_ENV_CTRL_BLK     *usbms_env_ctrl_blk_p;
extern T_USBMS_XFER_BUFFER      *xfer_buffer;


/****************************************************************************************
 |  LOCAL FUNCTIONS DECLARATION                                                         |
 V**************************************************************************************V*/
/** BOT status  */
T_RV_RET send_bot_status(   T_COMMAND_BLOCK_STATUS  command_block_status,
                            UINT32                  data_residue    );
/** RBC command processing  */
T_USBMS_RETURN rbc_inquiry(T_USBMS_INQUIRY *inquiry);
T_USBMS_RETURN rbc_mode_select(T_USBMS_MODE_SELECT *mode_select);
T_USBMS_RETURN rbc_mode_sense(T_USBMS_MODE_SENSE *mode_sense);
T_USBMS_RETURN rbc_prevent_allow_removal(T_USBMS_MEDIA_REMOVAL *media_removal);
T_USBMS_RETURN rbc_start_stop_unit(T_USBMS_START_STOP_UNIT *start_stop_unit);
T_USBMS_RETURN rbc_read_capacity(T_USBMS_READ_CAPACITY *read_capacity);
T_USBMS_RETURN rbc_test_unit_ready(T_USBMS_TEST_UNIT_READY *test_unit_ready);
T_USBMS_RETURN rbc_read_10(T_USBMS_READ_10 *read_10);
T_USBMS_RETURN rbc_write_10(T_USBMS_WRITE_10 *write_10);
T_USBMS_RETURN rbc_verify(T_USBMS_VERIFY *verify);
T_USBMS_RETURN rbc_request_sense(T_USBMS_REQUEST_SENSE *request_sense);

/** Class request processing    */
T_USBMS_RETURN cr_get_max_lun(void);
T_USBMS_RETURN cr_mass_storage_reset(void);

#define     NB_ELEMENT  20


void usbms_store_message (T_RV_HDR *msg_p);
void process_stored_message(void);

#endif /* __USBMS_INST_I_H_ */
