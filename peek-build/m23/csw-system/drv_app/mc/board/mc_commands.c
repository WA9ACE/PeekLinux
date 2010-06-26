/**
 * @file  mc_commands.c
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
#include "nucleus.h"
#include "inth/sys_inth.h"
#include "rvf/rvf_api.h"
#ifndef _WINDOWS
#include "memif/mem.h"
#endif
#include "mc/mc_i.h"
#include "mc/mc_api.h"
#include "mc/board/mc_func.h"
#include "mc/board/mc_inth.h"
#include "mc/mc_cfg.h"
#include "mc/mc_commands.h"
#include "spi/spi_drv.h"
static char mc_buf1[100];
#define MC_TRACE_WARNING_PARAM(string, param) { \
		rvf_send_trace((string), sizeof(string) - 1, param, RV_TRACE_LEVEL_WARNING, MC_USE_ID); }
/*******************************************************************************************/
// ADDED FOR CREATING DATA TOKEN FOR LOCOSTO SDCard **/
#define CRC_GEN 0x11021
#define START_BLOCK_SINGLE 0xFE  //start block for single write/read(single block)
#define START_BLOCK_MULTIPLE 0xFC  //start block for multiple write/read(multiple block)
#define START_MULTI_BLOCK 0xFC//start block for multiple write
#define STOP_BLOCK 0xFD// start block for stop transaction
#define DATA_TOKEN_SIZE
UINT8 crc16[2];
UINT8 *data_token=NULL;
#define MAX_ATTEMPT 2
UINT16  i, nb, crc = 0;
UINT16 get_CRC16(UINT8 *);
UINT8* data_ptr;
UINT8 data_token_mul[515];
UINT8 *data_token_temp=NULL;
/* This function will create a data token of specified length +3.
 * The remaing two bytes will store the 16 bit CRC And Start Block
 */

T_RV_RET Data_Token_Create(UINT8 *data,INT32 length,UINT8 type)
{
T_RVF_MB_STATUS	mb_status;
T_RVF_MB_ID data_p=NULL;

 if(type==1)
	{

				data_token=&data_token_mul[0];
				data_token_temp=data_token;

				 *data_token++=START_BLOCK_SINGLE;
				 memcpy(data_token,data,length+1);
				 data_token+=length+1;
			     *data_token++=0x00;
			     *data_token=0x00;
				data_token=data_token_temp;
			 return RV_OK;

	}
 else
	{
		   data_token_temp=&data_token_mul[0];
		   *data_token_temp=START_BLOCK_MULTIPLE;
		   data_token_temp++;
		   memcpy(data_token_temp,data,length);
		   data_token_temp=&data_token_mul[0];
		   return RV_OK;


	}

}


//added for locosto

/*
 * This function generates a CRC16 for the data token
 */

UINT16 get_CRC16(UINT8 *data_bytes)
{
  UINT16 byte;
  UINT8 ibit;
  UINT16 reg = 0;

  for (byte = SD_DATA_SIZE-1; byte > 0; byte--)
  {
    for (ibit = 0; ibit < 8; ibit++)
    {
      reg <<= 1;
      reg ^= ((((*(data_bytes+byte) << ibit) ^ reg) & 0x8000) ? 0x1021 : 0);
    }
  }
  reg=reg<<1;
  return reg;
}

/******************************************************************************************/

 // Added for MMC compilation in Locosto
  #define    C_DMA_CHANNEL_MMC_SD_RX        16
  #define    C_DMA_CHANNEL_MMC_SD_TX        17
  #define    DMA_SYNC_DEVICE_MC_SD_RX       16
  #define    DMA_SYNC_DEVICE_MC_SD_TX        17

// Added for MMC compilation in Locosto


/*
 * This file contains the core functions for the MC driver.
 * See [MultiMediaCard System, A.5].
 */


//static UINT8  get_CRC7(UINT8 cmd_bytes[]);

static T_RV_RET mc_cmd_read_mblock_dma(  UINT16 nblk,
                             UINT16 blk_len, UINT32 addr,
                             UINT8 *data_p, UINT32 data_size,
                             UINT32 *card_stat_p);

static T_RV_RET mc_cmd_read_mblock_non_dma(  UINT16 nblk,
                             UINT16 blk_len, UINT32 addr,
                             UINT8 *data_p, UINT32 data_size,
                             UINT32 *card_stat_p);

static T_RV_RET mc_cmd_write_mblock_dma( UINT16 nblk,
                             UINT16 blk_len, UINT32 addr,
                             UINT8 *data_p, UINT32 data_size,
                             UINT32 *card_stat_p);

static T_RV_RET mc_cmd_write_mblock_non_dma( UINT16 nblk,
                             UINT16 blk_len, UINT32 addr,
                             UINT8 *data_p, UINT32 data_size,
                             UINT32 *card_stat_p);

/** Max attempts for polling register tests */
//#define MAX_RETRY      200
#define MAX_ATTEMPTS   10

/**
 * This function generates a CRC7 for a data buffer of minimal length of
 * 48 bits.
 *
 */
UINT8  get_CRC7(UINT8 cmd_bytes[])
{
  UINT8 byte;
  UINT8 ibit;
  UINT8 reg = 0;

  for (byte = SD_CMD_SIZE-1; byte > 0; byte--)
  {
    for (ibit = 0; ibit < 8; ibit++)
    {
      reg <<= 1;
      reg ^= ((((cmd_bytes[byte] << ibit) ^ reg) & 0x80) ? 0x9 : 0);
    }
  }
  reg=reg<<1;
  return reg;
}




/**
 * @name Functions implementation
 *
 */
/*@{*/

/**
 * function: mc_wait_card_state
 */
//T_RV_RET mc_wait_card_state(T_MC_RCA rca, UINT16 req_state, UINT8 mode)
T_RV_RET mc_wait_card_state(UINT16 req_state, UINT8 mode)
{
  UINT32 card_stat = 0;
  T_RV_RET retval;
  UINT16 attempts;
  UINT16 correct_state=FALSE;

  for(attempts=0; (attempts < MAX_ATTEMPTS) && (correct_state == FALSE); ++attempts)
  {
    retval=mc_cmd_send_status(&card_stat);
    if(retval == RV_OK)
    {
      if((mode == TRUE) && (((card_stat >> 9) & 0x000F) == req_state))
      {
        /* card is in requested state, exit */
        correct_state=TRUE;
      }
      else if((mode == FALSE) && (((card_stat >> 9) & 0x000F) != req_state))
      {
        /* card is not in requested state, exit */
        correct_state=TRUE;
      }
    }
    /* card not ready, wait 10 ticks */
    if(correct_state==FALSE)
    {
      rvf_delay(10);
    }
  }

  if(correct_state == TRUE)
  {
    retval = RV_OK;
  }
  else
  {
    /* Not in requested state */
    MC_SEND_TRACE("MC mc_wait_card_state: card not in requested state",
    RV_TRACE_LEVEL_WARNING);
    retval = RV_INTERNAL_ERR;
  }

  return retval;
}



/**
 * Initialise host controller
 *
 * Detailled description
 * Initialises the host controller. Sets configuration registers
 *
 * @return RV_OK
 */
T_RV_RET mc_cmd_init_host(T_MC_HOST_CONF *conf, UINT16 irq_mask)
{
  UINT8 i = 0;

  MC_SEND_TRACE("MC commands: mc_cmd_init_host called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* Check if conf is valid */
  if (conf == NULL)
  {
    MC_SEND_TRACE("MC mc_cmd_init_host: Error, invalid pointer",
                    RV_TRACE_LEVEL_ERROR);
    return RV_INVALID_PARAMETER;
  }
return RV_OK;
}

/**
 * Update aquisition
 *
 * Detailled description.
 * Starts an identification cycle of a card stack. The card management
 * information in the controller will be updated. New cards will be
 * initialized; old cards keep their configuration. At the end all
 * active cards are in Stand-by state.
 * This function supports only ONE MC or SD card to be initialised.
 *
 * @return  RV_OK
 */
/************************************************************************/

/************************************************************************/
/*card initialisation or reset sequence to be done for locosto          */
/************************************************************************/

T_RV_RET mc_cmd_update_acq(void)
{

T_RV_RET retval  = RV_OK;
UINT8 resp=0xFF;
T_MC_OCR ret_ocr  = 0;
T_MC_OCR ocr = 0;
UINT32 card_stat = 0;
UINT32   result = 0;
UINT16   i = 1;
UINT16 attempt=0;
UINT32 BeginTime=0;
UINT32 EndTime=0;
UINT32 TimeTaken=0;
UINT32 TimeInSeconds=0;

T_RVF_MB_STATUS mb_status;
T_MC_CARD_INFO *card_info = NULL;
MC_SEND_TRACE("MC commands: mc_cmd_update_acq called",RV_TRACE_LEVEL_DEBUG_LOW);
/** Clear Card administration */
if(mc_env_ctrl_blk_p->reserved_card_id != 0)
  {
    /** Clear RCA stack */
    for(i = 0; i < MC_MAX_STACK_SIZE;i++)
    {
      if(mc_env_ctrl_blk_p->card_infos[i] != NULL)
      {
        MC_FREE_BUF(mc_env_ctrl_blk_p->card_infos[0]);
        mc_env_ctrl_blk_p->card_infos[i] = NULL;
      }
    }
  }
  mc_env_ctrl_blk_p->reserved_card_id = 0;

  /********************************************************/
  /* Card search mechanism
  * Here below the card search actions will be performed.
  * This search is according to the SD card specifications (1.0)
  * and mc card specifications (3.3)
  * First the SD initialize procedure will be used.
  * GO_IDLE_STATE is used first for reseting the SDCard, If card
  * responds then SEND_OP_COND is used to start card initialization process
  * after response obtained from SEND_OP_COND then card asked
  * for its Card Identification Data(CID) by using CMD10 and then Card Specific Data
  * (CSD)is asked from card by using CMD9
  */
  /********************************************************/

   /** Reset card(s) by sending CMD0, this will put available SD cards   */
   /** into idle state
   */

AI_SetBit(MC_GPIO_1);

  go_idle_state();/*CMD0*/
  /* sending cmd1 until response obtained or maximum tries */
  //rvf_send_trace("Sending cmd1",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  //retval=send_op_cond();//sending CMD1 for activation of card initialization process

BeginTime=rvf_get_tick_count();
  do{

  		Send_App_Cmd();
  		resp=Send_SD_Op_Cond();
		//  	attempt++;
        EndTime=rvf_get_tick_count();
	TimeTaken=RVF_TICKS_TO_MS(EndTime-BeginTime); //total number of milliseconds taken
//	TimeInSeconds=(TimeTaken / 1000 );  // total time taken in seconds
//	if(TimeInSeconds >= 1)
//		break;

  }while((resp!=0x00)&& (TimeTaken<1000)); //&&(attempt<400));//while ((resp != 0x00FF)&&(resp != 0xFF00)&&(attempt<500));

sprintf(mc_buf1,"  CMD1 Attempt=%d   \n",attempt);
rvf_send_trace(mc_buf1,(UINT8) (strlen (mc_buf1)),NULL_PARAM, RV_TRACE_LEVEL_WARNING,MC_USE_ID);

if((resp != 0xFF))//||(resp == 0xFF00))
{
retval = RV_OK;
rvf_send_trace("CMD1 Response Zero",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
}
else
{
rvf_send_trace("CMD1 Response Not Zero",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
retval = RV_INTERNAL_ERR;
}

// fix for Board W/O daughter card attached.
if (attempt == 1)
retval = RV_INTERNAL_ERR;


  if(retval == RV_OK)
  {

    /** There has been a response to CMD1 so use SD init done */
  	rvf_send_trace("MC_cmd_update_acq SD card search",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);

    /* Reserve memory for card_info informations */
    mb_status = rvf_get_buf (mc_env_ctrl_blk_p->prim_mb_id,
                              sizeof(T_MC_CARD_INFO),
                              (void **) &mc_env_ctrl_blk_p->card_infos[0]);

    if (mb_status == RVF_RED) /* Memory allocation unsuccessfull */
    {
      /** No memory for card info*/
      MC_SEND_TRACE("MC no memory available exiting",RV_TRACE_LEVEL_ERROR);
      AI_ResetBit(MC_GPIO_1); //Taking GPIO_1 low turns off the 2.8V regulator
      return RV_MEMORY_ERR;
    }

    card_info = mc_env_ctrl_blk_p->card_infos[0];
    i = 0; /* Re-use variable i */
  	rvf_send_trace("MC_cmd_update_acq try ocr",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);



    if(retval == RV_OK)
    {
     /*GET OCR */
     //get the OCR and store it into card info
     rvf_send_trace("calling CMD58",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  	 result=send_ocr(&card_info->ocr);

    }


  // send_status(NULL);
   if(retval == RV_OK)
   {
    rvf_send_trace("MC CMD58 response received",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      /** Get CID */
     /**call cmd10 to get CID*/
       rvf_send_trace("calling CMD10",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
   retval=mc_cmd_send_cid(&card_info->cid[0]);//CMD10
   }
  // send_status(NULL);
   /** wait a while to transfer to Identification state*/
   //rvf_delay(RVF_MS_TO_TICKS(3));

   /** retreive the CSD register*/
   /*Now retreive the CSD*/
   if (retval == RV_OK)
   {
   /** get CSD parameter */
   rvf_send_trace("calling CMD9",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
   retval = mc_cmd_send_csd(&card_info->csd[0]);
  // mc_unpack_csd(&card_info->csd[0]);
   }

    /** perform latest update of the card administration*/
    if(retval == RV_OK)
    {
      /** card has been initialised succesfull*/
      mc_env_ctrl_blk_p->reserved_card_id |= 1 << 0;
    retval= send_status(NULL);
	*(SYS_UWORD32*)SPI_REG_SET1=0x00000003; // Pre-scaler of 2 // 24MHz
	set_blocklen(512,NULL);
	send_status(NULL);

    }
    else
    {
      /** card has not been initialised succesfull*/
      /** remove used card_info buffer*/
      MC_FREE_BUF(mc_env_ctrl_blk_p->card_infos[0]);
    }

    if(retval == RV_OK)
      {
      /** Card has been indentified and initialised so store this card*/
      /** With SD there is only 1 card possibble*/
      card_info->card_type= SD_CARD;
      mc_env_ctrl_blk_p->conf.dw = MC_CONF_BUS_WIDTH_4;
      mc_env_ctrl_blk_p->initialised=TRUE;//added for locosto  if card initialsation complete
    	  									  //then updated in stack
      card_info->phys_rca=0x0001;// AS RCA NOT SUPPORTED IN SPI MODE PASSING A
           						  //DUMMY ADDRESS PARAMETER
      }
     /** SD card search is done !!!!!*/

  	}
  else
  {
	rvf_send_trace("SD Card Search is Failed",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
	AI_ResetBit(MC_GPIO_1);//Taking GPIO_1 low turns off the 2.8V regulator
  }


  return retval;
   }

  /************************************************************************/
  /************************************************************************/
  T_RV_RET mc_cmd_init_card(void)
  {

     T_MC_OCR ret_ocr  = 0;
     T_MC_OCR ocr = 0;
     T_RV_RET retval  = RV_OK;
     T_RV_RET   result = RV_OK;
     UINT16   i = 1;
     T_MC_CARD_INFO *card_info = NULL;
     T_RVF_MB_ID data_p=NULL;
     const UINT32 addr = 0x0000;
     T_MC_RCA rca = 0x0001;
     T_MC_SUBSCRIBER subscriber = 0;
     UINT32 data_size = 512;
     /** Fill the data block with dummy data */
     T_RVF_MB_STATUS	mb_status;
     UINT8* msg_ptr;
     UINT8 resp;
     UINT16 arr[515]={0};
     int k=0;
     arr[0]=0x00;
     arr[1]=0x00;
     for(k=2;k<514;k++)
     	{
     	arr[k]=0xBB;
     	}
     arr[514]=0xFE;
     //rvf_get_buf (data_p,data_size,(void **)&msg_ptr);
     //memset(msg_ptr,0xAA, data_size);

     result=set_blocklen(data_size,NULL);
     if(result==RV_OK)
     	{
      rvf_send_trace("set_blocklen response received",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     	}
     else
     	{
        rvf_send_trace("set_blocklen response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     	return result;
     	}
     result=write_single_block(addr,NULL);
     if(result==RV_OK)
     	{
      rvf_send_trace("write_single_block response received",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     	}
     else
     	{
      rvf_send_trace("write_single_block response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  	  return result;
     	}
     // Data_Token_Create(msg_ptr,data_size,1);

      Spi_Mc_Write(&arr[514],data_size+3);
       // Spi_Mc_Write(msg_ptr,data_size);
     // rvf_free_buf(data_ptr);
      resp=Spi_Mc_Data_Read(515,&arr[0]);

     result=send_status(NULL);

     if(result==RV_OK)
     	{
      rvf_send_trace("send_status response received",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     	}
     else
     	{
      rvf_send_trace("send_status response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  	return result;
     	}
      retval = erase_wr_blk_start(addr,NULL);
  	if(result==RV_OK)
     	{
      rvf_send_trace("erase_wr_blk_start response received",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     	}
     else
     	{
      rvf_send_trace("erase_wr_blk_start response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  	return result;
     	}

     retval = erase_wr_blk_end(addr,NULL);
     if(result==RV_OK)
     	{
      rvf_send_trace("erase_wr_blk_end response received",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     	}
     else
     	{
      rvf_send_trace("erase_wr_blk_end response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  	return result;
     	}

     retval = erase(NULL);
     if(result==RV_OK)
     	{
      rvf_send_trace("erase response received",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     	}
     else
     	{
      rvf_send_trace("erase_wr_blk_end response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  	return result;
     	}
    #if 0
  	  else
        {
          msg_p = rtest_wait_for_message(0);
          if (msg_p->msg_id != MC_WRITE_RSP_MSG)
          {
            /* Received an unexpected message */
            RV_TEST_TRACE_ERROR("MC Test: Unexpected message (expected MC_WRITE_RSP_MSG)");
            result = TEST_FAILED;

          } else {

            T_MC_WRITE_RSP_MSG *rsp = (T_MC_WRITE_RSP_MSG *) msg_p;

            RV_TEST_TRACE_WARNING("MC Test: MC_WRITE_RSP_MSG");
            RV_TEST_TRACE_WARNING_PARAM("MC Test: result", rsp->result);
            RV_TEST_TRACE_WARNING_PARAM("MC Test: card status", rsp->card_status);
            RV_TEST_TRACE_WARNING_PARAM("MC Test: data_size", rsp->data_size);

            if (rsp->result != RV_OK)
            {
              RV_TEST_TRACE_ERROR("MC Test: Write data failed");
              result = TEST_FAILED;
            }
          }
          /** Free message buffer */
          rvf_free_msg(msg_p);
        }
        /** Free data buffer */
        rvf_free_buf(data_p);
      }
     	}
    retval = mc_cmd_send_cid(&card_info->csd[0]);
  return retval;
  #endif
  return result;
 }






/************************************************************************************/



/**
 * Identification
 *
 * Detailled description.
 * Sends all cards to idle-state and starts a new identification cycle
 * using the acquisition sequence.
 *
 * @return  RV_OK
 */
T_RV_RET mc_cmd_init_stack(void)
{
  T_RV_RET retval = RV_OK;

  MC_SEND_TRACE("MC commands: mc_cmd_init_stack called",RV_TRACE_LEVEL_DEBUG_LOW);
  retval = mc_cmd_update_acq();

  if (retval == RV_OK)
  {
   MC_SEND_TRACE("MC:Success, updating stack complete",RV_TRACE_LEVEL_ERROR);
  }
  else
  {
   MC_SEND_TRACE("MC: Error, updating stack failed",RV_TRACE_LEVEL_ERROR);
  }
  return retval;
}

/**
 * check stack
 *
 * Detailled description.
 * Tries to read the CSD of every active card in the stack. As a result, the
 * card management information in the controller is updated.
 *
 * @return  RV_OK
 */
//T_RV_RET mc_cmd_check_stack(T_MC_RCA *rca_p, UINT8 *stack_size)
T_RV_RET mc_cmd_check_stack(UINT8 *stack_size)
{
  UINT8 i, new_stack_size = 0;

  MC_SEND_TRACE("MC commands: mc_cmd_check_stack called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* Check the availability of the cards on stack */
  for (i = 0; i < *stack_size && i < MC_MAX_STACK_SIZE; i++)
  {
    if (send_csd(NULL) == RV_OK)
    {
      //rca_p[new_stack_size++] = rca_p[i];
    }
  }
  /* Update new stack size */
  *stack_size = new_stack_size;

  return RV_OK;
}

/**
 * setup card
 *
 * Detailled description.
 * Select a card by writing the RCA and reads its CSD.
 *
 * @return  RV_OK
 */

//T_RV_RET mc_cmd_setup_card(T_MC_RCA rca, UINT8 *csd_p, UINT32 *card_stat_p)
T_RV_RET mc_cmd_setup_card(UINT8 *csd_p, UINT32 *card_stat_p)
{
  T_RV_RET retval=RV_OK;
  UINT32* status;
  MC_SEND_TRACE("MC commands: mc_cmd_setup_card called",RV_TRACE_LEVEL_DEBUG_LOW);

  retval=send_status(NULL);
  if(retval==RV_OK)
  	{

  /* Send CSD */
  retval = send_csd(csd_p);

		  if (retval == RV_OK)
		  {
		   MC_SEND_TRACE("MC commands: send_csd",RV_TRACE_LEVEL_DEBUG_LOW);
		  }
		  else
		  {
		   MC_SEND_TRACE("MC commands: send_csd error",RV_TRACE_LEVEL_DEBUG_LOW);
		   retval=RV_INTERNAL_ERR;
		   return retval;
		  }
  	}

  // do{
         retval = send_status(NULL);
   //	 }while(status!=0x0000);

   /** Send card status */
   *card_stat_p=0x0000;

 return retval;
}




/**
 * deselect card
 *
 * Detailled description.
 * Deselect a card by writing the RCA and reads its CSD.
 *
 * @return  RV_OK
 */
T_RV_RET mc_cmd_deselect_card(UINT32 *card_stat_p)
{
  MC_SEND_TRACE("MC commands: mc_cmd_deselect_card called",RV_TRACE_LEVEL_DEBUG_LOW);

  /* Select card by RCA number (CMD 7) */
  return   RV_OK;// deselect_card(card_stat_p);
}

/**
 * stream read
 *
 * Detailled description.
 * Sets the start address and reads a continuous stream of data from the
 * card.
 *
 * @return  RV_OK
 */
T_RV_RET mc_cmd_stream_read(BOOL enable_dma, UINT16 nblk,
                             UINT16 blk_len, UINT32 addr,
                             UINT8 *data_p, UINT32 data_size,
                             UINT32 *card_stat_p)
{
  UINT32 i = 0;
  T_RV_RET retval=RV_OK;
  UINT16 data_16;
  UINT16 retevent = 0;

  MC_SEND_TRACE("MC commands: mc_cmd_stream_read called NOT SUPPORTED ",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  return retval;
}
/**********************************************************************************************************//**
 * read block
 *
 * Detailled description.
 * Sets the block length and the starting address and reads a data block from
 * the card.
 *
 * @return  RV_OK
 */
/*ADDED FOR LOCOSTO*/



T_RV_RET mc_cmd_read_block(BOOL enable_dma,UINT16 blk_len,
                            UINT32 addr, UINT8 *data_p,
                            UINT32 data_size, UINT32 *card_stat_p)
{
  UINT32 no_of_blocks=0;
  T_RV_RET retval = RV_OK;
  UINT16 i=0;
  UINT16 data_resp;
 T_RVF_MB_STATUS	mb_status;
 T_RVF_MB_ID data_p1=NULL;


  MC_SEND_TRACE("MC commands: mc_cmd_read_mblock_non_dma called",RV_TRACE_LEVEL_DEBUG_LOW);
  /*call set_blocklen command*/

  retval = read_single_block(addr,card_stat_p);//CMD25
  if(retval==RV_OK)
   	{
     MC_SEND_TRACE("single_block complete", RV_TRACE_LEVEL_DEBUG_LOW);
  	}
  else
  	{
	 MC_SEND_TRACE("single_block error", RV_TRACE_LEVEL_DEBUG_LOW);
	 return RV_INTERNAL_ERR;
	}

	AI_ResetBit(MC_GPIO_26);
	if(data_p!=NULL)
	{
		Spi_Mc_Init_Read(data_p,data_size,2);
	}

	AI_SetBit(MC_GPIO_26);

 return retval;
}
/**************************************************************************************************************/


/**
 * read multiple blocks
 *
 * Detailled description.
 * Sets the block length and the starting address and reads (continuously) data
 * blocks from the card. Data transfer is terminated by a stop command.
 *
 * @return  RV_OK
 */
T_RV_RET mc_cmd_read_mblock(T_MC_DMA_MODE enable_dma, UINT16 nblk,
                             UINT16 blk_len, UINT32 addr,
                             UINT8 *data_p, UINT32 data_size,
                             UINT32 *card_stat_p)
{
  T_RV_RET retval = RV_OK;


//#ifdef MC_DMA_USED
/*Check if DMA transfer is used or not*/
//  if(((enable_dma == MC_DMA_AUTO) && (data_size > MC_DMA_AUTO_THRESHOLD))
//     ||(enable_dma == MC_FORCE_DMA))
//  {
//      retval = mc_cmd_read_mblock_dma(nblk,blk_len,addr, data_p, data_size, card_stat_p);//DMA MODE
//  }
//  else
 // {
//     retval = mc_cmd_read_mblock_non_dma(nblk,blk_len,addr, data_p, data_size, card_stat_p);//NON DMA
//  }
//#else
  retval = mc_cmd_read_mblock_non_dma(nblk,blk_len,addr, data_p, data_size, card_stat_p);//NON DMA
//#endif
  return retval;
}

/**
 * read multiple blocks
 *
 * Detailled description.
 * Sets the block length and the starting address and reads (continuously) data
 * blocks from the card. Data transfer is terminated by a stop command.
 *
 * @return  RV_OK
 */
static T_RV_RET mc_cmd_read_mblock_non_dma(UINT16 nblk,
                             UINT16 blk_len, UINT32 addr,
                             UINT8 *data_p, UINT32 data_size,
                             UINT32 *card_stat_p)
{
  UINT32 no_of_blocks=0;
  T_RV_RET retval = RV_OK;
  UINT16 i=0;
  UINT16 no_of_odd_bytes_to_read=0;
   UINT16 data_resp;
   T_RVF_MB_STATUS	mb_status;
  T_RVF_MB_ID data_p1=NULL;


  MC_SEND_TRACE("MC commands: mc_cmd_read_mblock_non_dma called",RV_TRACE_LEVEL_DEBUG_LOW);

  retval = read_multiple_block(addr,card_stat_p);//CMD25
  if(retval==RV_OK)
   	{
     MC_SEND_TRACE("write_multiple_block complete", RV_TRACE_LEVEL_DEBUG_LOW);
  	}
  else
  	{
	 MC_SEND_TRACE("write_multiple_block error", RV_TRACE_LEVEL_DEBUG_LOW);
	 return RV_INTERNAL_ERR;
	}
	AI_ResetBit(MC_GPIO_26);
   	no_of_blocks= data_size/blk_len;
   	no_of_odd_bytes_to_read=data_size%blk_len;
   MC_TRACE_WARNING_PARAM("no_of_blocks",	no_of_blocks);

   	for(i=0;i<no_of_blocks;i++)
   	{
    	if(data_p!=NULL)
   	{
     Spi_Mc_Init_Read(data_p,blk_len,2);
       	data_p+=blk_len;
	}
	}

	if(no_of_odd_bytes_to_read)
	{
	Spi_Mc_Data_Block_Read(no_of_odd_bytes_to_read, data_p);
	}

stop_transmission(TRUE, card_stat_p);


AI_SetBit(MC_GPIO_26);
  return retval;
}




/************************************************************************************************/
/**
 * write block
 *
 * Detailled description.
 * Sets the block length and the starting address and writes a data block
 * to the card.
 *
 * @return  RV_OK
 */
T_RV_RET mc_cmd_write_block(BOOL enable_dma, UINT16 blk_len, UINT32 addr,
                             UINT8 *data_p, UINT32 data_size,
                             UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  UINT8 data_resp;
  UINT16 i, numberfifotransfer;
  UINT16 resp, data_16=0;
  UINT32 j=0;
  UINT16 irq;
  UINT16 arr[515]={0};
  int k=0;
  MC_SEND_TRACE("MC commands: mc_cmd_write_block called",RV_TRACE_LEVEL_DEBUG_LOW);

  /*Call Single Block write command */
  retval = write_single_block(addr, card_stat_p); //CMD24

  /* check return value of CMD24 for error*/
  if(retval== RV_OK)

  	     {
         MC_SEND_TRACE("write_single_block command complete", RV_TRACE_LEVEL_DEBUG_LOW);
  		 }
  else
  		 {
	 	 MC_SEND_TRACE("write_single_block error", RV_TRACE_LEVEL_DEBUG_LOW);
	 	 return RV_INTERNAL_ERR;
	 	 //retval=RV_OK;
	 	 }
	 AI_ResetBit(MC_GPIO_26);
   /* Creation of data token*/

       Spi_Mc_MegaWrite(data_p,blk_len,2);

/************************DATA TOKEN WRITE COMPLETE ********************************/

    /* When we wil get a data response token we put it to data_resp */

	data_resp = Spi_Mc_Init_Read(NULL,1,3);

	 if((data_resp&0xFF)==0xE5)
      	{
//      	 rvf_send_trace("data written sucessfully",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
    	 Spi_Mc_Busy_Read();
   		 if(data_token!=NULL)
   	  	 rvf_free_buf(data_token);
     	 }
     else
        {
		 rvf_send_trace("data not written sucessfully",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
		 if(data_token!=NULL)
         rvf_free_buf(data_token);

  	   if((data_resp&0xB)==0xB)
  		 {
         	rvf_send_trace("data write Error:CRC Error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
        	return RV_INTERNAL_ERR;
  		 }

  	   if((data_resp&0xD)==0xD)
  		 {
        	rvf_send_trace("data write Error:Write Error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
        	return RV_INTERNAL_ERR;
  		 }
        }
   AI_SetBit(MC_GPIO_26);
   return retval;

}



/**
 * write multiple blocks
 *
 * Detailled description.
 * Sets the block length and the starting address and writes (continuously)
 * data blocks to the card. Data transfer is terminated by a stop command.
 *
 * @return  RV_OK
 */
T_RV_RET mc_cmd_write_mblock(T_MC_DMA_MODE enable_dma, UINT16 nblk,
                              UINT16 blk_len, UINT32 addr,
                              UINT8 *data_p, UINT32 data_size,
                              UINT32 *card_stat_p)
{
  T_RV_RET retval = RV_OK;

//#ifdef MC_DMA_USED
  /*Check if DMA transfer is used or not*/
/*  if(((enable_dma == MC_DMA_AUTO) && (data_size > MC_DMA_AUTO_THRESHOLD))
     ||(enable_dma == MC_FORCE_DMA))
  {
      retval = mc_cmd_write_mblock_dma(nblk,blk_len,addr, data_p, data_size, card_stat_p);
  }
  else
  {
     retval = mc_cmd_write_mblock_non_dma(nblk,blk_len,addr, data_p, data_size, card_stat_p);
  }
#else
*/
     retval = mc_cmd_write_mblock_non_dma(nblk,blk_len,addr, data_p, data_size, card_stat_p);
//#endif
  return retval;
}
/*****************************************************************************************************/
/**
 * write multiple blocks
 *
 * Detailled description.
 * Sets the block length and the starting address and writes (continuously)
 * data blocks to the card. Data transfer is terminated by a stop command.
 *
 * @return  RV_OK
 */
/******************************************************************************************************/
static T_RV_RET mc_cmd_write_mblock_non_dma(UINT16  nblk,
                              UINT16 blk_len, UINT32 addr,
                              UINT8 *data_p, UINT32 data_size,
                              UINT32 *card_stat_p)
{
  UINT32 no_of_blocks=0;
  T_RV_RET retval = RV_OK;
  UINT16 i=0;
  UINT8 j=0xFD;
  UINT8 k;
  UINT16 l;
 UINT16 no_of_odd_bytes_to_write=0;
  UINT8 data_resp;
 T_RVF_MB_STATUS	mb_status;
 T_RVF_MB_ID data_p1=NULL;


  MC_SEND_TRACE("MC commands: mc_cmd_write_mblock_non_dma called",RV_TRACE_LEVEL_DEBUG_LOW);

  retval = write_multiple_block(addr,card_stat_p);//CMD25
  if(retval==RV_OK)
   	{
     MC_SEND_TRACE("write_multiple_block complete", RV_TRACE_LEVEL_DEBUG_LOW);
  	}
  else
  	{
	 MC_SEND_TRACE("WRITE_MULTIPLE_BLOCK COMMAND RESPONSE ERROR", RV_TRACE_LEVEL_DEBUG_LOW);
	 return RV_INTERNAL_ERR;
	 //retval=RV_OK;
	}
	no_of_blocks= data_size/blk_len;
	no_of_odd_bytes_to_write=data_size%blk_len;
	AI_ResetBit(MC_GPIO_26);
	 for(i=0;i<no_of_blocks;i++)
	{

		Spi_Mc_MegaWrite(data_p,blk_len,3);
		data_resp = Spi_Mc_Init_Read(NULL,1,3);
	 if((data_resp&0xFF)==0xE5)
	    {
//         rvf_send_trace("data written sucessfully",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
         if(data_token!=NULL)
         {
		   rvf_free_buf(data_token);
		 }
         data_p+=blk_len;
 		 Spi_Mc_Busy_Read();
		}
      else
       {
		 rvf_send_trace("data not written sucessfully",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
		if(data_token!=NULL)
		rvf_free_buf(data_token);

		if((data_resp&0xB)==0xB)
     		{

         	rvf_send_trace("data write Error:CRC Error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
        	//return RV_INTERNAL_ERR;
        	return RV_MEMORY_REMAINING;
     		}

     	if((data_resp&0xD)==0xD)
     		{
         	rvf_send_trace("data write Error:Write Error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       		//return RV_INTERNAL_ERR;
       		return RV_MEMORY_REMAINING;
     		}

     		//return RV_INTERNAL_ERR;
     		return RV_MEMORY_REMAINING;
		}

	}


	if(no_of_odd_bytes_to_write)
	{
	memset(&data_token_mul[0],0x00,blk_len+3);
	Data_Token_Create(data_p,no_of_odd_bytes_to_write,2);
	 data_resp = Spi_Mc_Block_Write(data_token_temp,blk_len+1);
	 if(((data_resp&0x00FF)==0x00E5))//||((data_resp&0xFF00)==0xE500))
	    {
//         rvf_send_trace("data written sucessfully",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
         if(data_token!=NULL)
         rvf_free_buf(data_token);
		 Spi_Mc_Busy_Read();
		}
	 else
       {
		 rvf_send_trace("data not written sucessfully",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
		if(data_token!=NULL)
		rvf_free_buf(data_token);
	   }

	}
	//sending STOP TOKEN for ending Multiple Write Block
	Spi_Mc_MegaWrite(&j,1,1);
	Spi_Mc_Busy_Read();

  AI_SetBit(MC_GPIO_26);
  return retval;


}


/**
 * erase group
 *
 * Detailled description.
 * Erases a range of erase groups on the card.
 *
 * @return  RV_OK
 */
//T_RV_RET mc_cmd_erase_group(T_MC_RCA rca, T_MC_CARD_TYPE card_type,UINT32 start_addr,UINT32 end_addr,UINT32 *card_stat_p)
T_RV_RET mc_cmd_erase_group(T_MC_CARD_TYPE card_type,UINT32 start_addr,UINT32 end_addr,UINT32 *card_stat_p)
{
  T_RV_RET retval = RV_OK;

  MC_SEND_TRACE("MC commands: mc_cmd_erase_group not supported in SPI MODE",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  retval=send_status(NULL);
  if (retval !=RV_OK)
    {
    return retval;
    }
    /**
      * Call Erase start
      */
    retval = erase_wr_blk_start(start_addr,NULL);

    /**
      * Call Erase end
      */
  if (retval!= RV_OK)
      {
      return retval;
      }
  retval  = erase_wr_blk_end(end_addr,NULL);

    /**
      * Call Erase group
      */
  if (retval!= RV_OK)
      {
      return retval;
      }
  retval = erase(card_stat_p);
    /**
      * Call Erase group
      */
  if (retval!= RV_OK)
      {
      return retval;
      }
  retval = send_status(NULL);
    /**
      * Call Erase group
      */
  if (retval!= RV_OK)
      {
      return retval;
      }
  return retval;
}

/**
 * stream write
 *
 * Detailled description.
 * Sets the start address and writes a continuous stream of data to the
 * card.
 *
 * @return  RV_OK
 */
T_RV_RET mc_cmd_stream_write(BOOL enable_dma, UINT16 nblk,
                              UINT16 blk_len, UINT32 addr,
                              UINT8 *data_p, UINT32 data_size,
                              UINT32 *card_stat_p)
{
  UINT32 i = 0;
  T_RV_RET retval=RV_OK;
  UINT16 data_16;

  MC_SEND_TRACE("MC commands: mc_cmd_stream_write not supported in SP mode",RV_TRACE_LEVEL_DEBUG_LOW);

  return retval;
}

T_RV_RET mc_cmd_send_controller_status(UINT16 *status)
{
  MC_SEND_TRACE("MC commands: get_controller_status called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  /* Assign status register value to parameter */
  *status = 0;//MC_STAT;     //

  return RV_OK;
}

T_RV_RET mc_cmd_reset(void)
{
  MC_SEND_TRACE("MC commands: mc_cmd_reset called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  return go_idle_state();
}


//T_RV_RET mc_cmd_send_csd (T_MC_RCA rca, UINT8 *csd_p)
T_RV_RET mc_cmd_send_csd (UINT8 *csd_p)
{
  UINT32 status;
  MC_SEND_TRACE("MC commands: mc_cmd_send_csd called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  //MC_SEND_TRACE_PARAM("MC mc_cmd_send_csd: rca", rca,RV_TRACE_LEVEL_DEBUG_LOW);

 return send_csd(csd_p);
}

T_RV_RET mc_cmd_send_cid (UINT8 *cid_p)

//T_RV_RET mc_cmd_send_cid (T_MC_RCA rca,UINT8 *cid_p)
{
  MC_SEND_TRACE("MC commands: mc_cmd_send_cid called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
//  send_status(NULL);
  return send_cid(cid_p);
}

T_RV_RET mc_cmd_send_status (UINT32 *card_stat_p)
{
  MC_SEND_TRACE("MC commands: mc_cmd_send_status called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  return send_status(card_stat_p);
}

T_RV_RET mc_cmd_write_csd (UINT8 *csd_p,
                            T_MC_CSD_FIELD field,
                            UINT8 value,
                            UINT32 *card_stat_p)
{
  T_RV_RET retval = RV_OK;
  UINT16   resp,csd_prog, data_16, k = 0;
  UINT8    count=0,crc7;

  MC_SEND_TRACE("MC commands: mc_cmd_write_csd called",
                  RV_TRACE_LEVEL_DEBUG_LOW);


    /**
     * copy programmable part in uint16
     */
    csd_prog = (csd_p[0] | (csd_p[1] << 8));

    /** Program field */
    switch (field)
    {
      case CSD_FIELD_FILE_FORMAT_GRP:
        csd_prog &= ~(0x8000);  /*clear old value */
        csd_prog |= ((value & 0x1) << 15);

        break;

      case CSD_FIELD_COPY:
        csd_prog &= ~(0x4000);  /*clear old value */
        csd_prog |= ((value & 0x1) << 14);

        break;

      case CSD_FIELD_PERM_WRITE_PROTECT:
        csd_prog &= ~(0x2000);  /*clear old value */
        csd_prog |= ((value & 0x1) << 13);

        break;

      case CSD_FIELD_TMP_WRITE_PROTECT:
        csd_prog &= ~(0x1000);  /*clear old value */
        csd_prog |= ((value & 0x1) << 12);

        break;

      case CSD_FIELD_FILE_FORMAT:
        csd_prog &= ~(0x0C00);  /*clear old value */
        csd_prog |= ((value & 0x3) << 10);

        break;

      case CSD_FIELD_FILE_ECC:
        csd_prog &= ~(0x0300);  /*clear old value */
        csd_prog |= ((value & 0x3) << 8);

        break;

      case CSD_FIELD_FILE_CRC: //useless
        break;

     default:
        MC_SEND_TRACE("MC mc_cmd_write_csd: Invalid field",
                        RV_TRACE_LEVEL_ERROR);
        retval = RV_INVALID_PARAMETER;

    }


      /** Assign programmable part to CSD */
      csd_p[1] = ((UINT8) (csd_prog >> 8));

      /** Calculate CRC7 */
      crc7     = get_CRC7(&csd_p[0]);
      csd_p[0] = ((UINT8) (crc7 << 1) | 1);

      /*Send write CSD command */
      retval = program_csd(card_stat_p);


      /** Write CSD to data buffer */
      csd_p+=15;
      while (k < MC_CSD_SIZE)
      {
        /** Data bus is 16 bits wide */
        data_16 = *csd_p;
        csd_p--;
        k++;
        data_16 |= (((UINT16)(*csd_p)) << 8);
        csd_p--;
        k++;
      }
	  /* calling CMD13 to check status of card and if busy will wait*/

  MC_SEND_TRACE_PARAM("MC mc_cmd_write_csd: exit with status:", retval,
                        RV_TRACE_LEVEL_DEBUG_LOW);
  return retval;
}

/**
 * clear write protect
 *
 * Detailled description.
 * Clears write protection of a addressed write protect group
 *
 * @return  RV_OK
 */
//T_RV_RET mc_cmd_clr_write_protect(UINT16 rca,
//                                   UINT32 wr_prot_group,
//                                   UINT32 *card_stat_p)
T_RV_RET mc_cmd_clr_write_protect( UINT32 wr_prot_group,UINT32 *card_stat_p)
{
  T_RV_RET retval = RV_OK;

  MC_SEND_TRACE("MC commands: mc_cmd_clr_write_protect called",RV_TRACE_LEVEL_DEBUG_LOW);

  return retval;
}

/**
 * get write protect
 *
 * Detailled description.
 * Clears write protection of a addressed write protect group
 *
 * @return  RV_OK
 */
//T_RV_RET mc_cmd_get_write_protect(UINT16 rca,
//                                   UINT16 blk_len,
//                                   UINT32 wr_prot_group,
//                                   UINT32 *wp_prot_grps,
//                                   UINT32 *card_stat_p)
T_RV_RET mc_cmd_get_write_protect(
                                   UINT16 blk_len,
                                   UINT32 wr_prot_group,
                                   UINT32 *wp_prot_grps,
                                   UINT32 *card_stat_p)
{
  T_RV_RET retval = RV_OK;
  UINT16 data_16;
  UINT16 i;
  UINT16 received_event;


  MC_SEND_TRACE("MC commands: mc_cmd_get_write_protect called",
                  RV_TRACE_LEVEL_DEBUG_LOW);


  retval =  send_write_prot(wr_prot_group, card_stat_p);//CMD30

  if (retval == RV_OK)
	  {
	    MC_SEND_TRACE("MC mc_cmd_get_write_protect: Data received from card",RV_TRACE_LEVEL_DEBUG_LOW);
	  }
  else
	  {
	    MC_SEND_TRACE("MC mc_cmd_get_write_protect: Data not received from card",RV_TRACE_LEVEL_DEBUG_LOW);
	    retval=RV_INTERNAL_ERR;
	  }


  return retval;
}

/**
 * set write protect
 *
 * Detailled description.
 * Set write protection of a addressed write protect group
 *
 * @return  RV_OK
 */
//T_RV_RET mc_cmd_set_write_protect(UINT16 rca,
//                                   UINT32 wr_prot_group,
//                                   UINT32 *card_stat_p)
T_RV_RET mc_cmd_set_write_protect( UINT32 wr_prot_group,
                                   UINT32 *card_stat_p)
{
  T_RV_RET retval = RV_OK;

  MC_SEND_TRACE("MC commands: mc_cmd_set_write_protect called",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  if (retval == RV_OK)
  {
    retval =  set_write_prot(wr_prot_group, card_stat_p); //CMD28
  }

  return retval;
}
/*********************************************************************************************/
/**
 * get hardware version
 *
 * Detailled description.
 * returns the hardware version
 *
 * @return  hardware version
 */
//added for locosto
#if 0
UINT16 mc_cmd_get_hw_version(void)
{
  //UINT8 version;
  MC_SEND_TRACE("MC commands: mc_cmd_get_hw_version called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
 // version=*(volatile SYS_UWORD32*)MEM_SPI;
  return *(volatile UINT16*)MEM_SPI;//(UINT16)mc_env_ctrl_blk_p->card_infos[0]->cid[15];
}
#endif
/*********************************************************************************************/
/**
 * get hardware version
 *
 * Detailled description.
 * returns the hardware version
 *
 * @return  hardware version
 */
UINT16 mc_cmd_get_hw_version(void)
{
  MC_SEND_TRACE("MC commands: mc_cmd_get_hw_version called",
                  RV_TRACE_LEVEL_DEBUG_LOW);
  return 0;//MC_REV;
}

/**
 * retreives the SD status register
 *
 *
 * @return  RV_OK
 */
//T_RV_RET mc_cmd_sd_send_status(UINT16  rca,
//                                UINT8   *sd_status_p,
//                                UINT32  *card_stat_p)
T_RV_RET mc_cmd_sd_send_status(UINT8   *sd_status_p,UINT32  *card_stat_p)
{
  T_RV_RET retval = RV_OK;
  UINT16 data_16[32]={0};
  UINT16 i;
  UINT16 received_event;
  retval=app_cmd(NULL);
  if(retval==RV_OK)
  	{
     MC_SEND_TRACE("MC commands: app_cmd successful",
                  RV_TRACE_LEVEL_DEBUG_LOW);
    }
  else
  	{
     MC_SEND_TRACE("MC commands: app_cmd error",
                  RV_TRACE_LEVEL_DEBUG_LOW);

     return retval;
    }
  retval=sd_status(sd_status_p);
  if(retval==RV_OK)
  	{
     MC_SEND_TRACE("MC commands: app_cmd PASS!!!",
                  RV_TRACE_LEVEL_DEBUG_LOW);

  	}
  else
  	{
     MC_SEND_TRACE("MC commands: app_cmd FAIL!!!",
                  RV_TRACE_LEVEL_DEBUG_LOW);
     return retval;
  	}

  return retval;
}


/**
 * get scr register
 *
 * Detailled description.
 * Retreives the SCR register from a SD-card
 *
 * @return  RV_OK
 */
//T_RV_RET mc_cmd_send_scr(UINT16  rca,UINT8   *scr_p,UINT32  *card_stat_p)
T_RV_RET mc_cmd_send_scr(UINT8 *scr_p,UINT32  *card_stat_p)
{
  T_RV_RET retval = RV_OK;
  UINT16 data_16;
  UINT16 i;
  UINT16 received_event;


  /* SCR reg is 64 bits */
  /*
   *send ACMD13
   */
retval=app_cmd(NULL);//SPECIFYING THAT THE NEXT COMMAND IS AN ACD COMMAND
if(retval==RV_OK)
  {
      MC_SEND_TRACE("MC app_cmd: PASS!!!", RV_TRACE_LEVEL_DEBUG_LOW);
  }
else
  {
      MC_SEND_TRACE("MC app_cmd: Failure!!!", RV_TRACE_LEVEL_DEBUG_LOW);
   return retval;
  }

retval =  send_scr(scr_p);

if (retval == RV_OK)
  {
      MC_SEND_TRACE("MC mc_cmd_send_scr: Datareceived from card",
                        RV_TRACE_LEVEL_DEBUG_LOW);
  }
else
  {
      MC_SEND_TRACE("MC mc_cmd_send_scr: Failure!!!", RV_TRACE_LEVEL_DEBUG_LOW);
      retval = RV_INTERNAL_ERR;
  }

  return retval;
}

