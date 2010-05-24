





#ifndef __FSM_DEFS_H__

#define __FSM_DEFS_H__


#define OS_NUCLEUS				1

#define OS_WINDOWS				2

/*#define OS_TYPE					OS_WINDOWS */

#define OS_TYPE					OS_NUCLEUS


/*#define USE_NFFS_OPTION*/

/*#define USE_DOSFS_OPTION*/

#define FSM_TIME_SUPPORT

#if (OS_TYPE == OS_NUCLEUS)

#include "nucleus.h"
#include "sysapi.h"
#include "monapi.h"

#else

#include "windows.h"

#include "stdio.h"
#include "assert.h"

typedef DWORD		uint32;
typedef WORD		uint16;
typedef BYTE		uint8;

typedef long		int32;
typedef short		int16;
typedef char		int8;

#undef PACKED
#define PACKED		

#undef FIELD_OFFSET

extern int FsmPrintf(const char * format, ...);
/*
#define MonPrintf		FsmPrintf
*/
#define ASSERT(x)	
#define MonPrintf		printf

#define MonFault		assert
#define MonTrace

#define MON_FSM_FAULT_UNIT 0
#define MON_CP_FSM_FILE_READ_TRACE_ID 0
#define MON_CP_FSM_FILE_WRITE_TRACE_ID 0
#define MON_HALT 0

#endif	/* OS_WINDOWS */

/*--------------------------------------------*/

/*#define DEBUG_FSM*/

/*--------------------------------------------*/


#define FIELD_SIZE(type, field)		(sizeof((((type *)0)->field)))

#define FIELD_OFFSET(type, field)	((uint32)(&(((type *)0)->field)))

#ifndef TRUE
#define TRUE	(bool)1
#endif
#ifndef FALSE
#define FALSE	(bool)0
#endif
/*
#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif
*/

#include "errors.h"

#endif /* __FSM_DEFS_H__ */






