/**
 * @file  nan_bm_hw_version.h
 *
 * TASK Definition for nan_bm SWE.
 *  BOARD file
 * @author  ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  2/27/2006 J.A. Renia
 *
 * (C) Copyright 2006 by TI, All Rights Reserved
 */

#ifndef __NAN_BM_HW_FUNCTIONS_H_
#define __NAN_BM_HW_FUNCTIONS_H_

#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */
#include "sys_conf.h"
#include "nan/nan_api.h"
#include "chipset.cfg"
#include "nan/nan_i.h"
#include "nan/nan_cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif



#ifdef NAN_BM_POLLING
static T_RVM_RETURN nan_bm_poll_event (UINT32 event);
#else 
static void nan_bm_poll_end_transfer (void);
#endif

#ifdef NAN_BM_POLLING

/*just a random value */
#define NAN_BM_POLL_EVENT_MAX 100000
#define NAN_BM_POLL_COUNT_MAX 1000

#endif


#define COL_OFFSET_0 0
#define COL_OFFSET_1 1
#define COL_OFFSET_2 2

#ifdef NAN_BM_32_BITS_FIFO
#define NAN_BUFFER_ACCESS_SIZE 4  //  8,16,32 bits of data from buffer can be accessed by CPU 
#endif

void
nan_bm_poll_bad_blocks (void);

/* to define the macros to refer mask bits of controller registers */

#define COMMAND_BYTES_MASK 	0x03U
#define ADDRESS_BYTES_MASK	0x07U
#define DATA_BYTES_MASK		0x1ffffU
#define CLK_MASK				0x0fU

#if(CHIPSET == 15)


typedef unsigned char T_MUX_DEVS;

void conf_acquire_pins(T_MUX_DEVS dev_id);
void conf_release_pins(T_MUX_DEVS dev_id);
void initialize_nand_mutex();
void delete_nand_mutex();
BOOL is_nand_mutex_initialized();

#endif


/********************************************************************************/
/*                                                                              */
/*    Function Name:   nan_bm_hw_handle_read_req                                */
/*                                                                              */
/*    Purpose:     Reads data from the NAND from any location.                  */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - chip_select : must be <= NAN_MAX_CHIP_SELECT                   */
/*             - mode : mode0, 1, 2 (lower half, upper half, spare)             */
/*             - column : column offset within the page                         */
/*             - row : row (page) number                                        */
/*             - data_p : where to put the data                                 */
/*             - data_size : # of bytes to be read                              */
/*             - ecc_values_p : returned ECC values, allocated by caller        */
/*             - ecc_sizes_p : selected ECC spacings                            */
/*             - ecc_count : size of the ecc_values_p and ecc_size_p arrays     */
/*                                                                              */
/*    Return :                                                                  */
/*             - RV_OK                                                          */
/*             - RV_INVALID_PARAMETER                                           */
/*                                                                              */
/********************************************************************************/
T_NAN_BMD_RET nan_bm_hw_handle_read_req (UINT8 chip_select,
                                        T_NAN_BM_RW_MODE mode, 
                                        UINT32 row,
                                        UINT32 column,
                                        UINT32*data_p,
                                        UINT32 data_size, 
                                        T_NAN_BM_ECC *ecc_values_p,
                                        UINT32 *ecc_sizes_p,
                                        UINT32 ecc_count);


/********************************************************************************/
/*                                                                              */
/*    Function Name:   nan_bm_hw_handle_program_req                             */
/*                                                                              */
/*    Purpose:     Writes data to the NAND to any location.                     */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - chip_select : must be <= NAN_MAX_CHIP_SELECT                   */
/*             - mode : mode0, 1, 2 (lower half, upper half, spare)             */
/*             - column : column offset within the page                         */
/*             - row : row (page) number                                        */
/*             - data_p : where to put the data                                 */
/*             - data_size : # of bytes to be read                              */
/*             - ecc_values_p : returned ECC values, allocated by caller        */
/*             - ecc_sizes_p : selected ECC spacings                            */
/*             - ecc_count : size of the ecc_values_p and ecc_size_p arrays     */
/*                                                                              */
/*    Return :                                                                  */
/*             - RV_OK                                                          */
/*             - RV_INVALID_PARAMETER                                           */
/*                                                                              */
/********************************************************************************/
T_NAN_BMD_RET nan_bm_hw_handle_program_req ( UINT8 chip_select,
                                            T_NAN_BM_RW_MODE mode,
                                            UINT32 row,         
                                            UINT32 column, 
                                            UINT32 *data_p,      
                                            UINT32 data_size,
                                            T_NAN_BM_ECC *ecc_values_p,  
                                            UINT32 *ecc_sizes_p,
                                            UINT32 ecc_count );


/********************************************************************************/
/*                                                                              */
/*    Function Name:   nan_bm_handle_read_status_req                            */
/*                                                                              */
/*    Purpose:     Reads the NAND status register                               */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - chip_select : must be <= NAN_MAX_CHIP_SELECT                   */
/*                                                                              */
/*    Return :                                                                  */
/*             - RV_OK                                                          */
/*             - RV_INVALID_PARAMETER                                           */
/*                                                                              */
/********************************************************************************/
UINT16 nan_bm_handle_read_status_req      ( UINT8 chip_select);

/********************************************************************************/
/*                                                                              */
/*    Function Name:   nan_bm_hw_flash_erase_block                              */
/*                                                                              */
/*    Purpose:     Erases a NAND block                                          */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - chip_select : must be <= NAN_MAX_CHIP_SELECT                   */
/*             - block number: NAND block to erase                              */
/*    Return :                                                                  */
/*             - RV_OK                                                          */
/*             - RV_INVALID_PARAMETER                                           */
/*                                                                              */
/********************************************************************************/
T_NAN_BMD_RET nan_bm_hw_flash_erase_block  ( UINT8 chip_select, UINT32 block_number);


/********************************************************************************/
/*                                                                              */
/*    Function Name:   nan_bm_handle_copy_req                                   */
/*                                                                              */
/*    Purpose:     Copies a NAND page using the hardware build in copy back     */
/*                 function.                                                    */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - chip_select : must be <= NAN_MAX_CHIP_SELECT                   */
/*             - src_row : source row (page)                                    */
/*             - src_column : source column(page offset)                        */
/*             - dest_row : destination row (page)                              */
/*             - dest_column : destination column(page offset)                  */
/*    Return :                                                                  */
/*             - RV_OK                                                          */
/*             - RV_INVALID_PARAMETER                                           */
/*                                                                              */
/********************************************************************************/
T_NAN_BMD_RET nan_bm_handle_copy_req       ( UINT8 chip_select, UINT32 src_row,
                                            UINT32 src_column, UINT32 dest_row,
                                            UINT32 dest_column);

T_NAN_BMD_RET nan_bm_handle_reset_req      ( UINT8 chip_select);



static void nan_bm_wait_event_to          ( UINT16 expected_event, 
                                            UINT16 saved_event, 
                                            UINT16 timeout);

static BOOL nan_bm_ecc_params_ok          ( UINT32 *ecc_sizes_p, 
                                            T_NAN_BM_ECC *ecc_values_p,
                                            UINT32 ecc_count,
                                            UINT32 data_size);

#ifdef __cplusplus
}
#endif


#endif /*__NAN_BM_HW_FUNCTIONS_H_*/

