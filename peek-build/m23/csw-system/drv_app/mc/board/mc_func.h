/**
 * @file  mc_func.h
 *
 * Library functions for MC SWE.
 *
 * @author  ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/4/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __MC_FUNC_H_
#define __MC_FUNC_H_

#include "mc/mc_i.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name MC commands
 *
 * MC functions declarations.
 */
/*@{*/

/**
 * CRC7 Calculation
 *
 * Detailled description.
 * This function executes calculation of CRC7
 *
 * @return  UINT8
 */
UINT8  get_CRC7(UINT8 cmd_bytes[]);

/**
 * Send MC CMD0 command
 *
 * Detailled description.
 * This command executes CMD0. It resets all card states to idle.
 *
 * @return  RV_OK
 */
//extern T_RV_RET go_idle_state(T_MC_CARD_TYPE card_type);
extern T_RV_RET go_idle_state();

// Executes CMD55
extern T_RV_RET Send_App_Cmd();

// Executes ACMD41
extern UINT16 Send_SD_Op_Cond();

/**
 * Send MC CMD1 command
 *
 * Detailled description.
 * This command executes CMD1. It resets all card states to idle.
 *
 * @return  RV_OK
 */
//extern T_RV_RET send_op_cond(T_MC_OCR ocr, T_MC_OCR *ocr_p);
extern T_RV_RET send_op_cond();

/**
 * Send MC CMD2 command
 *
 * Detailled description.
 * This command executes CMD2. It asks all cards to send their CID numbers on
 * the CMD line.
 *
 * @return  RV_OK
 */
//extern T_RV_RET all_send_cid(UINT8 *cid_p, T_MC_CARD_TYPE card_type);


/**
 * Send MC CMD3 command
 *
 * Detailled description.
 * This command executes CMD3. It assigns relative address to the card.
 *
 * @return  RV_OK
 */
//extern T_RV_RET set_relative_addr (T_MC_RCA rca, UINT32 *card_stat_p);


/**
 * Send MC CMD4 command
 *
 * Detailled description.
 * This command executes CMD4. It programs the DSR of all cards.
 *
 * @return  RV_OK
 */
//extern T_RV_RET set_dsr (T_MC_DSR dsr, UINT32 *card_stat_p);


/**
 * Send MC CMD7 command
 *
 * Detailled description.
 * This command executes CMD7. It command toggles a card between the stand-by
 * and transfer states or between the programming and disconnect states. In
 * both cases the card is selected by its own relative address and gets
 * deselected by any other address; address 0 deselects all.
 *
 * @return  RV_OK
 */
extern T_RV_RET select_deselect_card (UINT32 *card_stat_p);


/**
 * Send MC CMD7 command
 *
 * Detailled description.
 * This command executes CMD7. It command toggles a card between the stand-by
 * and transfer states or between the programming and disconnect states. In
 * both cases the card is selected by its own relative address and gets
 * deselected by any other address; address 0 deselects all.
 *
 * @return  RV_OK
 */
extern T_RV_RET deselect_card (UINT32 *card_stat_p);


/**
 * Send MC CMD9 command
 *
 * Detailled description.
 * This command executes CMD9. It addressed card sends its card-specific
 * data (CSD) on the CMD line.
 *
 * @return  RV_OK
 */
extern T_RV_RET send_csd (UINT8 *csd_p);


/**
 * Send MC CMD10 command
 *
 * Detailled description.
 * This command executes CMD10. It addressed card sends its card identification
 * (CID) on CMD the line.
 *
 * @return  RV_OK
 */
extern T_RV_RET send_cid (UINT8 *cid_p);


/**
 * Send MC CMD12 command
 *
 * Detailled description.
 * This command executes CMD12. It Terminates a read/write stream/multiple
 * block operation. When CMD12 is used to terminate a read transaction the
 * card will respond with R1. When it is used to stop a write transaction
 * the card will respond with R1b.
 *
 * @return  RV_OK
 */
extern T_RV_RET stop_transmission (BOOL stop_read, UINT32 *card_stat_p);


/**
 * Send MC CMD13 command
 *
 * Detailled description.
 * This command executes CMD13. It addressed card sends its status
 * register.
 *
 * @return  RV_OK
 */
extern T_RV_RET send_status (UINT32 *card_stat_p);


/**
 * Send MC CMD15 command
 *
 * Detailled description.
 * This command executes CMD15. It sets the card to inactive state in order
 * to protect the card stack against communication breakdowns.
 *
 * @return  RV_OK
 */
//extern T_RV_RET go_inactive_state (T_MC_RCA rca);


/**
 * Send MC CMD11 command
 *
 * Detailled description.
 * This command executes CMD11. It reads data stream from the card, starting
 * at the given address, until a STOP_TRANSMISSION follows.
 *
 * @return  RV_OK
 */
extern T_RV_RET read_dat_until_stop (UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD16 command
 *
 * Detailled description.
 * This command executes CMD16. It sets the block length (in bytes) for all
 * following block commands (read and write). Default block length is
 * specified in the CSD.
 *
 * @return  RV_OK
 */
extern T_RV_RET set_blocklen (UINT32 blocklen, UINT32 *card_stat_p);


/**
 * Send MC CMD17 command
 *
 * Detailled description.
 * This command executes CMD17. It reads a block of the size selected by the
 * SET_BLOCKLEN command.
 *
 * @return  RV_OK
 */
//extern T_RV_RET read_single_block (UINT32 data_addr, UINT32 *card_stat_p,UINT32 data_length,UINT16 *data_ptr);
extern T_RV_RET read_single_block (UINT32 data_addr, UINT32 *card_stat_p);

/**
 * Send MC CMD18 command
 *
 * Detailled description.
 * This command executes CMD18. It continuously transfers data blocks from
 * card to host until interrupted by a stop command or the requested number
 * of data block transmitted
 *
 * @return  RV_OK
 */
extern T_RV_RET read_multiple_block (UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD23 command
 *
 * Detailled description.
 * This command executes CMD23. It Defines the number of blocks which are
 * going to be transferred in the immediately succeeding multiple block read
 * or write command.
 *
 * @return  RV_OK
 */
extern T_RV_RET set_block_count (UINT16 block_count, UINT32 *card_stat_p);


/**
 * Send MC CMD20 command
 *
 * Detailled description.
 * This command executes CMD20. It writes data stream from the host, starting
 * at the given address, until a STOP_TRANSMISSION follows.
 *
 * @return  RV_OK
 */
extern T_RV_RET write_dat_until_stop (UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD24 command
 *
 * Detailled description.
 * This command executes CMD24. It writes a block of the size selected by the
 * SET_BLOCKLEN command
 *
 * @return  RV_OK
 */
extern T_RV_RET write_single_block (UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD25 command
 *
 * Detailled description.
 * This command executes CMD25. It continuously writes blocks of data until a
 * STOP_TRANSMISSION follows or the requested number of block received.
 *
 * @return  RV_OK
 */
extern T_RV_RET write_multiple_block (UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD26 command
 *
 * Detailled description.
 * This command executes CMD26. It programming of the card identification
 * register. This command shall be issued only once per card. The card
 * contains hardware to prevent this operation after the first programming.
 * Normally this command is reserved for the manufacturer.
 *
 * @return  RV_OK
 */
//extern T_RV_RET program_cid (UINT32 *card_stat_p);


/**
 * Send MC CMD27 command
 *
 * Detailled description.
 * This command executes CMD27. It programming of the programmable bits of
 * the CSD.
 *
 * @return  RV_OK
 */
extern T_RV_RET program_csd (UINT32 *card_stat_p);


/**
 * Send MC CMD35 command
 *
 * Detailled description.
 * This command executes CMD35. It sets the address of the first erase group
 * within a range to be selected for erase
 *
 * @return  RV_OK
 */
extern T_RV_RET erase_group_start (UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD36 command
 *
 * Detailled description.
 * This command executes CMD36. It sets the address of the last erase group
 * within a continuous range to be selected for erase.
 *
 * @return  RV_OK
 */
extern T_RV_RET erase_group_end (UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD38 command
 *
 * Detailled description.
 * This command executes CMD38. It erases all previously selected write
 * blocks
 *
 * @return  RV_OK
 */
extern T_RV_RET erase (UINT32 *card_stat_p);


/**
 * Send MC CMD28 command
 *
 * Detailled description.
 * This command executes CMD28. It if the card has write protection features,
 * this command sets the write protection bit of the addressed group. The
 * properties of write protection are coded in the card specific data
 * (WP_GRP_SIZE).
 *
 * @return  RV_OK
 */
extern T_RV_RET set_write_prot (UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD29 command
 *
 * Detailled description.
 * This command executes CMD29. It if the card provides write protection
 * features, this command clears the write protection bit of the
 * addressed group.
 *
 * @return  RV_OK
 */
extern T_RV_RET clr_write_prot (UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD30 command
 *
 * Detailled description.
 * This command executes CMD30. It if the card provides write protection
 * features, this command asks the card to send the status of the write
 * protection bits.
 *
 * @return  RV_OK
 */
extern T_RV_RET send_write_prot (UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD32 command
 *
 * Detailled description.
 * This command executes CMD32. It sets the address of the first write
 * block to be erased
 *
 * @return  RV_OK
 */
extern T_RV_RET erase_wr_blk_start(UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD33 command
 *
 * Detailled description.
 * This command executes CMD33. It sets the address of the last write
 * block of the continuous range to be erased.
 *
 * @return  RV_OK
 */
extern T_RV_RET erase_wr_blk_end(UINT32 data_addr, UINT32 *card_stat_p);


/**
 * Send MC CMD55 command
 *
 * Detailled description.
 * This command executes CMD55. Indicates to the card that the next
 * command is an application specific command rather than a
 * standard command.
 *
 * @return  RV_OK
 */
extern T_RV_RET app_cmd (UINT32 *card_stat_p);


/**
 * Send MC CMD56 command
 *
 * Detailled description.
 * This command executes CMD56. Used either to transfer a data block
 * to the card or to get a data block from the card for general purpose/
 * application specific commands. The size of the data block shall be
 * set by the SET_BLOCK_LEN command.
 *
 * @return  RV_OK
 */
extern T_RV_RET gen_cmd (BOOL rd_wr, UINT32 *card_stat_p);


/**
 * Send MC ACMD6 command
 *
 * Detailled description.
 * This command executes ACMD6. Defines the data bus width ('00'=1bit or
 * '10'=4 bit bus) to be used for data transfer. The allowed data bus widths
 * are given in the SCR register
 *
 * @return  RV_OK
 * NOT SUPPORTED IN SPI MODE
 */
//extern T_RV_RET sd_set_bus_wdth (T_MC_RCA rca, UINT8 bus_width, UINT32 *card_stat_p);


/**
 * Send ACMD13 command
 *
 * Detailled description.
 * This command executes ACMD13. Send the SD Memory Card Status. The
 * status fields are given in the table 26 [SD Specification].
 *
 * @return  RV_OK
 * NOT SUPPORTED IN SPI MODE
 */
//extern T_RV_RET sd_status (UINT32 *card_stat_p);
extern T_RV_RET sd_status (UINT8 *sd_status_p);

/**
 * Send ACMD22 command
 *
 * Detailled description.
 * This command executes ACMD22. Send the number of written (with-
 * out errors) write blocks. Responds with 32bit + CRC data block.
 *
 * @return  RV_OK
 * NOT SUPPORTED IN SPI MODE
 */
//extern T_RV_RET send_num_wr_blocks (UINT32 *card_stat_p);


/**
 * Send ACMD23 command
 *
 * Detailled description.
 * This command executes ACMD23. Set the number of write blocks to be
 * pre-erased before writing (to be used for faster Multiple Block WR com-
 * mand). "1"=default (one wr block).
 *
 * @return  RV_OK
 * NOT SUPPORTED IN SPI MODE
 */
//extern T_RV_RET set_wr_blk_erase_count (UINT32 num_of_blocks, UINT32 *card_stat_p);


/**
 * Send ACMD41 command
 *
 * Detailled description.
 * This command executes ACMD41. Asks the accessed card to send its
 * operating condition register (OCR) content in the response on the CMD
 * line.
 * CMD55 will be called from here
 *
 * @return  RV_OK
 * NOT SUPPORTED IN SPI MODE
 */
//extern T_RV_RET sd_send_op_cond (T_MC_OCR ocr, T_MC_OCR *ocr_p);


/**
 * Send ACMD42 command
 *
 * Detailled description.
 * This command executes ACMD42. Connect[1]/Disconnect the 50KOhm
 * pull-up resistor on CD/DAT3 (pin 1) of the card. The pull-up may be used for
 * card detection.
 *
 * @return  RV_OK
 * NOT SUPPORTED IN SPI MODE
 */
//extern T_RV_RET set_clr_card_detect (BOOL set_cd, UINT32 *card_stat_p);


/**
 * Send ACMD51 command
 *
 * Detailled description.
 * This command executes ACMD51. Reads the SD Configuration Register
 * (SCR).
 *
 * @return  RV_OK
 * NOT SUPPORTED IN SPI MODE
 */
// extern T_RV_RET send_scr (UINT32 *card_stat_p);
 extern T_RV_RET send_scr (UINT8 *card_stat_p);

/**
 * Send MC CMD3 command
 *
 * Detailled description.
 * This command executes CMD3. It asks the card to publish a new
 * relative address (RCA
 *
 * @return  RV_OK
 */


/*
 * Send MC CMD58 command
 *
 * Detailled description.
 * This command executes CMD58. It asks the card for the OCR register
 *
 *
 * @return  RV_OK
 */
T_RV_RET send_ocr(UINT32 *card_stat_p);
//T_RV_RET send_ocr(void);

extern T_RV_RET send_relative_addr(UINT32 *card_stat_p);
/*@}*/

#ifdef __cplusplus
}
#endif

/***************************ADDED FOR LOCOSTO CMD TOKEN CREATION**************/
#define MC_GPIO_1   1
#define MC_GPIO_26  26
#define MC_CMD_SIZE 6
#define MC_RESP_SIZE 2
#define MC_SEND_SIZE 512
#define MC_RECEIVE_SIZE 512
#define CMD_TOKEN_START_BIT 0x00
#define CMD_TOKEN_TRANSMIT_BIT 0x40
#define CMD_TOKEN_END_BIT 0x01

#define START_BYTE 0xFE;
#define START_MULTI_BYTE 0xFC;
#define STOP_BYTE 0xFD;
#define CMD1_TRIALS_MAX 5;

/*****************************************************************************/


#endif /*__MC_FUNC_H_*/

