/*****************************************************************************
 * $Id: res_cach.c,v 1.6 2002/07/03 08:49:31 af Exp $
 * $Name: rel_1_3_b_7 $
 *----------------------------------------------------------------------------
 * NexGenRESOLV v1.3
 * DNS resolver cache
 *----------------------------------------------------------------------------
 *              Copyright (c) 1998-2001 NexGen Software.
 *
 *    All rights reserved. NexGen Software' source code is an
 *  unpublished work and the use of a copyright notice does not imply
 *  otherwise.  This source code contains confidential, trade secret
 *  material of NexGen Software. Any attempt or participation in
 *  deciphering, decoding, reverse engineering or in any way altering
 *  the source code is strictly prohibited, unless the prior written
 *  consent of NexGen Software is obtained.
 *
 *    This software is supplied under the terms of a license agreement
 *  or nondisclosure agreement with NexGen Software, and may not be
 *  copied or disclosed except in accordance with the terms of that
 *  agreement.
 *
 *----------------------------------------------------------------------------
 * ngResolvCacheEntAlloc()
 * ngResolvCacheEntFree()
 * ngResolvCacheLookup()
 * ngResolvFillWithCacheEnt()
 *----------------------------------------------------------------------------
 * 19/03/2001 - Adrien Felon
 * 03/04/2002 -
 *  - cleanup: renaming exported internal functions (now prefixed by "ng")
 * 03/07/2002 -
 *  - removing ngresolv.h inclusion (now included from ngresolv/resolv.h)
 *****************************************************************************/

#include <ngos.h>
#include <ngresolv/resolv.h>

static int cacheGetFromName( char const * name, NGslvcaent ** ce);
static int cacheGetFromAddr( void const * addr, int af, NGslvcaent ** ce);
static int ngStrICmp( char const * str1, char const * s2);

/*****************************************************************************
 * ngResolvCacheEntAlloc()
 *****************************************************************************
 * Allocates a new cache entry from the cache. If there are no more
 * free entry in the cache, we try to find an expired busy entry. Then
 * if there is no expired busy entry, we take the busy entry which
 * will expire next.
 *****************************************************************************
 * Parameters:
 *   ce          Cache entry returned
 * Return value:
 *   NG_EOK         always
 * Caller:
 *   ngResolvPendingQueryAutomaton()
 */

int ngResolvCacheEntAlloc( NGslvcaent ** ce)
{
  *ce = NULL;
  /* First check if something can be extracted from free list... */
  if ((NGuint) ngResolv.s_cabusyn < ngResolv.s_camax) {
    *ce = ngResolv.s_cafreeq;
    ngResolv.s_cafreeq = (*ce)->sce_next;
    /* Insert it into circular busy list */
    ngResolv.s_cabusyn++;
    if (NULL == ngResolv.s_cabusyq) {
      ngResolv.s_cabusyq = (*ce);
      (*ce)->sce_prev = *ce;
      (*ce)->sce_next = *ce;
    } else {
      (*ce)->sce_prev = ngResolv.s_cabusyq->sce_prev;
      (*ce)->sce_next = ngResolv.s_cabusyq;
      (*ce)->sce_prev->sce_next = (*ce);
      (*ce)->sce_next->sce_prev = (*ce);
    }
  } else {
    int i;
    NGslvcaent * next;
    NGuint curtime;
    /* Else look for the best (expired or next) busy cache entry */
    ngDebugResolv(( NG_DBG_RESOLV, NG_DBG_INIT, 0,
                    "Cache full: using expired entry or next expiring one"));
    i = 0;
    curtime = ngOSClockGetTime();
    *ce = next = ngResolv.s_cabusyq->sce_prev;
    while (i < ngResolv.s_cabusyn) {
      if (0 == (*ce)->sce_pquercount) {
        if ((*ce)->sce_ttl < curtime) break;
        if ((*ce)->sce_ttl <= next->sce_ttl)
          next = (*ce);
      }
      *ce = (NGslvcaent*)((*ce)->sce_prev);
      i++;
    }
    if (i >= ngResolv.s_cabusyn) {
      *ce = next;
    }
  }
  return NG_EOK;
}

/*****************************************************************************
 * ngResolvCacheEntFree()
 *****************************************************************************
 * Free given cache entry and return it to free list.
 *****************************************************************************
 * Parameters:
 *   ce          Cache entry to be freed
 * Return value:
 *   none
 * Caller:
 *   ngResolvPendingQueryAutomaton()
 */

void ngResolvCacheEntFree( NGslvcaent * ce)
{
  /* Removing from busy queue... */
  if (1 == ngResolv.s_cabusyn) {
    /* Removing last entry */
    ngResolv.s_cabusyq = NULL;
  } else {
    NGslvcaent * ceprev = ce->sce_prev;
    NGslvcaent * cenext = ce->sce_next;
    if (ce == ngResolv.s_cabusyq)
      ngResolv.s_cabusyq = cenext;
    ceprev->sce_next = cenext;
    cenext->sce_prev = ceprev;
  }
  ngResolv.s_cabusyn--;
  ce->sce_next = ngResolv.s_cafreeq;
  ngResolv.s_cafreeq = ce;
}

/*****************************************************************************
 * ngResolvCacheLookup()
 *****************************************************************************
 * Search the cache for any entry matching the given query parameter
 *****************************************************************************
 * Parameters:
 *   sq          DNS query parameter
 *   ce          Cache entry returned
 * Return value:
 *   NG_EOK      always
 * Caller:
 *   ngResolvQuery()
 */

int ngResolvCacheLookup( NGslvquer const * sq, NGslvcaent ** ce )
{
  if (sq->sq_flags & NG_RSLVF_QTYPE_GET_NAME) {
    return cacheGetFromAddr( sq->sq_addr, sq->sq_af, ce);
  } else {
    char qname[NG_RSLV_BUF_NAME_LEN];
    ngResolvBuildQueryByName( sq->sq_name, qname, sizeof(qname));
    return cacheGetFromName( qname, ce);
  }
}

/*****************************************************************************
 * ngResolvFillWithCacheEnt()
 *****************************************************************************
 * Fill the hostent and buf parameter with underlying given cache entry
 *****************************************************************************
 * Parameters:
 *   h           Host entry to be set (pointers into buf)
 *   buf         Core memory to copy cache entry
 *   buflen      Size of previous memory
 *   ce          Cache entry returned
 * Return value:
 *   none
 * Caller:
 *   ngResolvPendingQueryAutomaton()
 *   ngResolvQuery()
 */

void ngResolvFillWithCacheEnt(
  NGhostent * h,
  void * buf,
  int buflen,
  NGslvcaent * ce
) {
  char * get;
  int namelen;
  int aliascount;
  int addrcount;
  int ptrcount, i;
  int size;
  int maxsize;
  size_t len_str;

  h->h_name = NULL;
  h->h_aliases = NULL;
  h->h_addrtype = NG_AF_INET;
  h->h_length = sizeof(NGuint);
  h->h_addr_list = NULL;

  if ((unsigned)buflen < NG_RSLV_APP_MIN_BUF) return;

  /*
   * First we need to computes the number of alias we will be able to
   * write into buf. This depends on the amount of mem required to
   * store each alias (1 pointer + a '\0' terminated string)
   */

  /* Skipping first host name */
  get = (char *)ce->sce_buf;
  namelen = ngStrLen( get) + 1;
  get += namelen;

  size = 0;
  aliascount = 0;

  maxsize = sizeof(char*) + 2*sizeof(NGuint*) + namelen + sizeof(NGuint);

  if (buflen > maxsize) {
    /* Set max amount of mem the next loop for alias setting can use */
    maxsize = buflen - maxsize;
    while (*get != '\0') {
      namelen = ngStrLen( get) + 1;
      /* Storing this alias will cost us (sizeof(char *) + namelen) bytes */
      if ((size + sizeof(char*) + namelen ) >= (size_t) maxsize) break;
      size += sizeof(char*) + namelen;
      aliascount++;
      get += namelen;
    }
  }

  /*
   * Now we compute the number of IP addresses we will be able to
   * write into buf. Provided that each IP address will cost us
   * (sizeof(NGuint *) + sizeof(NGuint)) bytes, and that we have
   * already used "size" bytes (may be 0...) for aliases and that we
   * want to save sizeof(NGuint *) bytes for terminating the list of IP
   * address pointer with NULL.
   */

  addrcount = (buflen - (size + sizeof(NGuint *))) /
    (sizeof( NGuint*) + sizeof(NGuint));

  /*
   * Now take into account the number of IP addresses we really have
   * in this cache entry...
   */

  addrcount = NG_MIN( addrcount, ce->sce_addrcount);

  /* Now we know where to start writing stuf... */

  size = (1 + addrcount) * sizeof(NGuint *)
    + (1 + aliascount) * sizeof(char *);

  /* Copy first host name */
  h->h_name = (char *)buf + size;
  get = (char *)ce->sce_buf;
  len_str = ngStrLen( get);
  namelen = NG_MIN( len_str , (size_t) (buflen - (size + 2)));
 
  ngMemCpy( (char *)buf + size, get, namelen);
  size += namelen;
  ((char *)buf)[size++] = '\0';
  get += ngStrLen( get) + 1;

  /* Now ok to fill the alias + addr pointer table */
  ptrcount = 0;

  /* Setting alias pointers + copying them to user memory */
  h->h_aliases = (char **)buf;
  while ( (ptrcount < aliascount) && (*get != '\0') ) {
    namelen = ngStrLen( get) + 1;
    if ((size + namelen) > buflen) break;
    /* Writing the alias into buf */
    ngMemCpy( (char *)buf + size, get, namelen);
    /* Set the pointer */
    ((char **)buf)[ptrcount++] = (char *)buf + size;
    /* Update size */
    size += namelen;
  }
  /* Terminates list of alias pointers */
  ((char **)buf)[ptrcount++] = NULL;

  /* We must make sure that size is a 32 bit aligned offset */
  size = (size + 3) & (~3);

  /* Setting IP address pointers + copying IP addresses to user memory */
  h->h_addr_list = (void **)((char *)buf + sizeof(char *)*ptrcount);
  i = 0;
  while (i < addrcount) {
    if ((size + sizeof( NGuint)) > (size_t) buflen) break;
    /* Writing the IP address into buf */
    ngMemCpy( (char *)buf + size,
              ce->sce_buf + (sizeof(ce->sce_buf) - sizeof( NGuint)*(i+1)),
              sizeof(NGuint) );
    /* Set the pointer... */
    ((NGuint **)buf)[ptrcount++] = (NGuint *)((char *)buf + size);
    /* Update size */
    size += sizeof( NGuint);
    i++;
  }
  /* Terminates list of IP address pointers */
  ((NGuint **)buf)[ptrcount] = NULL;
}

static int cacheGetFromName( char const * name, NGslvcaent ** ce)
{
  NGuint curtime;
  int i;
  i = 0;
  curtime = ngOSClockGetTime();

  *ce = ngResolv.s_cabusyq;
  while (i < ngResolv.s_cabusyn) {
    /* First check that the entry is not expired */
    if ((*ce)->sce_ttl >= curtime) {
      char const * hostname = (char const *)((*ce)->sce_buf);
      /* Checking name and all aliases... */
      while (*hostname != '\0') {
        if (0 == ngStrICmp( hostname, name)) {
          return NG_EOK;
        }
        hostname += ngStrLen( hostname) + 1;
      }
    }
    *ce = (*ce)->sce_next;
    i++;
  }
  *ce = NULL;
  return NG_ENOENT;
}

static int cacheGetFromAddr( void const * addr, int af, NGslvcaent ** ce)
{
  NGuint curtime;
  NGuint inetaddr = (NGuint)addr;
  int i;

  if (af != NG_AF_INET) return NG_ENOENT;

  i = 0;
  curtime = ngOSClockGetTime();

  *ce = ngResolv.s_cabusyq;
  while (i < ngResolv.s_cabusyn) {
    /* Check that the entry is not expired */
    if ( (*ce)->sce_ttl >= curtime) {
      int addridx = 0;
      while (addridx < (*ce)->sce_addrcount) {
        NGuint addr = *(NGuint*)
          ((NGubyte*)(*ce)->sce_buf +
           (sizeof((*ce)->sce_buf) - sizeof( NGuint) * (addridx + 1)));
        if (addr == inetaddr)
          return NG_EOK;
        addridx++;
      }
    }
    *ce = (*ce)->sce_next;
    i++;
  }
  *ce = NULL;
  return NG_ENOENT;
}

static int ngStrICmp( char const * s1, char const * s2)
{
#define TO_LOWER(c) (((c)>='A') && ((c)<='Z'))?((c) - ('A' - 'a')):(c)
  int c1, c2, i;
  i = 0;
  c1 = TO_LOWER( s1[i]);
  c2 = TO_LOWER( s2[i]);
  while ( c1 == c2) {
    if ('\0' == c1) return 0;
    c1 = TO_LOWER( s1[i]);
    c2 = TO_LOWER( s2[i]);
    i++;
  }
  return (c2 - c1);
}

