/**
 * @file	rfsfat_blk.h
 *
 * header file for rfsfat_blk.c
 *
 * @author	Anton van Breemen
 * @version 0.1 
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	02/11/2004	Anton van Breemen		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */


#ifndef RFSFAT_BLK_H
#define RFSFAT_BLK_H


/******************************************************************************
 *     Defines and enums                                                      *
 ******************************************************************************/

/******************************************************************************
 *     Macros                                                                 *
 ******************************************************************************/

/******************************************************************************
 *     Typedefs                                                               *
 ******************************************************************************/
typedef struct
{
  UINT32 last_read_blk;			//last block that was read and is still kept in the 
  //read cache  
  UINT32* read_cache;			//pointer to the actual read cache
  UINT32 last_write_blk;		//last block that was accessed for write without 
  //actually writing it 
  UINT32* write_cache;			//pointer to the actual write cache
  UINT8 last_mpt_idx_rd;			//mountpoint index belonging with last read blk
  UINT8 last_mpt_idx_wr;	//mountpoint index belonging with last written blk
} BLK_DATA_STRUCT;
/******************************************************************************
 *     Variable Declaration                                                   *
 ******************************************************************************/

/******************************************************************************
 *     Function prototypes                                                    *
 ******************************************************************************/
UINT8 rfsfat_blk_read (MMA_RW_STRUCT * read_info, UINT8 mpt_idx);
UINT8 rfsfat_blk_write (MMA_RW_STRUCT * write_info, UINT8 mpt_idx);
UINT8    flush_write_blk (void);
UINT8    flush_write_blk_kill_cache(void); 
void RFSFAT_invalidate_buffer_cache(UINT8 mptidx);
void     kill_gbi_readcash (void);

void gbi_callback_function(void * resp_msg_p);



#endif //RFSFAT_BLK_H

