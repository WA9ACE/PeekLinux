/**
 * @file  rfs_api.c
 *
 * API for RFS SWE.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  1/23/2004  ()   Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#include "rfs/rfs_i.h"
#include "rfs/rfs_api.h"
#include "rfs/rfs_fm.h"
#include <string.h> 
#include "relfs.h"

// These 2 numbers convert the speed-factors as provided by GBI to
// the speed in kbit/sec as obtained in benchmark tests:
#define RFS_READ_SPEED_DIVISION_FACTOR  (2000)
#define RFS_WRITE_SPEED_DIVISION_FACTOR (8400)

static T_RFS_RET rfs_search_fd_table_entry(T_RFS_FD  fd, UINT8 *entry);
static T_RFS_RET rfs_get_free_pair_id (void);
static T_RFS_RET rfs_mount_unmount(T_RFS_FSTYPE fs_type, int op);

static const T_RFS_RETURN_PATH rfs_default_return_path =
  { {RVF_INVALID_ADDR_ID, NULL}, NULL, NULL };


/* Table containing all supported media names */
const char      *media_name_table[GBI_MEDIA_TYPE_END] = {GBI_MEDIA_NAMES};


/*Unicode and string operation APIS  */



 T_RFS_RET wstrcmp (const T_WCHAR *str1,const T_WCHAR* str2)
{
   while (*str1 == *str2 && *str1 != '\0') {
      str1++;
      str2++;
   }


   return *str1 - *str2;
}

 void convert_unicode_to_u8(const T_WCHAR *pathname, char *mp)
{
  UINT16  i = 0;

  while (pathname[i] != '\0')
  {
    mp[i] = (char)pathname[i]; //just copy lsb
    i++;
  }
  mp[i] = '\0';  // add end of string
}

  void convert_u8_to_unicode(const char *mp, T_WCHAR *mp_uc)
{
  UINT16  i = 0;

  while (mp[i] != '\0')
  {
    mp_uc[i] = (INT16)mp[i]; //just copy lsb
    i++;
  }
  mp_uc[i] = '\0';  // add end of string
}


void convert_unicode_to_u8_length(const T_WCHAR *pathname, char *mp,UINT32 length)
{
  UINT16  i = 0;

  while ((pathname[i] != '\0')  && (i <length) )
  {
    mp[i] = (char)pathname[i]; //just copy lsb
    i++;
  }
  mp[i] = '\0';  // add end of string
}

  void convert_u8_to_unicode_length(const char *mp, T_WCHAR *mp_uc,UINT32 length)
{
  UINT16  i = 0;

  while ((mp[i] != '\0') && (i <length))
  {
    mp_uc[i] = (INT16)mp[i]; //just copy lsb
    i++;
  }
  mp_uc[i] = '\0';  // add end of string
}


/****************************************************************************/
/*  wstrrchr                                                                 */
/****************************************************************************/
T_WCHAR * wstrchr(const T_WCHAR *s, int c)
{
   T_WCHAR *result = NULL;

   do
   	{
      if (*s == c) 
      	{
	  	   result = (T_WCHAR*)s;
		   return result;
      	}  
   	}
   while (*s++);

   return(result);
}

//returns number of bytes in unicode array
//Terminating null not included
UINT16  wstrlen(const T_WCHAR  *str_uc)
{
  UINT16  i = 0;

  while (str_uc[i] != EOSTR)
  {
    i++;
  }
  return i;
}

void  wstrcpy(  T_WCHAR *dest, const T_WCHAR *src)
{
  UINT16  i = 0;

  while (src[i] != EOSTR)
  {
    dest[i] = src[i];
    i++;
  }
  dest[i] = src[i]; //terminating char
}


void  wstrncpy(  T_WCHAR *dest, const T_WCHAR *src,UINT16 n)
{
  UINT16  i = 0;

  while (src[i] != EOSTR && i <n)
  {
    dest[i] = src[i];
    i++;
  }

  while ( i<n)
  	{
  		dest[i] = EOSTR;
		i++;
  	}	
}


T_WCHAR * wstrcat(T_WCHAR *dest, const T_WCHAR *src)
  {
    const T_WCHAR *p;
    T_WCHAR *q;

    for (q = dest; *q != '\0'; q++)
       ;

    for(p = src; *p != '\0'; p++, q++)
       *q = *p;

    *q = '\0';

    return dest;
  }

 
T_WCHAR *wstrrchr(const T_WCHAR *s, int c)
{
   T_WCHAR *result = NULL;
 
   do
   	{
      if (*s == c) result = (T_WCHAR*)s;
   	} 
   while (*s++);
 
   return(result);
}


/**
 * function:  rfs_fcntl
 */                 
T_RFS_RET rfs_fcntl(T_RFS_FD  fd, 
                    INT8      cmd,
                    void      *arg)
{
  UINT8       *op_mode;
  T_RV_RETURN *ret_path;
  UINT8       fd_table_entry;

  RFS_SEND_TRACE      ("RFS API: rfs_fcntl", RV_TRACE_LEVEL_WARNING);

  /* Immediate Return: T_RFS_RET
   *    (Positive value)  Value of flags (in case of successful F_GETFL command
   *                    execution)
   *    RFS_EOK         Ok (in case of successful command execution others than 
   *                    F_GETFL)
   *    RFS_EBADFD      Invalid file descriptor.
   *    RFS_EINVALID    invalid argument  (Invalid command)
   */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if (fd == RFS_FD_DEF_VALUE)
  {
    /* Invalid file descriptor */
    return RFS_EBADFD;
  }

  if(arg== NULL)
  	{
  	 return RFS_EINVALID;
  	}
  /* Search entry in file descriptor table */
  if (rfs_search_fd_table_entry(fd, &fd_table_entry) == RFS_EBADFD)
  {
    return RFS_EBADFD;
  }

  switch (cmd)
  {
    case F_SETFLAG:
    {
      op_mode = arg;
      if (*op_mode != O_AIO && *op_mode != O_SIO)
      {
        /* op_mode is invalid */
        RFS_SEND_TRACE ("RFS API: op_mode is invalid", RV_TRACE_LEVEL_WARNING);
        return RFS_EINVALID;
      }

      RFS_SEND_TRACE_PARAM("*op_mode", *op_mode, RV_TRACE_LEVEL_DEBUG_LOW);
 
      /* set op_mode in first free file descriptor table entry */
      rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].op_mode = *op_mode;
      break;
    }
    case F_SETRETPATH:
    {
      ret_path = arg;

      RFS_SEND_TRACE("Set return path", RV_TRACE_LEVEL_DEBUG_LOW);

      /* Check return_path */
      if (ret_path == NULL)
      {
        /* return_path is invalid */
        RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
        return RFS_EINVALID;
      }

      /* set return_path in first free FD_TABLE entry */
      rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].client_return_path.return_path = *ret_path;
      break;
    }
    case F_GETFLAG:
    {
      return rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].op_mode;
    }
  
    default: 
    {
      /*  invalid command */
      return RFS_EINVALID;
    }
  }

  return RFS_EOK;
}


/**
 * function:  rfs_close
 */     
T_RFS_RET rfs_close(T_RFS_FD fd)
{
  UINT8         fd_table_entry;
  T_RVF_MUTEX   mutex;
  T_RFS_RET     result;
  T_RFS_CLOSE_REQ_MSG *msg_p;
  T_RFS_RET     pair_id;

  RFS_SEND_TRACE ("RFS API: rfs_close", RV_TRACE_LEVEL_WARNING);

  /*
   *    Synchronous immediate return:
   *    RFS_EOK     Ok.
   *    RFS_EBADFD  The file argument is not a valid file descriptor.
   *
   */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if (fd == RFS_FD_DEF_VALUE)
  {
    return RFS_EBADFD;
  }

  /* Search entry in file descriptor table */
  if (rfs_search_fd_table_entry(fd, &fd_table_entry) == RFS_EBADFD)
  {
    return RFS_EBADFD;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_CLOSE_REQ_MSG),
                       RFS_CLOSE_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS API: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* Compose message */
  msg_p->hdr.msg_id   = RFS_CLOSE_REQ_MSG; 
  msg_p->fd           = fd;
  msg_p->return_path  = rfs_env_ctrl_blk_p->pair_value_map[
                                fd_table_entry].client_return_path.return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Check if this open file requires Asynchronous or synchronous operation */
  if (rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].op_mode == O_SIO)
  {
    /* Initialise and set mutex first time */
    rvf_initialize_mutex (&mutex);
    rvf_lock_mutex       (&mutex);
    /* Adjust message */
    msg_p->mutex        = &mutex;
    msg_p->result       = &result;

    /* Send message to msg handler */
    if (rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p) != RV_OK) 
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY;
    }

    /* Set mutex second time and wait for mutex to be freed */
//    RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);
    rvf_lock_mutex (&mutex);
//    RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

    /* Delete the mutex */
    rvf_delete_mutex (&mutex); 
    return result;
  }
  else
  {
    /* Asynchronous mode, send request message */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY; 
    }
    return pair_id;
  }
}



/**
 * function:  rfs_closedir
 */     
T_RFS_RET rfs_closedir(T_RFS_DIR *dirp)
{
  UINT8         fd_table_entry;
  T_RVF_MUTEX   mutex;
  T_RFS_RET     result;
  T_RFS_CLOSEDIR_REQ_MSG *msg_p;
  T_RFS_RET     pair_id;

  RFS_SEND_TRACE ("RFS API: rfs_close", RV_TRACE_LEVEL_WARNING);

  /*
   *    Synchronous immediate return:
   *    RFS_EOK     Ok.
   *    RFS_EBADFD  The file argument is not a valid file descriptor.
   *
   */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

   /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_CLOSEDIR_REQ_MSG),
                       RFS_CLOSEDIR_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS API: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }



  /* Initialise and set mutex first time */
  rvf_initialize_mutex (&mutex);
  rvf_lock_mutex       (&mutex);

  /* Compose message */
  msg_p->hdr.msg_id   = RFS_CLOSEDIR_REQ_MSG; 
  msg_p->dirp           = dirp;
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  pair_id = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Send message to msg handler */
  if (rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p) != RV_OK) 
  {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY;
   }

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex (&mutex);


  /* Delete the mutex */
  rvf_delete_mutex (&mutex); 
  return result;
 
}



/**
 * function:  rfs_write
 */     
T_RFS_SIZE rfs_write(T_RFS_FD    fd,
                     const void  *buf,
                     T_RFS_SIZE  size)
{  
  UINT8         fd_table_entry;
  T_RVF_MUTEX   mutex;
  T_RFS_RET     result;
  T_RFS_WRITE_REQ_MSG *msg_p;
  T_RFS_RET     pair_id;

  RFS_SEND_TRACE ("RFS API: rfs_write", RV_TRACE_LEVEL_WARNING);

 /*    Synchronous immediate return:
  *    Immediate Return: T_RFS_SIZE
  *    (Positive value) Number of bytes actually written.
  *    RFS_EBADFD       The file argument is not a valid file descriptor.
  *    RFS_EACCES       The file is not writable.
  *    RFS_EBADOP       The file is not open for writing.
  *    RFS_EFBIG        An attempt was made to write a file that exceeds the maximum file size. 
  *    RFS_ENOSPACE     Out of data space.
  *    RFS_EDEVICE      Device I/O error
  */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if (fd == RFS_FD_DEF_VALUE)
  {
    return RFS_EBADFD;
  }

  if(buf == NULL)
  	{
  	 return RFS_EINVALID;
  	}

   if(size <= NULL)
  	{
  	 return RFS_EINVALID;
  	}
  /* Search entry in file descriptor table */
  if (rfs_search_fd_table_entry(fd, &fd_table_entry) == RFS_EBADFD)
  {
    return RFS_EBADFD;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_WRITE_REQ_MSG),
                       RFS_WRITE_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS API: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* Compose message */
  msg_p->hdr.msg_id   = RFS_WRITE_REQ_MSG; 
  msg_p->fd           = fd;
  msg_p->buf          = buf;
  msg_p->size         = size;
  msg_p->return_path  = rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].client_return_path.return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Check if this open file requires Asynchronous or synchronous operation */
  if (rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].op_mode == O_SIO)
  {
    /* Initialise and set mutex first time */
    rvf_initialize_mutex  (&mutex);
    rvf_lock_mutex        (&mutex);

    /* Adjust message */
    msg_p->mutex        = &mutex;
    msg_p->result       = &result;

    /* Send message */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) return RFS_EMEMORY; 

    /* Set mutex second time and wait for mutex to be freed */
    rvf_lock_mutex (&mutex);

    //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

    /* Delete the mutex */
    rvf_delete_mutex (&mutex);
    return result;
  }
  else
  {
    /* Send message */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY; 
    }
    return pair_id;
  }

}

/**
 * function:  rfs_read
 */
T_RFS_SIZE rfs_read(T_RFS_FD   fd,
                    void *buf,
                    T_RFS_SIZE size)
{
  UINT8         fd_table_entry;
  T_RVF_MUTEX   mutex;
  T_RFS_RET     result;
  T_RFS_READ_REQ_MSG *msg_p;
  T_RFS_RET     pair_id;
  
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_RETURN_PATH client_return_path = { {RVF_INVALID_ADDR_ID, NULL}, NULL, NULL };
  UINT8 fscore_nmb;
  
  RFS_SEND_TRACE ("RFS API: rfs_read", RV_TRACE_LEVEL_WARNING);

  /*    Synchronous immediate return:
   *  Immediate Return: T_RFS_SIZE
   *  (Positive value)  Number of bytes actually read.
   *  RFS_EBADFD        The file argument is not a valid file descriptor.
   *  RFS_EACCES        The file is not readable.
   *  RFS_EBADOP        The file is not open for reading.
   *  RFS_ENOSPACE      Out of data space.
   *  RFS_EDEVICE       Device I/O error
   */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if (fd == RFS_FD_DEF_VALUE)
  {
    return RFS_EBADFD;
  }

  if(buf == NULL)
  	{
  	 return RFS_EINVALID;
  	}

   if(size <= NULL)
  	{
  	 return RFS_EINVALID;
  	}


#if (WCP_PROF == 1)
	prf_LogPointOfInterest("RFS FILE READ START\0\0");
#endif


   
  /* Search entry in file descriptor table */
  if (rfs_search_fd_table_entry(fd, &fd_table_entry) == RFS_EBADFD)
  {
    return RFS_EBADFD;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_RFS_READ_REQ_MSG),
                       RFS_READ_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS API: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_READ_REQ_MSG;
  msg_p->buf          = buf;
  msg_p->fd           = fd;
  msg_p->size         = size;
  msg_p->return_path  = rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].client_return_path.return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Check if this open file requires Asynchronous or synchronous operation */
  if (rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].op_mode == O_SIO)
  {
    /* Synchronous mode. (no message send back to the client) */

	client_return_path.mutex = &mutex;
  	client_return_path.result = &result;
  	client_return_path.return_path =rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].client_return_path.return_path;
  	client_pair_value.fd = fd;
  	core_pair_value =	rfs_map_pair_value (client_pair_value, client_pair_value /*unused */ ,
						&client_return_path,
						&fscore_nmb,
						RFS_PAIR_MAP_FD | RFS_PAIR_MAP_TO_CORE |
						RFS_PAIR_MAP_UPDATE_CLIENT_PATH);
	
	if (core_pair_value.fd < 0)
	{
		return RFS_EBADFD;
	}

	if (fscore_nmb==RFS_RELIANCE_NUM)
	{
      result = ffs_read(core_pair_value.fd,buf,size);
	  rvf_free_buf (msg_p);
	}
	else /*FAT file system*/
	{
		/* Initialise and set mutex first time */
    	rvf_initialize_mutex (&mutex);
    	rvf_lock_mutex (&mutex);

    	/* Adjust message */
    	msg_p->mutex        = &mutex;
    	msg_p->result       = &result;

    	if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) return RFS_EMEMORY;

    	//RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

    	/* Set mutex second time and wait for mutex to be freed */
    	rvf_lock_mutex (&mutex);

    	//RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

    	/* Delete the mutex */
    	rvf_delete_mutex (&mutex);
	}	


#if (WCP_PROF == 1)
	prf_LogPointOfInterest("RFS FILE READ END \0\0");
#endif


	
    return result;
  }
  else
  {
    /* Asynchronous mode, send request message */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p))
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY;
    }
    return pair_id;
  }

}

/**
 * function:  rfs_lseek
 */     
T_RFS_OFFSET rfs_lseek(T_RFS_FD     fd,
                       T_RFS_OFFSET offset,
                       T_FSCORE_WHENCE whence)
{
  UINT8           fd_table_entry;
  T_RVF_MUTEX     mutex;
  T_RFS_RET       result;
  T_RFS_LSEEK_REQ_MSG *msg_p;
  T_RFS_RET       pair_id;

  RFS_SEND_TRACE ("RFS API: rfs_lseek", RV_TRACE_LEVEL_WARNING);

  /*    Synchronous immediate return:
   *  Immediate Return: T_RFS_OFFSET
   *  (Positive value)  New position of the file pointer
   *  RFS_EBADFD        The fd argument is not a valid file descriptor.
   *  RFS_EINVALID      The whence argument is not a proper value, or the resulting file offset would be invalid.
   *  RFS_EBADOP        Bad operation. Seek not allowed with the flags used to open the file.
   *  RFS_EDEVICE       Device I/O error
   */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if (fd == RFS_FD_DEF_VALUE)
  {
    return RFS_EBADFD;
  }

  if((whence != RFS_SEEK_SET) && (whence != RFS_SEEK_CUR) && (whence != RFS_SEEK_END))
  	{
  	   return RFS_EINVALID;
  	}
  /* Search entry in file descriptor table */
  if (rfs_search_fd_table_entry(fd, &fd_table_entry) == RFS_EBADFD)
  {
    return RFS_EBADFD;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_LSEEK_REQ_MSG),
                       RFS_LSEEK_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS API: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_LSEEK_REQ_MSG; 
  msg_p->fd           = fd;
  msg_p->offset       = offset;
  msg_p->whence       = whence;
  msg_p->return_path  = rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].client_return_path.return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Check if this open file requires Asynchronous or synchronous operation */
  if (rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].op_mode == O_SIO)
  {
    /* Synchronous mode. (no message send back to the client) */
    
    /* Initialise and set mutex first time */
    rvf_initialize_mutex (&mutex);
    rvf_lock_mutex (&mutex);

    /* Adjust message */
    msg_p->mutex        = &mutex;
    msg_p->result       = &result;

    /* Send message */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) return RFS_EMEMORY; 

    //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

    /* Set mutex second time and wait for mutex to be freed */
    rvf_lock_mutex (&mutex);

    //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

    /* Delete the mutex */
    rvf_delete_mutex (&mutex);
    return result;
  }
  else
  {
    /* Asynchronous mode, send request message  */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY; 
    }
    return pair_id;
  }
}


/**
 * function:  rfs_fchmod
 */     
T_RFS_RET rfs_fchmod(T_RFS_FD    fd,
                     T_RFS_MODE  mode)
{
  UINT8         fd_table_entry;
  T_RVF_MUTEX   mutex;
  T_RFS_RET     result;
  T_RFS_FCHMOD_REQ_MSG *msg_p;
  T_RFS_RET     pair_id;

  RFS_SEND_TRACE ("RFS API: rfs_fchmod", RV_TRACE_LEVEL_WARNING);

  /*    Synchronous immediate return:
   *  Immediate Return: T_RFS_RET
   *  RFS_EOK           Ok
   *  RFS_EBADFD        The fd argument is not a valid file descriptor.
   *  RFS_ENAMETOOLONG  Object's name is too long.
   *  RFS_EACCES        Search permission is denied for a component of the path prefix.
   *  RFS_EBADNAME      Object's name contains illegal characters.
   *  RFS_ENOTAFILE     Object is not a file.
   *  RFS_ENOENT        No such file or directory.
   *  RFS_EINVALID      Bad mod option
   *  RFS_ELOCKED       The file is locked (already opened in a conflicting mode).
   */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if (fd == RFS_FD_DEF_VALUE)
  {
    return RFS_EBADFD;
  }

  /* Search entry in file descriptor table */
  if (rfs_search_fd_table_entry(fd, &fd_table_entry) == RFS_EBADFD)
  {
    return RFS_EBADFD;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_FCHMOD_REQ_MSG),
                       RFS_FCHMOD_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS API: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_FCHMOD_REQ_MSG; 
  msg_p->fd           = fd;
  msg_p->mode         = mode;
  msg_p->return_path  = rfs_env_ctrl_blk_p->pair_value_map[
                                fd_table_entry].client_return_path.return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Check if this open file requires Asynchronous or synchronous operation */
  if (rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].op_mode == O_SIO)
  {
    /* Synchronous mode. (no message send back to the client) */
    
    /* Initialise and set mutex first time */
    rvf_initialize_mutex  (&mutex);
    rvf_lock_mutex        (&mutex);

    /* Adjust message */
    msg_p->mutex        = &mutex;
    msg_p->result       = &result;

    /* Send message */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY; 
    }

    //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

    /* Set mutex second time and wait for mutex to be freed */
    rvf_lock_mutex (&mutex);

    //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

    /* Delete the mutex */
    rvf_delete_mutex (&mutex);
    return result;
  }
  else
  {
    /* Asynchronous mode, send request message  */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY; 
    }
    return pair_id;
  }

}

/**
 * function:  rfs_fstat
 */     
T_RFS_RET rfs_fstat(T_RFS_FD   fd,
                    T_RFS_STAT *stat)
{
  UINT8         fd_table_entry;
  T_RVF_MUTEX   mutex;
  T_RFS_RET     result;
  T_RFS_FSTAT_REQ_MSG *msg_p;
  T_RFS_RET     pair_id;

  RFS_SEND_TRACE ("RFS API: rfs_fstat", RV_TRACE_LEVEL_WARNING);

  /*    Synchronous immediate return:
   *  Immediate Return: T_RFS_RET
   *  RFS_EOK           Ok
   *  RFS_ENOENT        Object not found.
   *  RFS_EBADFD        Bad file descriptor.
   *  RFS_EACCES        Search permission is denied for a component of the path prefix.
   *  RFS_ENAMETOOLONG  Object's name is too long.
   *  RFS_EBADNAME      Object's name contains illegal characters.
   */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if (fd == RFS_FD_DEF_VALUE)
  {
    return RFS_EBADFD;
  }

  if(stat == NULL)
  	{
  	   return RFS_EINVALID;
  	}
  /* Search entry in file descriptor table */
  if (rfs_search_fd_table_entry(fd, &fd_table_entry) == RFS_EBADFD)
  {
    return RFS_EBADFD;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_FSTAT_REQ_MSG),
                       RFS_FSTAT_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS API: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_FSTAT_REQ_MSG; 
  msg_p->fd           = fd;
  msg_p->stat         = stat;
  msg_p->return_path  = rfs_env_ctrl_blk_p->pair_value_map[
                                fd_table_entry].client_return_path.return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Check if this open file requires Asynchronous or synchronous operation */
  if (rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].op_mode == O_SIO)
  {
    /* Synchronous mode. (no message send back to the client) */
    
    /* Initialise and set mutex first time */
    rvf_initialize_mutex (&mutex);
    rvf_lock_mutex (&mutex);

    /* Adjust message */
    msg_p->mutex        = &mutex;
    msg_p->result       = &result;

    /* Send message */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY; 
    }

    //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

    /* Set mutex second time and wait for mutex to be freed */
    rvf_lock_mutex (&mutex);

    //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

    /* Delete the mutex */
    rvf_delete_mutex (&mutex);
    return result;
  }
  else
  {
    /* Asynchronous mode, send request message  */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY; 
    }
    return pair_id;
  }

}

/**
 * function:  rfs_fsync
 */     
T_RFS_RET rfs_fsync(T_RFS_FD fd)
{
  UINT8           fd_table_entry;
  T_RVF_MUTEX     mutex;
  T_RFS_RET       result;
  T_RFS_FSYNC_REQ_MSG *msg_p;
  T_RFS_RET       pair_id;

  RFS_SEND_TRACE ("RFS API: rfs_fsync", RV_TRACE_LEVEL_WARNING);

  /*    Synchronous immediate return:
   *  Immediate Return: T_RFS_RET
   *  RFS_EOK           Ok
   *  RFS_EBADFD        Invalid file descriptor.
   *  RFS_ENOSPACE      Out of data space.
   *  RFS_EFSFULL       File system full, no free inodes.
   *  RFS_EDEVICE       Device I/O error
   */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if (fd == RFS_FD_DEF_VALUE)
  {
    return RFS_EBADFD;
  }

  /* Search entry in file descriptor table */
  if (rfs_search_fd_table_entry(fd, &fd_table_entry) == RFS_EBADFD)
  {
    return RFS_EBADFD;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_FSYNC_REQ_MSG),
                       RFS_FSYNC_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS API: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_FSYNC_REQ_MSG; 
  msg_p->fd           = fd;
  msg_p->return_path  = rfs_env_ctrl_blk_p->pair_value_map[
                                fd_table_entry].client_return_path.return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Check if this open file requires Asynchronous or synchronous operation */
  if (rfs_env_ctrl_blk_p->pair_value_map[fd_table_entry].op_mode == O_SIO)
  {
    /* Synchronous mode. (no message send back to the client) */
    
    /* Initialise and set mutex first time */
    rvf_initialize_mutex (&mutex);
    rvf_lock_mutex (&mutex);

    /* Adjust message */
    msg_p->mutex        = &mutex;
    msg_p->result       = &result;

    /* Send message */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY; 
    }

    //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

    /* Set mutex second time and wait for mutex to be freed */
    rvf_lock_mutex (&mutex);

    //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

    /* Delete the mutex */
    rvf_delete_mutex (&mutex);
    return result;
  }
  else
  {
    /* Asynchronous mode, send request message */
    if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
    {
      rvf_free_buf (msg_p);
      return RFS_EMEMORY; 
    }
    return pair_id;
  }
}


/**
 * function:  rfs_open
 */     
 T_RFS_FD rfs_open	(	const T_WCHAR *pathname, 
                  T_RFS_FLAGS flags, 
                  T_RFS_MODE  mode)
{
  T_RVF_MUTEX   mutex;
  T_RFS_RET     result;
  T_RFS_OPEN_REQ_MSG *msg_p;

  RFS_SEND_TRACE ("RFS API: rfs_open", RV_TRACE_LEVEL_WARNING);

  /*  Immediate Return: T_RFS_FD
   *  (Positive value)  File descriptor of file opened.
   *  RFS_EEXISTS       An object of the same name already exists.
   *  RFS_EACCES        - Search permission is denied for a component of the path prefix.
   *                    - or the required permissions (for reading and/or writing) are 
   *                      denied for the given flags.
   *                    - or RFS_O_CREAT is specified, the file does not exist, and the 
   *                      directory in which it  is to be created does not permit writing.
   *  RFS_ENAMETOOLONG  Object's name is too long.
   *  RFS_EBADNAME      Object's name contains illegal characters.
   *  RFS_ENUMFD        Max number of used file descriptors reach
   *  RFS_ENOENT        No such file or directory.
   *  RFS_EINVALID      Bad open flag options.
   *  RFS_ELOCKED       The file is locked (already opened for writing, in a conflicting mode).
   *  RFS_EMOUNT        Invalid mount point
   */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  }

  /* Initialise and set mutex first time */
  rvf_initialize_mutex (&mutex);
  rvf_lock_mutex (&mutex);

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_OPEN_REQ_MSG),
                       RFS_OPEN_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_open: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_OPEN_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->flags        = flags;
  msg_p->mode         = mode;
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();

  if (rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p) != RV_OK)
  {
    RFS_SEND_TRACE ("RFS API: could not send message", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);  
  return (T_RFS_FD)result;
}


/**
 * function:  rfs_open_nb
 */     
T_RFS_RET rfs_open_nb(const T_WCHAR   *pathname, 
                      T_RFS_FLAGS  flags, 
                      T_RFS_MODE   mode,
                      T_RV_RETURN  return_path)
{
  T_RFS_OPEN_REQ_MSG    *msg_p;
  T_RFS_RET             pair_id;

  RFS_SEND_TRACE ("rfs_open_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RFS_EAGAIN;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

  if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_OPEN_REQ_MSG),
                       RFS_OPEN_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_open_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* Compose message */
  msg_p->hdr.msg_id   = RFS_OPEN_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->flags        = flags;
  msg_p->mode         = mode;
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;
  
  /* Send message to mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p))
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY;
  }

  return pair_id;  
}

/**
 * function:  rfs_chmod
 */     
T_RFS_RET  rfs_chmod(const T_WCHAR  *pathname,
                     T_RFS_MODE  mode)
{
  T_RFS_CHMOD_REQ_MSG *msg_p;
  T_RVF_MUTEX         mutex;
  T_RFS_RET           result;

  RFS_SEND_TRACE ("RFS API: rfs_chmod", RV_TRACE_LEVEL_WARNING);

  /*  Immediate Return: T_RV_RET
   *  RFS_EOK           Ok
   *  RFS_EBADFD        The fd argument is not a valid file descriptor.
   *  RFS_ENAMETOOLONG  Object's name is too long.
   *  RFS_EACCES        Search permission is denied for a component of the path prefix.
   *  RFS_EBADNAME      Object's name contains illegal characters.
   *  RFS_ENOTAFILE     Object is not a file.
   *  RFS_ENOENT        No such file or directory.
   *  RFS_EINVALID      Bad mod option
   *  RFS_ELOCKED       The file is locked (already opened in a conflicting mode).
   *  RFS_EMOUNT        Invalid mount point
   */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  } 

if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* Initialise and set mutex first time */
  rvf_initialize_mutex (&mutex);
  rvf_lock_mutex (&mutex);

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_CHMOD_REQ_MSG),
                       RFS_CHMOD_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_chmod_nb: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_CHMOD_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->mode         = mode;
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();

   
  /* Send message */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  return result;
}

/**
 * function:  rfs_chmod_nb
 */     
T_RFS_RET rfs_chmod_nb(const T_WCHAR*pathname, 
                       T_RFS_MODE   mode,
                       T_RV_RETURN  return_path)
{
  T_RFS_CHMOD_REQ_MSG   *msg_p;
  T_RFS_RET             pair_id;

  RFS_SEND_TRACE ("rfs_chmod_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RFS_EAGAIN;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

  if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_CHMOD_REQ_MSG),
                       RFS_CHMOD_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_chmod_nb: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_CHMOD_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->mode         = mode;
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p) != RV_OK)
  {
    RFS_SEND_TRACE ("RFS API: could not send message", RV_TRACE_LEVEL_WARNING);
    rvf_free_buf (msg_p);
    return RFS_EMEMORY;
  }

  return pair_id;
}

/**
 * function:  rfs_stat
 */     
 T_RFS_RET rfs_stat(const T_WCHAR *pathname,
                   T_RFS_STAT  *stat)
{
  T_RVF_MUTEX mutex;
  T_RFS_RET   result = RFS_OK;
  T_RFS_STAT_REQ_MSG   *msg_p;
  const T_WCHAR *temp_buf = NULL;
  
 const  T_WCHAR root_dir[3]={0x002F,0x0000,0x0000};

  RFS_SEND_TRACE ("RFS API: rfs_stat", RV_TRACE_LEVEL_WARNING);



  /*  Immediate Return: T_RFS_RET

  RFS_EOK Ok.
  RFS_ENOENT  Object not found.
  RFS_EBADFD  Bad file descriptor.
  RFS_EACCES  Search permission is denied for a component of the path prefix.
  RFS_EMOUNT  Invalid mount point
  RFS_ENAMETOOLONG  Object's name is too long.
  RFS_EBADNAME  Object's name contains illegal characters.

  */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if(pathname == NULL || stat == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
 
  /* Initialise and set mutex first time */
  rvf_initialize_mutex (&mutex);
  rvf_lock_mutex (&mutex);

  /* Reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_STAT_REQ_MSG),
                       RFS_STAT_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_stat_nb: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_STAT_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->stat         = stat;
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();

  /* Send message */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  return result;
}


/**
 * function:  rfs_stat_nb
 */     
T_RFS_RET rfs_stat_nb(const T_WCHAR*pathname,
                      T_RFS_STAT  *stat,
                      T_RV_RETURN return_path)
{
  T_RFS_STAT_REQ_MSG   *msg_p;
  T_RFS_RET             pair_id;
  const  T_WCHAR root_dir[3]={0x002F,0x0000,0x0000};

  RFS_SEND_TRACE ("rfs_stat_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RFS_EAGAIN;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

 if(pathname == NULL || stat == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_STAT_REQ_MSG),
                       RFS_STAT_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_stat_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_STAT_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->stat         = stat;
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }
 
  return pair_id; 
}

/**
 * function:  rfs_remove
 */     
T_RFS_RET rfs_remove(const T_WCHAR *pathname)
{
  T_RVF_MUTEX          mutex;
  T_RFS_RET            result;
  T_RFS_REMOVE_REQ_MSG *msg_p;

  RFS_SEND_TRACE ("RFS API: rfs_remove", RV_TRACE_LEVEL_WARNING);

  /*  Immediate Return: T_RFS_RET

  RFS_EOK Ok.
  RFS_ENOENT  File was not found.
  RFS_ENAMETOOLONG  Object's name is too long.
  RFS_EBADNAME  Object's name contains illegal characters.
  RFS_EACCES  Search permission is denied for a component of the path prefix.
  RFS_EACCES  File could not be removed (read-only).
  RFS_ELOCKED The file is open
  RFS_EDEVICE Device I/O error
  RFS_EMOUNT  Invalid mount point
  RFS_ENOTEMPTY The named directory contains files other than '.' and '..' in it.
  */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }


if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_REMOVE_REQ_MSG),
                       RFS_REMOVE_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_remove_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* Initialise and set mutex first time */
  rvf_initialize_mutex (&mutex);
  rvf_lock_mutex (&mutex);

  /* Compose message */
  msg_p->hdr.msg_id   = RFS_REMOVE_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();
     
  /* Send message */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  return result;
}

/**
 * function:  rfs_remove_nb
 */     
T_RFS_RET rfs_remove_nb(const T_WCHAR *pathname,
                        T_RV_RETURN return_path)
{
  T_RFS_REMOVE_REQ_MSG   *msg_p;
  T_RFS_RET              pair_id;

  RFS_SEND_TRACE ("rfs_remove_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RFS_EAGAIN;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

 if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_REMOVE_REQ_MSG),
                       RFS_REMOVE_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_remove_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_REMOVE_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }
 
  return pair_id; 
}

/**
 * function:  rfs_rename
 */     
T_RFS_RET rfs_rename(const T_WCHAR *oldname,
                     const T_WCHAR  *newname)
{
  T_RVF_MUTEX           mutex;
  T_RFS_RET             result;
  T_RFS_RENAME_REQ_MSG  *msg_p;

  RFS_SEND_TRACE ("RFS API: rfs_rename", RV_TRACE_LEVEL_WARNING);

  /*  Immediate Return: T_RFS_RET

  RFS_EOK Ok.
  RFS_ENOENT  oldname  object does not exist.
  RFS_EEXISTS newname object already exists.
  RFS_EACCES  Object could not be modified (read-only).
  RFS_ENAMETOOLONG  Object's name is too long.
  RFS_EBADNAME  Object's name contains illegal characters.
  RFS_EFSFULL Failed to allocate an inode for the changed object.
  RFS_ENOTALLOWED Renaming is not allowed (another new path contains another mountpoint)
  */
  
  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

   if((oldname == NULL)||(newname == NULL))
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_RENAME_REQ_MSG),
                       RFS_RENAME_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_rename_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* Initialise and set mutex first time */
  rvf_initialize_mutex (&mutex);
  rvf_lock_mutex (&mutex);

  /* compose message */
  msg_p->hdr.msg_id   = RFS_RENAME_REQ_MSG; 
  msg_p->oldname      = oldname;
  msg_p->newname      = newname;    
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();

  /* Send message */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  return result;
}

/**
 * function:  rfs_rename_nb
 */     
T_RFS_RET rfs_rename_nb(const T_WCHAR *oldname, 
                        const T_WCHAR  *newname, 
                        T_RV_RETURN return_path)
{
  T_RFS_RENAME_REQ_MSG  *msg_p;
  T_RFS_RET              pair_id;

  RFS_SEND_TRACE ("rfs_rename_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RV_NOT_READY;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

  if((oldname == NULL)||(newname == NULL))
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_RENAME_REQ_MSG),
                       RFS_RENAME_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_rename_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_RENAME_REQ_MSG; 
  msg_p->oldname      = oldname;
  msg_p->newname      = newname;    
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }
 
  return pair_id; 
}


/**
 * function:  rfs_set_label_nb
 */     
T_RFS_RET rfs_set_label_nb(const T_WCHAR *mpt_name,  const T_WCHAR *label, 
                        T_RV_RETURN return_path)
{
  T_RFS_SET_LABEL_REQ_MSG  *msg_p;
  T_RFS_RET              pair_id;

  RFS_SEND_TRACE ("rfs_set_label_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RV_NOT_READY;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

  if((mpt_name == NULL)||(label == NULL))
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_SET_LABEL_REQ_MSG),
                       RFS_SET_LABEL_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_set_label_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_SET_LABEL_REQ_MSG; 
  msg_p->mpt_name      = mpt_name;
  msg_p->label      = label;    
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }
 
  return pair_id; 
}


/**
 * function:  rfs_get_label_nb
 */     
T_RFS_RET rfs_get_label_nb(const T_WCHAR *mpt_name, T_WCHAR  *label, 
                        T_RV_RETURN return_path)
{
  T_RFS_GET_LABEL_REQ_MSG  *msg_p;
  T_RFS_RET              pair_id;

  RFS_SEND_TRACE ("rfs_get_label_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RV_NOT_READY;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

  if((mpt_name == NULL)||(label == NULL))
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_GET_LABEL_REQ_MSG),
                       RFS_GET_LABEL_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_get_label_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_GET_LABEL_REQ_MSG; 
  msg_p->mpt_name      = mpt_name;
  msg_p->label      = label;    
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }
 
  return pair_id; 
}

/**
 * function:  rfs_mkdir
 */     
T_RFS_RET rfs_mkdir(const T_WCHAR  *pathname, 
                    T_RFS_MODE  mode)
{
  T_RVF_MUTEX         mutex;
  T_RFS_RET           result;
  T_RFS_MKDIR_REQ_MSG *msg_p;

  RFS_SEND_TRACE ("RFS API: rfs_mkdir", RV_TRACE_LEVEL_WARNING);

  /*  Immediate Return: T_RFS_RET

  RFS_EOK Ok.
  RFS_EEXISTS Directory already exists.
  RFS_ENAMETOOLONG  Object's name is too long.
  RFS_EACCES  Search permission is denied for a component of the path prefix.
  RFS_EBADNAME  Name of the directory contains illegal characters
  RFS_ENOSPACE  Failed to allocate space for object's data.
  RFS_EFSFULL Failed to allocate an inode for the object.
  RFS_EDEVICE Device I/O error
  RFS_EMOUNT  Invalid mount point
  */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

 if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_MKDIR_REQ_MSG),
                       RFS_MKDIR_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_mkdir: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* Initialise and set mutex first time */
  rvf_initialize_mutex  (&mutex);
  rvf_lock_mutex        (&mutex);
  /* compose message */
  msg_p->hdr.msg_id   = RFS_MKDIR_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->mode         = mode;    
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();

  /* Send message */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex  (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  return result;
}

/**
 * function:  rfs_mkdir_nb
 */     

T_RFS_RET rfs_mkdir_nb(const T_WCHAR   *pathname, 
                       T_RFS_MODE   mode,
                       T_RV_RETURN  return_path)
{
  T_RFS_MKDIR_REQ_MSG  *msg_p;
  T_RFS_RET             pair_id;

  RFS_SEND_TRACE ("rfs_mkdir_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

 if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_MKDIR_REQ_MSG),
                       RFS_MKDIR_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_mkdir_nb: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_MKDIR_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->mode         = mode;    
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }
 
  return pair_id; 
}

/**
 * function:  rfs_rmdir
 */     
T_RFS_RET rfs_rmdir(const T_WCHAR *pathname)
{
  T_RVF_MUTEX         mutex;
  T_RFS_RET           result;
  T_RFS_RMDIR_REQ_MSG *msg_p;

  RFS_SEND_TRACE ("RFS API: rfs_rmdir", RV_TRACE_LEVEL_WARNING);

  /*  Immediate Return: T_RFS_RET

  RFS_EOK Ok.
  RFS_ENOENT  The named directory does not exist
  RFS_ENAMETOOLONG  Object's name is too long.
  RFS_ENOTEMPTY The named directory contains files other than '.' and '..' in it.
  RFS_EACCES  Search permission is denied for a component of the path prefix.
  RFS_EACCES  No write permission to delete the directory entry
  RFS_EBUSY The directory to be removed is the mount point for a mounted file system or the current directory.
  RFS_EBADNAME  Name of the directory contains illegal characters
  RFS_EMOUNT  Invalid mount point
  */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

 if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_RMDIR_REQ_MSG),
                       RFS_RMDIR_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_rmdir_nb: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* Initialise and set mutex first time */
  rvf_initialize_mutex  (&mutex);
  rvf_lock_mutex        (&mutex);
  /* compose message */
  msg_p->hdr.msg_id   = RFS_RMDIR_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();

  /* Send message */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex  (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  return result;
}

/**
 * function:  rfs_rmdir_nb
 */     
T_RFS_RET rfs_rmdir_nb(const T_WCHAR  *pathname,
                       T_RV_RETURN return_path)
{
  T_RFS_RMDIR_REQ_MSG  *msg_p;
  T_RFS_RET             pair_id;

  RFS_SEND_TRACE ("rfs_rmdir_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RFS_EAGAIN;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }


 if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_RMDIR_REQ_MSG),
                       RFS_RMDIR_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_rmdir_nb: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_RMDIR_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  return pair_id; 
}

/**
 * function:  rfs_opendir
 */     
T_RFS_SIZE rfs_opendir(const T_WCHAR *pathname,
                       T_RFS_DIR   *dirp)
{
  T_RVF_MUTEX           mutex;
  T_RFS_RET             result;
  T_RFS_OPENDIR_REQ_MSG *msg_p;
 const  T_WCHAR root_dir[3]={0x002F,0x0000,0x0000};

  RFS_SEND_TRACE ("RFS API: rfs_opendir", RV_TRACE_LEVEL_WARNING);

  /*  Immediate Return: T_RFS_DIR

  (Positive value)  pointer to the directory structure
  RFS_ENOENT  Directory not found.
  RFS_EACCES  Search permission is denied for a component of the path prefix.
  RFS_ENAMETOOLONG  Object's name is too long.
  RFS_EBADNAME  Name of the directory contains illegal characters
  RFS_ENOSPACE  Out of data space.
  RFS_EMOUNT  Invalid mount point
  */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  /* Check if pathname = NULL */
   if((pathname == NULL) || (dirp == NULL))
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}

  
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_OPENDIR_REQ_MSG),
                       RFS_OPENDIR_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_opendir: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* Initialise and set mutex first time */
  rvf_initialize_mutex  (&mutex);
  rvf_lock_mutex        (&mutex);
  /* compose message */
  msg_p->hdr.msg_id   = RFS_OPENDIR_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->dirp         = dirp;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();

  /* Send message */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex  (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);

 
  return result;
}

/**
 * function:  rfs_opendir_nb
 */     
T_RFS_RET rfs_opendir_nb(const T_WCHAR  *pathname,
                         T_RFS_DIR   *dirp,
                         T_RV_RETURN return_path)
{
  T_RFS_OPENDIR_REQ_MSG  *msg_p;
  T_RFS_RET              pair_id;
  T_RFS_PAIR_VALUE       pair_value;
  T_RFS_SIZE             ret_size;
  T_RFS_RET              func_result;  
  const  T_WCHAR root_dir[3]={0x002F,0x0000,0x0000};

  RFS_SEND_TRACE ("rfs_opendir_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RFS_EAGAIN;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

  /* Check if pathname = NULL */
 if((pathname == NULL) || (dirp == NULL))
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}

   
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_OPENDIR_REQ_MSG),
                       RFS_OPENDIR_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_opendir_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_OPENDIR_REQ_MSG; 
  msg_p->pathname     = pathname;
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  msg_p->dirp         = dirp;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }
 
  return pair_id;
}

/**
 * function:  rfs_readdir
 */     
T_RFS_SIZE rfs_readdir(T_RFS_DIR    *dirp, 
                       T_WCHAR   *buf, 
                       T_RFS_SIZE   size)
{
  T_RVF_MUTEX           mutex;
  T_RFS_RET             result;
  T_RFS_READDIR_REQ_MSG *msg_p;

  RFS_SEND_TRACE ("RFS API: rfs_readdir", RV_TRACE_LEVEL_WARNING);

  /* Immediate Return: T_RFS_SIZE

  (Positive value)  Number of bytes actually read. 
  RFS_EBADDIR Invalid directory descriptor
  RFS_ENOSPACE  Out of data space.
  */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

  if((buf == NULL) || (dirp == NULL))
  	{
		  /* The RFS is not able to handle this request at this moment */
		  RFS_SEND_TRACE ("RFS API: NULL input buffer ", RV_TRACE_LEVEL_ERROR);
		  return RFS_EINVALID;
  	  
  	}

 
  /* Initialise and set mutex first time */
  rvf_initialize_mutex  (&mutex);
  rvf_lock_mutex        (&mutex);

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_READDIR_REQ_MSG),
                       RFS_READDIR_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_readdir_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_READDIR_REQ_MSG; 
  msg_p->dirp         = dirp;
  msg_p->buf          =  buf;
  msg_p->size         = size; 
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();

  /* Send message */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex  (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  return result;
}

/**
 * function:  rfs_readdir_nb
 */     
T_RFS_RET rfs_readdir_nb(T_RFS_DIR    *dirp, 
                         T_WCHAR     *buf, 
                         T_RFS_SIZE   size,
                         T_RV_RETURN  return_path)
{
  T_RFS_READDIR_REQ_MSG  *msg_p;
  T_RFS_RET              pair_id;
  T_RFS_PAIR_VALUE       pair_value;
  T_RFS_SIZE             ret_size;
  T_RFS_RET              func_result;  

  RFS_SEND_TRACE ("rfs_readdir_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RFS_EAGAIN;
  }


 if((buf == NULL) || (dirp == NULL)||(size <= NULL))
  	{
		  /* The RFS is not able to handle this request at this moment */
		  RFS_SEND_TRACE ("RFS API: NULL input buffer ", RV_TRACE_LEVEL_ERROR);
		  return RFS_EINVALID;
  	  
  	}
  /* set all the contents of temp_buf as 0 */
  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

  
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_READDIR_REQ_MSG),
                       RFS_READDIR_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_readdir_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_READDIR_REQ_MSG; 
  msg_p->dirp         = dirp;
  msg_p->buf          = (T_WCHAR*)buf;
  msg_p->size         = size; 
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id; 

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }
 
  return pair_id; 
}

/**
 * function:  rfs_preformat
 */     
T_RFS_RET rfs_preformat(const T_WCHAR *pathname,
                        UINT16     magic)
{
  T_RVF_MUTEX             mutex;
  T_RFS_RET               result;
  T_RFS_PREFORMAT_REQ_MSG *msg_p;

  RFS_SEND_TRACE ("RFS API: rfs_preformat", RV_TRACE_LEVEL_WARNING);

  /*  Immediate Return: T_RFS_RET

  RFS_EOK Ok.
  RFS_EMAGIC  Magic number is incorrect.
  RFS_EINVALID  An erase operation is currently in progress. Retry the operation again later.
  RFS_ENODEVICE The flash device is unknown (not supported).
  RFS_EMEMORY Message allocation failed.
  RFS_EMSGSEND  Message sending failed.
  RFS_EDEVICE Device I/O error
  RFS_ENOTALLOWED Pre-formatting is not allowed (pathname is no mountpoint)
  */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

 if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  }

  /* Initialise and set mutex first time */
  rvf_initialize_mutex  (&mutex);
  rvf_lock_mutex        (&mutex);

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_PREFORMAT_REQ_MSG),
                       RFS_PREFORMAT_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_preformat_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_PREFORMAT_REQ_MSG; 
  msg_p->magic        = magic;
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->pathname     = pathname;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();

  /* Send message */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex  (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  return result;
}

/**
 * function:  rfs_preformat_nb
 */     

T_RFS_RET rfs_preformat_nb(const T_WCHAR   *pathname,
                           UINT16       magic,
                           T_RV_RETURN  return_path)
{
  T_RFS_PREFORMAT_REQ_MSG *msg_p;
  T_RFS_RET               pair_id;

  RFS_SEND_TRACE ("rfs_preformat_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RFS_EAGAIN;
  }

 if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_PREFORMAT_REQ_MSG),
                       RFS_PREFORMAT_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_preformat_nb: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_PREFORMAT_REQ_MSG; 
  msg_p->magic        = magic;
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  msg_p->pathname     = pathname;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }
 
  return pair_id; 
}

/**
 * function:  rfs_format
 */     
T_RFS_RET  rfs_format(const T_WCHAR *pathname,
                      const T_WCHAR *name, 
                      UINT16     magic)
{
  T_RVF_MUTEX           mutex;
  T_RFS_RET             result;
  T_RFS_FORMAT_REQ_MSG  *msg_p;

  RFS_SEND_TRACE ("RFS API: rfs_format", RV_TRACE_LEVEL_WARNING);

  /*  Immediate Return: T_RFS_RET

  RFS_EOK Ok.
  RFS_EAGAIN  Previous RFS_preformat() has not finished yet.
  RFS_EINVALID  Magic number is incorrect.
  RFS_EBADNAME  Name contains illegal characters
  RFS_EMEMORY Message allocation failed.
  RFS_EMSGSEND  Message sending failed.
  RFS_EDEVICE Device I/O error
  RFS_ENOTALLOWED Formatting is not allowed (pathname is no mountpoint)
  */

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

 if(pathname == NULL) 
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* Initialise and set mutex first time */
  rvf_initialize_mutex  (&mutex);
  rvf_lock_mutex        (&mutex);

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_FORMAT_REQ_MSG),
                       RFS_FORMAT_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_format: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_FORMAT_REQ_MSG; 
  msg_p->name         = name;
  msg_p->magic        = magic;
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  msg_p->pathname     = pathname;
  msg_p->pair_value.pair_id = rfs_get_free_pair_id ();

  /* Send message */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }

  //RFS_SEND_TRACE ("RFS API: wait till mutex is freed", RV_TRACE_LEVEL_WARNING);

  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex  (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  return result;
}

/**
 * function:  rfs_format_nb
 */     
T_RFS_RET rfs_format_nb(const T_WCHAR  *pathname,
                        const T_WCHAR  *name,
                        UINT16      magic,
                        T_RV_RETURN return_path)
{
  T_RFS_FORMAT_REQ_MSG  *msg_p;
  T_RFS_RET              pair_id;

  RFS_SEND_TRACE ("rfs_format_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);

    return RFS_EAGAIN;
  }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
  {
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: return_path is invalid", RV_TRACE_LEVEL_WARNING);

    return RFS_EINVALID;
  }

 if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_FORMAT_REQ_MSG),
                       RFS_FORMAT_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_format_nb: out of memory", RV_TRACE_LEVEL_WARNING);

    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id   = RFS_FORMAT_REQ_MSG; 
  msg_p->name         = name;
  msg_p->magic        = magic;
  msg_p->return_path  = return_path;
  msg_p->mutex        = NULL;
  msg_p->result       = NULL;
  msg_p->pathname     = pathname;
  pair_id             = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message mailbox */
  if (RV_OK != rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p)) 
  {
    rvf_free_buf (msg_p);
    return RFS_EMEMORY; 
  }
 
  return pair_id; 
}

T_RFS_RET rfs_getattribute(const T_WCHAR * pathname, T_FFS_ATTRIB_TYPE * attr)
{
  T_RFS_GETATTRIB_REQ_MSG   *msg_p;
  INT32 pair_id;
  T_RVF_MUTEX           mutex;
  T_RFS_RET             result;
  
  RFS_SEND_TRACE ("RFSFAT API: rfsfat_getattribute_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

 if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
 
 /* Initialise and set mutex first time */
  rvf_initialize_mutex  (&mutex);
  rvf_lock_mutex        (&mutex);

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_GETATTRIB_REQ_MSG),
                       RFS_GETATTRIB_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_format: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }
  
  /* compose message */
  msg_p->hdr.msg_id = RFS_GETATTRIB_REQ_MSG;
  msg_p->pathname 		= pathname;
   msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  pair_id = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfs_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  
  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex  (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);

  if((result >= 0) && (attr != NULL)) 
  	{
   (*attr) =(T_FFS_ATTRIB_TYPE)result; 
  	}
   
   return   (T_RFS_RET )result;
  
}


T_RFS_RET rfs_setattribute(const T_WCHAR * pathname, T_FFS_ATTRIB_TYPE  attr)
{
  T_RFS_SETATTRIB_REQ_MSG   *msg_p;
  INT32 pair_id;
  T_RVF_MUTEX           mutex;
  T_RFS_RET             result;
   

  RFS_SEND_TRACE ("RFSFAT API: rfsfat_setattribute_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

 if(pathname == NULL)
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
 /* Initialise and set mutex first time */
  rvf_initialize_mutex  (&mutex);
  rvf_lock_mutex        (&mutex);

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_SETATTRIB_REQ_MSG),
                       RFS_SETATTRIB_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_format: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }
  
  /* compose message */
  msg_p->hdr.msg_id = RFS_SETATTRIB_REQ_MSG;
  msg_p->pathname 		= pathname;
  msg_p->attr = attr;   /* attribute to set */
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  pair_id = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfs_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  
  /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex  (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  
  return result;
  
}

T_RFS_RET rfs_getdatetime(const T_WCHAR *pathname,T_RTC_DATE_TIME *date_time_p,T_RTC_DATE_TIME *crdate_time_p)
{

  T_RFS_GETDATETIME_REQ_MSG   *msg_p;
  INT32 pair_id;
  T_RVF_MUTEX           mutex;
  T_RFS_RET             result;
  
  RFS_SEND_TRACE ("RFSFAT API: rfsfat_getdatetime_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

 if((pathname == NULL)|| ((date_time_p == NULL) && (crdate_time_p == NULL)))
  	{
    /* return_path is invalid */
    RFS_SEND_TRACE ("RFS API: pathname is invalid", RV_TRACE_LEVEL_ERROR);

    return RFS_EINVALID;
  	
  	}
 /* Initialise and set mutex first time */
  rvf_initialize_mutex  (&mutex);
  rvf_lock_mutex        (&mutex);

 /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_GETDATETIME_REQ_MSG),
                       RFS_GETDATETIME_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_format: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
    msg_p->hdr.msg_id = RFS_GETDATETIME_REQ_MSG;
  msg_p->pathname 		= pathname;
  msg_p->date_time_p	= date_time_p;   /* last modified date and time */
  msg_p->crdate_time_p = crdate_time_p; /* Creation time and date */
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
   pair_id = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfs_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
   /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex  (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  
  return result;
}




T_RFS_RET rfs_unmount(T_RFS_FSTYPE fs_type)
{
   return rfs_mount_unmount(fs_type, RFS_UNMOUNT);

}


T_RFS_RET rfs_mount(T_RFS_FSTYPE fs_type)
{
    return rfs_mount_unmount(fs_type, RFS_MOUNT);
}



static T_RFS_RET rfs_mount_unmount(T_RFS_FSTYPE fs_type, int op)
{

  T_RFS_MOUNT_UNMOUNT_REQ_MSG   *msg_p;
  INT32 pair_id;
  T_RVF_MUTEX           mutex;
  T_RFS_RET             result;
  
  RFS_SEND_TRACE ("RFSFAT API: rfs_mount_unmount", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if(rfs_env_ctrl_blk_p->state != RFS_OPERATIONAL)
  {
    /* The RFS is not able to handle this request at this moment */
    RFS_SEND_TRACE ("RFS API: not ready", RV_TRACE_LEVEL_WARNING);
    return RFS_EAGAIN;
  }

 
 /* Initialise and set mutex first time */
  rvf_initialize_mutex  (&mutex);
  rvf_lock_mutex        (&mutex);

 /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id, 
                       sizeof (T_RFS_MOUNT_UNMOUNT_REQ_MSG),
                       RFS_MOUNT_UNMOUNT_REQ_MSG, 
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /* Insufficient memory to create message request */
    RFS_SEND_TRACE ("RFS rfs_mount_unmount: out of memory", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

  /* compose message */
  msg_p->hdr.msg_id = RFS_MOUNT_UNMOUNT_REQ_MSG;
  msg_p->op 		= op;
  msg_p->fs_type    = fs_type;
  msg_p->mutex        = &mutex;
  msg_p->result       = &result;
  pair_id = rfs_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfs_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
   /* Set mutex second time and wait for mutex to be freed */
  rvf_lock_mutex  (&mutex);

  //RFS_SEND_TRACE ("RFS API: mutex has been freed", RV_TRACE_LEVEL_WARNING);

  /* Delete the mutex */
  rvf_delete_mutex (&mutex);
  
  return result;
}



T_RFS_RET  rfs_send_message(T_RV_HDR * msg_p)
{
 if (rvf_send_msg(rfs_env_ctrl_blk_p->addr_id, msg_p) != RV_OK)
  {
    RFS_SEND_TRACE ("RFS API: could not send message", RV_TRACE_LEVEL_WARNING);
    return RFS_EMEMORY;
  }

 return RFS_OK;
} 



/**
 * static function:  rfs_search_fd_table_entry
 */ 
static  T_RFS_RET rfs_search_fd_table_entry(T_RFS_FD  fd, UINT8 *entry)
{
  UINT8       tmp_entry;
  BOOL        fd_found = FALSE;  

  /* Retrieve file descriptor information */
  for (tmp_entry = 0; 
       ((tmp_entry < RFS_MAX_NR_OPEN_FILES) && (fd_found == FALSE)); 
       tmp_entry++)
  {
    if (rfs_env_ctrl_blk_p->pair_value_map[tmp_entry].is_fd)
    {
      if (rfs_env_ctrl_blk_p->pair_value_map[tmp_entry].pair_value[0].fd == fd)
      {
        /* File descriptor is openend and information is available */
        fd_found    = TRUE;
        *entry      = tmp_entry;
      }
    }
  }
 
  if (fd_found == FALSE)
  {
    /* File descriptor is not openend */
    return RFS_EBADFD;
  }
  else
  {
    return RFS_EOK;
  }
}


static T_RFS_RET rfs_get_free_pair_id (void)
{
  T_RFS_RET pair_id;

  rfs_lock_mutex_pair_id ();
  pair_id = rfs_env_ctrl_blk_p->pair_id++;
  rfs_unlock_mutex_pair_id ();

  return pair_id;
}


                  


