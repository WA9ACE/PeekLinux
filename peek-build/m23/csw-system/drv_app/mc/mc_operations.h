/**
 * @file  mc_operations.h
 *
 * Operations definitions for the MMC instance.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/23/2003  ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __MC_OPERATIONS_H_
#define __MC_OPERATIONS_H_


/**
 * @name Operations Functions
 *
 * These functions are called by the handle message to do
 * some administrative work and call the MMC commands.
 */
/*@{*/

/**
 * Initialises the MMC SD driver
 *
 * This function will initialise the MMC SD driver.
 * First the mmc driver will be put to initialised to 
 * enable event handling.
 * Second, the host controller will be initialised at low speed.
 * Third a card aquisition will be started to indentify
 * the available cards.
 * When this is succesfull the hostcontroller will
 * be set into normal operation speed
 *
 *
 * @return  T_RV_RET:
 */
extern T_RV_RET mc_op_init_driver(void);


/**
 * function: mmc_op_driver_not_ready
 */
extern T_RV_RET mc_op_driver_not_ready(T_RV_HDR *msg_p);


/**
 * function: mmc_op_subscribe
 */
extern T_RV_RET mc_op_subscribe(T_MC_SUBSCRIBE_REQ_MSG *msg_p);


/**
 * function: mc_op_unsubscribe
 */
extern T_RV_RET mc_op_unsubscribe(T_MC_UNSUBSCRIBE_REQ_MSG *msg_p);


/**
 * function: mmc_op_update_acq
 */
extern T_RV_RET mc_op_update_acq_req(T_MC_UPDATE_ACQ_REQ_MSG *msg_p);


/**
 * function: mmc_op_reset_req
 */
extern T_RV_RET mc_op_reset_req(T_MC_RESET_REQ_MSG *msg_p);


/**
 * function: mmc_op_read_req
 */
extern T_RV_RET mc_op_read_req(T_MC_READ_REQ_MSG *msg_p);


/**
 * function: mmc_op_write_req
 */
extern T_RV_RET mc_op_write_req(T_MC_WRITE_REQ_MSG *msg_p);


/**
 * function: mmc_op_read_ocr_req
 */
extern T_RV_RET mc_op_read_ocr_req(T_MC_READ_OCR_REQ_MSG *msg_p);


/**
 * function: mmc_op_read_cid_req
 */
extern T_RV_RET mc_op_read_cid_req(T_MC_READ_CID_REQ_MSG *msg_p);


/**
 * function: mmc_op_card_status_req
 */
extern T_RV_RET mc_op_read_csd_req(T_MC_READ_CSD_REQ_MSG *msg_p);


/**
 * function: mmc_op_card_status_req
 */
extern T_RV_RET mc_op_card_status_req(T_MC_CARD_STATUS_REQ_MSG *msg_p);


/**
 * function: mmc_op_erase_group
 */
extern T_RV_RET mc_op_erase_group_req(T_MC_ERASE_GROUP_REQ_MSG *msg_p);


/**
 * function: MC_op_write_csd_req
 */
extern T_RV_RET mc_op_write_csd_req(T_MC_WRITE_CSD_REQ_MSG *msg_p);


///**
// * function: mc_op_erase_csd_req
// */
//extern T_RV_RET mc_op_erase_csd_req(T_MC_ERASE_CSD_REQ_MSG *msg_p);


/**
 * function: mmc_op_get_prot_req
 */
extern T_RV_RET mc_op_get_prot_req(T_MC_GET_PROTECT_REQ_MSG *msg_p);


/**
 * function: mmc_op_set_prot_req
 */
extern T_RV_RET mc_op_set_prot_req(T_MC_SET_PROTECT_REQ_MSG *msg_p);


/**
 * function: mmc_op_clr_prot_req
 */
extern T_RV_RET mc_op_clr_prot_req(T_MC_CLR_PROTECT_REQ_MSG *msg_p);


/**
 * Retrieves the SD card status
 *
 * This function will retrieve the SD card status
 *
 * @param   msg_p Reference to message
 *                      
 * @return  RV_OK
 *          RV_INTERNAL_ERR
 */
extern T_RV_RET mc_op_sd_card_status_req(T_MC_SD_CARD_STATUS_REQ_MSG *msg_p);

/**
 * Retrieves the scr register
 *
 * This function will retrieve the scr register
 *
 * @param   msg_p Reference to message
 *                      
 * @return  RV_OK
 *          RV_INTERNAL_ERR
 */
extern T_RV_RET mc_op_read_scr_req_msg(T_MC_READ_SCR_REQ_MSG *msg_p);

/**
 * Will subscribe a subscriber to certain card events
 *
 * This function will subscribe the subscriber to the desired events
 *
 * @param   msg_p     Reference to message
 *                    
 *                      
 * @return  RV_OK
 *          RV_INTERNAL_ERR
 */
extern T_RV_RET mc_op_send_notification(T_MC_NOTIFICATION_REQ_MSG *msg_p);

/**
 * Handle a card insert/remove action and will send an event
 *
 * This function will handle a card insert action and will 
 * send all subscribers to the MMC_EVENT_INSERTION event a message
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */
T_RV_RET mc_op_handle_card_event(T_MC_EVENTS event);

/**
 * Handle a card insert/remove action and will send an event
 *
 * This function will handle a card insert action and will 
 * send all subscribers to the MMC_EVENT_INSERTION event a message
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */
extern T_RV_RET mc_op_handle_card_event(T_MC_EVENTS event);


#endif /* __MC_OPERATIONS_H_ */
