/****************************************************************************/
/*  _lock v2.54                                                             */
/*  Copyright (c) 2000-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <_lock.h>
#include "nucleus.h"

static NU_SEMAPHORE mutex_file;
static int init = 0;

extern void emo_printlock(char *msg);

void mutex_file_init()
{
	NU_Create_Semaphore(&mutex_file, "peek_file_mutex", 1, NU_PRIORITY);
}

_CODE_ACCESS void mutex_file_lock()
{
	if(!init) {
		mutex_file_init();
		init=1;
	}
	emo_printlock("mutex_file_lock() BEGIN");
	NU_Obtain_Semaphore(&mutex_file, NU_SUSPEND); 
    emo_printlock("mutex_file_lock() END");
}

_CODE_ACCESS void mutex_file_unlock()
{
    if(!init) {
        mutex_file_init();
        init=1;
    }
	emo_printlock("mutex_file_unlock() BEGIN");
	NU_Release_Semaphore(&mutex_file); 
    emo_printlock("mutex_file_unlock() END");
}

_CODE_ACCESS void _nop()
{
  /* Do nothing. */
}

_DATA_ACCESS void (*  _lock)() = mutex_file_lock; //_nop;
_DATA_ACCESS void (*_unlock)() = mutex_file_unlock; //_nop;

_CODE_ACCESS void _register_lock(void (*lock)())
{
    _lock = lock;
}

_CODE_ACCESS void _register_unlock(void (*unlock)())
{
    _unlock = unlock;
}
