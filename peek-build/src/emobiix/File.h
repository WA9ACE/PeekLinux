#ifndef _FILE_H_
#define _FILE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct File_t;
typedef struct File_t File;

typedef enum {FP_START, FP_END, FP_CURRENT} FilePosition;

File *file_openRead(const char *filename);
File *file_openWrite(const char *filename);
File *file_openAppend(const char *filename);

int file_read(File *f, int bytes, void *buffer);
int file_write(File *f, int bytes, void *buffer);

int file_seek(File *f, int offset, FilePosition pos);
int file_pos(File *f);
int file_eof(File *f);
int file_size(File *f);

int file_close(File *f);

int file_mkdir(const char *directory);

#ifdef __cplusplus
}
#endif

#endif /* _FILE_H_ */
