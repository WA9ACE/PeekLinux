#line 1 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/tok.c"








 

 

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





#line 14 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/tok.c"

#line 1 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"
 
 
 
 




#line 15 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"






typedef char *va_list;










#line 44 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"

#line 73 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"





#line 16 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/tok.c"
#line 1 "/opt/TI/TMS470CGT4.6.1/include/stdlib.h"
 
 
 
 




 
 
 
 




 
 
 
#line 27 "/opt/TI/TMS470CGT4.6.1/include/stdlib.h"

#line 35 "/opt/TI/TMS470CGT4.6.1/include/stdlib.h"

typedef struct { int quot, rem; } div_t;

typedef struct { int quot, rem; } ldiv_t;


typedef struct { long long quot, rem; } lldiv_t;

#line 49 "/opt/TI/TMS470CGT4.6.1/include/stdlib.h"







typedef unsigned size_t;





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

#line 17 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/tok.c"
#line 1 "/opt/TI/TMS470CGT4.6.1/include/stdio.h"
 
 
 
 



#line 1 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"
 
 
 
 

#line 77 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"

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


#line 18 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/tok.c"
#line 1 "/opt/TI/TMS470CGT4.6.1/include/ctype.h"
 
 
 
 

 
 
 
 
 
 
 
 





 
 
 
 




 
 
 
#line 37 "/opt/TI/TMS470CGT4.6.1/include/ctype.h"

#line 46 "/opt/TI/TMS470CGT4.6.1/include/ctype.h"

extern const  unsigned char _ctypes_[];

 
 
 
#line 1 "/opt/TI/TMS470CGT4.6.1/include/_isfuncdcl.h"

extern  int isalnum(int _c);
extern  int isalpha(int _c);
extern  int iscntrl(int _c);
extern  int isdigit(int _c);
extern  int isgraph(int _c);
extern  int islower(int _c);
extern  int isprint(int _c);
extern  int ispunct(int _c);
extern  int isspace(int _c);
extern  int isupper(int _c);
extern  int isxdigit(int _c);
extern  int isascii(int _c);
extern  int toascii(int _c);

#line 53 "/opt/TI/TMS470CGT4.6.1/include/ctype.h"

extern  int toupper(int _c);
extern  int tolower(int _c);





#line 97 "/opt/TI/TMS470CGT4.6.1/include/ctype.h"

 
 
 
 
 
 
 
#line 113 "/opt/TI/TMS470CGT4.6.1/include/ctype.h"

#line 125 "/opt/TI/TMS470CGT4.6.1/include/ctype.h"








#line 179 "/opt/TI/TMS470CGT4.6.1/include/ctype.h"



#line 200 "/opt/TI/TMS470CGT4.6.1/include/ctype.h"

#line 19 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/tok.c"
#line 1 "/opt/TI/TMS470CGT4.6.1/include/string.h"
 
 
 
 




#line 17 "/opt/TI/TMS470CGT4.6.1/include/string.h"
 











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

#line 20 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/tok.c"
#line 1 "/opt/TI/TMS470CGT4.6.1/include/limits.h"
 
 
 
 




 
 
 
 




#line 27 "/opt/TI/TMS470CGT4.6.1/include/limits.h"






#line 40 "/opt/TI/TMS470CGT4.6.1/include/limits.h"
 
















#line 21 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/tok.c"








 
int nmea_calc_crc(const char *buff, int buff_sz)
{
    int chsum = 0,
        it;

    for(it = 0; it < buff_sz; ++it)
        chsum ^= (int)buff[it];

    return chsum;
}



 
int nmea_atoi(const char *str, int str_sz, int radix)
{
    char *tmp_ptr;
    char buff[(256)];
    int res = 0;

    if(str_sz < (256))
    {
        memcpy(&buff[0], str, str_sz);
        buff[str_sz] = '\0';
        res = strtol(&buff[0], &tmp_ptr, radix);
    }

    return res;
}



 
double nmea_atof(const char *str, int str_sz)
{
    char *tmp_ptr;
    char buff[(256)];
    double res = 0;

    if(str_sz < (256))
    {
        memcpy(&buff[0], str, str_sz);
        buff[str_sz] = '\0';
        res = strtod(&buff[0], &tmp_ptr);
    }

    return res;
}



 
int nmea_printf(char *buff, int buff_sz, const char *format, ...)
{
    int retval, add = 0;
    va_list arg_ptr;

    if(buff_sz <= 0)
        return 0;

    ((arg_ptr) = (__va_argref(format) ? ( va_list)((int)__va_parmadr(format) + 4 ) : (sizeof(format) < sizeof(int)) ? ( va_list)((int)__va_parmadr(format) + 4 & ~3) : ( va_list)((int)__va_parmadr(format) + sizeof(format))));

    retval = vsnprintf(buff, buff_sz, format, arg_ptr);

    if(retval > 0)
    {
        add = snprintf(
            buff + retval, buff_sz - retval, "*%02x\r\n",
            nmea_calc_crc(buff + 1, retval - 1));
    }

    retval += add;

    if(retval < 0 || retval > buff_sz)
    {
        memset(buff, ' ', buff_sz);
        retval = buff_sz;
    }

    ;

    return retval;
}



 
int nmea_scanf(const char *buff, int buff_sz, const char *format, ...)
{
    const char *beg_tok;
    const char *end_buf = buff + buff_sz;

    va_list arg_ptr;
    int tok_type = (1);
    int width = 0;
    const char *beg_fmt = 0;
    int snum = 0, unum = 0;

    int tok_count = 0;
    void *parg_target;

    ((arg_ptr) = (__va_argref(format) ? ( va_list)((int)__va_parmadr(format) + 4 ) : (sizeof(format) < sizeof(int)) ? ( va_list)((int)__va_parmadr(format) + 4 & ~3) : ( va_list)((int)__va_parmadr(format) + sizeof(format))));
    
    for(; *format && buff < end_buf; ++format)
    {
        switch(tok_type)
        {
        case (1):
            if('%' == *format)
                tok_type = (2);
            else if(*buff++ != *format)
                goto fail;
            break;
        case (2):
            width = 0;
            beg_fmt = format;
            tok_type = (3);
        case (3):
            if(isdigit(*format))
                break;
            {
                tok_type = (4);
                if(format > beg_fmt)
                    width = nmea_atoi(beg_fmt, (int)(format - beg_fmt), 10);
            }
        case (4):
            beg_tok = buff;

            if(!width && ('c' == *format || 'C' == *format) && *buff != format[1])
                width = 1;

            if(width)
            {
                if(buff + width <= end_buf)
                    buff += width;
                else
                    goto fail;
            }
            else
            {
                if(!format[1] || (0 == (buff = (char *)memchr(buff, format[1], end_buf - buff))))
                    buff = end_buf;
            }

            if(buff > end_buf)
                goto fail;

            tok_type = (1);
            tok_count++;

            parg_target = 0; width = (int)(buff - beg_tok);

            switch(*format)
            {
            case 'c':
            case 'C':
                parg_target = (void *)(__va_argref(char *) ? ((arg_ptr += sizeof(char * *)),(* *(char * * *)(arg_ptr-(sizeof(char * *))))) : ((arg_ptr += (((sizeof(char *)) + 3) & ~3)),(*(char * *)(arg_ptr-(sizeof(char *))))));
                if(width && 0 != (parg_target))
                    *((char *)parg_target) = *beg_tok;
                break;
            case 's':
            case 'S':
                parg_target = (void *)(__va_argref(char *) ? ((arg_ptr += sizeof(char * *)),(* *(char * * *)(arg_ptr-(sizeof(char * *))))) : ((arg_ptr += (((sizeof(char *)) + 3) & ~3)),(*(char * *)(arg_ptr-(sizeof(char *))))));
                if(width && 0 != (parg_target))
                {
                    memcpy(parg_target, beg_tok, width);
                    ((char *)parg_target)[width] = '\0';
                }
                break;
            case 'f':
            case 'g':
            case 'G':
            case 'e':
            case 'E':
                parg_target = (void *)(__va_argref(double *) ? ((arg_ptr += sizeof(double * *)),(* *(double * * *)(arg_ptr-(sizeof(double * *))))) : ((arg_ptr += (((sizeof(double *)) + 3) & ~3)),(*(double * *)(arg_ptr-(sizeof(double *))))));
                if(width && 0 != (parg_target))
                    *((double *)parg_target) = nmea_atof(beg_tok, width);
                break;
            };

            if(parg_target)
                break;
            if(0 == (parg_target = (void *)(__va_argref(int *) ? ((arg_ptr += sizeof(int * *)),(* *(int * * *)(arg_ptr-(sizeof(int * *))))) : ((arg_ptr += (((sizeof(int *)) + 3) & ~3)),(*(int * *)(arg_ptr-(sizeof(int *))))))))
                break;
            if(!width)
                break;

            switch(*format)
            {
            case 'd':
            case 'i':
                snum = nmea_atoi(beg_tok, width, 10);
                memcpy(parg_target, &snum, sizeof(int));
                break;
            case 'u':
                unum = nmea_atoi(beg_tok, width, 10);
                memcpy(parg_target, &unum, sizeof(unsigned int));
                break;
            case 'x':
            case 'X':
                unum = nmea_atoi(beg_tok, width, 16);
                memcpy(parg_target, &unum, sizeof(unsigned int));
                break;
            case 'o':
                unum = nmea_atoi(beg_tok, width, 8);
                memcpy(parg_target, &unum, sizeof(unsigned int));
                break;
            default:
                goto fail;
            };

            break;
        };
    }

fail:

    ;

    return tok_count;
}
