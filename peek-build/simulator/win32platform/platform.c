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

const char *cacheBaseDir = "./";

struct Semaphore_t {
	HANDLE sem;
};

Semaphore *semaphore_create(int initialValue)
{
	Semaphore *output;

	output = (Semaphore *)p_malloc(sizeof(Semaphore));
	if (output == NULL)
		return NULL;

	output->sem = CreateSemaphore(NULL, initialValue, 255, NULL);
	if (output->sem == NULL) {
		p_free(output);
		return NULL;
	}

	return output;
}

void semaphoreP(Semaphore *s)
{
	if (s == NULL)
		return;

	WaitForSingleObject(s->sem, INFINITE);
}

void semaphoreV(Semaphore *s)
{
	if (s == NULL)
		return;

	ReleaseSemaphore(s->sem, 1, NULL);
}

struct Thread_t {
	HANDLE thread;
	DWORD threadID;
};

Thread *thread_run(void (*proc)(void *), void *param)
{
	Thread *output;
	HANDLE thread;
	DWORD threadID;

	thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)proc, param, 0, &threadID);
	if (thread == NULL)
		return NULL;

	output = (Thread *)p_malloc(sizeof(Thread));
	output->thread = thread;
	output->threadID = threadID;

	return output;
}

struct File_t {
	FILE *f;
};

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
int file_write(File *_f, int bytes, void *buffer)
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

extern unsigned char screenBuf[320*240*2];
void PlatformInit(void)
{
	lgui_attach(screenBuf);
}