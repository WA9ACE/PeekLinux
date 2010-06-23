#line 1 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/info.c"








 

#line 1 "/opt/TI/TMS470CGT4.6.1/include/string.h"
 
 
 
 




#line 17 "/opt/TI/TMS470CGT4.6.1/include/string.h"
 






typedef unsigned size_t;


#line 1 "/opt/TI/TMS470CGT4.6.1/include/linkage.h"
 
 
 
 




 





 
 
 
#line 27 "/opt/TI/TMS470CGT4.6.1/include/linkage.h"

#line 28 "/opt/TI/TMS470CGT4.6.1/include/string.h"

#line 36 "/opt/TI/TMS470CGT4.6.1/include/string.h"

 size_t  strlen(const char *_string);

 char *strcpy(char *_dest, const char *_src);
 char *strncpy(char *_to, const char *_from, size_t _n);
 char *strcat(char *_string1, const char *_string2);
 char *strncat(char *_to, const char *_from, size_t _n);
 char *strchr(const char *_string, int _c);
 char *strrchr(const char *_string, int _c);

 int  strcmp(const char *_string1, const char *_string2);
 int  strncmp(const char *_string1, const char *_string2, size_t _n);

 int     strcoll(const char *_string1, const char *_string2);
 size_t  strxfrm(char *_to, const char *_from, size_t _n);
 char   *strpbrk(const char *_string, const char *_chs);
 size_t  strspn(const char *_string, const char *_chs);
 size_t  strcspn(const char *_string, const char *_chs);
 char   *strstr(const char *_string1, const char *_string2);
 char   *strtok(char *_str1, const char *_str2);
 char   *strerror(int _errno);

 void   *memmove(void *_s1, const void *_s2, size_t _n);
 void   *memcpy(void *_s1, const void *_s2, size_t _n);

 int     memcmp(const void *_cs, const void *_ct, size_t _n);
 void   *memchr(const void *_cs, int _c, size_t _n);

 void   *memset(void *_mem, int _ch, size_t _n);






#line 269 "/opt/TI/TMS470CGT4.6.1/include/string.h"



#line 299 "/opt/TI/TMS470CGT4.6.1/include/string.h"

#line 12 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/info.c"

#line 1 "./include/nmea/info.h"








 

 




#line 1 "./include/nmea/time.h"








 

 




#line 1 "./include/nmea/config.h"








 


























#line 43 "./include/nmea/config.h"

#line 1 "/opt/TI/TMS470CGT4.6.1/include/assert.h"
 
 
 
 






 
 
 
 




 
 
 
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





#line 17 "./include/nmea/info.h"























 
typedef struct _nmeaPOS
{
    double lat;          
    double lon;          

} nmeaPOS;





 
typedef struct _nmeaSATELLITE
{
    int     id;          
    int     in_use;      
    int     elv;         
    int     azimuth;     
    int     sig;         

} nmeaSATELLITE;





 
typedef struct _nmeaSATINFO
{
    int     inuse;       
    int     inview;      
    nmeaSATELLITE sat[(12)];  

} nmeaSATINFO;






 
typedef struct _nmeaINFO
{
    int     smask;       

    nmeaTIME utc;        

    int     sig;         
    int     fix;         

    double  PDOP;        
    double  HDOP;        
    double  VDOP;        

    double  lat;         
    double  lon;         
    double  elv;         
    double  speed;       
    double  direction;   
    double  declination;  

    nmeaSATINFO satinfo;  

} nmeaINFO;

void nmea_zero_INFO(nmeaINFO *info);





#line 14 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/info.c"

void nmea_zero_INFO(nmeaINFO *info)
{
    memset(info, 0, sizeof(nmeaINFO));
    nmea_time_now(&info->utc);
    info->sig = (0);
    info->fix = (1);
}
