/* zi8cfg.h 
*/
/*****************************************************************************
* COPYRIGHT ZI AND SUBJECT TO CONFIDENTIALITY RESTRICTIONS                   *
*                                                                            *
* This file is the confidential and proprietary property of Zi Corporation   *
* of Canada, Inc. ("Zi") or one of its affiliates.                           *
**************************************************************************** */

/* prevent multiple inclusion */
#ifndef _ZI8_CFG_H
#define _ZI8_CFG_H

#ifdef __SYMBIAN32__
#include <e32def.h>
#endif

#define ZI8STRUCTURE_VERSION   5

#define ZI8_STROKE_EXTRA_BYTES 2
#define ZI8_STROKE_MATCH_BYTES (2+ZI8_STROKE_EXTRA_BYTES)
#define ZI8_COMP_MATCH_BYTES   4
#define ZI8_MAX_STROKES        23

#define ZI8_BASE_LATIN_KEYS    0xeff0
#define ZI8_BASE_PRIVATE       0xf000
#define ZI8_BASE_STROKES       ZI8_BASE_PRIVATE
#define ZI8_BASE_COMPONENTS    (ZI8_BASE_PRIVATE+0x0010)
#define ZI8_BASE_BPMF          (ZI8_BASE_PRIVATE+0x0405)
#define ZI8_BASE_TONES         (ZI8_BASE_PRIVATE+0x0431)
#define ZI8_BASE_PINYIN        (ZI8_BASE_PRIVATE+0x0461)
#define ZI8_LAST_ELEMENT       (ZI8_BASE_PRIVATE+0x04ff)

#define ZI8_INTERNAL_DATA_SIZE1 2048

/* platform related definition */
#ifdef __IAR_SYSTEMS_ICC
#define ZI8_HUGE               huge
#define ZI8_CHANGE_CONSTSEG
#elif defined(_C166)
#define ZI8_HUGE               huge
#else
#define ZI8_HUGE
#endif

#ifdef __SYMBIAN32__

#define ZI8_NO_GLOBAL_VARIABLES
#define ZI8_EXPORT_C EXPORT_C
#define ZI8_IMPORT_C IMPORT_C

#else /* #ifdef __SYMBIAN32__ */

#define ZI8_EXPORT_C
#define ZI8_IMPORT_C

#endif /* #else    #ifdef __SYMBIAN32__ */

#ifdef __cplusplus
#define ZI8FUNCTION extern "C"
#else
#define ZI8FUNCTION extern
#endif

#endif /* #ifndef _ZI8_CFG_H */
