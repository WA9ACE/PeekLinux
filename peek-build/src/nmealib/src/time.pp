#line 1 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/time.c"








 

 

#line 1 "./include/nmea/time.h"








 

 




#line 1 "./include/nmea/config.h"








 


























#line 43 "./include/nmea/config.h"

#line 1 "/opt/TI/TMS470CGT4.6.1/include/assert.h"
 
 
 
 




#line 1 "/opt/TI/TMS470CGT4.6.1/include/linkage.h"
 
 
 
 




 





 
 
 
#line 27 "/opt/TI/TMS470CGT4.6.1/include/linkage.h"

#line 10 "/opt/TI/TMS470CGT4.6.1/include/assert.h"

 
 
 
 




 
 
 
#line 29 "/opt/TI/TMS470CGT4.6.1/include/assert.h"

#line 43 "/opt/TI/TMS470CGT4.6.1/include/assert.h"

 
extern  void _nassert(int);
extern  void _assert(int, const char *);

extern  void _abort_msg(const char *);




#line 68 "/opt/TI/TMS470CGT4.6.1/include/assert.h"







#line 83 "/opt/TI/TMS470CGT4.6.1/include/assert.h"

#line 46 "./include/nmea/config.h"





#line 17 "./include/nmea/time.h"








 
typedef struct _nmeaTIME
{
    int     year;        
    int     mon;         
    int     day;         
    int     hour;        
    int     min;         
    int     sec;         
    int     hsec;        

} nmeaTIME;



 
void nmea_time_now(nmeaTIME *t);





#line 14 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/time.c"

#line 1 "/opt/TI/TMS470CGT4.6.1/include/time.h"
 
 
 
 






 
 
 
 




 
 
 
#line 29 "/opt/TI/TMS470CGT4.6.1/include/time.h"

#line 36 "/opt/TI/TMS470CGT4.6.1/include/time.h"









typedef unsigned int clock_t;
typedef unsigned int time_t;



typedef unsigned size_t;


struct tm 
{
    int tm_sec;       
    int tm_min;       
    int tm_hour;      
    int tm_mday;      
    int tm_mon;       
    int tm_year;      
    int tm_wday;      
    int tm_yday;      
    int tm_isdst;     




};

 
 
 
typedef struct 
{
    short daylight;
    long  timezone;
    char  tzname[4];
    char  dstname[4];
} TZ;

extern  TZ _tz;

 
 
 
 clock_t    clock(void);             
 time_t     time(time_t *_timer);   
 
 time_t     mktime(struct tm *_tptr);
 double     difftime(time_t _time1, time_t _time0);
      extern  char      *ctime(const time_t *_timer);
 char      *asctime(const struct tm *_timeptr);
 struct tm *gmtime(const time_t *_timer);
 struct tm *localtime(const time_t *_timer);
 size_t     strftime(char *_out, size_t _maxsize, 
				 const char *_format,
				 const struct tm *_timeptr);

#line 107 "/opt/TI/TMS470CGT4.6.1/include/time.h"







#line 131 "/opt/TI/TMS470CGT4.6.1/include/time.h"

#line 25 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/time.c"


#line 45 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/time.c"

void nmea_time_now(nmeaTIME *stm)
{
    time_t lt;
    struct tm *tt;

    time(&lt);
    tt = gmtime(&lt);

    stm->year = tt->tm_year;
    stm->mon = tt->tm_mon;
    stm->day = tt->tm_mday;
    stm->hour = tt->tm_hour;
    stm->min = tt->tm_min;
    stm->sec = tt->tm_sec;
    stm->hsec = 0;
}

