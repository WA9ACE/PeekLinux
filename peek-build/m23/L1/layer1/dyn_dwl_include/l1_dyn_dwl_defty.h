/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1_DYN_DWL_DEFTY.H
 *
 *        Filename l1_dyn_dwl_defty.h
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#if(L1_DYN_DSP_DWNLD==1)

#ifndef _L1_DYN_DWL_DEFTY_H_
#define _L1_DYN_DWL_DEFTY_H_

#include "l1_dyn_dwl_const.h"

/***************************************************************************************/
/* Dynamic download L1A structure to store dynamic download L1A global variables       */
/***************************************************************************************/
typedef struct
{
  UWORD32 signal_code_vect[MAX_NUM_OF_PATCH_IDS];
  UWORD32 num_of_elem;
}T_FIFO_DYN_DWNLD ;
typedef struct
{
 UWORD8  state;
 UWORD16 num_patches_installed;
 UWORD16 patch_id[MAX_NUM_OF_PATCH_IDS];
 UWORD16 num_of_elem_to_copy;
 UWORD16 next_patch_id[MAX_NUM_OF_PATCH_IDS];
 BOOL    semaphore_vect[MAX_NUM_OF_SEMAPHORES];
 BOOL    melody0_E2_flag_activated;
 BOOL    melody1_E2_flag_activated; 
 UWORD16 num_of_primitives;
 T_FIFO_DYN_DWNLD waiting_patch_fifo;
}T_L1A_DYN_DWNLD_GLOBAL;

/***************************************************************************************/
/* Dynamic DSP L1A-L1S structure...                                                    */
/***************************************************************************************/
typedef struct
{
  BOOL start;
  BOOL stop;
} T_DYN_DWNLD_TASK_COMMAND;

/***************************************************************************************/
/* Dynamic DSP download HISR - L1A structure...                                        */
/***************************************************************************************/
typedef struct
{
  BOOL  restart;
} T_DYN_DWNLD_HISR_COMMAND;

typedef struct
{
  UWORD16 num_of_elem;
  UWORD32 address[MAX_NUM_OF_PATCH_IDS];
} T_DYN_DWNLD_HISR_UN_PARAM;

typedef struct  
{
  UWORD32 start_MCU_copy_address;
  UWORD32 address_to_install;
  UWORD16 crc;
  UWORD16 size_array;
}T_DYN_PATCH_DATA;

typedef struct
{
  UWORD16 size_of_dwnld_area;
  UWORD16 start_of_dwnld_area; 
  UWORD16 num_of_elem;
  T_DYN_PATCH_DATA patch_data[MAX_NUM_OF_PATCH_IDS]; 
}T_DYN_DWNLD_HISR_CPY_PARAM;

typedef struct
{
  T_DYN_DWNLD_HISR_COMMAND command;
  T_DYN_DWNLD_HISR_UN_PARAM uninstall_parameters;
  T_DYN_DWNLD_HISR_CPY_PARAM copy_parameters;
} T_L1A_DYN_DWNLD_HISR_COM;

/***************************************************************************************/
/* Dynamic download HISR structure to store dynamic download HISR global variables     */
/***************************************************************************************/

typedef struct
{
  UWORD8 state;
  BOOL   running;
  UWORD16 uninstall_counter;
  UWORD16 patch_ids_counter;
  UWORD32 running_source_pointer;
  UWORD16 tmp_patch_size;
} T_L1_DYN_DWNLD_API_HISR;

/***************************************************************************************/
/* Dynamic Download MCU-DSP API                                                        */
/***************************************************************************************/
typedef struct
{
  API d_api_dwl_download_ctrl;
  API d_api_dwl_error_code;
  API d_api_dwl_function_address[2];
  API d_api_dwl_crc;
  API d_api_dwl_size;
  API d_api_dwl_write_pointer;
} T_DYN_DWNLD_MCU_DSP;

typedef struct
{
  UWORD32 primitive;
  UWORD16 msg_id;
}T_SIGNAL_PATCH;

#endif  // _L1_DYN_DWL_DEFTY_H_
#endif  // L1_DYN_DSP_DWNLD

