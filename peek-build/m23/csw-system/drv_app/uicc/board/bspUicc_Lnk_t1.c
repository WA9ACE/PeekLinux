/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */

#include "sys_types.h"
#include "types.h"
#include "general.h"
#include "bspUicc_Phy.h"
#include "bspUicc_Lnk_msgUtil.h"
#include "bspUicc_Lnk_command.h"
#include "bspUicc_Lnk_t1.h"
#include "bspUicc_Lnk_dataConfig.h"
#include "stdlib.h"
/*=============================================================================
 *  File Description:
 *  Source File with code specific to the T1 or Block protocol
 */

/*=============================================================================
 *   Defines
 *============================================================================*/

/*=============================================================================
 *   Description
 *   These are the defines used specifically for the T1 protocol
 */
#define BSP_UICC_LNK_T1_S_BLOCK_INFO_DATA_NUM_OCTETS             1

#define BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS                3

#define BSP_UICC_LNK_T1_BLOCK_EPILOGUE_NUM_OCTETS                1

#define BSP_UICC_LNK_T1_READ_DATA_SIZE_NUM_OCTETS                1

#define BSP_UICC_LNK_T1_BLOCK_MAX_NUM_OCTETS                     254

#define BSP_UICC_LNK_T1_BLOCK_MIN_NUM_OCTETS                     0

#define BSP_UICC_LNK_T1_BLOCK_MAX_COMMAND_DATA_NUM_OCTETS        260

#define BSP_UICC_LNK_T1_S_BLOCK_MAX_NUM_OCTETS                   5 

#define BSP_UICC_LNK_T1_R_BLOCK_MAX_NUM_OCTETS                   4

#define BSP_UICC_LNK_T1_R_BLOCK_INF_NUM_OCTETS                   0

#define BSP_UICC_LNK_T1_BLOCK_READ_MAX_RETRIES                   3

#define BSP_UICC_LNK_T1_BLOCK_RESYNCH_MAX_RETRIES                3

#define BSP_UICC_LNK_T1_MAX_BLOCK_WAIT_TIME_VALUE                0x7FFFFF




/*=============================================================================
 *   Types and Enumerations
 *============================================================================*/

/*=============================================================================
 *  Description
 *  This type specifies the kind of block
 */
enum
{
    BSP_UICC_LNK_T1_BLOCK_INVALID      = 0,
    BSP_UICC_LNK_T1_BLOCK_TYPE_I       = 1,
    BSP_UICC_LNK_T1_BLOCK_TYPE_S       = 2,
    BSP_UICC_LNK_T1_BLOCK_TYPE_R       = 3
};
typedef SYS_WORD8 BspUicc_Lnk_T1BlockType;

/*=============================================================================
 *  Description
 *  This type specifies the kind of block
 */
enum
{
    BSP_UICC_LNK_T1_S_BLOCK_TYPE_ABORT        = 0,
    BSP_UICC_LNK_T1_S_BLOCK_TYPE_RESYNCH      = 1,
    BSP_UICC_LNK_T1_S_BLOCK_TYPE_VPP_ERROR    = 2,
    BSP_UICC_LNK_T1_S_BLOCK_TYPE_IFS          = 3,
    BSP_UICC_LNK_T1_S_BLOCK_TYPE_WTX          = 4
};
typedef SYS_UWORD8 BspUicc_Lnk_T1SBlockType;

/*=============================================================================
 *  Description
 *  This type specifies any data in a block
 */
typedef SYS_UWORD8 BspUicc_Lnk_T1BlockData;

/*=============================================================================
 *  Description
 *  This type specifies the size of a block
 */
typedef SYS_UWORD16 BspUicc_Lnk_T1BlockSize;

/*=============================================================================
 *  Description
 *  This type specifies the Block Wait time Extension value
 */
typedef SYS_UWORD8 BspUicc_Lnk_T1BwtExt;

/*=============================================================================
 *  Description
 *  This type specifies the size of the information field in a block
 */
typedef SYS_WORD16 BspUicc_Lnk_T1BlockInfSize;

/*=============================================================================
 *  Description
 *  This type specifies the sequence number of a block
 */
typedef SYS_UWORD8 BspUicc_Lnk_T1BlockSeqNum;

/*=============================================================================
 *  Description
 *  This type specifies the existence of more blocks
 */
enum
{
    BSP_UICC_LNK_T1_MORE_BLOCKS_NOT_PRESENT       = 0,
    BSP_UICC_LNK_T1_MORE_BLOCKS_PRESENT           = 1
};
typedef SYS_UWORD8 BspUicc_Lnk_T1MoreBlocks;
/*=============================================================================
 *  Description
 *  This type specifies the existence of more blocks
 */
enum
{
    BSP_UICC_LNK_T1_FIRST_COMMAND       = 0,
    BSP_UICC_LNK_T1_NOT_FIRST_COMMAND   = 1
};
typedef SYS_UWORD8 BspUicc_Lnk_T1FirstCommandInd;
/*=============================================================================
 *  Description
 *  This type is to check for the first command to initialize sequence variables
 */
/*enum
{
    BSP_UICC_LNK_T1_FIRST_COMMAND               = 0,
    BSP_UICC_LNK_T1_NOT_FIRST_COMMAND           = 1
};
typedef SYS_UWORD8 BspUicc_Lnk_T1FirstCommandIndicator;
*/
/*=============================================================================
 *  Description
 *  This type specifies the error in the R Block
 */
typedef SYS_UWORD8 BspUicc_Lnk_T1RBlockError;

/*=============================================================================
 *  Description
 *  This type specifies the direction of a S block
 */
typedef SYS_UWORD8 BspUicc_Lnk_T1SBlockDirection;

/*=============================================================================
 *  Description
 *  This type specifies the information field in the PCB of a S block
 */
typedef SYS_UWORD8 BspUicc_Lnk_T1SBlockInfo;

/*=============================================================================
 *  Description
 *  This type specifies the actual information data of an S block
 */
enum
{
    BSP_UICC_LNK_T1_S_BLOCK_INFO_DATA_NOT_PRESENT   = 0 
};
typedef SYS_UWORD8 BspUicc_Lnk_T1SBlockInfoData;

/*============================================================================= 
 *  Description
 *  Specifies the type for holding the Link layer control data
 */
typedef struct 
{
    BspUicc_Lnk_T1BlockSeqNum        sendSeqNum;
    BspUicc_Lnk_T1BlockSeqNum        receiveSeqNum;
    BspUicc_Lnk_T1MoreBlocks         moreBlocks;
    BspUicc_Lnk_T1BlockInfSize       readBlockInfSize;
    BspUicc_Lnk_T1BlockSize          resultDataSize;
}
BspUicc_Lnk_T1Control;


/*=============================================================================
 *   Public Variables
 *============================================================================*/
/*============================================================================= 
 *  Description
 *  Contains the current control information for the block protocol
 */
BspUicc_Lnk_T1Control bspUicc_Lnk_t1Control;

/*============================================================================= 
 *  Description
 *  Buffer to hold all the  data being written to the card
 */
BspUicc_Lnk_T1BlockData bspUicc_Lnk_T1BlockDataWriteBuffer[BSP_UICC_LNK_T1_BLOCK_MAX_COMMAND_DATA_NUM_OCTETS];

/*============================================================================= 
 *  Description
 *  Buffer for the  block being written or transmitted
 */
BspUicc_Lnk_T1BlockData bspUicc_Lnk_T1BlockDataTxBuffer[BSP_UICC_LNK_T1_BLOCK_MAX_COMMAND_DATA_NUM_OCTETS];


/*============================================================================= 
 *  Description
 *  Buffer for the block being read or received
 */
BspUicc_Lnk_T1BlockData bspUicc_Lnk_T1BlockDataRxBuffer[BSP_UICC_LNK_T1_BLOCK_MAX_NUM_OCTETS];

/*============================================================================= 
 *  Description
 *  Buffer to hold the status bytes
 */
BspUicc_Lnk_T1BlockData bspUicc_Lnk_T1StatusBytes[BSP_UICC_LNK_COMMAND_STATUS_BYTES_NUM_OCTETS];

/*=============================================================================
 *  Description
 *  Indicator to check for the first command to initialize sequence variables
 */
BspUicc_Lnk_T1FirstCommandInd firstCommandInd = BSP_UICC_LNK_T1_FIRST_COMMAND;



/*=============================================================================
 *   Private Functions
 *============================================================================*/
 
/*=============================================================================
 * Description:
 *   Function to build a command header
 *
 * Parameters
 *   dataPtr        -  Pointer to the command header
 *   class          -  Class of the command
 *   instruction    -  Instruction of the command
 *   parameter1     -  Command Parameter 1
 *   parameter2     -  Command Parameter 2
 *   parameter3     -  Command Parameter 3
 *   param3Flag     -  Flag that specifies if the parameter 3 needs to be sent
 *                     Case 1 commands do not need parameter3
 *
 * Returns
 *   None
 *
 */
static void
bspUicc_Lnk_t1BuildCommandHeader( BspUicc_Lnk_CommandWriteData      *dataPtr,
                                  BspUicc_Lnk_CommandClass          class,
                                  BspUicc_Lnk_CommandInstruction    instruction,
                                  BspUicc_Lnk_CommandParameter1     parameter1,
                                  BspUicc_Lnk_CommandParameter2     parameter2,
                                  BspUicc_Lnk_CommandParameter3     parameter3,
                                  BspUicc_Lnk_CommandParam3Flag     param3Flag )
{
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_CLA,
                           dataPtr,
                           class );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_INS,
                           dataPtr,
                           instruction );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_P1,
                           dataPtr,
                           parameter1 );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_P2,
                           dataPtr,
                           parameter2 );

    if( param3Flag == BSP_UICC_LNK_COMMAND_PARAM3_PRESENT  )
    {
        BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_APDU_P3,
                               dataPtr,
                               parameter3 );
    }
    
}

/*=============================================================================
 * Description:
 *   Function to compute the check byte/epilogue for a block
 *
 * Parameters
 *   blockDataPtr       -  Pointer to the block whose check byte is to be calculated
 *   blockSize          -  Size of the block
 *
 * Returns
 *   checkByte          -  The check byte calculated for the block
 *
 */
static BspUicc_Lnk_T1BlockData
bspUicc_Lnk_t1ComputeCheckByte( BspUicc_Lnk_T1BlockData   *blockDataPtr,
                             BspUicc_Lnk_T1BlockSize   blockSize )
{
    SYS_UWORD16                        i;
    BspUicc_Lnk_T1BlockData         checkByte;

    /* Compute the check byte for the block */
    checkByte = 0;
    for( i=0; i<blockSize; i++)
    {
        checkByte ^= ( *blockDataPtr );
        blockDataPtr++;
        
    }
    return checkByte;
}

/*=============================================================================
 * Description:
 *   Function to increment the send block sequence number
 * 
 * Parameters
 *   None
 *
 * Returns
 *   None
 *
 */
static void
bspUicc_Lnk_t1BumpSendSeqNum( void )
{
    if( bspUicc_Lnk_t1Control.sendSeqNum == 0 )
    {
        bspUicc_Lnk_t1Control.sendSeqNum = 1;
    }
    else
    {
        bspUicc_Lnk_t1Control.sendSeqNum = 0;
    }
}

/*=============================================================================
 * Description:
 *   Function to increment the receive block sequence number
 * 
 * Parameters
 *   None
 *
 * Returns
 *   None
 *
 */
static void
bspUicc_Lnk_t1BumpReceiveSeqNum( void )
{
    if( bspUicc_Lnk_t1Control.receiveSeqNum == 0 )
    {
        bspUicc_Lnk_t1Control.receiveSeqNum = 1;
    }
    else
    {
        bspUicc_Lnk_t1Control.receiveSeqNum = 0;
    }
}

/*=============================================================================
 * Description:
 *   Function to initialize a block header, Block buffer will have the
 *  previously sent block's header
 * 
 * Parameters
 *   Pointer to the block start address
 *
 * Returns
 *   None
 *
 */
void
bspUicc_Lnk_t1InitBlockHeader( BspUicc_Lnk_T1BlockData   *blockDataPtr )
{
    SYS_UWORD8     i;
    
    for( i=0; i<BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;i++ )
    {
        *blockDataPtr = 0x00;
        blockDataPtr++;
    }   
}

/*=============================================================================
 * Description:
 *   Function to build and Send a I block
 *
 * Parameters
 *   sendSeqNum     -  Sequence number of the block being sent
 *   moreBlocks     -  Indicate the presence of more blocks to be sent
 *   infPtr         -  Pointer to the information part of the block
 *   infSize        -  Size of the information being sent
 *
 * Return
 *   status         -  Contains the number of bytes sent by physical layer
 *
 */
static BspUicc_Lnk_T1ReturnCode
bspUicc_Lnk_t1SendIBlock( BspUicc_Lnk_T1BlockSeqNum    sendSeqNum,
                          BspUicc_Lnk_T1MoreBlocks     moreBlocks,
                          BspUicc_Lnk_T1BlockData      *infPtr,
                          SYS_UWORD16                       infSize )
{
    SYS_UWORD8                          i;
    SYS_UWORD16                         txDataSize;
    BspUicc_Lnk_T1BlockData        checkByte;
    BspUicc_Lnk_T1ReturnCode       status;
    BspUicc_Lnk_T1BlockData        *blockDataPtr;
    

    blockDataPtr = bspUicc_Lnk_T1BlockDataTxBuffer;

    bspUicc_Lnk_t1InitBlockHeader( blockDataPtr );
    
    /* Set the Prologue of the block */
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD,
                               blockDataPtr,
                               BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD_VALUE );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD,
                               blockDataPtr,
                               BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD_VALUE );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_TYPE,
                               blockDataPtr,
                               BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_TYPE_VALUE );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_SEQ_NUM,
                               blockDataPtr,
                               sendSeqNum );

    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_MORE_DATA,
                               blockDataPtr,
                               moreBlocks );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH,
                               blockDataPtr,
                               infSize );
    
    blockDataPtr+= BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;

    
    /* Copy the Information field to the Block buffer */
    for( i=0; i<infSize; i++ )
    {
        *blockDataPtr = *infPtr;
        blockDataPtr++;
        infPtr++;
    }
    txDataSize = ( infSize + BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS );
    
    checkByte = bspUicc_Lnk_t1ComputeCheckByte( bspUicc_Lnk_T1BlockDataTxBuffer,
                                                txDataSize );
    /* Set the Epilogue of the block */        
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_BLOCK_EPILOGUE,
                               blockDataPtr,
                               checkByte );

    txDataSize++;

    status = bspUicc_Phy_write( txDataSize, bspUicc_Lnk_T1BlockDataTxBuffer );

    return status;
    
}

/*=============================================================================
 * Description:
 *   Function to build and Send a R block
 *
 * Parameters
 *   sendSeqNum     -  Sequence number of the block being sent
 *   error          -  Error value if any to be sent in the response block
 * 
 * Return
 *   status         -  Contains the number of bytes sent by physical layer
 *
 */
static BspUicc_Lnk_T1ReturnCode
bspUicc_Lnk_t1SendRBlock( BspUicc_Lnk_T1BlockSeqNum  sendSeqNum,
                          BspUicc_Lnk_T1RBlockError  error )
{
    BspUicc_Lnk_T1BlockData        *blockDataPtr;
    BspUicc_Lnk_T1BlockData        checkByte;
    BspUicc_Lnk_T1ReturnCode       status;
    SYS_UWORD8                          txDataSize;

    blockDataPtr = bspUicc_Lnk_T1BlockDataTxBuffer;
    
    bspUicc_Lnk_t1InitBlockHeader( blockDataPtr );
    
    /* Set the Prologue of the block */
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD,
                               blockDataPtr,
                               BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD_VALUE );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD,
                               blockDataPtr,
                               BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD_VALUE );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_TYPE,
                               blockDataPtr,
                               BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_TYPE_VALUE );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_SEQ_NUM,
                               blockDataPtr,
                               sendSeqNum );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_ERR_IND,
                               blockDataPtr,
                               error );
    
    /* R block has no Inf field, length set to zero */
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH,
                               blockDataPtr,
                               BSP_UICC_LNK_T1_R_BLOCK_INF_NUM_OCTETS );
    
    checkByte = bspUicc_Lnk_t1ComputeCheckByte( blockDataPtr,
                                                BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS );
    
    blockDataPtr += BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;
    
    /* Set the Epilogue of the block */        
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_BLOCK_EPILOGUE,
                               blockDataPtr,
                               checkByte );

    /* R Block Size is prologue + epilogue */
    txDataSize = ( BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS +
                   BSP_UICC_LNK_T1_BLOCK_EPILOGUE_NUM_OCTETS );

    status = bspUicc_Phy_write( txDataSize, bspUicc_Lnk_T1BlockDataTxBuffer );

    return status;  
    
}

/*=============================================================================
 * Description:
 *   Function to build and Send a S block
 *
 * Parameters
 *   blockDirection -  Indicates Request or Response direction
 *   blockInfo      -  Information for the PCB to determines the type of S Block
 *   infoData       -  Information carried by the S Block, max 1 byte according to spec
 *   
 * Return
 *   status         -  Contains the number of bytes sent by physical layer
 *
 */
static BspUicc_Lnk_T1ReturnCode
bspUicc_Lnk_t1SendSBlock( BspUicc_Lnk_T1SBlockDirection   blockDirection,
                          BspUicc_Lnk_T1SBlockInfo        blockInfo,
                          BspUicc_Lnk_T1SBlockInfoData    infoData )
{
    SYS_BOOL                           infoDataPresent = FALSE;
    SYS_UWORD8                          txDataSize;
    BspUicc_Lnk_T1BlockData        checkByte;
    BspUicc_Lnk_T1ReturnCode       status;
    BspUicc_Lnk_T1BlockData        *blockDataPtr;

    blockDataPtr = bspUicc_Lnk_T1BlockDataTxBuffer;

    bspUicc_Lnk_t1InitBlockHeader( blockDataPtr );
    
    /* Set the Prologue of the block */
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD,
                               blockDataPtr,
                               BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD_VALUE );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD,
                               blockDataPtr,
                               BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD_VALUE );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_TYPE,
                               blockDataPtr,
                               BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_TYPE_VALUE );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION,
                               blockDataPtr,
                               blockDirection );
    
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO,
                               blockDataPtr,
                               blockInfo );
    
    if( ( blockInfo ==  BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_IFS_VALUE )
        || ( blockInfo ==  BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_WTX_VALUE ) )
    {
        BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH,
                                   blockDataPtr,
                                   BSP_UICC_LNK_T1_S_BLOCK_INFO_DATA_NUM_OCTETS );
        infoDataPresent = TRUE;
    }
    else
    {
        BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH,
                                   blockDataPtr,
                                   0 );
    }
    blockDataPtr+= BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;

    txDataSize = BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;
    
    /* Copy the Information field to the Block buffer */
    if( infoDataPresent == TRUE )
    {
        *blockDataPtr = infoData; /* contains WTX time or IFS value */
        blockDataPtr++;
        txDataSize++;
    }
    
    checkByte = bspUicc_Lnk_t1ComputeCheckByte( bspUicc_Lnk_T1BlockDataTxBuffer,
                                                txDataSize );
    /* Set the Epilogue of the block */        
    BSP_UICC_LNK_MSG_UTIL_SET( BSP_UICC_LNK_MSG_UTIL_BLOCK_EPILOGUE,
                               blockDataPtr,
                               checkByte );
    /* The Check byte needs to be sent too */
    txDataSize++;

    status = bspUicc_Phy_write( txDataSize, bspUicc_Lnk_T1BlockDataTxBuffer );

    return status;
    
}

/*=============================================================================
 * Description:
 *   Function to determine the type of any block
 *
 * Parameters
 *   blockDataPtr   -  Pointer to the read block buffer
 *   
 * Return
 *   blockType      -  The type of block to which the blockDataPtr is pointing
 *
 */
static BspUicc_Lnk_T1BlockType
bspUicc_Lnk_t1GetBlockType( BspUicc_Lnk_T1BlockData  *blockDataPtr )
{
    BspUicc_Lnk_T1BlockType         blockType = BSP_UICC_LNK_T1_BLOCK_INVALID;
    
    if( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_TYPE,
                               blockDataPtr )
        == BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_TYPE_VALUE )
    {
        blockType = BSP_UICC_LNK_T1_BLOCK_TYPE_S;
    }
    else if( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_TYPE,
                                    blockDataPtr )
             == BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_TYPE_VALUE )
    {
        blockType = BSP_UICC_LNK_T1_BLOCK_TYPE_R;
    }
    else if( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_TYPE,
                                    blockDataPtr )
             == BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_TYPE_VALUE )
    {
        blockType = BSP_UICC_LNK_T1_BLOCK_TYPE_I;
    }
    
    return blockType;
}

/*=============================================================================
 * Description:
 *   Function to check the block for errors
 *
 * Parameters
 *   blockDataPtr   -  Pointer to the read block buffer
 *   blockSize      -  Size of the block
 *   
 * Return
 *   status         -  Contains the number of bytes received by physical layer
 *
 */
static BspUicc_Lnk_T1ReturnCode
bspUicc_Lnk_t1CheckBlock( BspUicc_Lnk_T1BlockData   *blockDataPtr,
                       BspUicc_Lnk_T1BlockSize   totalBlockSize )
{
    BspUicc_Lnk_T1ReturnCode        status;
    BspUicc_Lnk_T1BlockSize         configBlockSize;
    BspUicc_Lnk_T1BlockData         checkByte = 0;
    BspUicc_Lnk_T1BlockType         blockType;
    SYS_UWORD8                        lengthInPrologue;
    SYS_UWORD8                        i;
        

    status = BSP_UICC_LNK_T1_RETURN_CODE_SUCCESS;
    
    /* Check for Length consistency */
    lengthInPrologue = BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH,
                                              blockDataPtr );

    configBlockSize = bspUicc_Lnk_dataConfigGetBlockSize();

    if( lengthInPrologue > configBlockSize )
    {
        status = BSP_UICC_LNK_T1_RETURN_CODE_LENGTH_INVALID;
    }
    
    if( ( lengthInPrologue +
          BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS +
          BSP_UICC_LNK_T1_BLOCK_EPILOGUE_NUM_OCTETS ) !=
        totalBlockSize )
    {
        status = BSP_UICC_LNK_T1_RETURN_CODE_LENGTH_INVALID;
    }

    if( bspUicc_Lnk_t1GetBlockType( blockDataPtr ) == BSP_UICC_LNK_T1_BLOCK_INVALID )
    {
        status = BSP_UICC_LNK_T1_RETURN_CODE_PCB_INVALID;
    }

    if( ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD,
                                     blockDataPtr)
          ==  BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_NAD_DAD,
                                     blockDataPtr) )
         &&
        ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD,
                                    blockDataPtr) !=  BSP_UICC_LNK_MSG_UTIL_T1_NAD_SAD_VALUE)
      )
    {
        status = BSP_UICC_LNK_T1_RETURN_CODE_NAD_INVALID;
    }

    /* Check for PCB block errors */
     blockType = bspUicc_Lnk_t1GetBlockType( blockDataPtr );

     if( blockType ==  BSP_UICC_LNK_T1_BLOCK_TYPE_R )
     {
         if( ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_UNDEF,
                                           blockDataPtr) !=
                                     BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_UNDEF_VALUE )
             ||
             ( ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_ERR_IND,
                                            blockDataPtr) !=
                                     BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_NO_ERROR_VALUE )
               &&
               ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_ERR_IND,
                                            blockDataPtr) !=
                                     BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_EDC_ERROR_VALUE )
               &&
               ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_ERR_IND,
                                            blockDataPtr) !=
                                     BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_OTHER_ERROR_VALUE )
             )
             ||
             (
                 BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH,
                                             blockDataPtr ) !=  BSP_UICC_LNK_T1_R_BLOCK_INF_NUM_OCTETS
             )
           )
         {
             status = BSP_UICC_LNK_T1_RETURN_CODE_PCB_INVALID;
         }
     }

     
     
     if( blockType ==  BSP_UICC_LNK_T1_BLOCK_TYPE_S )
     {
       if(( ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO,
                                           blockDataPtr) !=
               BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_RESYNCH_VALUE )
            &&
             ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO,
                                          blockDataPtr) !=
               BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_IFS_VALUE )
            &&
            ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO,
                                           blockDataPtr) !=
              BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_ABORT_VALUE )
            &&
            ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO,
                                         blockDataPtr) !=
              BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_WTX_VALUE )
            &&
            ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO,
                                         blockDataPtr) !=
              BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_VPP_ERROR_VALUE )
          )
          ||
          (  ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO,
                                          blockDataPtr) ==
              BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_VPP_ERROR_VALUE )
             &&
             ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION,
                                          blockDataPtr) !=
               BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_RSP_VALUE )
          )
          ||
          (  ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO,
                                         blockDataPtr) ==
              BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_WTX_VALUE )
             &&
             ( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION,
                                          blockDataPtr) ==
               BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_RSP_VALUE )
          
           )
         )
         {
             status = BSP_UICC_LNK_T1_RETURN_CODE_PCB_INVALID;
         }
     }
     
    /* physical layer should not have reported any timeout error or other errors */
    if( totalBlockSize > 0)
    {
        /* do a EDC check on the block and return error status if check fails */
        for( i = 0; i< totalBlockSize; i++)
        {
            checkByte ^= *blockDataPtr;
            blockDataPtr++;
        }
        if( checkByte != 0 )
        {
            status = BSP_UICC_LNK_T1_RETURN_CODE_EDC_ERROR;
        }
    }
    return status;
}   

/*=============================================================================
 * Description:
 *   Function to Read block of any type
 *
 * Parameters
 *   blockDataPtr   -  Pointer to the read block buffer
 *   
 * Return
 *   status         -  Contains the bytes received or any error code
 *
 */
static BspUicc_Lnk_T1ReturnCode
bspUicc_Lnk_t1GetBlock( BspUicc_Lnk_T1BlockData  *blockDataPtr )
{
    BspUicc_Lnk_T1BlockInfSize      infSize;
    BspUicc_Lnk_T1ReturnCode        readStatus;
    BspUicc_Lnk_T1ReturnCode        checkStatus = BSP_UICC_LNK_T1_RETURN_CODE_FAILURE;
    BspUicc_Lnk_T1BlockSize         totalBlockSize;
    SYS_UWORD8                           rxDataSize;
    BspUicc_Lnk_T1BlockData         *blockStartAddr;

    /* Remember the Start Address of the block */
    blockStartAddr = blockDataPtr;
    
    readStatus = BSP_UICC_LNK_T1_RETURN_CODE_FAILURE;
    
    rxDataSize = BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;
    totalBlockSize = rxDataSize;

    if( (readStatus = bspUicc_Phy_read( rxDataSize, blockDataPtr )) > 0 )
    {
        infSize = BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH,
                                             blockDataPtr );

        bspUicc_Lnk_t1Control.readBlockInfSize = infSize;
       
        
        if(( infSize <  BSP_UICC_LNK_T1_BLOCK_MIN_NUM_OCTETS )
           ||
           ( infSize > BSP_UICC_LNK_T1_BLOCK_MAX_NUM_OCTETS )
          )
              
        {
            checkStatus = BSP_UICC_LNK_T1_RETURN_CODE_LENGTH_INVALID;
        }
        else
        {
         
            blockDataPtr += BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;
    
            /* Read the Information field and epilogue of the block */
            rxDataSize = infSize + BSP_UICC_LNK_T1_BLOCK_EPILOGUE_NUM_OCTETS;

            totalBlockSize += rxDataSize;

            readStatus += bspUicc_Phy_read( rxDataSize, blockDataPtr );
            checkStatus = bspUicc_Lnk_t1CheckBlock( blockStartAddr, readStatus );
        }
    }
  

    
    if( ( readStatus > 0 ) && ( checkStatus != BSP_UICC_LNK_T1_RETURN_CODE_SUCCESS ) )
    {
        readStatus = checkStatus;
    }
    
    return readStatus;
}

/*=============================================================================
 * Description:
 *   Function to respond to a S block with another S block
 *
 * Parameters
 *   blockDataPtr   -  Pointer to the block
 *   
 * Return
 *   status         -  The type of Response sent for the received S block
 *
 */
static BspUicc_Lnk_T1SBlockType
bspUicc_Lnk_t1ProcessSBlock( BspUicc_Lnk_T1BlockData   *blockDataPtr)
{
    BspUicc_Lnk_T1BlockData         *tempDataPtr;
    BspUicc_Lnk_T1SBlockInfo        sBlockInfo;
    BspUicc_Lnk_T1BlockSize         newBlockSize = 0;
    BspUicc_Lnk_T1BlockInfSize      infSize;
    BspUicc_Lnk_T1SBlockType        status = BSP_UICC_LNK_T1_S_BLOCK_TYPE_ABORT;
    BspUicc_Lnk_DataConfig          configReq;
    SYS_BOOL                            reconfigNeeded = FALSE;
    SYS_UWORD32                          newBwt;
    SYS_UWORD8                           bwtMultiplier;
    
    tempDataPtr = blockDataPtr;

    /* Check if the S block has the request direction  */
    if( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION,
                                   blockDataPtr )
        == BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_REQ_VALUE )
    {
        sBlockInfo =  BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO,
                                                 blockDataPtr );
        infSize =  BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_BLOCK_LENGTH,
                                              blockDataPtr );
        
        if( ( sBlockInfo == BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_IFS_VALUE ) &&
            ( infSize > 0 ) )
        {
            /* The card is offering another max block information size */
            tempDataPtr += BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;
            newBlockSize =  *tempDataPtr;
            
            bspUicc_Lnk_t1SendSBlock( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_RSP_VALUE,
                                      BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_IFS_VALUE,
                                      newBlockSize );
            
            bspUicc_Lnk_dataConfigSetBlockSize( newBlockSize );
            reconfigNeeded = TRUE;
            status = BSP_UICC_LNK_T1_S_BLOCK_TYPE_IFS;
        }
        else if( (sBlockInfo == BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_WTX_VALUE ) &&
                 ( infSize > 0) )
        {
            /* Card is asking for a different block waiting time */
            tempDataPtr += BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;
            bwtMultiplier = *tempDataPtr;

            newBwt = ( bspUicc_Lnk_dataConfigGetBwt() * bwtMultiplier );

            if(newBwt > BSP_UICC_LNK_T1_MAX_BLOCK_WAIT_TIME_VALUE)
            {
                newBwt = BSP_UICC_LNK_T1_MAX_BLOCK_WAIT_TIME_VALUE;
            }
                       
            /* Calculate Block Wait time if needed */
            bspUicc_Lnk_dataConfigSetBwt( newBwt );

            reconfigNeeded = TRUE;

            bspUicc_Lnk_t1SendSBlock( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_RSP_VALUE,
                                      BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_WTX_VALUE,
                                      bwtMultiplier );

            status = BSP_UICC_LNK_T1_S_BLOCK_TYPE_WTX;
            
        }
        else if( sBlockInfo == BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_ABORT_VALUE )
        {
            bspUicc_Lnk_t1SendSBlock( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_RSP_VALUE,
                                      BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_ABORT_VALUE,
                                      BSP_UICC_LNK_T1_S_BLOCK_INFO_DATA_NOT_PRESENT  );
            /* Get out of any Chain */
            status = BSP_UICC_LNK_T1_S_BLOCK_TYPE_ABORT;
        }
    }
    else
    {
        /* S block Response sent by card, this is an error,
           send an R block so that the card can repeat the previous block */
           bspUicc_Lnk_t1SendRBlock( bspUicc_Lnk_t1Control.receiveSeqNum,
                                  BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_NO_ERROR_VALUE );
    }
    
    if( reconfigNeeded == TRUE )
    {
        /* commit the Configuration changes */
        bspUicc_Lnk_dataConfigCommitPending();

        configReq.configType = BSP_UICC_PHY_CONFIG_TYPE_BWT_UPDATE;

        /* Get the current Configuration after the commit */
        bspUicc_Lnk_dataConfigGetPhyConfig( &configReq );
           
        bspUicc_Phy_configure( configReq.protocol,
                               configReq.sclkDiv,
                               configReq.etuPeriod,
                               configReq.cgt,
                               configReq.cwt,
                               configReq.bwt,
                               configReq.fiValue,
                               configReq.diValue,
                               configReq.waitIValue,
                               configReq.clkStopInd,
                               configReq.configType );
    }
    return status;   
}

#if 0//Commenting out as the only reference to this function has been commented out.
/*=============================================================================
 * Description:
 *   Function to send a Resynch S Block and receive a S block Response
 *
 * Parameters
 *   blockDataPtr   -  Pointer to the block
 *   
 * Return
 *   readStatus     -  Indicates sucess or failure of reception
 *
 */
static BspUicc_Lnk_T1ReturnCode
bspUicc_Lnk_t1Resynch( BspUicc_Lnk_T1BlockData     *readBlockPtr )
{
    BspUicc_Lnk_T1ReturnCode        readStatus;
    BspUicc_Lnk_T1SBlockInfo        sBlockInfo = BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_ABORT_VALUE;
    BspUicc_Lnk_T1BlockType         blockType;
    SYS_BOOL                         retryFlag = FALSE;
    SYS_UWORD8                        retryCount = 0;

    while( ( retryFlag == FALSE ) && ( retryCount < BSP_UICC_LNK_T1_BLOCK_RESYNCH_MAX_RETRIES ) )
    {
        bspUicc_Lnk_t1SendSBlock( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_DIRECTION_RSP_VALUE,
                                  BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_RESYNCH_VALUE,
                                  BSP_UICC_LNK_T1_S_BLOCK_INFO_DATA_NOT_PRESENT );

        readStatus = bspUicc_Lnk_t1GetBlock( readBlockPtr );
        
        if( readStatus >= 0 )
        {
            blockType = bspUicc_Lnk_t1GetBlockType( readBlockPtr );

            if( blockType ==  BSP_UICC_LNK_T1_BLOCK_TYPE_S )
            {
                sBlockInfo =  BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO,
                                                         readBlockPtr );
            }
            
            if( ( blockType ==  BSP_UICC_LNK_T1_BLOCK_TYPE_S )
                && ( sBlockInfo == BSP_UICC_LNK_MSG_UTIL_T1_PCB_S_BLOCK_INFO_RESYNCH_VALUE ) )
            {
                bspUicc_Lnk_t1Control.sendSeqNum = 0;
                bspUicc_Lnk_t1Control.receiveSeqNum = 0;
                retryFlag = TRUE;
            }
            else
            {
                retryCount++;
                retryFlag = FALSE;              
            }
        }
        else
        {
            retryCount++;
            retryFlag = FALSE;
        }       
    }

    if( retryFlag == FALSE )
    {
        /* Resynch has failed */
        readStatus = BSP_UICC_LNK_T1_RETURN_CODE_RESYNCH_FAIL;
    }
    
    return readStatus;
}
#endif
/*=============================================================================
 * Description:
 *   Function to get an I Block or an R block
 *
 * Parameters
 *   readBlockPtr   -  Pointer to the read block buffer
 *   writeInfSize   -  Size of the information for retry
 *   writeInfPtr    -  Pointer to the data to be resent before getting the block
 *   expBlockType   -  The Type of block currently expected by the parent function
 *
 * Return
 *   status         -  Indicates sucess or failure of reception
 *
 */
static BspUicc_Lnk_T1ReturnCode
bspUicc_Lnk_t1GetIRBlock( BspUicc_Lnk_T1BlockData      *readBlockPtr,
                          BspUicc_Lnk_T1BlockData      *writeInfPtr,
                          BspUicc_Lnk_T1BlockInfSize   writeInfSize,
                          BspUicc_Lnk_T1BlockType      expBlockType,
                          BspUicc_Lnk_T1BlockSeqNum    expSeqNum)
{
    BspUicc_Lnk_T1BlockType        blockType;
    BspUicc_Lnk_T1ReturnCode       readStatus;
    SYS_BOOL                           retryFlag = FALSE;
    SYS_UWORD8                          retryCount = 0;

    

    while( ( retryFlag == FALSE ) && ( retryCount < BSP_UICC_LNK_T1_BLOCK_READ_MAX_RETRIES ) )
    {        
        readStatus = bspUicc_Lnk_t1GetBlock( readBlockPtr );

        if( readStatus >= 0 )
        {
            blockType = bspUicc_Lnk_t1GetBlockType( readBlockPtr );
        
            if( blockType ==  BSP_UICC_LNK_T1_BLOCK_TYPE_R )
            {
                if( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_SEQ_NUM,
                                               readBlockPtr )
                    == bspUicc_Lnk_t1Control.sendSeqNum )
                {
                    /* R block has same sequence number as the one that
                       was sent, Retransmit I block */
                    bspUicc_Lnk_t1SendIBlock( bspUicc_Lnk_t1Control.sendSeqNum,
                                              bspUicc_Lnk_t1Control.moreBlocks,
                                              writeInfPtr,
                                              writeInfSize );
                    retryCount++;
                }
                else
                {
                    if( expBlockType == BSP_UICC_LNK_T1_BLOCK_TYPE_R )
                    {
                        /* Received the correct Block */
                        retryFlag = TRUE;
                    }
                    else if( expBlockType == BSP_UICC_LNK_T1_BLOCK_TYPE_I )
                    {
                        /* Received R block with diff seq number */
                        /* Waiting for I block, reset the R block seq num, and send it */
                           bspUicc_Lnk_t1SendRBlock(expSeqNum,
                                                  BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_OTHER_ERROR_VALUE );
                    }
                }
            }
            else if( blockType ==  BSP_UICC_LNK_T1_BLOCK_TYPE_I )
            {
                if( expBlockType == BSP_UICC_LNK_T1_BLOCK_TYPE_I )
                {
                     if( BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_SEQ_NUM,
                                             readBlockPtr )
                          == expSeqNum )
                     {   
                       /* Received the correct Block */
                       retryFlag = TRUE;
                     }  
                     else
                     {
                        /* Received I block with wrong sequence number */
                         bspUicc_Lnk_t1SendRBlock( expSeqNum,
                                            BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_NO_ERROR_VALUE );
                     }
                
                
                }
                else if( expBlockType == BSP_UICC_LNK_T1_BLOCK_TYPE_R )
                {
                    /* Received I block while waiting for R block, send an R block
                     to get the correct R block */
                    bspUicc_Lnk_t1SendRBlock( expSeqNum,
                                              BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_NO_ERROR_VALUE );
                    
                }
            }
            else if( blockType ==  BSP_UICC_LNK_T1_BLOCK_TYPE_S )
            {
                bspUicc_Lnk_t1ProcessSBlock( readBlockPtr );
            }
        }
        else
        {
            if( readStatus == BSP_UICC_LNK_T1_RETURN_CODE_EDC_ERROR )
            {
                bspUicc_Lnk_t1SendRBlock( expSeqNum,
                                          BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_EDC_ERROR_VALUE );
            }
            else
            {
                bspUicc_Lnk_t1SendRBlock( expSeqNum,
                                          BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_OTHER_ERROR_VALUE );
            }
            retryCount++;
        }
    }

    if( retryFlag == FALSE )
    {
        /*readStatus = bspUicc_Lnk_t1Resynch( readBlockPtr );
          Dont Resynch The Orga PC crashes does not like it */
        readStatus = BSP_UICC_LNK_T1_RETURN_CODE_FAILURE;
    }
   
    return readStatus;
}           
        

/*=============================================================================
 * Description:
 *   Function to get the result of a command
 *
 * Parameters
 *   readBlockPtr   -  Pointer to the read block buffer
 *   writeInfSize   -  Size of the information for retry
 *   writeInfPtr    -  Pointer to the data to be resent before getting the block
 *   readDataPtr    -  Pointer to the buffer into which the data sent by the
 *                     card must be copied.
 *
 * Return
 *   status         -  The Procedure status bytes of the command( command Result )
 *   
 */
static BspUicc_Lnk_T1ReturnCode
bspUicc_Lnk_t1GetCommandResult( BspUicc_Lnk_T1BlockData       *readBlockPtr,
                                BspUicc_Lnk_T1BlockInfSize    writeInfSize,
                                BspUicc_Lnk_T1BlockData       *writeInfPtr,
                                BspUicc_Lnk_CommandReadData   *readDataPtr )
{
    BspUicc_Lnk_T1BlockData      *readInfPtr;
    BspUicc_Lnk_T1ReturnCode     status;
    BspUicc_Lnk_T1MoreBlocks     moreBlocks;
    BspUicc_Lnk_T1ReturnCode     readStatus;
    BspUicc_Lnk_T1BlockSeqNum    expSeqNum;
    SYS_UWORD8                     i;


    moreBlocks = BSP_UICC_LNK_T1_MORE_BLOCKS_PRESENT;
    expSeqNum = bspUicc_Lnk_t1Control.receiveSeqNum ;
    
    while( moreBlocks == BSP_UICC_LNK_T1_MORE_BLOCKS_PRESENT )
    {
        
        readStatus = bspUicc_Lnk_t1GetIRBlock( readBlockPtr,
                                               writeInfPtr,
                                               writeInfSize,
                                               BSP_UICC_LNK_T1_BLOCK_TYPE_I,
                                               expSeqNum);

        
        if( expSeqNum == 0 )
        {
            expSeqNum = 1;
        }
        else
        {
            expSeqNum = 0;
        }
        
        
        if( readStatus > 0)
        {
            moreBlocks = BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_MORE_DATA,
                                                    readBlockPtr );
            
            readInfPtr = readBlockPtr + BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;
            
            if( moreBlocks == BSP_UICC_LNK_T1_MORE_BLOCKS_PRESENT )
            {
                /* Data is copied out here when there is more data coming */
                for( i= 0; i< bspUicc_Lnk_t1Control.readBlockInfSize; i++ )
                {
                    *readDataPtr = *readInfPtr;
                    readDataPtr++;
                    readInfPtr++;
                }
                
                bspUicc_Lnk_t1Control.resultDataSize += bspUicc_Lnk_t1Control.readBlockInfSize;
                bspUicc_Lnk_t1Control.receiveSeqNum = BSP_UICC_LNK_MSG_UTIL_GET( BSP_UICC_LNK_MSG_UTIL_T1_PCB_I_BLOCK_SEQ_NUM,
                                                                                 readBlockPtr );
                bspUicc_Lnk_t1BumpReceiveSeqNum( );
                
                bspUicc_Lnk_t1SendRBlock( bspUicc_Lnk_t1Control.receiveSeqNum,
                                          BSP_UICC_LNK_MSG_UTIL_T1_PCB_R_BLOCK_NO_ERROR_VALUE );
            }
            else
            {    bspUicc_Lnk_t1BumpReceiveSeqNum( );
                 bspUicc_Lnk_t1BumpSendSeqNum(); 
            }
        }
        else
        {
           moreBlocks = BSP_UICC_LNK_T1_MORE_BLOCKS_NOT_PRESENT;
          
        }
    }

    if( readStatus > 0)
    {
        /* Extract the last 2 bytes into the command status */
        if( bspUicc_Lnk_t1Control.readBlockInfSize >= BSP_UICC_LNK_COMMAND_STATUS_BYTES_NUM_OCTETS )
        {
            /* The last block is still not copied out */
            readInfPtr = readBlockPtr + BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;
            
            /* readDataPtr may not have memory allocated for status bytes
               So, dont copy it.                                           */
            for( i= 0; i< ( bspUicc_Lnk_t1Control.readBlockInfSize -
                            BSP_UICC_LNK_COMMAND_STATUS_BYTES_NUM_OCTETS ) ; i++ )
            {
                *readDataPtr = *readInfPtr;
                readDataPtr++;
                readInfPtr++;
            }
            
            bspUicc_Lnk_t1Control.resultDataSize += ( bspUicc_Lnk_t1Control.readBlockInfSize -
                                                   BSP_UICC_LNK_COMMAND_STATUS_BYTES_NUM_OCTETS );
            readInfPtr = ( readBlockPtr +
                           BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS +
                           bspUicc_Lnk_t1Control.readBlockInfSize -
                           BSP_UICC_LNK_COMMAND_STATUS_BYTES_NUM_OCTETS );
            
            bspUicc_Lnk_T1StatusBytes[BSP_UICC_LNK_MSG_UTIL_APDU_SW1_OCTET_OFFSET] = *readInfPtr;
            readInfPtr++;
            bspUicc_Lnk_T1StatusBytes[BSP_UICC_LNK_MSG_UTIL_APDU_SW2_OCTET_OFFSET] = *readInfPtr;
        }
        /* If the block information size is 1, it means that the procedure bytes
           are spread over 2 successive blocks, handle it */
        else if( bspUicc_Lnk_t1Control.readBlockInfSize == 1 )
        {
            /* This is the last byte of the previous block */
            bspUicc_Lnk_T1StatusBytes[BSP_UICC_LNK_MSG_UTIL_APDU_SW1_OCTET_OFFSET] = *readInfPtr;
            
            /* First byte of the next block is the second procedure char */
            readInfPtr = readBlockPtr + BSP_UICC_LNK_T1_BLOCK_PROLOGUE_NUM_OCTETS;
            bspUicc_Lnk_T1StatusBytes[BSP_UICC_LNK_MSG_UTIL_APDU_SW2_OCTET_OFFSET] = *readInfPtr;     
            
        }
        
        status = ( bspUicc_Lnk_T1StatusBytes[BSP_UICC_LNK_MSG_UTIL_APDU_SW1_OCTET_OFFSET]
                   << BSP_UICC_LNK_MSG_UTIL_APDU_SW1_BIT_WIDTH );
        
        status |= bspUicc_Lnk_T1StatusBytes[BSP_UICC_LNK_MSG_UTIL_APDU_SW2_OCTET_OFFSET];
       
    }
    else
    {
        status = readStatus; /* Pass back the Error Condition */
    }
    return status;
}


/*=============================================================================
 * Description:
 *   Function to write all the blocks in the command
 *
 * Parameters
 *   writeBlockPtr  -  Pointer to the buffer that contains data to be written
 *   totalInfSize   -  Size of the data to be written
 *   readDataPtr    -  Pointer to the buffer into which the data sent by the
 *                     card must be copied.
 *
 * Return
 *   status         -  The Procedure status bytes of the command( command Result )
 *   
 */
static BspUicc_Lnk_T1ReturnCode
bspUicc_Lnk_t1WriteBlocks( BspUicc_Lnk_T1BlockData         *writeBlockPtr,
                           SYS_UWORD16                          totalInfSize,
                           BspUicc_Lnk_CommandReadData     *readDataPtr )
{
    BspUicc_Lnk_T1BlockSize           blockSize;
    BspUicc_Lnk_T1BlockData           *infPtr;
    BspUicc_Lnk_T1BlockSize           infSize = 0;
    BspUicc_Lnk_T1ReturnCode          status;
    BspUicc_Lnk_T1BlockData           *readBlockPtr;
    BspUicc_Lnk_T1BlockSeqNum         expSeqNum;
    SYS_UWORD8                          numBlocks;
    SYS_UWORD8                          sentBlockSize = 0;
    SYS_UWORD8                          writeStatus;
    SYS_UWORD8                          extraBlockSize;
    SYS_UWORD8                          i;

    infPtr = writeBlockPtr;
    readBlockPtr = bspUicc_Lnk_T1BlockDataRxBuffer;
    blockSize = bspUicc_Lnk_dataConfigGetBlockSize();

    numBlocks = totalInfSize/blockSize;
    extraBlockSize = totalInfSize % blockSize;
    expSeqNum = bspUicc_Lnk_t1Control.sendSeqNum;

    if( numBlocks > 0 )
    {
        for( i = 0; i< numBlocks; i++ )
        {
            if( ( i == numBlocks - 1 ) && ( extraBlockSize == 0 ) )
            {
                /* This is the last block */
                bspUicc_Lnk_t1Control.moreBlocks = BSP_UICC_LNK_T1_MORE_BLOCKS_NOT_PRESENT;
                
            }
            else
            {
                bspUicc_Lnk_t1Control.moreBlocks = BSP_UICC_LNK_T1_MORE_BLOCKS_PRESENT;
            }
            /* The information is split into size equal to blockSize */
            infSize = blockSize;
            writeStatus  = bspUicc_Lnk_t1SendIBlock( bspUicc_Lnk_t1Control.sendSeqNum,
                                                     bspUicc_Lnk_t1Control.moreBlocks,
                                                     infPtr,
                                                     infSize );            
            sentBlockSize += writeStatus;

            
            
            if( bspUicc_Lnk_t1Control.moreBlocks == BSP_UICC_LNK_T1_MORE_BLOCKS_PRESENT )
            {
                                                       
                if( expSeqNum == 0 )
                {
                    expSeqNum = 1;
                }
                else
                {
                    expSeqNum = 0;
                }
                status = bspUicc_Lnk_t1GetIRBlock( readBlockPtr,
                                                   infPtr,
                                                   infSize,
                                                   BSP_UICC_LNK_T1_BLOCK_TYPE_R,
                                                   bspUicc_Lnk_t1Control.receiveSeqNum);
                
                /* point to next block information */
                infPtr += blockSize;
            }
             bspUicc_Lnk_t1BumpSendSeqNum();
            
        }
    }
    
    if( extraBlockSize > 0 )
    {
        /* This is the last block */
        bspUicc_Lnk_t1Control.moreBlocks = BSP_UICC_LNK_T1_MORE_BLOCKS_NOT_PRESENT;

        infSize = extraBlockSize;
        
        sentBlockSize = bspUicc_Lnk_t1SendIBlock( bspUicc_Lnk_t1Control.sendSeqNum,
                                                  bspUicc_Lnk_t1Control.moreBlocks,
                                                  infPtr,
                                                  infSize );
   
    }
    
    status = bspUicc_Lnk_t1GetCommandResult( readBlockPtr, 
                                             infSize,
                                             infPtr,
                                             readDataPtr );
    return status;
}


/*=============================================================================
 *   Public Functions
 *============================================================================*/

//Commenting out as there is no reference to this function any more.
#if 0
static void
bspUicc_Lnk_t1Init()
{
    /* initialize all the T1 control data */
    bspUicc_Lnk_t1Control.sendSeqNum       = 0;
    bspUicc_Lnk_t1Control.receiveSeqNum    = 0;
    bspUicc_Lnk_t1Control.moreBlocks       = 0;
    bspUicc_Lnk_t1Control.readBlockInfSize = 0;
    bspUicc_Lnk_t1Control.resultDataSize   = 0;
}
#endif

/*=============================================================================
 * Description:
 *   General interface to write any T1 command
 */
BspUicc_Lnk_T1ReturnCode bspUicc_Lnk_t1WriteCommand( BspUicc_Lnk_CommandClass          class,
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
    BspUicc_Lnk_T1BlockData           *writeBlockPtr;
    BspUicc_Lnk_T1BlockData           *indexPtr;
    BspUicc_Lnk_CommandParam3Flag     param3Flag = BSP_UICC_LNK_COMMAND_PARAM3_NOT_PRESENT;
    BspUicc_Lnk_CommandParameter3     parameter3 = 0;
    BspUicc_Lnk_T1ReturnCode          status;
   
    SYS_UWORD16                            totalInfSize = 0;
    SYS_UWORD8                             i;

    bspUicc_Lnk_t1Control.resultDataSize = 0;
    writeBlockPtr = bspUicc_Lnk_T1BlockDataWriteBuffer;
    indexPtr = writeBlockPtr;

    /*   if(  firstCommandInd  ==  BSP_UICC_LNK_T1_FIRST_COMMAND  )
    {   
      bspUicc_Lnk_t1Init();
      firstCommandInd =  BSP_UICC_LNK_T1_NOT_FIRST_COMMAND ;
      
    }
    */
    
    /* evaluate The total block size*/
    if( commandType == BSP_UICC_LNK_COMMAND_TYPE_1 )
    {
        /* Case 1 command */
        param3Flag = BSP_UICC_LNK_COMMAND_PARAM3_NOT_PRESENT;
        bspUicc_Lnk_t1BuildCommandHeader( writeBlockPtr,
                                          class,
                                          instruction,
                                          parameter1,
                                          parameter2,
                                          parameter3,
                                          param3Flag );

        totalInfSize = ( BSP_UICC_LNK_COMMAND_HEADER_NUM_OCTETS -
                         BSP_UICC_LNK_T1_READ_DATA_SIZE_NUM_OCTETS );

    }
    else if( commandType == BSP_UICC_LNK_COMMAND_TYPE_2 )
    {
        /* Case 2 */
        totalInfSize = ( BSP_UICC_LNK_COMMAND_HEADER_NUM_OCTETS );
        parameter3 = readDataSize;
        param3Flag = BSP_UICC_LNK_COMMAND_PARAM3_PRESENT;
        
        bspUicc_Lnk_t1BuildCommandHeader( writeBlockPtr,
                                          class,
                                          instruction,
                                          parameter1,
                                          parameter2,
                                          parameter3,
                                          param3Flag );
    }
    else if( ( commandType == BSP_UICC_LNK_COMMAND_TYPE_4 ) ||
             ( commandType == BSP_UICC_LNK_COMMAND_TYPE_3 ) )
    {
        /* Case 3/4 command */
        parameter3 = writeDataSize;
        param3Flag = BSP_UICC_LNK_COMMAND_PARAM3_PRESENT;
        
        bspUicc_Lnk_t1BuildCommandHeader( writeBlockPtr,
                                          class,
                                          instruction,
                                          parameter1,
                                          parameter2,
                                          parameter3,
                                          param3Flag );
        
        indexPtr += BSP_UICC_LNK_COMMAND_HEADER_NUM_OCTETS;
        for( i = 0; i< writeDataSize; i++ )
        {
            *indexPtr = *writeDataPtr;
            indexPtr++;
            writeDataPtr++;
        }
        
        if( commandType == BSP_UICC_LNK_COMMAND_TYPE_4 )
        {
            *indexPtr = readDataSize;
            /* For Case 4 command the length of data expected is also sent after the command
               data is sent, so another byte is added */
            totalInfSize = ( writeDataSize +
                             BSP_UICC_LNK_COMMAND_HEADER_NUM_OCTETS +
                             BSP_UICC_LNK_T1_READ_DATA_SIZE_NUM_OCTETS );
        }
        else
        {
            totalInfSize = ( writeDataSize +
                             BSP_UICC_LNK_COMMAND_HEADER_NUM_OCTETS );
        }

    }


    status = bspUicc_Lnk_t1WriteBlocks( writeBlockPtr, totalInfSize, readDataPtr );

    /* Check if the card sent back a correct length procedure */
    if( bspUicc_Lnk_T1StatusBytes[BSP_UICC_LNK_MSG_UTIL_APDU_SW1_OCTET_OFFSET] ==
        BSP_UICC_LNK_MSG_UTIL_APDU_SW1_CORRECT_LEN_PROC_VALUE )
    {
        parameter3 = bspUicc_Lnk_T1StatusBytes[BSP_UICC_LNK_MSG_UTIL_APDU_SW2_OCTET_OFFSET];
    
        bspUicc_Lnk_t1BuildCommandHeader( writeBlockPtr,
                                          class,
                                          instruction,
                                          parameter1,
                                          parameter2,
                                          parameter3,
                                          param3Flag );
        
        status = bspUicc_Lnk_t1WriteBlocks( writeBlockPtr, totalInfSize, readDataPtr );
    }
    
    if( ( commandType == BSP_UICC_LNK_COMMAND_TYPE_2 ) ||
        ( commandType == BSP_UICC_LNK_COMMAND_TYPE_4 ) )
    {
        *resultSizePtr = bspUicc_Lnk_t1Control.resultDataSize;
    }  

   
    
    if( status <= 0)
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

   return status;
}
