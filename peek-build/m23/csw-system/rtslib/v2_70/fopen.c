/*****************************************************************************/
/*  FOPEN.C v2.54                                                            */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* FUNCTIONS:                                                                */
/*    FOPEN       -  Open a file and return a pointer to it                  */
/*    FREOPEN     -  Switch a FILE pointer to a different stream             */
/*    _OPENFILE   -  Set file flags, and call lowlevel OPEN                  */
/*    _SEARCH_FP  -  Find an open slot in the file table                     */
/*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <_lock.h>
#include "file.h"
 
extern _DATA_ACCESS int _ft_end;
extern _DATA_ACCESS void (*_cleanup_ptr)(void);

extern void   _cleanup(void);
 
static FILE *_search_fp(void);
static FILE *_openfile(const char *_fname, register FILE *_fp, const char
                       *_mode);


/*****************************************************************************/
/* FOPEN    -  Open a file and return a pointer to it                        */
/*                                                                           */
/*    This function calls _SEARCH_FP to locate an empty slot in the file     */
/*    table (_ftable), and calls _OPENFILE with it to open a stream to the   */
/*    file _FNAME.  It returns a pointer to the stream that was allocated,   */
/*    or NULL if it was not successful.                                      */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS FILE *fopen(const char *_fname, const char *_mode)
{
    FILE *f;

    /*-----------------------------------------------------------------------*/
    /* This is a critical section because search_fp looks for a new file     */
    /* slot in the global table _ftable.				     */
    /*-----------------------------------------------------------------------*/
    _lock();
    f = _openfile(_fname, _search_fp(), _mode);
    _unlock();
    return f;
}

 
/*****************************************************************************/
/* FREOPEN  -  Switch a FILE pointer to a different stream                   */
/*                                                                           */
/*    This function attempts to close any file associated with _FP, and then */
/*    reassigns _FP to the file _FNAME by calling _OPENFILE.  It returns a   */
/*    pointer to the stream if successful, or NULL if the function is not    */
/*    successful.                                                            */
/*                                                                           */
/*****************************************************************************/
_CODE_ACCESS FILE *freopen(const char *_fname, const char *_mode, register FILE *_fp)
{
    FILE *f;

    /*-----------------------------------------------------------------------*/
    /* This is a critical section because it expects the same slot in the    */
    /* global table _ftable to be available.				     */
    /*-----------------------------------------------------------------------*/
    _lock();
    fclose(_fp); 
    f = _openfile(_fname, _fp, _mode);
    _unlock();
    return f;
}


/*****************************************************************************/
/* _SEARCH_FP  -  Find an open slot in the file table                        */
/*                                                                           */
/*    This function looks through the array _ftable, searching for a null    */
/*    pointer which indicates an empty slot.  It mallocs a stream for that   */
/*    slot, and assigns a pointer to it there, initializing it and returning */
/*    it to the calling function.                                            */
/*                                                                           */
/*****************************************************************************/
static FILE *_search_fp(void)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int index;
	
   /*------------------------------------------------------------------------*/
   /* Search the file table for an empty slot.  Return a NULL is there       */
   /* aren't any available.                                                  */
   /*------------------------------------------------------------------------*/
   for(index = 0; (index < _ft_end) && _ftable[index].fd != -1; index++);
   if (index == _NFILE) return (NULL);
   if (index == _ft_end) _ft_end++;

   /*------------------------------------------------------------------------*/
   /* Initialize the new stream.                                             */
   /*------------------------------------------------------------------------*/
   memset(&_ftable[index], '\0', sizeof(FILE));

   return (&_ftable[index]);
}
 

/*****************************************************************************/
/* _OPENFILE   -  Set file flags, and call lowlevel OPEN                     */
/*                                                                           */
/*    This function sets FILE flags for reading or writing by the character  */
/*    string _MODE.  It then calls the lowlevel OPEN function to open the    */
/*    file, and assigns the file descriptor returned from that function into */
/*    the FD member of the stream.                                           */
/*                                                                           */
/*****************************************************************************/
static FILE *_openfile(const char *_fname, register FILE *_fp, const char 
                       *_mode)
{
   /*------------------------------------------------------------------------*/
   /* Local variables                                                        */
   /*------------------------------------------------------------------------*/
   int         wr, 
               bin = 0, 
               plus = 0;
   unsigned    lflags = 0;

   if (! _fp) return (NULL);
 
   /*------------------------------------------------------------------------*/
   /* SETUP _CLEANUP_PTR SO THAT ALL OPENED FILES WILL BE CLOSED AT EXIT.    */
   /*------------------------------------------------------------------------*/
   _cleanup_ptr = _cleanup;
   
   /*------------------------------------------------------------------------*/
   /* Set the flags in the stream to reflect to I/O mode of the stream to be */
   /* opened.                                                                */
   /*------------------------------------------------------------------------*/
   wr    = _mode[0];
   if (_mode[1])
   {
      bin  = ((_mode[1] == 'b') || (_mode[2] == 'b'));
      plus = ((_mode[1] == '+') || (_mode[2] == '+'));
   }

   _fp->flags = 0;

   if(!plus)
      _SET(_fp, (wr == 'r') ? _MODER : 
                (wr == 'w' || wr == 'a') ? _MODEW : 0);

   _SET(_fp, (wr == 'a') ? _MODEA : 0);
   _SET(_fp, (bin)  ? _MODEBIN : 0);
   _SET(_fp, (plus) ? _MODERW  : 0);
 
   if(bin) lflags |= (O_BINARY);

   /*------------------------------------------------------------------------*/
   /* Set the flags in LFLAGS to reflect the flags that will be necessary to */
   /* call the lowlevel OPEN function properly for this stream.              */
   /*------------------------------------------------------------------------*/
   switch (wr)
   {
      case 'r' : lflags |= (plus) ? O_RDWR : O_RDONLY;
                 break;
 
      case 'a' : lflags |= (plus) ? O_RDWR : O_WRONLY;
                 lflags |= (O_APPEND | O_CREAT);
                 break;
 
      case 'w' : lflags |= (plus) ? O_RDWR : O_WRONLY;
                 lflags |= (O_TRUNC | O_CREAT);
                 break;
   }

   /*------------------------------------------------------------------------*/
   /* Call the lowlevel OPEN function, and store the returned file           */
   /* descriptor into the stream.  If the OPEN function fails, return NULL.  */
   /*------------------------------------------------------------------------*/
   if ((_fp->fd = open(_fname, lflags, 0666)) < 0) return (NULL);

   return (_fp);
}


/*****************************************************************************/
/* _CLEANUP   -  CLOSE ALL OPEN STREAMS.  THIS IS CALLED BY EXIT() IF A FILE */
/*               IS EVER OPENED OR A BUFFER IS EVER CREATED.                 */
/*****************************************************************************/
void _cleanup(void)
{
   int j;

   /*------------------------------------------------------------------------*/
   /* CLOSE ALL STREAMS THAT ARE CURRENTLY OPEN.                             */
   /*------------------------------------------------------------------------*/
   /* This is a critical section because it depends on the global variable   */
   /* _ft_end.								     */
   /*------------------------------------------------------------------------*/
   _lock();
   fclose(&_ftable[0]);
   for(j = 1; j < _ft_end; j++) 
       if (_ftable[j].fd > 0) fclose(&_ftable[j]); 
   _unlock();
}
