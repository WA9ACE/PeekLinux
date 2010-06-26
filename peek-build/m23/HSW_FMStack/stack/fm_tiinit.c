
#include <stdio.h>
#include "utils.h"
#include "bthal_fs.h"
#include "fm_tiinit.h"
#include "fm_utils.h"
#include "btl_config.h"

#if 0

#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((unsigned short)(((unsigned char)(a)) | ((unsigned short)((unsigned char)(b))) << 8))
#endif

#define TI_MANUFACTURER_ID    13

/* The value 0x42535442 stands for (in ASCII) BTSB
 * which is Bluetooth Script Binary */
#define FILE_HEADER_MAGIC    0x42535442




typedef struct tagCHeader 
{
    unsigned long  m_nMagicNumber;
    unsigned long  m_nVersion;
    unsigned char m_nFuture[24];
} CHeader;

static char s_szInitFileName[BTHAL_FS_MAX_PATH_LENGTH];
static BthalFsFileDesc fdInitScript = BTHAL_FS_INVALID_FILE_DESC;
static TIFM_U8 *memoryInitScript;
static TIFM_U8 *memoryInitScriptBase;
static TIFM_U16 memoryInitScriptSize;

/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/

static FmStatus fm_getHeader(CHeader * header, BTHAL_U32 size);

static FmStatus fm_getHeader(CHeader * header, BTHAL_U32 size)
{
	UNUSED_PARAMETER(size);
	
	/* Read header from file*/
	if (BTHAL_FS_INVALID_FILE_DESC != fdInitScript)
	{
		BTHAL_U32 numRead;
		
    	if (BT_STATUS_HAL_FS_SUCCESS != BTHAL_FS_Read(fdInitScript, header, sizeof(CHeader), &numRead))
    	{
    		TIFM_Report(("TIINIT: Error reading from %s\n", s_szInitFileName));
			BTHAL_FS_Close(fdInitScript);
			return FM_STATUS_FAILED;
    	}
	}
	else /* read from memory */
	{
		/* avoid returning pointer, because of the 
		 * increasing of memoryInitScript */
		BTHAL_UTILS_MemCopy((TIFM_U8*)header, memoryInitScript, sizeof(CHeader));
		memoryInitScript += sizeof(CHeader);
	}

	return FM_STATUS_SUCCESS;
}

unsigned long FM_bts_GetNextAction(unsigned char* pActionBuffer, unsigned long nMaxSize, unsigned short* pnType)
{
	TIFM_U32 nTotalReturned = 0;
	TIFM_U16 nType;
	TIFM_U16 nSize;
	
	/* loaded from FFS */
    if (BTHAL_FS_INVALID_FILE_DESC != fdInitScript)
    {
        /* Each Action has the following:
		   UINT16 type of this action
		   UINT16 size of rest
           BYTE[] action buffer (for HCI, includes the type byte e.g. 1 for hci command)	*/
        
		BtFsStatus fsStatus;
        TIFM_U8 abActionHeader[4];
		BTHAL_U32 nReadSize;

		fsStatus = BTHAL_FS_Read(fdInitScript, abActionHeader, sizeof(abActionHeader), &nReadSize);
		if(BT_STATUS_HAL_FS_SUCCESS == fsStatus)
        {
        	nType = (TIFM_U16)(abActionHeader[0]);
			nSize = (TIFM_U16)(abActionHeader[2]);

            if (nSize <= nMaxSize)
            {
            	fsStatus = BTHAL_FS_Read(fdInitScript, pActionBuffer,  sizeof(pActionBuffer[0])*nSize, &nReadSize);
				if ((BT_STATUS_HAL_FS_SUCCESS == fsStatus) && (nReadSize == nSize))
				{
                    *pnType = nType;
                    nTotalReturned = (TIFM_U32)nSize;
				}
				else
				{
					TIFM_Report(("DeviceTiInit: Error reading the TI Init script \n"));
                }
            }
		}
    }
	else /* load from memory */
	{
		/* verify we didn't reach to the end of init script */
		if(memoryInitScript < memoryInitScriptBase + memoryInitScriptSize)
		{	
			nType = TIFM_LittleEtoHost16(memoryInitScript);
			nSize = TIFM_LittleEtoHost16(memoryInitScript+2);
				
			memoryInitScript += 4;
		
			if (nSize <= nMaxSize)
			{
				BTHAL_UTILS_MemCopy(pActionBuffer, memoryInitScript, nSize);
			
				memoryInitScript += nSize;
			
				*pnType = nType;
				nTotalReturned = (unsigned long)nSize;
			}
		}
	}

    return nTotalReturned;
}

void FM_bts_UnloadScript(void)
{
    if (BTHAL_FS_INVALID_FILE_DESC != fdInitScript)
    {
        BTHAL_FS_Close(fdInitScript);
    }
}


FmStatus FM_bts_LoadFmScript(const char* filename)
{
	FmStatus	status;
	CHeader		header;
			
	/* get the Init Script file full name */
	sprintf(s_szInitFileName, "%s%s", BTHAL_FS_TI_INIT_SCRIPT_PATH, filename);

	/* try to open the file */
	if (BT_STATUS_HAL_FS_SUCCESS == BTHAL_FS_Open((const BTHAL_U8*)s_szInitFileName,
							  					  (BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY),
                                 				  &fdInitScript))
	{
		/* File exists */
		TIFM_Report(("TIINIT: FM Init Script %s found on FFS\n", s_szInitFileName));
	}
	else
	{
		/* Error opening file. Assume file doesn't exists */
		TIFM_Report(("TIINIT: FM Init Script not found on FFS. Loading it from memory\n"));

		fdInitScript = BTHAL_FS_INVALID_FILE_DESC;
		
		fmGetMemoryInitScript(filename, &memoryInitScriptSize, (char**)(&memoryInitScript));
		if (0 == memoryInitScriptSize)
		{
			TIFM_Report(("TIINIT: Failed getting Init Script from memory\n"));	
			return FM_STATUS_FAILED;
		}

		/* set the initial starting position for comparison */
		memoryInitScriptBase = memoryInitScript;
	}

	/* Get the header */
	status = fm_getHeader(&header, sizeof(header));
	if (status != FM_STATUS_SUCCESS)
	{
		TIFM_Report(("TIINIT: Failed getting header\n"));
		return FM_STATUS_FAILED;
	}

	/* check magic number */
	if (header.m_nMagicNumber == FILE_HEADER_MAGIC)
	{
		TIFM_Report(("TIINIT: Magic Number is Correct\n"));
		return FM_STATUS_SUCCESS;
	}

	/* Wromg magic number */
	TIFM_Report(("TIINIT: Magic Number is Incorrect\n"));
	if (fdInitScript != BTHAL_FS_INVALID_FILE_DESC)
	{
		BTHAL_FS_Close(fdInitScript);
	}

	return FM_STATUS_FAILED;
}


#endif /*BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED*/

