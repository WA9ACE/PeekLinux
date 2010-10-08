/*****************************************************************************/
/*  memory.c  v2.54                                                          */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/
/*									     */
/*  This module contains the functions which implement the dynamic memory    */
/*  management routines. The following assumptions/rules apply:              */
/*									     */
/*   1) Packets are allocated a minimum of MIN_BLOCK + BLOCK_OVERHEAD bytes. */
/*   2) The size of the heap is set at link time, using the -heap flag       */
/*      The allocation and sizing of the heap is a cooperative effort        */
/*      involving the linker, this file, and "sysmem.c".                     */
/*   3) The heap can be reset at any time by calling the function "minit"    */
/*									     */
/*  The following items are defined in this module :			     */
/*    minit()	 : Function to initialize dynamic memory management	     */
/*    malloc()	 : Function to allocate memory from mem mgmt system.	     */
/*    calloc()	 : Allocate an clear memory from mem mgmt system.	     */
/*    realloc()  : Reallocate a packet					     */
/*    free()	 : Function to free allocated memory.			     */
/*    memalign() : Function to allocate aligned memory from mem mgmt system. */
/*---------------------------------------------------------------------------*/
/*    minsert()  : Insert a packet into free list, sorted by size	     */
/*    mremove()  : Remove a packet from the free list.			     */
/*    sys_free	 : Pointer to free list 				     */
/*									     */
/*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <_lock.h>

/*---------------------------------------------------------------------------*/
/* MIN_BLOCK MUST BE A MULTIPLE OF THE SIZEOF (LARGE_TYPE)                   */
/*---------------------------------------------------------------------------*/
#define LARGE_TYPE      long
#define MIN_BLOCK       sizeof(LARGE_TYPE)
#define BLOCK_OVERHEAD  MIN_BLOCK
#define BLOCK_USED      1
#define BLOCK_MASK      (MIN_BLOCK-1)

/*---------------------------------------------------------------------------*/
/* "PACKET" is the template for a data packet.  Packet size contains         */
/* the number of bytes allocated for the user, excluding the size required   */
/* for management of the packet (BLOCK_OVERHEAD bytes).  Packets are always  */
/* allocated memory in MIN_BLOCK byte chunks. The lowest order bit of the    */
/* size field is used to indicate whether the packet is free(0) or used(1).  */
/* The size_ptr field is used to manage the free list, and is a pointer      */
/* to the next element in the free list.  The free list is sorted by size.   */
/*---------------------------------------------------------------------------*/
typedef struct pack 
{
    size_t        packet_size;     /* number of bytes        */
    struct pack  *size_ptr;        /* next elem in free list */
} PACKET;


/*---------------------------------------------------------------------------*/
/* _SYSMEM_SIZE is a symbol whose *address* is set by the linker to the      */
/* size of the heap, via the -heap flag.                                     */
/*---------------------------------------------------------------------------*/
extern int _SYSMEM_SIZE;              /* address = size of heap */

/*---------------------------------------------------------------------------*/
/* Size of the heap area as defined by the linker.                           */
/*---------------------------------------------------------------------------*/
static int _memory_size = (int)&_SYSMEM_SIZE;

/*---------------------------------------------------------------------------*/
/* Define the heap memory area.  Note that allocated space is actually       */
/* (re)allocated in the linker.  The object must appear in the ".sysmem"     */
/* data section, and it must be aligned properly.                            */
/*---------------------------------------------------------------------------*/
/* Autoinitialize the first packet to indicate a single heap packet whose    */
/* size is equal to the total heap minus the size of the header (a PACKET).  */
/* This is equivalent to calling minit() at boot time.                       */
/*---------------------------------------------------------------------------*/
#pragma DATA_SECTION(_sys_memory, ".sysmem")

PACKET _sys_memory[8] = { ((size_t)&_SYSMEM_SIZE) - sizeof(PACKET), 0 };

#define heap_mem ((char *)_sys_memory)

/*---------------------------------------------------------------------------*/
/* SYS_FREE - This variable is a pointer to the free list.                   */
/*---------------------------------------------------------------------------*/
static PACKET *sys_free = _sys_memory;

/*---------------------------------------------------------------------------*/
/* Function declarations                                                     */
/*---------------------------------------------------------------------------*/
static void minsert(PACKET *);         /* insert a packet into the free list */
static void mremove(PACKET *);         /* delete packet from the free list   */
void minit(void);         

/*****************************************************************************/
/*									     */
/*  MINSERT - Insert a packet into the free list.  This list is sorted by    */
/*	      size in increasing order. 				     */
/*									     */
/*****************************************************************************/
static void minsert(PACKET *ptr)
{
    register PACKET *current = (PACKET *) sys_free;
    register PACKET *last    = NULL;

    /*-----------------------------------------------------------------------*/
    /* CHECK SPECIAL CASE, EMPTY FREE LIST.				     */
    /*-----------------------------------------------------------------------*/
    if (current == NULL)
    {
        sys_free      = ptr;
        ptr->size_ptr = NULL;
        return;
    }

    /*-----------------------------------------------------------------------*/
    /* SCAN THROUGH LIST, LOOKING FOR A LARGER PACKET.			     */
    /*-----------------------------------------------------------------------*/
    while (current && current->packet_size < ptr->packet_size)
    {
	last	= current;
	current = current->size_ptr;
    }

    /*-----------------------------------------------------------------------*/
    /* LINK THE NEW PACKET INTO THE LIST. THERE ARE THREE CASES :	     */
    /*	 THE NEW POINTER WILL EITHER BE THE FIRST, THE LAST, OR IN THE	     */
    /*	 MIDDLE SOMEWHERE.						     */
    /*-----------------------------------------------------------------------*/
    if (current == NULL) 	        /* PTR WILL BE LAST IN LIST          */
    {
        last->size_ptr = ptr;
        ptr->size_ptr  = NULL;
    }
    else if (last == NULL)	        /* PTR WILL BE FIRST IN THE LIST     */
    {
        ptr->size_ptr  = sys_free;
        sys_free       = ptr;
    }
    else			        /* PTR IS IN THE MIDDLE OF THE LIST  */
    {
        ptr->size_ptr  = current;
        last->size_ptr = ptr;
    }
}


/*****************************************************************************/
/*									     */
/* MREMOVE - REMOVE AN ITEM FROM THE FREE LIST. 			     */
/*									     */
/*****************************************************************************/
static void mremove(PACKET *ptr)
{
    register PACKET *current = sys_free;
    register PACKET *last    = NULL;

    /*-----------------------------------------------------------------------*/
    /* SCAN THROUGH LIST, LOOKING FOR PACKET TO REMOVE                       */
    /*-----------------------------------------------------------------------*/
    while (current && current != ptr)
    {
	last	= current;
	current = current->size_ptr;
    }

    /*-----------------------------------------------------------------------*/
    /* REMOVE THE PACKET FROM THE LIST.   THERE ARE TWO CASES :              */
    /*   THE OLD POINTER WILL EITHER BE THE FIRST, OR NOT THE FIRST.         */
    /*-----------------------------------------------------------------------*/
    if      (current == NULL) sys_free       = NULL;          /* NOT FOUND   */
    else if (last    == NULL) sys_free       = ptr->size_ptr; /* 1ST IN LIST */
    else                      last->size_ptr = ptr->size_ptr; /* MID OF LIST */
}

/*****************************************************************************/
/*									     */
/*  MINIT - This function can be called by the user to completely reset the  */
/*	    memory management system.					     */
/*									     */
/*****************************************************************************/
void minit(void)
{
    _lock();
    /*-----------------------------------------------------------------------*/
    /* TO INITIALIZE THE MEMORY SYSTEM, SET UP THE FREE LIST TO POINT TO     */
    /* THE ENTIRE HEAP, AND INITIALIZE HEAP TO A SINGLE EMPTY PACKET.        */
    /*-----------------------------------------------------------------------*/
    sys_free = _sys_memory;

    sys_free->packet_size = _memory_size - BLOCK_OVERHEAD;
    sys_free->size_ptr	  = NULL;
    _unlock();
}

/*****************************************************************************/
/*									     */
/*  MALLOC - Allocate a packet of a given size, and return a pointer to it.  */
/*	     This function only allocates in multiples of MIN_BLOCK bytes.   */
/*									     */
/*****************************************************************************/
void *malloc(size_t size)
{
    register PACKET *current;
    register size_t  newsize = (size + BLOCK_MASK) & ~BLOCK_MASK;
    register size_t  oldsize;

    if (size <= 0) return NULL;

    _lock();
    current = sys_free;
    /*-----------------------------------------------------------------------*/
    /* SCAN THROUGH FREE LIST FOR PACKET LARGE ENOUGH TO CONTAIN PACKET      */
    /*-----------------------------------------------------------------------*/
    while (current && current->packet_size < newsize)
       current = current->size_ptr;

    if (!current)
    {
	_unlock();
	return NULL;
    }
    
    oldsize = current->packet_size;	    /* REMEMBER OLD SIZE	     */
    mremove(current);		            /* REMOVE PACKET FROM FREE LIST  */

    /*-----------------------------------------------------------------------*/
    /* IF PACKET IS LARGER THAN NEEDED, FREE EXTRA SPACE AT END	             */
    /* BY INSERTING REMAINING SPACE INTO FREE LIST.			     */
    /*-----------------------------------------------------------------------*/
    if (oldsize - newsize >= (MIN_BLOCK + BLOCK_OVERHEAD))
    {
       register PACKET *next = 
	       (PACKET *) ((char *) current + BLOCK_OVERHEAD + newsize);
 
       next->packet_size    = oldsize - newsize - BLOCK_OVERHEAD;
       minsert(next);
       current->packet_size = newsize;
    }

   current->packet_size |= BLOCK_USED;
   _unlock();
   return (char *)current + BLOCK_OVERHEAD;
}

/*****************************************************************************/
/*									     */
/*  CALLOC - Allocate a packet of a given size, set the data in the packet   */
/*	     to nulls, and return a pointer to it.			     */
/*									     */
/*****************************************************************************/
void *calloc(size_t num, size_t size)
{
    register size_t      i	 = size * num;
    register LARGE_TYPE *current = (LARGE_TYPE *)malloc(i);
    register void       *save    = current;

    if (current == 0) return NULL;

    i = ((i + BLOCK_MASK) & ~BLOCK_MASK) / sizeof(LARGE_TYPE);

    while (i--) *current++ = 0;
    return save;
}

/*****************************************************************************/
/*									     */
/*  REALLOC - Reallocate a packet to a new size.			     */
/*									     */
/*****************************************************************************/
void *realloc(void *packet, size_t size)
{
    register char    *pptr    = (char *) packet - BLOCK_OVERHEAD;
    register size_t   newsize = (size + BLOCK_MASK) & ~BLOCK_MASK;
    register size_t   oldsize;

    if (packet == 0)  return malloc(size);
    if (size   == 0)  { free(packet); return NULL; }

    _lock();

    oldsize = ((PACKET *)pptr)->packet_size;

    if (!(oldsize & BLOCK_USED)) { _unlock(); return NULL; }
    if (newsize == --oldsize)    { _unlock(); return packet; }

    /*-----------------------------------------------------------------------*/
    /* IF NEW SIZE IS LESS THAN CURRENT SIZE, TRUNCATE PACKET AND RETURN END */
    /* TO FREE LIST		                                             */
    /*-----------------------------------------------------------------------*/
    if (newsize < oldsize)
    {
       if (oldsize - newsize < (MIN_BLOCK + BLOCK_OVERHEAD))
       {
	  _unlock();
	  return packet;
       }
       ((PACKET *)pptr)->packet_size = newsize | BLOCK_USED;

       oldsize -= newsize + BLOCK_OVERHEAD;
       pptr    += newsize + BLOCK_OVERHEAD;
       ((PACKET *)pptr)->packet_size = oldsize | BLOCK_USED;
       free(pptr + BLOCK_OVERHEAD);
       _unlock();
       return packet;
    }

    /*-----------------------------------------------------------------------*/
    /* IF NEW SIZE IS BIGGER THAN CURRENT PACKET,		             */
    /*	1) CHECK NEXT PACKET IN LIST, SEE IF PACKET CAN BE EXPANDED          */
    /*	2) IF NOT, MOVE PACKET TO NEW LOCATION. 		             */
    /*-----------------------------------------------------------------------*/
    else
    {
	PACKET *next = (PACKET *)(pptr + oldsize + BLOCK_OVERHEAD);
	int     temp;

	if (((char *)next < &heap_mem[_memory_size - BLOCK_OVERHEAD]) &&
           (!(next->packet_size & BLOCK_USED))                        &&
           ((temp = oldsize +next->packet_size +BLOCK_OVERHEAD -newsize) >= 0))
	{
	    mremove(next);
	    if (temp < MIN_BLOCK + BLOCK_OVERHEAD)
	    {
	       ((PACKET *)pptr)->packet_size = newsize + temp | BLOCK_USED;
	       _unlock();
	       return packet;
	    }

	    ((PACKET *)pptr)->packet_size = newsize | BLOCK_USED;
	    pptr += newsize + BLOCK_OVERHEAD;
	    ((PACKET *)pptr)->packet_size = temp - BLOCK_OVERHEAD;
	    minsert((PACKET *)pptr);
	    _unlock();
	    return packet;
	}
	else
	{
            /*---------------------------------------------------------------*/
	    /* ALLOCATE NEW PACKET AND MOVE DATA INTO IT. 	             */
            /*---------------------------------------------------------------*/
	    register char *new_packet = (char *)malloc(size);
	    if (new_packet == 0) { _unlock(); return NULL; }
	    memcpy(new_packet, packet, oldsize);
	    free(packet);
	    _unlock();
	    return new_packet;
	}
    }
}

/*****************************************************************************/
/*									     */
/*  FREE - Return a packet allocated by malloc to free memory pool.	     */
/*	   Return NULL if successful, -1 if not successful.		     */
/*									     */
/*****************************************************************************/
void free(void *packet)
{
    register char   *ptr = (char *)packet;
    register PACKET *last;	      /* POINT TO PREVIOUS PACKET            */
    register PACKET *current;	      /* POINTER TO THIS PACKET              */
    register PACKET *next;	      /* POINTER TO NEXT PACKET              */

    if (ptr == NULL) return;

    last = next = NULL;		      /* INITIALIZE POINTERS                 */
    ptr -= BLOCK_OVERHEAD;	      /* ADJUST POINT TO BEGINNING OF PACKET */

    _lock();

    current = _sys_memory;

    /*-----------------------------------------------------------------------*/
    /* SEARCH FOR THE POINTER IN THE PACKET POINTED TO			     */
    /*-----------------------------------------------------------------------*/
    while (current < (PACKET *) ptr)
    {
        last    = current;
        current = (PACKET *)((char *)current + 
		  (current->packet_size & ~BLOCK_USED) + BLOCK_OVERHEAD);
    }

    /*-----------------------------------------------------------------------*/
    /* CHECK FOR POINTER OR PACKET ERRORS.				     */
    /*-----------------------------------------------------------------------*/
    if ((current != (PACKET *) ptr) || (!(current->packet_size & BLOCK_USED)))
    {
	 _unlock();
         return;
    }

    current->packet_size &= ~BLOCK_USED;   /* MARK PACKET AS FREE */

    /*-----------------------------------------------------------------------*/
    /* GET POINTER TO NEXT PACKET IN MEMORY, IF ANY.			     */
    /*-----------------------------------------------------------------------*/
    next = (PACKET *) ((char *)current + BLOCK_OVERHEAD +current->packet_size);
    if (next > (PACKET *) &heap_mem[_memory_size - BLOCK_OVERHEAD]) 
	next = NULL;

    if (last->packet_size & BLOCK_USED) last = NULL;
    if (next->packet_size & BLOCK_USED) next = NULL;

    /*-----------------------------------------------------------------------*/
    /* ATTEMPT TO COLLESCE THE THREE PACKETS (PREVIOUS, CURRENT, NEXT)	     */
    /*-----------------------------------------------------------------------*/
    if (last && next)
    {
	mremove(last);
	mremove(next);
	last->packet_size += current->packet_size + next->packet_size + 
			     BLOCK_OVERHEAD + BLOCK_OVERHEAD;
	minsert(last);
	_unlock();
	return;
    }

    /*-----------------------------------------------------------------------*/
    /* ATTEMPT TO COLLESCE THE CURRENT WITH LAST PACKET. (LAST, CURRENT)     */
    /*-----------------------------------------------------------------------*/
    if (last)
    {
	mremove(last);
	last->packet_size += current->packet_size + BLOCK_OVERHEAD;
	minsert(last);
	_unlock();
	return;
    }

    /*-----------------------------------------------------------------------*/
    /* ATTEMPT TO COLLESCE THE CURRENT WITH NEXT PACKET. (CURRENT, NEXT)     */
    /*-----------------------------------------------------------------------*/
    if (next)
    {
       mremove(next);
       current->packet_size += next->packet_size + BLOCK_OVERHEAD;
       minsert(current);
	_unlock();
       return;
    }

    /*-----------------------------------------------------------------------*/
    /* NO COLLESCENCE POSSIBLE, JUST INSERT THIS PACKET INTO LIST	     */
    /*-----------------------------------------------------------------------*/
    minsert(current);
    _unlock();
}

/*****************************************************************************/
/*                                                                           */
/*  MEMALIGN - Allocate a packet of a given size, and on a given boundary.   */
/*                                                                           */
/*****************************************************************************/
void *memalign(size_t alignment, size_t size)
{
    PACKET *aln_packet;
    PACKET *current;
    size_t  newsize  = (size + BLOCK_MASK) & ~BLOCK_MASK;
    size_t  aln_mask = alignment - 1;
    int     leftover = -1;
    char   *aln_start;
    char   *un_aln_start;
 
    if (size <= 0) return NULL;
      
    /*--------------------------------------------------------------------*/
    /* IF ALIGNMENT IS NOT A POWER OF TWO OR IS LESS THAN THE DEFAULT     */
    /* ALIGNMENT OF MALLOC, THEN SIMPLY RETURN WHAT MALLOC RETURNS.       */
    /*--------------------------------------------------------------------*/
    if (alignment <= BLOCK_OVERHEAD || (alignment & (alignment-1)))
	  return malloc(size);

    _lock();
    current = sys_free;

    /*-----------------------------------------------------------------------*/
    /* SCAN THROUGH FREE LIST FOR PACKET LARGE ENOUGH TO CONTAIN ALIGNED     */
    /* PACKET                                                                */
    /*-----------------------------------------------------------------------*/
    for ( ; current ; current = current->size_ptr)
    {
       un_aln_start = (char *) current + BLOCK_OVERHEAD;
       aln_start    = (char *)(((size_t) un_aln_start + aln_mask) & ~aln_mask);
       leftover     = un_aln_start + current->packet_size - aln_start -newsize;

       /*--------------------------------------------------------------------*/
       /* MAKE SURE THAT THE PRE BLOCK SPACE IS LARGE ENOUGH TO BE A BLOCK   */
       /* OF ITS OWN.                                                        */
       /*--------------------------------------------------------------------*/
       for ( ; (char *)current+sizeof(PACKET) > aln_start-BLOCK_OVERHEAD ;
	       aln_start += alignment, leftover -= alignment);

       if (leftover >= 0) break;
    }

    if (!current) { _unlock(); return NULL; }

    /*-----------------------------------------------------------------------*/
    /* SETUP NEW PACKET FOR ALIGNED MEMORY.                                  */
    /*-----------------------------------------------------------------------*/
    mremove(current);
    aln_packet              = (PACKET *) (aln_start - BLOCK_OVERHEAD);
    aln_packet->packet_size = newsize | BLOCK_USED;

    /*-----------------------------------------------------------------------*/
    /* HANDLE THE FREE SPACE BEFORE THE ALIGNED BLOCK.  IF THE ORIGINAL      */
    /* BLOCK WAS ALIGNED, THERE WON'T BE FREE SPACE BEFORE THE ALIGNED BLOCK.*/
    /*-----------------------------------------------------------------------*/
    if (aln_start != un_aln_start) 
    {
	current->packet_size = (char *)aln_packet - un_aln_start;
	minsert(current);
    }

    /*-----------------------------------------------------------------------*/
    /* HANDLE THE FREE SPACE AFTER THE ALIGNED BLOCK. IF IT IS LARGE ENOUGH  */
    /* TO BE A BLOCK OF ITS OWN, THEN MAKE IT ONE, OTHERWISE ADD THE         */
    /* LEFTOVER SIZE TO THE ALIGNED BLOCK.                                   */ 
    /*-----------------------------------------------------------------------*/ 
    if (leftover >= BLOCK_OVERHEAD + MIN_BLOCK)
    {
       register PACKET *next = (PACKET *) (aln_start + newsize);
       next->packet_size     = leftover - BLOCK_OVERHEAD;
       minsert(next);
    }
    else aln_packet->packet_size += leftover;

    _unlock();
    return aln_start;
}
       

/*****************************************************************************/
/*									     */
/*  MEMMAP -  Print dynamic memory allocation statistics                     */
/*									     */
/*****************************************************************************/
#include <stdio.h>
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

void emo_printf(const char *fmt, ...);

void memmap()
{
    PACKET *current;
    int free_block_num		= 0;
    int free_block_space	= 0;
    int free_block_max		= 0;
    int used_block_num		= 0;
    int used_block_space	= 0;
    int used_block_max		= 0;

    _lock();
    current = _sys_memory;

    /*-----------------------------------------------------------------------*/
    /* LOOP THROUGH ALL PACKETS                                              */
    /*-----------------------------------------------------------------------*/
    while (current < (PACKET *) &heap_mem[_memory_size-BLOCK_OVERHEAD]) 
    {
	int size = current->packet_size & ~BLOCK_USED;
        int used = current->packet_size & BLOCK_USED;

        //emo_printf(">> Used:%1d size:%d addr:%x\n", used, size, current);

        if (used)
	{
	    used_block_num++;
	    used_block_space += size;
	    used_block_max   = MAX(used_block_max, size);
	}
	else
	{
	    free_block_num++;
	    free_block_space += size;
	    free_block_max   = MAX(free_block_max, size);
	}

        current = (PACKET *)((char *)current + size + BLOCK_OVERHEAD);
    }

    _unlock();

    emo_printf("free_b_num:%d free_b_space:%d", free_block_num, free_block_space);

		emo_printf("free_b_max:%d used_b_num:%d", free_block_max, used_block_num);

		emo_printf("used_b_space:%d used_b_max:%d", used_block_space, used_block_max);
		emo_printf("overhead:%d", (free_block_num + used_block_num) * BLOCK_OVERHEAD);

    //fflush(stdout);
}
