#ifndef IOPERRS_H
#define IOPERRS_H


/*------------------------------------------------------------------------
*  The following definitions are fault ids for MonFault.
*-----------------------------------------------------------------------*/

typedef enum 
{
    IOP_RX_CHECKSUM_ERR       = 0x00,
    IOP_RX_PACKET_LEN_ERR     = 0x01,
    IOP_RX_OVERRUN_ERR        = 0x02,
    IOP_RX_SYNC_ERR           = 0x03,
    IOP_RX_SYNC_STATE_ERR     = 0x04,
    IOP_RX_MSG_ID_ERR         = 0x05,
    IOP_TX_MSG_LEN_ERR        = 0x06,
    IOP_UART_PORT_ERR         = 0x07,
    IOP_NO_MSG_IN_MBOX_ERR    = 0x08,
    IOP_MSG_ID_ERR            = 0x09,
    IOP_BAUD_RATE_ERR         = 0x0A,
    IOP_TX_OVERRUN_ERR        = 0x0B,
    IOP_UART_NOT_EXIST_ERR    = 0x0C,
    IOP_ETS_DATA_DELETED_ERR  = 0x0D,
    IOP_GPS_SM_ERR                  = 0x0E,
    IOP_GPS_INIT_TO_ERR             = 0x0F,
    IOP_GPS_BOOT_TO_ERR             = 0x10,
    IOP_GPS_DNLD_CHECKSUM_ERR       = 0x11,
    IOP_GPS_RX_UNEXPECTED_MSG_ERR   = 0x12,
    IOP_GPS_TX_REQ_DURING_INIT_ERR  = 0x13,
    IOP_GPS_TX_INVALID_STATE_ERR    = 0x14,
    IOP_GPS_UART_NOT_CONFIGURED_ERR = 0x15,
    IOP_GPS_TX_MSG_ID_ERR           = 0x16,
    IOP_GPS_DOWNLOAD_ABORT_ERR      = 0x17,
    IOP_GPS_TMR_NOT_AVAIL           = 0x18 

} IopErrsT;



#endif
