#ifndef __SYS_SOCK__
#define __SYS_SOCK__
#include "ngsocket.h"
#include "ngresolv.h"
#include "peekerrno.h"

#define htons(a) ((((a) & 0xff) << 8) | (((a) & 0xff00) >> 8))
#define ntohs(a) htons(a)

struct hostent *gethostbyname(const char *name);
#define FD_SETSIZE ((NG_FD_MAX+31)/32)
struct hostent
{
  char *h_name;                 /* Official name of host.  */
  char **h_aliases;             /* Alias list.  */
  int h_addrtype;               /* Host address type.  */
  int h_length;                 /* Length of address.  */
  char **h_addr_list;           /* List of addresses from name server.  */
#define h_addr  h_addr_list[0]  /* Address, for backward compatibility.  */
};

#define MSG_NOSIGNAL        0x4000  /* Do not generate SIGPIPE */
#endif
