#include "File.h"
#include "ffsapi.h"

struct File_t
{
    FsiHandleT handle;
    char *filename;
};

/*typedef enum {FP_START, FP_END, FP_CURRENT} FilePosition;*/

File *file_openRead(const char *filename)
{
    File *output;
    FsiResultT result;

    output = (File *)p_malloc(sizeof(File));
    if (output == NULL)
        return NULL;
    result = FsiFileOpen(&output->handle, filename, FSI_FILE_OPEN_READ_EXIST);
    if (result != FSI_SUCCESS) {
        emo_printf("Failed to open %s reason %d\n", filename, result);
        p_free(output);
        return NULL;
    }
    output->filename = (char *)p_strdup(filename);
    return output;
}

File *file_openWrite(const char *filename)
{
    File *output;
    FsiResultT result;

    output = (File *)p_malloc(sizeof(File));
    if (output == NULL)
        return NULL;
    result = FsiFileOpen(&output->handle, filename, FSI_FILE_OPEN_WRITE_ALWAYS);
    if (result != FSI_SUCCESS) {
        emo_printf("Failed to open %s reason %d\n", filename, result);
        p_free(output);
        return NULL;
    }
    output->filename = (char *)p_strdup(filename);
    return output;
}

File *file_openAppend(const char *filename)
{
    File *output;
    FsiResultT result;

    output = (File *)p_malloc(sizeof(File));
    if (output == NULL)
        return NULL;
    result = FsiFileOpen(&output->handle, filename, FSI_FILE_OPEN_WRITE_EXIST);
    if (result != FSI_SUCCESS) {
        emo_printf("Failed to open %s reason %d\n", filename, result);
        p_free(output);
        return NULL;
    }
    output->filename = (char *)p_strdup(filename);
    file_seek(output, 0, FP_END);
    return output;
}

int file_read(File *f, int bytes, void *buffer)
{
    uint32 readbytes;
    FsiResultT result;

    readbytes = bytes;
    result = FsiFileRead(buffer, 1, &readbytes, f->handle);
    if (result == 0)
        return (int)readbytes;
    return -1;
}

int file_write(File *f, int bytes, void *buffer)
{
    uint32 writebytes;
    FsiResultT result;

    writebytes = bytes;
    result = FsiFileRead(buffer, 1, &writebytes, f->handle);
    if (result == 0)
        return (int)writebytes;
    return -1;
}

int file_seek(File *f, int offset, FilePosition pos)
{
    FsiFileSeekTypeT from;
    switch (pos) {
        case FP_START: from = FSI_FILE_SEEK_START; break;
        case FP_END: from = FSI_FILE_SEEK_END; break;
        case FP_CURRENT:
        default: from = FSI_FILE_SEEK_CURRENT; break;
    }
    return FsiSeek(f->handle, from, offset);
}

int file_pos(File *f)
{
    uint32 pos;
    FsiResultT result;
    
    result = FsiTell(f->handle, &pos);
    if (result == 0)
        return (int)pos;
    return -1;
}

int file_eof(File *f)
{
    return -1;
}

int file_size(File *f)
{
    FsiResultT result;
    uint32 length;

    result = FsiGetFileLength(f->filename, &length);
    if (result == 0)
        return (int)length;
    return -1;
}

int file_close(File *f)
{
    FsiResultT result;

    result = FsiFileClose(f->handle);
    p_free(f->filename);
    p_free(f);

    return result;
}
