#ifndef __BAL_DNS_H__
#define __BAL_DNS_H__


#include "balhlwapi.h"

#ifdef SYS_OPTION_HL

/* added by lqs */
#define MAX_DN_LEN  256
#define MAX_DNS_BUF_LEN 512
#define MAX_IP_NUM 10
#define MAX_DNSSERV_NUM 2
#define DNS_TIMEOUT_TICK 500
#define DNS_TIMEOUT_TICK2 2
#define DNS_TIMEOUT_TICK3 300

#define TIME_BASE        10000

#define MAX_DNS_CACHE_NUM   100
#define MAX_IP_LEN          20
#define MAX_DNS_NUM         10

#define DNS_NUM_MAX          4
typedef enum
{
  DNS_CREATE,
  DNS_BIND,
  DNS_READ,
  DNS_WRITE,
  DNS_CLOSE,
  DNS_PPPOPEN,
  DNS_PPPCLOSE,
  DNS_END,
  DNS_TIMEROUT1,
  DNS_TIMEROUT2,
  DNS_NUM
}DnsEventT;

typedef enum
{
  PPP_NULL,
  PPP_OPENNING,
  PPP_OPEN,
  PPP_CLOSING,
  NUM_STATE
}DnsPppStateT;

typedef struct {
    char* h_name;
    /*we don't support alias query now*/
    /*char* h_aliases;*/
    /*always IN(0)*/
    int16 h_addrtype;
    /*the length of each address, always 4 here*/
    int16 h_length;
    /*ended with a null pointer*/
    char ** h_addr_list;    
} NetHostEntT;

typedef void (*DNS_CallBackT)(RegIdT id, void *pParam, NetHostEntT *pHostEnt);

typedef void (*APP_CallBackT)(RegIdT id, BalNetEventIdT EventId, void *EventMsgP);
/*
typedef struct {
    void *pData; //the pointer to which will be used in call back function
    char hostName[MAX_DN_LEN]; //the hostName which to query
    uint32 ipAddr[MAX_IP_NUM]; //the address where the ret addresses are 
    //located;
    int16 ipNUm; //the num of the ip addresses returned
} DNSCallBackParamT;
*/

typedef struct {
    bool bRecUse;
    RegIdT oriRegId;
    DNS_CallBackT callBackFunc;
    APP_CallBackT appcallbackFunc;
    char hostName[MAX_DN_LEN];
    void *pParam;
    int16 sockfd;
    int16 dnsServIndex; /*which server is used now;*/
    uint32 dnsServIP[MAX_DNSSERV_NUM];
    /* the timer where we receive the packet, we must cancel the timer*/
    NetHostEntT *pHostEnt;
    ExeTimerT mDNSTimer;/*the timer of one query test*/
    SockStateT sockstate;
    int16 dnsReqNum;
    bool bBinded;
    bool bPending;
} DNSRecBlkT;

typedef struct
{
  DnsPppStateT pppstate;
  RegIdT regId;
  uint8 DnsRecNum;
  NetStatusT netstate;
  int16 dnsPppReconnNum;
  ExeTimerT DNSPppReconnTimer;
  DNSRecBlkT DNSRecBlk[DNS_NUM_MAX];
}DnsMgrT;

typedef struct
{
  bool bAlive;
  uint32 ttl;
  uint16 rlen;
  char ipAddr[MAX_IP_LEN];
}DNSInfoT;

typedef struct
{
  char hostName[MAX_DN_LEN];
  DNSInfoT DnsInfo[MAX_DNS_NUM];
  uint32 ttlStarted;
  uint32 DnsNum;
}DNSRecT;

NetResultT BalDNSQueryMsg(char *address, APP_CallBackT appFunc, RegIdT regId);
void BalDNsQueryStop(RegIdT regId);
void NetDnsInit(void);
void DnsQueryMsg(void *DataPtr);
void DnsHandleNetMsg(void *MsgDataP);

#endif /*SYS_OPTION_HL*/

#endif /*__BAL_DNS_H__*/






