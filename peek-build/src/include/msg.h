#ifndef __MSG_H__
#define __MSG_H__

typedef struct _emsMsg
{
  int msgA;
  int msgB;
	
	int fd;
	char host[256];
	int port;

	int readSize;
	char *readBuffer;

	int writeSize;
	char *writeBuffer;

} emsMsg;

typedef enum
{
  EM_S_SCHEDULER_INIT_APPT,
  EM_S_SCHEDULER_PREPARE_APPT,

	EM_S_SOCKET,
	EM_S_CONNECT,
	EM_S_PEEK,
	EM_S_RECV,
	EM_S_SEND,
	EM_S_CLOSE

} EMSMsgIdT;

#endif // __MSG_H__
