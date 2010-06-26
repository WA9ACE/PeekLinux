/*=============================================================================
 *    Copyright 1996-2002 Texas Instruments Inc. All rights reserved.
 */
#include "sys_types.h"
#include "types.h"
#include "general.h"
#include "bspUicc_Lnk_command.h"
#include "bspUicc_Lnk.h"
#include "bspUicc_Lnk_msgUtil.h"
#include "bspUicc_Phy.h"
#include "bspUicc.h"
#include "bspUicc_Power.h"
#include "bspUicc_Phy_intC.h"


/*=============================================================================
 *  File Description:
 *  Source File for BSP_UICC Device driver top layer code
 */

/*=============================================================================
 *   Defines
 *============================================================================*/

Uint8 card_present=0;
#define BSP_UICC_CMD_NO_READ_DATA                      0x00
#define BSP_UICC_CMD_NO_WRITE_DATA                     0x00
#define BSP_UICC_DEFAULT_READER_ID                     0x01
#define BSP_UICC_CMD_STATUS_SUCCESS_VALUE              0x0000
#define BSP_UICC_RESET_UNKNOWN_FAILURE                 (-1) 

/*  Defines for valid values of b8 - b1 bits of uicc characteristics  
 *  other values are RFU
 */
/* UICC CHARACTERISTICS BYTE - CLOCK STOP & VOLT CLASS DECODING
 *
 * b8 b7 b6 b5 b4 b3 b2 b1    Meaning
 * -  -  -  -  X  X  -  1     Clock stop allowed
 * -  -  -  -  0  0  -  1     No preferred level
 * -  -  -  -  0  1  -  1     High level preferred
 * -  -  -  -  1  0  -  1     Low level preferred
 * -  -  -  -  1  1  -  1     RFU
 * -  -  -  -  X  X  -  0     Clock stop not allowed
 * -  -  -  -  0  0  -  0     Never
 * -  -  -  -  0  1  -  0     Unless at high
 * -  -  -  -  1  0  -  0     Unless at low
 * -  -  -  -  1  1  -  0     RFU
 * -  -  -  X  -  -  -  -     Supply voltage A
 * -  -  X  -  -  -  -  -     Supply voltage B
 * -  X  -  -  -  -  -  -     Supply voltage C
 * X  -  -  -  -  -  X  -     RFU (shall be set to 0)
 */

/*Clock stop prefernece  values after for bits b1,b3,b4 */
#define BSP_UICC_CLK_STOP_ALLOWED_PREF_NONE          0x01
#define BSP_UICC_CLK_STOP_ALLOWED_PREF_HIGH          0x05
#define BSP_UICC_CLK_STOP_ALLOWED_PREF_L0W           0x09
#define BSP_UICC_CLK_STOP_NOT_ALLOWED_NEVER          0x00
#define BSP_UICC_CLK_STOP_NOT_ALLOWED_UNLESS_HIGH    0x04
#define BSP_UICC_CLK_STOP_NOT_ALLOWED_UNLESS_L0W     0x08

/*voltage class preference values for b5, b6, b7 */
#define BSP_UICC_CLASS_A_ONLY                        0x10
#define BSP_UICC_CLASS_A_AND_B                       0x30
#define BSP_UICC_CLASS_B_ONLY                        0x20
#define BSP_UICC_CLASS_B_AND_C                       0x60
#define BSP_UICC_CLASS_C_ONLY                        0x40


#define CLOCK_STOP_ENABLE

/*=============================================================================
 *   Public Data
 *============================================================================*/

BspUicc_ReaderId                     bspUicc_readerId;
BspUicc_CardPresentDetectHandler     bspUicc_insert = NULL;
BspUicc_AtrDataInfo                  bspUicc_atr;
BspUicc_Lnk_ResetReq                 resetReq;
BspUicc_AtrSize                      bspUicc_atrSize;
BspUicc_AtrData                      bspUicc_atrData[33];
BspUicc_ResetResult                  bspUicc_resetResult;
BspUicc_ConfigChars configCharsPtr; //added for SIM CLK cutoff support

/*=============================================================================
 *   Public Functions
 *============================================================================*/


/*=============================================================================
 * Description:
 *   Initialize the Link layer of the BSP_UICC device driver 
 */
void
bspUicc_bootInit()
{
   
    bspUicc_Lnk_init();
    
}


/*=============================================================================
 * Description:
 *   Initialize the Link layer of the BSP_UICC device driver 
 */
void
bspUicc_drvRegister( BspUicc_CardPresentDetectHandler insert,
                     BspUicc_CardAbsentDetectHandler  remove )
{
    BspUicc_Lnk_InitReq  initReq;

    bspUicc_insert = insert;
    initReq.removeFunc = remove;
    
    
    bspUicc_Lnk_initReq( &initReq );
	bspUicc_Phy_intCEnableSource(BSP_UICC_PHY_INTC_SOURCE_ID_CD);
}

/*=============================================================================
 * Description:
 *   Initiate a reset of the card
 */
BspUicc_ResetResult
bspUicc_reset( BspUicc_ReaderId readerId,
               BspUicc_VoltageSelect voltageSelect )
{
    BspUicc_Lnk_ReturnCode status;
    //BspUicc_ConfigChars *configCharsPtr;
    BspUicc_ClkStopPreference clkStopPreference;
    BspUicc_VoltClassPreference voltClassPreference;
    BspUicc_SClkLev stopClockLevel;
   
    resetReq.resetDataSizePtr = &bspUicc_atrSize  ;
    resetReq.resetDataPtr = bspUicc_atrData;
    resetReq.readerId = readerId;
    bspUicc_readerId = readerId;
    resetReq.voltageSelect = voltageSelect;
    resetReq.resetNewVoltClass = FALSE;
    resetReq.newVoltClass =  BSP_UICC_POWER_LEVEL_18;  
    
    bspUicc_Lnk_dataConfigSetPhyResetConfig();
    if( readerId == BSP_UICC_DEFAULT_READER_ID )
    {
         status = bspUicc_Lnk_resetReq( &resetReq );
         if( status == BSP_UICC_RESET_UNKNOWN_FAILURE )
         {  bspUicc_resetResult =  BSP_UICC_RETURN_CODE_INVALID_CARD;
         }
         else
         {
            bspUicc_resetResult = status;
         }
         
          bspUicc_atr.atrDataPtr = resetReq.resetDataPtr;
          bspUicc_atr.atrSize = *(resetReq.resetDataSizePtr);
        
    }
    else
    {
         bspUicc_resetResult = BSP_UICC_RETURN_CODE_INVALID_CARD;   
    }

                   
    if( ( bspUicc_resetResult == BSP_UICC_RETURN_CODE_CARD_INSERTED ) &&
        (  bspUicc_insert != NULL ) )
    {
        if(( bspUicc_Lnk_ResetMgr_xiUiBytePresent() == TRUE ) ||
           (  voltageSelect ==  BSP_UICC_VOLTAGE_SELECT_OMIT ))
         {
             bspUicc_insert( &bspUicc_atr,
                             BSP_UICC_OMIT_CONFIG_CHARACTERISTICS,  
                             NULL );
         }
         else
         {
             bspUicc_insert( &bspUicc_atr,
                             BSP_UICC_REQUEST_CONFIG_CHARACTERISTICS,  
                             &configCharsPtr );

             
              /* Decoding of voltage class  prefernece requires b7 b6 b5 only 
               */
             voltClassPreference =  (configCharsPtr.clkStopPreference & 0x70);
             if(((voltClassPreference ==  BSP_UICC_CLASS_B_ONLY) ||
                 (voltClassPreference ==  BSP_UICC_CLASS_B_AND_C) ||
                 (voltClassPreference ==  BSP_UICC_CLASS_A_AND_B ))
                &&
                (bspUicc_Lnk_dataConfigGetPowerLevel() == BSP_UICC_POWER_LEVEL_18 ))
             {
                 resetReq.resetNewVoltClass =  TRUE;
                 resetReq.newVoltClass = BSP_UICC_POWER_LEVEL_3;
                 status = bspUicc_Lnk_resetReq(&resetReq); 
                

                 if (status == BSP_UICC_RESET_UNKNOWN_FAILURE)
                 {
                     bspUicc_resetResult = BSP_UICC_RETURN_CODE_INVALID_CARD;
                 }
                 else
                 {
                     bspUicc_resetResult = status;
                 }
             }

#ifdef CLOCK_STOP_ENABLE             
             /* Decoding of clock stop prefernece requires b4 b3 b1 only 
              * refer to bspUicc.h for more details */
             clkStopPreference = (configCharsPtr.clkStopPreference & 0x0D);
             
             switch ( clkStopPreference )
             {
                   case  BSP_UICC_CLK_STOP_ALLOWED_PREF_NONE :
                   {
                       /* Default prefernce to LOW level */
                       stopClockLevel = BSP_UICC_STOP_CLOCK_LEVEL_LOW;
                       break;
                   }
                   case  BSP_UICC_CLK_STOP_ALLOWED_PREF_L0W :
                   {
                       stopClockLevel = BSP_UICC_STOP_CLOCK_LEVEL_LOW;
                       break;
                   }
                   case  BSP_UICC_CLK_STOP_ALLOWED_PREF_HIGH :
                   {
                       stopClockLevel = BSP_UICC_STOP_CLOCK_LEVEL_HIGH;
                       break;
                   }
                   case BSP_UICC_CLK_STOP_NOT_ALLOWED_NEVER :
                   {
                       stopClockLevel = BSP_UICC_STOP_CLOCK_NO_STOP;
                       break;
                   }
                   case BSP_UICC_CLK_STOP_NOT_ALLOWED_UNLESS_L0W :
                   {
                       stopClockLevel = BSP_UICC_STOP_CLOCK_LEVEL_LOW;
                       break;
                   }
                    case BSP_UICC_CLK_STOP_NOT_ALLOWED_UNLESS_HIGH :
                   {
                       stopClockLevel = BSP_UICC_STOP_CLOCK_LEVEL_HIGH;
                       break;
                   }
                   default:
                   {
                       stopClockLevel = BSP_UICC_STOP_CLOCK_NO_STOP;
                       break;
                   }
                  
              }
                
             bspUicc_Lnk_dataConfigDoPhyConfigClkStop ( stopClockLevel );
             bspUicc_Lnk_dataConfigSetPrevSclkLev(stopClockLevel); 		
#endif			 
             bspUicc_insert( &bspUicc_atr,
                             BSP_UICC_OMIT_CONFIG_CHARACTERISTICS,  
                             NULL );

         	}

    }
    
    if(bspUicc_resetResult == BSP_UICC_RETURN_CODE_CARD_INSERTED)
		card_present=1;			
          
    return  bspUicc_resetResult; 
}


/*=============================================================================
 * Description:
 *   PowerOff the card
 */
void
bspUicc_powerOff(BspUicc_ReaderId readerId )
{
    bspUicc_Lnk_powerOffReq();
	bspUicc_Phy_intCDisableSource(BSP_UICC_PHY_INTC_SOURCE_ID_CD);
}

/*=============================================================================
 * Description:
 *   Exchange APDU command
 */
BspUicc_CmdResult
bspUicc_xchApdu( BspUicc_ReaderId                   cmdReaderId,
                 BspUicc_CmdHeader                  cmdHeader,
                 BspUicc_CmdWriteDataInfo           cmdWriteData,
                 BspUicc_CmdReadDataInfo            cmdReadData
               )
{
    BspUicc_CmdResult                 status;
    BspUicc_Lnk_SendCommandReq        sendCommandReq;
    
    status =  BSP_UICC_CMD_STATUS_SUCCESS_VALUE ;
    
    sendCommandReq.class         = cmdHeader.cmdClass;
    sendCommandReq.instruction   = cmdHeader.cmdInstruction;
    sendCommandReq.parameter1    = cmdHeader.cmdParameter1;
    sendCommandReq.parameter2    = cmdHeader.cmdParameter2;
    sendCommandReq.readDataPtr   = cmdReadData.cmdReadDataPtr;
    sendCommandReq.writeDataSize = cmdWriteData.cmdWriteDataSize;
    sendCommandReq.writeDataPtr  = cmdWriteData.cmdWriteDataPtr;
    sendCommandReq.resultSizePtr   = cmdReadData.cmdResultSizePtr;

    if( cmdReadData.cmdReadDataSize == BSP_UICC_CMD_UNKNOWN_RESULT_SIZE )
    {
        sendCommandReq.readDataSize = BSP_UICC_CMD_MAX_RESULT_SIZE;
    }
    else
    {
        sendCommandReq.readDataSize  = cmdReadData.cmdReadDataSize;
    }   

        
    if( sendCommandReq.readDataSize ==  BSP_UICC_CMD_NO_READ_DATA )
    {   
        if( sendCommandReq.writeDataSize == BSP_UICC_CMD_NO_WRITE_DATA )
        {    
            sendCommandReq.commandType = BSP_UICC_LNK_COMMAND_TYPE_1;
        }
        else
        {
            sendCommandReq.commandType = BSP_UICC_LNK_COMMAND_TYPE_3;
        }
    }
    else
    {   
        if( sendCommandReq.writeDataSize == BSP_UICC_CMD_NO_WRITE_DATA )
        {    
            sendCommandReq.commandType = BSP_UICC_LNK_COMMAND_TYPE_2;
        }
        else
        {
            sendCommandReq.commandType = BSP_UICC_LNK_COMMAND_TYPE_4;
        } 
    }


   /* Need to add more error checks */ 
    if( bspUicc_readerId != BSP_UICC_DEFAULT_READER_ID )
    {
         status = BSP_UICC_CMD_STATUS_ERR_NOT_RESET;
    }
    else if( (
               ( sendCommandReq.readDataSize != BSP_UICC_CMD_NO_READ_DATA  ) &&
               ( sendCommandReq.readDataPtr == NULL)
             )
                 ||
             (
               ( sendCommandReq.writeDataSize != BSP_UICC_CMD_NO_WRITE_DATA ) &&
               ( sendCommandReq.writeDataPtr == NULL )
             )
                ||
             (
                sendCommandReq.readDataSize > BSP_UICC_CMD_MAX_RESULT_SIZE 
             )
              ||
             (
                sendCommandReq.writeDataSize > BSP_UICC_CMD_MAX_DATA_SIZE 
             )
           )
    {
         status =  BSP_UICC_CMD_STATUS_ERR_PARAM_WRONG ; 
    }   
         

    if(  bspUicc_resetResult !=  BSP_UICC_RETURN_CODE_CARD_INSERTED )
    {
        status = BSP_UICC_CMD_STATUS_ERR_NOT_RESET;
    }
                            
    if( status == BSP_UICC_CMD_STATUS_SUCCESS_VALUE )
    {
       status = bspUicc_Lnk_sendCommandReq( &sendCommandReq );
    }

    
    return status;
    
}


