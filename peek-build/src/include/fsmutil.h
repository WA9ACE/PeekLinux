





#ifndef __FSMUTIL_H__

#define __FSMUTIL_H__


/* functions from the fileutil.c */

uint32		GetFirstPath(const char * path, uint32 * start, uint32 * length);
char *		ExtractFilename(const char * path);
uint32		ExtractDir(const char * path, uint32 * end);

uint32		FsmStringMatched(const char * wildcard, const char * target);

uint32		FsmCmpFileName(const char * src, const char * dst, uint32 length);

#endif



