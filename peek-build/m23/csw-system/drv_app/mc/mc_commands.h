/**
 * @file  mc_commands.h
 *
 * TASK for MC SWE.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  6/3/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

/*
 * Note: This module will be build for target environment.
 */
#ifndef __MC_COMMANDS_H_
#define __MC_COMMANDS_H_

#ifndef _WINDOWS
#include "memif/mem.h"
#endif

#include "mc/mc_i.h"
#include "mc/mc_api.h" 

#ifdef __cplusplus
extern "C"
{
#endif

/** HIGH and LOW voltage range */
#define MC_HIGH_VOLT_RANGE     0xFF8000
#define MC_LOW_VOLT_RANGE      0xFF8010

/** CARD states */
/** These defines reflect the states available */
/** in the status returened from the SDMC card*/
#define MC_CARD_STATE_IDLE       (0)
#define MC_CARD_STATE_READY      (1)
#define MC_CARD_STATE_INDENT     (2)
#define MC_CARD_STATE_STBY       (3)
#define MC_CARD_STATE_TRAN       (4)
#define MC_CARD_STATE_DATA       (5)
#define MC_CARD_STATE_RCV        (6)
#define MC_CARD_STATE_PRG        (7)
#define MC_CARD_STATE_DIS        (8)

/** 
 * function: mc_wait_card_state
 */ 
//extern T_RV_RET mc_wait_card_state(T_MC_RCA rca, UINT16 req_state, UINT8 mode);
extern T_RV_RET mc_wait_card_state(UINT16 req_state, UINT8 mode);
/** 
 * Initialise host controller 
 *
 * Detailled description
 * Initialises the host controller. Sets configuration registers
 *
 * @return RV_OK
 */ 
extern T_RV_RET mc_cmd_init_host (T_MC_HOST_CONF *conf, UINT16 irq_mask);
extern T_RV_RET mc_cmd_update_acq(void);
extern T_RV_RET mc_cmd_init_stack (void);
//extern T_RV_RET mc_cmd_check_stack (T_MC_RCA *rca_p, UINT8 *stack_size);
extern T_RV_RET mc_cmd_check_stack (UINT8 *stack_size);

//extern T_RV_RET mc_cmd_setup_card (T_MC_RCA rca, UINT8 *csd_p,UINT32 *card_stat_p);
extern T_RV_RET mc_cmd_setup_card (UINT8 *csd_p,UINT32 *card_stat_p);

extern T_RV_RET mc_cmd_stream_read (BOOL enable_dma, UINT16 nblk, 
                                     UINT16 blk_size, UINT32 addr, 
                                     UINT8 *data_p, UINT32 data_size, 
                                     UINT32 *card_stat_p);

extern T_RV_RET mc_cmd_read_block (BOOL enable_dma, UINT16 blk_len, 
                                    UINT32 addr, UINT8 *data_p,
                                    UINT32 data_size, UINT32 *card_stat_p);

extern T_RV_RET mc_cmd_read_mblock (T_MC_DMA_MODE enable_dma, UINT16 nblk,
                                     UINT16 blk_len, UINT32 addr, 
                                     UINT8 *data_p, UINT32 data_size, 
                                     UINT32 *card_stat_p);

extern T_RV_RET mc_cmd_write_block (BOOL enable_dma, UINT16 blk_len, 
                                     UINT32 addr, UINT8 *data_p, 
                                     UINT32 data_size, UINT32 *card_stat_p);

extern T_RV_RET mc_cmd_write_mblock (T_MC_DMA_MODE enable_dma, UINT16 nblk,
                                      UINT16 blk_len, UINT32 addr, 
                                      UINT8 *data_p, UINT32 data_size, 
                                      UINT32 *card_stat_p);

//extern T_RV_RET mc_cmd_erase_group (T_MC_RCA rca,T_MC_CARD_TYPE card_type,UINT32 start_addr, UINT32 end_addr,UINT32  *card_stat_p);
extern T_RV_RET mc_cmd_erase_group (T_MC_CARD_TYPE card_type,UINT32 start_addr, UINT32 end_addr,UINT32  *card_stat_p);

extern T_RV_RET mc_cmd_stream_write (BOOL enable_dma, UINT16 nblk, 
                                      UINT16 blk_len, UINT32 addr, 
                                      UINT8 *data_p, UINT32 data_size, 
                                      UINT32 *card_stat_p);
extern T_RV_RET mc_cmd_send_controller_status (UINT16 *status_p);

extern T_RV_RET mc_cmd_reset (void);

//extern T_RV_RET mc_cmd_send_csd (T_MC_RCA rca, UINT8 *csd_p);
extern T_RV_RET mc_cmd_send_csd ( UINT8 *csd_p);

//extern T_RV_RET mc_cmd_send_cid (T_MC_RCA rca, UINT8 *cid_p);
extern T_RV_RET mc_cmd_send_cid (UINT8 *cid_p);

extern T_RV_RET mc_cmd_send_status (UINT32 *card_stat_p);

extern T_RV_RET mc_cmd_write_csd (UINT8 *csd_p, T_MC_CSD_FIELD field, UINT8 value,
                                   UINT32 *card_stat_p);
extern T_RV_RET mc_cmd_deselect_card (UINT32 *card_stat_p);
//extern T_RV_RET mc_cmd_set_write_protect(T_MC_RCA rca, UINT32 wr_prot_group,UINT32 *card_stat_p);
extern T_RV_RET mc_cmd_set_write_protect(UINT32 wr_prot_group,UINT32 *card_stat_p);


extern T_RV_RET mc_cmd_get_write_protect(UINT16 blk_len, UINT32 wr_prot_group, 
                                          UINT32 *wr_prot_grsp, UINT32 *card_stat_p);


//extern T_RV_RET mc_cmd_clr_write_protect(T_MC_RCA rca, UINT32 wr_prot_group, 
//                                          UINT32 *card_stat_p);
extern T_RV_RET mc_cmd_clr_write_protect(UINT32 wr_prot_group, 
                                          UINT32 *card_stat_p);



extern UINT16   mc_cmd_get_hw_version(void);

//extern T_RV_RET mc_cmd_sd_send_status(UINT16 rca,
//                                UINT8   *sd_status_p,
//                                UINT32  *card_stat_p);
extern T_RV_RET mc_cmd_sd_send_status(
                                UINT8   *sd_status_p,
                                UINT32  *card_stat_p);


/** 
 * get scr register 
 *
 * Detailled description. 
 * Retreives the SCR register from a SD-card
 *
 * @return  RV_OK 
 */ 
//T_RV_RET mc_cmd_send_scr(UINT16  rca,UINT8 *scr_p,UINT32  *card_stat_p);
T_RV_RET mc_cmd_send_scr(UINT8 *scr_p,UINT32  *card_stat_p);
                          
                          
#ifdef __cplusplus
}
#endif


#endif /*__MC_COMMANDS_H_*/
