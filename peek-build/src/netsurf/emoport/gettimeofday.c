#include <sys/time.h>
#include "typedefs.h"
#include "vsi.h"

extern T_HANDLE UI_handle;

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	T_TIME t;
	vsi_t_time(UI_handle, &t);

	tv->tv_sec = t / 1000;
	tv->tv_usec = t % 1000 * 1000;
	return 0;
}
