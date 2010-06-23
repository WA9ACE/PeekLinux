#line 1 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/context.c"








 

#line 1 "./include/nmea/context.h"








 




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





#line 15 "./include/nmea/context.h"








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





#line 12 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/context.c"

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

#line 14 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/context.c"
#line 1 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"
 
 
 
 




#line 15 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"






typedef char *va_list;










#line 44 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"

#line 73 "/opt/TI/TMS470CGT4.6.1/include/stdarg.h"





#line 15 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/context.c"
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


#line 16 "/home/cmw/dev/clean/non-lcd/peek-build/src/nmealib/src/context.c"

nmeaPROPERTY * nmea_property()
{
    static nmeaPROPERTY prop = {
        0, 0, (1024)
        };

    return &prop;
}

void nmea_trace(const char *str, ...)
{
    int size;
    va_list arg_list;
    char buff[(1024)];
    nmeaTraceFunc func = nmea_property()->trace_func;

    if(func)
    {
        ((arg_list) = (__va_argref(str) ? ( va_list)((int)__va_parmadr(str) + 4 ) : (sizeof(str) < sizeof(int)) ? ( va_list)((int)__va_parmadr(str) + 4 & ~3) : ( va_list)((int)__va_parmadr(str) + sizeof(str))));
        size = vsnprintf(&buff[0], (1024) - 1, str, arg_list);
        ;

        if(size > 0)
            (*func)(&buff[0], size);
    }
}

void nmea_trace_buff(const char *buff, int buff_size)
{
    nmeaTraceFunc func = nmea_property()->trace_func;
    if(func && buff_size)
        (*func)(buff, buff_size);
}

void nmea_error(const char *str, ...)
{
    int size;
    va_list arg_list;
    char buff[(1024)];
    nmeaErrorFunc func = nmea_property()->error_func;

    if(func)
    {
        ((arg_list) = (__va_argref(str) ? ( va_list)((int)__va_parmadr(str) + 4 ) : (sizeof(str) < sizeof(int)) ? ( va_list)((int)__va_parmadr(str) + 4 & ~3) : ( va_list)((int)__va_parmadr(str) + sizeof(str))));
        size = vsnprintf(&buff[0], (1024) - 1, str, arg_list);
        ;

        if(size > 0)
            (*func)(&buff[0], size);
    }
}
