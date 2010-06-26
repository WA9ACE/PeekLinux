/*
*******************************************************************************
*
*      COPYRIGHT (C) 2000 BY ERICSSON EUROLAB DEUTSCHLAND GmbH
*      90411 NUERNBERG, GERMANY, Tel Int + 49 911 5217 100
*
*      The program(s) may be used and/or copied only with the
*      written permission from Ericsson or in accordance
*      with the terms and conditions stipulated in the agreement or
*      contract under which the program(s) have been supplied.
*
*******************************************************************************
*
*      File             : typedefs.h
*      Author           : EED/N/RV Peter Bloecher
*      Tested Platforms : DEC Alpha (OSF 3.2, 4.0), SUN (SunOS 5.5.1), Linux,
*                         Win95/NT
*      Description      : Definition of platform independent data
*                         types and constants
*
*      Revision history
*
*      Rev  Date       Name            Description
*      -------------------------------------------------------------------
*      pB1  1998-12-09 V.Springer      added PC Linux, PC (MSDOS/Win95/NT) tests
*                                      also for __BORLANDC__ and _MSC_VER 
*                                      compiler now
*      A    1998-05-06 P. Bloecher     first official release
*      pA2  28-MAY-97  R.Schleifer     corrected error at definitin of
*                                      maxFloat and minFloat
*      pA1  12-MAR-97  P.Bloecher      initial version
*
*
*      The following platform independent data types and corresponding
*      preprocessor (#define) constants are defined:
*
*        defined type  meaning           corresponding constants
*        ----------------------------------------------------------
*        Char          character         (none)
*        BOOL          boolean           true, false
*        Byte          8-bit signed      minByte,      maxByte
*        UByte         8-bit unsigned    minUByte,     maxUByte
*        Shortint      16-bit signed     minShortint,  maxShortint
*        UShortint     16-bit unsigned   minUShortint, maxUShortint
*        Longint       32-bit signed     minLongint,   maxLongint
*        ULongint      32-bit unsigned   minULongint,  maxULongint
*        Float         floating point    minFloat,     maxFloat
*
*
*      The following compile switches are #defined:
*
*        PLATFORM      string indicating platform progam is compiled on
*                      possible values: "OSF", "PC", "SUN"
*
*        OSF           only defined if the current platform is an Alpha
*        PC            only defined if the current platform is a PC
*        SUN           only defined if the current platform is a Sun
*        
*        LSBFIRST      is defined if the byte order on this platform is
*                      "least significant byte first" -> defined on DEC Alpha
*                      and PC, undefined on Sun
*
*******************************************************************************
*/
#ifndef ctm_typedefs_h
#define ctm_typedefs_h 

/*
*******************************************************************************
*                         INCLUDE FILES
*******************************************************************************
*/
#include <float.h>
#include <limits.h>

/*
*******************************************************************************
*                         DEFINITION OF CONSTANTS 
*******************************************************************************
*/

/*
 ********* define char type
 */
typedef char Char;  // word8

/*
 ********* define floating point type & constants
 */
/* use "#if 0" below if Float should be double;
   use "#if 1" below if Float should be float
 */
#if 0
typedef float Float;
#define maxFloat      FLT_MAX
#define minFloat      FLT_MIN
#else
typedef double Float;
#define maxFloat      DBL_MAX
#define minFloat      DBL_MIN
#endif

/*
 ********* define complex type
 */
typedef struct {
  Float r;  /* real      part */
  Float i;  /* imaginary part */
} CPX;

/*
 ********* define boolean type
 */
//typedef int BOOL;
#define false 0
#define true 1


#endif 
