/**
 * @file  mc_message.h
 *
 * Data structures:
 * 1) used to send messages to the MC SWE,
 * 2) MC can receive.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/1/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __MC_MESSAGE_H_
#define __MC_MESSAGE_H_


#include "rv/rv_general.h"

#include "mc/mc_cfg.h"
#include "mc/mc_env.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * The message offset must differ for each SWE in order to have
 * unique msg_id in the system.
 */
#define MC_MESSAGE_OFFSET   BUILD_MESSAGE_OFFSET(MC_USE_ID)

/**
 * @name MC_SUBSCRIBE_REQ_MSG
 *
 * Detailled description
 * The T_MC_SUBSCRIBE_REQ_MSG message can be used to subscribe to the
 * MC-driver. This message is simular to the mc_subscribe(). The driver
 * responds with a T_MC_SUBSCRIBE_RSP_MSG message.
 *
 */
/*@{*/

/** Subscribe request. */
#define MC_SUBSCRIBE_REQ_MSG (MC_MESSAGE_OFFSET | 0x001)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_SUBSCRIBER  *subscriber_p;
  T_RV_RETURN       return_path;
} T_MC_SUBSCRIBE_REQ_MSG;

/** Subscribe response. */
#define MC_SUBSCRIBE_RSP_MSG (MC_MESSAGE_OFFSET | 0x002)

typedef struct {
  T_RV_HDR          os_hdr;
  T_RV_RET          result;
  T_MC_SUBSCRIBER  *subscriber_p;
} T_MC_SUBSCRIBE_RSP_MSG;

/*@}*/

/**
 * @name MC_UNSUBSCRIBE_REQ_MSG
 *
 * Detailled description
 * The T_MC_UNSUBSCRIBE_REQ_MSG message can be used to unsubscribe from the MC-driver.
 * This message is simular to the mc_unsubscribe() function. The driver responds with
 * a T_MC_UNSUBSCRIBE_RSP_MSG message.
 *
 */
/*@{*/

/** Unsubscribe request. */
#define MC_UNSUBSCRIBE_REQ_MSG (MC_MESSAGE_OFFSET | 0x003)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_SUBSCRIBER  *subscriber_p;
} T_MC_UNSUBSCRIBE_REQ_MSG;

/** Unsubscribe response. */
#define MC_UNSUBSCRIBE_RSP_MSG (MC_MESSAGE_OFFSET | 0x004)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
} T_MC_UNSUBSCRIBE_RSP_MSG;

/*@}*/

/**
 * @name MC_READ_REQ_MSG
 *
 * Detailled description
 * The T_MC_READ_REQ_MSG message can be used to read data from a MC card.
 * This message is simular to the mc_read() function. The driver responds with
 * a T_MC_READ_RSP_MSG message.
 *
 */
/*@{*/

/** Read request */
#define MC_READ_REQ_MSG (MC_MESSAGE_OFFSET | 0x005)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_RCA         rca;
  T_MC_RW_MODE     mode;
  UINT32            addr;
  UINT8             *data_p;
  UINT32            data_size;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_READ_REQ_MSG;

/** Read response */
#define MC_READ_RSP_MSG (MC_MESSAGE_OFFSET | 0x006)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT32    card_status;
  UINT8     *data_p;
  UINT32    data_size;
} T_MC_READ_RSP_MSG;

/*@}*/

/**
 * @name MC_WRITE_REQ_MSG
 *
 * Detailled description
 * The T_MC_ WRITE_REQ_MSG message can be used to write data to a MC card.
 * This message is simular to the mc_write() function. The driver responds
 * with a T_MC_WRITE_RSP_MSG message.
 *
 */
/*@{*/

/** Write request */
#define MC_WRITE_REQ_MSG (MC_MESSAGE_OFFSET | 0x007)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_RCA         rca;
  T_MC_RW_MODE     mode;
  UINT32            addr;
  UINT8             *data_p;
  UINT32            data_size;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_WRITE_REQ_MSG;

/** Write response */
#define MC_WRITE_RSP_MSG (MC_MESSAGE_OFFSET | 0x008)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT32    card_status;
  UINT8     *data_p;
  UINT32    data_size;
} T_MC_WRITE_RSP_MSG;

/*@}*/

/**
 * @name MC_ERASE_GROUP_REQ_MSG
 *
 * Detailled description
 * The T_MC_ERASE_GROUP_REQ_MSG message can be used to erase a range of erase
 * groups on the card. This message is simular to the mc_erase_group ()
 * function. The driver re-sponds with a T_MC_ERASE_GROUP_RSP_MSG message.
 *
 */
/*@{*/

/** Erase group request */
#define MC_ERASE_GROUP_REQ_MSG (MC_MESSAGE_OFFSET | 0x009)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_RCA         rca;
  UINT32            erase_group_start;
  UINT32            erase_group_end;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_ERASE_GROUP_REQ_MSG;

/** Erase group respond */
#define MC_ERASE_GROUP_RSP_MSG (MC_MESSAGE_OFFSET | 0x00A)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT32    card_status;
} T_MC_ERASE_GROUP_RSP_MSG;

/*@}*/

/**
 * @name T_MC_SET_PROTECT_REQ_MSG
 *
 * Detailled description
 * The T_MC_SET_PROTECT_REQ_MSG message can be used to set the write
 * protection of the addressed write protect group. This message is simular
 * to the mc_set_write_protect() function (see 2.1.6). The driver responds
 * with a T_MC_SET_PROTECT_RSP_MSG message.
 *
 */
/*@{*/

/** Set protect request */
#define MC_SET_PROTECT_REQ_MSG (MC_MESSAGE_OFFSET | 0x00B)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_RCA         rca;
  UINT32            wr_prot_group;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_SET_PROTECT_REQ_MSG;

/** Set protect respond */
#define MC_SET_PROTECT_RSP_MSG (MC_MESSAGE_OFFSET | 0x00C)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT32    card_status;
} T_MC_SET_PROTECT_RSP_MSG;

/*@}*/

/**
 * @name MC_CLR_PROTECT_REQ_MSG
 *
 * Detailled description
 * The T_MC_CLR_PROTECT_REQ_MSG message can be used to clear the write
 * protection of the addressed write protect group. This message is simular
 * to the mc_clr_write_protect() function. The driver responds to a
 * MC_CLR_PROTECT_REQ_MSG message with a T_MC_CLR_PROTECT_RSP_MSG message.
 *
 */
/*@{*/

/** Clear write protect request */
#define MC_CLR_PROTECT_REQ_MSG (MC_MESSAGE_OFFSET | 0x00D)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_RCA         rca;
  UINT32            wr_prot_group;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_CLR_PROTECT_REQ_MSG;

/** Clear write protect respond */
#define MC_CLR_PROTECT_RSP_MSG (MC_MESSAGE_OFFSET | 0x00E)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT32    card_status;
} T_MC_CLR_PROTECT_RSP_MSG;

/*@}*/

/**
 * @name MC_GET_PROTECT_REQ_MSG
 *
 * Detailled description
 * The T_MC_GET_PROTECT_REQ_MSG message can be used to read 32 write
 * protection bits representing 32 write protect groups starting at a
 * specified address. This message is simular to the mc_get_write_protect()
 * function. The driver responds with a T_MC_GET_PROTECT_RSP_MSG message.
 * The wr_proo_grps variable contains the write pro-tection groups.
 *
 */
/*@{*/

/** Get write protect request */
#define MC_GET_PROTECT_REQ_MSG (MC_MESSAGE_OFFSET | 0x00F)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_RCA         rca;
  UINT32            wr_prot_group;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_GET_PROTECT_REQ_MSG;

/** Get write protect respond */
#define MC_GET_PROTECT_RSP_MSG (MC_MESSAGE_OFFSET | 0x010)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT32    card_status;
  UINT32    wr_prot_grps;
} T_MC_GET_PROTECT_RSP_MSG;

/*@}*/

/**
 * @name MC_CARD_STATUS_REQ_MSG
 *
 * Detailled description
 * The T_MC_CARD_STATUS_REQ_MSG message can be used to read 32-bit status
 * register of a MC-card. This message is simular to the
 * mc_get_card_status() function. The driver responds with a
 * T_MC_CARD_STATUS_RSP_MSG message.
 *
 */
/*@{*/

/** Get card status request */
#define MC_CARD_STATUS_REQ_MSG (MC_MESSAGE_OFFSET | 0x011)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_RCA         rca;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_CARD_STATUS_REQ_MSG;

/** Get card status respond */
#define MC_CARD_STATUS_RSP_MSG (MC_MESSAGE_OFFSET | 0x012)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT32    card_status;
} T_MC_CARD_STATUS_RSP_MSG;


/*@}*/

/**
 * @name MC_UPDATE_ACQ_REQ_MSG
 *
 * Detailled description
 * The T_MC_UPDATE_ACQ_REQ_MSG message can be used to start an
 * identification cycle of the card stack. This message is simular to the
 * mc_update_acq () function. The driver re-sponds with a
 * T_MC_UPDATE_ACQ_RSP_MSG message. The stack_size variable in the response
 * message contains the number of connected MC-cards.
 *
 */
/*@{*/

/** Update acquisition request */
#define MC_UPDATE_ACQ_REQ_MSG (MC_MESSAGE_OFFSET | 0x013)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_UPDATE_ACQ_REQ_MSG;

/** Update acquisition response */
#define MC_UPDATE_ACQ_RSP_MSG (MC_MESSAGE_OFFSET | 0x014)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT16    stack_size;
} T_MC_UPDATE_ACQ_RSP_MSG;

/*@}*/

/**
 * @name MC_RESET_REQ_MSG
 *
 * Detailled description
 * This function resets all cards to idle state. This message is simular to
 * the mc_reset() function. The driver responds with a T_MC_RESET_MSG
 * message.
 *
 */
/*@{*/

/** Reset cards request */
#define MC_RESET_REQ_MSG (MC_MESSAGE_OFFSET | 0x015)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_RESET_REQ_MSG;

/** Reset cards response */
#define MC_RESET_RSP_MSG (MC_MESSAGE_OFFSET | 0x016)

typedef struct {
  T_RV_HDR    os_hdr;
  T_RV_RET    result;
} T_MC_RESET_RSP_MSG;

/*@}*/

/**
 * @name MC_READ_OCR_REQ_MSG
 *
 * Detailled description
 * The T_MC_READ_OCR_REQ_MSG message can be used to retrieve the 32-bit OCR
 * register of a MC-card. This message is simular to the mc_read_OCR ()
 * function. The driver re-sponds with a T_MC_READ_OCR_RSP_MSG message.
 *
 */
/*@{*/

/** Read OCR request */
#define MC_READ_OCR_REQ_MSG (MC_MESSAGE_OFFSET | 0x017)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_RCA         rca;
  UINT32            *ocr_p;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_READ_OCR_REQ_MSG;

/** Read OCR response */
#define MC_READ_OCR_RSP_MSG (MC_MESSAGE_OFFSET | 0x018)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT32    *ocr_p;
} T_MC_READ_OCR_RSP_MSG;

/*@}*/

/**
 * @name MC_READ_CID_REQ_MSG
 *
 * Detailled description
 * The T_MC_READ_CID_REQ_MSG message can be used to retrieve the 128-bit OCR
 * register of a MC-card. This message is simular to the mc_read_CID ()
 * function. The driver re-sponds with a T_MC_READ_CID_RSP_MSG message.
 *
 */
/*@{*/

/** Read CID request */
#define MC_READ_CID_REQ_MSG (MC_MESSAGE_OFFSET | 0x019)

typedef struct {
  T_RV_HDR        os_hdr;
  T_MC_RCA       rca;
  UINT8           *cid_p;
T_MC_SUBSCRIBER  subscriber;
} T_MC_READ_CID_REQ_MSG;

/** Read CID response */
#define MC_READ_CID_RSP_MSG (MC_MESSAGE_OFFSET | 0x01A)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT8     *cid_p;
} T_MC_READ_CID_RSP_MSG;

/*@}*/

/**
 * @name MC_READ_CSD_REQ_MSG
 *
 * Detailled description
 * The T_MC_READ_CSD_REQ_MSG message can be used to retrieve the 128-bit CSD
 * register of a MC-card. This message is simular to the mc_read_CSD()
 * function. The driver re-sponds with a T_MC_READ_CSD_RSP_MSG message.
 *
 */
/*@{*/

/** Read CSD register request */
#define MC_READ_CSD_REQ_MSG (MC_MESSAGE_OFFSET | 0x01B)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_RCA         rca;
  UINT8             *csd_p;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_READ_CSD_REQ_MSG;

/** Read CSD register respond */
#define MC_READ_CSD_RSP_MSG (MC_MESSAGE_OFFSET | 0x01C)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT8   *csd_p;
} T_MC_READ_CSD_RSP_MSG;

/*@}*/

/**
 * @name MC_WRITE_CSD_REQ_MSG
 *
 * Detailled description
 * The T_MC_WRITE_CSD_REQ_MSG message can be used to write the programmable
 * part of the CSD register of a MC-card. This message is simular to the
 * mc_write_CSD() function. The driver responds with a
 * T_MC_WRITE_CSD_RSP_MSG message.
 *
 */
/*@{*/

/** Write CSD field request */
#define MC_WRITE_CSD_REQ_MSG (MC_MESSAGE_OFFSET | 0x01D)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_RCA         rca;
  T_MC_CSD_FIELD   field;
  UINT8             value;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_WRITE_CSD_REQ_MSG;

/** Write CSD field response */
#define MC_WRITE_CSD_RSP_MSG (MC_MESSAGE_OFFSET | 0x01E)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT32    card_status;
} T_MC_WRITE_CSD_RSP_MSG;

/*@}*/

///**
// * @name MC_ERASE_CSD_REQ_MSG
// *
// * Detailled description
// * The T_MC_ERASE_CSD_REQ_MSG message can be used to erase the programmable
// * part of the CSD register of a MC-card. This message is simular to the
// * mc_erase_CSD() function. The driver responds with a
// * T_MC_ERASE_CSD_RSP_MSG message.
// *
// */
///*@{*/
//
///** Erase CSD field request */
//#define MC_ERASE_CSD_REQ_MSG (MC_MESSAGE_OFFSET | 0x01F)
//
//typedef struct {
//  T_RV_HDR          os_hdr;
//  T_MC_RCA         rca;
//  T_MC_CSD_FIELD   field;
//  T_MC_SUBSCRIBER  subscriber;
//} T_MC_ERASE_CSD_REQ_MSG;
//
///** Erase CSD field response */
//#define MC_ERASE_CSD_RSP_MSG (MC_MESSAGE_OFFSET | 0x020)
//
//typedef struct {
//  T_RV_HDR  os_hdr;
//  T_RV_RET  result;
//  UINT32    card_status;
//} T_MC_ERASE_CSD_RSP_MSG;



/** MC Initialize message, will be send from mc_start */
#define MC_INIT_MSG (MC_MESSAGE_OFFSET | 0x021)

typedef struct {
  T_RV_HDR          os_hdr;
} T_MC_INIT_MSG;


/**
 * @name MC_NOTIFICATION_REQ_MSG
 *
 * Detailled description
 * The T_MC_NOTIFICATION_REQ_MSG message can be used to subscribe to the
 * MC-driver. This message is simular to the mc_subscribe(). The driver
 * responds with a T_MC_NOTIFICATION_RSP_MSG message.
 *
 *
 */
/*@{*/

/** Subscribe request. */
#define MC_NOTIFICATION_REQ_MSG (MC_MESSAGE_OFFSET | 0x0022)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_EVENTS      events;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_NOTIFICATION_REQ_MSG;

/** Subscribe response. */
#define MC_NOTIFICATION_RSP_MSG (MC_MESSAGE_OFFSET | 0x0023)

typedef struct {
  T_RV_HDR          os_hdr;
  T_RV_RET          result;
} T_MC_NOTIFICATION_RSP_MSG;


#define MC_EVENT_IND_MSG (MC_MESSAGE_OFFSET | 0x0024)

typedef struct {
  T_RV_HDR          os_hdr;
  T_MC_EVENTS      event;
} T_MC_EVENT_IND_MSG;

/**
 * @name MC_CARD_STATUS_REQ_MSG
 *
 * Detailled description
 * The T_MC_SD_CARD_STATUS_REQ_MSG message can be used to read 512-bit SD status
 * register of a SD-card. This message is simular to the
 * mc_sd_get_card_status() function. The driver responds with a
 * T_MC_SD_CARD_STATUS_RSP_MSG message.
 *
 */
/*@{*/

/** Get card status request */
#define MC_SD_CARD_STATUS_REQ_MSG (MC_MESSAGE_OFFSET | 0x025)

typedef struct {
  T_RV_HDR        os_hdr;
  T_MC_RCA       rca;
  UINT8           *sd_status_p;
  T_MC_SUBSCRIBER  subscriber;
} T_MC_SD_CARD_STATUS_REQ_MSG;


/** Get card status respond */
#define MC_SD_CARD_STATUS_RSP_MSG (MC_MESSAGE_OFFSET | 0x026)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT8     *sd_status_p;
} T_MC_SD_CARD_STATUS_RSP_MSG;


/**
 * @name MC_READ_SCR_REQ_MSG
 *
 * Detailled description
 * The T_MC_READ_SCR_REQ_MSG message can be used to retrieve the 64-bit SCR
 * register of a MC-card.
 * function. The driver re-sponds with a T_MC_READ_SCR_RSP_MSG message.
 *
 */
/*@{*/

/** Read SCR request */
#define MC_READ_SCR_REQ_MSG (MC_MESSAGE_OFFSET | 0x027)

typedef struct {
  T_RV_HDR        os_hdr;
  T_MC_RCA       rca;
  UINT8           *scr_p;
T_MC_SUBSCRIBER  subscriber;
} T_MC_READ_SCR_REQ_MSG;

/** Read CID response */
#define MC_READ_SCR_RSP_MSG (MC_MESSAGE_OFFSET | 0x028)

typedef struct {
  T_RV_HDR  os_hdr;
  T_RV_RET  result;
  UINT8     *scr_p;
} T_MC_READ_SCR_RSP_MSG;


//#ifdef T_FLASH_HOT_SWAP

/*	New Message definations to convey card insertion/removal and general event	*/
#define MC_CARD_INS_REM_MSG (MC_MESSAGE_OFFSET | 0x029)

typedef struct {
  T_RV_HDR  os_hdr;
} T_MC_CARD_INS_REM_MSG;


/** Card insertion event */
#define MC_CARD_INS_MSG (MC_MESSAGE_OFFSET | 0x030)

typedef struct {
  T_RV_HDR  os_hdr;
} T_MC_CARD_INS_MSG;


/** Card removal event */
#define MC_CARD_REM_MSG (MC_MESSAGE_OFFSET | 0x031)

typedef struct {
  T_RV_HDR  os_hdr;
} T_MC_CARD_REM_MSG;

//#endif


/*@}*/



/**defines needed by other components to use MC api*/
/*
#define MC_MESSAGE_OFFSET                 MC_MESSAGE_OFFSET
#define MC_SUBSCRIBE_REQ_MSG      MC_SUBSCRIBE_REQ_MSG
#define MC_SUBSCRIBE_RSP_MSG      MC_SUBSCRIBE_RSP_MSG
#define T_MC_SUBSCRIBE_RSP_MSG      T_MC_SUBSCRIBE_RSP_MSG
#define MC_UNSUBSCRIBE_REQ_MSG      MC_UNSUBSCRIBE_REQ_MSG
#define T_MC_UNSUBSCRIBE_REQ_MSG      T_MC_UNSUBSCRIBE_REQ_MSG
#define MC_UNSUBSCRIBE_RSP_MSG      MC_UNSUBSCRIBE_RSP_MSG
#define T_MC_UNSUBSCRIBE_RSP_MSG      T_MC_UNSUBSCRIBE_RSP_MSG
#define MC_READ_REQ_MSG      MC_READ_REQ_MSG
#define MC_READ_RSP_MSG      MC_READ_RSP_MSG
#define T_MC_READ_RSP_MSG      T_MC_READ_RSP_MSG
#define MC_WRITE_REQ_MSG      MC_WRITE_REQ_MSG
#define T_MC_WRITE_REQ_MSG      T_MC_WRITE_REQ_MSG
#define MC_WRITE_RSP_MSG      MC_WRITE_RSP_MSG
#define T_MC_WRITE_RSP_MSG      T_MC_WRITE_RSP_MSG
#define MC_ERASE_GROUP_REQ_MSG      MC_ERASE_GROUP_REQ_MSG
#define T_MC_ERASE_GROUP_REQ_MSG      T_MC_ERASE_GROUP_REQ_MSG
#define MC_ERASE_GROUP_RSP_MSG      MC_ERASE_GROUP_RSP_MSG
#define T_MC_ERASE_GROUP_RSP_MSG      T_MC_ERASE_GROUP_RSP_MSG
#define MC_SET_PROTECT_REQ_MSG      MC_SET_PROTECT_REQ_MSG
#define T_MC_SET_PROTECT_REQ_MSG      T_MC_SET_PROTECT_REQ_MSG
#define MC_SET_PROTECT_RSP_MSG      MC_SET_PROTECT_RSP_MSG
#define T_MC_SET_PROTECT_RSP_MSG      T_MC_SET_PROTECT_RSP_MSG
#define MC_CLR_PROTECT_REQ_MSG      MC_CLR_PROTECT_REQ_MSG
#define T_MC_CLR_PROTECT_REQ_MSG      T_MC_CLR_PROTECT_REQ_MSG
#define MC_CLR_PROTECT_RSP_MSG      MC_CLR_PROTECT_RSP_MSG
#define T_MC_CLR_PROTECT_RSP_MSG      T_MC_CLR_PROTECT_RSP_MSG
#define MC_GET_PROTECT_REQ_MSG      MC_GET_PROTECT_REQ_MSG
#define MC_GET_PROTECT_RSP_MSG      MC_GET_PROTECT_RSP_MSG
#define T_MC_GET_PROTECT_RSP_MSG      T_MC_GET_PROTECT_RSP_MSG
#define MC_CARD_STATUS_REQ_MSG      MC_CARD_STATUS_REQ_MSG
#define T_MC_CARD_STATUS_REQ_MSG      T_MC_CARD_STATUS_REQ_MSG
#define MC_CARD_STATUS_RSP_MSG      MC_CARD_STATUS_RSP_MSG
#define T_MC_CARD_STATUS_RSP_MSG      T_MC_CARD_STATUS_RSP_MSG
#define MC_UPDATE_ACQ_REQ_MSG      MMC_UPDATE_ACQ_REQ_MSG
#define T_MC_UPDATE_ACQ_REQ_MSG      T_MMC_UPDATE_ACQ_REQ_MSG
#define MC_UPDATE_ACQ_RSP_MSG      MMC_UPDATE_ACQ_RSP_MSG
#define T_MC_UPDATE_ACQ_RSP_MSG      T_MMC_UPDATE_ACQ_RSP_MSG
#define MC_RESET_REQ_MSG      MMC_RESET_REQ_MSG
#define T_MC_RESET_REQ_MSG      T_MMC_RESET_REQ_MSG
#define MC_RESET_RSP_MSG      MMC_RESET_RSP_MSG
#define T_MC_RESET_RSP_MSG      T_MMC_RESET_RSP_MSG
#define MC_READ_OCR_REQ_MSG      MMC_READ_OCR_REQ_MSG
#define T_MC_READ_OCR_REQ_MSG      T_MMC_READ_OCR_REQ_MSG
#define MC_READ_OCR_RSP_MSG      MMC_READ_OCR_RSP_MSG
#define T_MC_READ_OCR_RSP_MSG      T_MMC_READ_OCR_RSP_MSG
#define MC_READ_CID_REQ_MSG      MMC_READ_CID_REQ_MSG
#define T_MC_READ_CID_REQ_MSG      T_MMC_READ_CID_REQ_MSG
#define MC_READ_CID_RSP_MSG      MMC_READ_CID_RSP_MSG
#define T_MC_READ_CID_RSP_MSG      T_MMC_READ_CID_RSP_MSG
#define MC_READ_CSD_REQ_MSG      MMC_READ_CSD_REQ_MSG
#define T_MC_READ_CSD_REQ_MSG      T_MMC_READ_CSD_REQ_MSG
#define MC_READ_CSD_RSP_MSG      MMC_READ_CSD_RSP_MSG
#define T_MC_READ_CSD_RSP_MSG      T_MMC_READ_CSD_RSP_MSG
#define MC_WRITE_CSD_REQ_MSG      MMC_WRITE_CSD_REQ_MSG
#define MC_WRITE_CSD_RSP_MSG      MMC_WRITE_CSD_RSP_MSG
#define T_MC_WRITE_CSD_RSP_MSG      T_MMC_WRITE_CSD_RSP_MSG
#define MC_INIT_MSG      MMC_INIT_MSG
#define T_MC_INIT_MSG      T_MMC_INIT_MSG
#define MC_NOTIFICATION_REQ_MSG      MMC_NOTIFICATION_REQ_MSG
#define T_MC_NOTIFICATION_REQ_MSG      T_MMC_NOTIFICATION_REQ_MSG
#define MC_NOTIFICATION_RSP_MSG      MMC_NOTIFICATION_RSP_MSG
#define T_MC_NOTIFICATION_RSP_MSG      T_MMC_NOTIFICATION_RSP_MSG
#define MC_EVENT_IND_MSG      MMC_EVENT_IND_MSG
#define T_MC_EVENT_IND_MSG      T_MMC_EVENT_IND_MSG
#define MC_SD_CARD_STATUS_REQ_MSG      MMC_SD_CARD_STATUS_REQ_MSG
#define T_MC_SD_CARD_STATUS_REQ_MSG      T_MMC_SD_CARD_STATUS_REQ_MSG
#define MC_SD_CARD_STATUS_RSP_MSG      MMC_SD_CARD_STATUS_RSP_MSG
#define T_MC_SD_CARD_STATUS_RSP_MSG      T_MMC_SD_CARD_STATUS_RSP_MSG
#define MC_READ_SCR_REQ_MSG      MMC_READ_SCR_REQ_MSG
#define T_MC_READ_SCR_REQ_MSG      T_MMC_READ_SCR_REQ_MSG
#define MC_READ_SCR_RSP_MSG      MMC_READ_SCR_RSP_MSG
#define T_MC_READ_SCR_RSP_MSG      T_MMC_READ_SCR_RSP_MSG
*/
/*@}*/

/*@}*/


#ifdef __cplusplus
}
#endif

#endif /* __MC_MESSAGE_H_ */
