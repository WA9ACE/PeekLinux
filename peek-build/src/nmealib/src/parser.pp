#line 1 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c"








 



 

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





#line 16 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c"
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





#line 17 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c"
#line 1 "./include/nmea/parser.h"








 












 

typedef struct _nmeaPARSER
{
    void *top_node;
    void *end_node;
    unsigned char *buffer;
    int buff_size;
    int buff_use;

} nmeaPARSER;

int     nmea_parser_init(nmeaPARSER *parser);
void    nmea_parser_destroy(nmeaPARSER *parser);

int     nmea_parse(
        nmeaPARSER *parser,
        const char *buff, int buff_sz,
        nmeaINFO *info
        );



 

int     nmea_parser_push(nmeaPARSER *parser, const char *buff, int buff_sz);
int     nmea_parser_top(nmeaPARSER *parser);
int     nmea_parser_pop(nmeaPARSER *parser, void **pack_ptr);
int     nmea_parser_peek(nmeaPARSER *parser, void **pack_ptr);
int     nmea_parser_drop(nmeaPARSER *parser);
int     nmea_parser_buff_clear(nmeaPARSER *parser);
int     nmea_parser_queue_clear(nmeaPARSER *parser);





#line 18 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c"
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





#line 19 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c"

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

#line 21 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c"
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

#line 22 "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c"

typedef struct _nmeaParserNODE
{
    int packType;
    void *pack;
    struct _nmeaParserNODE *next_node;

} nmeaParserNODE;



 




 
int nmea_parser_init(nmeaPARSER *parser)
{
    int resv = 0;
    int buff_size = nmea_property()->parse_buff_size;

     _assert((parser) != 0, "Assertion failed, (" "parser" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c" ", line " "44" "\n");

    if(buff_size < (256))
        buff_size = (256);

    memset(parser, 0, sizeof(nmeaPARSER));

    if(0 == (parser->buffer = p_malloc(buff_size)))
        nmea_error("Insufficient memory!");
    else
    {
        parser->buff_size = buff_size;
        resv = 1;
    }    

    return resv;
}



 
void nmea_parser_destroy(nmeaPARSER *parser)
{
     _assert((parser && parser->buffer) != 0, "Assertion failed, (" "parser && parser->buffer" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c" ", line " "67" "\n");
    p_free(parser->buffer);
    nmea_parser_queue_clear(parser);
    memset(parser, 0, sizeof(nmeaPARSER));
}




 
int nmea_parse(    
    nmeaPARSER *parser,
    const char *buff, int buff_sz,
    nmeaINFO *info
    )
{
    int ptype, nread = 0;
    void *pack = 0;

     _assert((parser && parser->buffer) != 0, "Assertion failed, (" "parser && parser->buffer" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c" ", line " "86" "\n");

    nmea_parser_push(parser, buff, buff_sz);

    while(GPNON != (ptype = nmea_parser_pop(parser, &pack)))
    {
        nread++;

        switch(ptype)
        {
        case GPGGA:
            nmea_GPGGA2info((nmeaGPGGA *)pack, info);
            break;
        case GPGSA:
            nmea_GPGSA2info((nmeaGPGSA *)pack, info);
            break;
        case GPGSV:
            nmea_GPGSV2info((nmeaGPGSV *)pack, info);
            break;
        case GPRMC:
            nmea_GPRMC2info((nmeaGPRMC *)pack, info);
            break;
        case GPVTG:
            nmea_GPVTG2info((nmeaGPVTG *)pack, info);
            break;
        };

        p_free(pack);
    }

    return nread;
}



 

int nmea_parser_real_push(nmeaPARSER *parser, const char *buff, int buff_sz)
{
    int nparsed = 0, crc, sen_sz, ptype;
    nmeaParserNODE *node = 0;

     _assert((parser && parser->buffer) != 0, "Assertion failed, (" "parser && parser->buffer" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c" ", line " "128" "\n");

     
    




 

     
    if(parser->buff_use + buff_sz >= parser->buff_size)
        nmea_parser_buff_clear(parser);

    memcpy(parser->buffer + parser->buff_use, buff, buff_sz);
    parser->buff_use += buff_sz;

     
    for(;;node = 0)
    {
        sen_sz = nmea_find_tail(
            (const char *)parser->buffer + nparsed,
            (int)parser->buff_use - nparsed, &crc);

        if(!sen_sz)
        {
            if(nparsed)
                memcpy(
                parser->buffer,
                parser->buffer + nparsed,
                parser->buff_use -= nparsed);
            break;
        }
        else if(crc >= 0)
        {
            ptype = nmea_pack_type(
                (const char *)parser->buffer + nparsed + 1,
                parser->buff_use - nparsed - 1);

            if(0 == (node = p_malloc(sizeof(nmeaParserNODE))))
                goto mem_fail;

            node->pack = 0;

            switch(ptype)
            {
            case GPGGA:
                if(0 == (node->pack = p_malloc(sizeof(nmeaGPGGA))))
                    goto mem_fail;
                node->packType = GPGGA;
                if(!nmea_parse_GPGGA(
                    (const char *)parser->buffer + nparsed,
                    sen_sz, (nmeaGPGGA *)node->pack))
                {
                    p_free(node);
                    node = 0;
                }
                break;
            case GPGSA:
                if(0 == (node->pack = p_malloc(sizeof(nmeaGPGSA))))
                    goto mem_fail;
                node->packType = GPGSA;
                if(!nmea_parse_GPGSA(
                    (const char *)parser->buffer + nparsed,
                    sen_sz, (nmeaGPGSA *)node->pack))
                {
                    p_free(node);
                    node = 0;
                }
                break;
            case GPGSV:
                if(0 == (node->pack = p_malloc(sizeof(nmeaGPGSV))))
                    goto mem_fail;
                node->packType = GPGSV;
                if(!nmea_parse_GPGSV(
                    (const char *)parser->buffer + nparsed,
                    sen_sz, (nmeaGPGSV *)node->pack))
                {
                    p_free(node);
                    node = 0;
                }
                break;
            case GPRMC:
                if(0 == (node->pack = p_malloc(sizeof(nmeaGPRMC))))
                    goto mem_fail;
                node->packType = GPRMC;
                if(!nmea_parse_GPRMC(
                    (const char *)parser->buffer + nparsed,
                    sen_sz, (nmeaGPRMC *)node->pack))
                {
                    p_free(node);
                    node = 0;
                }
                break;
            case GPVTG:
                if(0 == (node->pack = p_malloc(sizeof(nmeaGPVTG))))
                    goto mem_fail;
                node->packType = GPVTG;
                if(!nmea_parse_GPVTG(
                    (const char *)parser->buffer + nparsed,
                    sen_sz, (nmeaGPVTG *)node->pack))
                {
                    p_free(node);
                    node = 0;
                }
                break;
            default:
                p_free(node);
                node = 0;
                break;
            };

            if(node)
            {
                if(parser->end_node)
                    ((nmeaParserNODE *)parser->end_node)->next_node = node;
                parser->end_node = node;
                if(!parser->top_node)
                    parser->top_node = node;
                node->next_node = 0;
            }
        }

        nparsed += sen_sz;
    }

    return nparsed;

mem_fail:
    if(node)
        p_free(node);

    nmea_error("Insufficient memory!");

    return -1;
}




 
int nmea_parser_push(nmeaPARSER *parser, const char *buff, int buff_sz)
{
    int nparse, nparsed = 0;

    do
    {
        if(buff_sz > parser->buff_size)
            nparse = parser->buff_size;
        else
            nparse = buff_sz;

        nparsed += nmea_parser_real_push(
            parser, buff, nparse);

        buff_sz -= nparse;

    } while(buff_sz);

    return nparsed;
}





 
int nmea_parser_top(nmeaPARSER *parser)
{
    int retval = GPNON;
    nmeaParserNODE *node = (nmeaParserNODE *)parser->top_node;

     _assert((parser && parser->buffer) != 0, "Assertion failed, (" "parser && parser->buffer" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c" ", line " "300" "\n");

    if(node)
        retval = node->packType;

    return retval;
}





 
int nmea_parser_pop(nmeaPARSER *parser, void **pack_ptr)
{
    int retval = GPNON;
    nmeaParserNODE *node = (nmeaParserNODE *)parser->top_node;

     _assert((parser && parser->buffer) != 0, "Assertion failed, (" "parser && parser->buffer" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c" ", line " "318" "\n");

    if(node)
    {
        *pack_ptr = node->pack;
        retval = node->packType;
        parser->top_node = node->next_node;
        if(!parser->top_node)
            parser->end_node = 0;
        p_free(node);
    }

    return retval;
}





 
int nmea_parser_peek(nmeaPARSER *parser, void **pack_ptr)
{
    int retval = GPNON;
    nmeaParserNODE *node = (nmeaParserNODE *)parser->top_node;

     _assert((parser && parser->buffer) != 0, "Assertion failed, (" "parser && parser->buffer" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c" ", line " "343" "\n");

    if(node)
    {
        *pack_ptr = node->pack;
        retval = node->packType;
    }

    return retval;
}





 
int nmea_parser_drop(nmeaPARSER *parser)
{
    int retval = GPNON;
    nmeaParserNODE *node = (nmeaParserNODE *)parser->top_node;

     _assert((parser && parser->buffer) != 0, "Assertion failed, (" "parser && parser->buffer" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c" ", line " "364" "\n");

    if(node)
    {
        if(node->pack)
            p_free(node->pack);
        retval = node->packType;
        parser->top_node = node->next_node;
        if(!parser->top_node)
            parser->end_node = 0;
        p_free(node);
    }

    return retval;
}




 
int nmea_parser_buff_clear(nmeaPARSER *parser)
{
     _assert((parser && parser->buffer) != 0, "Assertion failed, (" "parser && parser->buffer" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c" ", line " "386" "\n");
    parser->buff_use = 0;
    return 1;
}




 
int nmea_parser_queue_clear(nmeaPARSER *parser)
{
     _assert((parser) != 0, "Assertion failed, (" "parser" "), file " "/home/andrey/dev/repo/trunk/peek-build/src/nmealib/src/parser.c" ", line " "397" "\n");
    while(parser->top_node)
        nmea_parser_drop(parser);
    return 1;
}
