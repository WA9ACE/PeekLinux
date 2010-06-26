/**
 * @file  mc_func.c
 *
 *
 * @author  ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/4/2003                    Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

/*
 * Note: This module will be build for target environment.
 */

#ifndef _WINDOWS
#include "memif/mem.h"
#endif
#include "mc/mc_i.h"
#include "mc/mc_commands.h"
#include "mc/mc_api.h"
#include "mc/board/mc_inth.h"
#include "mc/board/mc_func.h"
#include "spi_drv.h"
#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_ext_priorities.h"
#include "rvm/rvm_use_id_list.h"
#include "armio/Armio.h"
#define MC_MAX_ATTEMPTS    (15)
#define COMMAND_ATTEMPTS   (50)
static char mc_buf[100];

/********************************LOCOSTO SD SPECIFIC *************************************/
UINT8 cmdarg[4]={0};


/***************************************************************************************/

/** ADDED FOR CREATING CMD TOKEN FOR LOCOSTO SDCard **/
  // In this token first bit is the start bit which is set to 0
  // followed by Transmitter Bit which is set to 1 then followd
  // by 32 bit argument and 6 bit command index and then followed
  // by 7 bit CRC and ended by 1 bit end bit
/** ADDED FOR CREATING CMD TOKEN FOR LOCOSTO SDCard **/
UINT8 Cmd_Token[MC_CMD_SIZE]={0};
UINT8 CRC7;

UINT8* DATA_BYTE;
UINT16 NB,N;
UINT32 Data_Count=0;
//UINT8 CRC16[2];


void Cmd_Token_Create(UINT8 index)
{   /* initialize to 0*/
  Cmd_Token[5]=0;
  	Cmd_Token[4]=0;
  Cmd_Token[3]=0;
  Cmd_Token[2]=0;
  Cmd_Token[1]=0;


  /*transmit bit*/
  index&=0x3f;
  Cmd_Token[5]=CMD_TOKEN_TRANSMIT_BIT|index;//Command index
  	Cmd_Token[4]|=cmdarg[3];
  Cmd_Token[3]|=cmdarg[2];
  Cmd_Token[2]|=cmdarg[1];
  Cmd_Token[1]|=cmdarg[0];

  /*getting the CRC value*/

  CRC7=get_CRC7(Cmd_Token);//7 bit CRC
  Cmd_Token[0]=CRC7|1;//Cmd_Token_END_BIT;

}

/*****************************************************************************************/

/**
 * Send MC CMD0 command
 *
 * Detailled description.
 * This command executes CMD0. It resets all card states to idle.
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET go_idle_state(T_MC_CARD_TYPE card_type)
{
  T_RV_RET retval  = RV_OK;
  UINT16   attempt = 0;
  BOOL  complete;
  UINT8 resp;
  MC_SEND_TRACE("MC go_idle_state: Sending CMD0",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  /*COMMAND TOKEN CREATION*/
  Cmd_Token_Create(GO_IDLE_STATE);
  AI_SetBit(MC_GPIO_26);//CS HIGH
  spi_mc_preinitialisation(10);
  AI_ResetBit(MC_GPIO_26);
  rvf_send_trace("Spi_Mc_Write START go_idle_state",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
 do{
  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
  resp=Spi_Mc_Init_Read(NULL,1,0);

  attempt++;
 }while ((resp==0xFF)&&(attempt<500));// while ((resp != 0x01FF)&&(resp != 0xFF01)&&(attempt<500));
 sprintf(mc_buf,"  go_idle_state resp=%d   \n",resp);
 rvf_send_trace(mc_buf,(UINT8) (strlen (mc_buf)),NULL_PARAM, RV_TRACE_LEVEL_WARNING,MC_USE_ID);
  AI_SetBit(MC_GPIO_26);//CS HIGH
  return retval;
}


/**
 * Send MC CMD55 command
 *
 * Detailled description.
 * This command executes CMD55. It resets all card states to idle.
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET Send_App_Cmd()
{
  T_RV_RET retval  = RV_OK;
  UINT16   attempt = 0;
  BOOL  complete;
  UINT8 resp;
  MC_SEND_TRACE("MC go_idle_state: Sending CMD0",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  /*COMMAND TOKEN CREATION*/
  Cmd_Token_Create(SEND_APP_CMD);
  AI_ResetBit(MC_GPIO_26);
  do{
  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
  resp=Spi_Mc_Init_Read(NULL,1,0);
  attempt++;
   }while ((resp==0xFF)&&(attempt<10));
 //sprintf(mc_buf,"  Send_App_Cmd resp=%d   \n",resp);
 //rvf_send_trace(mc_buf,(UINT8) (strlen (mc_buf)),NULL_PARAM, RV_TRACE_LEVEL_WARNING,MC_USE_ID);
  AI_SetBit(MC_GPIO_26);//CS HIGH
  return retval;
}


/**
 * Send MC ACMD41 command
 *
 * Detailled description.
 * This command executes ACMD41. It resets all card states to idle.
 * response R1
 *
 * @return  RV_OK
 */
UINT16 Send_SD_Op_Cond()
{
  T_RV_RET retval  = RV_OK;
  UINT16   attempt = 0;
  BOOL  complete;
  UINT8 resp;
  MC_SEND_TRACE("MC go_idle_state: Sending CMD0",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  /*COMMAND TOKEN CREATION*/
  Cmd_Token_Create(SEND_SD_OP_COND);
  AI_ResetBit(MC_GPIO_26);
  //CHANGE FOR MAKING CS TO LOW DURING WRITE
  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
    resp=Spi_Mc_Init_Read(NULL,1,0);
  AI_SetBit(MC_GPIO_26);//CS HIGH
  return resp;
}


/**
 * Send MC CMD9 command
 *
 * Detailled description.
 * This command executes CMD9. It addressed card sends its card-specific
 * data (CSD) on the CMD line.
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET send_csd(UINT8 *csd_p)
{
  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT8 temp[20]={0};
  int i=0;

  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  Cmd_Token_Create(SEND_CSD);
  AI_ResetBit(MC_GPIO_26);

  do{

  	  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
      Spi_Mc_Init_Read(temp,20,1);
	  attempt++;
      }while( (temp[0] == 0xFF)&& (attempt<=MC_MAX_ATTEMPTS));
  if (temp[0] != 0x00)
     {
       rvf_send_trace("MC csd cmd response timeout",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       return retval=RV_INTERNAL_ERR;
     }
  else
     {
//   	rvf_send_trace("MC card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     }


	csd_p[ 0] = (UINT8)temp[17]; csd_p[ 1] =  (UINT8)temp[16];
	csd_p[ 2] = (UINT8)temp[15]; csd_p[ 3] =  (UINT8)temp[14];
	csd_p[ 4] = (UINT8)temp[13]; csd_p[ 5] =  (UINT8)temp[12];
	csd_p[ 6] = (UINT8)temp[11]; csd_p[ 7] =  (UINT8)temp[10];
	csd_p[ 8] = (UINT8)temp[9];  csd_p[ 9] =  (UINT8)temp[8];
	csd_p[10] = (UINT8)temp[7];  csd_p[11] =  (UINT8)temp[6];
	csd_p[12] = (UINT8)temp[5];  csd_p[13] =  (UINT8)temp[4];
	csd_p[14] = (UINT8)temp[3];  csd_p[15] =  (UINT8)temp[2];


AI_SetBit(MC_GPIO_26);
return retval;
}

/*************************************************************************************************************************/

/**
 * Send MC CMD10 command
 *
 * Detailled description.
 * This command executes CMD10. It addressed card sends its card identification
 * (CID) on CMD the line.
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET send_cid(UINT8 *cid_p)
{
  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT8 i;
  UINT8 cid_data[16];
  UINT8 temp[20]={0};
  MC_SEND_TRACE("MC send_cid: Sending CMD10",RV_TRACE_LEVEL_DEBUG_LOW);

		  cmdarg[3]=0;
		  cmdarg[2]=0;
		  cmdarg[1]=0;
		  cmdarg[0]=0;
		  Cmd_Token_Create(SEND_CID);
	 	  AI_ResetBit(MC_GPIO_26);

  do
     {

      Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
      Spi_Mc_Init_Read(temp,20,1);
	  attempt++;
      }while((temp[0] == 0xFF) && (attempt<=MC_MAX_ATTEMPTS));

  if (temp[0] != 0x00)
   {
    rvf_send_trace("MC cid cmd response timeout",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       return retval=RV_INTERNAL_ERR;
     }
  else
     {
//   	rvf_send_trace("MC card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     }

	 cid_p[ 0] = (UINT8)temp[17]; cid_p[ 1] =  (UINT8)temp[16];
	 cid_p[ 2] = (UINT8)temp[15]; cid_p[ 3] =  (UINT8)temp[14];
	 cid_p[ 4] = (UINT8)temp[13]; cid_p[ 5] =  (UINT8)temp[12];
	 cid_p[ 6] = (UINT8)temp[11]; cid_p[ 7] =  (UINT8)temp[10];
	 cid_p[ 8] = (UINT8)temp[9];  cid_p[ 9] =  (UINT8)temp[8];
	 cid_p[10] = (UINT8)temp[7];  cid_p[11] =  (UINT8)temp[6];
	 cid_p[12] = (UINT8)temp[5];  cid_p[13] =  (UINT8)temp[4];
	 cid_p[14] = (UINT8)temp[3];  cid_p[15] =  (UINT8)temp[2];

  AI_SetBit(MC_GPIO_26);
  return retval;
}
/*************************************************************************************************************************/

/*************************************************************************************************************************/
/**
 * Send MC CMD12 command
 *
 * Detailled description.
 * This command executes CMD12. It Terminates a read/write stream/multiple
 * block operation. When CMD12 is used to terminate a read transaction the
 * card will respond with R1b. When it is used to stop a write transaction
 * the card will respond with R1b.
 * response R1b
 *
 * @return  RV_OK
 */
T_RV_RET stop_transmission(BOOL stop_read, UINT32 *card_stat_p)
{
  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
   UINT8 resp;
  MC_SEND_TRACE("MC erase: Sending CMD12",RV_TRACE_LEVEL_DEBUG_LOW);
  		cmdarg[3]=0;
		cmdarg[2]=0;
		cmdarg[1]=0;
		cmdarg[0]=0;
//COMMAND TOKEN CREATION
Cmd_Token_Create(STOP_TRANSMISSION);
do{
  AI_ResetBit(MC_GPIO_26);//CS LOW
  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
  resp=Spi_Mc_Init_Read(NULL,1,0);
  if(resp==0)
  resp=Spi_Mc_Init_Read(NULL,1,0);
	attempt++;
}while ((resp==0xFF)&&(attempt<50));//while ((resp != 0x00FF)&&(resp != 0xFF00));

 //if resp==0x00 card is busy ..if resp==Non Zero value card is ready for next command.
   if (resp!=0x00)
   	 {
    //  rvf_send_trace("MC CMD12 card sent response received",36, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      retval = RV_INTERNAL_ERR;
      }
 else
   	 {
      rvf_send_trace("MC CMD12 response error ",36, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      }
 AI_SetBit(MC_GPIO_26);
 return retval;
}
/*************************************************************************************************************************/

/*************************************************************************************************************************/

/**
 * Send MC CMD13 command
 *
 * Detailled description.
 * This command executes CMD13. It addressed card sends its status
 *
 * response R2
 *
 * @return  RV_OK
 */
T_RV_RET send_status(UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
 UINT16 attempt = 0;
  UINT16 resp;
  UINT8 temp[2]={0};
  int i;
  MC_SEND_TRACE("MC send_status: Sending CMD13",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  //COMMAND TOKEN CREATION
  Cmd_Token_Create(SEND_STATUS);
  AI_ResetBit(MC_GPIO_26);//CS LOW

  do
     {
	  AI_ResetBit(MC_GPIO_26);
      Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
      Spi_Mc_Init_Read(temp,2,0);
	  attempt++;
      }while((temp[0] == 0xFF)&& (temp[1] == 0xFF) && (attempt<=MC_MAX_ATTEMPTS));
  if((temp[0] == 0x00)&&(temp[1] == 0x00))
		{

//		rvf_send_trace("MC send Status response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
		retval=RV_OK;
  		AI_SetBit(MC_GPIO_26);  //CS HIGH
        return retval;


		}
	else
		{
//	   rvf_send_trace("MC send Status response timeout",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       retval=RV_INTERNAL_ERR;
       AI_SetBit(MC_GPIO_26);  //CS HIGH
       return retval;
		}

}
/*************************************************************************************************************************/

/**
 * Send MC CMD16 command
 *
 * Detailled description.
 * This command executes CMD16. It sets the block length (in bytes) for all
 * following block commands (read and write). Default block length is
 * specified in the CSD.
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET set_blocklen(UINT32 blocklen,UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT8 resp;
  MC_SEND_TRACE("MC set_blocklen: Sending CMD16",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  cmdarg[3]=blocklen>>24;
  cmdarg[2]=blocklen>>16;
  cmdarg[1]=blocklen>>8;
  cmdarg[0]=blocklen;    //added for SD card for setting block length 512 bytes
  //COMMAND TOKEN CREATION
  do{
          AI_ResetBit(MC_GPIO_26);//CS LOW
    	  Cmd_Token_Create(SET_BLOCKLEN);
     	  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
  	    resp=Spi_Mc_Init_Read(NULL,1,0);
  			attempt++;
}while ((resp==0xFF)&&(attempt<50));
 if (resp!=0x00)
   	 {
	      rvf_send_trace("MC CMD16 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
	      retval = RV_INTERNAL_ERR;
     }
 else
   	 {
	      rvf_send_trace("MC CMD16 card sent response received ",40, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
		  retval=RV_OK;
     }
  if (card_stat_p != NULL)
     {
       	   /* Return (card state) */
    	   *card_stat_p =resp;
     }
 AI_SetBit(MC_GPIO_26);  //CS HIGH
 return retval;
}
/*************************************************************************************************************************/
/*************************************************************************************************************************/
/**
 * Send MC CMD17 command
 *
 * Detailled description.
 * This command executes CMD18. It continuously transfers data blocks from
 * card to host until interrupted by a stop command or the requested number
 * of data block transmitted
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET read_single_block(UINT32 data_addr, UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT8 resp;
  MC_SEND_TRACE("MC read_multiple_block: Sending CMD18",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  cmdarg[3]=data_addr>>24;
  cmdarg[2]=data_addr>>16;
  cmdarg[1]=data_addr>>8;
  cmdarg[0]=data_addr;
  //COMMAND TOKEN CREATION
  Cmd_Token_Create(READ_SINGLE_BLOCK);
  AI_ResetBit(MC_GPIO_26);//CS LOW
  do{
  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
  resp=Spi_Mc_Init_Read(NULL,1,0);
  	attempt++;
}while ((resp==0xFF)&&(attempt<COMMAND_ATTEMPTS));
 if (resp!=0x00)
       {
       rvf_send_trace("MC read_single_block CMD17 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       retval = RV_INTERNAL_ERR;
       }
  else
       {
//       rvf_send_trace("MC CMD17 card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
		      if (card_stat_p != NULL)
		      {
		        /* Return  (card response) */
		        *card_stat_p =resp;// NULL;
		      }
       }

  AI_SetBit(MC_GPIO_26);

  return retval;
}


/*************************************************************************************************************************/
/**
 * Send MC CMD18 command
 *
 * Detailled description.
 * This command executes CMD18. It continuously transfers data blocks from
 * card to host until interrupted by a stop command or the requested number
 * of data block transmitted
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET read_multiple_block(UINT32 data_addr, UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT8 resp;
  MC_SEND_TRACE("MC read_multiple_block: Sending CMD18",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  cmdarg[3]=data_addr>>24;
  cmdarg[2]=data_addr>>16;
  cmdarg[1]=data_addr>>8;
  cmdarg[0]=data_addr;
  //COMMAND TOKEN CREATION
  Cmd_Token_Create(READ_MULTIPLE_BLOCK);
  AI_ResetBit(MC_GPIO_26);//CS LOW
  do{
  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
  resp=Spi_Mc_Init_Read(NULL,1,0);
	attempt++;
}while ((resp==0xFF)&&(attempt<COMMAND_ATTEMPTS));
 if (resp!=0x00)       {
       rvf_send_trace("MC read_multiple_block CMD18 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       retval = RV_INTERNAL_ERR;
       }

  else
       {
//       rvf_send_trace("MC CMD18 card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
		      if (card_stat_p != NULL)
		      {
		        /* Return  (card response) */
		        *card_stat_p =resp;// NULL;
		      }
       }

  AI_SetBit(MC_GPIO_26);

  return retval;
}
 /*************************************************************************************************************************/
/**
 * Send MC CMD24 command
 *
 * Detailled description.
 * This command executes CMD24. It writes a block of the size selected by the
 * SET_BLOCKLEN command
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET write_single_block(UINT32 data_addr, UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  int attempt = 0;
  UINT8 resp=0;
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  cmdarg[3]=data_addr>>24;
  cmdarg[2]=data_addr>>16;
  cmdarg[1]=data_addr>>8;
  cmdarg[0]=data_addr;


	Cmd_Token_Create(WRITE_BLOCK);
	AI_ResetBit(MC_GPIO_26);//CS LOW
 do{
  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
  resp=Spi_Mc_Init_Read(NULL,1,0);
	attempt++;
}while ((resp==0xFF)&&(attempt<COMMAND_ATTEMPTS));
 if (resp!=0x00)
   	 {
      rvf_send_trace("MC write_single_block CMD24 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      retval = RV_INTERNAL_ERR;
      }
 else
   	 {
//      rvf_send_trace("MC CMD24 card sent response received ",40, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     }
   AI_SetBit(MC_GPIO_26);  //CS HIGH
   return retval;
}

/*************************************************************************************************************************/

/*************************************************************************************************************************/
/**
 * Send MC CMD25 command
 *
 * Detailled description.
 * This command executes CMD25. It continuously writes blocks of data until a
 * STOP_TRANSMISSION follows or the requested number of block received.
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET write_multiple_block(UINT32 data_addr, UINT32 *card_stat_p)
{

   T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT8 resp;
  MC_SEND_TRACE("MC write_multiple_block: Sending CMD25",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  cmdarg[3]=data_addr>>24;
  cmdarg[2]=data_addr>>16;
  cmdarg[1]=data_addr>>8;
  cmdarg[0]=data_addr;

  //COMMAND TOKEN CREATION
  Cmd_Token_Create(WRITE_MULTIPLE_BLOCK);
  AI_ResetBit(MC_GPIO_26);//CS LOW
 do{
  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
  resp=Spi_Mc_Init_Read(NULL,1,0);
	attempt++;
}while ((resp==0xFF)&&(attempt<COMMAND_ATTEMPTS));
 if (resp!=0x00)

       {
       rvf_send_trace("MC write_multiple_block CMD25 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       retval = RV_INTERNAL_ERR;
       }
  else
       {
//       rvf_send_trace("MC CMD25 card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       Data_Count++;
		      if (card_stat_p != NULL)
		      {
		        /* Return  (card response) */
		        *card_stat_p =resp;// NULL;
		      }
       }

  AI_SetBit(MC_GPIO_26);

  return retval;
}


/*************************************************************************************************************************/
/* Below Functions doesnt have much impact system functionality kept them for future implementation*/
/*************************************************************************************************************************/

/**
 * Send MC CMD27 command
 *
 * Detailled description.
 * This command executes CMD27. It programming of the programmable bits of
 * the CSD.
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET program_csd(UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT16 resp;
  MC_SEND_TRACE("MC program_csd: Sending CMD27",RV_TRACE_LEVEL_DEBUG_LOW);
  		  cmdarg[3]=0;
		  cmdarg[2]=0;
		  cmdarg[1]=0;
		  cmdarg[0]=0;
	      //COMMAND TOKEN CREATION
		  Cmd_Token_Create(PROGRAM_CSD);
		  AI_ResetBit(MC_GPIO_26);//CS LOW
 do{
 	      Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
 	      resp=Spi_Mc_Read();
		  attempt++;
 } while ( (resp != 0x00FF)&&(resp != 0xFF00) && (attempt<MC_MAX_ATTEMPTS));
 if((resp != 0x00FF)&&(resp != 0xFF00))
   	 {
      rvf_send_trace("MC CMD27 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      retval = RV_INTERNAL_ERR;
     }
 else
   	 {
      rvf_send_trace("MC CMD27 card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
     }
 if ((resp != 0x00FF)&&(resp != 0xFF00))
 	  {
 	    if (card_stat_p != NULL)
 	    {
 	      /* Return  (card response) */
 	      *card_stat_p =resp;
 	    }
 	  }
 else
 	  {
 	    MC_SEND_TRACE("MC program_csd: sending CMD27 failed",RV_TRACE_LEVEL_ERROR);
 	    retval = RV_INTERNAL_ERR;
 	  }
 retval=RV_OK;
 AI_SetBit(MC_GPIO_26);

 return retval;
}
/*************************************************************************************************************************/
/*************************************************************************************************************************/
/**
 * Send MC CMD38 command
 *
 * Detailled description.
 * This command executes CMD38. It erases all previously selected write
 * blocks
 * response R1b
 *
 * @return  RV_OK
 */
T_RV_RET erase(UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT16 resp;
  MC_SEND_TRACE("MC erase: Sending CMD38",RV_TRACE_LEVEL_DEBUG_LOW);
  		cmdarg[3]=0;
		cmdarg[2]=0;
		cmdarg[1]=0;
		cmdarg[0]=0;    //added for SD card for erasing
 do{
	//COMMAND TOKEN CREATION
	Cmd_Token_Create(ERASE);
	AI_ResetBit(MC_GPIO_26);//CS LOW
//    AI_SetBit(MC_GPIO_1);
	Spi_Mc_Write(Cmd_Token, MC_CMD_SIZE);
	resp=Spi_Mc_Read();
	//resp=Spi_Mc_Read();
	attempt++;
    }while((resp != 0x00FF)&&(resp != 0xFF00)&&(resp != 0xFFFF));

 if((resp != 0x00FF)&&(resp != 0xFF00)&&(resp != 0xFFFF))
   	 {
      rvf_send_trace("MC CMD38 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      retval = RV_INTERNAL_ERR;
      }
 else
   	 {
      rvf_send_trace("MC CMD38 card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
      }
 AI_SetBit(MC_GPIO_26);
 return retval;
}
/*************************************************************************************************************************/
/**
 * Send MC CMD28 command
 *
 * Detailled description.
 * This command executes CMD28. It if the card has write protection features,
 * this command sets the write protection bit of the addressed group. The
 * properties of write protection are coded in the card specific data
 * (WP_GRP_SIZE).
 * response R1b
 *
 * @return  RV_OK
 */
T_RV_RET set_write_prot(UINT32 data_addr, UINT32 *card_stat_p)
{
  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT16 resp;
  MC_SEND_TRACE("MC set_write_prot: Sending CMD28",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  cmdarg[3]=data_addr>>24;
  cmdarg[2]=data_addr>>16;
  cmdarg[1]=data_addr>>8;
  cmdarg[0]=data_addr;
  //COMMAND TOKEN CREATION
  Cmd_Token_Create(SET_WRITE_PROT);
  do{
	   AI_ResetBit(MC_GPIO_26);//CS LOW
//	   AI_SetBit(MC_GPIO_1);
       Spi_Mc_Write(Cmd_Token, MC_CMD_SIZE);
       resp=Spi_Mc_Read();
	   attempt++;
    }while((resp != 0x00FF)&&(resp != 0xFF00));//&&(attempt<MC_MAX_ATTEMPTS));
 if((resp != 0x00FF)&&(resp != 0xFF00))
       {
       rvf_send_trace("MC CMD28 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       retval = RV_INTERNAL_ERR;
       }
  else
       {
       rvf_send_trace("MC CMD28 card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
		      if (card_stat_p != NULL)
		      {
		        /* Return  (card response) */
		        *card_stat_p =resp;// NULL;
		      }
       }

  AI_SetBit(MC_GPIO_26);

  return retval;
}
/*************************************************************************************************************************/

/**
 * Send MC CMD29 command
 *
 * Detailled description.
 * This command executes CMD29. It if the card provides write protection
 * features, this command clears the write protection bit of the
 * addressed group.
 * response R1b
 *
 * @return  RV_OK
 */
T_RV_RET clr_write_prot(UINT32 data_addr, UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT16 resp;
  MC_SEND_TRACE("MC clr_write_prot: Sending CMD29",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  cmdarg[3]=data_addr>>24;
  cmdarg[2]=data_addr>>16;
  cmdarg[1]=data_addr>>8;
  cmdarg[0]=data_addr;
  //COMMAND TOKEN CREATION
  Cmd_Token_Create(CLR_WRITE_PROT);


do{
	    Spi_Mc_Write(Cmd_Token, MC_CMD_SIZE);
        resp=Spi_Mc_Read();
  } while (resp!=0x00);
  if(resp!=0x00)
    	 {
       rvf_send_trace("MC CMD29 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       retval = RV_INTERNAL_ERR;
       }
  else
    	 {
       rvf_send_trace("MC CMD29 card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
	   retval=RV_OK;
       }
  if (resp==0x00)
    {
      if (card_stat_p != NULL)
      {
        /* Return R1b (card response) */
        *card_stat_p =resp;
      }
    }
  else
    {
      MC_SEND_TRACE("MC erase: sending CMD29 failed", RV_TRACE_LEVEL_ERROR);
      retval = RV_INTERNAL_ERR;
    }
  AI_SetBit(MC_GPIO_26);
  return retval;
}
/*************************************************************************************************************************/
/**
 * Send MC CMD30 command
 *
 * Detailled description.
 * This command executes CMD30. It if the card provides write protection
 * features, this command asks the card to send the status of the write
 * protection bits.
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET send_write_prot(UINT32 data_addr, UINT32 *wp_prot_grps)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT16 event;
  UINT16 resp;
  MC_SEND_TRACE("MC send_write_prot: Sending CMD30",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  cmdarg[3]=data_addr>>24;
  cmdarg[2]=data_addr>>16;
  cmdarg[1]=data_addr>>8;
  cmdarg[0]=data_addr;
  do{
     Cmd_Token_Create(SEND_WRITE_PROT);
     AI_SetBit(MC_GPIO_26);
     AI_ResetBit(MC_GPIO_1);
  	 Spi_Mc_Write(Cmd_Token, MC_CMD_SIZE);
     resp=Spi_Mc_Read();
	 attempt++;
    } while ( (resp != 0x00FF)&&(resp != 0xFF00));

  AI_SetBit(MC_GPIO_26);
  return retval;
}
 /*************************************************************************************************************************/
 /*************************************************************************************************************************/
/**
 * Send MC CMD32 command
 *
 * Detailled description.
 * This command executes CMD32. It sets the address of the first write
 * block to be erased
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET erase_wr_blk_start(UINT32 data_addr, UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT16 resp;
  MC_SEND_TRACE("MC erase_wr_blk_start: Sending CMD32",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  cmdarg[3]=data_addr>>24;
  cmdarg[2]=data_addr>>16;
  cmdarg[1]=data_addr>>8;
  cmdarg[0]=data_addr;
  //COMMAND TOKEN CREATION
  Cmd_Token_Create(ERASE_WR_BLK_START);
  do{
	   AI_ResetBit(MC_GPIO_26);//CS LOW
//	   AI_SetBit(MC_GPIO_1);
       Spi_Mc_Write(Cmd_Token, MC_CMD_SIZE);
       resp=Spi_Mc_Read();
	   attempt++;
    }while((resp != 0x00FF)&&(resp != 0xFF00) &&(attempt<MC_MAX_ATTEMPTS));
 if((resp != 0x00FF)&&(resp != 0xFF00))
       {
       rvf_send_trace("MC CMD32 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       retval = RV_INTERNAL_ERR;
       }
  else
       {
       rvf_send_trace("MC CMD32 card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
		      if (card_stat_p != NULL)
		      {
		        /* Return  (card response) */
		        *card_stat_p =resp;// NULL;
		      }
       }

  AI_SetBit(MC_GPIO_26);

  return retval;
}

 /*************************************************************************************************************************/
/**
 * Send MC CMD33 command
 *
 * Detailled description.
 * This command executes CMD33. It sets the address of the last write
 * block of the continuous range to be erased.
 * response R1
 *
 * @return  RV_OK
 */
T_RV_RET erase_wr_blk_end(UINT32 data_addr, UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT16 resp=0xFFFF;
  MC_SEND_TRACE("MC erase: Sending CMD33",RV_TRACE_LEVEL_DEBUG_LOW);
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  cmdarg[3]=data_addr>>24;
  cmdarg[2]=data_addr>>16;
  cmdarg[1]=data_addr>>8;
  cmdarg[0]=data_addr;
  //COMMAND TOKEN CREATION
  Cmd_Token_Create(ERASE_WR_BLK_END);
do{
	AI_ResetBit(MC_GPIO_26);//CS LOW
//    AI_SetBit(MC_GPIO_1);
	Spi_Mc_Write(Cmd_Token, MC_CMD_SIZE);
	resp=Spi_Mc_Read();
	attempt++;
    }while((resp != 0x00FF)&&(resp != 0xFF00)&&(attempt<MC_MAX_ATTEMPTS));
 if((resp != 0x00FF)&&(resp != 0xFF00))
       {
       rvf_send_trace("MC CMD33 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
       retval = RV_INTERNAL_ERR;
       }
  else
      {
       rvf_send_trace("MC CMD33 card sent response received ",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
	   if (card_stat_p != NULL)
        {
        /* Return  (card response) */
        *card_stat_p =resp;
        }
       }
  AI_SetBit(MC_GPIO_26);
  return retval;
}
  /*************************************************************************************************************************/
 /*************************************************************************************************************************/

/**
 * Send MC CMD55 command
 *
 * Detailled description.
 * This command executes CMD55. Indicates to the card that the next
 * command is an application specific command rather than a
 * standard command.
 * response R1
 *
 * @return  RV_OK
 */

T_RV_RET app_cmd (UINT32 *card_stat_p)
{

  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT16 resp;
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  //COMMAND TOKEN CREATION
  Cmd_Token_Create(APP_CMD);
 do{
  AI_ResetBit(MC_GPIO_26);//CS LOW
//  AI_SetBit(MC_GPIO_1);
  Spi_Mc_Write(Cmd_Token,MC_CMD_SIZE);
  resp=Spi_Mc_Read();
  attempt++;
 } while ((resp != 0x00FF)&&(resp != 0xFF00));//&&(attempt<25));
 if((resp != 0x00FF)&&(resp != 0xFF00))
 {
  rvf_send_trace("CMD55 Response Not Zero",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  retval= RV_INTERNAL_ERR;
 }
 else
 {
  rvf_send_trace("CMD55 Response Zero",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  retval=RV_OK;
 }
 AI_SetBit(MC_GPIO_26);  //CS HIGH
 return retval;
}

/**
 * Send ACMD13 command
 *
 * Detailled description.
 * This command executes ACMD13. Send the SD Memory Card Status. The
 * status fields are given in the table 26 [SD Specification].
 *
 * @return  RV_OK
 *
 */

T_RV_RET sd_status (UINT8 *sd_status_p)
{
  T_RV_RET retval = RV_OK;
  UINT16 attempt  = 0;
  BOOL complete   = FALSE;
  UINT16 resp;
  UINT32 i;
  UINT16 temp[34]={0};
  cmdarg[3]=0;
  cmdarg[2]=0;
  cmdarg[1]=0;
  cmdarg[0]=0;
  //COMMAND TOKEN CREATION
  Cmd_Token_Create(SD_STATUS);
   MC_SEND_TRACE("MC sd_status: Sending ACMD13",RV_TRACE_LEVEL_DEBUG_LOW);
  do{
        AI_ResetBit(MC_GPIO_26);//CS LOW
//        AI_SetBit(MC_GPIO_1);
	    Spi_Mc_Write(Cmd_Token, MC_CMD_SIZE);
        resp = spi_mc_read_resp_data (34,&temp[0]);
        attempt++;
    } while ((resp != 0x00)&& (attempt<MC_MAX_ATTEMPTS));
 AI_SetBit(MC_GPIO_26);  //CS HIGH

      for(i=2; i < 34; ++i)
     {
      *sd_status_p  = (UINT8)(temp[i]);
       sd_status_p++;
       *sd_status_p = (UINT8)(temp[i]>>8);
       sd_status_p++;
     }
 return retval;
}

/*************************************************************************************************************************/
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
 */

T_RV_RET sd_send_op_cond (T_MC_OCR ocr, T_MC_OCR *ocr_p)
{
  T_RV_RET retval = RV_OK;
  UINT16 attempt  = 0;
  BOOL complete   = FALSE;
  UINT8 resp;
  /* As sd_send_op_cond is a ACMD command we specify that to the card
   * that the command to follow is a ACMD command*/

MC_SEND_TRACE("MC sd_send_op_cond: Sending ACMD41",RV_TRACE_LEVEL_DEBUG_LOW);
AI_ResetBit(MC_GPIO_26);//CS LOW
//AI_SetBit(MC_GPIO_1);

  do{
		  cmdarg[3]=0;
		  cmdarg[2]=0;
		  cmdarg[1]=0;
		  cmdarg[0]=0;
		    //COMMAND TOKEN CREATION
          Cmd_Token_Create(SD_SEND_OP_COND);
          Spi_Mc_Write(Cmd_Token, MC_CMD_SIZE);
		  resp=(UINT8)Spi_Mc_Read();

 	}while (resp!=0x00);
  if(resp==0x00)
  	{
    rvf_send_trace("MC ACMD41 response obtained",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
	retval=RV_OK;
  	}
  else
    {
    rvf_send_trace("MC ACMD41 response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
    retval=RV_INTERNAL_ERR;
  	}
    AI_SetBit(MC_GPIO_26);  //CS HIGH

  return retval;
}

/**
 * Send ACMD51 command
 *
 * Detailled description.
 * This command executes ACMD51. Reads the SD Configuration Register
 * (SCR).
 *
 * @return  RV_OK
 *
 */

T_RV_RET send_scr ( UINT8 *scr_p)
{
  T_RV_RET retval = RV_OK;
  UINT16 attempt  = 0;
  BOOL complete   = FALSE;
  UINT16 temp[6]={0};
  UINT16 resp;
   cmdarg[3]=0;
   cmdarg[2]=0;
   cmdarg[1]=0;
   cmdarg[0]=0;
  /* As sd_send_op_cond is a ACMD command we specify that to the card
   * that the command to follow is a ACMD command*/
  //COMMAND TOKEN CREATION
  do{
	Cmd_Token_Create(SEND_SCR);
	AI_ResetBit(MC_GPIO_26);//CS LOW
//    AI_SetBit(MC_GPIO_1);
	Spi_Mc_Write(Cmd_Token, MC_CMD_SIZE);
	//resp=Spi_Mc_Read();
	resp = spi_mc_read_resp_data (12,&temp[0]);
	attempt++;
    } while ( (resp != 0x00FF)&&(resp != 0xFF00) && (attempt<MC_MAX_ATTEMPTS));


     scr_p[ 0] = (UINT8)(temp[5]>>8); scr_p[ 1] =(UINT8)(temp[4]);
	 scr_p[ 2] = (UINT8)(temp[4]>>8); scr_p[ 3] =(UINT8)(temp[3]);
	 scr_p[ 4] = (UINT8)(temp[3]>>8); scr_p[ 5] =(UINT8)(temp[2]);
 	 scr_p[ 6] = (UINT8)(temp[2]>>8); scr_p[ 7] =(UINT8)(temp[1]);

   AI_SetBit(MC_GPIO_26);  //CS HIGH
   return retval;

}


/*
 * Send MC CMD58 command
 *
 * Detailled description.
 * This command executes CMD58. It asks the card for the OCR register
 *
 *
 * @return  RV_OK
 */
T_RV_RET send_ocr(UINT32 *card_stat_p)
{
  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  int i=0;
  UINT8 count=0;
  UINT8 arr[6]={0};//for collecting the R3 response(R1 response followed by OCR register value"
  UINT16 temp_arr;
  rvf_send_trace("MC send_ocr: Sending CMD58",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
	  cmdarg[3]=0;
	  cmdarg[2]=0;
	  cmdarg[1]=0;
	  cmdarg[0]=0;
	  //COMMAND TOKEN CREATION
	  Cmd_Token_Create(READ_OCR);
	  AI_ResetBit(MC_GPIO_26);
//	  AI_SetBit(MC_GPIO_1);

	  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);

      //Spi_Mc_Data_Read(5,&arr[0]);
      Spi_Mc_Init_Read(&arr[0],5,0);

	  if(arr[0]==0x00)
	//	if((arr[1]&0x80)==1)
	  	{
			temp_arr=arr[1] ;
			*card_stat_p=(temp_arr<<8)||arr[2];
	     *card_stat_p<<=16;
			temp_arr=arr[3] ;
			*card_stat_p=(temp_arr<<8)||arr[4];
         rvf_send_trace("MC send_ocr: response received",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
		 retval=RV_OK;
	  	}
	  else
	  	{
	     rvf_send_trace("MC send_ocr: response error",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
	  	 retval=RV_INTERNAL_ERR;
	  	}
 AI_SetBit(MC_GPIO_26);//CS HIGH
 return retval;
}


/*************************************************************************************************************************/
/**
 * Send MC CMD1 command
 *
 * Detailled description.
 * This command executes CMD1. It resets all card states to idle.
 *
 * response R1
 * @return  RV_OK
 */
T_RV_RET send_op_cond()
{
 T_RV_RET retval;
 UINT16 attempt = 0;
 BOOL complete = FALSE;
 UINT16 resp;
 UINT32 loop_count = 0;
 MC_SEND_TRACE("MC send_op_cond: Sending CMD1",RV_TRACE_LEVEL_DEBUG_LOW);
 cmdarg[3]=0;
 cmdarg[2]=0;
 cmdarg[1]=0;
 cmdarg[0]=0;
 //COMMAND TOKEN CREATION

 do
 {
  Cmd_Token_Create(SEND_OP_COND);
  AI_ResetBit(MC_GPIO_26);//CS LOW
  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
  resp=Spi_Mc_Read();
  attempt++;
 } while ((resp != 0x00FF)&&(resp != 0xFF00)&&(attempt<500));
 if((resp != 0x00FF)&&(resp != 0xFF00))
 {
  rvf_send_trace("CMD1 Response Not Zero",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  retval= RV_INTERNAL_ERR;
 }
 else
 {
  rvf_send_trace("CMD1 Response Zero",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  retval=RV_OK;
 }
 AI_SetBit(MC_GPIO_26);  //CS HIGH
 return retval;

}




T_RV_RET send_crc_on_off()
{
  T_RV_RET retval = RV_OK;
  BOOL complete;
  UINT16 attempt = 0;
  UINT16 resp;
  int i=0;
  UINT8 count=0;
  UINT16 arr[3]={0};//for collecting the R3 response(R1 response followed by OCR register value"
  rvf_send_trace("MC send_crc_on_off: Sending CMD59",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
	  cmdarg[3]=0;
	  cmdarg[2]=0;
	  cmdarg[1]=0;
	  cmdarg[0]=0;
 do
 {
  Cmd_Token_Create(CRC_ON_OFF);
  AI_ResetBit(MC_GPIO_26);//CS LOW
//  AI_SetBit(MC_GPIO_1);
  Spi_Mc_MegaWrite(Cmd_Token, MC_CMD_SIZE,1);
  resp=Spi_Mc_Read();
  attempt++;
 } while ((resp != 0x00FF)&&(resp != 0xFF00));//&&(attempt<25));
 if((resp != 0x00FF)&&(resp != 0xFF00))
 {
  rvf_send_trace("CMD59 Response Not Zero",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  retval= RV_INTERNAL_ERR;
 }
 else
 {
  rvf_send_trace("CMD59 Response Zero",32, NULL_PARAM,RV_TRACE_LEVEL_WARNING,0);
  retval=RV_OK;
 }
 AI_SetBit(MC_GPIO_26);  //CS HIGH
 return retval;

}


