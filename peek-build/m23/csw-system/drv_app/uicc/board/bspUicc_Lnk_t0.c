
/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */

#include "sys_types.h"
#include "types.h"
#include "general.h"
#include "bspUicc_Phy.h"
#include "bspUicc_Lnk_msgUtil.h"
#include "bspUicc_Lnk_command.h"
#include "bspUicc_Lnk_t0.h"

#include "bspUicc_Phy_map.h"
#include "bspUicc_Phy_llif.h"
#include "bspUicc_Phy.h"
#include "nucleus.h"

/*=============================================================================
 *  File Description:
 *  Source File with code specific to the T0 protocol
 */

/*=============================================================================
 *   Defines
 *============================================================================*/
#define BSP_UICC_LNK_T0_NO_DATA_TO_READ_FROM_CARD                       0
#define BSP_UICC_LNK_T0_NO_DATA_INVALID_PROCEDURE_STATUS_BYTE           -1
#define BSP_UICC_LNK_T0_NO_READ_WRITE_ERROR                             0
#define BSP_UICC_LNK_T0_READ_WRITE_ERROR                               -1
#define BSP_UICC_LNK_T0_READ_DATA_NOT_COMPLETE                          0
#define BSP_UICC_LNK_T0_READ_DATA_COMPLETE                              1
#define BSP_UICC_LNK_T0_WRITE_DATA_NOT_COMPLETE                         0
#define BSP_UICC_LNK_T0_WRITE_DATA_COMPLETE                             1

/*=============================================================================
 *   Static Data
 *============================================================================*/
BspUicc_Lnk_CommandReadData     bspUicc_Lnk_t0StatusBytes[ BSP_UICC_LNK_COMMAND_STATUS_BYTES_NUM_OCTETS ];
SYS_UWORD8                      bspUicc_Lnk_t0StatusByteCount;
BspUicc_Lnk_T0ReturnCode        bspUicc_Lnk_t0_read_write_error = 0;
extern  volatile SYS_BOOL clockstopDone;
extern BspUicc_Phy_Ctrl  bspUicc_Phy_ctrl;

/*=============================================================================
  *  set before writing the trick STATUS command and reset upon getting the expected lastbyte
  *
  */
extern  volatile SYS_BOOL waittingToSleep;


/*=============================================================================
  *  expectedDataCount  is set to Le
  *
  */
extern SYS_UWORD8 expectedDataCount;
/*=============================================================================
   *  expectedDataCount  is set to Le
   *
   */
extern SYS_UWORD8 dataCount;
/*=============================================================================
   *  clockStopWAstate  keeps the state of the decoding STATEMACHINE
   *
   */
extern  BspUicc_clockStopWAstate clockStopWAstate ;

extern volatile SYS_BOOL bspUicc_sClk_running;


/*=============================================================================
 *   Private Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 *   Function to Send the command header
 *
 * Parameters
 *   class          -  Class of the command
 *   instruction    -  Instruction of the command
 *   parameter1     -  Command Parameter 1
 *   parameter2     -  Command Parameter 2
 *   parameter3     -  Command Parameter 3
 *
 * Return
 *   status         -  The write status/ number of bytes sent
 *   
 */
static BspUicc_Lnk_T0ReturnCode
bspUicc_Lnk_t0WriteHeader( BspUicc_Lnk_CommandClass           class,
                           BspUicc_Lnk_CommandInstruction     instruction,
                           BspUicc_Lnk_CommandParameter1      parameter1,
                           BspUicc_Lnk_CommandParameter2      parameter2,
                           BspUicc_Lnk_CommandParameter3      parameter3 )
{
    BspUicc_Lnk_T0ReturnCode        status = BSP_UICC_LNK_T0_RETURN_CODE_SUCCESS;
    BspUicc_Lnk_CommandWriteData    commandHeader[ BSP_UICC_LNK_COMMAND_HEADER_NUM_OCTETS ];
    
    

    if( instruction == 0xF2) // STATUS command
    {
      if (bspUicc_Phy_ctrl.sClkLev != BSP_UICC_PHY_SCLOCK_NO_STOP) // clcokstop is allowed   ,  we are not in clockstop 
      {
	   dataCount =0;
       expectedDataCount = parameter3;
       clockStopWAstate = WAIT_FOR_ACK_NAK_SW_NULL;
       waittingToSleep = TRUE;  // do a clock stop in rx handler

	   BSP_UICC_PHY_LLIF_SET( BSP_UICC_PHY_LLIF_FIFO_RX_TRIGGER,
									  BSP_UICC_PHY_MAP_REG_USIM_FIFOS_PTR,
									  BSP_UICC_PHY_LLIF_FIFO_MIN_RX_TRIG_SIZE );
      }  
    }
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_CLA,
                               commandHeader,
                               class );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_INS,
                               commandHeader,
                               instruction );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_P1,
                               commandHeader,
                               parameter1 );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_P2,
                               commandHeader,
                               parameter2 );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_P3,
                               commandHeader,
                               parameter3 );
    
    status = bspUicc_Phy_write( BSP_UICC_LNK_COMMAND_HEADER_NUM_OCTETS,
                                commandHeader );
    
    return status;
}

/*=============================================================================
 * Description:
 *   Function to get Status Bytes
 *
 * Parameters
 *   statusBytesPtr  - Pointer to the status Bytes data
 *
 * Return
 *   status         -  The read status/ number of bytes read
 *   
 */
static BspUicc_Lnk_T0ReturnCode
bspUicc_Lnk_t0ReadStatusBytes( BspUicc_Lnk_CommandReadData *statusBytesPtr )
{
    BspUicc_Lnk_T0ReturnCode        status;
    BspUicc_Phy_ReturnCode          bytesRead = 0;


    if(bspUicc_Lnk_t0StatusByteCount == 0)
    {
        
        bytesRead = bspUicc_Phy_read( BSP_UICC_LNK_MSG_UTIL_APDU_SW1_NUM_OCTETS,
                                      statusBytesPtr );
        if (bytesRead < BSP_UICC_LNK_MSG_UTIL_APDU_SW1_NUM_OCTETS)
        {
            bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR;
        }
        while ((*statusBytesPtr ==
                BSP_UICC_LNK_COMMAND_APDU_NULL_PROCEDURE_VALUE)  &&
               (bspUicc_Lnk_t0_read_write_error !=
                BSP_UICC_LNK_T0_READ_WRITE_ERROR))
        {
            *statusBytesPtr = 0x00;
            bytesRead = bspUicc_Phy_read( BSP_UICC_LNK_MSG_UTIL_APDU_SW1_NUM_OCTETS,
                                      statusBytesPtr );
            if (bytesRead < BSP_UICC_LNK_MSG_UTIL_APDU_SW1_NUM_OCTETS)
            {
                bspUicc_Lnk_t0_read_write_error =
                    BSP_UICC_LNK_T0_READ_WRITE_ERROR;
            }
        }
     }
      
    /* check for valid status bytes */
    if (((*statusBytesPtr & 0x60) !=
         BSP_UICC_LNK_COMMAND_APDU_SW1_VALID_VALUE) &&
        ((*statusBytesPtr & 0x90) !=
         BSP_UICC_LNK_COMMAND_APDU_SW1_NORMAL_VALUE))
    {
        bytesRead = BSP_UICC_LNK_COMMAND_STATUS_UNRECOVERABLE_FAILURE ;
    }
    else
    {
        statusBytesPtr++;
        bytesRead = bspUicc_Phy_read( BSP_UICC_LNK_MSG_UTIL_APDU_SW2_NUM_OCTETS,
                                      statusBytesPtr );
        if (bytesRead < BSP_UICC_LNK_MSG_UTIL_APDU_SW2_NUM_OCTETS)
        {
            bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR;
        }
    }
    
    status = bytesRead;
    return status;  
}

    
/*=============================================================================
 * Description:
 *   Function to read a given amount of data, Command Case 2
 *
 * Parameters
 *   instruction     -  Instruction of the command
 *   readDataSize    -  Size of Data to be read
 *   readDataPtr     -  Pointer to read the data into
 *   statusBytesPtr  -  Pointer to the status Bytes data
 * 
 * Return
 *   status         -  The read status/ number of bytes read
 *   
 */
static BspUicc_Lnk_T0ReturnCode
bspUicc_Lnk_t0ReadData( BspUicc_Lnk_CommandInstruction     instruction,
                        BspUicc_Lnk_CommandReadDataSize    readDataSize,
                        BspUicc_Lnk_CommandReadData        *readDataPtr,
                        BspUicc_Lnk_CommandReadData        *statusBytesPtr )
{
    BspUicc_Lnk_T0ReturnCode        status;
    BspUicc_Phy_ReturnCode          bytesRead = 0;
    BspUicc_Lnk_CommandReadData          readResult= 0x00; /* Ack/Nack/procedure */
    BspUicc_Lnk_CommandReadDataSize      readCount = 0; /* Actual Rx bytes */
    BspUicc_Lnk_CommandReadDataSize      i;
    BspUicc_Lnk_CommandReadDataSize      nullProcedureByteCount=0;

    SYS_UWORD8 readDataComplete = BSP_UICC_LNK_T0_READ_DATA_NOT_COMPLETE;

    BspUicc_Lnk_CommandReadMoreDataFlag  moreDataToReadFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_REQUIRED;
    bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_NO_READ_WRITE_ERROR   ;
    bytesRead = bspUicc_Phy_read( BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS,
                                  &readResult );
    if (bytesRead < BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS)
    {
        bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR;
    }
    while ((readDataComplete ==
            BSP_UICC_LNK_T0_WRITE_DATA_NOT_COMPLETE) &&
           (bspUicc_Lnk_t0_read_write_error !=
            BSP_UICC_LNK_T0_READ_WRITE_ERROR))
    {
    	if( readResult == instruction )/* Ack received get the command Data */
    	{
        	readCount = bspUicc_Phy_read( readDataSize,
                                      readDataPtr );
            	if(readCount != readDataSize)
            	{
                	bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR ;
            	}	 
            	readDataComplete = BSP_UICC_LNK_T0_READ_DATA_COMPLETE ;
        
    	}
        else if( readResult == ((~instruction) & 0x000000FF ))/* Nack received get data char by char */
    	{        
            for( i=0; i<(readDataSize + nullProcedureByteCount) &&  moreDataToReadFlag; i++ )
        	{
                
                if(readResult  == BSP_UICC_LNK_COMMAND_APDU_NULL_PROCEDURE_VALUE )
                {
                    bytesRead =  bspUicc_Phy_read( BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS,
                              &readResult );
            
                    nullProcedureByteCount++;
                    if(bytesRead <= 0)
                    {
                        bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR  ;
                        readDataComplete = BSP_UICC_LNK_T0_READ_DATA_COMPLETE ;
                        moreDataToReadFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
                    }
                    
                }
                /* Read the remaining data all at once */
                else if( readResult == instruction )
                {
                    bytesRead = bspUicc_Phy_read( (readDataSize- readCount),
                                                  readDataPtr );
                    if(bytesRead <= 0)
                    {
                          bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR  ;
                          readDataComplete = BSP_UICC_LNK_T0_READ_DATA_COMPLETE ;
                          moreDataToReadFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
                    }
                    else
                    {
                         readCount = readCount + bytesRead;
                    }
                    
                    moreDataToReadFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
                    readDataComplete = BSP_UICC_LNK_T0_READ_DATA_COMPLETE;
                    
                }
                else if( readResult == ((~instruction) & 0x000000FF)) 
                {
            		bytesRead = bspUicc_Phy_read( BSP_UICC_LNK_COMMAND_CHAR_NUM_OCTETS,
                                          readDataPtr );
                    if(bytesRead <= 0)
                    {
                        bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR  ;
                        readDataComplete = BSP_UICC_LNK_T0_READ_DATA_COMPLETE ;
                        moreDataToReadFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
			   continue;                        
                    }
                    readCount = readCount + bytesRead;
                    
            readDataPtr += bytesRead;

                    if(readCount < readDataSize)
                    {
                        bytesRead =  bspUicc_Phy_read( BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS,
                                                       &readResult );
                        if(bytesRead <= 0)
                        {
                            bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR  ;
                            readDataComplete = BSP_UICC_LNK_T0_READ_DATA_COMPLETE ;
                            moreDataToReadFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
                            
        			}
    			}
            } 
     	}

            
          
        readDataComplete = 1;
            
            
    }
    else
    {
            if( readResult != BSP_UICC_LNK_COMMAND_APDU_NULL_PROCEDURE_VALUE )
            {
                if (((readResult & 0x60) ==
                     BSP_UICC_LNK_COMMAND_APDU_SW1_VALID_VALUE) ||
                    ((readResult & 0x90) ==
                     BSP_UICC_LNK_COMMAND_APDU_SW1_NORMAL_VALUE))
                {
                    /* Card has sent a procedure character instead */
        			BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_SW1,
                                   statusBytesPtr,
                                   readResult );
        			bspUicc_Lnk_t0StatusByteCount++;
    			}
                else
                {
                    /* Card has sent a procedure invalid status byte instead */
                    bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR  ;
                }
                readDataComplete = BSP_UICC_LNK_T0_READ_DATA_COMPLETE ;
                
            }
            else
            {    /* Read next procedure byte when the curr procedure byte=0x60 */
                bspUicc_Phy_read( BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS,
                                  &readResult );  
                
                if (bytesRead < BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS)
                {
                    bspUicc_Lnk_t0_read_write_error =
                        BSP_UICC_LNK_T0_READ_WRITE_ERROR;
                }        
        }
        
    }
 }
    status = readCount;
    
    return status;
}

/*=============================================================================
 * Description:
 *   Function to write a known amount of data, Command Case 3/4
 *
 * Parameters
 *   instruction     -  Instruction of the command
 *   writeDataSize   -  Size of Data to be written
 *   writeDataPtr    -  Pointer to  the data  to be written
 * 
 * Return
 *   status         -  The read status/ number of bytes read
 *   
 */
static BspUicc_Lnk_T0ReturnCode
bspUicc_Lnk_t0WriteData( BspUicc_Lnk_CommandInstruction     instruction,
                         BspUicc_Lnk_CommandWriteDataSize   writeDataSize,
                         BspUicc_Lnk_CommandWriteData       *writeDataPtr )
{
    BspUicc_Lnk_T0ReturnCode        status;
    BspUicc_Phy_ReturnCode          bytesSent = 0;
    BspUicc_Lnk_CommandReadData     readValue;
    BspUicc_Lnk_CommandWriteDataSize     i;
    BspUicc_Lnk_CommandWriteDataSize     totalBytesSent=0;
    BspUicc_Lnk_CommandWriteDataSize     nullProcedureByteCount=0;
    BspUicc_Lnk_CommandReadDataSize      bytesRead = 0;

    BspUicc_Lnk_CommandReadMoreDataFlag  moreDataToWriteFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_REQUIRED;
    SYS_UWORD8 writeDataComplete = BSP_UICC_LNK_T0_WRITE_DATA_NOT_COMPLETE;
    bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_NO_READ_WRITE_ERROR   ;


    bytesRead = bspUicc_Phy_read(BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS,
                                 &readValue);
    if (bytesRead < BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS)
    {
        bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR;
    }
    while ((writeDataComplete ==
            BSP_UICC_LNK_T0_WRITE_DATA_NOT_COMPLETE) &&
           (bspUicc_Lnk_t0_read_write_error !=
            BSP_UICC_LNK_T0_READ_WRITE_ERROR))
    {
        /* Ack received write all the command Data */
        if( readValue == instruction )
    	{
        	bytesSent = bspUicc_Phy_write( writeDataSize,
                                       writeDataPtr );
            totalBytesSent = totalBytesSent + bytesSent;
            
            writeDataComplete = BSP_UICC_LNK_T0_WRITE_DATA_COMPLETE ;
    	}
        /* Nack received write data char by char */
        
        else if( readValue == ((~instruction) & 0x000000FF))
    	{
            for( i=0; ((i<(writeDataSize + nullProcedureByteCount) ) && (moreDataToWriteFlag)); i++ )
        	{
                /* Procedure byte is a NULL byte , so next byte is also a procedure byte*/
                if(readValue == BSP_UICC_LNK_COMMAND_APDU_NULL_PROCEDURE_VALUE )
                {
                    bytesRead =  bspUicc_Phy_read( BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS,
                              &readValue );
            
                    
                    nullProcedureByteCount++;
                    if(bytesRead <= 0)
                    {
                        bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR  ;
                        writeDataComplete = BSP_UICC_LNK_T0_WRITE_DATA_COMPLETE ;
                        moreDataToWriteFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
                    }
                    
                }
                /* Write the rest of teh data to writeen all at once */
                else if( readValue == instruction )
                {
                    bytesSent = bspUicc_Phy_write( (writeDataSize- totalBytesSent),
                                                   writeDataPtr );
                    moreDataToWriteFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
                    writeDataComplete = BSP_UICC_LNK_T0_WRITE_DATA_COMPLETE;
                    if(bytesSent <= 0 )
                    {
                        bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR  ;
                        writeDataComplete = BSP_UICC_LNK_T0_WRITE_DATA_COMPLETE ;
                        moreDataToWriteFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
                    }
                    else
                    {
                        totalBytesSent = totalBytesSent + bytesSent;
                    }
                    
                }
                else if( readValue == ((~instruction) & 0x000000FF)) 
                {
            		bytesSent = bspUicc_Phy_write( BSP_UICC_LNK_COMMAND_CHAR_NUM_OCTETS,
                                           writeDataPtr );
                    totalBytesSent = totalBytesSent + bytesSent;
                    
            		writeDataPtr += bytesSent;
                    
                    if (totalBytesSent < writeDataSize)
                    {
                        bytesRead =  bspUicc_Phy_read( BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS,
                                                       &readValue );
                        if(bytesRead <= 0)
                        {
                            bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR  ;
                            writeDataComplete = BSP_UICC_LNK_T0_WRITE_DATA_COMPLETE ;
                            moreDataToWriteFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
                            
                        }
                    }
                    else
                    {
                        writeDataComplete = BSP_UICC_LNK_T0_WRITE_DATA_COMPLETE ;
                        moreDataToWriteFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
                    }
                }
                
                
                
        }
            
            writeDataComplete = BSP_UICC_LNK_T0_WRITE_DATA_COMPLETE;
            
            
    }
    else /* The command resulted in a status byte or a procedure byte */
    {
            if( readValue != BSP_UICC_LNK_COMMAND_APDU_NULL_PROCEDURE_VALUE )
            {
                if(((readValue & 0x60) == 0x60) ||((readValue & 0x90) == 0x90))
                if (((readValue &  0x60)
                     == BSP_UICC_LNK_COMMAND_APDU_SW1_VALID_VALUE) ||
                    ((readValue & 0x90) ==
                     BSP_UICC_LNK_COMMAND_APDU_SW1_NORMAL_VALUE))
                {
                    /* Card has sent status byte */
        			BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_SW1,
                                   bspUicc_Lnk_t0StatusBytes,
                                   readValue );
        			bspUicc_Lnk_t0StatusByteCount++;
    		}
                else
                {
                    /* Card has sent invalid byte */
                    bspUicc_Lnk_t0_read_write_error = BSP_UICC_LNK_T0_READ_WRITE_ERROR  ;
                }
                writeDataComplete = BSP_UICC_LNK_T0_WRITE_DATA_COMPLETE ;
            }
            else
            {    /* Read next procedure byte when the curr procedure byte=0x60 */
                bspUicc_Phy_read( BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS,
                                  &readValue );  
                
                if (bytesRead < BSP_UICC_LNK_COMMAND_ACK_NUM_OCTETS)
                {
                    bspUicc_Lnk_t0_read_write_error =
                        BSP_UICC_LNK_T0_READ_WRITE_ERROR;
                }  
        }
        
    }
 }   
    
    status = totalBytesSent; 
    
    return status;
}
/*=============================================================================
 * Description:
 *   Send the Application Protocol Data Unit.
 */
BspUicc_Lnk_T0ReturnCode
bspUicc_Lnk_t0ProcessApdu( BspUicc_Lnk_CommandClass          class,
                           BspUicc_Lnk_CommandInstruction    instruction,
                           BspUicc_Lnk_CommandParameter1     parameter1,
                           BspUicc_Lnk_CommandParameter2     parameter2,
                           BspUicc_Lnk_CommandWriteDataSize  writeDataSize,
                           BspUicc_Lnk_CommandWriteData      *writeDataPtr,
                           BspUicc_Lnk_CommandReadDataSize   readDataSize,
                           BspUicc_Lnk_CommandReadData       *readDataPtr,
                           BspUicc_Lnk_CommandReadDataSize   *resultSizePtr,
                           BspUicc_Lnk_CommandType           commandType )
{
    BspUicc_Lnk_T0ReturnCode        status = BSP_UICC_LNK_T0_RETURN_CODE_SUCCESS;
    BspUicc_Lnk_T0ReturnCode        readStatus = BSP_UICC_LNK_T0_RETURN_CODE_SUCCESS;
    BspUicc_Lnk_CommandParameter3   parameter3;

    BspUicc_Lnk_CommandReadDataSize resultSize = 0;
    
    /* initalize the status byte count */
    bspUicc_Lnk_t0StatusByteCount = 0;
    
    /* initialize locals */
    bspUicc_Lnk_t0StatusBytes[BSP_UICC_LNK_MSG_UTIL_APDU_SW1_OCTET_OFFSET] = 0;
    bspUicc_Lnk_t0StatusBytes[BSP_UICC_LNK_MSG_UTIL_APDU_SW2_OCTET_OFFSET] = 0;

      clockstopDone = FALSE;
      bspUicc_Phy_sleepModeDisable();
    
    if( commandType == BSP_UICC_LNK_COMMAND_TYPE_1 )
    {
        /* Case 1 command */
        parameter3 = BSP_UICC_LNK_MSG_UTIL_APDU_P3_CASE1_VALUE;
        bspUicc_Lnk_t0WriteHeader( class,
                                                 instruction,
                                                 parameter1,
                                                 parameter2,
                                                 parameter3 );
        
    }
    else if( commandType == BSP_UICC_LNK_COMMAND_TYPE_2 )
    {
        /* Case 2 command */
        parameter3 = readDataSize;
        bspUicc_Lnk_t0WriteHeader( class,
                                   instruction,
                                   parameter1,
                                   parameter2,
                                   parameter3 );
        
        resultSize = bspUicc_Lnk_t0ReadData( instruction,
                                             readDataSize,
                                             readDataPtr,
                                             bspUicc_Lnk_t0StatusBytes );
    }
    else if( ( commandType == BSP_UICC_LNK_COMMAND_TYPE_4 ) ||
             ( commandType == BSP_UICC_LNK_COMMAND_TYPE_3 ) )
    {
        parameter3 = writeDataSize;
        bspUicc_Lnk_t0WriteHeader( class,
                                   instruction,
                                   parameter1,
                                   parameter2,
                                   parameter3 );
        
        bspUicc_Lnk_t0WriteData( instruction,
                                 writeDataSize,
                                 writeDataPtr );
    }
    
    
    if ( bspUicc_Lnk_t0_read_write_error == BSP_UICC_LNK_T0_NO_READ_WRITE_ERROR  )
    {                                                    
        readStatus = bspUicc_Lnk_t0ReadStatusBytes(  bspUicc_Lnk_t0StatusBytes );
        if ((readStatus > 0) &&
            (bspUicc_Lnk_t0_read_write_error ==
             BSP_UICC_LNK_T0_NO_READ_WRITE_ERROR))
    	{
       		/* Normal termination of a command, Create a 16bit result value*/
        	status = ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_APDU_SW1,
                                              bspUicc_Lnk_t0StatusBytes )
                   << BSP_UICC_LNK_MSG_UTIL_APDU_SW1_BIT_WIDTH );
        
        	status |= ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_APDU_SW2,
                                               bspUicc_Lnk_t0StatusBytes ) );
        
        	if( ( commandType == BSP_UICC_LNK_COMMAND_TYPE_2 ) ||
             ( commandType == BSP_UICC_LNK_COMMAND_TYPE_4 ) )
        	{
            *resultSizePtr = resultSize;
        	}
    	}
    	else
    	{
        	if ( bspUicc_Phy_getCardInsertStatus() == BSP_UICC_LNK_CARD_NOT_PRESENT_STATUS )
        	{
            	status = BSP_UICC_LNK_COMMAND_STATUS_CARD_NOT_INSERTED;
        	}
        	else
        	{    
           		status = BSP_UICC_LNK_COMMAND_STATUS_UNRECOVERABLE_FAILURE;
        	}    
    	}
    }
    else
    {
        if ( bspUicc_Phy_getCardInsertStatus() == BSP_UICC_LNK_CARD_NOT_PRESENT_STATUS )
        {
            status = BSP_UICC_LNK_COMMAND_STATUS_CARD_NOT_INSERTED;
        }
        else
        {    
            status = BSP_UICC_LNK_COMMAND_STATUS_UNRECOVERABLE_FAILURE;
        }      
    }

    
	   bspUicc_Phy_setDir( BSP_UICC_PHY_DIRECTION_TX ); // move this to int handler
	
	   if(clockstopDone == TRUE) // only true for status commands
	   {
          int timeOut =0;   
	      clockstopDone = FALSE;
		  timeOut =0;
			   while ((BSP_UICC_PHY_LLIF_GET(BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_SM,
								BSP_UICC_PHY_MAP_REG_USIM_DEBUG_REG_PTR) !=
								BSP_UICC_PHY_LLIF_DEBUG_MAIN_STATE_CLOCK_STOP)
									)
			   {
				   timeOut++;
				   NU_Sleep(1); // wait till clock stop is not done
			   };
		  bspUicc_sClk_running = FALSE; // allow PM sleep
	   } 
    return status;
}

/*=============================================================================
 *   Public Functions
 *============================================================================*/

/*=============================================================================
 * Description:
 *   General interface to write any T0 command. Process any T=0 specific response
 *  from the card. i.e. if the card asks the software to read a known length
 *  of data, read it here using the GetResponse command.
 */
BspUicc_Lnk_T0ReturnCode
bspUicc_Lnk_t0WriteCommand( BspUicc_Lnk_CommandClass          class,
                            BspUicc_Lnk_CommandInstruction    instruction,
                            BspUicc_Lnk_CommandParameter1     parameter1,
                            BspUicc_Lnk_CommandParameter2     parameter2,
                            BspUicc_Lnk_CommandWriteDataSize  writeDataSize,
                            BspUicc_Lnk_CommandWriteData      *writeDataPtr,
                            BspUicc_Lnk_CommandReadDataSize   readDataSize,
                            BspUicc_Lnk_CommandReadData       *readDataPtr,
                            BspUicc_Lnk_CommandReadDataSize   *resultSizePtr,
                            BspUicc_Lnk_CommandType           commandType )
{
    BspUicc_Lnk_T0ReturnCode        status = BSP_UICC_LNK_T0_RETURN_CODE_SUCCESS;
    BspUicc_Lnk_CommandReadDataSize     getResponseSize=0;
    BspUicc_Lnk_CommandReadMoreDataFlag bspMoreDataFlag =  BSP_UICC_LNK_COMMAND_MORE_DATA_REQUIRED ;

  

   
    status = bspUicc_Lnk_t0ProcessApdu( class,
                                        instruction,
                                        parameter1,
                                        parameter2,
                                        writeDataSize,
                                        writeDataPtr,
                                        readDataSize,
                                        readDataPtr,
                                        resultSizePtr,
                                        commandType );

     if( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_APDU_SW1, bspUicc_Lnk_t0StatusBytes )
         == BSP_UICC_LNK_MSG_UTIL_APDU_SW1_CORRECT_LEN_PROC_VALUE )
     {
         /* Repeat the same command with the correct length */
         getResponseSize = BSP_UICC_LNK_MSG_UTIL_GET(
             BSP_UICC_LNK_MSG_UTIL_APDU_SW2, bspUicc_Lnk_t0StatusBytes );
        
         if( getResponseSize > readDataSize )
         {
             getResponseSize = readDataSize; 
         }

        
         status = bspUicc_Lnk_t0ProcessApdu( class,
                                             instruction,
                                             parameter1,
                                             parameter2,
                                             writeDataSize,
                                             writeDataPtr,
                                             getResponseSize, /* Correct Size */
                                             readDataPtr,
                                             resultSizePtr,
                                             commandType );
            
     }
     while( ( ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_APDU_SW1, bspUicc_Lnk_t0StatusBytes )
               ==  BSP_UICC_LNK_MSG_UTIL_APDU_SW1_MORE_DATA_GSM_PROC_VALUE  ) ||
             ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_APDU_SW1, bspUicc_Lnk_t0StatusBytes )
               ==  BSP_UICC_LNK_MSG_UTIL_APDU_SW1_MORE_DATA_PROC_VALUE ) )
           && ( bspMoreDataFlag == BSP_UICC_LNK_COMMAND_MORE_DATA_REQUIRED))
    {

     	if( ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_APDU_SW1, bspUicc_Lnk_t0StatusBytes )
          	== BSP_UICC_LNK_MSG_UTIL_APDU_SW1_MORE_DATA_PROC_VALUE ) ||
         	( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_APDU_SW1, bspUicc_Lnk_t0StatusBytes )
           	== BSP_UICC_LNK_MSG_UTIL_APDU_SW1_MORE_DATA_GSM_PROC_VALUE ) )
     	{                           /*  */
         	/* The card is asking for a get response procedure */
         	getResponseSize = BSP_UICC_LNK_MSG_UTIL_GET(
             	BSP_UICC_LNK_MSG_UTIL_APDU_SW2, bspUicc_Lnk_t0StatusBytes );
         	if( ( readDataSize != BSP_UICC_LNK_T0_NO_DATA_TO_READ_FROM_CARD ) &&
             	( readDataPtr != NULL ) )
         	{
             
               
             	status = bspUicc_Lnk_t0ProcessApdu( class,
                                                 BSP_UICC_LNK_COMMAND_INSTRUCTION_GET_RESPONSE,
                                                 BSP_UICC_CMD_PARAMETER_NOT_CURRENTLY_USED,
                                                 BSP_UICC_CMD_PARAMETER_NOT_CURRENTLY_USED,
                                                 BSP_UICC_NO_DATA_WRITE_VALUE,
                                                 NULL,
                                                 getResponseSize,
                                                 readDataPtr,
                                                 resultSizePtr,
                                                 BSP_UICC_LNK_COMMAND_TYPE_2 );
         	}
         	else
         	{
         		/* Create a 16 bit result value 61XX */
         		status = ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_APDU_SW1,
                                               bspUicc_Lnk_t0StatusBytes )
                    << BSP_UICC_LNK_MSG_UTIL_APDU_SW1_BIT_WIDTH );
        
         		status |= ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_APDU_SW2,
                                                bspUicc_Lnk_t0StatusBytes ) );  
         		bspMoreDataFlag = BSP_UICC_LNK_COMMAND_MORE_DATA_NOT_REQUIRED;
         	}    
     	}

    }
    
       
       

    return status;
}




