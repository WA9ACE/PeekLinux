#ifndef DEF_MFW_BTIPS_OPPC_H
#define DEF_MFW_BTIPS_OPPC_H
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
#define OPPC_MAX_FILES 10
//Number of files in content directory.


/*Function Declarations*/
int mfw_btips_oppcPopulateFiles(char **file_names, int max_files_count,
									char* dir_name_p);

void mfw_btips_oppcUnpopulateFiles(char **file_names);
 char *mfw_btips_oppcGetExtension(char *src);
 int mfw_btips_oppcGetNumFiles(void);
 char* mfw_btips_oppcGetFileName(UBYTE index);
 BOOL mfw_btips_oppcGetConnectedDevice (BD_ADDR *bdAddr, U8 *name);
 void mfw_btips_oppc_Disconnect(void);


#endif


