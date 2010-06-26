#ifndef DEF_MFW_BTIPS_A2DP
#define DEF_MFW_BTIPS_A2DP
/* =========================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ========================================================== */
/*
 $Project name: Basic Bluetooth MMI
 $Project code:
 $Module:   Bluetooth BMG MFW
 $File:       Mfw_BtipsA2dp.h
 $Revision:   1.0
 $Author:   Texas Instruments
 $Date:       26/06/07

********************************************************************************

 Description:

    This module provides the BTIPS A2DP APPlication functionality.

********************************************************************************
 $History: Mfw_BtipsA2dp.h

  26/06/07 Sasken original version

 $End

*******************************************************************************/

/* Maximum files to be displayed in the MMI */
#define A2DP_MAX_FILES 10
//Number of files in content directory.


/*Function Declarations*/
int mfw_btips_a2dpPopulateFiles(char **file_names, int max_files_count,
									char* dir_name_p);

void mfw_btips_a2dpUnpopulateFiles(char **file_names);
 char *mfw_btips_a2dpGetExtension(char *src);
 int mfw_btips_a2dpGetNumFiles(void);
 char* mfw_btips_a2dpGetFileName(UBYTE index);
 BtStatus mfw_btips_a2dpConnectStream(BD_ADDR bdAddr);
  BtStatus mfw_btips_a2dpCloseStream();

#endif


