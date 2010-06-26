/*****************************************************************************
 * $Id: strerror.c,v 1.2 2001/03/16 18:42:35 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Standart Librairies Replacement Functions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2001 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *    This software is  supplied  under  the terms of a
 *    license agreement or nondisclosure agreement with
 *    NexGen Software, and may not be copied or disclosed
 *    except  in  accordance  with  the  terms of  that
 *    agreement.
 *
 *----------------------------------------------------------------------------
 * ngStrError()
 *----------------------------------------------------------------------------
 * 29/11/2000 - Regis Feneon
 *****************************************************************************/

#include <ngos.h>

static const struct {
  int err;
  const char *msg;
} msglist[] = {
  NG_EOK, "no error",
  NG_E2BIG, "argument list too long",
  NG_EACCES, "permission denied",
  NG_EADDRINUSE, "address already in use",
  NG_EADDRNOTAVAIL, "address not available from this host",
  NG_EAFNOSUPPORT, "address family not supported",
  NG_EAGAIN, "resource temporarily unavailable",
  NG_EALREADY, "operation already in progress",
  NG_EAUTH, "authentication error",
  NG_EBADF, "bad descriptor",
  NG_EBUSY, "resource busy",
  NG_ECHILD, "no child process",
  NG_ECONNABORTED, "connection aborted by user",
  NG_ECONNREFUSED, "connection refused",
  NG_ECONNRESET, "connection reseted by peer",
  NG_EDEADLK, "resource deadlock avoided",
  NG_EDESTADDRREQ, "destination address required",
  NG_EDOM, "value out of range",
  NG_EEXIST, "file exists",
  NG_EFAULT, "bad address",
  NG_EFBIG, "file too big",
  NG_EHOSTDOWN, "host down",
  NG_EHOSTUNREACH, "host unreachable",
  NG_EIDRM, "identifier removed",
  NG_EINPROGRESS, "operation now in progress",
  NG_EINTR, "interrupted function call",
  NG_EINVAL, "invalid argument",
  NG_EIO, "input/output error",
  NG_EISCONN, "already connected",
  NG_EISDIR, "is a directory",
  NG_ELOOP, "too many levels of symbolic links",
  NG_EMFILE, "out of descriptors",
  NG_EMLINK, "too many links",
  NG_EMSGSIZE, "message too big",
  NG_ENEEDAUTH, "need authenticator",
  NG_ENETDOWN, "network down",
  NG_ENETRESET, "network dropped connection on reset",
  NG_ENETUNREACH, "network unreachable",
  NG_ENFILE, "too many open files in system",
  NG_ENOBUFS, "out of resources",
  NG_ENODEV, "device not found",
  NG_ENOENT, "no such file or directory",
  NG_ENOEXEC, "exec format error",
  NG_ENOLCK, "no locks available",
  NG_ENOMEM, "cannot allocate memory",
  NG_ENOMSG, "no message of desired type",
  NG_ENOPROTOOPT, "not a valid protocol option",
  NG_ENOSPC, "not enougth space to create object",
  NG_ENOSYS, "function not implemented",
  NG_ENOTBLK, "block device required",
  NG_ENOTCONN, "socket not connected",
  NG_ENOTDIR, "not a directory",
  NG_ENOTEMPTY, "directory not empty",
  NG_ENOTSOCK, "not a socket descriptor",
  NG_ENOTTY, "not a tty",
  NG_ENXIO, "device not configured",
  NG_EOPNOTSUPP, "operation not supported",
  NG_EPERM, "operation not permitted",
  NG_EPFNOSUPPORT, "protocol family not supported",
  NG_EPIPE, "pipe broken",
  NG_EPROTONOSUPPORT, "protocol not supported",
  NG_EPROTOTYPE, "wrong protocol type",
  NG_ERANGE, "result too large",
  NG_EROFS, "read-only file system",
  NG_ESHUTDOWN, "can't send after shutdown",
  NG_ESOCKTNOSUPPORT, "socket type not supported",
  NG_ESPIPE, "illegal seek",
  NG_ESRCH, "no such process",
  NG_ETIMEDOUT, "connection timed out",
  NG_ETOOMANYREFS, "too many references",
  NG_EXDEV, "cross-device link",
};

/*****************************************************************************
 * ngStrError()
 *****************************************************************************
 * Returns a string associated with an error code
 *****************************************************************************
 * Parameters:
 *  err     error number
 * Return value: a pointer to the string
 */

const char *ngStrError( int err)
{
  unsigned int i;

  for( i=0; i<sizeof(msglist)/sizeof(msglist[0]); i++) {
    if( msglist[i].err == err)
      return( msglist[i].msg);
  }
  return( "unknown error");
}

