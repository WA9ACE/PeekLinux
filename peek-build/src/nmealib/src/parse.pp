#line 1 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c"








 













































 

#line 1 "./include/nmea/tok.h"








 




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





#line 15 "./include/nmea/tok.h"





int     nmea_calc_crc(const char *buff, int buff_sz);
int     nmea_atoi(const char *str, int str_sz, int radix);
double  nmea_atof(const char *str, int str_sz);
int     nmea_printf(char *buff, int buff_sz, const char *format, ...);
int     nmea_scanf(const char *buff, int buff_sz, const char *format, ...);





#line 58 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c"
#line 1 "./include/nmea/parse.h"








 




#line 1 "./include/nmea/sentence.h"








 

 




#line 1 "./include/nmea/info.h"








 

 




#line 1 "./include/nmea/time.h"








 

 













 
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





#line 17 "./include/nmea/sentence.h"







 
enum nmeaPACKTYPE
{
    GPNON   = 0x0000,    
    GPGGA   = 0x0001,    
    GPGSA   = 0x0002,    
    GPGSV   = 0x0004,    
    GPRMC   = 0x0008,    
    GPVTG   = 0x0010     
};



 
typedef struct _nmeaGPGGA
{
    nmeaTIME utc;        
	double  lat;         
    char    ns;          
	double  lon;         
    char    ew;          
    int     sig;         
	int     satinuse;    
    double  HDOP;        
    double  elv;         
    char    elv_units;   
    double  diff;        
    char    diff_units;  
    double  dgps_age;    
    int     dgps_sid;    

} nmeaGPGGA;



 
typedef struct _nmeaGPGSA
{
    char    fix_mode;    
    int     fix_type;    
    int     sat_prn[(12)];  
    double  PDOP;        
    double  HDOP;        
    double  VDOP;        

} nmeaGPGSA;



 
typedef struct _nmeaGPGSV
{
    int     pack_count;  
    int     pack_index;  
    int     sat_count;   
    nmeaSATELLITE sat_data[(4)];

} nmeaGPGSV;



 
typedef struct _nmeaGPRMC
{
    nmeaTIME utc;        
    char    status;      
	double  lat;         
    char    ns;          
	double  lon;         
    char    ew;          
    double  speed;       
    double  direction;   
    double  declination;  
    char    declin_ew;   
    char    mode;        

} nmeaGPRMC;



 
typedef struct _nmeaGPVTG
{
    double  dir;         
    char    dir_t;       
    double  dec;         
    char    dec_m;       
    double  spn;         
    char    spn_n;       
    double  spk;         
    char    spk_k;       

} nmeaGPVTG;

void nmea_zero_GPGGA(nmeaGPGGA *pack);
void nmea_zero_GPGSA(nmeaGPGSA *pack);
void nmea_zero_GPGSV(nmeaGPGSV *pack);
void nmea_zero_GPRMC(nmeaGPRMC *pack);
void nmea_zero_GPVTG(nmeaGPVTG *pack);





#line 15 "./include/nmea/parse.h"





int nmea_pack_type(const char *buff, int buff_sz);
int nmea_find_tail(const char *buff, int buff_sz, int *res_crc);

int nmea_parse_GPGGA(const char *buff, int buff_sz, nmeaGPGGA *pack);
int nmea_parse_GPGSA(const char *buff, int buff_sz, nmeaGPGSA *pack);
int nmea_parse_GPGSV(const char *buff, int buff_sz, nmeaGPGSV *pack);
int nmea_parse_GPRMC(const char *buff, int buff_sz, nmeaGPRMC *pack);
int nmea_parse_GPVTG(const char *buff, int buff_sz, nmeaGPVTG *pack);

void nmea_GPGGA2info(nmeaGPGGA *pack, nmeaINFO *info);
void nmea_GPGSA2info(nmeaGPGSA *pack, nmeaINFO *info);
void nmea_GPGSV2info(nmeaGPGSV *pack, nmeaINFO *info);
void nmea_GPRMC2info(nmeaGPRMC *pack, nmeaINFO *info);
void nmea_GPVTG2info(nmeaGPVTG *pack, nmeaINFO *info);





#line 59 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c"
#line 1 "./include/nmea/context.h"








 













typedef void (*nmeaTraceFunc)(const char *str, int str_size);
typedef void (*nmeaErrorFunc)(const char *str, int str_size);

typedef struct _nmeaPROPERTY
{
    nmeaTraceFunc   trace_func;
    nmeaErrorFunc   error_func;
    int             parse_buff_size;

} nmeaPROPERTY;

nmeaPROPERTY * nmea_property();

void nmea_trace(const char *str, ...);
void nmea_trace_buff(const char *buff, int buff_size);
void nmea_error(const char *str, ...);





#line 60 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c"
#line 1 "./include/nmea/gmath.h"








 






#line 24 "./include/nmea/gmath.h"







 

double nmea_degree2radian(double val);
double nmea_radian2degree(double val);



 

double nmea_ndeg2degree(double val);
double nmea_degree2ndeg(double val);

double nmea_ndeg2radian(double val);
double nmea_radian2ndeg(double val);



 

double nmea_calc_pdop(double hdop, double vdop);
double nmea_dop2meters(double dop);
double nmea_meters2dop(double meters);



 

void nmea_info2pos(const nmeaINFO *info, nmeaPOS *pos);
void nmea_pos2info(const nmeaPOS *pos, nmeaINFO *info);

double  nmea_distance(
        const nmeaPOS *from_pos,
        const nmeaPOS *to_pos
        );

double  nmea_distance_ellipsoid(
        const nmeaPOS *from_pos,
        const nmeaPOS *to_pos,
        double *from_azimuth,
        double *to_azimuth
        );

int     nmea_move_horz(
        const nmeaPOS *start_pos,
        nmeaPOS *end_pos,
        double azimuth,
        double distance
        );

int     nmea_move_horz_ellipsoid(
        const nmeaPOS *start_pos,
        nmeaPOS *end_pos,
        double azimuth,
        double distance,
        double *end_azimuth
        );





#line 61 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c"
#line 1 "./include/nmea/units.h"








 








 







 



#line 62 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c"

#line 1 "/opt/TI/TMS470CGT4.6.1/include/string.h"
 
 
 
 




#line 17 "/opt/TI/TMS470CGT4.6.1/include/string.h"
 






typedef unsigned size_t;




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

#line 64 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c"
#line 1 "/opt/TI/TMS470CGT4.6.1/include/stdio.h"
 
 
 
 



#line 1 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"
 
 
 
 




#line 15 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"






typedef char *va_list;










#line 44 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"

#line 73 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"





#line 10 "/opt/TI/TMS470CGT4.6.1/include/stdio.h"

 
 
 
 




 
 
 
#line 29 "/opt/TI/TMS470CGT4.6.1/include/stdio.h"

#line 37 "/opt/TI/TMS470CGT4.6.1/include/stdio.h"

 
 
 





typedef struct {
      int fd;                     
      unsigned char* buf;         
      unsigned char* pos;         
      unsigned char* bufend;      
      unsigned char* buff_stop;   
      unsigned int   flags;       
} FILE;

#line 65 "/opt/TI/TMS470CGT4.6.1/include/stdio.h"
typedef long fpos_t;



 
 
 
 
 
 
#line 101 "/opt/TI/TMS470CGT4.6.1/include/stdio.h"

#line 115 "/opt/TI/TMS470CGT4.6.1/include/stdio.h"







 
 
 



























 



 
 
 



extern  FILE _ftable[10];
extern  char _tmpnams[10][16];

 
 
 
 
 
 
extern  int     remove(const char *_file);
extern  int     rename(const char *_old, const char *_new);
extern  FILE   *tmpfile(void);
extern  char   *tmpnam(char *_s);

 
 
 
extern  int     fclose(FILE *_fp); 
extern  FILE   *fopen(const char *_fname, const char *_mode);
extern  FILE   *freopen(const char *_fname, const char *_mode,
			            register FILE *_fp);
extern  void    setbuf(register FILE *_fp, char *_buf);
extern  int     setvbuf(register FILE *_fp, register char *_buf, 
			            register int _type, register size_t _size);
extern  int     fflush(register FILE *_fp); 

 
 
 
extern  int fprintf(FILE *_fp, const char *_format, ...);
extern  int fscanf(FILE *_fp, const char *_fmt, ...);
extern  int emo_printf(const char *_format, ...);
extern  int scanf(const char *_fmt, ...);
extern  int sprintf(char *_string, const char *_format, ...);
extern  int snprintf(char *_string, size_t _n, 
				 const char *_format, ...);
extern  int sscanf(const char *_str, const char *_fmt, ...);
extern  int vfprintf(FILE *_fp, const char *_format, va_list _ap);
extern  int vprintf(const char *_format, va_list _ap);
extern  int vsprintf(char *_string, const char *_format,
				 va_list _ap);
extern  int vsnprintf(char *_string, size_t _n, 
				  const char *_format, va_list _ap);

 
 
 
extern  int     fgetc(register FILE *_fp);
extern  char   *fgets(char *_ptr, register int _size,
				  register FILE *_fp);
extern  int     fputc(int _c, register FILE *_fp);
extern  int     fputs(const char *_ptr, register FILE *_fp);
extern  int     getc(FILE *_p);
extern  int     getchar(void);
extern  char   *gets(char *_ptr); 
extern  int     putc(int _x, FILE *_fp);
extern  int     putchar(int _x);
extern  int     puts(const char *_ptr); 
extern  int     ungetc(int _c, register FILE *_fp);

 
 
 
extern  size_t  fread(void *_ptr, size_t _size, size_t _count,
				  FILE *_fp);
extern  size_t  fwrite(const void *_ptr, size_t _size,
				   size_t _count, register FILE *_fp); 

 
 
 
extern  int     fgetpos(FILE *_fp, fpos_t *_pos);
extern  int     fseek(register FILE *_fp, long _offset,
				  int _ptrname);
extern  int     fsetpos(FILE *_fp, const fpos_t *_pos);
extern  long    ftell(FILE *_fp);
extern  void    rewind(register FILE *_fp); 

 
 
 
extern  void    clearerr(FILE *_fp);
extern  int     feof(FILE *_fp);
extern  int     ferror(FILE *_fp);
extern  void    perror(const char *_s);















#line 324 "/opt/TI/TMS470CGT4.6.1/include/stdio.h"


#line 65 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c"

int _nmea_parse_time(const char *buff, int buff_sz, nmeaTIME *res)
{
    int success = 0;

    switch(buff_sz)
    {
    case sizeof("hhmmss") - 1:
        success = (3 == nmea_scanf(buff, buff_sz,
            "%2d%2d%2d", &(res->hour), &(res->min), &(res->sec)
            ));
        break;
    case sizeof("hhmmss.s") - 1:
    case sizeof("hhmmss.ss") - 1:
    case sizeof("hhmmss.sss") - 1:
        success = (4 == nmea_scanf(buff, buff_sz,
            "%2d%2d%2d.%d", &(res->hour), &(res->min), &(res->sec), &(res->hsec)
            ));
        break;
    default:
        nmea_error("Parse of time error (format error)!");
        success = 0;
        break;
    }

    return (success?0:-1);        
}







 
int nmea_pack_type(const char *buff, int buff_sz)
{
    static const char *pheads[] = {
        "GPGGA",
        "GPGSA",
        "GPGSV",
        "GPRMC",
        "GPVTG",
    };

     _assert((buff) != 0, "Assertion failed, (" "buff" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "110" "\n");

    if(buff_sz < 5)
        return GPNON;
    else if(0 == memcmp(buff, pheads[0], 5))
        return GPGGA;
    else if(0 == memcmp(buff, pheads[1], 5))
        return GPGSA;
    else if(0 == memcmp(buff, pheads[2], 5))
        return GPGSV;
    else if(0 == memcmp(buff, pheads[3], 5))
        return GPRMC;
    else if(0 == memcmp(buff, pheads[4], 5))
        return GPVTG;

    return GPNON;
}







 
int nmea_find_tail(const char *buff, int buff_sz, int *res_crc)
{
    static const int tail_sz = 3   + 2  ;

    const char *end_buff = buff + buff_sz;
    int nread = 0;
    int crc = 0;

     _assert((buff && res_crc) != 0, "Assertion failed, (" "buff && res_crc" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "143" "\n");

    *res_crc = -1;

    for(;buff < end_buff; ++buff, ++nread)
    {
        if(('$' == *buff) && nread)
        {
            buff = 0;
            break;
        }
        else if('*' == *buff)
        {
            if(buff + tail_sz <= end_buff && '\r' == buff[3] && '\n' == buff[4])
            {
                *res_crc = nmea_atoi(buff + 1, 2, 16);
                nread = buff_sz - (int)(end_buff - (buff + tail_sz));
                if(*res_crc != crc)
                {
                    *res_crc = -1;
                    buff = 0;
                }
            }

            break;
        }
        else if(nread)
            crc ^= (int)*buff;
    }

    if(*res_crc < 0 && buff)
        nread = 0;

    return nread;
}







 
int nmea_parse_GPGGA(const char *buff, int buff_sz, nmeaGPGGA *pack)
{
    char time_buff[(256)];

     _assert((buff && pack) != 0, "Assertion failed, (" "buff && pack" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "190" "\n");

    memset(pack, 0, sizeof(nmeaGPGGA));

    nmea_trace_buff(buff, buff_sz);

    if(14 != nmea_scanf(buff, buff_sz,
        "$GPGGA,%s,%f,%C,%f,%C,%d,%d,%f,%f,%C,%f,%C,%f,%d*",
        &(time_buff[0]),
        &(pack->lat), &(pack->ns), &(pack->lon), &(pack->ew),
        &(pack->sig), &(pack->satinuse), &(pack->HDOP), &(pack->elv), &(pack->elv_units),
        &(pack->diff), &(pack->diff_units), &(pack->dgps_age), &(pack->dgps_sid)))
    {
        nmea_error("GPGGA parse error!");
        return 0;
    }

    if(0 != _nmea_parse_time(&time_buff[0], (int)strlen(&time_buff[0]), &(pack->utc)))
    {
        nmea_error("GPGGA time parse error!");
        return 0;
    }

    return 1;
}







 
int nmea_parse_GPGSA(const char *buff, int buff_sz, nmeaGPGSA *pack)
{
     _assert((buff && pack) != 0, "Assertion failed, (" "buff && pack" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "225" "\n");

    memset(pack, 0, sizeof(nmeaGPGSA));

    nmea_trace_buff(buff, buff_sz);

    if(17 != nmea_scanf(buff, buff_sz,
        "$GPGSA,%C,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f*",
        &(pack->fix_mode), &(pack->fix_type),
        &(pack->sat_prn[0]), &(pack->sat_prn[1]), &(pack->sat_prn[2]), &(pack->sat_prn[3]), &(pack->sat_prn[4]), &(pack->sat_prn[5]),
        &(pack->sat_prn[6]), &(pack->sat_prn[7]), &(pack->sat_prn[8]), &(pack->sat_prn[9]), &(pack->sat_prn[10]), &(pack->sat_prn[11]),
        &(pack->PDOP), &(pack->HDOP), &(pack->VDOP)))
    {
        nmea_error("GPGSA parse error!");
        return 0;
    }

    return 1;
}







 
int nmea_parse_GPGSV(const char *buff, int buff_sz, nmeaGPGSV *pack)
{
    int nsen, nsat;

     _assert((buff && pack) != 0, "Assertion failed, (" "buff && pack" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "256" "\n");

    memset(pack, 0, sizeof(nmeaGPGSV));

    nmea_trace_buff(buff, buff_sz);

    nsen = nmea_scanf(buff, buff_sz,
        "$GPGSV,%d,%d,%d,"
        "%d,%d,%d,%d,"
        "%d,%d,%d,%d,"
        "%d,%d,%d,%d,"
        "%d,%d,%d,%d*",
        &(pack->pack_count), &(pack->pack_index), &(pack->sat_count),
        &(pack->sat_data[0].id), &(pack->sat_data[0].elv), &(pack->sat_data[0].azimuth), &(pack->sat_data[0].sig),
        &(pack->sat_data[1].id), &(pack->sat_data[1].elv), &(pack->sat_data[1].azimuth), &(pack->sat_data[1].sig),
        &(pack->sat_data[2].id), &(pack->sat_data[2].elv), &(pack->sat_data[2].azimuth), &(pack->sat_data[2].sig),
        &(pack->sat_data[3].id), &(pack->sat_data[3].elv), &(pack->sat_data[3].azimuth), &(pack->sat_data[3].sig));

    nsat = (pack->pack_index - 1) * (4);
    nsat = (nsat + (4) > pack->sat_count)?pack->sat_count - nsat:(4);
    nsat = nsat * 4 + 3  ;

    if(nsen < nsat || nsen > ((4) * 4 + 3))
    {
        nmea_error("GPGSV parse error!");
        return 0;
    }

    return 1;
}







 
int nmea_parse_GPRMC(const char *buff, int buff_sz, nmeaGPRMC *pack)
{
    int nsen;
    char time_buff[(256)];

     _assert((buff && pack) != 0, "Assertion failed, (" "buff && pack" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "299" "\n");

    memset(pack, 0, sizeof(nmeaGPRMC));

    nmea_trace_buff(buff, buff_sz);

    nsen = nmea_scanf(buff, buff_sz,
        "$GPRMC,%s,%C,%f,%C,%f,%C,%f,%f,%2d%2d%2d,%f,%C,%C*",
        &(time_buff[0]),
        &(pack->status), &(pack->lat), &(pack->ns), &(pack->lon), &(pack->ew),
        &(pack->speed), &(pack->direction),
        &(pack->utc.day), &(pack->utc.mon), &(pack->utc.year),
        &(pack->declination), &(pack->declin_ew), &(pack->mode));

    if(nsen != 13 && nsen != 14)
    {
        nmea_error("GPRMC parse error!");
        return 0;
    }

    if(0 != _nmea_parse_time(&time_buff[0], (int)strlen(&time_buff[0]), &(pack->utc)))
    {
        nmea_error("GPRMC time parse error!");
        return 0;
    }

    if(pack->utc.year < 90)
        pack->utc.year += 100;
    pack->utc.mon -= 1;

    return 1;
}







 
int nmea_parse_GPVTG(const char *buff, int buff_sz, nmeaGPVTG *pack)
{
     _assert((buff && pack) != 0, "Assertion failed, (" "buff && pack" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "341" "\n");

    memset(pack, 0, sizeof(nmeaGPVTG));

    nmea_trace_buff(buff, buff_sz);

    if(8 != nmea_scanf(buff, buff_sz,
        "$GPVTG,%f,%C,%f,%C,%f,%C,%f,%C*",
        &(pack->dir), &(pack->dir_t),
        &(pack->dec), &(pack->dec_m),
        &(pack->spn), &(pack->spn_n),
        &(pack->spk), &(pack->spk_k)))
    {
        nmea_error("GPVTG parse error!");
        return 0;
    }

    if( pack->dir_t != 'T' ||
        pack->dec_m != 'M' ||
        pack->spn_n != 'N' ||
        pack->spk_k != 'K')
    {
        nmea_error("GPVTG parse error (format error)!");
        return 0;
    }

    return 1;
}





 
void nmea_GPGGA2info(nmeaGPGGA *pack, nmeaINFO *info)
{
     _assert((pack && info) != 0, "Assertion failed, (" "pack && info" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "377" "\n");

    info->utc.hour = pack->utc.hour;
    info->utc.min = pack->utc.min;
    info->utc.sec = pack->utc.sec;
    info->utc.hsec = pack->utc.hsec;
    info->sig = pack->sig;
    info->HDOP = pack->HDOP;
    info->elv = pack->elv;
    info->lat = ((pack->ns == 'N')?pack->lat:-(pack->lat));
    info->lon = ((pack->ew == 'E')?pack->lon:-(pack->lon));
    info->smask |= GPGGA;
}





 
void nmea_GPGSA2info(nmeaGPGSA *pack, nmeaINFO *info)
{
    int i, j, nuse = 0;

     _assert((pack && info) != 0, "Assertion failed, (" "pack && info" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "400" "\n");

    info->fix = pack->fix_type;
    info->PDOP = pack->PDOP;
    info->HDOP = pack->HDOP;
    info->VDOP = pack->VDOP;

    for(i = 0; i < (12); ++i)
    {
        for(j = 0; j < info->satinfo.inview; ++j)
        {
            if(pack->sat_prn[i] && pack->sat_prn[i] == info->satinfo.sat[j].id)
            {
                info->satinfo.sat[j].in_use = 1;
                nuse++;
            }
        }
    }

    info->satinfo.inuse = nuse;
    info->smask |= GPGSA;
}





 
void nmea_GPGSV2info(nmeaGPGSV *pack, nmeaINFO *info)
{
    int isat, isi, nsat;

     _assert((pack && info) != 0, "Assertion failed, (" "pack && info" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "432" "\n");

    if(pack->pack_index > pack->pack_count ||
        pack->pack_index * (4) > (12))
        return;

    if(pack->pack_index < 1)
        pack->pack_index = 1;

    info->satinfo.inview = pack->sat_count;

    nsat = (pack->pack_index - 1) * (4);
    nsat = (nsat + (4) > pack->sat_count)?pack->sat_count - nsat:(4);

    for(isat = 0; isat < nsat; ++isat)
    {
        isi = (pack->pack_index - 1) * (4) + isat;
        info->satinfo.sat[isi].id = pack->sat_data[isat].id;
        info->satinfo.sat[isi].elv = pack->sat_data[isat].elv;
        info->satinfo.sat[isi].azimuth = pack->sat_data[isat].azimuth;
        info->satinfo.sat[isi].sig = pack->sat_data[isat].sig;
    }

    info->smask |= GPGSV;
}





 
void nmea_GPRMC2info(nmeaGPRMC *pack, nmeaINFO *info)
{
     _assert((pack && info) != 0, "Assertion failed, (" "pack && info" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "465" "\n");

    if('A' == pack->status)
    {
        if((0) == info->sig)
            info->sig = (2);
        if((1) == info->fix)
            info->fix = (2);
    }
    else if('V' == pack->status)
    {
        info->sig = (0);
        info->fix = (1);
    }

    info->utc = pack->utc;
    info->lat = ((pack->ns == 'N')?pack->lat:-(pack->lat));
    info->lon = ((pack->ew == 'E')?pack->lon:-(pack->lon));
    info->speed = pack->speed * (1.852);
    info->direction = pack->direction;
    info->smask |= GPRMC;
}





 
void nmea_GPVTG2info(nmeaGPVTG *pack, nmeaINFO *info)
{
     _assert((pack && info) != 0, "Assertion failed, (" "pack && info" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parse.c" ", line " "495" "\n");

    info->direction = pack->dir;
    info->declination = pack->dec;
    info->speed = pack->spk;
    info->smask |= GPVTG;
}
