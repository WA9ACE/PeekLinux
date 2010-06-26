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
#define RFSFAT_MAX_BLK_SIZE       4096
#define PARTITION_START_OFFSET    0x00	//the offset of the startpoint of a
#define BLK_EMPTY                 0	//this is used to indicate an empty
#define BLK_NULL				  0xFFFFFFFF
#define RFSFAT_INVALID_MPT   	  0xFF 



/******************************************************************************
 *     Typedefs                                                               *
 ******************************************************************************/
typedef struct
{
  UINT32 last_read_blk;			//last block that was read and is still kept in the 
  UINT32 last_write_blk;		//last block that was accessed for write without 
  //actually writing it 

    //read cache  
  UINT8 read_cache[RFSFAT_MAX_BLK_SIZE];			//pointer to the actual read cache
  UINT8 write_cache[RFSFAT_MAX_BLK_SIZE];			//pointer to the actual write cache

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

