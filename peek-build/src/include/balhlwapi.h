#ifndef BALHLW_H
#define BALHLW_H


#include "sysdefs.h"
#include "balapi.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NET_INADDR_ANY        0x0L           
#define NET_MAX_SOCKS         8
#define BAL_NET_MAX_REG_TASK  5
 #define NET_MAX_LISTEN_SOCK   5
 #define NET_LINGER_TIMER      120 /*linger at most 2m*/
 #define BAL_INADDR_LOOPBACK 0x7F000001UL

typedef enum 
{
  NET_IP_PROTOCOL   = 0,
  NET_ICMP_PROTOCOL = 1,
  NET_TCP_PROTOCOL  = 6,     
  NET_UDP_PROTOCOL  = 17
} NetProtocolT;

typedef enum 
{
  NET_SOCK_STREAM_TYPE,
  NET_SOCK_DGRAM_TYPE,
  SOCK_ICMP_TYPE
} NetTypeT;    

typedef enum 
{
  NET_AF_INET_DOMAIN
} NetDomainT;

typedef enum 
{
  NET_WRITE_EVENT           = 0x0001,     /*socket ready to write */
  NET_READ_EVENT            = 0x0002,      /*socket ready to read */
  NET_CLOSE_EVENT           = 0x0004,     /*socket close successfully */
  NET_ACCEPT_EVENT          = 0x0008,    /*socket ready to accept connection request */
  NET_CONNECT_EVENT         = 0x0010,   /*socket connect successfully */
  NET_CREATE_EVENT          = 0x0020,    /*socket created successfully */
  NET_BIND_EVENT            = 0x0040,       /*socket binded successfully*/
  NET_LISTEN_EVENT          = 0x0080,
  NET_BIND_FAIL_EVENT       = 0x0100,
  NET_SHUTDOWN_FAIL_EVENT   = 0x0200,  /* shut down failed*/
  NET_SHUTDOWN_SUCESS_EVENT = 0x0400,
  NET_OPT_SET_FAIL_EVENT    = 0x0800,
  NET_OPT_SET_SUCESS_EVENT  = 0x1000,
  NET_LISTEN_FAIL_EVENT     = 0x2000, 
  NET_CONNECT_FAIL_EVENT    = 0x4000,
  NET_NUM_EVENT
} NetEvenTypeT;

typedef struct 
{
  uint16 SinFamily;
  uint16 SinPort;
  uint32 Address;
  char   SizeZero[8];
} NetSockAddrT;

typedef enum
{
  SOCKET_NULL_STATE,                  
  SOCKET_OPENING_STATE,  
  SOCKET_OPEN_STATE, 
  SOCKET_CLOSING_STATE,
  SOCKET_WAITFOR_CLOSE_STATE,  
  SOCKET_CLOSED_STATE
} SockStateT;

 typedef enum
 {
   SOCKET_OP_LISTEN     = 0x0001,
   SOCKET_OP_LINGER     = 0x0002,
   SOCKET_OP_KEEPALIVE  = 0x0004,
   SOCKET_OP_ACCEPTCONN = 0x0008,
   SOCKET_OP_NALGE      = 0x0010,
   SOCKET_OP_RCVSIZE   =  0x0020,
   SOCKET_OP_SNDSIZE  =  0x00040,
   All_SOCKET_OP
 }SocketOptionT;

 typedef enum
 {
   SOCKET_SEND_NO_DELAY = 0x01,
   SOCKET_SEND_IDLE     = 0x02
 }SocketSendTypeT;

 typedef enum
 {
   SOCK_SD_NONE = 0x00,
   SOCK_SD_RECEIVE = 0x01,
   SOCK_SD_SEND = 0x02,
   SOCK_SD_BOTH = 0x03
}SockSDStateT;

 typedef enum
 {
   SOCKET_FP_TOS =1,                      /* set IP type-of-service          */
   SOCKET_FP_TTL =2,                      /* set IP datagram time-to-live    */
   SOCKET_FP_FLAGS =3,                    /* set or clear binary option      */
   SOCKET_FP_RECVBUF =4,                  /* set receive buffer size         */
   SOCKET_FP_MAXSEG = 5,
   SOCKET_FP_MAXRXTSHIFT =6,              /* set maximum frame re-transmission */
   SOCKET_FP_KEEPALIVE=7
 }SocketOpFlagT;

typedef enum
{
  NET_SUCCESS,   /* The operation was a success. */
  NET_EBADF,     /* Bad file number */
  NET_EFAULT,    /*  Bad address     */
  NET_EWOULDBLOCK,   /*    Operation would block */
  NET_EAFNOSUPPORT,  /*  Address family not supported by protocol. */
  NET_EPROTOTYPE,    /*  Protocol wrong type for socket */
  NET_ESOCKNOSUPPORT,/*  Socket type not supported. */
  NET_EPROTONOSUPPORT,/* Protocol not supported */
  NET_EMFILE,         /* Too many open files. */
  NET_EOPNOTSUPP,     /* Operation not supported on transport endpoint. */
  NET_EADDRINUSE,     /* Address already in use. */
  NET_EADDRREQ,       /* Destination address required */
  NET_EINPROGRESS,    /* Operation now in progress */
  NET_ESHUTDOWN,    /* Socket was closed */
  NET_EISCONN,         /* 	Transport endpoint is already connected. */
  NET_EIPADDRCHANGED,  /* Remote address changed. */
  NET_ENOTCONN,       	/* Transport endpoint is not connected. */
  NET_ECONNREFUSED,   	/* Connection refused. */ 
  NET_ETIMEDOUT,      	/* Connection timed out. */
  NET_ECONNRESET,     	/* Connection reset by peer. */
  NET_ECONNABORTED,   	/* Software caused connection abort. */
  NET_ENETDOWN,       	/* Network is down. */
  NET_EPIPE,          	/* Broken pipe. */
  NET_EMAPP,          	/* No mapping found. */
  NET_EBADAPP,        	/* RegId invalid. */
  NET_ESOCKEXIST,      	/* The socket doesn't exist. */
  NET_EINVAL,         	/* Invalid argument. */
  NET_EMSGSIZE,       	/* Message too long. */
  NET_EEOF,           	/* End of file reached. */
  NET_EHOSTNOTFOUND,  	/* The host wasn't found. */
  NET_ETRYAGAIN,      	/* Try again. */
  NET_ENORECOVERY,    	/* Can't recover from error. */
  NET_ENOADDRESS,     	/* No address given. */
  NET_ENETEXIST
} NetResultT;

typedef enum 
{
  NET_RLP_DEFAULT_SETTINGS,
  NET_RLP_CURRENT_SETTINGS,
  NET_RLP_NEGOTIATED_SETTINGS
} NetRlpSettingsTypeT;

typedef struct 
{
	uint8 FwdNakRounds;
	uint8 FwdNaksPerRounds[7];
	uint8 RevNakRounds;
	uint8 RevNaksPerRounds[7];
} NetRlpSettingsT;

/*This data type is the PPP connection status.
 * NET_ISCONN_STATUS	            -	PPP connection is established and available.
 * NET_INPROGRESS_STATUS,   	  -	PPP connection in progress.
 * NET_NONET_STATUS,	            -	PPP connection disconnected.
 * NET_CLOSEINPROGRESS_STATUS 	  -	PPP connection is closing.
 */
typedef enum 
{
  NET_ISCONN_STATUS,
  NET_INPROGRESS_STATUS,
  NET_NONET_STATUS,
  NET_CLOSEINPROGRESS_STATUS,
  NET_DORMINPROGRESS_STATUS,
  NET_INDORMANCY_STATUS,
  NET_WAKINGUP_STATUS
} NetStatusT;

typedef enum 
{
  NET_EVT_SOCKET,
  NET_EVT_NET,
  NET_EVT_DNS_LOOKUP,
  NET_EVT_MAX
} BalNetEventIdT;

typedef struct 
{
  uint16 SockFd;
  uint32 EventMask;
} NetSocketMsgT;

typedef struct 
{
  RegIdT     RegId;
  NetStatusT Status;
} NetEventMsgT;

typedef struct 
{
  char*        NameP;
  NetSockAddrT Addr;
} NetDnsLookupMsgT;

/*some test message body - hyang*/
typedef struct 
{
  RegIdT regid;
} NetTestRegIdT;

typedef struct 
{
  int16 sockfd;
} NetTestSockFdT;

/*valnet call back prototype*/
typedef void (*BalNetEventFunc)( RegIdT         RegId,
                                 BalNetEventIdT EventId,
                                 void*          EventMsgP );

#define NET_SOCKUSABLE 0
#define NULL_REGID -1
#define UDP_MAX_PAYLOAD_LEN  1472
#define MAX_IP_DATAGRAM_LEN 1536
#define SOCKFD_BASE 100


typedef struct 
{
  uint32        evtExpected;  
  int16         sockfd;           
  RegIdT        regid;
  uint8         sap;			
  NetDomainT    family;            
  NetTypeT      type;               
  NetProtocolT  protocol;         
  bool          bDataReady;                
  bool          bDataSendReady;
  bool          bSapValid;
  bool          bBinded;
  SockStateT    SockState;
  HlwAppAddrT   localAddr;            
  HlwAppAddrT   destAddr;             
  unsigned char bufRecvData[MAX_IP_DATAGRAM_LEN];
  int           lenRecvData;
   int           maxLenRecvData;
   int           lenBytesToBeRead;
  unsigned char bufSendData[MAX_IP_DATAGRAM_LEN];
  int           lenSendData;
   int           maxLenSendData;
   uint16        socketOpt;
   int16         sockListenfd;
   bool          bAppUsed;
   uint32        lingerTime;
   SockSDStateT  sdState;
} SockCtrlBlkT;

/* ETS Messages
 */
typedef PACKED struct
{
  uint8 result;
} BalNetResultRspT;

typedef PACKED struct
{
  uint8 status;
} BalNetStatusRspT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  int16      sockfd;
  uint32     evtMask;
} BalNetEventSelectMsgT;

typedef PACKED struct
{
  ExeRspMsgT  RspInfo;
  uint16      RegId;
  uint8       domain;   /* NetDomainT   */
  uint8       type;     /* NetTypeT     */
  uint8       protocol; /* NetProtocolT */
} BalNetSocketCreateMsgT;

typedef PACKED struct
{
  uint8  result;
  uint16 sockfd;
} BalNetSocketCreateRspT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  int16      sockfd;
  uint16     SinFamily;
  uint16     SinPort;
  uint32     Address;
  char       SizeZero[8];
  int16      namelen;
} BalNetConnectMsgT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  int16      sockfd;
} BalNetCloseMsgT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  int16      sockfd;
} BalNetGetSockStatusMsgT;

typedef PACKED struct
{
  SockStateT sockState;
} BalNetGetSockStatusRspT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  int16      sockfd;
  int32      nbytes;
  uint8*     buffer;
} BalNetRecvMsgT;

typedef PACKED struct
{
  uint8 result;
  int16 numBytesRead;
} BalNetRecvRspT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  int16      sockfd;
  int32      nbytes;
  uint8*     buffer;
} BalNetSendMsgT;

typedef PACKED struct
{
  uint8 result;
  int16 numBytesSend;
} BalNetSendRspT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  int16      sockfd;
  uint16     SinFamily;
  uint16     SinPort;
  uint32     Address;
  char       SizeZero[8];
  int16      namelen;
} BalNetBindMsgT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  int16      sockfd;
  int16      backlog;
} BalNetListenMsgT;

typedef PACKED struct
{
  ExeRspMsgT RspInfo;
  int16      sockfd;
  uint16     SinFamily;
  uint16     SinPort;
  uint32     Address;
  char       SizeZero[8];
} BalNetAcceptMsgT;

typedef PACKED struct
{
  uint8  result;
  int16  addrlen;
  int16  newsock;
} BalNetAcceptRspT;

typedef PACKED struct
{
  int16  sockfd;
  int    nbytes;
  uint8* buffer;
  uint16 SinFamily;
  uint16 SinPort;
  uint32 Address;
  uint8  SizeZero[8];
  int    tolen;
} BalNetSendToMsgT;

typedef PACKED struct
{
  int16  sockfd;
  int    nbytes;
  uint8* buffer;
  uint16 SinFamily;
  uint16 SinPort;
  uint32 Address;
  uint8  SizeZero[8];
  int    tolen;
} BalNetRecvFromMsgT;

typedef PACKED struct
{
  uint16 RegId;
  char*  HostName;
  uint32 IpAddrP;
} BalNetGetHostByNameMsgT;

typedef PACKED struct
{
  char* cp;
} BalNetInetAddrMsgT;

typedef PACKED struct
{
  int   LenP;
  char* AuthStrP;
} BalNetGetPPPAuthParmsMsgT;

typedef PACKED struct
{
  char* AuthStrP;
} BalNetSetPPPAuthParmsMsgT;

typedef PACKED struct
{
  int   LenP;
  char* DialStrP;
} BalNetGetDialStrParmsMsgT;

typedef PACKED struct
{
  char* DialStrP;
} BalNetSetDialStrParmsMsgT;


extern SockCtrlBlkT SockCtrlBlkTable[NET_MAX_SOCKS];       

extern SockCtrlBlkT* GetAvailableSockCtrlBlk( void );
extern void          FreeSockCtrlBlk(SockCtrlBlkT* pSCB);
extern SockCtrlBlkT* GetSockCtrlBlkBySockfd( int16 sockfd);
extern void          NetworkEventNotify(RegIdT RegId,NetStatusT NetStatus);
extern void          SocketEventNotify(int16 SockFd, uint32 EventMask);
extern uint32        GetSocketEventMask( SockCtrlBlkT* pSCB,
                                         uint32        evtExpected );
extern bool          CanClosePPPSession( void );
 extern SockCtrlBlkT *GetAcceptSockCtrlBlkfd(int16 sockfd, int16 *clinsockfd, int8 *acceptBlkNum);

/* Message handler */

void BalPppConnStatusMsg( void* MsgDataP );
void BalPppCloseStatusMsg( void* MsgDataP );
void BalSocketCreateStatusMsg( void* MsgDataP );
void BalSocketBindStatusMsg( void* MsgDataP );
void BalSocketConnStatusMsg( void* MsgDataP );
void BalSocketCloseStatusMsg( void* MsgDataP );
void BalTcpbDataRecvMsg( void* MsgDataP );
void BalSocketInactTmoMsg(void *MsgData);
void BalTcpbDataSendRspMsg( void* MsgDataP );
void ValUpbRecvDataMsg( void* MsgDataP );
void ValUdpbSentDataRspMsg( void* MsgDataP );
 void BalSocketListenStatusMsg(void* MsgDataP);
 void BalSocketOptStatusMsg(void *MsgDataP);
 void BalSocketShutdownStatusMsg(void *MsgDataP);
 void BalSocketLingerStatusMsg(void *MsgDataP);
 
 #ifdef __cplusplus
 extern "C" {
 #endif
/*===========================================================================
FUNCTION BalNetInit()

DESCRIPTION

DEPENDENCIES
  None.
  
PARAMETERS:

RETURN VALUE
===========================================================================*/
void BalNetInit( void );

/***************************************************************************

  FUNCTION NAME: BalNetRegister

  DESCRIPTION:
    This function registers a callback for all network services related events.

  PARAMETERS:
    NetCallback - The callback function to be called for the notification of network 
                  services events.
    
  RETURNED VALUES:

    The assigned registration ID of the subscriber.

*****************************************************************************/
RegIdT BalNetRegister( BalNetEventFunc NetCallback );

/***************************************************************************

  FUNCTION NAME: BalNetUnregister

  DESCRIPTION:
    This function unregisters the given client so that it won't receive additional
    network services related events.

  PARAMETERS:
    RegId - The assigned registration ID of the subscriber.
  
  RETURNED VALUES:
    void

*****************************************************************************/
void BalNetUnregister( RegIdT RegId );

/***************************************************************************

  FUNCTION NAME: NetPppOpen

  DESCRIPTION:
    This function opens a new PPP connection

  PARAMETERS:
    RegId - The assigned registration ID of the subscriber.
  
  RETURNED VALUES:
    A success or failure code from NetResultT

*****************************************************************************/
NetResultT BalNetPppOpen( RegIdT RegId );

/***************************************************************************

  FUNCTION NAME: BalNetPppClose

  DESCRIPTION:
    This function closes a PPP connection

  PARAMETERS:
    RegId - The assigned registration ID of the subscriber.
  
  RETURNED VALUES:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetPppClose( RegIdT RegId ); 

/***************************************************************************

  FUNCTION NAME: BalNetPppDormantReq

  DESCRIPTION:
    This function request the PPP connection to enter dormancy state.

  PARAMETERS:
    RegId - The assigned registration ID of the subscriber.
  
  RETURNED VALUES:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetPppDormantReq( RegIdT RegId );

/***************************************************************************

  FUNCTION NAME: BalNetGetPppStatus

  DESCRIPTION:
    This function get the PPP connection status.

  PARAMETERS:
    RegId - The assigned registration ID of the subscriber.
 
  RETURNED VALUES:


*****************************************************************************/
NetStatusT BalNetGetPppStatus( RegIdT RegId );

/***************************************************************************

  FUNCTION NAME: BalNetEventSelect

  DESCRIPTION:
    This function registers the client to receive the events as indicated by the event mask.
    
  PARAMETERS:
    SockFd    - The file decriptor ID of the socket.
    EventMask - The event mask of the events that have occurred.
  
  RETURNED VALUES:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetEventSelect( int16  sockfd,
                              uint32 evtMask );

/***************************************************************************

  FUNCTION NAME: BalNetSocket

  DESCRIPTION:
    This function opens a socket of the specified protocol type
    
  PARAMETERS:
    RegId    - The assigned registration ID of the subscriber.
    Domain   - The protocol family to be used.
    Type     - Stream, datagram etc.
    Protocol - UDP, TCP, etc.
    SockFd   - [out] The new file descriptor for the socket.
  
  RETURNED VALUES:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetSocket( RegIdT       RegId,
                         NetDomainT   af,
                         NetTypeT     type,
                         NetProtocolT protocol,
                         int16*       SockFd );

/***************************************************************************

  FUNCTION NAME: BalNetConnect

  DESCRIPTION:
    This function makes a connection the given network node
    
  PARAMETERS:
    SockFd    - The file decriptor ID of the socket.
    ServAddrP - The address and port of the server.
    AddrLen   - The length of the server address structure.
  
  RETURNED VALUES:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetConnect( int16               sockfd,
                          const NetSockAddrT* name,
                          int16               namelen );

/***************************************************************************

  FUNCTION NAME: BalNetClose

  DESCRIPTION:
    This function closes the given socket.
    
  PARAMETERS:
    SockFd	-	The file decriptor ID of the socket.
  
  RETURNED VALUES:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetClose( int16 sockfd );

/***************************************************************************

  FUNCTION NAME: NetGetSockStatus

  DESCRIPTION:

    This function get the status of  the given socket.
    
  PARAMETERS:

    SockFd	-	The file decriptor ID of the socket.
  
  RETURNED VALUES:

   .

*****************************************************************************/
SockStateT BalNetGetSockStatus( int16 sockfd );

/***************************************************************************

  FUNCTION NAME: BalNetRecv

  DESCRIPTION:

    This function receives bytes from a socket into the given buffer
    
  PARAMETERS:

    sockfd        - The file decriptor ID of the socket.
    buffer        - The buffer where incoming data is copied to.
    nbytes        - The length of BufferP.
    NumBytesReadP - [out] The number of bytes copied into BufferP

  
  RETURNED VALUES:

    A success or failure code from NetResultT

*****************************************************************************/
NetResultT BalNetRecv( int16  sockfd,
                       char*  buffer,
                       int    nbytes,
                       int16* NumBytesReadP );

/***************************************************************************

  FUNCTION NAME: BalNetSend

  DESCRIPTION:

    This function sends bytes through the opened socket.
    
  PARAMETERS:

    sockfd        - The file decriptor ID of the socket.
    buffer        - The buffer where incoming data is copied to.
    nbytes        - The length of BufferP.
    NumBytesReadP - [out] The number of bytes copied into BufferP

  
  RETURNED VALUES:

    A success or failure code from NetResultT

*****************************************************************************/
NetResultT BalNetSend( int16  sockfd,
                       char*  buffer,
                       int    nbytes,
                       int16* NumBytesSendP );

/***************************************************************************

  FUNCTION NAME: BalNetBind

  DESCRIPTION:

    This function binds the given socket to a given network node
    
  PARAMETERS:

    sockfd   - The file decriptor ID of the socket.
    name     - The address and port of the server.
    namelen  - The length of the server address structure.
  
  RETURNED VALUES:

    A success or failure code from NetResultT

*****************************************************************************/
NetResultT BalNetBind( int16               sockfd,
                       const NetSockAddrT* name,
                       int16               namelen );

/***************************************************************************

  FUNCTION NAME: BalNetListen

  DESCRIPTION:
    This function registers the client for new connection events associated with the socket.

  PARAMETERS:
    sockfd  - The file decriptor ID of the socket.
    backlog - The maximum number of pending connections allowed.

  RETURN VALUE:
	A success or failure code from NetResultT.


*****************************************************************************/
NetResultT BalNetListen( int16 sockfd,
                         int16 backlog );

/***************************************************************************

  FUNCTION NAME: BalNetAccept

  DESCRIPTION:
    This function accepts a new connection from another network node.

  PARAMETERS:
    sockfd  - The file decriptor ID of the socket.
    addr    - The address and port of the server. This is an output parameter, so 
              it doesn't have to be initialized.
    addrlen - The length of the server address structure. This is an output
              parameter, so it doesn't have to be initialized.
    newsock - Pointer to the new connetion socket when a new connection is 
              sucessfully established. This is an output parameter, so it
              doesn't have to be initialized.

  RETURN VALUE:
	A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetAccept( int16         sockfd,
                         NetSockAddrT* addr,
                         int16*        addrlen,
                         int16*        newsock );

/***************************************************************************

  FUNCTION NAME: BalNetSendTo

  DESCRIPTION:
    This function sends a datagram to the specified network node

  PARAMETERS:
    sockfd        - The file decriptor ID of the socket.
    buffer        - The buffer where outgoing data is copied from.
    nbytes        - The length of the data ready to be written from BufferP.
    toAddr        - The address the datagram should be sent to.
    tolen         - The length of the address in ToAddrP.
    NumBytesSendP - The number of bytes written to the socket. This is an output 
                     parameter, so it doesn't have to be initialized.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetSendTo( int16         sockfd, 
                         char*         buffer, 
                         int           nbytes, 
                         NetSockAddrT* toAddr, 
                         int           tolen, 
                         int16*        NumBytesSendP );

/***************************************************************************

  FUNCTION NAME: BalNetRecvFrom

  DESCRIPTION:
    This function receives a datagram from the specified network node.

  PARAMETERS:
    sockfd        - The file decriptor ID of the socket.
    buffer        - The buffer where incoming data is copied to.
    nbytes        - The length of the buffer in BufferP.
    from          - The address the datagram should be recieved from.
    fromlen       - point to the length of the receive socket address. This is an 
                     output parameter, so it doesn't have to be initialized.
    NumBytesReadP - The number of bytes receiving from the socket. This is an 
                     output parameter, so it doesn't have to be initialized.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetRecvFrom( int           sockfd, 
                           char*         buffer, 
                           int           nbytes, 
                           NetSockAddrT* from, 
                           int*          fromlen, 
                           int16*        NumBytesReadP );

/***************************************************************************

  FUNCTION NAME: BalNetGetHostByName

  DESCRIPTION
    This function converts a host name to an IP address.

  PARAMETERS
    RegId    - The assigned registration ID of the subscriber.
    HostName - The name of the host.
    IPAddrP  - The quad IP address number. This is an output parameter, so it 
                doesn't have to be initialized.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetGetHostByName( RegIdT  RegId,
                                char*   HostName,
                                uint32* IPAddrP );

/***************************************************************************

  FUNCTION NAME: BalNetInetAddr

  DESCRIPTION
    This function converts the Internet host address cp from numbers and 
    dots notation into binary data in network byte order.

  PARAMETERS:
    cp - The address as a string in numbers and dots notation.

  RETURN VALUE: 
    Host address converted to binary or -1 (0xFFFF) if the input string is invalid.

*****************************************************************************/
uint32 BalNetInetAddr( char* cp );

/***************************************************************************

  FUNCTION NAME: NetHToNl

  DESCRIPTION:

    This function converts the unsigned integer HostLong from host byte order 
    to network byte order
    
  PARAMETERS:

    HostLong	-	A host byte order number
   
  RETURNED VALUES:

    An unsigned long of net byte order

*****************************************************************************/
uint32 NetHToNl (uint32 hostlong);

/***************************************************************************

  FUNCTION NAME: NetHToNs

  DESCRIPTION:

    This function converts the unsigned short integer hostshort from host byte 
    order to network byte order
    
  PARAMETERS:

    HostLong	-	A host byte order number
   
  RETURNED VALUES:

    An unsigned short of net byte order

*****************************************************************************/
uint16 NetHToNs(uint16 hostshort);

/***************************************************************************

  FUNCTION NAME: NetNToHl

  DESCRIPTION:

    This function converts the unsigned integer netlong from network byte order
    to host byte order.
    
  PARAMETERS:

    NetLong	-	A net byte order number
   
  RETURNED VALUES:

    An unsigned long of host byte order

*****************************************************************************/
uint32 NetNToHl (uint32 netlong);

/***************************************************************************

  FUNCTION NAME: NetNToHs

  DESCRIPTION:

    This function converts the unsigned short integer netshort from network byte 
    order to host byte order
    
  PARAMETERS:

    NetShort	-	A net byte order number
   
  RETURNED VALUES:

    An unsigned short of host byte order

*****************************************************************************/
uint16 NetNToHs (uint16 netshort);

/***************************************************************************

  FUNCTION NAME: NetGetPppAuthParms

  DESCRIPTION:
    This function gets the PPP authentication parameters from the protocol stack.

  PARAMETERS:
    AuthStrP - The PPP authentication parameters as a concatentation of 2 NULL 
                terminated strings "userid@domain.com\0password\0".
    LenP     - The length of the AuthStrP buffer is passed in. The size of the 
                string is passed out.
  RETURN VALUE: 
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetGetPppAuthParms( char* AuthStrP,
                                  int*  LenP );

/***************************************************************************

  FUNCTION NAME: NetSetPppAuthParms

  DESCRIPTION:
    This function sets the PPP authentication parameters from the protocol stack.

  PARAMETERS:
    AuthStrP - The PPP authentication parameters as a concatentation of 2 NULL
                terminated strings "userid@domain.com\0password\0".

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetSetPppAuthParms( char* AuthStrP );

/***************************************************************************

  FUNCTION NAME: NetGetDialStrParms

  DESCRIPTION:
    This function gets the dial number used while setup the PPP session.

  PARAMETERS:
    DialStrP - the dial number used.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetGetDialStrParms( char* DialStrP,
                                  int*  LenP );

/***************************************************************************

  FUNCTION NAME: NetSetDialStrParms

  DESCRIPTION:
    This function Sets the dial number used while setup the PPP session.

  PARAMETERS:
    DialStrP - the dial number used.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetSetDialStrParms( char* DialStrP );

/***************************************************************************

  FUNCTION NAME: NetSetRlpInactTmoReq

  DESCRIPTION:
    Tells the Browser and RLP to activate/deactivate its inactivity timer on the
     specified channel.

  PARAMETERS:
    nInactTime - 0   : Disable Inactivity Monitor Feature.
                 Else: Enable Inactivity Monitoring for the specified number of seconds.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetSetRlpInactTmoReq( uint16 nInactTime );

/***************************************************************************

  FUNCTION NAME: NetGetRlpSettings

  DESCRIPTION:
    This function gets the NAK related RLP settings from the protocol stack.

  PARAMETERS:
    RlpSettingsType - The current, default, or negotiated RLP settings.
    SettingsP       - The RLP settings. This is an output parameter, so it doesn't 
                       have to be initialized.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetGetRlpSetting( NetRlpSettingsTypeT RlpSettingsType, 
                                NetRlpSettingsT*    SettingsP );

/***************************************************************************

  FUNCTION NAME: NetSetRlpSettings

  DESCRIPTION:
    This function sets the NAK related RLP settings from the protocol stack.

  PARAMETERS:
    RlpSettingsType - The current, default, or negotiated RLP settings.
    SettingsP       - The RLP settings.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetSetRlpSetting( NetRlpSettingsTypeT RlpSettingsType,
                                   NetRlpSettingsT*    SettingsP );

/***************************************************************************

  FUNCTION NAME: NetGetIpAddress

  DESCRIPTION:
    This function gets the IP address of the MS if any.

  PARAMETERS: 
    IpAddrP - The IP address of the MS. This is an output parameter, so it doesn't 
               have to be initialized.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetGetIpAddress( uint32* IpAddrP );

/***************************************************************************

  FUNCTION NAME: BalNetGetDNSServerIpAddr

  DESCRIPTION:
    This function get the current DNS servers IP address.

  PARAMETERS:
    IpAddrP - Pointer to the DNS server's IP address. This is an output parameter, 
               so it doesn't have to be initialized.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetGetDNSServerIpAddr( uint32* IpAddrP );

/***************************************************************************

  FUNCTION NAME: BalNetGetPeerName

  DESCRIPTION:
    This function gets the peer name of the network node the socket it is associated with.

  PARAMETERS:
	SockFd   - The file decriptor ID of the socket.
    AddrP    - The address of the other end of the socket connection. This
  	            is an output parameter, so it doesn't have to be initialized.
    AddrLenP - The size of the address returned. This is an output parameter, so 
                it doesn't have to be initialized.

  RETURN VALUE:
    A success or failure code from NetResultT.

*****************************************************************************/
NetResultT BalNetGetPeerName( int16         SockFd, 
                              NetSockAddrT* AddrP, 
                              int16*        AddrLenP );

/*===========================================================================
FUNCTION BalNetShutdown()

DESCRIPTION
   Shut down socket descriptor.
   
   SHUT_RD: No more receives can be issued on socket; process can still send on socket;
                  socket receive buffer discarded; any further data received is discarded by TCP; 
                  no effect on socket send buffer;
  SHUT_WR: No more sends can be issued on socket; process can still receive on socket;
                  contents of socket send buffer sent to other end, followed by normal
                  TCP connection termintation (FIN; no effect on socket recive buffer;

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID
  shut dow: SHUT_RD SHUT_WR SHUT_WRRD

RETURN VALUE:
  NetResultT
===========================================================================*/

NetResultT NetShutdown(int16 sockfd, int8 how );

/*===========================================================================
FUNCTION BalNetSetNoDelay()

DESCRIPTION
   Use nagle algorthm

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID
  shut dow: TRUE: no use nagle algorithm; FALSE: use nagle algorithm

RETURN VALUE:
  NetResultT
===========================================================================*/

NetResultT NetSetNoDelay(int16 sockfd, bool bNodaly);

/*===========================================================================
FUNCTION BalNetGetNoDelay()

DESCRIPTION
   get if to use nagle algorthm

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID

RETURN VALUE:
  bool
===========================================================================*/

NetResultT NetGetNoDelay(int16 sockfd, bool* delay);

/*===========================================================================
FUNCTION BalNetSetKeepAlive()

DESCRIPTION
   Set keep Alive

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID
  keepAlive: if keep alive;

RETURN VALUE:
  NONE
===========================================================================*/

NetResultT NetSetKeepAlive(int16 sockfd, bool keepAlive);

/*===========================================================================
FUNCTION BalNetGetKeepAlive()

DESCRIPTION
   get if to keep Alive

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID
 
RETURN VALUE:
  keep alive or not.
===========================================================================*/

NetResultT NetGetKeepAlive(int16 sockfd, bool* keepAlive);

/*===========================================================================
FUNCTION BalNetSetLinger()

DESCRIPTION
   set if to linger

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID
  lingerOnOff: linger on or off
  lingerTime: time to linger
 
RETURN VALUE:
  NetResultT
===========================================================================*/

NetResultT NetSetLinger(int16 sockfd, bool lingerOnOff, uint32 lingerTime);

/*===========================================================================
FUNCTION BalNetGetLinger()

DESCRIPTION
   Get if to linger

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID
  lingerOnOff: linger on or off
  lingerTime: time to linger
 
RETURN VALUE:
  NONE
===========================================================================*/
NetResultT NetGetLinger(int16 sockfd, bool *lingerOnOff, uint32 *lingerTime);

/*===========================================================================
FUNCTION BalNetSetSendBufSize()

DESCRIPTION
   set send buffer size

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID
  size: buffer's size
 
RETURN VALUE:
  int
===========================================================================*/

NetResultT NetSetSendBufSize(int16 sockfd, int size );

/*===========================================================================
FUNCTION NetGetSendBufSize()

DESCRIPTION
   get send buffer size

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID

RETURN VALUE:
  int
===========================================================================*/

int NetGetSendBufSize(int16 sockfd);

/*===========================================================================
FUNCTION BalNetSetRecvBufSize()

DESCRIPTION
   Set receive buffer size

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID
  size: buffer size

RETURN VALUE:
  NetResultT
===========================================================================*/

NetResultT NetSetRecvBufSize(int16 sockfd, int size);

/*===========================================================================
FUNCTION BalNetGetRecvBufSize()

DESCRIPTION
   Get receive buffer size

DEPENDENCIES
  None.
  
PARAMETERS:
  sockfd:  socket ID

RETURN VALUE:
  int
===========================================================================*/

int NetGetRecvBufSize(int16 sockfd);

  /*===========================================================================
FUNCTION BalNetGetSockName()

DESCRIPTION

DEPENDENCIES
  None.
  
PARAMETERS:

RETURN VALUE
===========================================================================*/
NetResultT NetGetSockName(int16 sockfd, uint32 *IpAddrP, uint16 *port);

 #ifdef __cplusplus
 }
 #endif
 
/***************************************************************************

  FUNCTION NAME: BalNetProcessEvent

  DESCRIPTION:

    This function processes the event received and calls the registered call back   
    
  PARAMETERS:

    NetEventId   - The id of the val net event
    NetEventMsgP - the event message body
    
  RETURNED VALUES:
	<NONE>

*****************************************************************************/
void BalNetProcessEvent( BalNetEventIdT NetEventId,
                         void*          NetEventMsgP );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__BAL_NET_H__*/






