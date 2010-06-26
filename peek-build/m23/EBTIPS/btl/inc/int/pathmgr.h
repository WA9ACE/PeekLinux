/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      pathmgr.h
*
*   DESCRIPTION:    This file specifies the interface to the folder manager.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef __PATHMGR_H
#define __PATHMGR_H


#include "bthal_fs.h"
#include "osapi.h"
#include "btl_unicode.h"

#define PATHMGR_MAX_PATH   BTHAL_FS_MAX_PATH_LENGTH
 
/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL PATHMGR_AppendNewAscii(const U8 *Path, U16 Len )
 *
 * Description:   Append the Unicode path to the new ASCII path.
 *
 * Parameters:    Path  - Unicode path string.
 *                Len   - Length of the path.
 *
 * Returns:       TRUE  - The path was successfully appended.
 *                FALSE - Insufficient space to append the path.
 */
BOOL PATHMGR_AppendNewAscii(const U8 *Path, U16 Len);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void PATHMGR_FlushNewPath()
 *
 * Description:   Cancel a path update in progress.
 *
 */
void PATHMGR_FlushNewPath(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL PATHMGR_Init()
 *
 * Description:   Initialize the folder manager.
 *
 * Parameters:    none 
 *
 * Returns:       TRUE - folder manager was initialized successfully. 
 *                FALSE - Unable to init.
 */
BOOL PATHMGR_Init(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void PATHMGR_Switch2Root()
 *
 * Description:   Reset the path to the file transfer server root.
 *
 */
BOOL PATHMGR_Switch2Root(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     PATHMGR_SetRootFolder(const BtlUtf8* rootFolder)
 *
 * Description:   Set root folder.
 *
 */
BOOL PATHMGR_SetRootFolder(const BtlUtf8* rootFolder);

/*---------------------------------------------------------------------------
 *
 * Prototype:     PATHMGR_IsFolderExists(const BtlUtf8* folder)
 *
 * Description:   Returns if a folder with the given name exists in the 
 *					current folder.
 *
 */
BOOL PATHMGR_IsFolderExists(const BtlUtf8* folder);

/*---------------------------------------------------------------------------
 *
 * Prototype:     BOOL PATHMGR_Switch2NewFolder(BOOL readOnly)
 *
 * Description:   Apply the path flags and the new path.
 *
 * Parameters:    readOnly  - read-only mode.
 *
 * Return:    3 - success
 *            2 - Failed since the folder wasn't found (and the client didn't ask to create it),
 *			  1 - Failed since the folder wasn't found, the client asked to create it, but the server is in read only mode.
 *			  0 - Failed because of an error.	
 */
U8 PATHMGR_Switch2NewFolder(BOOL readOnly);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void PATHMGR_SetFlags
 *
 * Description:   Set the flags for the setpath command.
 *
 */
void PATHMGR_SetFlags(U8 setPathFlags);

/*---------------------------------------------------------------------------
 *
 * Prototype:     const BtlUtf8 *PATHMGR_GetNewPath()
 *
 * Description:   Returns a pointer to the current newPath if it is valid.
 *
 */
const BtlUtf8 *PATHMGR_GetNewPath(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     const BtlUtf8 *PATHMGR_GetCurrentPath()
 *
 * Description:   Returns a pointer to the current path of the path manager.
 *
 */
const BtlUtf8 *PATHMGR_GetCurrentPath(void);

/*---------------------------------------------------------------------------
 *
 * Prototype:     PATHMGR_IsFileExists(const BtlUtf8* fileName)
 *
 * Description:   Checks if a file with the given name exists in the 
 *					current folder.
 * Return:    TRUE - File exists
 *            FALSE - File does not exists.
 */
BOOL PATHMGR_IsFileExists(const BtlUtf8* fileName);


#endif /* __PATHMGR_H */
