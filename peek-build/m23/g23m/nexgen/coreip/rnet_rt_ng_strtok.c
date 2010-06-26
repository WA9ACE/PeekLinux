/*****************************************************************************
 * $Id: strtok.c,v 1.2 2001/03/28 18:25:10 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Standart Librairies Replacement Functions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2001 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *    This software is  supplied  under  the terms of a
 *    license agreement or nondisclosure agreement with
 *    NexGen Software, and may not be copied or disclosed
 *    except  in  accordance  with  the  terms of  that
 *    agreement.
 *
 *----------------------------------------------------------------------------
 * ngStrTok()
 *----------------------------------------------------------------------------
 * 29/08/2000 - Regis Feneon
 *****************************************************************************/

#include <ngos.h>

/*****************************************************************************
 * ngStrTok()
 *****************************************************************************
 * Break a string into tokens (reentrant version)
 *****************************************************************************
 * Parameters:
 *  s       string to search
 *  sep     list of separators
 *  savs    used to store the last token pointer
 * Return value: a pointer to the token or NULL
 */

char *ngStrTok( char *s, const char *sep, char **savs)
{
    const char *p;
    char *ret;
    int c, sepc;

    /* get saved pointer */
    if( s == NULL) s = *savs;

    /* end of string ? */
    if( s == NULL) return( NULL);

    /* skip leading separators */
    while( (c = *s++) != 0) {
        p = sep;
        while( (sepc = *p++) != 0) {
            if( c == sepc) break; /* character is a separator */
        }
        if( c != sepc) break;
    }

    /* end of string, only separators found */
    if( c == 0) {
        *savs = NULL;
        return( NULL);
    }

    /* start of token */
    ret = s - 1;
    /* scan token until separator found or end of string */
    while( 1) {
        if( (c = *s++) == 0) {
            *savs = NULL;
            return( ret);
        }
        p = sep;
        while( (sepc = *p++) != 0) {
            if( c == sepc) {
                *(s-1) = 0;
                *savs = s;
                return( ret);
            }
        }
    }
}

