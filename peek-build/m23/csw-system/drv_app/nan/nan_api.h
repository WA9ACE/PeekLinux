/**
 * @file  nan_bm_api.h
 *
 * API Definition for NAN_BM SWE.
 *
 * @author  J.A. Renia
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  2/27/2006 J.A. Renia    Create.
 *
 * (C) Copyright 2006 by ICT Embedded, All Rights Reserved
 */

#ifndef __NAN_BM_API_H_
#define __NAN_BM_API_H_

#include "rvm/rvm_gen.h"    /* Generic RVM types and functions. */
#include "rvm/rvm_ext_use_id_list.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* msg ids - NOTE: rsp = req + 1 */
// #define NAN_BM_MESSAGE_OFFSET       BUILD_MESSAGE_OFFSET(NAN_BM_USE_ID)
#define NAN_BM_MESSAGE_OFFSET       BUILD_MESSAGE_OFFSET(NAN_USE_ID)
#define NAN_BM_READ_REQ_MSG        (NAN_BM_MESSAGE_OFFSET+0)
#define NAN_BM_READ_RSP_MSG        (NAN_BM_MESSAGE_OFFSET+1)
#define NAN_BM_WRITE_REQ_MSG       (NAN_BM_MESSAGE_OFFSET+2)
#define NAN_BM_WRITE_RSP_MSG       (NAN_BM_MESSAGE_OFFSET+3)
#define NAN_BM_ERASE_REQ_MSG       (NAN_BM_MESSAGE_OFFSET+4)
#define NAN_BM_ERASE_RSP_MSG       (NAN_BM_MESSAGE_OFFSET+5)
#define NAN_BM_FLUSH_REQ_MSG       (NAN_BM_MESSAGE_OFFSET+6)
#define NAN_BM_FLUSH_RSP_MSG       (NAN_BM_MESSAGE_OFFSET+7)
#define NAN_BM_DMA_MODE_REQ_MSG    (NAN_BM_MESSAGE_OFFSET+8)
#define NAN_BM_DMA_MODE_RSP_MSG    (NAN_BM_MESSAGE_OFFSET+9)
#define NAN_BM_GC_MSG              (NAN_BM_MESSAGE_OFFSET+10) /*garbage collector message*/

 T_RV_RET nan_bm_block_valid (UINT8 chip_select, UINT32 block,
								  BOOL * valid_p);


typedef enum
{ 
  NAN_BM_FORCE_CPU, 
  NAN_BM_FORCE_DMA, 
  NAN_BM_DMA_AUTO 
}T_NAN_BM_DMA_MODE;

typedef struct 
{
  UINT32  nr_logical_blocks;
  UINT32  logical_block_size;
  UINT32  logical_block_spare_size;
} T_NAND_BM_MEDIA_INFO;

/* framework for all nan_bm messages */
  typedef struct
  {
    T_RV_HDR os_hdr;
    T_RV_RET driver_status; // added for ICT merge 
    T_RV_RET result;
    T_RV_RETURN rp;
    union
    {
      struct
      {
        UINT32      first_block;
        UINT32      number_of_blocks;
        UINT32      remainder_length;
        UINT32      *buffer_p;
        UINT32      *spare_buffer_p;
        T_NAND_BM_MEDIA_INFO *media_info;
      } read_write;

      // added for ICT merge; verify the same 
      UINT16 nf_status;
      T_NAN_BM_DMA_MODE dma_mode;
    } body;
  }
  T_NAN_BM_MSG;

#if (CHIPSET != 15)
BOOL Is_nan_tristated();
#endif


/**
 * nand_bm_read
 *
 * Detailled description.
 * This function reads a number of logical blocks including spare data from NAND. 
 * data_buffer_p points to a buffer allocated by the client in which the main area 
 * data will be copied. spare_buffer_p points to a buffer allocated by the client in 
 * which the spare area data will be copied. The read starts from the first logical 
 * block first_block and reads number_of_blocks blocks.  After number_of_blocks 
 * blocks it reads additional remainder_length bytes. The spare buffer data does not 
 * support a re-mainder and shall always be a multiple of NAND_BM_CLIENT_SPARE_SIZE.  
 *
 * The user is free to read only the main area or the spare area. In this case the 
 * unused buffer shall point to NULL.
 *
 * The function returns immediately. Only the return path is verified immediately. 
 * Processing is done asynchronous and the result is returned by means of the return
 * path.
 *
 * @param   first_block       The first logical block number from where the data 
 *							              is requested.
 * @param   number_of_blocks  The number of logical blocks to read. The number may be 
 *                            from one to the last possible block number.
 * @param   remainder_length  The number of bytes to read after the last whole block. 
 *                            A value of zero indicates no remainder. 
 * @param   buffer_p          This is a pointer to the destination buffer to which the main 
 *                            data will be copied. Shall be set to NULL if not used.
 * @param   spare_buffer_p    This is a pointer to the destination buffer to which the 
 *                            spare data shall be copied. Shall be set to NULL if not used.
 * @param   return_path       This structure provides information about the way the 
 *                            driver must react asynchronous (call-back pointer or a 
 *                            return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY, RV_INTERNAL_ERR, RV_MEMORY_ERR
 */
T_RV_RET nand_bm_read  (UINT32      first_block,
                        UINT32      number_of_blocks,
                        UINT32      remainder_length,
                        UINT32      *buffer_p,
                        UINT32      *spare_buffer_p,
                        T_RV_RETURN return_path);

/**
 * nand_bm_write
 *
 * Detailled description.
 * This function writes a number of logical blocks including spare data to NAND. data_buffer_p 
 * points to a buffer allocated by the client from which the main page data will be copied. 
 * spare_buffer_p points to a buffer allocated by the client from which the spare data will 
 * be copied. The read starts from the first logical block first_block and reads number_of_blocks 
 * blocks.  After number_of_blocks blocks it reads additional remainder_length bytes. The 
 * spare buffer data does not support a remainder and shall always be a multiple of 
 * logical_block_spare_size as returned by the nand_bm_get_media_info()function.
 * 
 * The user is free to write only the main area or the spare area. In this case the unused buffer 
 * shall point to NULL. No nand_bm_erase() call is required between two writes to the same 
 * logical page. The NAND_BM will do an implicit erase in this case. Note that in case one 
 * of the areas is set to NULL the existing content of this area will be lost. If the content 
 * needs is to be preserved then the user is re-sponsible for reading it first.
 * 
 * The function returns immediately. Only the return path is verified immediately. 
 * Processing is done asynchronous and the result is returned by means of the return path.
 *
 * @param   first_block       The first logical block number from where the data 
 *							              is requested.
 * @param   number_of_blocks  The number of logical blocks to read. The number may be 
 *                            from one to the last possible block number.
 * @param   remainder_length  The number of bytes to read after the last whole block. 
 *                            A value of zero indicates no remainder. 
 * @param   buffer_p          This is a pointer to the destination buffer to which the main 
 *                            data will be copied. Shall be set to NULL if not used.
 * @param   spare_buffer_p    This is a pointer to the destination buffer to which the 
 *                            spare data shall be copied. Shall be set to NULL if not used.
 * @param   return_path       This structure provides information about the way the 
 *                            driver must react asynchronous (call-back pointer or a 
 *                            return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY, RV_INTERNAL_ERR, RV_MEMORY_ERR
 */
T_RV_RET nand_bm_write (UINT32      first_block,
                        UINT32      number_of_blocks,
                        UINT32      remainder_length,
                        UINT32      *buffer_p,
                        UINT32      *spare_buffer_p,
                        T_RV_RETURN return_path);

/**
 * nand_bm_erase
 *
 * Detailled description.
 * This function erases a number of logical data blocks from the NAND. The erase starts 
 * from the first logical block first_block and continues for number_of_blocks blocks. 
 * After an erase the block data will be 0xFF.
 *
 * The function returns immediately. Only the return path is verified immediately. 
 * Processing is done asynchronous and the result is returned by means of the return
 * path.
 *
 * @param   first_block       The first logical block number from where the data 
 *							              is requested.
 * @param   number_of_blocks  The number of logical blocks to read. The number may be 
 *                            from one to the last possible block number.
 * @param   return_path       This structure provides information about the way the 
 *                            driver must react asynchronous (call-back pointer or a 
 *                            return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY, RV_INTERNAL_ERR, RV_MEMORY_ERR
 */
T_RV_RET nand_bm_erase (UINT32      first_block,
                        UINT32      number_of_blocks,
                        T_RV_RETURN return_path);

/**
 * nand_bm_flush
 *
 * Detailled description.
 * Flushes all cached data inside NAND_BM to the NAND-hardware. After receiving the response 
 * of this function the user is guaranteed that all data is sychronised to the NAND-hardware.
 *
 * The function returns immediately. Only the return path is verified immediately. 
 * Processing is done asynchronous and the result is returned by means of the return
 * path.
 *
 * @param   return_path       This structure provides information about the way the 
 *                            driver must react asynchronous (call-back pointer or a 
 *                            return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY, RV_INTERNAL_ERR, RV_MEMORY_ERR
 */
T_RV_RET nand_bm_flush (T_RV_RETURN return_path);

/**
 * nand_bm_dma_mode
 *
 * Detailled description.
 * This function selects the DMA mode to be used by the entity.
 *
 * The function returns immediately. Only the return path is verified immediately. 
 * Processing is done asynchronous and the result is returned by means of the return
 * path.
 *
 * @param   dma_mode          Indicates whether to use DMA or let the CPU handle the copying. 
 *                            Possible values: NAND_BM_CPU (default), NAND_BM_FORCE_DMA,
 *                            NAND_BM_DMA_AUTO
 * @param   return_path       This structure provides information about the way the 
 *                            driver must react asynchronous (call-back pointer or a 
 *                            return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY, RV_INTERNAL_ERR, RV_MEMORY_ERR
 */
T_RV_RET nand_bm_dma_mode (T_NAN_BM_DMA_MODE dma_mode,
                           T_RV_RETURN    return_path);
/**
 * nand_bm_get_media_info
 *
 * Detailled description.
 * This function returns NAND-media info. Information returned in T_NAND_BM_MEDIA_INFO.
 *
 * @param   media_info        Pointer to T_NAND_BM_MEDIA_INFO structure which is allocated 
 *                            by the user.
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY, RV_INTERNAL_ERR, RV_MEMORY_ERR
 */
T_RV_RET nand_bm_get_media_info (T_NAND_BM_MEDIA_INFO *media_info);



/**
 * nand_bm_get_sw_version
 *
 * Detailled description.
 * This function returns the version of this service entity.
 * 
 * @return    [0-15]	BUILD	Build number
 *            [16-23]	MINOR	Minor version number
 *            [24-31]	MAJOR	Major version number
 */
UINT32 nand_bm_get_sw_version (void);

#endif /*__NAN_BM_API_H_*/

