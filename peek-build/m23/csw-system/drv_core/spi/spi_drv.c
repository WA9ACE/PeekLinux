
#include "spi/spi_drv.h"
#if (OP_L1_STANDALONE == 0)
#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#endif
#if (OP_L1_STANDALONE == 1)
#include "general.h"
#include "timer/timer.h"
#endif
#define EIGHT_CYCLES_13M_NS 616

//extern UINT8 cmd_response;
void Spi_Mc_Write(UINT8 *,INT32 );
UINT16 Spi_Mc_Write1(UINT8 *,INT32 );
//UINT16 Spi_Mc_Write_Multiple(UINT8 *,INT32 );
UINT16 Spi_Mc_Block_Write(UINT8 *,INT32 );
void Spi_Mc_Write_32(UINT8 *TOKEN,INT32 length);
UINT16 Spi_Mc_Read();
UINT16 Spi_Mc_Read_Multiple();
void spi_poll_end_transfer(void);
UINT16 Spi_Mc_Read_csd();
void Spi_Mc_Data_Read(UINT32,UINT16*);
void Spi_Mc_Data_Block_Read(UINT32,UINT16*);
UINT16 Spi_Mc_Busy_Read();

UINT8 Spi_Mc_Init_Read(UINT8*,INT16,UINT8);
void Spi_Mc_MegaWrite(UINT8 *,INT32 ,UINT8 );
#define SPI_REG_REV		(MEM_SPI + 0)

#pragma CODE_SECTION(Spi_Mc_Init_Read, ".spiread")
#pragma CODE_SECTION(Spi_Mc_MegaWrite, ".spiwrite")

/*-----------------------------------------------------------------------*/
/* SPI_initDev()                                                         */
/*                                                                       */
/* This function initializes the SPI registers for an external device    */
/* connected to the serial port.                                         */
/*                                                                       */
/*-----------------------------------------------------------------------*/
//initialize spi
/*bose function*/
void spi_initdev()
	{

	  *(volatile SYS_UWORD32*)SPI_REG_SET1 |=0x0000001; // Enabling the functional clock

	 //system condition register setting for a softwrae reset
   	 *(volatile SYS_UWORD32*)SPI_REG_SCR |= 0x2;

	 //checking if reset complete
	 while(!(*(volatile SYS_UWORD32*)SPI_REG_SSR & 0x1));

	 //Setting Auto Idle Mode
	 *(volatile SYS_UWORD32*)SPI_REG_SCR |= 0x1;

	 //enabling Interrupt status register
	*(volatile SYS_UWORD32*)SPI_REG_ISR=0x00000001;

	*(volatile SYS_UWORD32*)SPI_REG_SET1 =0x00000013; // Pre-scaler of 512  // 93.75 Khz

	 //set master mode operation
	 *(volatile SYS_UWORD32*)SPI_REG_SET2=0x00008000;

     *(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000003C; //Set up for 16-bit transfer

	}

/* SPI Pre Initialisation routine */
/* SENDING count * 16 NUMBER clock*/
void spi_mc_preinitialisation(INT32 count)
{
	INT16 loop_count = 0;
    SYS_UWORD32 data_status;


	// Sending initial 0XFFFF before sending CMD0
	*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000003C; //Set up for 16-bit transfer
	do
	{
			SPI_WRITE_TX_LSB(0xFFFF);
			*(volatile SYS_UWORD32*)SPI_REG_CTRL |=0x02; // Enable W Mode of SPI
     		while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02));
     		*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02; //Clearing the Tx interrupt

			loop_count++;
	}while (loop_count < count);
}




/*
flag=1: To Write Any Command
flag=2: To Write single Data into the card
flasg=3 To Write Multiple Data into the card
*/


void Spi_Mc_MegaWrite(UINT8 *TOKEN,INT32 length,UINT8 flag)
{

	int i, odd,rem;
	register UINT16 TxDataLSB,TxDataMSB;
	UINT32 TxData;
	SYS_UWORD32 data_status;
	int loop_count=0,count=0;

	if(flag==1)					// Command and Stop Transmission
	{
		loop_count = length -1;
			if(length==1)
				{
					*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C; //Set up for 8-bit transfer mode enable
					SPI_WRITE_TX_LSB(TOKEN[loop_count]);
					*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode of SPI
					while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02));
					*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;
				}
			else
				{
					 do
					 {
						 *(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000003C; //Set up for 16-bit transfer mode enable
							TxDataLSB=0x00;
							TxDataLSB|=TOKEN[loop_count];
							TxDataLSB<<=8;
							loop_count--;
							TxDataLSB|=TOKEN[loop_count];
							SPI_WRITE_TX_LSB(TxDataLSB);
							*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode of SPI
							loop_count--;
							while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02));

							*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;

					 } while(loop_count >0);

				 }

		}

	if(flag!=1)					//Single and Multiple Block Write
	{

			*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C; //Set up for 8-bit transfer mode enable
			if(flag==2)
			SPI_WRITE_TX_LSB(0xFE);								// Single Block
			if(flag==3)
			SPI_WRITE_TX_LSB(0xFC);								// Multiple Block
			*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode of SPI
			while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02));
			*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;


		count=length;
		*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000007C; //Set up for 32-bit transfer mode enable

		 do
		 {

				TxData=0;
				TxDataMSB=TOKEN[loop_count++];
				TxDataMSB<<=8;
				TxDataMSB|=TOKEN[loop_count++];


				TxDataLSB=TOKEN[loop_count++];
				TxDataLSB<<=8;
				TxDataLSB|=TOKEN[loop_count];


				TxData=TxDataMSB;
				TxData=((TxData<<16)|(TxDataLSB));

				SPI_WRITE_TX(TxData);
				*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode of SPI
				loop_count++;
				while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02)); // check for Tx completion
				*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;
		   }while(loop_count < count );

			if(flag==2)
			{
						*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000003C; //Set up for 16-bit transfer mode enable
						SPI_WRITE_TX_LSB(0x0000);
						*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode of SPI
						while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02));
						*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;
			}
	}


}





/*
flag=0: Just to read Any command response
flag=1: To Read Card Registers
flag=2: To Read Card Data contents
flag=3: To Read Data Write Response.
*/

UINT8 Spi_Mc_Init_Read(UINT8* data_poin,INT16 data_len,UINT8 flag)
{

   	 SYS_UWORD32 data_status;
	 int count=0;
	 UINT8 response8=0xFF;
	 int loop_count=0;
	 UINT32 temp;
	 register UINT16 templsb,tempmsb;
	 UINT8* temp_data_poin=NULL;
	 if(data_poin!=NULL)
	 temp_data_poin=(UINT8*)data_poin;


		if(flag==2)
		{

				do
				{
					*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C; //Set up for 8-bit transfer
					data_status = *(volatile SYS_UWORD32*)SPI_REG_STATUS;
					if ((data_status & 0x03) > 0x00)
					{
						  SPI_WRITE_TX_LSB(0xFFFF);
						*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode of SPI
						while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));
					  //Copy the contents to cmd_response
						response8 = * (volatile UINT8*)SPI_REG_RX_LSB;
						*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
						if(response8 == 0xFE)
						break;

					}
					loop_count++;
				}while (loop_count < 1500);


			count=data_len/4;
			*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000007C;//setting 32 bit data transfer
	    	SPI_WRITE_TX_LSB(0xFFFF);//WRITE FOR CLOCK GENERATION
			SPI_WRITE_TX_MSB(0xFFFF);
		   	while(count>0)
			  {
			  *(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode in SPI
			  while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));
								{
									  temp=*(volatile UINT32 *)SPI_REG_RX;
                 					  tempmsb=((temp>>16));
                 					  templsb=((temp)&0x0000FFFF);
									  *data_poin++ =((tempmsb>>8)&0x00FF);
									  *data_poin++ =((tempmsb)&0x00FF);
									  *data_poin++ =((templsb>>8)&0x00FF);
									  *data_poin++ =((templsb)&0x00FF);

									  *(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt

								}
			   count--;
			  }

				*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000003C;//setting 16 bit data transfer
				*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode in SPI
				while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));
				*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
			return 0;
		}


		if(flag==3)
		{
			*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C;//setting 8 bit data transfer
			loop_count=0;
			do{

			   *(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode in SPI
			   //wait till we have received something
				while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));
				response8 = * (volatile UINT8*)SPI_REG_RX_LSB;
				*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
				if((response8&0x1F) == 0x05)
			   	    return response8;
				loop_count++;
			}while (loop_count <18);
		return response8;
		}



      *(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C;//setting 8 bit data transfer
	   SPI_WRITE_TX_LSB(0xFFFF);//WRITE FOR CLOCK GENERATION

	   do{

		   *(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode in SPI
		   //wait till we have received something
			while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));
			response8 = * (volatile UINT8*)SPI_REG_RX_LSB;
			 *(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
			loop_count++;
			if(response8!=0xFF)
				break;

		}while(loop_count<9);

	if(response8==0xFF)
		{
		return response8;// Error
		}

	loop_count=0;
	if(temp_data_poin!=NULL)
	*temp_data_poin++ =  response8;
	--data_len;

	if (data_len == 0)
		{
		*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode in SPI
		while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02));
		*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;
		return response8;//NO Error
		}

	if (flag)
		{

	    do{

		   *(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode in SPI
		   //wait till we have received something
			while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));
			response8 = * (volatile UINT8*)SPI_REG_RX_LSB;
			 *(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
			loop_count++;
			if(response8!=0xFF)
				break;

		}while(loop_count<8);

	if(response8==0xFF)
		{
		return response8; // Error
		}

		if(temp_data_poin!=NULL)
		*temp_data_poin++ =  response8;
		--data_len;
		}

	do{
	   *(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode in SPI
	   //wait till we have received something
		while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));
		response8 = * (volatile UINT8*)SPI_REG_RX_LSB;
		 *(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
		loop_count++;
		if(temp_data_poin!=NULL)
		*temp_data_poin++ =  response8;
		--data_len;
	}while(data_len);

		*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode in SPI
		while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02));
		*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;

	return 0;  //NO Error


}

UINT16 Spi_Mc_Busy_Read()
		{
	int loop_count = 0;
	UINT16 response=0x0;
	SYS_UWORD32 data_status;


	do
		{
			*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000003C; //Set up for 16-bit transfer
			data_status = *(volatile SYS_UWORD32*)SPI_REG_STATUS;
			//if (((*(SYS_UWORD32*)SPI_REG_STATUS) & 0x03) > 0x00)
			if ((data_status & 0x03) > 0x00)
			{
	              SPI_WRITE_TX_LSB(0xFFFF);
				*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode of SPI
				//Check if we have received anything
				while(! ((*(volatile SYS_UWORD32*)SPI_REG_ISR)&0x01) );
			  			{
						// Check if the contents of the rx register is not 0x00
						if(* (volatile UINT16 *)SPI_REG_RX_LSB != 0xFFFF)
							{
							  *(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
							  break;
							}
			     		}
	          loop_count++;
	 		}
	}while(loop_count<5);//Check if the card is busy by monitoring the MISO line.




	do
	{
		*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000003C; //Set up for 16-bit transfer
		data_status = *(volatile SYS_UWORD32*)SPI_REG_STATUS;
		if ((data_status & 0x03) > 0x00)
		{
              SPI_WRITE_TX_LSB(0xFFFF);
			*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode of SPI
			//Check if we have received anything
			while(! ((*(volatile SYS_UWORD32*)SPI_REG_ISR)&0x01) );
		  			{
					// Check if the contents of the rx register is not 0x00
					if(* (volatile UINT16 *)SPI_REG_RX_LSB != 0x0000)
						{
						  //Copy the contents to cmd_response
							response = * (volatile UINT16*)SPI_REG_RX_LSB;

							*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
						}
		     		}
          loop_count++;
 		}
	}while(!response);//Check if the card is busy by monitoring the MISO line.
	return response;
}




/* Below functions doesnt have much impact on any functionality  You can ignore them*/



void Spi_Mc_Write(UINT8 *TOKEN,INT32 length)
{
	int i, odd;
	UINT16 TxData;
	SYS_UWORD32 data_status;
	int loop_count = length -1;

	odd = length%2;

  if(length>1)
	{

	 do
	 {
		 *(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000003C; //Set up for 16-bit transfer mode enable
		{
			TxData = 0x0000;
			TxData|=TOKEN[loop_count];
			TxData<<=8;
			loop_count--;
			TxData|=TOKEN[loop_count];
			SPI_WRITE_TX_LSB(TxData);
			*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode of SPI
			loop_count--;
			while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02))
				{
				if ( (*(volatile SYS_UWORD32*)SPI_REG_REV)== 0)
					{
					volatile  UINT8 spinLock = 1;
					while (spinLock);
					}
				}
			*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;
			if (odd&&(loop_count ==0))
			{

				*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C; //Set up for 8-bit transfer mode enable
				SPI_WRITE_TX_LSB(TOKEN[loop_count]);
				*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode of SPI
				while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02));
				*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;

			}
		}

	 } while(loop_count >0);

    }
	else
	{
		*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C; //Set up for 8-bit transfer mode enable
		SPI_WRITE_TX_LSB(TOKEN[loop_count]);
		*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode of SPI
		while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02));
		*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;
	}
}


UINT16 Spi_Mc_Block_Write(UINT8 *TOKEN,INT32 length)
{
	int i, odd;
	UINT16 TxDataLSB;
	UINT16 TxDataMSB;
	UINT32 TxData;
	SYS_UWORD32 data_status;
	int loop_count = 0,loop_cnt=0;
	UINT16 response=0;


	odd = length%2;

  if(length>1)
	{
	 *(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000007C; //Set up for 32-bit transfer mode enable
	 do
	 {
		// *(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000007C; //Set up for 32-bit transfer mode enable
		data_status = *(volatile SYS_UWORD32*)SPI_REG_STATUS;//0x09E00030;
		data_status &= 0x02;
		if (data_status)
		{
			TxData=0;
			TxDataMSB=TOKEN[loop_count++];
			TxDataMSB<<=8;
			TxDataMSB|=TOKEN[loop_count++];
			TxDataLSB=TOKEN[loop_count++];
			TxDataLSB<<=8;
			TxDataLSB|=TOKEN[loop_count];
			TxData=TxDataMSB;
			TxData=((TxData<<16)|(TxDataLSB));
			SPI_WRITE_TX(TxData);
			*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode of SPI
			loop_count++;
			while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02)); // check for Tx completion
			*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;
		}
	   }while(loop_count <length -1);

	    if (odd&&(loop_count ==(length -1)))
		{
			*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C; //Set up for 8-bit transfer mode enable
			do
			{
				SPI_WRITE_TX_LSB(TOKEN[loop_count]);
				*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable W Mode of SPI

				while(!((*(volatile SYS_UWORD32*)SPI_REG_STATUS) & 0x03));
					if(* (UINT16 *)SPI_REG_RX_LSB != 0xFFFF)
					{
					  //Copy the contents to cmd_response
					  response = * (volatile UINT8*)SPI_REG_RX_LSB;
					*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x03; //Clearing the rx interrupt
				    }
					if((response&0x001F) == 0x0005)
				   	{
				  	    break;
				   	}

				    loop_cnt++;

		   }while (loop_cnt <8);
	     }
    }
	else
	{
	*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C; //Set up for 8-bit transfer mode enable
		SPI_WRITE_TX_LSB(TOKEN[loop_count]);
		*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x02; // Enable W Mode of SPI
		while(!(*(volatile SYS_UWORD32*)SPI_REG_ISR &0x02));
		*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x02;

	}
return response;
}


UINT16 Spi_Mc_Read()
{
	int loop_count = 0;
	UINT16 response=0xFFFF;
	SYS_UWORD32 data_status;

	do
	{
		*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000003C; //Set up for 16-bit transfer
		{

              SPI_WRITE_TX_LSB(0xFFFF);
			*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode of SPI
			//Check if we have received anything
			while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));
					// Check if the contents of the rx register is not 0xFF
					if(* (UINT16 *)SPI_REG_RX_LSB != 0xFFFF)
						{
						  //Copy the contents to cmd_response
							response = * (volatile UINT16*)SPI_REG_RX_LSB;

							*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
						}
					if((response == 0x00FF)||(response == 0xFF00)||(response == 0x01FF)||(response == 0xFF01))
				      	{
				      	    //return response;
				      	    break;
				      	}

          loop_count++;
 		}
	}while (loop_count <8);//Check if there is a response in the Read register or Timeout
	return response;

	}



/* GENERIC FUNCTION FOR RETURNING DATA OF 16BIT SIZE*/
void Spi_Mc_Data_Read(UINT32 data_len,UINT16* data_poin)
 {	 UINT16 local[3]={0};
     SYS_UWORD32 data_status;
	 int count=0;
	 UINT8 response8=0xFF;
	 int loop_count=0;

	 if(data_len>1)
	 	{
		 	if((data_len%2)!=0)
		 	{
		 	 count=(data_len+1)/2;
		 	}
		    else
		 	{
		 	 count=data_len/2;
		 	}
	 	}
	 else
	 	{
	 	 count=data_len;
	 	}




		*(SYS_UWORD32*)SPI_REG_ISR=0x00000001;
      	*(SYS_UWORD32*)SPI_REG_CTRL=0x0000001C;//setting 16 bit data transfer

	   do{

	       SPI_WRITE_TX_LSB(0xFFFF);//WRITE FOR CLOCK GENERATION
	      *(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode in SPI
		while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));

						response8 = * (volatile UINT8*)SPI_REG_RX_LSB;
						*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt

						if(response8 != 0xFF)
						break;
			loop_count++;


	               			{

								 *data_poin =*(volatile UINT16 *)SPI_REG_RX_LSB;
                                 *(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
								  data_poin++;

       	                    }
		   count--;
          }while(loop_count<9);

}



/* GENERIC FUNCTION FOR RETURNING DATA OF 16BIT SIZE*/
void Spi_Mc_Data_Block_Read(UINT32 data_len,UINT16* data_poin)
 {	 UINT16 local[3]={0};
     UINT8 response8=0xFF;
	 UINT16 temp;
     SYS_UWORD32 data_status;
	 int count=0;
	 int i;
	 int loop_count=0;
	 int odd;
	 if(data_len>1)
	 	{
		 	if((data_len%2)!=0)
		 	{
		 	 count=(data_len+1)/2;
		 	}
		    else
		 	{
		 	 count=data_len/2;
		 	}
	 	}
	 else
	 	{
	 	 count=data_len;
	 	}

	odd = data_len%2;
	count=data_len/2+1;
	do
		{
			*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C; //Set up for 8-bit transfer
			data_status = *(volatile SYS_UWORD32*)SPI_REG_STATUS;
			if ((data_status & 0x03) > 0x00)
			{
				  SPI_WRITE_TX_LSB(0xFFFF);
				*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode of SPI
				while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));

			  //Copy the contents to cmd_response
				response8 = * (volatile UINT8*)SPI_REG_RX_LSB;
								*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt

							if(response8 == 0xFE)
				break;

			    		  	}
 			loop_count++;
 		}while (loop_count < 500);



		*(SYS_UWORD32*)SPI_REG_ISR=0x00000001;
      	*(SYS_UWORD32*)SPI_REG_CTRL=0x0000003C;//setting 16 bit data transfer
	   while(count>0)
          {
	       SPI_WRITE_TX_LSB(0xFFFF);//WRITE FOR CLOCK GENERATION
	      *(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode in SPI

           data_status = *(volatile SYS_UWORD32*)SPI_REG_STATUS;
	    //check if we have received anything
		while(!((*(volatile SYS_UWORD32*)SPI_REG_STATUS) & 0x03));
	               			{
                 				  temp=*(volatile UINT16 *)SPI_REG_RX_LSB;
                                 *data_poin =((temp>>8)&0x00FF);
								 *data_poin |=((temp<<8)&0xFF00);
                                 *(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
								  data_poin++;
       	                    }
		   count--;
          }

          if (odd)
          {
			*(SYS_UWORD32*)SPI_REG_CTRL=0x0000001C;//setting 8 bit data transfer
	       SPI_WRITE_TX_LSB(0xFFFF);//WRITE FOR CLOCK GENERATION
	      *(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode in SPI
           data_status = *(volatile SYS_UWORD32*)SPI_REG_STATUS;
	    //check if we have received anything
		while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));
		response8 = * (volatile UINT8*)SPI_REG_RX_LSB;
		*data_poin =response8;
        }


}



/**********************************************************************/
/*SPI_MC_READ_RESP_DATA*/
/**********************************************************************/
UINT8 spi_mc_read_resp_data(UINT32 data_len,UINT16* p_data)
{
	INT8 loop_count = 0;
	INT8 i=0;
	UINT8 response=0xFF;
	UINT32 no_of_blocks=0;

	do
	{
		*(volatile SYS_UWORD32*)SPI_REG_CTRL=0x0000001C; //Set up for 16-bit transfer
		{
			SPI_WRITE_TX_LSB(0xFFFF);

			*(volatile SYS_UWORD32*)SPI_REG_CTRL|=0x01; // Enable RW Mode of SPI

			while(!((*(volatile SYS_UWORD32*)SPI_REG_ISR) & 0x01));
		  	{
					response = * (volatile UINT8*)SPI_REG_RX_LSB;
					*(volatile SYS_UWORD32*)SPI_REG_ISR |= 0x01; //Clearing the rx interrupt
			      if(response == 0x01)
			      	{
			      		Spi_Mc_Data_Read(data_len, p_data);
			      		return response;
			      	}

		       }
 			loop_count++;
 		}

	}while (loop_count < 8);

  return response;


}

