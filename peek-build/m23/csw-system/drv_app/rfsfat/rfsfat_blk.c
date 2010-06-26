/**
 * @file  rfsfat_blk.c
 *
 * In this file the block manager is created. The block manager
 * ensures that bytes oriented read/write actions (offset,nr_bytes,pBuf)
 * are coverted to block oriented actions, needed to communicate with the GBI
 * module
 *
 * @author  Anton van Breemen
 * @version 0.1 
 */

/*
 * History:
 *
 *  Date (mm/dd/yyyy)     Author              Modification
 *  -------------------------------------------------------------------
 *  02/11/2004            Anton van Breemen   Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

/******************************************************************************
 *     Includes                                                               *
 ******************************************************************************/

#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "gbi/gbi_api.h"
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"

#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_api.h"
#include "rfsfat/rfsfat_mma.h"
#include "rfsfat/rfsfat_blk.h"
#include "rfsfat/rfsfat_env.h"
#include "rfsfat/rfsfat_fam.h"
//#include "rfsfat/rfsfat_pei.h"


/******************************************************************************
 *     Typedefs                                                               *
 *****************************************************************************/

/**
 * @name BLK_READ_CASES
 *
 * This type definition holds all possible cases that the read block manager
 * can encounter during CONFIGURED operational use. 
 *  // possible situations! 
 *   //
 *   //1) readblk < 1 blk (spread over 2 blks)
 *   //2) readblk < 1 blk (in 1 blk)
 *   //3) readblk = 1 blk (spread over 2 blks)
 *   //4) readblk = 1 blk (exactly 1 blk)
 *   //5) readblk > 1 blk (fragmented 1st blk next n blks are whole)
 *   //6) readblk > 1 blk (n blks are all exactly whole blks)
 *   //7) readblk > 1 blk (first n blks are exactly whole blks last block is fragmented)
 *   //8) readblk > 1 blk (fragmented 1st blk, next n blks are whole, last block is fragmented)
 *   //
 *   //strategy is to cache fragmented last blocks as a whole blk:
 *     //situation 2 require only 1 read action 
 *       //read data to cache buffer and copy to client buffer 
 *       //(leave cache intact)
 *     //situation 1 & 3 require 2 read actions:
 *       //1)read 1st blk to cache -> copy data to client buffer
 *       //2)read 2nd blk to cache -> copy data to client buffer
 *       //(leave cache intact)
 *     //situation 5 requires 2 read actions
 *       //1)read 1st blk to cache -> copy data to client buffer
 *       //2)read remaining blks directly to client buffer (offset with step1)
 *       //(free cache)
 *     //situation 4 & 6 requires 1 read action
 *       //1)read all blks directly to client buffer
 *       //(no cache)
 *     //situation 7 requires 2 read actions
 *       //1)read all but the last block directly to client buffer
 *       //2)read last blk to cache -> copy data to client buffer
 *       //(leave cache intact)
 *     //situation 8  requires 3 read actions
 *       //1)read 1st blk to cache -> copy data to client buffer
 *       //2)read all remaining but the last block directly to client buffer
 *       //3)read last blk to cache -> copy data to client buffer
 *       //(leave cache intact)
 */
/*@{*/
typedef enum
{
  BLK_READ_CASE_1 = 0,
  BLK_READ_CASE_2,
  BLK_READ_CASE_3,
  BLK_READ_CASE_4,
  BLK_READ_CASE_5,
  BLK_READ_CASE_6,
  BLK_READ_CASE_7,
  BLK_READ_CASE_8
} BLK_READ_CASES;
/*@}*/

/**
 * @name BLK_WRITE_CASES
 *
 * This type definition holds all possible cases that the write block manager
 * can encounter during CONFIGURED operational use. 
 *  // possible situations! 
 *   //
 *   //1) writeblk < 1 blk (spread over 2 blks)
 *   //2) writeblk < 1 blk (in 1 blk)
 *   //3) writeblk = 1 blk (spread over 2 blks)
 *   //4) writeblk = 1 blk (exactly 1 blk)
 *   //5) writeblk > 1 blk (fragmented 1st blk next n blks are whole)
 *   //6) writeblk > 1 blk (n blks are all exactly whole blks)
 *   //7) writeblk > 1 blk (first n blks are exactly whole blks last block is fragmented)
 *   //8) writeblk > 1 blk (fragmented 1st blk, next n blks are whole, last block is fragmented)
 *   //
 *   //strategy is to cache fragmented last blocks as a whole blk:
 *     //situation 1 & 3 
 *       //1)read 1st blk to cache 
 *       //2)copy content over read data
 *       //3)write block 1 to GBI
 *       //4)copy data to write into cache (don't write yet)
 *       //(leave cache intact)
 *     //situation 2 
 *       //1)(if not from offset 0 ->read 1st blk to cache )
 *       //2)copy content over read data or copy it in empty cache
 *       //(leave cache intact)
 *     //situation 5 requires 2 read actions
 *       //1)read 1st blk to cache 
 *       //2)copy content over read data
 *       //3)write block 1 to GBI (free cache)
 *       //4)copy remaining blocks directly to GBI
 *     //situation 4 & 6 requires 1 read action
 *       //1)write all blks directly to GBI (no cache)
 *     //situation 7 requires 2 read actions
 *       //1)write all blks directly to GBI except for the last block
 *       //2)write last blk to cache 
 *       //(leave cache intact)
 *     //situation 8  requires 3 read actions
 *       //1)read 1st blk to cache 
 *       //2)copy content over read data
 *       //3)write block 1 to GBI (free cache)
 *       //4)copy remaining blocks directly to GBI
 *       //5)write last blk to cache
 *       //(leave cache intact)
 */
/*@{*/
typedef enum
{
  BLK_WRITE_CASE_1 = 0,
  BLK_WRITE_CASE_2,
  BLK_WRITE_CASE_3,
  BLK_WRITE_CASE_4,
  BLK_WRITE_CASE_5,
  BLK_WRITE_CASE_6,
  BLK_WRITE_CASE_7,
  BLK_WRITE_CASE_8
} BLK_WRITE_CASES;
/*@}*/

/******************************************************************************
 *      Module specific Defines and enums                                     *
 ******************************************************************************/

										
#define BLK_FIRST                 0	//first block
#define STND_REMNDR               0	//standard remainder
#define STND_OFFSET               0	//standard offset
#define STND_NR_BLK               1	//standard nr of blocks
#define READ_CACHE  (&(rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.read_cache[0]))
#define LAST_READ   rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.last_read_blk
#define WRITE_CACHE (&(rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.write_cache[0]))
#define LAST_WRITE  rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.last_write_blk
#define LAST_MPT_IDX_RD    rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.last_mpt_idx_rd
#define LAST_MPT_IDX_WR    rfsfat_env_ctrl_blk_p->ffs_globals.blk_data.last_mpt_idx_wr

#define READ_CACHE_VALID(mp,blk) ((LAST_READ == blk) && (LAST_MPT_IDX_RD == mp))

#define WRITE_CACHE_VALID(mp,blk) ((LAST_WRITE == blk) && (LAST_MPT_IDX_WR == mp))


#define KILL_CACHE         do {LAST_READ = BLK_NULL; LAST_MPT_IDX_RD=RFSFAT_INVALID_MPT; } while(0)
#define KILL_WRITE_CACHE   do {LAST_WRITE = BLK_NULL; LAST_MPT_IDX_WR=RFSFAT_INVALID_MPT; } while(0)

/******************************************************************************
*      Function Declaration                                                   *
******************************************************************************/

UINT8 copy_cache_to_buf (MMA_RW_STRUCT * buf_info_p, UINT8 * cache_p);
UINT8 create_buf_read_data (UINT8 mpt_idx, UINT32 start_blk);
UINT8 read_blk (MMA_RW_STRUCT * read_info, UINT8 mpt_idx,
				   BLK_READ_CASES read_case);
UINT8 read_from_gbi (UINT32 start_blk, UINT32 nr_blk, UINT16 remainder,
						UINT32 * buf, UINT8 mpt_idx);
UINT8 first_fragmented (MMA_RW_STRUCT * read_info, UINT32 rb,
						   UINT8 mpt_idx, MMA_RW_STRUCT * read_support);
UINT8 read_fragmented (MMA_RW_STRUCT * read_support, UINT32 start_blk,
						  UINT8 mpt_idx);
UINT8 determine_case (MMA_RW_STRUCT * rw_info, BOOL read_write,
						 UINT8 mpt_idx);
UINT8 write_blk (MMA_RW_STRUCT * write_info, UINT8 mpt_idx,
					BLK_WRITE_CASES write_case);
UINT8 copy_buf_to_cache (MMA_RW_STRUCT * buf_p, UINT16 offset,
							UINT32 nr_byte);
//UINT8 fill_cache_orig_data (MMA_RW_STRUCT * read_data, UINT8 mpt_idx);
UINT8 create_cache_write_data (UINT32 start_blk, UINT8 mpt_idx);
UINT8 gbi_blk_write (UINT8 mpt_idx, UINT32 start_blk, UINT32 nr_blk,
						UINT32 * clnt_buf_p);
UINT8 read_fragmented_write (UINT32 start_blk, UINT8 mpt_idx,
								MMA_RW_STRUCT * support);
UINT8 first_fragmented_write (MMA_RW_STRUCT * write_info, UINT8 mpt_idx,
								 MMA_RW_STRUCT * support);
UINT8 random_block_write (MMA_RW_STRUCT * write_info, UINT8 mpt_idx,
							 MMA_RW_STRUCT * support);
UINT8 copy_buf2cache (MMA_RW_STRUCT * support, UINT16 offset,
						 UINT32 nr_byte);
UINT8 wc_newer_rc (UINT32 rb, UINT8 mpt_idx);
UINT8 safe_read (UINT32 rb, UINT8 mpt_idx, MMA_RW_STRUCT * read_support);
UINT8 intermediate_flush (UINT32 startblk, UINT32 bufsize, UINT8 mpt_idx);
UINT8 gbi_blk_erase (UINT8 mpt_idx, UINT32 start_blk, UINT32 nr_blk);

/***************************************************************************
*
*   BLOCK READ FUNCTIONS
*
****************************************************************************/

/**
* This function is called by the media manager. It is called to read data from 
* the media. The mma module has determined where the data is located on the 
* media, now the block manager will have to determine in which logical block
* this data is located and will have to read that block or those blocks and 
* read the data from it. The requested data will be stored in the provided buffer
*
* The read action is a-synchrone, the read will be issued, but the actual 
* response (a read block) will be a-synchroneously delivered. Therefore the 2nd 
* mailbox principle is introduced that provides a blocking solution ensuring 
* sequential behaviour. (a more sophisticated solution will have to be integrated
* to provide the same functionallity in a non blocking way.)
*
* When the rfsfat is not yet configured, the block manager cannot see if the 
* block to read is within the partition borders. Therefore it is only possible to
* read block 0 (bootsector info) while not configured.
* 
* @param             MMA_RW_STRUCT *read_info contains byte offset, length 
*                                             and a pointer to databuffer.
*
* @return            RFSFAT_OK              when executed successfully
*                    RFSFAT_INVALID_PARAM   when input parameter is not valid
*/
UINT8
rfsfat_blk_read (MMA_RW_STRUCT * read_info, UINT8 mpt_idx)
{
 
  //check input paramters
  RFSFAT_ASSERT_LEAVE ((read_info != NULL), return RFSFAT_INVALID_PARAM);


  if (MPT_TABLE[mpt_idx].blk_size != BLK_EMPTY)
	{

	  if (determine_case (read_info, TRUE, mpt_idx) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: determine_case failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	}
  else
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: cannot read blocks with size 0",
						 RV_TRACE_LEVEL_ERROR);
	  return RFSFAT_INTERNAL_ERR;
	}

  return RFSFAT_OK;
}

/**
* This function is used to copy data from one buffer to another!
* the boundary checks etc, are to be performed by the caller.
* 
* @param             MMA_RW_STRUCT buf_info_p
* @param             UINT32* cache_p
*
* @return            RFSFAT_OK  is successfully executed
*/
UINT8
copy_cache_to_buf (MMA_RW_STRUCT * buf_info_p, UINT8 * cache_p)
{
 
  memcpy(buf_info_p->pucBuffer,cache_p+buf_info_p->tOffset,buf_info_p->tNrBytes);
 
  return RFSFAT_OK;
}

/**
* 
* Called to reserve memory for the read cache and to call the GBI
* with a read request.
* 
* @param             UINT32 blk_sz
* @param             UINT8 mpt_idx
* @param             UINT32 start_blk
* @param             UINT32 nr_blk
* @param             UINT16 remainder
*
* @return            RFSFAT_MEMORY_ERR
*                    RFSFAT_INTERNAL_ERR
*                    RFSFAT_OK
*/
UINT8
create_buf_read_data (UINT8 mpt_idx, UINT32 start_blk)
	{

  if (read_from_gbi (start_blk,STND_NR_BLK, STND_REMNDR, ((UINT32*)READ_CACHE), mpt_idx) !=
	  RFSFAT_OK)
	{

	  RFSFAT_SEND_TRACE ("RFSFAT: read_from_gbi failed",
						 RV_TRACE_LEVEL_ERROR);
	  KILL_CACHE;
	  return RFSFAT_INTERNAL_ERR;
	}
  LAST_READ = start_blk;
  LAST_MPT_IDX_RD = mpt_idx;
  return RFSFAT_OK;
}

/**
* 
* This function is called to perform a block read action. First it determines for 
* which case a block read has to be executed. always check if the requested buffer
* is still in the cache.
*
* @param             MMA_RW_STRUCT *read_info
* @param             UINT8 mpt_idx
* @param             BLK_READ_CASES read_case
*
* @return            RFSFAT_OK            successfully executed
*                    RSFFAT_INVALID_PARAM invalid input parameter
*                    RFSFAT_INTERNAL_ERR  not able to perform requested funcitonality
*                    RFSFAT_MEMORY_ERR    not enough memory available (for cache)  
*/
UINT8
read_blk (MMA_RW_STRUCT * read_info, UINT8 mpt_idx, BLK_READ_CASES read_case)
{
  UINT32 start_blk;
  MMA_RW_STRUCT read_support={0,0,NULL};
  UINT8 *client_buf_p;
  UINT32 nr_blk = 0;

  //check input paramters
  RFSFAT_ASSERT_LEAVE ((read_info != NULL), return RFSFAT_INVALID_PARAM);

  switch (read_case)
	{
	case BLK_READ_CASE_1:		//deliberate FALLTROUGH
	case BLK_READ_CASE_3:
	  // readblk < 1 blk (spread over 2 blks)
	  // readblk = 1 blk (spread over 2 blks)
	  //determine start block
	  start_blk = (read_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
	  if (first_fragmented (read_info, start_blk, mpt_idx, &read_support) !=
		  RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: CASE 1/3 first_fragmented failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
		  start_blk++;
		  read_support.pucBuffer = (read_info->pucBuffer + read_support.tNrBytes);
		  read_support.tOffset = STND_OFFSET;
		  read_support.tNrBytes = (read_info->tNrBytes - read_support.tNrBytes);


    /* Update block may be in WRITE_CACHE , include WRITE_CACHE while reading */
    
   if (safe_read (start_blk, mpt_idx, &read_support) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: safe read in case 2 failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}

	  RFSFAT_SEND_TRACE ("RFSFAT: READ CASE 1/3 succes",
						 RV_TRACE_LEVEL_DEBUG_LOW);
	  break;
	case BLK_READ_CASE_2:
	  // readblk < 1 blk (in 1 blk)
	  start_blk = (read_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
	  read_support.pucBuffer = read_info->pucBuffer;
	  read_support.tOffset =(read_info->tOffset % MPT_TABLE[mpt_idx].blk_size);
	  read_support.tNrBytes = read_info->tNrBytes;

	  if (safe_read (start_blk, mpt_idx, &read_support) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: safe read in case 2 failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  RFSFAT_SEND_TRACE("RFSFAT: READ CASE 2 succes",  RV_TRACE_LEVEL_DEBUG_LOW);  
	  break;
	case BLK_READ_CASE_4:
	  // readblk = 1 blk (exactly 1 blk)
	  start_blk = (read_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
	  read_support.pucBuffer = read_info->pucBuffer;
	  read_support.tOffset = STND_OFFSET;
	  read_support.tNrBytes = read_info->tNrBytes;

  	  /* Update block may be in WRITE_CACHE , include WRITE_CACHE while reading */
      if (safe_read (start_blk, mpt_idx, &read_support) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: safe read in case 4 failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	RFSFAT_SEND_TRACE("RFSFAT: READ CASE 4 succes",  RV_TRACE_LEVEL_DEBUG_LOW);  
	  break;
	case BLK_READ_CASE_5:
	  // readblk > 1 blk (fragmented 1st blk next n blks are whole)
	  start_blk = (read_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
	  //have we recently read this block into cache? 
	  if (first_fragmented (read_info, start_blk, mpt_idx, &read_support) !=
		  RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: CASE 5 first_fragmented failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  start_blk++;
	  nr_blk = ((read_info->tNrBytes - read_support.tNrBytes) /
				MPT_TABLE[mpt_idx].blk_size);
	  //see if an associated block must be flushed first
	  if (intermediate_flush (start_blk, nr_blk, mpt_idx) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: CASE 5 intermediate flush failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  client_buf_p = read_info->pucBuffer + read_support.tNrBytes;
	  //read directly to client buffer
	  if (read_from_gbi
		  (start_blk, nr_blk, STND_REMNDR, (UINT32 *) client_buf_p,
		   mpt_idx) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: CASE 5 read_from_gbi failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  else
		{
		  KILL_CACHE;
		}
	  RFSFAT_SEND_TRACE ("RFSFAT: READ CASE 5 succes",
						 RV_TRACE_LEVEL_DEBUG_LOW);
	  break;
	case BLK_READ_CASE_6:
	  // readblk > 1 blk (n blks are all exactly whole blks)
	  start_blk = (read_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
	  nr_blk = (read_info->tNrBytes / MPT_TABLE[mpt_idx].blk_size);
	  //see if an associated block must be flushed first
	  if (intermediate_flush (start_blk, nr_blk, mpt_idx) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: CASE 6 intermediate flush failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  //read directly to client buffer
	  if (read_from_gbi
		  (start_blk, nr_blk, STND_REMNDR, (UINT32 *) read_info->pucBuffer,
		   mpt_idx) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: CASE 6 read_from_gbi failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  else
		{
		  KILL_CACHE;
		}
	  RFSFAT_SEND_TRACE ("RFSFAT: READ CASE 6 succes",
						 RV_TRACE_LEVEL_DEBUG_LOW);
	  break;
	case BLK_READ_CASE_7:
	  // readblk > 1 blk (first n blks are exactly whole blks last block is fragmented)

	  start_blk = (read_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
	  nr_blk = (read_info->tNrBytes / MPT_TABLE[mpt_idx].blk_size);
	  //see if an associated block must be flushed first
	  if (intermediate_flush (start_blk, nr_blk, mpt_idx) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: CASE 6 intermediate flush failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  //read directly to client buffer
	  if (read_from_gbi
		  (start_blk, nr_blk, STND_REMNDR, (UINT32 *) read_info->pucBuffer,
		   mpt_idx) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: CASE 7 read_from_gbi failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  start_blk = start_blk + nr_blk;
	  read_support.pucBuffer =
		(read_info->pucBuffer + (nr_blk * MPT_TABLE[mpt_idx].blk_size));
	  read_support.tOffset = STND_OFFSET;
	  read_support.tNrBytes =
		(read_info->tNrBytes % MPT_TABLE[mpt_idx].blk_size);

   /* Update block may be in WRITE_CACHE , include WRITE_CACHE while reading */
   if (safe_read (start_blk, mpt_idx, &read_support) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: safe read in case 7 failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}

	  RFSFAT_SEND_TRACE ("RFSFAT: READ CASE 7 succes",	 RV_TRACE_LEVEL_DEBUG_LOW);
	  break;
	case BLK_READ_CASE_8:
	  // readblk > 1 blk (fragmented 1st blk, next n blks are whole, last block is fragmented)
	  start_blk = (read_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
	  if (first_fragmented (read_info, start_blk, mpt_idx, &read_support) !=
		  RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: CASE 8 first_fragmented failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  else
		{
		  start_blk++;
		  nr_blk = ((read_info->tNrBytes - read_support.tNrBytes) /
					MPT_TABLE[mpt_idx].blk_size);
		  //see if an associated block must be flushed first
		  if (intermediate_flush (start_blk, nr_blk, mpt_idx) != RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE ("RFSFAT: CASE 8 intermediate flush failed",
								 RV_TRACE_LEVEL_ERROR);
			  return RFSFAT_INTERNAL_ERR;
			}
		  client_buf_p = (read_info->pucBuffer + read_support.tNrBytes);
		  //read directly to client buffer
      if (nr_blk > 0)
      {
        /* BUG: read_info->pucBuffer should not be passed a buffer 
		        client_bug_p should be passed as a paramter to this function */
        if (read_from_gbi(start_blk, nr_blk, STND_REMNDR,
			   (UINT32 *) client_buf_p, mpt_idx) != RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE ("RFSFAT: CASE 8 read_from_gbi failed",
								 RV_TRACE_LEVEL_ERROR);
			  return RFSFAT_INTERNAL_ERR;
			}
      

			start_blk += nr_blk;
			  read_support.pucBuffer =
				(client_buf_p + (nr_blk * MPT_TABLE[mpt_idx].blk_size));
			  read_support.tOffset = STND_OFFSET;
			  read_support.tNrBytes = (read_info->tNrBytes -
									   ((nr_blk *
										 MPT_TABLE[mpt_idx].blk_size) +
										read_support.tNrBytes));
        }  /* if(nr_blk > 0) */
	  else
	  	{
	  	   /* just read the remaing bytes */
           read_support.pucBuffer = client_buf_p;
		   read_support.tOffset = STND_OFFSET;
		   read_support.tNrBytes = read_info->tNrBytes-read_support.tNrBytes;
		
	  	}

  /* Update block may be in WRITE_CACHE , include WRITE_CACHE while reading */
   if (safe_read (start_blk, mpt_idx, &read_support) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: safe read in case 8 failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}

		  
		
		}   /*else */
	  RFSFAT_SEND_TRACE ("RFSFAT: READ CASE 8 succes",	 RV_TRACE_LEVEL_DEBUG_LOW);
	  break;
	default:
	  RFSFAT_SEND_TRACE
		("RFSFAT: a valid read_case should have been provided",
		 RV_TRACE_LEVEL_ERROR);
	  return RFSFAT_INTERNAL_ERR;
	}							//end switch

  return RFSFAT_OK;
}

/**
* 
* This function performs a read to the GBI entity providing a buffer
* into which the data is copied
* 
* @param             UINT32 start_blk
* @param             UINT32 nr_blk
* @param             UINT16 remainder
* @param             UINT8* buf
* @param             UINT8 mpt_idx
* @param             
*
* @return            RFSFAT_OK            successfully executed
*                    RFSFAT_INTERNAL_ERR  not successfully read from GBI
*                    
*/
UINT8
read_from_gbi (UINT32 start_blk, UINT32 nr_blk, UINT16 remainder,
			   UINT32 * buf, UINT8 mpt_idx)
{
  T_GBI_READ_RSP_MSG *msg_p;	//pointer to GBI response message
  UINT8 ret_val = RFSFAT_OK;

 RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: Read from GBI ..start \n");
  /* protective fixes */
  /* AT any point of time, we should not call this function with 
     nr_blk as zero 
   */
  if((buf == NULL) || (nr_blk == 0))
  	{
  	  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: gbi_blk_write  Invalid parameters \n\r");
	  return RFSFAT_INTERNAL_ERR;
  	}
  //start reading from GBI (Generic Block Interface)
  if (gbi_read (MPT_TABLE[mpt_idx].media_nbr,
				MPT_TABLE[mpt_idx].partition_nbr,
				start_blk,
				nr_blk,
				remainder,
				buf, rfsfat_env_ctrl_blk_p->return_path_2nd_queue) != RV_OK)
	{

	  RFSFAT_SEND_TRACE ("RFSFAT: GBI read failed", RV_TRACE_LEVEL_ERROR);
	  return RFSFAT_INTERNAL_ERR;
	}
  // WAIT for GBI response (blocking)
 msg_p = (T_GBI_READ_RSP_MSG *) return_queue_get_msg ();
 /* In case of NULL return ERROR */
 if(msg_p==NULL)
 	    return RFSFAT_INTERNAL_ERR;
  
  switch (msg_p->result)
	{
	case RV_OK:
	  ret_val = RFSFAT_OK;
	  break;
	case RV_INVALID_PARAMETER:	//delliberate fallthrough
	case RV_NOT_READY:			//delliberate fallthrough
	case RV_MEMORY_ERR:		//delliberate fallthrough
	case RV_INTERNAL_ERR:		//delliberate fallthrough
	  RFSFAT_SEND_TRACE ("RFSFAT: GBI read failed error response! ",
						 RV_TRACE_LEVEL_ERROR);
	  ret_val = RFSFAT_INTERNAL_ERR;
	  break;
	default:
	  RFSFAT_SEND_TRACE ("RFSFAT: undefined response msg from GBI! ",
						 RV_TRACE_LEVEL_ERROR);
	  ret_val = RFSFAT_INTERNAL_ERR;
	  break;
	}
  rvf_free_msg ((T_RVF_MSG *) msg_p);
  msg_p=NULL;
 RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: Read from GBI ..end \n");
  return ret_val;
}

/**
* this function reads the first "fragmented" block either from the cache
* to the client buffer or from the GBI
* 
* @param             MMA_RW_STRUCT *read_info
* @param             UINT32 rb
* @param             UINT8 mpt_idx
* @param             MMA_RW_STRUCT *read_support
*
* @return            RFSFAT_OK
*                    RFSFAT_INTERNAL_ERR 
*/
UINT8
first_fragmented (MMA_RW_STRUCT * read_info, UINT32 rb,
				  UINT8 mpt_idx, MMA_RW_STRUCT * read_support)
{
  read_support->pucBuffer = read_info->pucBuffer;
  read_support->tOffset = (read_info->tOffset % MPT_TABLE[mpt_idx].blk_size);
  read_support->tNrBytes =
	(MPT_TABLE[mpt_idx].blk_size - read_support->tOffset);

  return safe_read (rb, mpt_idx, read_support);
}

/**
* 
* This function reads the last block. this fragmented last block
* is read from the GBI and placed in the cache from where it is copied 
* to the client buffer. The cache is stored.
* 
* @param             MMA_RW_STRUCT *read_support
* @param             UINT32 start_blk
* @param             UINT8 mpt_idx
*
* @return            
*/
UINT8
read_fragmented (MMA_RW_STRUCT * read_support, UINT32 start_blk,
				 UINT8 mpt_idx)
{

  //free previously aclaimed buffer
  KILL_CACHE;
  if (create_buf_read_data ( mpt_idx, start_blk) != RFSFAT_OK)
	{

	  RFSFAT_SEND_TRACE ("RFSFAT: CASE  create_buf_read_data failed",
						 RV_TRACE_LEVEL_ERROR);
	  KILL_CACHE;
	  return RFSFAT_INTERNAL_ERR;
	}
  else if (copy_cache_to_buf (read_support, READ_CACHE) != RFSFAT_OK)
	{

	  RFSFAT_SEND_TRACE ("RFSFAT: CASE  copy read_cache to client buf failed",
						 RV_TRACE_LEVEL_ERROR);
	  KILL_CACHE;
	  return RFSFAT_INTERNAL_ERR;
	}
  return RFSFAT_OK;
}

/**
* This function frees the GBI read cash.
* The buffer is freed, various administartion isreset and status set to free.
* This function allows the usage of the local KILL_CACHE macro.
*
* @return       none
*
*/
void
kill_gbi_readcash (void)
{
  KILL_CACHE;
}
/***************************************************************************
*
*   BLOCK WRITE FUNCTIONS
*
****************************************************************************/

/**
* This function copies the remaining data from the write cache to the medium
* After this action the cache is cleared.
* When the block is not successfully written the cache is killed anyway.
*
* @return       FFS_NO_ERRORS           successfully executed
*               GEN_INTERNAL_ERROR      error occured during execution -> abort    
*/
UINT8
flush_write_blk (void)
{
  if((LAST_MPT_IDX_WR != RFSFAT_INVALID_MPT) && (LAST_WRITE != BLK_NULL))
  	{
  
	 if ((LAST_WRITE == LAST_READ ) && (LAST_MPT_IDX_WR == LAST_MPT_IDX_RD) )
	   {

			  KILL_CACHE;
		}

	  if (gbi_blk_write (LAST_MPT_IDX_WR, LAST_WRITE, STND_NR_BLK, ((UINT32*)(WRITE_CACHE)))
		  != RFSFAT_OK)
		{

		  KILL_WRITE_CACHE;
		  RFSFAT_SEND_TRACE ("RFSFAT: FLUSH gbi_write failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	}   /* valid mount point */
  
   KILL_WRITE_CACHE;				//just written -> not needed anymore
	  
  return FFS_NO_ERRORS;
}


/* flush the write blk and kill the cache */
UINT8    flush_write_blk_kill_cache()
{
  UINT8 retVal;

  retVal = flush_write_blk();

  KILL_CACHE;

  return retVal;
}



/**
* This function is called by the media manager. It is called to write data to
* the media. The mma module has determined where the data is located on the 
* media, now the block manager will have to determine if it can write directly 
* to that block (whole block is invlolved) or if it has to read the block first
* and to copy the the data to write into that read data and write it back as a 
* whole block again.
*
* The Write action is a-synchrone, the write will be issued, but the actual 
* response (a write okay resp) will be a-synchroneously delivered. Therefore the 
* 2nd mailbox principle is introduced that provides a blocking solution ensuring 
* sequential behaviour. (a more sophisticated solution will have to be integrated
* to provide the same funcitonality in a non blocking way.)
* 
* When the rfsfat is not yet configured, the block manager cannot see if the 
* block to write is within the partition borders. Therefore it is only possible to
* write block 0 (bootsector info) while not configured.
*
* @param             MMA_RW_STRUCT *pstWrite  contains byte offset, length 
*                                             and a pointer to databuffer.
*
* @return            RFSFAT_OK              when executed successfully
*                    RFSFAT_INVALID_PARAM   when input parameter is not valid
*                    RFSFAT_INTERNAL_ERR    when problem occured during execution 
*/
UINT8
rfsfat_blk_write (MMA_RW_STRUCT * write_info, UINT8 mpt_idx)
{
  //RFSFAT_SEND_TRACE("RFSFAT (blk): rfsfat_blk_write is entered",
  //  RV_TRACE_LEVEL_DEBUG_LOW);  

  //check input paramters
  RFSFAT_ASSERT_LEAVE ((write_info != NULL), return RFSFAT_INVALID_PARAM);


  if (MPT_TABLE[mpt_idx].blk_size != BLK_EMPTY)
	{

	  if (determine_case (write_info, FALSE, mpt_idx) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: determine_case failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	}
  else
	{
	  RFSFAT_SEND_TRACE ("RFSFAT: can't write blocks of size 0!",
						 RV_TRACE_LEVEL_ERROR);
	  return RFSFAT_INTERNAL_ERR;
	}

  //RFSFAT_SEND_TRACE("RFSFAT (blk): exit rfsfat_blk_write",
  //  RV_TRACE_LEVEL_DEBUG_LOW);  
  return RFSFAT_OK;
}

/**
* 
* This function is called to perform a block write action. First it determines for 
* which case a block write has to be executed. always check if the requested buffer
* is still in the cache.
*
* @param             MMA_RW_STRUCT *write_info
* @param             UINT8 mpt_idx
* @param             BLK_WRITE_CASES write_case
*
* @return            RFSFAT_OK            successfully executed
*                    RSFFAT_INVALID_PARAM invalid input parameter
*                    RFSFAT_INTERNAL_ERR  not able to perform requested funcitonality
*                    RFSFAT_MEMORY_ERR    not enough memory available (for cache)  
*/
UINT8
write_blk (MMA_RW_STRUCT * write_info, UINT8 mpt_idx,
		   BLK_WRITE_CASES write_case)
{
  MMA_RW_STRUCT support={0,0,NULL};   /* Init values */
  UINT32 start_blk;
  UINT32 nr_blk;
  UINT16 first_nr_bytes;
  UINT32 i; 

  //check input paramters
  RFSFAT_ASSERT_LEAVE ((write_info != NULL), return RFSFAT_INVALID_PARAM);

  switch (write_case)
	{
	case BLK_WRITE_CASE_1:		//deliberate FALLTROUGH
	  //1) writeblk < 1 blk (spread over 2 blks)
	case BLK_WRITE_CASE_3:
	  //3) writeblk = 1 blk (spread over 2 blks) 
	  if (first_fragmented_write (write_info, mpt_idx, &support) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: first_fragmented_write failed");
		  return RFSFAT_INTERNAL_ERR;
		}
		  start_blk = (write_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
		  //already the next blk
		  start_blk++;
		  support.pucBuffer = (write_info->pucBuffer + support.tNrBytes);
		  //the remaining number of bytes to write
		  support.tNrBytes = (write_info->tNrBytes - support.tNrBytes);
		  if (read_fragmented_write (start_blk, mpt_idx, &support) !=
			  RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE_ERROR
				("RFSFAT:BLK_WRITE_CASE_1/3 read_fragmented_write failed");
			  return RFSFAT_INTERNAL_ERR;
			}
		
	  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: BLK_WRITE_CASE_1/3 successfully executed");
	  break;
	case BLK_WRITE_CASE_2:
	  //2) writeblk < 1 blk (in 1 blk)
	  if (random_block_write (write_info, mpt_idx, &support) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE_ERROR
			("RFSFAT: BLK_WRITE_CASE_2 random_block_write failed");
		  return RFSFAT_INTERNAL_ERR;
		}
	  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: BLK_WRITE_CASE_2 successfully executed");
	  break;
	case BLK_WRITE_CASE_4:
	  //4) writeblk = 1 blk (exactly 1 blk)
	  start_blk = (write_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
if(mpt_idx == LAST_MPT_IDX_WR)
{
	  /* check the write cache */
	  if(start_blk == LAST_WRITE)
	  	{
	  	     KILL_WRITE_CACHE;
	  	}

	  /* check the read cache */
	  if(start_blk == LAST_READ)
	  	{
	  	     KILL_CACHE;
	  	}
}	  
	  if (gbi_blk_write
		  (mpt_idx, start_blk, STND_NR_BLK,
		   (UINT32 *) write_info->pucBuffer) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE_ERROR("RFSFAT: BLK_WRITE_CASE_4 gbi_blk_write failed");
		  return RFSFAT_INTERNAL_ERR;
		}
	  RFSFAT_SEND_TRACE_DEBUG_LOW	("RFSFAT: BLK_WRITE_CASE_4 successfully executed");
	  break;
	case BLK_WRITE_CASE_5:
	  //5) writeblk > 1 blk (fragmented 1st blk next n blks are whole)
	  if (first_fragmented_write (write_info, mpt_idx, &support) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE_ERROR
			("RFSFAT: BLK_WRITE_CASE_5 first_fragmented_write failed");
		  return RFSFAT_INTERNAL_ERR;
		}
	  else
		{
		  start_blk = (write_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
		  //already the next blk
		  start_blk++;
		  support.pucBuffer = (write_info->pucBuffer + support.tNrBytes);
		  //the remaining number of bytes to write
		  support.tNrBytes = (write_info->tNrBytes - support.tNrBytes);
		  nr_blk = (write_info->tNrBytes / MPT_TABLE[mpt_idx].blk_size);
if(mpt_idx == LAST_MPT_IDX_WR)
{
	     		/* Check for read cache */
	  	 for(i=start_blk;i<(start_blk+nr_blk);i++)
      		{
      	   	if( i == LAST_READ)
		   		{
		   		     KILL_CACHE;     /* Kill the Read cache  */
					 break;
		   		}
      		}
}		 

		  if (gbi_blk_write
			  (mpt_idx, start_blk, nr_blk,
			   (UINT32 *) support.pucBuffer) != RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE_ERROR
				("RFSFAT: BLK_WRITE_CASE_5 gbi_blk_write failed");
			  return RFSFAT_INTERNAL_ERR;
			}
		}
	  RFSFAT_SEND_TRACE_DEBUG_LOW("RFSFAT: BLK_WRITE_CASE_5 successfully executed");
	  break;
	case BLK_WRITE_CASE_6:
	  //6) writeblk > 1 blk (n blks are all exactly whole blks)
	  start_blk = (write_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
	  nr_blk = (write_info->tNrBytes / MPT_TABLE[mpt_idx].blk_size);
if(mpt_idx == LAST_MPT_IDX_WR)
{
		 
      /* Check for Write cache and Read cache  */
	   for(i=start_blk;i<(start_blk+nr_blk);i++)
      	{
      	   if( i== LAST_WRITE)
      	   	{
      	   	       KILL_WRITE_CACHE;    /* KILL the Write cache  */
      	   	}
		  
		   	if( i == LAST_READ)
		   		{
		   		     KILL_CACHE;     /* Kill the Read cache  */
		   		}
      	}
}
	  if (gbi_blk_write
		  (mpt_idx, start_blk, nr_blk,
		   (UINT32 *) write_info->pucBuffer) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE_ERROR
			("RFSFAT: BLK_WRITE_CASE_6 gbi_blk_write failed");
		  return RFSFAT_INTERNAL_ERR;
		}
	  RFSFAT_SEND_TRACE_DEBUG_LOW
		("RFSFAT: BLK_WRITE_CASE_6 successfully executed");
	  break;
	case BLK_WRITE_CASE_7:
	  //7) writeblk > 1 blk (first n blks are exactly whole blks last block is fragmented)
	  start_blk = (write_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
	  nr_blk = (write_info->tNrBytes / MPT_TABLE[mpt_idx].blk_size);
if(mpt_idx == LAST_MPT_IDX_WR)
{
	
     /* Check for Write cache and Read cache  */
	   for(i=start_blk;i<(start_blk+nr_blk);i++)
      	{
      	   if( i== LAST_WRITE)
      	   	{
      	   	       KILL_WRITE_CACHE;    /* KILL the Write cache  */
      	   	}
		  
		   	if( i == LAST_READ)
		   		{
		   		     KILL_CACHE;     /* Kill the Read cache  */
		   		}
      	}
}
	  if (gbi_blk_write
		  (mpt_idx, start_blk, nr_blk,
		   (UINT32 *) write_info->pucBuffer) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE_ERROR
			("RFSFAT: BLK_WRITE_CASE_7 gbi_blk_write failed");
		  return RFSFAT_INTERNAL_ERR;
		}
	  else
		{
		  start_blk = start_blk + nr_blk;
		  //the remaining number of bytes to write
		  support.tNrBytes =(write_info->tNrBytes % MPT_TABLE[mpt_idx].blk_size);
		  support.pucBuffer =(write_info->pucBuffer + (write_info->tNrBytes - support.tNrBytes));
		  
		  if (read_fragmented_write (start_blk, mpt_idx, &support) !=
			  RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE_ERROR
				("RFSFAT:BLK_WRITE_CASE_7 read_fragmented_write failed");
			  return RFSFAT_INTERNAL_ERR;
			}
		}
	  RFSFAT_SEND_TRACE_DEBUG_LOW
		("RFSFAT: BLK_WRITE_CASE_7 successfully executed");
	  break;
	case BLK_WRITE_CASE_8:
	  //8) writeblk > 1 blk (fragmented 1st blk, next n blks are whole, last block is fragmented)
	  if (first_fragmented_write (write_info, mpt_idx, &support) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE_ERROR
			("RFSFAT: BLK_WRITE_CASE_8 first_fragmented_write failed");
		  return RFSFAT_INTERNAL_ERR;
		}
	  else
		{
		  start_blk = (write_info->tOffset / MPT_TABLE[mpt_idx].blk_size);
		  //already the next blk
		  start_blk++;
		  first_nr_bytes = support.tNrBytes;
		  support.pucBuffer = (write_info->pucBuffer + first_nr_bytes);
		  //the remaining number of bytes to write
		  /* This incudes n-blocks and remaining length */
		  support.tNrBytes = (write_info->tNrBytes - support.tNrBytes);
      nr_blk = (support.tNrBytes / MPT_TABLE[mpt_idx].blk_size);
if(nr_blk != 0)
{
if(mpt_idx == LAST_MPT_IDX_WR)
{
	
    		/* Check for read cache */
	  	 for(i=start_blk;i<(start_blk+nr_blk);i++)
      		{
      	   	if( i == LAST_READ)
		   		{
		   		     KILL_CACHE;     /* Kill the Read cache  */
					 break;
		   		}
      		}
}
		  if (gbi_blk_write
			  (mpt_idx, start_blk, nr_blk,
			   (UINT32 *) support.pucBuffer) != RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE_ERROR
				("RFSFAT: BLK_WRITE_CASE_8 gbi_blk_write failed");
			  return RFSFAT_INTERNAL_ERR;
			}

         /* Now , remaining 1 block write */
			  start_blk += nr_blk;
		  //the remaining number of bytes to write
		  support.tNrBytes = ((write_info->tNrBytes - first_nr_bytes) % MPT_TABLE[mpt_idx].blk_size);
		  support.pucBuffer = (write_info->pucBuffer +
								   (write_info->tNrBytes - support.tNrBytes));
}	/* if (nr_blk != 0)*/		  

	if (read_fragmented_write (start_blk, mpt_idx, &support) !=  RFSFAT_OK)
	{

	  RFSFAT_SEND_TRACE_ERROR("RFSFAT:BLK_WRITE_CASE_8 read_fragmented_write failed");
				  return RFSFAT_INTERNAL_ERR;
	}
		
	} /* End of else */
	  RFSFAT_SEND_TRACE_DEBUG_LOW
		("RFSFAT: BLK_WRITE_CASE_8 successfully executed");
	  break;
	default:
	  RFSFAT_SEND_TRACE_ERROR
		("RFSFAT: a valid write_case should have been provided");
	  return RFSFAT_INTERNAL_ERR;
	}							//end switch 
  return RFSFAT_OK;
}

/**
* This function reads the first fragmented block (of a sequence) the first part
* of the block is not written, (part can be 1 till (blocksize -1) bytes). the 
* second part untill the end of block, will be written with with write data.
* The write cache will be written (flushed) at the end.
* A check is performed if the block not already exists in cache and if so deal 
* with it to add data to that cache.
* 
* @param             MMA_RW_STRUCT *write_info
* @param             UINT32 start_blk
* @param             UINT8 mpt_idx
* @param             MMA_RW_STRUCT *support
*
* @return            RFSFAT_OK            successfully executed
*                    RFSFAT_INTERNAL_ERR  not able to perform requested funcitonality
*/
UINT8
first_fragmented_write (MMA_RW_STRUCT * write_info, UINT8 mpt_idx,
						MMA_RW_STRUCT * support)
{
  if (random_block_write (write_info, mpt_idx, support) != RFSFAT_OK)
	{

	  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: random_block_write failed");
	  return RFSFAT_INTERNAL_ERR;
	}
  //Since this is the First fragmented write_cache must be entirely filled up now
  //--> flush cache (actually write it)
  if (flush_write_blk () != FFS_NO_ERRORS)
	{

	  KILL_WRITE_CACHE;
	  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: flush write block failed");
	  return RFSFAT_INTERNAL_ERR;
	}

  return RFSFAT_OK;
}

/**
* This function copies content from a client buffer into the write cache
* its starts copying from the offset indicated as many as indicated
* 
* 
* @param             MMA_RW_STRUCT *buf_p   client buffer pointer  
* @param             UINT16        offset   offset in write cache
* @param             UINT16        nr_byte  and nr bytes to write in write cache
*
* @return            RFSFAT_OK
*/
UINT8
copy_buf_to_cache (MMA_RW_STRUCT * buf_p, UINT16 offset, UINT32 nr_byte)
{
  UINT8 *buf_pntr;
  UINT8 *client_buf_p;

  buf_pntr = ((UINT8 *) WRITE_CACHE + offset);
  client_buf_p = buf_p->pucBuffer;

  memcpy(buf_pntr,client_buf_p,nr_byte);

  //set to actually copied nr of bytes
  buf_p->tNrBytes = nr_byte;
  return RFSFAT_OK;
}

/**
* 
* This function creates the write cache and fills it with original
* data read from the block that is represented by the write cache 
* Not the whole cache is filled with read data, only the part 
* that is specified by read_data (nr_bytes) And from offset 
* @param              MMA_RW_STRUCT *read_data
* @param              UINT8 mpt_idx
*
* @return             RFSFAT_MEMORY_ERR
*                     RFSFAT_INTERNAL_ERR
*                     RFSFAT_OK
*/
UINT8
create_cache_write_data (UINT32 start_blk, UINT8 mpt_idx)
	{
  
 if( read_from_gbi(start_blk,STND_NR_BLK,STND_REMNDR,((UINT32*)WRITE_CACHE), mpt_idx) != RFSFAT_OK)
  {
     KILL_WRITE_CACHE;
	 RFSFAT_SEND_TRACE ("RFSFAT: create_cache_write_data read_from_gbi failed",
								 RV_TRACE_LEVEL_ERROR);
	 return RFSFAT_INTERNAL_ERR;
  
   }
 
  LAST_WRITE = start_blk;
  LAST_MPT_IDX_WR = mpt_idx;

  if((LAST_WRITE == LAST_READ) && (LAST_MPT_IDX_WR == LAST_MPT_IDX_RD) )
  	{
  	     /* Read cache is newer , kill this */
		 KILL_CACHE;
  	}

  return RFSFAT_OK;
}

/**
* Write data block(s) to GBI
* 
* 
* @param             UINT8 mpt_idx
* @param             UINT32 start_blk
* @param             UINT32 nr_blk
*
* @return             RFSFAT_INTERNAL_ERR
*                     RFSFAT_OK          
*/
UINT8
gbi_blk_write (UINT8 mpt_idx, UINT32 start_blk, UINT32 nr_blk,
			   UINT32 * clnt_buf_p)
{
  T_GBI_WRITE_RSP_MSG *msg_p;	//pointer to GBI response message
  UINT8 ret_val = RFSFAT_OK;

  RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: Write to GBI ..start \n");
  
  /* protective fixes */
  /* AT any point of time, we should not call this function with 
     nr_blk as zero 
   */
  if((clnt_buf_p == NULL) || (nr_blk == 0))
  	{
  	  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: gbi_blk_write  Invalid parameters \n\r");
	  return RFSFAT_INTERNAL_ERR;
  	}
  if (gbi_write (MPT_TABLE[mpt_idx].media_nbr,
				 MPT_TABLE[mpt_idx].partition_nbr,
				 start_blk,
				 nr_blk,
				 STND_REMNDR,
				 clnt_buf_p,
				 rfsfat_env_ctrl_blk_p->return_path_2nd_queue) != RV_OK)
	{

	  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: gbi_write  failed");
	  return RFSFAT_INTERNAL_ERR;
	}
  else
	{
	  // WAIT for GBI response (blocking)
 	 msg_p = (T_GBI_WRITE_RSP_MSG *) return_queue_get_msg ();

 /* In case of NULL return ERROR */
 	if(msg_p==NULL)
 	    return RFSFAT_INTERNAL_ERR;
	  
	  switch (msg_p->result)
		{
		case RV_OK:
		  ret_val = RFSFAT_OK;
		  break;
		case RV_INVALID_PARAMETER:	//deliberate fallthrough
		case RV_NOT_READY:		//deliberate fallthrough
		case RV_MEMORY_ERR:	//deliberate fallthrough
		case RV_INTERNAL_ERR:	//deliberate fallthrough
		  RFSFAT_SEND_TRACE_ERROR
			("RFSFAT: GBI write failed error response! ");
		  ret_val = RFSFAT_INTERNAL_ERR;
		  break;
		default:
		  RFSFAT_SEND_TRACE_ERROR
			("RFSFAT: undefined response msg from GBI! ");
		  ret_val = RFSFAT_INTERNAL_ERR;
		  break;
		}
	}
  rvf_free_msg ((T_RVF_MSG *) msg_p);
  msg_p=NULL;
  RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: Write to GBI ..end \n");
  return ret_val;
}

/**
* Function creates a 1 blocksize write_cache into which data is placed.
* the remaining part (always because FRAGMENTED) will be left empty
* until a new write will fill it up or until it is flushed and the remainder
* is filled with new written data.
* 
* 
* @param             UINT32 start_blk
* @param             UINT8 mpt_idx
* @param             MMA_RW_STRUCT *support
*
* @return             RFSFAT_MEMORY_ERR
*                     RFSFAT_INTERNAL_ERR
*                     RFSFAT_OK
*/
UINT8
read_fragmented_write (UINT32 start_blk, UINT8 mpt_idx,
					   MMA_RW_STRUCT * support)
{
  UINT16 nr_byte;
  support->tOffset = BLK_FIRST;

 
	  //there is still a write_cache
	  RFSFAT_SEND_TRACE_WARNING
		("RFSFAT: There is a previously assigned write_cache\
      STRANGE --> flush it, before continuing");
	  if (flush_write_blk () != FFS_NO_ERRORS)
		{

		  KILL_WRITE_CACHE;
		  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: flush write block failed");
		  return RFSFAT_INTERNAL_ERR;
		}


  //write cache is created ->fill first part with the original data
 
 /*FILL the WRITE_CACHE from physical block  */
 if( read_from_gbi(start_blk,STND_NR_BLK,STND_REMNDR,((UINT32*)WRITE_CACHE), mpt_idx) != RFSFAT_OK)
 	{
 	   /* read failure */
	    RFSFAT_SEND_TRACE ("RFSFAT: read_fragmented_write read_from_gbi failed", RV_TRACE_LEVEL_ERROR);
			  return RFSFAT_INTERNAL_ERR;	   
	   
 	}
  nr_byte = (UINT16) support->tNrBytes;
  if (copy_buf_to_cache (support, 0, nr_byte) != RFSFAT_OK)
	{

	  KILL_WRITE_CACHE;
	  RFSFAT_SEND_TRACE_ERROR
		("RFSFAT: last fragmented copy_buf_to_cache failed ");
	  return RFSFAT_INTERNAL_ERR;
	}
  else if (support->tNrBytes != nr_byte)
	{

	  //check the amount of bytes copied to cache 
	  KILL_WRITE_CACHE;
	  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: wrong nr of bytes copied from client \
      buf to write cache");
	  return RFSFAT_INTERNAL_ERR;
	}

  LAST_WRITE = start_blk;
  LAST_MPT_IDX_WR = mpt_idx;


  if((LAST_WRITE == LAST_READ) && (LAST_MPT_IDX_WR == LAST_MPT_IDX_RD) )
  	{
  	   /* Read cache is  older than write cache for same block */
	   /* kill the read cache */
	   KILL_CACHE;
  	}

  return RFSFAT_OK;
}

/**
* This function writes data from a buffer provided by the client to the write
* cache. If the cache already exists it finds out where to place the data and 
* if gaps arise that need to be filled up with original data.
* If the write cache doesn't exist, one is created first. The write_cache is 
* not written to the media. This will be done when a new write is requested on 
* a different block
*
* Eventually the support->tNrBytes will be filled with the nr of bytes 
* actually written, before returning
* 
* @param              MMA_RW_STRUCT *write_info
* @param              UINT8 mpt_idx
* @param              MMA_RW_STRUCT *support
*
* @return             RFSFAT_INTERNAL_ERR
*                     RFSFAT_OK          
*/
UINT8
random_block_write (MMA_RW_STRUCT * write_info, UINT8 mpt_idx,
					MMA_RW_STRUCT * support)
{

 UINT32 start_blk;
  UINT16 offset;
  UINT16 nr_byte;

  support->pucBuffer = NULL;
  support->tOffset = BLK_EMPTY;
  support->tNrBytes = BLK_EMPTY;
  offset = (write_info->tOffset % MPT_TABLE[mpt_idx].blk_size);
  start_blk = (write_info->tOffset / MPT_TABLE[mpt_idx].blk_size);

	if (WRITE_CACHE_VALID(mpt_idx,start_blk))
	{
	;  /* Nothing to here */
	}
  else
	{
		  if (flush_write_blk () != FFS_NO_ERRORS)
			{

			  KILL_WRITE_CACHE;
			  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: flush write block failed");
			  return RFSFAT_INTERNAL_ERR;
			}

	  if (create_cache_write_data (start_blk, mpt_idx) != RFSFAT_OK)
		{

		  KILL_WRITE_CACHE;
		  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: create_buf_read_data failed");
		  return RFSFAT_INTERNAL_ERR;
		}
	}
  //write cache is (or was) created and untill the offset where the actual write 
  //shall start is filled with original data (or previously written data)
  if ((write_info->tNrBytes + offset) > MPT_TABLE[mpt_idx].blk_size)
	{

	  //first fragmented?!
	  nr_byte = (MPT_TABLE[mpt_idx].blk_size - offset);
	}
  else
	{
	  /* only one block to write , less or equal to block size */
	  nr_byte = write_info->tNrBytes;
	}
  //now support will be used to provide the client buffer the other 2 "support"
  //variables are not used
  support->pucBuffer = write_info->pucBuffer;
  support->tNrBytes = nr_byte;
  if (copy_buf2cache (support, offset, nr_byte) != RFSFAT_OK)
	{

	  RFSFAT_SEND_TRACE ("RFSFAT: copy_buf2cache failed",
						 RV_TRACE_LEVEL_ERROR);
	  return RFSFAT_INTERNAL_ERR;
	}

  if((LAST_WRITE == LAST_READ) && (LAST_MPT_IDX_WR == LAST_MPT_IDX_RD) )
  	{
  	     KILL_CACHE;    /* Write cache is newer than read cache , so kill this */
  	}
  return RFSFAT_OK;
}

/***************************************************************************
*
*   BLOCK WRITE / READ FUNCTIONS
*
****************************************************************************/

/**
* 
* This function determines which block read or write case is being
* handled
* 
* @param              MMA_RW_STRUCT *rw_info  read or write info
* @param              BOOL read_write         TRUE = read, 
*                                             FALSE = write
*
* @return             RFSFAT_OK           successfully executed
*                     RFSFAT_INTERNAL_ERR read or write block function
*                                         failed                          
*/
UINT8
determine_case (MMA_RW_STRUCT * rw_info, BOOL read_write, UINT8 mpt_idx)
{

  //check if read size is larger than 1 block
  if (rw_info->tNrBytes > MPT_TABLE[mpt_idx].blk_size)
	{

	  //case 5,6,7 or 8
	  if ((rw_info->tOffset % MPT_TABLE[mpt_idx].blk_size) != 0)
		{

		  //case 5 or 8
		  if (((rw_info->tNrBytes - (MPT_TABLE[mpt_idx].blk_size -
									 (rw_info->tOffset %
									  MPT_TABLE[mpt_idx].blk_size))) %
			   MPT_TABLE[mpt_idx].blk_size) != 0)
			{

			  //case 8
			  if (read_write)
				{

				  if (read_blk (rw_info, mpt_idx, BLK_READ_CASE_8) !=
					  RFSFAT_OK)
					{

					  RFSFAT_SEND_TRACE ("RFSFAT: read_blk for CASE 8 failed",
										 RV_TRACE_LEVEL_ERROR);
					  return RFSFAT_INTERNAL_ERR;
					}
				}
			  else if (write_blk (rw_info, mpt_idx, BLK_WRITE_CASE_8) !=
					   RFSFAT_OK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: write_blk for CASE 8 failed",
									 RV_TRACE_LEVEL_ERROR);
				  return RFSFAT_INTERNAL_ERR;
				}
			}
		  else if (read_write)
			{
			  //case 5
			  if (read_blk (rw_info, mpt_idx, BLK_READ_CASE_5) != RFSFAT_OK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: read_blk for CASE 5 failed",
									 RV_TRACE_LEVEL_ERROR);
				  return RFSFAT_INTERNAL_ERR;
				}
			}
		  else if (write_blk (rw_info, mpt_idx, BLK_WRITE_CASE_5) !=
				   RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE ("RFSFAT: write_blk for CASE 5 failed",
								 RV_TRACE_LEVEL_ERROR);
			  return RFSFAT_INTERNAL_ERR;
			}
		}
	  else if ((rw_info->tNrBytes % MPT_TABLE[mpt_idx].blk_size) == 0)
		{						//case 6 or 7

		  //case 6
		  if (read_write)
			{

			  if (read_blk (rw_info, mpt_idx, BLK_READ_CASE_6) != RFSFAT_OK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: read_blk for CASE 6 failed",
									 RV_TRACE_LEVEL_ERROR);
				  return RFSFAT_INTERNAL_ERR;
				}
			}
		  else if (write_blk (rw_info, mpt_idx, BLK_WRITE_CASE_6) !=
				   RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE ("RFSFAT: write_blk for CASE 6 failed",
								 RV_TRACE_LEVEL_ERROR);
			  return RFSFAT_INTERNAL_ERR;
			}
		}
	  else if (read_write)
		{

		  if (read_blk (rw_info, mpt_idx, BLK_READ_CASE_7) != RFSFAT_OK)
			{

			  //case 7    
			  RFSFAT_SEND_TRACE ("RFSFAT: read_blk for CASE 7 failed",
								 RV_TRACE_LEVEL_ERROR);
			  return RFSFAT_INTERNAL_ERR;
			}
		}
	  else if (write_blk (rw_info, mpt_idx, BLK_WRITE_CASE_7) != RFSFAT_OK)
		{

		  //case 7    
		  RFSFAT_SEND_TRACE ("RFSFAT: write_blk for CASE 7 failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	}
  else if (rw_info->tNrBytes == MPT_TABLE[mpt_idx].blk_size)
	{

	  //case 3 or 4
	  if ((rw_info->tOffset % MPT_TABLE[mpt_idx].blk_size) != 0)
		{

		  //case 3
		  if (read_write)
			{

			  if (read_blk (rw_info, mpt_idx, BLK_READ_CASE_3) != RFSFAT_OK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: read_blk for CASE 3 failed",
									 RV_TRACE_LEVEL_ERROR);
				  return RFSFAT_INTERNAL_ERR;
				}
			}
		  else if (write_blk (rw_info, mpt_idx, BLK_WRITE_CASE_3) !=
				   RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE ("RFSFAT: write_blk for CASE 3 failed",
								 RV_TRACE_LEVEL_ERROR);
			  return RFSFAT_INTERNAL_ERR;
			}
		}
	  else if (read_write)
		{
		  if (read_blk (rw_info, mpt_idx, BLK_READ_CASE_4) != RFSFAT_OK)
			{
			  //case 4  
			  RFSFAT_SEND_TRACE ("RFSFAT: read_blk for CASE 4 failed",
								 RV_TRACE_LEVEL_ERROR);
			  return RFSFAT_INTERNAL_ERR;
			}
		}
	  else if (write_blk (rw_info, mpt_idx, BLK_WRITE_CASE_4) != RFSFAT_OK)
		{
		  //case 4  
		  RFSFAT_SEND_TRACE ("RFSFAT: write_blk for CASE 4 failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	}
  else if ((rw_info->tOffset % MPT_TABLE[mpt_idx].blk_size) != 0)
	{							//case 1 or 2

	  if (((rw_info->tOffset % MPT_TABLE[mpt_idx].blk_size) +
		   rw_info->tNrBytes) <= MPT_TABLE[mpt_idx].blk_size)
		{

		  //case 2
		  if (read_write)
			{
			  if (read_blk (rw_info, mpt_idx, BLK_READ_CASE_2) != RFSFAT_OK)
				{

				  RFSFAT_SEND_TRACE ("RFSFAT: read_blk for CASE 2 failed",
									 RV_TRACE_LEVEL_ERROR);
				  return RFSFAT_INTERNAL_ERR;
				}
			}
		  else if (write_blk (rw_info, mpt_idx, BLK_WRITE_CASE_2) !=
				   RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE ("RFSFAT: write_blk for CASE 2 failed",
								 RV_TRACE_LEVEL_ERROR);
			  return RFSFAT_INTERNAL_ERR;
			}
		}
	  else if (read_write)
		{
		  //case 1  
		  if (read_blk (rw_info, mpt_idx, BLK_READ_CASE_1) != RFSFAT_OK)
			{

			  RFSFAT_SEND_TRACE ("RFSFAT: read_blk for CASE 1 failed",
								 RV_TRACE_LEVEL_ERROR);
			  return RFSFAT_INTERNAL_ERR;
			}
		}
	  else if (write_blk (rw_info, mpt_idx, BLK_WRITE_CASE_1) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: write_blk for CASE 1 failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	}
  else if (read_write)
	{
	  //case 2
	  if (read_blk (rw_info, mpt_idx, BLK_READ_CASE_2) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: read_blk for CASE 2 failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	}
  else
	{
	  if (write_blk (rw_info, mpt_idx, BLK_WRITE_CASE_2) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: write_blk for CASE 2 failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	}
  return RFSFAT_OK;
}



/**
* This is a support function to support both read and write functions
* to copy a small buffer with into the write cache
* 
* 
* @param             MMA_RW_STRUCT *support
* @param             UINT16 offset
* @param             UINT16 nr_byte
*
* @return            RFSFAT_INTERNAL_ERR
*                    RFSFAT_OK
*/
UINT8
copy_buf2cache (MMA_RW_STRUCT * support, UINT16 offset, UINT32 nr_byte)
{
  if (copy_buf_to_cache (support, offset, nr_byte) != RFSFAT_OK)
	{

	  KILL_WRITE_CACHE;
	  RFSFAT_SEND_TRACE ("RFSFAT: copy client buf to write cache failed",
						 RV_TRACE_LEVEL_ERROR);
	  return RFSFAT_INTERNAL_ERR;
	}
  else if (support->tNrBytes != nr_byte)
	{

	  KILL_WRITE_CACHE;
	  //check the amount of bytes copied to cache 
	  RFSFAT_SEND_TRACE ("RFSFAT: wrong nr of bytes copied from client \
      buf to write cache", RV_TRACE_LEVEL_ERROR);
	  return RFSFAT_INTERNAL_ERR;
	}
  else
	{
	  //succesfully written
	  support->tNrBytes = nr_byte;
	  return RFSFAT_OK;
	}
}

/**
* This is a support function that is called when a read is performed on a block 
* that has a more recent version in the Writecache than in the readcache OR medium
* 
* @param             UINT32 rb        readblock
* @param             UINT32 offset    offset block
* @param             UINT32 nr_byte   nr_byte to read from block
* @param             UINT32 mpt_idx   mountpoint indicator
*
* @return            RFSFAT_INTERNAL_ERR
*                    RFSFAT_OK
*/
UINT8
wc_newer_rc (UINT32 rb, UINT8 mpt_idx)
{
     /* copy the entire write cache */
     memcpy(READ_CACHE,WRITE_CACHE,MPT_TABLE[mpt_idx].blk_size);
     LAST_MPT_IDX_RD = LAST_MPT_IDX_WR;
	 LAST_READ = LAST_WRITE;
  
  //write cache is newer so use write cache
  RFSFAT_SEND_TRACE ("RFSFAT: using newer write cache in wc_newer_rc ",
					 RV_TRACE_LEVEL_WARNING);
  
  return RFSFAT_OK;
}

/**
* This is a support function that is called when a read is performed on a block 
* here is checked if it has a more recent version in the Writecache than in the 
* readcache OR in the medium
* 
* @param             UINT32 rb        readblock
* @param             UINT32 mpt_idx   mountpoint indicator
* @param             MMA_RW_STRUCT *read_support container of read info

* @return            RFSFAT_INTERNAL_ERR
*                    RFSFAT_OK
*/
UINT8
safe_read (UINT32 rb, UINT8 mpt_idx, MMA_RW_STRUCT * read_support)
{
  if ((rb == LAST_WRITE) && (mpt_idx == LAST_MPT_IDX_WR))
	{

	  //block to read has modified version in write cache 
	  if (wc_newer_rc (rb, mpt_idx) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE ("RFSFAT: wc_newer_rc in safe read failed",
							 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  //readcache is actually write cache now ->safe to read from write cache
	  if (copy_cache_to_buf (read_support, READ_CACHE) != RFSFAT_OK)
		{

	  RFSFAT_SEND_TRACE	("RFSFAT: copy_cache_to_buf in  safe read WR=rb failed",
			 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  else
		{
		  return RFSFAT_OK;
		}
	}
  else if (READ_CACHE_VALID(mpt_idx,rb))
	{

	  //blk to read is stored in readcache
	  if (copy_cache_to_buf (read_support, READ_CACHE) != RFSFAT_OK)
		{

		  RFSFAT_SEND_TRACE
			("RFSFAT: copy_cache_to_buf in safe read LR=rb failed",
			 RV_TRACE_LEVEL_ERROR);
		  return RFSFAT_INTERNAL_ERR;
		}
	  else
		{
		  return RFSFAT_OK;
		}
	}
  else
	{
	  //blk must be freshly read from medium
	  return read_fragmented (read_support, rb, mpt_idx);
	}
}

/**
* This is a support function that is called when a read is performed for multiple whole blocks 
* if one of these blocks happens to be recently modified in the writecache this blocks is
* completed with original data and written to the medium. After this the whole block stream can
* be read at once to the client buffer.
* if none of the blocks is in the writecache this function returns without doing anything
* 
* @param             UINT32 startblk  block where the multiple block read starts
* @param             UINT32 mpt_idx   mountpoint indicator
* @param             UINT32 bufsize   nr of blocks to read in the multiple block read

* @return            RFSFAT_INTERNAL_ERR
*                    RFSFAT_OK
*/
UINT8
intermediate_flush (UINT32 startblk, UINT32 bufsize, UINT8 mpt_idx)
{
  UINT32 i;

  if (mpt_idx != LAST_MPT_IDX_WR)
	{

	  //other mountpoint so stop check
	  return RFSFAT_OK;
	}
  //find if one of the blocks has a modified (newer) version in the writecache
  for (i = startblk; i < (bufsize + startblk); i++)
	{
	  if((LAST_READ == i) && (LAST_MPT_IDX_RD == LAST_MPT_IDX_WR) )
	  	{
	  	     KILL_CACHE;
	  	}

	  if (LAST_WRITE == i)
		{

	       if(flush_write_blk() != FFS_NO_ERRORS)
	       	{
	       	     RFSFAT_SEND_TRACE_ERROR("RFSFAT: Intermediate flush is failed"); 
	       	}
	
			  RFSFAT_SEND_TRACE_WARNING("RFSFAT: Intermediate flush is succesfully performed");
			  KILL_WRITE_CACHE;
		}
	  
	
	}
  return RFSFAT_OK;
}

void RFSFAT_invalidate_buffer_cache(UINT8 mptidx)
{

 if(LAST_MPT_IDX_WR == mptidx)
 	{
 	  /* Kill the write and read caches */
	  KILL_CACHE;  
	  KILL_WRITE_CACHE;
 	     
 	}
}

/**
* Erase data block(s) of GBI
* 
* 
* @param             UINT8 mpt_idx
* @param             UINT32 start_blk
* @param             UINT32 nr_blk
*
* @return             RFSFAT_INTERNAL_ERR
*                     RFSFAT_OK          
*/
UINT8
gbi_blk_erase (UINT8 mpt_idx, UINT32 start_blk, UINT32 nr_blk)
			   
{
  T_GBI_ERASE_RSP_MSG *msg_p;	//pointer to GBI response message
  UINT8 ret_val = RFSFAT_OK;

  RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: Erase GBI blocks..start \n");
  
  if (gbi_erase (MPT_TABLE[mpt_idx].media_nbr,
				 MPT_TABLE[mpt_idx].partition_nbr,
				 start_blk,
				 nr_blk,
				 rfsfat_env_ctrl_blk_p->gbi_return_path) != RV_OK)
	{

	  RFSFAT_SEND_TRACE_ERROR ("RFSFAT: gbi_erase  failed");
	  return RFSFAT_INTERNAL_ERR;
	}
  else
	{
	  // WAIT for GBI response (blocking)
 	 msg_p = (T_GBI_ERASE_RSP_MSG *) return_queue_get_msg ();

	 /* In case of NULL return ERROR */
 	if(msg_p==NULL)
 	    return RFSFAT_INTERNAL_ERR;

	  
	  
	  switch (msg_p->result)
		{
		case RV_OK:
		  ret_val = RFSFAT_OK;
		  break;
		case RV_INVALID_PARAMETER:	//deliberate fallthrough
		case RV_NOT_READY:		//deliberate fallthrough
		case RV_MEMORY_ERR:	//deliberate fallthrough
		case RV_INTERNAL_ERR:	//deliberate fallthrough
		  RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: GBI erase failed error response! ",msg_p->result);
		  ret_val = RFSFAT_INTERNAL_ERR;
		  break;
		default:
		  RFSFAT_SEND_TRACE_PARAM_ERROR("RFSFAT: undefined response msg from GBI! ",msg_p->result);
		  ret_val = RFSFAT_INTERNAL_ERR;
		  break;
		}
	}
  rvf_free_msg ((T_RVF_MSG *) msg_p);
  msg_p=NULL;
  RFSFAT_SEND_TRACE_DEBUG_HIGH("RFSFAT: Erase GBI blocks..end \n");
  return ret_val;
}

