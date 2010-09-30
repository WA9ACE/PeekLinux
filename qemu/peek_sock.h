#ifndef __PEEK_SOCK_H__
#define __PEEK_SOCK_H__

typedef struct peek_socket_s {
    int sock;
    int type;
    int domain;
    int protocol;
    int key;
    int state;
    /* Connect vars */
    int cRet;
    int wRet;
    int rRet;
    struct sockaddr_in *conn_in;

    /* Connect Sock */
    uint32_t cTransferSize;
    uint32_t cTransferCount;
    char *cTransferbuffer;

    /* Write Sock */
    uint32_t wTransferSize;
    uint32_t wTransferCount;
    char *wTransferbuffer;

    /* Read Sock */
    uint32_t rTransferSize;
    char *rTransferbuffer;
    uint32_t rTransferCount;

    /* GethostbyName */
    uint32_t gTransferSize;
    char *gTransferbuffer;
    uint32_t gTransferCount;
    int gRet;

} peek_socket_s;

#endif // __PEEK_SOCK_H__

