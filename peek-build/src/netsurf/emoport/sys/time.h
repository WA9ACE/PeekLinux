#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

struct timeval {
	long int	tv_sec;
	long int	tv_usec;
};

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

int gettimeofday(struct timeval *tv, struct timezone *tz);

#define timercmp(a, b, CMP)	\
	(((a)->tv_sec == (b)->tv_sec) ? \
	((a)->tv_usec CMP (b)->tv_usec) :	\
	((a)->tv_sec CMP (b)->tv_sec))

#define timeradd(a, b, result)                 \
  do {                        \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;           \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;            \
    if ((result)->tv_usec >= 1000000)               \
      {                       \
  ++(result)->tv_sec;                 \
  (result)->tv_usec -= 1000000;               \
      }                       \
  } while (0)

#define timersub(a, b, result)                 \
  do {                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;           \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;            \
    if ((result)->tv_usec < 0) {                \
      --(result)->tv_sec;                 \
      (result)->tv_usec += 1000000;               \
    }                       \
  } while (0)

#endif //__SYS_TIME_H__
