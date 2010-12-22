#include "Platform.h"
#include "File.h"

#include "p_malloc.h"

#include "lgui.h"
#ifdef WIN32
#include <windows.h>
#define fileno _fileno
#endif
#include <stdio.h>
#include <sys/stat.h>
#include <malloc.h>
#include <unistd.h>     
#include <sys/types.h> 
#include <errno.h>      
#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>    
#include <string.h>     

extern void gprs_dataobject_init(void);

const char *cacheBaseDir = "./";

struct File_t {
	FILE *f;
};

struct Thread_t {
        pthread_t *thread;
		unsigned int thread_id;
};

Thread *thread_run(void (*proc)(void *), void *param)
{
        Thread *output;
        pthread_t *thread = malloc(sizeof(pthread_t));

		pthread_create (thread, NULL, (void *) proc, (void *) param);

        if (thread == NULL)
                return NULL;

        output = (Thread *)p_malloc(sizeof(Thread));
        output->thread = thread;

        return output;
}

void thread_kill(Thread *t)
{ /* Is it wise to free thread before exiting? */
	free(t->thread);
	free(t);
	pthread_exit(NULL);
}

File *file_openRead(const char *filename)
{
	FILE *output;
	
	output = fopen(filename, "rb");
	return (File *)output;	
}

File *file_openWrite(const char *filename)
{
	FILE *output;

	output = fopen(filename, "wb");
	return (File *)output;
}

File *file_openAppend(const char *filename)
{
	FILE *output;

	output = fopen(filename, "a");
	return (File *)output;
}

int file_read(File *_f, int bytes, void *buffer)
{
	FILE *f;
	
	f = (FILE *)_f;
	return (int)fread(buffer, 1, bytes, f);
}
int file_write(File *_f, int bytes, const void *buffer)
{
	FILE *f;
	
	f = (FILE *)_f;
	return (int)fwrite(buffer, 1, bytes, f);
}

int file_seek(File *_f, int offset, FilePosition _pos)
{
	FILE *f;
	int pos;
	
	f = (FILE *)_f;
	if (_pos == FP_START)
		pos = SEEK_SET;
	else if (_pos == FP_END)
		pos = SEEK_END;
	else if (_pos == FP_CURRENT)
		pos = SEEK_CUR;
	return fseek(f, offset, pos);
}
int file_pos(File *_f)
{
	FILE *f;
	
	f = (FILE *)_f;
	return (int)ftell(f);
}

int file_eof(File *_f)
{
	FILE *f;
	
	f = (FILE *)_f;
	return (int)feof(f);
}
int file_size(File *_f)
{
	FILE *f;
	struct stat fDetails;

	f = (FILE *)_f;
	fstat(fileno(f), &fDetails);
	return (int)fDetails.st_size;
}

int file_close(File *_f)
{
	FILE *f;

	f = (FILE *)_f;
	return fclose(f);
}

int file_mkdir(const char *directory)
{
	return mkdir(directory, 0755);
}

int file_move(const char *fromstr, const char *tostr)
{
	struct stat statstr;

	if (stat(tostr, &statstr) == 0)
		unlink(tostr);
	return rename(fromstr, tostr);
}

extern unsigned char screenBuf[320*240*2];
void PlatformInit(void)
{
	lgui_attach(screenBuf);
	gprs_dataobject_init();
}
