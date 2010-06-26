/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      EBTIPS_version.h
*
*   BRIEF:          This file contains the BTIPS software version information
*
*	DESCRIPTION:    This file contains the BTIPS software version information
*
*                   The BTIPS version numbering scheme is defined as follows:
*
*						BTIPS (Letter)X.YZ.B
*
*							(Letter) - Target OS (Same convention as CS): W - Windows, N - Nucleus, L - Linux, S - Symbian.
*
*							X = Major ESI stack release
*
*							Y = Major API release (first release = 0)
*
*							Z = Additional features / profiles (first release = 0)
*
*							B = Bug fixes (first release = 0)
*
*   AUTHOR:         Amir Ayun
*
\*******************************************************************************/

#ifndef EBTIPS_VERSION_H
#define EBTIPS_VERSION_H



/********************************************************************************
 *
 * Constants
 *
 *******************************************************************************/


/*---------------------------------------------------------------------------
 * Target OS Types
 *		
 *	EBTIPS_OS_WIN		"W" - Windows
 *  EBTIPS_OS_NUCLEUS	"N" - Nucleus
 *  
 */

/* 
 * Target OS Types
 * Target OS type - Windows PC 
 */
#define EBTIPS_OS_WIN					"W"

/* 
 * Target OS Types
 * Target OS type - Nucleus 
 */
#define EBTIPS_OS_NUCLEUS				"N"


/*******************************************************************************\
 *												*
 * BTIPS Release: 	BTIPS_Win_2.21.2_26_8_07_For_Locosto label             *
 *								               *
 * Details:		    BTIPS 2.21 Locosto 5.X Label Release 	       *
 *									       *
 *									       *
\*******************************************************************************/

/* BTIPS Target OS */
#define EBTIPS_TARGET_OS					(EBTIPS_OS_NUCLEUS)

/* BTIPS Major stack version */
#define EBTIPS_SOFTWARE_VERSION_X				(2)

/* BTIPS Major API version */
#define EBTIPS_SOFTWARE_VERSION_Y				(2)

/* BTIPS Additional features / profiles */
#define EBTIPS_SOFTWARE_VERSION_Z				(1)

/* BTIPS Bug Fixes */
#define EBTIPS_SOFTWARE_VERSION_B				(2)

/* BTIPS version day */
#define BTIPS_SOFTWARE_VERSION_DAY				(2)

/* BTIPS version month */
#define BTIPS_SOFTWARE_VERSION_MONTH			(8)

/* BTIPS version day year */
#define BTIPS_SOFTWARE_VERSION_YEAR				(2007)

/* Platform Types */
#define PLATFORM_WINDOWS				(1)
#define PLATFORM_NEPTUNE					(2)
#define PLATFORM_LOCOSTO					(3)

/* Anchor Platform */
#define ANCHOR_VERSION_X				(PLATFORM_LOCOSTO)

/* Anchor Major Software Version */
#define ANCHOR_VERSION_Y				(13)

/* Anchor Minor Software Version */
#define ANCHOR_VERSION_Z				(0)

#endif /* EBTIPS_VERSION_H */

