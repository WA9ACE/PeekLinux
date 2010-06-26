/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_mmi.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 3               $|
| CREATED: 21.09.98                     $Modtime:: 2.02.99 17:02    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_MMI

   PURPOSE : MMI MFW application

   EXPORT  :

   TO DO   :

   $History:: mfw_mmi.h                                             $
 * 
 * *****************  Version 3  *****************
 * User: Es           Date: 17.02.99   Time: 20:00
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 2  *****************
 * User: Es           Date: 8.02.99    Time: 19:07
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 1  *****************
 * User: Es           Date: 8.12.98    Time: 16:59
 * Created in $/GSM/DEV/MS/SRC/MFW
*/

#ifndef _DEF_MFW_MMI_H_
#define _DEF_MFW_MMI_H_


                                        /* PROTOTYPES               */
void mmi_main (void);
void mmiInit (void);
void mmiExit (void);


/************************************************************************

The following definitions are procedures in the BMI that are accessed by the MFW.

This header file should be included by any MFW procedure that calls a BMI function

and by the BMI procedures that define the functions to ensure that the definitions

match

************************************************************************/

/**********************/
/* From MMIResources  */
/**********************/

//Function to get string associated with an ID
char *MmiRsrcGetText( int Id );

//Functions to select/restore colour
void resources_setColour(unsigned int colIndex);
void resources_setSKColour(unsigned int colIndex);
void resources_setHLColour(unsigned int colIndex);
void resources_setTitleColour(unsigned int colIndex);

void resources_restoreColour( void );
void resources_restoreMnuColour(void);

void resources_setColourMnuItem( int txtId);

//Function to get the height allocated for the menu title
int res_getTitleHeight( void );


#endif

