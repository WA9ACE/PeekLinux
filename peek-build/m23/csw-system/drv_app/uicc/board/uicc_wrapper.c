/*=============================================================================
 *    Copyright 1996-2003 Texas Instruments Inc. All rights reserved.
 */

#include "sys_types.h"

#define U8 SYS_UWORD8
#define U16 SYS_UWORD16
#define U32 SYS_UWORD32

#include "general.h"
#include "p_8010_136_simdrv_sap.val"
#include "p_8010_136_simdrv_sap.h"
#include "8010_136_simdrv_sap_inline.h"
#include "bspUicc.h"


/*=============================================================================
 *  File Description:
 *  Source File wrapping the UICC driver to match the SIM Entity in the 
 *  TI Protocol Stack.
 */

static void (* bspWrapper_insert) ( T_SIMDRV_atr_string_info *atr_string_info,
		U8 config_requested,
		T_SIMDRV_config_characteristics *config_characteristics);

static void (* bspWrapper_remove) ( void );

static void bspUicc_insert( BspUicc_AtrDataInfo     *atrData,
		BspUicc_ConfigReq       configReq,
		BspUicc_ConfigChars     *configChars) 
{
	T_SIMDRV_atr_string_info atr_string_info;
	T_SIMDRV_config_characteristics config_characteristics;
	int i=0;
	if(atrData !=NULL) 
	{	
		atr_string_info.c_atr_string = atrData->atrSize;
		for (i=0; i < atrData->atrSize; i++)
		{
			atr_string_info.atr_string[i] = (atrData->atrDataPtr)[i];
		}
		

		(* bspWrapper_insert) (&atr_string_info, configReq,
							   &config_characteristics);

		if(configChars && configReq)
		{
			configChars->clkStopPreference =
				config_characteristics.uicc_characteristics;	
		}
	}
	else  //Sim Reinsertion Event
	{
		(* bspWrapper_insert) (NULL, configReq,
						   &config_characteristics);
	}

	return;
}

static void bspUicc_remove(void)
{
	(*bspWrapper_remove)();
	return;
}

/*=============================================================================
 * GENERAL NOTE OF CAUTION:
 *    All generic command functions are bypassing the class parameter
 * because this param is  not as per the SIM Interface document.
 * UMTS class is used by default. This will be fixed in the PS.
 */


/*=============================================================================
 * Description:
 */
U16 simdrv_xch_apdu( U8                   reader_id,
		T_SIMDRV_cmd_header  cmd_header,
		T_SIMDRV_data_info  data_info,
		T_SIMDRV_result_info *result_info )
{
	BspUicc_CmdResult              status;
	BspUicc_CmdWriteDataInfo       bspUicc_cmdWriteData;
	BspUicc_CmdReadDataInfo        bspUicc_cmdReadData;
	BspUicc_CmdHeader              bspUicc_cmdHeader;      

	bspUicc_cmdHeader.cmdClass = cmd_header.cla;
	bspUicc_cmdHeader.cmdInstruction = cmd_header.ins;
	bspUicc_cmdHeader.cmdParameter1 = cmd_header.p1;
	bspUicc_cmdHeader.cmdParameter2 = cmd_header.p2;

	if(result_info)
	{
		bspUicc_cmdReadData.cmdResultSizePtr = &(result_info->c_result);
		bspUicc_cmdReadData.cmdReadDataSize = result_info->len;
		bspUicc_cmdReadData.cmdReadDataPtr  = result_info->result;
	}
	else
	{
		bspUicc_cmdReadData.cmdResultSizePtr = 0 ;
		bspUicc_cmdReadData.cmdReadDataSize = 0 ;
		bspUicc_cmdReadData.cmdReadDataPtr  = 0 ;

	}


	bspUicc_cmdWriteData.cmdWriteDataSize = data_info.c_data; 
	bspUicc_cmdWriteData.cmdWriteDataPtr  = data_info.data; 


	status = bspUicc_xchApdu( reader_id,
			bspUicc_cmdHeader,
			bspUicc_cmdWriteData,
			bspUicc_cmdReadData );

	return status;     
}


/*=============================================================================
 * Description:
 */
void simdrv_register( void (* insert) ( T_SIMDRV_atr_string_info *atr_string_info,
			U8 config_requested,
			T_SIMDRV_config_characteristics *config_characteristics),
		void (* remove) ( void ) )
{
	bspWrapper_insert = insert;
	bspWrapper_remove = remove;

	bspUicc_drvRegister( bspUicc_insert, bspUicc_remove );
}


/*=============================================================================
 * Description:
 */
U8 simdrv_reset( U8 reader_id, U8 voltage_select )
{
	BspUicc_ResetResult status;

	status = bspUicc_reset( reader_id, voltage_select );

	return status;
}

/*=============================================================================
 * Description:
 */
void simdrv_poweroff( U8 reader_id )
{
	bspUicc_powerOff( reader_id );
}

