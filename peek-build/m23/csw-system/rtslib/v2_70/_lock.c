/****************************************************************************/
/*  _lock v2.54                                                             */
/*  Copyright (c) 2000-2004 Texas Instruments Incorporated                  */
/****************************************************************************/
#include <_lock.h>

_CODE_ACCESS void _nop()
{
  /* Do nothing. */
}

_DATA_ACCESS void (*  _lock)() = _nop;
_DATA_ACCESS void (*_unlock)() = _nop;

_CODE_ACCESS void _register_lock(void (*lock)())
{
    _lock = lock;
}

_CODE_ACCESS void _register_unlock(void (*unlock)())
{
    _unlock = unlock;
}
