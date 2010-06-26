/***************************************************************************
 *
 * File:
 *     $Workfile:card.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision:2$
 *
 * Description: Source file for the Card Reader application.
 *
 * Created:     May 6, 2002
 *
 * Copyright 1999-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, 
 * Inc.  This work contains confidential and proprietary information of 
 * Extended Systems, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "btl_config.h"
#if BTL_CONFIG_SAPS == BTL_CONFIG_ENABLED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "card.h"
#include "Bthal_fs.h"

static struct _efFileInfo {
    U8   reserved[2];                             /* Reserved           */
    U16  fileSize;                                /* File size          */
    U16  fileId;                                  /* File Id            */
    U8   fileType;                                /* File Type          */
    U8   incFlag;                                 /* Increment flag     */
    U8   access[3];                               /* Access flags       */
    U8   remLen;                                  /* Length remaining   */
    U8   efStruct;                                /* Structure of EF    */
    U8   recLen;                                  /* Length of Record   */
} EfFileInfo;

/* workaround the standard ffs */
#define _MAX_PATH 10
#define _chdir(x) 0
#define _getcwd(x, y) 1
#define size_t U32

static U8    CurrentDirectory[_MAX_PATH] = "\\\0";
static U16   CurrentDirId;
static U8    RootDirectory[_MAX_PATH] = "\\\0";
static U8    ParentDirectory[_MAX_PATH] = "..\0";
static BthalFsFileDesc *CurrentFp = 0;
/* ATR response */

static U8 Atr[] = {
    0x3F, /* Initial character TS */
    0x00, /* Format character T=0 */
};

static U8  ResponseData[280];
static U16 ResponseDataLen;
static U8  StatusData[24];
static U16 StatusLen;
static U8  ApduResult[2];
static U8 *ApduBuffer;
static U16 ApduBufferLen = 0;

/* Initialize the SIM card emulation */
void CRD_Init(void)
{
//    _getcwd(CurrentDirectory, _MAX_PATH);
    memmove(RootDirectory, CurrentDirectory, strlen((const char *)CurrentDirectory));    
    CurrentDirId = 0x3F00;
}

/* Return the Sim Card ATR response */
void CRD_GetAtr(U8 **AtrPtr, U16 *Len)
{
    *AtrPtr = Atr;
    *Len = sizeof(Atr);
}

/* Parses an incoming PDU */
void CRD_ParseAPDU(U8 *ApduPtr, U16 Len, U8 **RspApduPtr, U16 *RspLen)
{
    switch (ApduPtr[1]) {
    case INS_SELECT:
        CrdSelect(ApduPtr, Len);
        break;
    case INS_STATUS:
        CrdStatus(ApduPtr, Len);
        break;
    case INS_READ_BINARY:
        CrdReadBinary(ApduPtr, Len);
        break;
    case INS_UPDATE_BINARY:
        CrdUpdateBinary(ApduPtr, Len);
        break;
    case INS_READ_RECORD:
        CrdReadRecord(ApduPtr, Len);
        break;
    case INS_UPDATE_RECORD:
        CrdUpdateRecord(ApduPtr, Len);
        break;
    case INS_SEEK:
        CrdSeek(ApduPtr, Len);
        break;
    case INS_INCREASE:
        CrdIncrease(ApduPtr, Len);
        break;
    case INS_VERIFY_CHV:
        CrdVerifyChv(ApduPtr, Len);
        break;
    case INS_CHANGE_CHV:
        CrdChangeChv(ApduPtr, Len);
        break;
    case INS_DISABLE_CHV:
        CrdDisableChv(ApduPtr, Len);
        break;
    case INS_ENABLE_CHV:
        CrdEnableChv(ApduPtr, Len);
        break;
    case INS_UNBLOCK_CHV:
        CrdUnblockChv(ApduPtr, Len);
        break;
    case INS_INVALIDATE:
        CrdInvalidate(ApduPtr, Len);
        break;
    case INS_GSM_ALGORITHM:
        CrdGsmAlgorithm(ApduPtr, Len);
        break;
    case INS_SLEEP:
        CrdSleep(ApduPtr, Len);
        break;
    case INS_RESPONSE:
        CrdResponse(ApduPtr, Len);
        break;
    case INS_TERMINAL_PROFILE:
        CrdTerminalProfile(ApduPtr, Len);
        break;
    case INS_ENVELOPE:
        CrdEnvelope(ApduPtr, Len);
        break;
    case INS_FETCH:
        CrdFetch(ApduPtr, Len);
        break;
    case INS_TERMINAL_RESPONSE:
        CrdTerminalResponse(ApduPtr, Len);
        break;
    default:
        ApduBuffer = ApduResult;
        StoreBE16(ApduBuffer, STATUS_INVALID_INS);
        ApduBufferLen = 2;
        break;
    }

    /* Set the Apdu */
    *RspApduPtr = ApduBuffer;
    *RspLen = ApduBufferLen;
}


/* Checks to see if a File ID is the parent directory of the
 * current directory.
 */
U8 _isparent(U8 *FileName)
{
    U8 *dptr = (U8 *) strrchr((const char *)CurrentDirectory, '\\');
    U8 *fptr = FileName + 3;
    I8 i;

    /* */
    if (dptr) {
        dptr--;
        for (i = 0; i < strlen((const char *)FileName); i++) {
            if (*dptr-- != *fptr--) {
                return 0;
            }
        }
        if (*dptr != '\\') {
            return 0;
        }
        return 1;
    } else {
        return 0;
    }
}

/* Sets the response data of the current DF (directory) */
void CrdSetDfParms(U16 FileId)
{
        ResponseData[0]  = 0;                           /* Reserved           */
        ResponseData[1]  = 0;                           /* Reserved           */
        ResponseData[2]  = 0xFF;                        /* Memory Available   */
        ResponseData[3]  = 0xFF;                        /* Memory Avialable   */
        StoreBE16(&ResponseData[4], FileId);            /* File Id            */

        switch (FileId >> 8) {
        case MASTER_FILE_ID:
            ResponseData[6]  = 0x01;                    /* Master File Type   */
            break;
        case LVL1_DEDICATED_ID:                         /* Dedicated File Type*/
        case LVL2_DEDICATED_ID:                         /* Dedicated File Type*/
            ResponseData[6]  = 0x02;                   
            break;
         default:
            ResponseData[6]  = 0x00;                    /* Unknown File Type  */
        break;
        }
        
        ResponseData[7]  = 0;                           /* Reserved           */
        ResponseData[8]  = 0;                           /* Reserved           */
        ResponseData[9]  = 0;                           /* Reserved           */
        ResponseData[10] = 0;                           /* Reserved           */
        ResponseData[11] = 0;                           /* Reserved           */
        ResponseData[12] = 9;                           /* Length remaining   */
        ResponseData[13] = 0x01;                        /* Characteristics    */
        ResponseData[14] = 0;                           /* #DFs in this dir   */
        ResponseData[15] = 0;                           /* #EFs in this dir   */
        ResponseData[16] = 0;                           /* #CHVs              */
        ResponseData[17] = 0;                           /* Reserved           */
        ResponseData[18] = 0x0F;                        /* CHV1 Status        */
        ResponseData[19] = 0x0F;                        /* CHV1 Unblk Status  */
        ResponseData[20] = 0x0F;                        /* CHV2 Status        */
        ResponseData[21] = 0x0F;                        /* CHV2 Unblk Status  */
        ResponseDataLen = 22;
        memmove(StatusData, ResponseData, ResponseDataLen);
        StatusLen = ResponseDataLen;
}

/* Sets the response data of the current EF (file) */
void CrdSetEfParms(U16 FileId)
{
        ResponseData[0]  = 0;                           /* Reserved           */
        ResponseData[1]  = 0;                           /* Reserved           */
        ResponseData[2]  = 0xFF;                        /* File Size          */
        ResponseData[3]  = 0xFF;                        /* File Size          */
        StoreBE16(&ResponseData[4], FileId);            /* File Id            */
        ResponseData[6]  = 0x01;                        /* File Type          */
        ResponseData[7]  = 0;                           /* Reserved           */
        ResponseData[8]  = 0;                           /* Reserved           */
        ResponseData[9]  = 0;                           /* Reserved           */
        ResponseData[10] = 0;                           /* Reserved           */
        ResponseData[11] = 0;                           /* Reserved           */
        ResponseData[12] = 2;                           /* Length remaining   */
        ResponseData[13] = 0x01;                        /* Structure of EF    */
        ResponseData[14] = 1;                           /* Length of Record   */
        ResponseDataLen = 15;

        /* Save the current EF info */
        EfFileInfo.fileSize = 0xFFFF;
        EfFileInfo.fileId = FileId;
        EfFileInfo.fileType = 0x01;
        EfFileInfo.incFlag = 0;
        memset(EfFileInfo.access, 0, 3);
        EfFileInfo.remLen = 2;
        EfFileInfo.efStruct = 0x01;
        EfFileInfo.recLen = 1;
}

/* Card Select Instruction */
void CrdSelect(U8 *ApduPtr, U16 Len) 
{
    U16 fileId = BEtoHost16(&ApduPtr[5]);
    U8  fileName[5];

    /* Convert the file ID to a name */
    sprintf((char *)fileName, "%x", fileId);
    switch (fileId >> 8) {
    case MASTER_FILE_ID:
        if (_chdir(RootDirectory) != 0) {
            /* Failed */
            ApduBuffer = ApduResult;
            StoreBE16(ApduBuffer, STATUS_FILE_ID_NOT_FOUND);
            ApduBufferLen = 2;
            break;
        } else {
            /* Changed to the root directory */
            if (_getcwd(CurrentDirectory, _MAX_PATH) == 0) {
                ApduBuffer = ApduResult;
                StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
                ApduBufferLen = 2;
                break;
            }
        }

        /* Close any open file */
        if (CurrentFp) {
            BTHAL_FS_Close(*CurrentFp);
        }

        /* Successfully changed directories */
        CurrentDirId = fileId;
        CrdSetDfParms(fileId);
        ApduBuffer = ApduResult;
        StoreBE16(ApduBuffer, STATUS_NORMAL_X);  /* Status byte        */
        ApduBuffer[1] = 22;                      /* Length of the data */
        ApduBufferLen = 2;                      /* Buffer length      */
        break;
    case LVL1_DEDICATED_ID:
    case LVL2_DEDICATED_ID:
        if (_isparent(fileName)) {
            /* Change to parent directory */
            if (_chdir("..") != 0) {
                ApduBuffer = ApduResult;
                StoreBE16(ApduBuffer, STATUS_FILE_ID_NOT_FOUND);
                ApduBufferLen = 2;
                break;
            } else {
                /* Changed to the parent directory */
                if (_getcwd(CurrentDirectory, _MAX_PATH) == 0) {
                    ApduBuffer = ApduResult;
                    StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
                    ApduBufferLen = 2;
                    break;
                }
            }
        } else if (_chdir(fileName) != 0) {
            /* Failed */
            ApduBuffer = ApduResult;
            StoreBE16(ApduBuffer, STATUS_FILE_ID_NOT_FOUND);
            ApduBufferLen = 2;
            break;
        } else {
            /* Changed to the sub directory */
            if (_getcwd(CurrentDirectory, _MAX_PATH) == 0) {
                ApduBuffer = ApduResult;
                StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
                ApduBufferLen = 2;
                break;
            }
        }

        /* Close any open file */
        if (CurrentFp) {
            BTHAL_FS_Close(*CurrentFp);
        }

        /* Successfully changed directories */
        CurrentDirId = fileId;
        CrdSetDfParms(fileId);
        ApduBuffer = ApduResult;
        StoreBE16(ApduBuffer, STATUS_NORMAL_X);
        ApduBuffer[1] = 22;
        ApduBufferLen = 2;
        break;
    case MASTER_ELEMENTARY_ID:
    case LVL1_ELEMENTARY_ID:
    case LVL2_ELEMENTARY_ID:
        /* Close any open file */
        if (CurrentFp) {
            BTHAL_FS_Close(*CurrentFp);
        }

        /* Elementary file */
	if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Open(fileName,  BTHAL_FS_O_RDWR, CurrentFp)) {
            /* Failed to open */
            ApduBuffer = ApduResult;
            StoreBE16(ApduBuffer, STATUS_FILE_ID_NOT_FOUND);
            ApduBufferLen = 2;
            break;
        }
        CrdSetEfParms(fileId);
        ApduBuffer = ApduResult;
        StoreBE16(ApduBuffer, STATUS_NORMAL_X);
        ApduBuffer[1] = 15;
        ApduBufferLen = 2;
        break;
    default:
        ApduBuffer = ApduResult;
        StoreBE16(ApduBuffer, STATUS_FILE_ID_NOT_FOUND);
        ApduBufferLen = 2;
        break;
    }
}

/* Card Status Instruction */
void CrdStatus(U8 *ApduPtr, U16 Len) 
{
    CrdSetDfParms(CurrentDirId);
    ApduBuffer = ResponseData;
    StoreBE16(ApduBuffer + 22, STATUS_NORMAL); /* Status byte        */
    ApduBufferLen = 24;                        /* Buffer length      */
    ResponseDataLen = 0;
}

/* Read Binary Instruction */
void CrdReadBinary(U8 *ApduPtr, U16 Len) 
{
    U16     offset = BEtoHost16(&ApduPtr[2]);
    U16     readLen = ApduPtr[4];
    size_t  bytesRead;

    if (readLen == 0) readLen = 256;
    if (CurrentFp) {
	if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Seek(*CurrentFp, offset, BTHAL_FS_START)) {
            /* Could not seek */
            StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
            ApduBufferLen = 2;
        } else {
            /* Read the data */
	     if (BT_STATUS_HAL_FS_SUCCESS == BTHAL_FS_Read(*CurrentFp, ApduBuffer, readLen, &bytesRead)) {
	            StoreBE16(ApduBuffer + bytesRead, STATUS_NORMAL);
	            ApduBuffer[bytesRead + 1] = (U8)bytesRead;
	            ApduBufferLen = bytesRead + 2;
	     }
        }
    } else {
        /* No file selected */
        StoreBE16(ApduBuffer, STATUS_NO_EF_SELECTED);
        ApduBufferLen = 2;
    }
}

/* Update Binary Instruction */
void CrdUpdateBinary(U8 *ApduPtr, U16 Len) 
{
    U16     offset = BEtoHost16(&ApduPtr[2]);
    U8      writeLen = ApduPtr[4];
    size_t  bytesWritten;

    if (CurrentFp) {
	 if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Seek(*CurrentFp, offset, BTHAL_FS_START)) {		
            /* Could not seek */
            StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
            ApduBufferLen = 2;
        } else {
            /* Write the data */
 	     if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Write(*CurrentFp, ApduPtr + 5, writeLen, &bytesWritten) ||
		 	(bytesWritten != writeLen))
		{
	                /* Couldn't write it all */
	                StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
	                ApduBufferLen = 2;        
	       }
             else {
                StoreBE16(ApduBuffer, STATUS_NORMAL);
                ApduBufferLen = 2;  
            } 
       }
    } else {
        /* No file selected */
        StoreBE16(ApduBuffer, STATUS_NO_EF_SELECTED);
        ApduBufferLen = 2;
    }
}

/* Read Record Instruction */
void CrdReadRecord(U8 *ApduPtr, U16 Len) 
{
    U8      recNum = ApduPtr[2];
    U8      mode = ApduPtr[3];
    size_t  bytesRead;

    if (CurrentFp) {
        switch (mode) {
        case 2:
            /* Next record */
	     if (BT_STATUS_HAL_FS_SUCCESS == BTHAL_FS_Read(*CurrentFp, ApduBuffer, EfFileInfo.recLen, &bytesRead)) {			
	            StoreBE16(ApduBuffer + bytesRead, STATUS_NORMAL);
	            ApduBuffer[bytesRead + 1] = (U8)bytesRead;
	            ApduBufferLen = bytesRead + 2;
	     }
            break;
        case 3:
            /* Previous record */
	     if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Seek(*CurrentFp, 0 - EfFileInfo.recLen, BTHAL_FS_CUR)) {			
                /* Could not seek */
                StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
                ApduBufferLen = 2;
            } else {
         	  if (BT_STATUS_HAL_FS_SUCCESS == BTHAL_FS_Read(*CurrentFp, ApduBuffer, EfFileInfo.recLen, &bytesRead)) {			            
                StoreBE16(ApduBuffer + bytesRead, STATUS_NORMAL);
                ApduBuffer[bytesRead + 1] = (U8)bytesRead;
                ApduBufferLen = bytesRead + 2;
         	 }
            }
            break;
        case 4:
            /* Absolute record */
	     if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Seek(*CurrentFp, recNum, BTHAL_FS_START)) {			     
                /* Could not seek */
                StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
                ApduBufferLen = 2;
            } else {
	     if (BT_STATUS_HAL_FS_SUCCESS == BTHAL_FS_Read(*CurrentFp, ApduBuffer, EfFileInfo.recLen, &bytesRead)) {			            
                StoreBE16(ApduBuffer + bytesRead, STATUS_NORMAL);
                ApduBuffer[bytesRead + 1] = (U8)bytesRead;
                ApduBufferLen = bytesRead + 2;
	     	}
            }
            break;
        default:
            ApduBuffer = ApduResult;
            StoreBE16(ApduBuffer, STATUS_INVALID_P3);
            ApduBufferLen = 2;
            break;
        }
    } else {
        /* No file selected */
        StoreBE16(ApduBuffer, STATUS_NO_EF_SELECTED);
        ApduBufferLen = 2;
    }
}

/* Update Record Instruction */
void CrdUpdateRecord(U8 *ApduPtr, U16 Len) 
{
    U8      recNum = ApduPtr[2];
    U8      mode = ApduPtr[3];
    size_t  bytesWritten;

    if (CurrentFp) {
        switch (mode) {
        case 2:
            /* Next record */
	     if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Write(*CurrentFp, ApduPtr + 5, EfFileInfo.recLen, &bytesWritten) ||
		 	(bytesWritten != EfFileInfo.recLen)) {
			
                /* Couldn't write it all */
                StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
                ApduBufferLen = 2;
            } else {
                StoreBE16(ApduBuffer, STATUS_NORMAL);
                ApduBufferLen = 2;  
            }
            break;
        case 3:
            /* Previous record */
	     if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Seek(*CurrentFp, 0 - EfFileInfo.recLen, BTHAL_FS_CUR)) {						
                /* Could not seek */
                StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
                ApduBufferLen = 2;
            } else {
                 if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Write(*CurrentFp, ApduPtr + 5, EfFileInfo.recLen, &bytesWritten) ||
		 	(bytesWritten != EfFileInfo.recLen)) {

                    /* Couldn't write it all */
                    StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
                    ApduBufferLen = 2;
                } else {
                    StoreBE16(ApduBuffer, STATUS_NORMAL);
                    ApduBufferLen = 2;  
                }
            }
            break;
        case 4:
            /* Absolute record */
	     if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Seek(*CurrentFp, recNum, BTHAL_FS_START)) {									
                /* Could not seek */
                StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
                ApduBufferLen = 2;
            } else {
               if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Write(*CurrentFp, ApduPtr + 5, EfFileInfo.recLen, &bytesWritten) ||
		 	(bytesWritten != EfFileInfo.recLen)) {            
                    /* Couldn't write it all */
                    StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);
                    ApduBufferLen = 2;
                } else {
                    StoreBE16(ApduBuffer, STATUS_NORMAL);
                    ApduBufferLen = 2;  
                }
            }
            break;
        default:
            ApduBuffer = ApduResult;
            StoreBE16(ApduBuffer, STATUS_INVALID_P3);
            ApduBufferLen = 2;
            break;
        }
    } else {
        /* No file selected */
        StoreBE16(ApduBuffer, STATUS_NO_EF_SELECTED);
        ApduBufferLen = 2;
    }
}

void CrdSeek(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdIncrease(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdVerifyChv(U8 *ApduPtr, U16 Len) 
{
#ifdef TI_CHANGES
    StoreBE16(ApduBuffer, STATUS_NORMAL); //always accept a PIN
#else
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
#endif
    ApduBufferLen = 2;
}

void CrdChangeChv(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdDisableChv(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdEnableChv(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdUnblockChv(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdInvalidate(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdGsmAlgorithm(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdSleep(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

/* Get Response Instruction */
void CrdResponse(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;

    if (ResponseDataLen != 0) {
        ApduBuffer = ResponseData;
        StoreBE16(ApduBuffer + ResponseDataLen, 
                  STATUS_NORMAL);                      /* Status byte        */
        ApduBufferLen = ResponseDataLen + 2;           /* Buffer length      */
        ResponseDataLen = 0;
    } else {
        ApduBuffer = ApduResult;
        StoreBE16(ApduBuffer, STATUS_UNKNOWN_ERR);     /* Status byte        */
        ApduBufferLen = 2;                             /* Buffer length      */
    }
}

void CrdTerminalProfile(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdEnvelope(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdFetch(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}

void CrdTerminalResponse(U8 *ApduPtr, U16 Len) 
{
    StoreBE16(ApduBuffer, STATUS_INVALID_INS);
    ApduBufferLen = 2;
}


#endif /*BTL_CONFIG_SAPS == BTL_CONFIG_ENABLED*/



