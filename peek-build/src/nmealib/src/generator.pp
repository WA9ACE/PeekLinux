#line 1 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generator.c"








 

#line 1 "./include/nmea/gmath.h"








 




#line 1 "./include/nmea/info.h"








 

 




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





#line 15 "./include/nmea/gmath.h"

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





#line 12 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generator.c"
#line 1 "./include/nmea/generate.h"








 




#line 1 "./include/nmea/sentence.h"








 

 












 
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





#line 15 "./include/nmea/generate.h"





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





#line 13 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generator.c"
#line 1 "./include/nmea/generator.h"








 












 

struct _nmeaGENERATOR;

enum nmeaGENTYPE
{
    NMEA_GEN_NOISE = 0,
    NMEA_GEN_STATIC,
    NMEA_GEN_ROTATE,

    NMEA_GEN_SAT_STATIC,
    NMEA_GEN_SAT_ROTATE,
    NMEA_GEN_POS_RANDMOVE,

    NMEA_GEN_LAST
};

struct _nmeaGENERATOR * nmea_create_generator(int type, nmeaINFO *info);
void    nmea_destroy_generator(struct _nmeaGENERATOR *gen);

int     nmea_generate_from(
        char *buff, int buff_sz,     
        nmeaINFO *info,              
        struct _nmeaGENERATOR *gen,  
        int generate_mask            
        );



 

typedef int (*nmeaNMEA_GEN_INIT)(struct _nmeaGENERATOR *gen, nmeaINFO *info);
typedef int (*nmeaNMEA_GEN_LOOP)(struct _nmeaGENERATOR *gen, nmeaINFO *info);
typedef int (*nmeaNMEA_GEN_RESET)(struct _nmeaGENERATOR *gen, nmeaINFO *info);
typedef int (*nmeaNMEA_GEN_DESTROY)(struct _nmeaGENERATOR *gen);

typedef struct _nmeaGENERATOR
{
    void                *gen_data;
    nmeaNMEA_GEN_INIT    init_call;
    nmeaNMEA_GEN_LOOP    loop_call;
    nmeaNMEA_GEN_RESET   reset_call;
    nmeaNMEA_GEN_DESTROY destroy_call;
    struct _nmeaGENERATOR *next;

} nmeaGENERATOR;

int     nmea_gen_init(nmeaGENERATOR *gen, nmeaINFO *info);
int     nmea_gen_loop(nmeaGENERATOR *gen, nmeaINFO *info);
int     nmea_gen_reset(nmeaGENERATOR *gen, nmeaINFO *info);
void    nmea_gen_destroy(nmeaGENERATOR *gen);
void    nmea_gen_add(nmeaGENERATOR *to, nmeaGENERATOR *gen);





#line 14 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generator.c"
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





#line 15 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generator.c"

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

#line 17 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generator.c"
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

#line 18 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/generator.c"





double nmea_random(double min, double max)
{
    static double rand_max = 32767;
    double rand_val = rand();
    double bounds = max - min;
    return min + (rand_val * bounds) / rand_max;
}



 

int nmea_gen_init(nmeaGENERATOR *gen, nmeaINFO *info)
{
    int RetVal = 1; int smask = info->smask;
    nmeaGENERATOR *igen = gen;

    nmea_zero_INFO(info);
    info->smask = smask;

    info->lat = (5001.2621);
    info->lon = (3613.0595);

    while(RetVal && igen)
    {
        if(igen->init_call)
            RetVal = (*igen->init_call)(igen, info);
        igen = igen->next;
    }

    return RetVal;
}

int nmea_gen_loop(nmeaGENERATOR *gen, nmeaINFO *info)
{
    int RetVal = 1;

    if(gen->loop_call)
        RetVal = (*gen->loop_call)(gen, info);

    if(RetVal && gen->next)
        RetVal = nmea_gen_loop(gen->next, info);

    return RetVal;
}

int nmea_gen_reset(nmeaGENERATOR *gen, nmeaINFO *info)
{
    int RetVal = 1;

    if(gen->reset_call)
        RetVal = (*gen->reset_call)(gen, info);

    return RetVal;
}

void nmea_gen_destroy(nmeaGENERATOR *gen)
{
    if(gen->next)
    {
        nmea_gen_destroy(gen->next);
        gen->next = 0;
    }

    if(gen->destroy_call)
        (*gen->destroy_call)(gen);

    p_free(gen);
}

void nmea_gen_add(nmeaGENERATOR *to, nmeaGENERATOR *gen)
{
    if(to->next)
        nmea_gen_add(to->next, gen);
    else
        to->next = gen;
}

int nmea_generate_from(
    char *buff, int buff_sz,
    nmeaINFO *info,
    nmeaGENERATOR *gen,
    int generate_mask
    )
{
    int retval;

    if(0 != (retval = nmea_gen_loop(gen, info)))
        retval = nmea_generate(buff, buff_sz, info, generate_mask);

    return retval;
}



 

int nmea_igen_noise_init(nmeaGENERATOR *gen, nmeaINFO *info)
{
    return 1;
}

int nmea_igen_noise_loop(nmeaGENERATOR *gen, nmeaINFO *info)
{
    int it;
    int in_use;

    info->sig = (int)nmea_random(1, 3);
    info->PDOP = nmea_random(0, 9);
    info->HDOP = nmea_random(0, 9);
    info->VDOP = nmea_random(0, 9);
    info->fix = (int)nmea_random(2, 3);
    info->lat = nmea_random(0, 100);
    info->lon = nmea_random(0, 100);
    info->speed = nmea_random(0, 100);
    info->direction = nmea_random(0, 360);
    info->declination = nmea_random(0, 360);
    info->elv = (int)nmea_random(-100, 100);

    info->satinfo.inuse = 0;
    info->satinfo.inview = 0;

    for(it = 0; it < 12; ++it)
    {
        info->satinfo.sat[it].id = it;
        info->satinfo.sat[it].in_use = in_use = (int)nmea_random(0, 3);
        info->satinfo.sat[it].elv = (int)nmea_random(0, 90);
        info->satinfo.sat[it].azimuth = (int)nmea_random(0, 359);
        info->satinfo.sat[it].sig = (int)(in_use?nmea_random(40, 99):nmea_random(0, 40));

        if(in_use)
            info->satinfo.inuse++;
        if(info->satinfo.sat[it].sig > 0)
            info->satinfo.inview++;
    }

    return 1;
}

int nmea_igen_noise_reset(nmeaGENERATOR *gen, nmeaINFO *info)
{
    return 1;
}



 

int nmea_igen_static_loop(nmeaGENERATOR *gen, nmeaINFO *info)
{
    nmea_time_now(&info->utc);
    return 1;
}

int nmea_igen_static_reset(nmeaGENERATOR *gen, nmeaINFO *info)
{
    info->satinfo.inuse = 4;
    info->satinfo.inview = 4;

    info->satinfo.sat[0].id = 1;
    info->satinfo.sat[0].in_use = 1;
    info->satinfo.sat[0].elv = 50;
    info->satinfo.sat[0].azimuth = 0;
    info->satinfo.sat[0].sig = 99;

    info->satinfo.sat[1].id = 2;
    info->satinfo.sat[1].in_use = 1;
    info->satinfo.sat[1].elv = 50;
    info->satinfo.sat[1].azimuth = 90;
    info->satinfo.sat[1].sig = 99;

    info->satinfo.sat[2].id = 3;
    info->satinfo.sat[2].in_use = 1;
    info->satinfo.sat[2].elv = 50;
    info->satinfo.sat[2].azimuth = 180;
    info->satinfo.sat[2].sig = 99;

    info->satinfo.sat[3].id = 4;
    info->satinfo.sat[3].in_use = 1;
    info->satinfo.sat[3].elv = 50;
    info->satinfo.sat[3].azimuth = 270;
    info->satinfo.sat[3].sig = 99;

    return 1;
}

int nmea_igen_static_init(nmeaGENERATOR *gen, nmeaINFO *info)
{
    info->sig = 3;
    info->fix = 3;

    nmea_igen_static_reset(gen, info);

    return 1;
}



 

int nmea_igen_rotate_loop(nmeaGENERATOR *gen, nmeaINFO *info)
{
    int it;
    int count = info->satinfo.inview;
    double deg = 360 / (count?count:1);
    double srt = (count?(info->satinfo.sat[0].azimuth):0) + 5;

    nmea_time_now(&info->utc);

    for(it = 0; it < count; ++it)
    {
        info->satinfo.sat[it].azimuth =
            (int)((srt >= 360)?srt - 360:srt);
        srt += deg;
    }

    return 1;
}

int nmea_igen_rotate_reset(nmeaGENERATOR *gen, nmeaINFO *info)
{
    int it;
    double deg = 360 / 8;
    double srt = 0;

    info->satinfo.inuse = 8;
    info->satinfo.inview = 8;

    for(it = 0; it < info->satinfo.inview; ++it)
    {
        info->satinfo.sat[it].id = it + 1;
        info->satinfo.sat[it].in_use = 1;
        info->satinfo.sat[it].elv = 5;
        info->satinfo.sat[it].azimuth = (int)srt;
        info->satinfo.sat[it].sig = 80;
        srt += deg;
    }

    return 1;
}

int nmea_igen_rotate_init(nmeaGENERATOR *gen, nmeaINFO *info)
{
    info->sig = 3;
    info->fix = 3;

    nmea_igen_rotate_reset(gen, info);

    return 1;
}



 

int nmea_igen_pos_rmove_init(nmeaGENERATOR *gen, nmeaINFO *info)
{    
    info->sig = 3;
    info->fix = 3;
    info->direction = info->declination = 0;
    info->speed = 20;
    return 1;
}

int nmea_igen_pos_rmove_loop(nmeaGENERATOR *gen, nmeaINFO *info)
{
    nmeaPOS crd;

    info->direction += nmea_random(-10, 10);
    info->speed += nmea_random(-2, 3);

    if(info->direction < 0)
        info->direction = 359 + info->direction;
    if(info->direction > 359)
        info->direction -= 359;

    if(info->speed > 40)
        info->speed = 40;
    if(info->speed < 1)
        info->speed = 1;

    nmea_info2pos(info, &crd);
    nmea_move_horz(&crd, &crd, info->direction, info->speed / 3600);
    nmea_pos2info(&crd, info);

    info->declination = info->direction;

    return 1;
}

int nmea_igen_pos_rmove_destroy(nmeaGENERATOR *gen)
{
    return 1;
}



 

nmeaGENERATOR * __nmea_create_generator(int type, nmeaINFO *info)
{
    nmeaGENERATOR *gen = 0;

    switch(type)
    {
    case NMEA_GEN_NOISE:
        if(0 == (gen = p_malloc(sizeof(nmeaGENERATOR))))
            nmea_error("Insufficient memory!");
        else
        {
            memset(gen, 0, sizeof(nmeaGENERATOR));
            gen->init_call = &nmea_igen_noise_init;
            gen->loop_call = &nmea_igen_noise_loop;
            gen->reset_call = &nmea_igen_noise_reset;
        }
        break;
    case NMEA_GEN_STATIC:
    case NMEA_GEN_SAT_STATIC:
        if(0 == (gen = p_malloc(sizeof(nmeaGENERATOR))))
            nmea_error("Insufficient memory!");
        else
        {
            memset(gen, 0, sizeof(nmeaGENERATOR));
            gen->init_call = &nmea_igen_static_init;
            gen->loop_call = &nmea_igen_static_loop;
            gen->reset_call = &nmea_igen_static_reset;
        }
        break;
    case NMEA_GEN_SAT_ROTATE:
        if(0 == (gen = p_malloc(sizeof(nmeaGENERATOR))))
            nmea_error("Insufficient memory!");
        else
        {
            memset(gen, 0, sizeof(nmeaGENERATOR));
            gen->init_call = &nmea_igen_rotate_init;
            gen->loop_call = &nmea_igen_rotate_loop;
            gen->reset_call = &nmea_igen_rotate_reset;
        }
        break;
    case NMEA_GEN_POS_RANDMOVE:
        if(0 == (gen = p_malloc(sizeof(nmeaGENERATOR))))
            nmea_error("Insufficient memory!");
        else
        {
            memset(gen, 0, sizeof(nmeaGENERATOR));
            gen->init_call = &nmea_igen_pos_rmove_init;
            gen->loop_call = &nmea_igen_pos_rmove_loop;
            gen->destroy_call = &nmea_igen_pos_rmove_destroy;
        }
        break;
    case NMEA_GEN_ROTATE:
        gen = __nmea_create_generator(NMEA_GEN_SAT_ROTATE, info);
        nmea_gen_add(gen, __nmea_create_generator(NMEA_GEN_POS_RANDMOVE, info));
        break;
    };

    return gen;
}

nmeaGENERATOR * nmea_create_generator(int type, nmeaINFO *info)
{
    nmeaGENERATOR *gen = __nmea_create_generator(type, info);

    if(gen)
        nmea_gen_init(gen, info);

    return gen;
}

void nmea_destroy_generator(nmeaGENERATOR *gen)
{
    nmea_gen_destroy(gen);
}

