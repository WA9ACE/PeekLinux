#line 1 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generate.c"








 

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





#line 12 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generate.c"
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





#line 13 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generate.c"
#line 1 "./include/nmea/generate.h"








 










int     nmea_generate(
        char *buff, int buff_sz,     
        const nmeaINFO *info,        
        int generate_mask            
        );

int     nmea_gen_GPGGA(char *buff, int buff_sz, nmeaGPGGA *pack);
int     nmea_gen_GPGSA(char *buff, int buff_sz, nmeaGPGSA *pack);
int     nmea_gen_GPGSV(char *buff, int buff_sz, nmeaGPGSV *pack);
int     nmea_gen_GPRMC(char *buff, int buff_sz, nmeaGPRMC *pack);
int     nmea_gen_GPVTG(char *buff, int buff_sz, nmeaGPVTG *pack);

void    nmea_info2GPGGA(const nmeaINFO *info, nmeaGPGGA *pack);
void    nmea_info2GPGSA(const nmeaINFO *info, nmeaGPGSA *pack);
void    nmea_info2GPRMC(const nmeaINFO *info, nmeaGPRMC *pack);
void    nmea_info2GPVTG(const nmeaINFO *info, nmeaGPVTG *pack);

int     nmea_gsv_npack(int sat_count);
void    nmea_info2GPGSV(const nmeaINFO *info, nmeaGPGSV *pack, int pack_idx);





#line 14 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generate.c"
#line 1 "./include/nmea/units.h"








 








 







 



#line 15 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generate.c"

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

#line 17 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generate.c"
#line 1 "/opt/TI/TMS470CGT4.6.1/include/stdlib.h"
 
 
 
 




 
 
 
 




 
 
 
#line 27 "/opt/TI/TMS470CGT4.6.1/include/stdlib.h"

#line 35 "/opt/TI/TMS470CGT4.6.1/include/stdlib.h"

typedef struct { int quot, rem; } div_t;

typedef struct { int quot, rem; } ldiv_t;


typedef struct { long long quot, rem; } lldiv_t;

#line 49 "/opt/TI/TMS470CGT4.6.1/include/stdlib.h"













typedef unsigned short wchar_t;










 
 
 
 
 
 
 
      int       abs(int _val); 
      long      labs(long _val);

      long long llabs(long long _val);

     int       atoi(const char *_st);
     long      atol(const char *_st);

     long long atoll(const char *_st);

     int       ltoa(long val, char *buffer);
          extern  double    atof(const char *_st);

     long      strtol(const char *_st, char **_endptr, int _base);
     unsigned long strtoul(const char *_st, char **_endptr,
    					  int _base);

     long long strtoll(const char *_st, char **_endptr, int _base);
     unsigned long long strtoull(const char *_st, char **_endptr,
					     int _base);

     double    strtod(const char *_st, char **_endptr);
     long double strtold(const char *_st, char **_endptr);
    
     int    rand(void);
     void   srand(unsigned _seed);
    
     void  *p_calloc(size_t _num, size_t _size);
     void  *p_malloc(size_t _size);
     void  *p_realloc(void *_ptr, size_t _size);
     void   p_free(void *_ptr);
     void  *memalign(size_t _aln, size_t _size);
    
     void   abort(void); 
     int    atexit(void (*_func)(void));
     void  *bsearch(const void *_key, const void *_base,
    				   size_t _nmemb, size_t _size, 
    			           int (*compar)(const void *,const void *));
     void   qsort(void *_base, size_t _nmemb, size_t _size, 
    			         int (*_compar)(const void *, const void *));
     void   exit(int _status);
    
     div_t  div(int _numer, int _denom);
     ldiv_t ldiv(long _numer, long _denom);

     lldiv_t lldiv(long long _numer, long long _denom);


     char  *getenv(const char *_string);
     int    system(const char *_name);

     int    mblen(const char *, size_t);
     size_t mbstowcs(wchar_t *, const char *, size_t);
     int    mbtowc(wchar_t *, const char *, size_t);

     size_t wcstombs(char *, const wchar_t *, size_t);
     int    wctomb(char *, wchar_t);






#line 159 "/opt/TI/TMS470CGT4.6.1/include/stdlib.h"



#line 208 "/opt/TI/TMS470CGT4.6.1/include/stdlib.h"

#line 18 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generate.c"
#line 1 "/opt/TI/TMS470CGT4.6.1/include/math.h"
 
 
 
 






 
 
 
 




 
 
 
#line 29 "/opt/TI/TMS470CGT4.6.1/include/math.h"


#line 1 "/opt/TI/TMS470CGT4.6.1/include/float.h"
 
 
 
 

 
 
 
 
 






#line 26 "/opt/TI/TMS470CGT4.6.1/include/float.h"

#line 36 "/opt/TI/TMS470CGT4.6.1/include/float.h"
 
#line 46 "/opt/TI/TMS470CGT4.6.1/include/float.h"

#line 32 "/opt/TI/TMS470CGT4.6.1/include/math.h"

#line 42 "/opt/TI/TMS470CGT4.6.1/include/math.h"

#line 50 "/opt/TI/TMS470CGT4.6.1/include/math.h"

 
 
 
        double modf(double x, double *y); 
        double asin(double x);
        double acos(double x);
        double atan(double x);
        double atan2(double y, double x);

        double ceil(double x);



        double cos(double x);
        double cosh(double x);
        double exp(double x);
        double fabs(double x);

        double floor(double x);



         double fmod(double x, double y);
         double frexp(double x, int *exp);
         double ldexp(double x, int exp);
         double log(double x);
         double log10(double x);
         double pow(double x, double y);
         double sin(double x);
         double sinh(double x);
         double tan(double x);
         double tanh(double x);
         double sqrt(double x);

#line 98 "/opt/TI/TMS470CGT4.6.1/include/math.h"

#line 115 "/opt/TI/TMS470CGT4.6.1/include/math.h"

 
 
 
#line 1 "/opt/TI/TMS470CGT4.6.1/include/cpp_inline_math.h"
 
 
 
  
#line 120 "/opt/TI/TMS470CGT4.6.1/include/math.h"


#line 19 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generate.c"

int nmea_gen_GPGGA(char *buff, int buff_sz, nmeaGPGGA *pack)
{
    return nmea_printf(buff, buff_sz,
        "$GPGGA,%02d%02d%02d.%02d,%07.4f,%C,%07.4f,%C,%1d,%02d,%03.1f,%03.1f,%C,%03.1f,%C,%03.1f,%04d",
        pack->utc.hour, pack->utc.min, pack->utc.sec, pack->utc.hsec,
        pack->lat, pack->ns, pack->lon, pack->ew,
        pack->sig, pack->satinuse, pack->HDOP, pack->elv, pack->elv_units,
        pack->diff, pack->diff_units, pack->dgps_age, pack->dgps_sid);
}

int nmea_gen_GPGSA(char *buff, int buff_sz, nmeaGPGSA *pack)
{
    return nmea_printf(buff, buff_sz,
        "$GPGSA,%C,%1d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%03.1f,%03.1f,%03.1f",
        pack->fix_mode, pack->fix_type,
        pack->sat_prn[0], pack->sat_prn[1], pack->sat_prn[2], pack->sat_prn[3], pack->sat_prn[4], pack->sat_prn[5],
        pack->sat_prn[6], pack->sat_prn[7], pack->sat_prn[8], pack->sat_prn[9], pack->sat_prn[10], pack->sat_prn[11],
        pack->PDOP, pack->HDOP, pack->VDOP);
}

int nmea_gen_GPGSV(char *buff, int buff_sz, nmeaGPGSV *pack)
{
    return nmea_printf(buff, buff_sz,
        "$GPGSV,%1d,%1d,%02d,"
        "%02d,%02d,%03d,%02d,"
        "%02d,%02d,%03d,%02d,"
        "%02d,%02d,%03d,%02d,"
        "%02d,%02d,%03d,%02d",
        pack->pack_count, pack->pack_index + 1, pack->sat_count,
        pack->sat_data[0].id, pack->sat_data[0].elv, pack->sat_data[0].azimuth, pack->sat_data[0].sig,
        pack->sat_data[1].id, pack->sat_data[1].elv, pack->sat_data[1].azimuth, pack->sat_data[1].sig,
        pack->sat_data[2].id, pack->sat_data[2].elv, pack->sat_data[2].azimuth, pack->sat_data[2].sig,
        pack->sat_data[3].id, pack->sat_data[3].elv, pack->sat_data[3].azimuth, pack->sat_data[3].sig);
}

int nmea_gen_GPRMC(char *buff, int buff_sz, nmeaGPRMC *pack)
{
    return nmea_printf(buff, buff_sz,
        "$GPRMC,%02d%02d%02d.%02d,%C,%07.4f,%C,%07.4f,%C,%03.1f,%03.1f,%02d%02d%02d,%03.1f,%C,%C",
        pack->utc.hour, pack->utc.min, pack->utc.sec, pack->utc.hsec,
        pack->status, pack->lat, pack->ns, pack->lon, pack->ew,
        pack->speed, pack->direction,
        pack->utc.day, pack->utc.mon + 1, pack->utc.year - 100,
        pack->declination, pack->declin_ew, pack->mode);
}

int nmea_gen_GPVTG(char *buff, int buff_sz, nmeaGPVTG *pack)
{
    return nmea_printf(buff, buff_sz,
        "$GPVTG,%.1f,%C,%.1f,%C,%.1f,%C,%.1f,%C",
        pack->dir, pack->dir_t,
        pack->dec, pack->dec_m,
        pack->spn, pack->spn_n,
        pack->spk, pack->spk_k);
}

void nmea_info2GPGGA(const nmeaINFO *info, nmeaGPGGA *pack)
{
    nmea_zero_GPGGA(pack);

    pack->utc = info->utc;
    pack->lat = fabs(info->lat);
    pack->ns = ((info->lat > 0)?'N':'S');
    pack->lon = fabs(info->lon);
    pack->ew = ((info->lon > 0)?'E':'W');
    pack->sig = info->sig;
    pack->satinuse = info->satinfo.inuse;
    pack->HDOP = info->HDOP;
    pack->elv = info->elv;
}

void nmea_info2GPGSA(const nmeaINFO *info, nmeaGPGSA *pack)
{
    int it;

    nmea_zero_GPGSA(pack);

    pack->fix_type = info->fix;
    pack->PDOP = info->PDOP;
    pack->HDOP = info->HDOP;
    pack->VDOP = info->VDOP;

    for(it = 0; it < (12); ++it)
    {
        pack->sat_prn[it] =
            ((info->satinfo.sat[it].in_use)?info->satinfo.sat[it].id:0);
    }
}

int nmea_gsv_npack(int sat_count)
{
    int pack_count = (int)ceil(((double)sat_count) / (4));

    if(0 == pack_count)
        pack_count = 1;

    return pack_count;
}

void nmea_info2GPGSV(const nmeaINFO *info, nmeaGPGSV *pack, int pack_idx)
{
    int sit, pit;

    nmea_zero_GPGSV(pack);

    pack->sat_count = (info->satinfo.inview <= (12))?info->satinfo.inview:(12);
    pack->pack_count = nmea_gsv_npack(pack->sat_count);

    if(pack->pack_count == 0)
        pack->pack_count = 1;

    if(pack_idx >= pack->pack_count)
        pack->pack_index = pack_idx % pack->pack_count;
    else
        pack->pack_index = pack_idx;

    for(pit = 0, sit = pack->pack_index * (4); pit < (4); ++pit, ++sit)
        pack->sat_data[pit] = info->satinfo.sat[sit];
}

void nmea_info2GPRMC(const nmeaINFO *info, nmeaGPRMC *pack)
{
    nmea_zero_GPRMC(pack);

    pack->utc = info->utc;
    pack->status = ((info->sig > 0)?'A':'V');
    pack->lat = fabs(info->lat);
    pack->ns = ((info->lat > 0)?'N':'S');
    pack->lon = fabs(info->lon);
    pack->ew = ((info->lon > 0)?'E':'W');
    pack->speed = info->speed / (1.852);
    pack->direction = info->direction;
    pack->declination = info->declination;
    pack->declin_ew = 'E';
    pack->mode = ((info->sig > 0)?'A':'N');
}

void nmea_info2GPVTG(const nmeaINFO *info, nmeaGPVTG *pack)
{
    nmea_zero_GPVTG(pack);

    pack->dir = info->direction;
    pack->dec = info->declination;
    pack->spn = info->speed / (1.852);
    pack->spk = info->speed;
}

int nmea_generate(
    char *buff, int buff_sz,
    const nmeaINFO *info,
    int generate_mask
    )
{
    int gen_count = 0, gsv_it, gsv_count;
    int pack_mask = generate_mask;

    nmeaGPGGA gga;
    nmeaGPGSA gsa;
    nmeaGPGSV gsv;
    nmeaGPRMC rmc;
    nmeaGPVTG vtg;

    if(!buff)
        return 0;

    while(pack_mask)
    {
        if(pack_mask & GPGGA)
        {
            nmea_info2GPGGA(info, &gga);
            gen_count += nmea_gen_GPGGA(buff + gen_count, buff_sz - gen_count, &gga);
            pack_mask &= ~GPGGA;
        }
        else if(pack_mask & GPGSA)
        {
            nmea_info2GPGSA(info, &gsa);
            gen_count += nmea_gen_GPGSA(buff + gen_count, buff_sz - gen_count, &gsa);
            pack_mask &= ~GPGSA;
        }
        else if(pack_mask & GPGSV)
        {
            gsv_count = nmea_gsv_npack(info->satinfo.inview);
            for(gsv_it = 0; gsv_it < gsv_count && buff_sz - gen_count > 0; ++gsv_it)
            {
                nmea_info2GPGSV(info, &gsv, gsv_it);
                gen_count += nmea_gen_GPGSV(buff + gen_count, buff_sz - gen_count, &gsv);
            }
            pack_mask &= ~GPGSV;
        }
        else if(pack_mask & GPRMC)
        {
            nmea_info2GPRMC(info, &rmc);
            gen_count += nmea_gen_GPRMC(buff + gen_count, buff_sz - gen_count, &rmc);
            pack_mask &= ~GPRMC;
        }
        else if(pack_mask & GPVTG)
        {
            nmea_info2GPVTG(info, &vtg);
            gen_count += nmea_gen_GPVTG(buff + gen_count, buff_sz - gen_count, &vtg);
            pack_mask &= ~GPVTG;
        }
        else
            break;

        if(buff_sz - gen_count <= 0)
            break;
    }

    return gen_count;
}
