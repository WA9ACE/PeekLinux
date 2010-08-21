#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

struct timeval {
	long int	tv_sec;
	long int	tv_usec;
};

#define timercmp(a, b, CMP)	\
	(((a)->tv_sec == (b)->tv_sec) ? \
	((a)->tv_usec CMP (b)->tv_usec) :	\
	((a)->tv_sec CMP (b)->tv_sec))

#endif //__SYS_TIME_H__
