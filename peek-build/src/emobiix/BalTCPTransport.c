#include "TCPTransport.h"
#include "p_malloc.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "bal_socket_api_ti.h"
#include "bal_os.h"
#include "msg.h"

#define AF_INET        0
#define SOCK_STREAM    0
#define IPPROTO_TCP    0

// XXX verify constants!
#define MSG_PEEK 0
#define EAGAIN 11

Endpoint *TCPSocket(void);
int TCPConnect(Endpoint *ep, URL *destination);
int TCPListen(Endpoint *ep, int backlog);
int TCPBind(Endpoint *ep, URL *bindaddr);
Endpoint *TCPAccept(Endpoint *ep);
int TCPWrite(Endpoint *ep, const void *buffer, size_t len);
int TCPRead(Endpoint *ep, void *output, size_t len);
int TCPPeek(Endpoint *ep, void *output, size_t len);
int TCPClose(Endpoint *ep);
void TCPFree(Endpoint *ep);
unsigned int TCPSequenceID(Endpoint *ep);

Transport TCPTransport = {TCPSocket, TCPConnect, TCPListen, TCPBind,
		TCPAccept, TCPWrite, TCPRead, TCPPeek, TCPClose, TCPFree,
		TCPSequenceID};

struct TCPEndpoint_t {
	Transport *transport;
	int fd;
	unsigned int sequenceID;
};
typedef struct TCPEndpoint_t TCPEndpoint;

static int TCPInit(void);

static int networkRequest_socket()
{
	BOSEventWaitT EvtStatus;
	uint32        MsgId;
	uint32        MsgSize;
	void          *MsgBufferP;

	emsMsg *socketMsg = (emsMsg *)BOSMsgBufferGet(sizeof(emsMsg));
	memset(socketMsg, 0, sizeof(emsMsg));

	socketMsg->msgA = 1;
	socketMsg->msgB = 2;

	emo_printf("ANDREY::Requesting socket from the network task...");
	
	// send a message to the network task
	BOSMsgSend(BOS_EM_S_ID, BOS_MAILBOX_1_ID, EM_S_SOCKET, (void *)socketMsg, sizeof(emsMsg));
	while (1)
	{
		// blocking wait for a reply from the network task
		EvtStatus = BOSEventWait(BOS_UI_ID, BOS_SIGNAL_FALSE, BOS_MESSAGE_TRUE, BOS_TIMEOUT_FALSE);

		emo_printf("ANDREY::Received socket response!");
		if (!(EvtStatus & BOS_MESSAGE_TYPE))
			continue;

		if (!BOSMsgRead(BOS_UI_ID, BOS_MAILBOX_1_ID, &MsgId, &MsgBufferP, &MsgSize))
			continue;

		socketMsg = (emsMsg *)MsgBufferP;
		emo_printf("ANDREY::Got a message id:%d, size:%d, socket: %d", MsgId, MsgSize, socketMsg->fd);

		return socketMsg->fd;
	}
}

static int networkRequest_connect(int fd, const char *host, int port)
{
	BOSEventWaitT EvtStatus;
	uint32        MsgId;
	uint32        MsgSize;
	void          *MsgBufferP;

	emsMsg *socketMsg = (emsMsg *)BOSMsgBufferGet(sizeof(emsMsg));
	memset(socketMsg, 0, sizeof(emsMsg));

	socketMsg->fd = fd;
	strcpy(socketMsg->host, host);
	socketMsg->port = port;

	emo_printf("ANDREY::Requesting connection from the network task...");
	
	// send a message to the network task
	BOSMsgSend(BOS_EM_S_ID, BOS_MAILBOX_1_ID, EM_S_CONNECT, (void *)socketMsg, sizeof(emsMsg));
	while (1)
	{
		// blocking wait for a reply from the network task
		EvtStatus = BOSEventWait(BOS_UI_ID, BOS_SIGNAL_FALSE, BOS_MESSAGE_TRUE, BOS_TIMEOUT_FALSE);

		emo_printf("ANDREY::Received connect response!");
		if (!(EvtStatus & BOS_MESSAGE_TYPE))
			continue;

		if (!BOSMsgRead(BOS_UI_ID, BOS_MAILBOX_1_ID, &MsgId, &MsgBufferP, &MsgSize))
			continue;

		socketMsg = (emsMsg *)MsgBufferP;
		emo_printf("ANDREY::Got a message id:%d, size:%d, status: %d", MsgId, MsgSize, socketMsg->msgA);

		return socketMsg->msgA;
	}
}

static int networkRequest_recv(int fd, char *buffer, int size)
{
	BOSEventWaitT EvtStatus;
	uint32        MsgId;
	uint32        MsgSize;
	void          *MsgBufferP;

	emsMsg *socketMsg = (emsMsg *)BOSMsgBufferGet(sizeof(emsMsg));
	memset(socketMsg, 0, sizeof(emsMsg));

	socketMsg->fd = fd;
	socketMsg->readSize = size;
	socketMsg->readBuffer = BOSMalloc(socketMsg->readSize);    

	emo_printf("ANDREY::recv(%d, %x, %d)", fd, buffer, size);
	
	// send a message to the network task
	BOSMsgSend(BOS_EM_S_ID, BOS_MAILBOX_1_ID, EM_S_RECV, (void *)socketMsg, sizeof(emsMsg));

	// block and wait for a read response
	while (1)
	{
		// blocking wait for a reply from the network task
		EvtStatus = BOSEventWait(BOS_UI_ID, BOS_SIGNAL_FALSE, BOS_MESSAGE_TRUE, BOS_TIMEOUT_FALSE);

		emo_printf("ANDREY::Received recv response!");
		if (!(EvtStatus & BOS_MESSAGE_TYPE))
			continue;

		if (!BOSMsgRead(BOS_UI_ID, BOS_MAILBOX_1_ID, &MsgId, &MsgBufferP, &MsgSize))
			continue;

		socketMsg = (emsMsg *)MsgBufferP;
		emo_printf("ANDREY::recv(%d, %x, %d) = %d", fd, buffer, size, socketMsg->readSize);

		if (socketMsg->readSize > 0)
			memcpy(buffer, socketMsg->readBuffer, socketMsg->readSize);

		BOSFree(socketMsg->readBuffer);

		return socketMsg->readSize;
	}
}

static int networkRequest_peek(int fd, char *buffer, int size)
{
	BOSEventWaitT EvtStatus;
	uint32        MsgId;
	uint32        MsgSize;
	void          *MsgBufferP;

	emsMsg *socketMsg = (emsMsg *)BOSMsgBufferGet(sizeof(emsMsg));
	memset(socketMsg, 0, sizeof(emsMsg));

	socketMsg->fd = fd;
	socketMsg->readSize = size;
	socketMsg->readBuffer = BOSMalloc(socketMsg->readSize);    

	emo_printf("ANDREY::Requesting a peek from the network task...");
	
	// send a message to the network task
	BOSMsgSend(BOS_EM_S_ID, BOS_MAILBOX_1_ID, EM_S_PEEK, (void *)socketMsg, sizeof(emsMsg));

	// block and wait for a read response
	while (1)
	{
		// blocking wait for a reply from the network task
		EvtStatus = BOSEventWait(BOS_UI_ID, BOS_SIGNAL_FALSE, BOS_MESSAGE_TRUE, BOS_TIMEOUT_FALSE);

		emo_printf("ANDREY::Received peek response!");
		if (!(EvtStatus & BOS_MESSAGE_TYPE))
			continue;

		if (!BOSMsgRead(BOS_UI_ID, BOS_MAILBOX_1_ID, &MsgId, &MsgBufferP, &MsgSize))
			continue;

		socketMsg = (emsMsg *)MsgBufferP;
		emo_printf("ANDREY::Got a message id:%d, size:%d, bytes: %d", MsgId, MsgSize, socketMsg->readSize);

		if (socketMsg->readSize > 0)
			memcpy(buffer, socketMsg->readBuffer, socketMsg->readSize);

		BOSFree(socketMsg->readBuffer);

		return socketMsg->readSize;
	}
}

static int networkRequest_send(int fd, const char *buffer, int size)
{
	BOSEventWaitT EvtStatus;
	uint32        MsgId;
	uint32        MsgSize;
	void          *MsgBufferP;

	emsMsg *socketMsg = (emsMsg *)BOSMsgBufferGet(sizeof(emsMsg));
	memset(socketMsg, 0, sizeof(emsMsg));

	socketMsg->fd = fd;
	socketMsg->writeSize = size;
	socketMsg->writeBuffer = BOSMalloc(size);
	memcpy(socketMsg->writeBuffer, buffer, size);

	emo_printf("ANDREY::Requesting a send from the network task...");
	
	// send a message to the network task
	BOSMsgSend(BOS_EM_S_ID, BOS_MAILBOX_1_ID, EM_S_SEND, (void *)socketMsg, sizeof(emsMsg));

	// block and wait for a read response
	while (1)
	{
		// blocking wait for a reply from the network task
		EvtStatus = BOSEventWait(BOS_UI_ID, BOS_SIGNAL_FALSE, BOS_MESSAGE_TRUE, BOS_TIMEOUT_FALSE);

		emo_printf("ANDREY::Received send response!");
		if (!(EvtStatus & BOS_MESSAGE_TYPE))
			continue;

		if (!BOSMsgRead(BOS_UI_ID, BOS_MAILBOX_1_ID, &MsgId, &MsgBufferP, &MsgSize))
			continue;

		socketMsg = (emsMsg *)MsgBufferP;
		emo_printf("ANDREY::Got a send message id:%d, size:%d, status: %d", MsgId, MsgSize, socketMsg->writeSize);

		BOSFree(socketMsg->writeBuffer);
		return socketMsg->writeSize;
	}
}

static int networkRequest_close(int fd)
{
	BOSEventWaitT EvtStatus;
	uint32        MsgId;
	uint32        MsgSize;
	void          *MsgBufferP;

	emsMsg *socketMsg = (emsMsg *)BOSMsgBufferGet(sizeof(emsMsg));
	memset(socketMsg, 0, sizeof(emsMsg));

	socketMsg->fd = fd;

	emo_printf("ANDREY::Requesting close from the network task...");
	
	// send a message to the network task
	BOSMsgSend(BOS_EM_S_ID, BOS_MAILBOX_1_ID, EM_S_CLOSE, (void *)socketMsg, sizeof(emsMsg));
	while (1)
	{
		// blocking wait for a reply from the network task
		EvtStatus = BOSEventWait(BOS_UI_ID, BOS_SIGNAL_FALSE, BOS_MESSAGE_TRUE, BOS_TIMEOUT_FALSE);

		emo_printf("ANDREY::Received close response!");
		if (!(EvtStatus & BOS_MESSAGE_TYPE))
			continue;

		if (!BOSMsgRead(BOS_UI_ID, BOS_MAILBOX_1_ID, &MsgId, &MsgBufferP, &MsgSize))
			continue;

		socketMsg = (emsMsg *)MsgBufferP;
		emo_printf("ANDREY::Got a message id:%d, size:%d, status: %d", MsgId, MsgSize, socketMsg->msgA);

		return socketMsg->msgA;
	}
}


Endpoint *TCPSocket(void)
{
	TCPEndpoint *ep;
	int on;

	TCPInit();

	ep = (TCPEndpoint *)p_malloc(sizeof(TCPEndpoint));
	ep->fd = networkRequest_socket();
	ep->transport = (Transport *)&TCPTransport;

	if (ep->fd == -1) {
		p_free(ep);
		return NULL;
	}

	return (Endpoint *)ep;
}

int TCPConnect(Endpoint *iep, URL *destination)
{
	TCPEndpoint *ep;
	unsigned short port;
	struct in_addr addr;
	struct sockaddr_in connect_in;
	int on;

	if (destination == NULL || destination->port == NULL || destination->hostname == NULL)
		return -1;

	ep = (TCPEndpoint *)iep;
	port = atoi(destination->port);

	if (networkRequest_connect(ep->fd, destination->hostname, port) == -1)
	{
		TCPClose((Endpoint *)ep);
		p_free(ep);
    return -1;
	}

	ep->sequenceID = 1;

	return 0;
}

int TCPListen(Endpoint *ep, int backlog)
{
	return -1;
}

int TCPBind(Endpoint *ep, URL *bindaddr)
{
	return 0;
}

Endpoint *TCPAccept(Endpoint *ep)
{
	return NULL;
}

int TCPWrite(Endpoint *ep, const void *buffer, size_t len)
{
	TCPEndpoint *tep;

	if (ep == NULL || buffer == NULL)
		return -1;

	if (len == 0)
		return 0;

	tep = (TCPEndpoint *)ep;
	return networkRequest_send(tep->fd, buffer, len);
}

int TCPRead(Endpoint *ep, void *output, size_t len)
{
	TCPEndpoint *tep;

	if (ep == NULL || output == NULL)
		return -1;

	if (len == 0)
		return 0;

	tep = (TCPEndpoint *)ep;
	return networkRequest_recv(tep->fd, output, len);
}

int TCPPeek(Endpoint *ep, void *output, size_t len)
{
	return networkRequest_peek(((TCPEndpoint *)ep)->fd, output, len);
}

int TCPClose(Endpoint *ep)
{
    TCPEndpoint *tep;

    if (ep == NULL)
        return -1;

    tep = (TCPEndpoint *)ep;
    return networkRequest_close(tep->fd); 
}

void TCPFree(Endpoint *ep)
{
    p_free(ep);
}

unsigned int TCPSequenceID(Endpoint *ep)
{
	TCPEndpoint *tep;
	tep = (TCPEndpoint *)ep;
	tep->sequenceID += 2;
	return tep->sequenceID - 2;
}

static int TCPInit(void)
{
	return 1;
}
