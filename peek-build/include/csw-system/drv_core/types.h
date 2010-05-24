#ifndef TYPES_H
#define TYPES_H

typedef int Int32;
typedef unsigned int Uint32;
typedef signed short Int16;
typedef unsigned short Uint16;
typedef signed char Int8;
typedef unsigned char Uint8;
typedef int Bool;

#define NULL 0

#ifndef FALSE
enum
{
	FALSE=0
};
#endif

#ifndef TRUE
enum
{
	TRUE=1
};
#endif

#define LOCAL static 

#endif
