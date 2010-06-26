/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_edt.h       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 7               $|
| CREATED: 21.09.98                     $Modtime:: 23.03.00 8:23    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_EDT

   PURPOSE : EDIT types & constants

   EXPORT  :

   TO DO   :

   $History:: mfw_edt.h                                             $
 * 
  *  * *****************  Version 8  *****************
 * User: MC           Date: 03/10/02	   Time: 14:40
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * Replaced most of the file with version on 3.3.3 line (MC, SPR 1242)
 * 
 * *****************  Version 7  *****************
 * User: Es           Date: 23.03.00   Time: 14:40
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * added edtUnhide()
 * 
 * *****************  Version 6  *****************
 * User: Es           Date: 18.02.00   Time: 15:45
 * Updated in $/GSM/Condat/MS/SRC/MFW
 * added edit controls: ecTop, ecBottom
 * 
 * *****************  Version 5  *****************
 * User: Es           Date: 6.07.99    Time: 12:03
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 4  *****************
 * User: Es           Date: 17.02.99   Time: 20:00
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 3  *****************
 * User: Es           Date: 14.01.99   Time: 17:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 2  *****************
 * User: Es           Date: 23.12.98   Time: 16:19
 * Updated in $/GSM/DEV/MS/SRC/MFW
*/

#ifndef _DEF_MFW_EDT_H_
#define _DEF_MFW_EDT_H_

#define edtCurNone      0x00            /* no cursor, read only     */
#define edtCurBar1      0x01            /* vertical bar thin        */
#define edtCurMask      0x07            /* mask for cursor type     */
#define edtModOverWr    0x08            /* overwrite mode           */
#define edtModWordSkip  0x10            /* GW 12/09/01 skip over words (for predictive text)  */
#define edtModAllowPredText 0x20		/* SPR957 - SH - whether predicted text is available in this editor */

#define MAX_PRED_TEXT   15

//GW Added bgd colour - and changed type to U32 (for future compatibility)
typedef struct MfwEdtAttrTag            /* EDITOR ATTRIBUTES        */
{
    MfwRect win;                        /* editor position and size */
    U32 edtCol;							/* index to fgd/bgd colour definition*/
    U8 font;                            /* character font           */
    U8 mode;                            /* edit modes & cursor type */
    U8 *controls;                       /* edit control keys        */
    char *text;                         /* edit buffer              */
    U16 size;                           /* buffer size              */
	char predText[MAX_PRED_TEXT];		/* GW 06/09/01 Added predicted word buffer    */
    
/* NM, 13.03, fast-edit 
   old:  to handle between the two modes (digit/alpha) was done by BMI
   new: handle by MFW
*/     
    U8 alphaMode;   					/* true(alpha)/false(digit) */                
} MfwEdtAttr;

typedef struct MfwEdtTag                /* EDITOR CONTROL BLOCK     */
{
    MfwEvt mask;                        /* selection of events      */
    MfwEvt flags;                       /* current event            */
    MfwCb handler;                      /* event handler            */
    MfwEdtAttr *attr;                   /* editor attributes        */
    U16 cp;                             /* cursor position          */
    U16 curOffs;                        /* start of view in text    */
	/* SPR920 - NH (SH) - these properties are now kept by the editor */
    int fontHeight;                  /* height of current font   */
    int display;                     /* really show the things   */
    U8 curMode;                      /* edit mode & cursor type  */
    S16 curCX, curCY;                /* screen cursor position   */
    S16 curCSize;					/* size of screen cursor    */
    /* end SPR920 */
} MfwEdt;

typedef enum                            /* EDITOR CONTROL KEYS      */
{
    ecNone,                             /* no valid edit control    */
    ecLeft,                             /* cursor left              */
    ecRight,                            /* cursor right             */
    ecUp,                               /* cursor up                */
    ecDown,                             /* cursor down              */
    ecTop,                              /* cursor to top of text    */
    ecBottom,                           /* cursor to end of text    */
    ecBack,                             /* backspace                */
    ecDel,                              /* delete                   */
    ecEnter,                            /* return key               */
    ecEscape                            /* escape key               */
} MfwEdtEC;

                                        /* EDITOR FLAGS             */
#define MfwEdtVisible       1           /* editor is visible        */


                                        /* PROTOTYPES               */
MfwRes edtInit (void);
MfwRes edtExit (void);
MfwHnd edtCreate (MfwHnd w, MfwEdtAttr *a, MfwEvt e, MfwCb f);
MfwRes edtDelete (MfwHnd e);
MfwRes edtReset (MfwHnd w);
MfwRes edtShow (MfwHnd e);
MfwRes edtHide (MfwHnd e);
MfwRes edtUnhide (MfwHnd e);
MfwRes edtClear (MfwHnd e);
MfwRes edtUpdate (MfwEdt *e);
MfwRes edtChar (MfwHnd e, int c);

MfwRes edtRefresh (MfwHnd e); /* SPR945 - SH*/

//Global procedures - Adding/ removing words in the editor.
MfwRes edtInsertString (MfwHnd e, char* insWord);		//Insert a string at the cursor
MfwRes edtCopyString (MfwHnd e, char* removeWord);		//Copy a word from the cursor pos
MfwRes edtRemoveString (MfwHnd e, char* removeWord);	//Delete a word from the cursor pos.

char getCursorChar(MfwHnd e, int offset );
void moveRight (char *b, int s, int d);
void moveLeft (char * str, int ofs);

/* Unicode editor procedures.
 * SPR957 - SH - moved some prototypes here from mfw_edt.c */
 
MfwRes	edtUpdateUnicode (MfwEdt *e);
MfwRes	edtCharUnicode (MfwHnd e, int c);
void	moveLeftUnicode(U16 *unicodeStr, int nChars);
void	moveRightUnicode(U16 *unicodeStr, int nChars);/*MC*/
U16		strlenUnicode(U16* str);/*MC*/
int		edtInsertUnicode (MfwEdt *e, int c, U16 *b, int s);
MfwRes	edtInsertUnicodeString (MfwHnd e, char* insWordChar);
void	convertToUnicode(U16 *outputString, char *inputString); /* SPR957 - SH - added */


#endif

