/*-*- c-basic-offset: 2 -*-
  +------------------------------------------------------------------------------
  |  File:       app_core.c
  +------------------------------------------------------------------------------
  |  Copyright 2003 Texas Instruments Berlin, AG
  |                 All rights reserved.
  |
  |                 This file is confidential and a trade secret of Texas
  |                 Instruments Berlin, AG
  |                 The receipt of or possession of this file does not convey
  |                 any rights to reproduce or disclose its contents or to
  |                 manufacture, use, or sell anything it may describe, in
  |                 whole, or in part, without the specific written consent of
  |                 Texas Instruments Berlin, AG.
  +-----------------------------------------------------------------------------
  |  Purpose :  Example application for TCP/IP and Socket API -- core functions.
  +-----------------------------------------------------------------------------
 */


/* This should only be compiled into the entity if TCP/IP is enabled */
#ifdef FF_GPF_TCPIP

#define APP_CORE_C

#define ENTITY_APP

/*==== INCLUDES =============================================================*/

#include <string.h>             /* String functions, e. g. strncpy(). */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef _SIMULATION_
#include "typedefs.h"           /* Condat data types. */
#endif /* _SIMULATION_ */
#include "vsi.h"                /* A lot of macros. */
#ifndef _SIMULATION_
#include "custom.h"
#include "gsm.h"                /* A lot of macros. */
#include "prim.h"               /* Definitions of used SAP and directions. */
#include "pei.h"                /* PEI interface. */
#include "tools.h"              /* Common tools. */
#endif /* _SIMULATION_ */
#include "socket_api.h"             /* Socket API. */
#include "app.h"                /* Global entity definitions. */
#include "p_8010_137_nas_include.h"
#include "p_sim.h"

extern void* p_malloc(int);
#ifdef EMO_SIM

#define bal_htons(a)          SOCK_HTONS(a)

struct in_addr 
{
	union 
	{
		struct { unsigned char s_b1,s_b2,s_b3,s_b4; } S_un_b;
		struct { unsigned short s_w1,s_w2; } S_un_w;
		unsigned long S_addr;
	} S_un;
#define s_addr  S_un.S_addr
#define s_host  S_un.S_un_b.s_b2
#define s_net   S_un.S_un_b.s_b1
#define s_imp   S_un.S_un_w.s_w2
#define s_impno S_un.S_un_b.s_b4
#define s_lh    S_un.S_un_b.s_b3
};

struct sockaddr_in 
{
	short   sin_family;
	unsigned short  sin_port;
	struct  in_addr sin_addr;
	char    sin_zero[8];
};

int emo_server_fd = -1;
#endif

#define hCommUI _ENTITY_PREFIXED (hCommUI)

extern T_HANDLE hCommUI;

/*==== Local data ============================================================*/

#define NPROCS 1                /* Maximum number of application processes. */

#define PORT_CHARGEN 19         /* Chargen service for download. */
#define PORT_ECHO     7         /* Echo port for tcpecho and udpecho. */
#define PORT_DISCARD  9         /* Discard port for upload. */

#define FQDN_LENGTH 255         /* Maximum length of a fully-qualified domain
								 * name. */

#undef HTONS
#define HTONS(a) ((((a) & 0xff) << 8) | (((a) & 0xff00) >> 8))
#undef NTOHS
#define NTOHS(a) HTONS(a)
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* Strings for process types; used for debugging and MUST correspond strictly
 * to the process type enum labels defined above. */
static char *proc_type_name[] = {
	"AP_NONE",                    /* 00 */
	"AP_TCPDL",                   /* dl */
	"AP_TCPUL",                   /* ul */
	"AP_UDPDL",
	"AP_UDPUL",
	"AP_TCPECHO",                 /* te */
	"AP_UDPECHO",                 /* ue */
	"AP_TCPSRV",
	"AP_DNSQRY",                  /* dq */
	"AP_UDPFORK",
	"AP_INVALID"
} ;

/* Strings for the process states; used for debugging and MUST correspond
 * strictly to the process state enum labels defined above, as the array is
 * indexed by those. */
static char *proc_state_name[PS_INVALID + 1] = {
	"PS_IDLE",
	"PS_W_DCM_OPEN",
	"PS_W_DCM_OPEN_ONLY",
	"PS_W_CREAT",
	"PS_W_SCONN",
	"PS_W_BIND",
	"PS_W_LISTN",
	"PS_W_LISTENS",
	"PS_W_DNS",
	"PS_COMM",
	"PS_W_SCLOS",
	"PS_W_DCLOS",
	"PS_W_CONN_INFO",
	"PS_DCM_OPEN",
	"PS_SOCK_OPEN",
	"PS_INVALID"
} ;

static PROC_CONTEXT_T proc_context_tcp ;
static PROC_CONTEXT_T proc_context_udp ;
static PROC_CONTEXT_T proc_context_dns ;

static PROC_CONTEXT_T cl_context[APP_N_CLIENTS] ;
static char server_name[FQDN_LENGTH+1] = APP_DEF_SERVER ;
/* Global server name. */
static char query_name[FQDN_LENGTH+1] = APP_DEF_DNSQUERY_ADDR ;
static int buffer_size = APP_DEF_BUFFER_SIZE ;
static U16 port_number = 0 ;    /* Port number override if non-zero. */
static int udp_interval = APP_DEF_UDP_INTERVAL ;
static T_SOCK_BEARER_TYPE bearer_select    = SOCK_BEARER_AS_SPECIFIED;
static T_SOCK_BEARER_TYPE sock_bearer_type = SOCK_BEARER_GPRS;
EXTERN BOOL custom_apn_valid;

void app_ui_send(PROC_CONTEXT_T *pcont, U32 event_type);
extern void peek_initialize_sockets();
extern void peek_process_socket_event(PROC_CONTEXT_T *context, unsigned int event);

static int s_tcp_socket_bearar_open = 0;
static int s_udp_socket_bearer_open = 0;


/*==== Local functions =======================================================*/

/*
 * Utility functions.
 */


static char *sock_bearer_type_string(T_SOCK_BEARER_TYPE btype)
{
	switch (btype)
	{
		case SOCK_BEARER_ANY: return "SOCK_BEARER_ANY" ;
		case SOCK_BEARER_GPRS: return "SOCK_BEARER_GPRS" ;
		case SOCK_BEARER_GSM: return "SOCK_BEARER_GSM" ;
		case SOCK_BEARER_USE_PROFILE: return "SOCK_BEARER_USE_PROFILE" ;
		case SOCK_BEARER_AS_SPECIFIED: return "SOCK_BEARER_AS_SPECIFIED" ;
		default: return "<unknown bearer type>" ;
	}
}

/** Give a print representation for the specified character. This is the
 * character itself for printable characters and a substitution character for
 * others.
 *
 * @param c    the character
 * @return     the print representation
 */
static char p_char(char c)
{
	return isprint(c) ? c : '~' ;
}


#define DUMP_LLENGTH 16         /* Data dump line length. */
#define DBUF_LENGTH (4 * DUMP_LLENGTH + 10) /* See example below. */
#define XDIGIT(n) ("0123456789abcdef"[n])

/** Dump the specified portion of the data as event traces like this:
 * 0000: 27 28 29 2a 2b 2c 2d 2e-2f 30 31 32 33 34 35 36 ['()*+,-./0123456]
 *
 * @param data    Pointer to data area
 * @param size    Size of data to dump
 */
void trace_dump_data(U8 *data, int size)
{
	char dump_buf[DBUF_LENGTH] ;  /* Buffer to dump a line into. */
	int lcount = 0 ;              /* Line count. */
	int i ;                       /* Index into data. */
	char *cp ;                    /* Pointer to current char in dump_buf[]. */

	while (size > 0)
	{
		cp = dump_buf ;
		/* Hex values. */
		for (i = 0; i < DUMP_LLENGTH && i < size; i++)
		{
			*cp++ = XDIGIT(data[i] >> 4) ;
			*cp++ = XDIGIT(data[i] & 0xf) ;
			*cp++ = (i == DUMP_LLENGTH/2 - 1) ? '-' : ' ' ;
		}

		/* Fill up with blanks. */
		for ( ; i < DUMP_LLENGTH; i++)
		{
			*cp++ = ' ' ; *cp++ = ' ' ; *cp++ = ' ' ;
		}

		/* Literal characters with some decoration. */
		*cp++ = '[' ;
		for (i = 0; i < DUMP_LLENGTH && i < size; i++, cp++)
		{
			*cp = p_char(data[i]) ;
		}
		*cp++ = ']' ;
		*cp++ = 0 ;
		TRACE_EVENT_P2("%04x: %s", DUMP_LLENGTH * lcount++, dump_buf) ;
		size -= DUMP_LLENGTH ;
		data += DUMP_LLENGTH ;
	}
}


/** Build a string, characterizing a process, suitable for tracing. The string
 * is statically allocated and will be overwritten with the next call.
 *
 * @param pcont    Pointer to process context.
 * @return         The string.
 */
char *proc_string(PROC_CONTEXT_T *pcont)
{
	static char procstring[128] = { 0 };
	unsigned int pstate = pcont->pstate;
	sprintf(procstring, "%d (%s)", pstate, (pstate <= PS_INVALID ? proc_state_name[pstate] : "PS_UNKNOWN")) ;
	return procstring;
}


/** Converts a numeric IP address in network order into an IP address in
 * dotted decimal string notation. The string returned is statically allocated
 * and will be overwritten on the next call.
 *
 * @param ipaddr    The IP address in network order
 * @return String with the IP address in dotted decimal..
 */
static char *inet_ntoa(T_SOCK_IPADDR ipaddr)
{
	U8 *addrbyte ;
	static char addr_string[sizeof("000.000.000.000")] ;

	addrbyte = (U8 *) &ipaddr ;
	sprintf(addr_string, "%u.%u.%u.%u",
			addrbyte[0], addrbyte[1], addrbyte[2], addrbyte[3]) ;
	return addr_string ;
}


/** Converts an IP address in dotted decimal string notation into a numeric IP
 * address in network order.
 *
 * @param addr_string    String with the IP address in dotted decimal.
 * @return The IP address in network order, or SOCK_IPADDR_ANY if the address
 *         string cannot be parsed.
 */
static T_SOCK_IPADDR inet_aton(char *addr_string)
{
	T_SOCK_IPADDR ipaddr ;
	U8 *addrbyte ;
	int o1, o2, o3, o4 ;

	if (sscanf(addr_string, "%d.%d.%d.%d", &o1, &o2, &o3, &o4) != 4)
	{
		TRACE_EVENT_P1("cannot parse '%s' as an IP address", addr_string) ;
		return SOCK_IPADDR_ANY ;
	}
	addrbyte = (U8 *) &ipaddr ;
	addrbyte[0] = (U8) o1 ;
	addrbyte[1] = (U8) o2 ;
	addrbyte[2] = (U8) o3 ;
	addrbyte[3] = (U8) o4 ;

	return ipaddr ;
}


LOCAL char *sock_result_string(T_SOCK_RESULT result)
{
	switch(result)
	{
		case SOCK_RESULT_OK                : return "SOCK_RESULT_OK";
		case SOCK_RESULT_INVALID_PARAMETER : return "SOCK_RESULT_INVALID_PARAMETER"; 
		case SOCK_RESULT_INTERNAL_ERROR    : return "SOCK_RESULT_INTERNAL_ERROR";
		case SOCK_RESULT_ADDR_IN_USE       : return "SOCK_RESULT_ADDR_IN_USE";
		case SOCK_RESULT_OUT_OF_MEMORY     : return "SOCK_RESULT_OUT_OF_MEMORY";
		case SOCK_RESULT_NOT_SUPPORTED     : return "SOCK_RESULT_NOT_SUPPORTED";
		case SOCK_RESULT_UNREACHABLE       : return "SOCK_RESULT_UNREACHABLE";
		case SOCK_RESULT_CONN_REFUSED      : return "SOCK_RESULT_CONN_REFUSED";
		case SOCK_RESULT_TIMEOUT           : return "SOCK_RESULT_TIMEOUT";
		case SOCK_RESULT_IS_CONNECTED      : return "SOCK_RESULT_IS_CONNECTED";
		case SOCK_RESULT_HOST_NOT_FOUND    : return "SOCK_RESULT_HOST_NOT_FOUND";
		case SOCK_RESULT_DNS_TEMP_ERROR    : return "SOCK_RESULT_DNS_TEMP_ERROR";
		case SOCK_RESULT_DNS_PERM_ERROR    : return "SOCK_RESULT_DNS_PERM_ERROR";
		case SOCK_RESULT_NO_IPADDR         : return "SOCK_RESULT_NO_IPADDR";
		case SOCK_RESULT_NOT_CONNECTED     : return "SOCK_RESULT_NOT_CONNECTED";
		case SOCK_RESULT_MSG_TOO_BIG       : return "SOCK_RESULT_MSG_TOO_BIG";
		case SOCK_RESULT_CONN_RESET        : return "SOCK_RESULT_CONN_RESET";
		case SOCK_RESULT_CONN_ABORTED      : return "SOCK_RESULT_CONN_ABORTED";
		case SOCK_RESULT_NO_BUFSPACE       : return "SOCK_RESULT_NO_BUFSPACE";
		case SOCK_RESULT_NETWORK_LOST      : return "SOCK_RESULT_NETWORK_LOST";
		case SOCK_RESULT_NOT_READY         : return "SOCK_RESULT_NOT_READY";
		case SOCK_RESULT_BEARER_NOT_READY  : return "SOCK_RESULT_BEARER_NOT_READY";
		case SOCK_RESULT_IN_PROGRESS       : return "SOCK_RESULT_IN_PROGRESS";
		case SOCK_RESULT_BEARER_ACTIVE     : return "SOCK_RESULT_BEARER_ACTIVE";
		default                            : return "<INVALID SOCKET RESULT!>";
	}
}



/** Trace a specific socket API result code with some context.
 *
 * @param pcont        Pointer to process context.
 * @param function     The function or event that reported the error.
 * @param result       Socket API result code.
 * @return
 */
static void sock_trace_result(PROC_CONTEXT_T *pcont, char *function,
		T_SOCK_RESULT result)
{
	if(result NEQ SOCK_RESULT_OK)
	{
		TRACE_ERROR("Sock Result Error");
	}
	TRACE_EVENT_P3("%s: %s for %s", function, 
			sock_result_string(result), proc_string(pcont));
}


/** Return the string for a Socket API event type. We don't have the values
 * under our (i. e. APP's) own control, so we rather do a switch than indexing
 * an array.
 *
 * @param event_type    Type of the event.
 * @return              String for the event type.
 */
char *sock_event_string(T_SOCK_EVENTTYPE event_type)
{
	switch (event_type)
	{
		case SOCK_CREATE_CNF:        return "SOCK_CREATE_CNF" ;
		case SOCK_CLOSE_CNF:         return "SOCK_CLOSE_CNF" ;
		case SOCK_BIND_CNF:          return "SOCK_BIND_CNF" ;
		case SOCK_LISTEN_CNF:        return "SOCK_LISTEN_CNF" ;
		case SOCK_CONNECT_CNF:       return "SOCK_CONNECT_CNF" ;
		case SOCK_SOCKNAME_CNF:      return "SOCK_SOCKNAME_CNF" ;
		case SOCK_PEERNAME_CNF:      return "SOCK_PEERNAME_CNF" ;
		case SOCK_HOSTINFO_CNF:      return "SOCK_HOSTINFO_CNF" ;
		case SOCK_MTU_SIZE_CNF:      return "SOCK_MTU_SIZE_CNF" ;
		case SOCK_RECV_IND:          return "SOCK_RECV_IND" ;
		case SOCK_CONNECT_IND:       return "SOCK_CONNECT_IND" ;
		case SOCK_CONN_CLOSED_IND:   return "SOCK_CONN_CLOSED_IND" ;
		case SOCK_ERROR_IND:         return "SOCK_ERROR_IND" ;
		case SOCK_FLOW_READY_IND:    return "SOCK_FLOW_READY_IND" ;
		case SOCK_OPEN_BEARER_CNF:   return "SOCK_OPEN_BEARER_CNF";
		case SOCK_CLOSE_BEARER_CNF:  return "SOCK_CLOSE_BEARER_CNF";
		case SOCK_BEARER_INFO_CNF:   return "SOCK_BEARER_INFO_CNF";
		case SOCK_BAERER_CLOSED_IND: return "SOCK_BAERER_CLOSED_IND";
		default:                     return "<INVALID EVENT>" ;
	}
}



/*
 * Process functions.
 */

static void proc_shutdown(PROC_CONTEXT_T *pcont) ;
static void proc_close_socket(PROC_CONTEXT_T *pcont) ;
static void proc_begin_comm(PROC_CONTEXT_T *pcont) ;
static void proc_close_conn(PROC_CONTEXT_T *pcont) ;


/** Switch process to a new state. Done mostly to have a single place to trace
 * process state transitions.
 *
 * @param pcont       Pointer to process context.
 * @param newstate    New state of process.
 */
static void proc_new_state(PROC_CONTEXT_T *pcont, PROC_STAT_T newstate)
{
	if (newstate < PS_INVALID)
	{
		TRACE_EVENT_P2("%s -> %s", proc_string(pcont), proc_state_name[newstate]) ;
		pcont->pstate = newstate ;
	}
	else
	{
		TRACE_EVENT_P2("%s invalid new state %d", proc_string(pcont), newstate) ;
		proc_shutdown(pcont) ;
	}
}

#if 0
/** Fork a new TCP server process context to handle a TCP client. Return a
 * pointer to the process context or NULL, if no process context is free any
 * more.
 *
 */
static PROC_CONTEXT_T *proc_new_tcpfork(PROC_CONTEXT_T *oldp)
{
	int i ;
	PROC_CONTEXT_T *pcont ;

	TRACE_FUNCTION("proc_new_tcpfork()") ;
	for (i = 0; i < APP_N_CLIENTS; i++)
	{
		if (cl_context[i].ptype EQ AP_NONE)
		{
			break ;
		}
	}
	if (i == APP_N_CLIENTS)
	{
		return NULL ;
	}

	pcont = &cl_context[i] ;
	memset(pcont, 0, sizeof(*pcont)) ;
	pcont->f_id = i ;
	pcont->ptype = AP_UDPFORK ;
	pcont->pstate = PS_IDLE ;
	pcont->ipproto = oldp->ipproto ;
	pcont->server_name = oldp->server_name ;
	pcont->server_ipaddr = oldp->server_ipaddr ;
	pcont->server_port = oldp->server_port ;
	pcont->network_is_open = TRUE ;
	pcont->psocket_is_open = TRUE ;

	return pcont ;
}

/** Free a TCP server process context.
 *
 */
static void proc_free_tcpfork(PROC_CONTEXT_T *pcont)
{
	TRACE_FUNCTION("proc_free_tcpfork()") ;
	proc_new_state(pcont, PS_IDLE) ;
	memset(pcont, 0, sizeof(*pcont)) ;
}

static void proc_init(PROC_CONTEXT_T *pcont, int prov, int size, int reps, APP_PROCTYPE_T ptype,
		T_SOCK_IPPROTO ipproto, U16 port)
{
	T_SOCK_BEARER_INFO bearer_info;
	BOOL bear_only = pcont->bearer_only;

	TRACE_FUNCTION("proc_init()") ;

	if (pcont->pstate != PS_IDLE)
	{
		TRACE_ERROR("proc_init: process still active, but ok...") ;
		return ;
	}

	memset(pcont, 0, sizeof(*pcont)) ;
	pcont->bearer_only = bear_only;
	pcont->ptype = ptype ;
	pcont->ipproto = ipproto ;
	pcont->server_name = (ptype EQ AP_TCPSRV) ? "<myself>" : server_name ;
	pcont->server_ipaddr = (ptype EQ AP_TCPSRV) ?
		SOCK_IPADDR_ANY : inet_aton(pcont->server_name) ;
	pcont->server_port = HTONS(port) ;
	pcont->spec_items = size ;
	pcont->spec_reps = reps ;
	pcont->in_shutdown = FALSE;

	pcont->psocket = 0;
	pcont->network_is_open = FALSE;
	pcont->psocket_is_open = FALSE;

	TRACE_EVENT_P7("%s for %d bytes %d reps, server %s:%d/%s on %s",
			proc_string(pcont), pcont->spec_items, pcont->spec_reps,
			inet_ntoa(pcont->server_ipaddr), NTOHS(pcont->server_port),
			(ipproto EQ SOCK_IPPROTO_UDP) ? "udp" : "tcp",
			sock_bearer_type_string(sock_bearer_type)) ;
	app_pstat(pcont);

	// fill connection params
	bearer_info.bearer_handle = sock_bearer_handle;
	bearer_info.app_handle    = APP_handle;
	bearer_info.bearer_type   = sock_bearer_type;

	if(sock_bearer_type == SOCK_BEARER_GPRS)
	{
		bearer_info.apn_valid = TRUE;
		bearer_info.phone_nr_valid = FALSE;
		bearer_info.cid = 1;

		switch(prov)
		{
			case APP_PROV_T_MOBILE:
				strcpy(bearer_info.apn, "wap.cingular");
				strcpy(bearer_info.user_id, "WAP@CINGULARGPRS.COM");
				strcpy(bearer_info.password, "CINGULAR1");
				break;

			case APP_PROV_HUTCH:
				strcpy(bearer_info.apn, "www");
				strcpy(bearer_info.user_id, "");
				strcpy(bearer_info.password, "");
				break;

			case APP_PROV_AIRTEL:
				strcpy(bearer_info.apn, "airtelgprs.com");
				strcpy(bearer_info.user_id, "");
				strcpy(bearer_info.password, "");
				break;

			case APP_PROV_CUSTOM:
				/* Copy valid APN */
				if(custom_apn_valid)
				{
					strcpy(bearer_info.apn, custom_apn);
					strcpy(bearer_info.user_id, custom_user_id);
					strcpy(bearer_info.password, custom_password);
					break;
				}
				/* Copy default settings for invalid APN settings */

			default:
				strcpy(bearer_info.apn,"web.vodafone.de");
				strcpy(bearer_info.user_id, "");
				strcpy(bearer_info.password, "");
				break;
		}
	}
	else
	{
		bearer_info.phone_nr_valid = TRUE;
		bearer_info.apn_valid = FALSE;
		bearer_info.cid = 0;
		if(prov == APP_PROV_T_MOBILE)
		{
			strcpy(bearer_info.phone_nr, "+491712524120");
			strcpy(bearer_info.user_id, "t-d1");
			strcpy(bearer_info.password, "wap");
		}
		else
		{
			strcpy(bearer_info.phone_nr, "+491722290000");
			strcpy(bearer_info.user_id, "");
			strcpy(bearer_info.password, "");
		}
	}
	bearer_info.user_id_valid = TRUE;
	bearer_info.password_valid = TRUE;

	bearer_info.ip_address = SOCK_IPADDR_ANY;
	bearer_info.dns1 = SOCK_IPADDR_ANY;
	bearer_info.dns2 = SOCK_IPADDR_ANY;
	bearer_info.gateway = SOCK_IPADDR_ANY;
	bearer_info.authtype = SOCK_AUTH_NO;
	bearer_info.data_compr = FALSE;
	bearer_info.header_comp = FALSE;
	bearer_info.precedence = 0;
	bearer_info.delay = 0;
	bearer_info.reliability = 0;
	bearer_info.peak_throughput = 0;
	bearer_info.mean_througput = 0;
	bearer_info.shareable = FALSE;

	sock_open_bearer(sock_api_inst,bearer_select,0,&bearer_info,app_sock_callback,pcont);
	if(pcont->bearer_only)
	{
		proc_new_state(pcont, PS_W_DCM_OPEN_ONLY) ;
	}
	else
	{
		proc_new_state(pcont, PS_W_DCM_OPEN) ;
	}
}

#endif

static void proc_client_closed(PROC_CONTEXT_T *pcont)
{
	T_SOCK_RESULT result ;
	TRACE_FUNCTION("proc_client_closed()") ;

	result = sock_close(pcont->psocket);
	if (result != SOCK_RESULT_OK)
	{
		TRACE_EVENT_P1("%s: error closing client socket", proc_string(pcont)) ;
		proc_shutdown(pcont) ;
		return;
	}
	app_free_udpfork(pcont) ;
}

#if 0

/***********************************************************************
 * Communication functions.
 */


/** Fill and send data buffer.
 *
 * @param pcont        Pointer to process context.
 * @param size         Size of data buffer.
 * @return
 */
static BOOL comm_send_buffer(PROC_CONTEXT_T *pcont, int size)
{
	char *payload ;               /* Pointer to payload buffer. */
	char *cp ;                    /* Pointer into paylaod buffer. */
	char *pp ;                    /* Pointer into test pattern. */
	T_SOCK_RESULT result ;        /* Result of send call. */

	TRACE_FUNCTION("comm_send_buffer()") ;
	MALLOC(payload, size) ;
	TRACE_EVENT_P1("PALLOC payload %x", payload) ;
	/* Fill buffer with pattern. */
	for (cp = payload, pp = APP_SEND_PATTERN; cp < payload + size; cp++, pp++)
	{
		if (pp >= APP_SEND_PATTERN + sizeof(APP_SEND_PATTERN) - 1)
		{
			pp = APP_SEND_PATTERN ;
		}
		*cp = *pp ;
	}
	if(pcont->ipproto == SOCK_IPPROTO_UDP)
	{
		// use UDP socket and specify destination IP address and destination port
		result = sock_sendto(pcont->psocket, payload, (U16)size,
				pcont->server_ipaddr,pcont->server_port) ;
	}
	else
	{
		result = sock_send(pcont->psocket, payload, (U16)size) ;
	}
	sock_trace_result(pcont, "sock_send()", result) ;
	MFREE(payload) ;
	switch (result)
	{
		case SOCK_RESULT_OK:
			TRACE_EVENT_P6("%s sent %d (%d/%d) bytes in rep %d/%d",
					proc_string(pcont), size, pcont->data_sent,
					pcont->spec_items,
					//* ((pcont->ipproto EQ SOCK_IPPROTO_TCP) ? 1 : size),
					pcont->n_reps, pcont->spec_reps) ;
			return TRUE ;
		case SOCK_RESULT_NO_BUFSPACE:
			return FALSE ;            /* Pause until SOCK_FLOW_READY_IND. */
		default:
			proc_shutdown(pcont) ;
			return FALSE ;
	}
}


/** Server: send some data to the client.
 *
 * @param pcont        Pointer to process context.
 * @return
 */
static BOOL comm_send_srvprompt(PROC_CONTEXT_T *pcont)
{
	char *payload ;               /* Pointer to payload buffer. */
	int size ;                    /* Actual size of payload. */
	T_SOCK_RESULT result ;        /* Result of send call. */
	TRACE_FUNCTION("comm_send_srv()") ;

	MALLOC(payload, 600) ;
	sprintf(payload, "%s: %sin_shutdown, last_evt %s, will %srepeat\n",
			proc_string(pcont), pcont->in_shutdown ? "" : "not ",
			pcont->last_evt
			? sock_event_string(pcont->last_evt->event_type) : "NULL",
			pcont->spec_reps ? "" : "not ") ;
	sprintf(payload + strlen(payload),
			"    rx %d B %d pkts, tx %d B %d pkts, errs %d conn %d\n",
			pcont->data_rcvd, pcont->items_rcvd,
			pcont->data_sent, pcont->items_sent,
			pcont->errors, pcont->n_reps) ;

	size = strlen(payload) ;
	result = sock_send(pcont->psocket, payload, (U16)size) ;
	sock_trace_result(pcont, "sock_send()", result) ;
	MFREE(payload) ;
	switch (result)
	{
		case SOCK_RESULT_OK:
			pcont->data_sent += size ;
			pcont->items_sent++ ;
			TRACE_EVENT_P5("%s sent %d (%d/%d) bytes in conn %d",
					proc_string(pcont), size, pcont->data_sent,
					pcont->spec_items * size,
					pcont->n_reps) ;
			return TRUE ;
		case SOCK_RESULT_NO_BUFSPACE:
			return FALSE ;            /* Pause until SOCK_FLOW_READY_IND. */
		default:
			proc_shutdown(pcont) ;
			return FALSE ;
	}
}

/** Issue a DNS query. Called for AP_DNSQRY in state PS_COMM.
 *
 * @param pcont        Pointer to process context.
 */
static void comm_query(PROC_CONTEXT_T *pcont)
{
	static int next_query = 0 ;   /* Next query index. */
	char *name ;                  /* Domain name to query for. */
	T_SOCK_RESULT result ;        /* Result of query call. */

	TRACE_FUNCTION("comm_query()") ;
	if (query_name[0])
	{
		name = query_name ;
	}
	else
	{
		name = domain_name[next_query] ;
	}
	TRACE_EVENT_P4("%s: query (%d/%d) for %s", proc_string(pcont),
			pcont->items_sent + 1, pcont->spec_items, name) ;
	result = sock_gethostbyname(sock_api_inst, name, app_sock_callback, pcont) ;
	sock_trace_result(pcont, "sock_gethostbyname()", result) ;
	if (result != SOCK_RESULT_OK)
	{
		pcont->errors++ ;
		TRACE_ERROR("sock_gethostbyname() failed, sleep...") ;
		vsi_t_sleep(VSI_CALLER 2000) ;
	}

	if (!domain_name[++next_query])
	{
		next_query = 0 ;
	}
	pcont->items_sent++ ;
}



/** Send data. Called for all but AP_DNSQRY in state PS_COMM.
 *
 * @param pcont        Pointer to process context.
 */
#if 0
static void comm_send(PROC_CONTEXT_T *pcont)
{
	TRACE_EVENT_P1("comm_send() %s", proc_string(pcont)) ;

	switch (pcont->ptype)
	{
		case AP_TCPDL:
			/* Do nothing -- the server will send again anyway. */
			return ;
		case AP_UDPDL:
			//      if (pcont->data_sent >= pcont->spec_items)
			//      {
			//        return;
			//      }
			break;
		case AP_TCPUL:
		case AP_UDPUL:
			//      if (pcont->data_sent >= pcont->spec_items)
			//      {
			TRACE_EVENT_P2("%s done after %d bytes",
					proc_string(pcont), pcont->data_sent) ;
			//        proc_close_socket(pcont) ;
			pcont->n_reps++ ;
			//				return ;
			//      }
			break ;
		case AP_TCPECHO:
		case AP_UDPECHO:
			//      if (pcont->items_sent >= pcont->spec_items)
			//      {
			TRACE_EVENT_P2("%s done after %d writes",
					proc_string(pcont), pcont->items_sent) ;
			//        proc_close_socket(pcont) ;
			//        pcont->n_reps++ ;
			//        return ;
			//      }
			break ;
		case AP_DNSQRY:
			comm_query(pcont) ;
			return ;
		case AP_UDPFORK:             /* Send some data, perhaps. */
			switch (pcont->server_port)
			{
				case PORT_CHARGEN:      /* Send something (below). */
					break ;
				case PORT_ECHO:         /* Send somewhere else. */
				case PORT_DISCARD:      /* Don't send anything. */
					return ;
				default:                /* Send a server prompt. */
					comm_send_srvprompt(pcont) ;
					break ;
			}
			return ;
		case AP_NONE:
		case AP_INVALID:
		default:
			TRACE_EVENT_P1("Invalid process type %s", proc_string(pcont)) ;
			return ;
	}

	if (comm_send_buffer(pcont, buffer_size))
	{
		pcont->items_sent++ ;
		TRACE_EVENT_P1("Sent Items: %u",pcont->items_sent);
		pcont->data_sent += buffer_size ;
		vsi_t_sleep(VSI_CALLER udp_interval);
	}
	else
	{
		if (pcont->ptype EQ AP_UDPUL)
		{
			TRACE_EVENT_P2("%s sleeps %d ms", proc_string(pcont), udp_interval) ;
			vsi_t_sleep(VSI_CALLER udp_interval);
		}
	}
}
#endif

#endif
/** Handle an incoming DNS result. Called for AP_DNSQRY in state PS_COMM.
 *
 * @param pcont        Pointer to process context.
 */
static void comm_dns_result(PROC_CONTEXT_T *pcont)
{
	T_SOCK_HOSTINFO_CNF *hinfo ;

	TRACE_FUNCTION("comm_dns_result()") ;

	hinfo = (T_SOCK_HOSTINFO_CNF *) pcont->last_evt ;
	if (hinfo->result != SOCK_RESULT_OK)
	{
		TRACE_EVENT_P3("lookup error %d in %d/%d queries",
				pcont->errors, pcont->items_rcvd, pcont->spec_items) ;
	}
	else
	{
		TRACE_EVENT_P1("Answer for host %s", hinfo->hostname) ;
		TRACE_EVENT_P3("has address %s (%d/%d)",
				inet_ntoa(hinfo->ipaddr),
				pcont->items_rcvd, pcont->spec_items) ;
	}

	pcont->server_ipaddr = hinfo->ipaddr;
	app_ui_send(pcont, EMOBIIX_NETSURF_DNS);
}

#if 0
/** Receive incoming data. Called for all but AP_TCPUL in state PS_COMM.
 *
 * @param pcont        Pointer to process context.
 */
static void comm_recv(PROC_CONTEXT_T *pcont)
{
	T_SOCK_RECV_IND *recv_ind = (T_SOCK_RECV_IND *) pcont->last_evt ;

	TRACE_FUNCTION("comm_recv()") ;
	if (pcont->ptype EQ AP_DNSQRY OR pcont->ptype EQ AP_TCPUL)
	{
		TRACE_EVENT_P2("%s: %s unexpected for ptype", proc_string(pcont),
				sock_event_string(pcont->last_evt->event_type)) ;
		proc_shutdown(pcont) ;
		return ;
	}
	pcont->data_rcvd += recv_ind->data_length ;
	pcont->items_rcvd++ ;
	TRACE_EVENT_P5("%s: recv #%d:%u bytes, total %u, total items sent:%u", proc_string(pcont),
			pcont->items_rcvd, recv_ind->data_length, pcont->data_rcvd,pcont->items_sent);
	trace_dump_data((U8 *) recv_ind->data_buffer,
			MIN(APP_DATA_DUMP_LENGTH, recv_ind->data_length)) ;
	MFREE(recv_ind->data_buffer) ;
	recv_ind->data_buffer = 0 ;
	switch (pcont->ptype)
	{
		case AP_UDPDL:
			/* After every sent UDP packet, a "answer" comes from the Chargen server. 
			 * If all packets are sent we are waiting for the last packet to 
			 * receive, else an unexpected event would be the result in the 
			 * "app_sock_callback()"; TCPIP_DATA_IND is received instead of SOCK_CLOSE_CNF
			 * TODO: why (pcont->items_sent-1), I assume that the server "confirms"
			 *       every packet
			 */
			//      if ((pcont->data_sent >= pcont->spec_items) &&
			//          (pcont->items_rcvd == pcont->items_sent))
			//      {
			TRACE_EVENT("last UDP-DL packet received");
			pcont->n_reps++ ;
			//        proc_close_socket(pcont) ;
			//			}
			//      else {
			comm_send(pcont);
			//      }
			break;
		case AP_TCPDL:
			//      if (pcont->data_rcvd >= pcont->spec_items)
			//      {
			TRACE_EVENT_P3("%s done after %d/%d bytes",
					proc_string(pcont), pcont->data_rcvd,
					pcont->spec_items) ;
			pcont->n_reps++ ;
			//        proc_close_socket(pcont) ;
			//      }
			break ;
		case AP_UDPECHO:
		case AP_TCPECHO:
		case AP_UDPFORK:
			comm_send(pcont) ;
			break ;
		default:
			TRACE_ERROR("Unexpected ptype in comm_recv()") ;
			break ;
	}
}


/** Handle a communication event according to the process type. Called for all
 * process types in state PS_COMM.
 *
 * @param pcont        Pointer to process context.
 */
static void comm_event(PROC_CONTEXT_T *pcont)
{
	TRACE_FUNCTION("comm_event()") ;

	switch (pcont->last_evt->event_type)
	{
		case SOCK_CONN_CLOSED_IND:
			if (pcont->ptype EQ AP_UDPFORK)
			{
				proc_client_closed(pcont) ;
				break ;
			}
			/*lint -fallthrough */
		case SOCK_ERROR_IND:
			TRACE_EVENT_P2("%s: %s, shutdown", proc_string(pcont),
					sock_event_string(pcont->last_evt->event_type)) ;
			// server should not reset , even if connection is reset by client.
			// but client should shutdown , if connection is reset by server.
			if((pcont->ptype EQ AP_UDPFORK)  AND
					(pcont->last_evt->result == SOCK_RESULT_CONN_RESET OR
					 pcont->last_evt->result == SOCK_RESULT_TIMEOUT))
			{
				proc_client_closed(pcont) ;
				return;
			}
			else
				proc_shutdown(pcont) ;
			return ;
		case SOCK_RECV_IND:
			app_comm_recv(pcont) ;
			break ;
		case SOCK_FLOW_READY_IND:
			// XXX: Event notify ready to send again
			/*
			   if(pcont->ptype NEQ AP_UDPDL) {
			   comm_send(pcont) ;
			   }
			 */
			break ;
		case SOCK_LISTEN_CNF:
			break;
		case SOCK_HOSTINFO_CNF:
			if (pcont->ptype EQ AP_DNSQRY) {
				comm_dns_result(pcont) ;
				break ;
			}
			/*lint -fallthrough */
		case SOCK_CREATE_CNF:
		case SOCK_CLOSE_CNF:
		case SOCK_BIND_CNF:
		case SOCK_CONNECT_CNF:
		case SOCK_SOCKNAME_CNF:
		case SOCK_PEERNAME_CNF:
		case SOCK_MTU_SIZE_CNF:
		case SOCK_CONNECT_IND:
			TRACE_EVENT_P2("%s: %s unexpected at all", proc_string(pcont),
					sock_event_string(pcont->last_evt->event_type)) ;
			proc_shutdown(pcont) ;
			return ;
		case SOCK_BAERER_CLOSED_IND:
			proc_shutdown(pcont);
			break;
		default:
			TRACE_EVENT_P2("comm_event(): %s unknown event %d",
					proc_string(pcont), pcont->last_evt->event_type) ;
			proc_shutdown(pcont) ;
			break ;
	}
}

#endif

/***********************************************************************
 * State machine functions (i. e. state-changing functions)
 */

/** Finish the process after the network connection has been closed.
 *
 * @param pcont       Pointer to process context.
 */
static void proc_finish(PROC_CONTEXT_T *pcont)
{
	TRACE_EVENT_P1("%s finished", proc_string(pcont)) ;
	pcont->network_is_open = FALSE ;
	pcont->in_shutdown = FALSE;
	proc_new_state(pcont, PS_IDLE) ;
}


/** Shutdown process hard, usually after an error or user request. This
 * includes closing the process's socket and network connection.
 *
 * @param pcont       Pointer to process context.
 */
static void proc_shutdown(PROC_CONTEXT_T *pcont)
{
	TRACE_FUNCTION("proc_shutdown()") ;

	app_ui_send(pcont, EMOBIIX_SOCK_DCON);

	if(pcont->in_shutdown)
	{
		TRACE_EVENT("Allready in shutdown");
		return;
	}
	pcont->in_shutdown = TRUE ;
	app_pstat(pcont) ;
	if (pcont->psocket_is_open)
	{
		proc_close_socket(pcont);
		return;
	}
	if (pcont->network_is_open OR
			pcont->pstate == PS_W_DCM_OPEN OR pcont->pstate == PS_W_DCM_OPEN_ONLY)
	{
		proc_close_conn(pcont);
		return;
	}
	else
	{
		proc_finish(pcont);
	}
}

void app_open_socket(PROC_CONTEXT_T *pcont)
{
	T_SOCK_RESULT result;

	emo_printf("app_open_socket(pcont=%08X)", pcont);

	result = sock_create(sock_api_inst, pcont->ipproto, app_sock_callback, pcont);
	if (result NEQ SOCK_RESULT_OK)
	{
		sock_trace_result(pcont, "sock_create()", result) ;
		proc_shutdown(pcont) ;
		return;
	}

	proc_new_state(pcont, PS_W_CREAT) ;
}


/** Close the network connection after the task has been done.
 *
 * @param pcont       Pointer to process context.
 */
static void proc_close_conn(PROC_CONTEXT_T *pcont)
{


	TRACE_FUNCTION("proc_close_conn()");
	if(pcont->network_is_open)
	{
		pcont->in_shutdown = TRUE;
		sock_close_bearer(sock_api_inst, sock_bearer_handle, app_sock_callback, pcont);
		proc_new_state(pcont, PS_W_DCLOS) ;
	}
	else
	{
		proc_finish(pcont);
	}
	app_ui_send(pcont, EMOBIIX_SOCK_DCON);
}


/** Connect the socket after it has been created.
 *
 * @param pcont       Pointer to process context.
 */
static void proc_connect_socket(PROC_CONTEXT_T *pcont)
{
	T_SOCK_RESULT result ;

	if (pcont != &proc_context_tcp && pcont != &proc_context_udp)
		return;

	TRACE_FUNCTION("proc_connect_socket()") ;
	/* If we do not yet have an IP address to connect to, look it up first. */
	if (pcont->server_ipaddr EQ SOCK_IPADDR_ANY)
	{
		result = sock_gethostbyname(sock_api_inst, pcont->server_name, app_sock_callback, pcont);
		if (result NEQ SOCK_RESULT_OK)
		{
			sock_trace_result(pcont, "sock_gethostbyname()", result) ;
			proc_shutdown(pcont) ;
			return;
		}
		proc_new_state(pcont, PS_W_DNS) ;
		return ;
	}

	result = sock_connect(pcont->psocket, pcont->server_ipaddr, pcont->server_port);
	if (result NEQ SOCK_RESULT_OK)
	{
		sock_trace_result(pcont, "sock_connect()", result) ;
		proc_shutdown(pcont) ;
		return;
	}
	proc_new_state(pcont, PS_W_SCONN) ;
}



#if 0
/** Begin communicating after the socket has been created.
 *
 * @param pcont       Pointer to process context.
 */
static void proc_begin_comm(PROC_CONTEXT_T *pcont)
{
	TRACE_FUNCTION("proc_begin_comm()") ;

	proc_new_state(pcont, PS_COMM) ;
	switch (pcont->ptype)
	{
		case AP_TCPDL:
			/* We wait for data from the server to arrive. */
			break ;
		case AP_UDPDL:
			/* Trigger the chargen server to send fisrt UDP packet */
			comm_send(pcont);
			break ;
		case AP_TCPUL:
		case AP_UDPUL:
		case AP_TCPECHO:
		case AP_UDPECHO:
		case AP_DNSQRY:
		case AP_UDPFORK:
			comm_send(pcont) ;
			break ;
		default:
			TRACE_EVENT_P2("%s unknown state (%d)",
					proc_string (pcont), pcont->ptype) ;
			break ;
	}
}
#endif

/** Close the socket after the requested communication has been done.
 *
 * @param pcont       Pointer to process context.
 */
static void proc_close_socket(PROC_CONTEXT_T *pcont)
{
	TRACE_FUNCTION("proc_close_socket()") ;

	sock_close(pcont->psocket) ;
	proc_new_state(pcont, PS_W_SCLOS) ;
}

#if 0
static void proc_bind_socket(PROC_CONTEXT_T *pcont)
{
	T_SOCK_RESULT result ;
	TRACE_FUNCTION("proc_bind_socket()") ;

	if ((result = sock_bind(pcont->psocket, pcont->server_port))
			!= SOCK_RESULT_OK)
	{
		sock_trace_result(pcont, "sock_bind()", result) ;
		proc_shutdown(pcont) ;
		return;
	}
	proc_new_state(pcont, PS_W_BIND) ;
}


static void proc_listen(PROC_CONTEXT_T *pcont)
{
	T_SOCK_RESULT result ;
	TRACE_FUNCTION("proc_listen()") ;

	if ((result = sock_listen(pcont->psocket)) != SOCK_RESULT_OK)
	{
		sock_trace_result(pcont, "sock_listen()", result) ;
		proc_shutdown(pcont) ;
		return;
	}
	proc_new_state(pcont, PS_W_LISTN) ;
}


static void proc_incoming(PROC_CONTEXT_T *pcont)
{
	T_SOCK_CONNECT_IND *conn_ind ;
	PROC_CONTEXT_T *newp ;
	T_SOCK_RESULT result ;
	TRACE_FUNCTION("proc_incoming()") ;

	conn_ind = (T_SOCK_CONNECT_IND *) pcont->last_evt ;

	if ((newp = proc_new_tcpfork(pcont)) EQ NULL)
	{
		TRACE_EVENT_P1("%s: failed to fork server, close new socket",
				proc_string(pcont)) ;
		sock_close(conn_ind->new_socket) ;
		return ;
	}

	/* We cannot make two calls to proc_string() without one overwriting the
	 * other, so we print the process strings in two successive traces. */
	TRACE_EVENT_P1("%s: forking to handle client connection...",
			proc_string(pcont)) ;
	TRACE_EVENT_P1("...forked process is %s", proc_string(newp)) ;
	newp->psocket = conn_ind->new_socket ;
	sock_set_callback(newp->psocket, app_sock_callback, newp) ;
	TRACE_EVENT_P3("%s connection from %s:%d, looking up...", proc_string(pcont),
			inet_ntoa(conn_ind->peer_ipaddr), NTOHS(conn_ind->peer_port)) ;
	if ((result = sock_gethostbyaddr(sock_api_inst, conn_ind->peer_ipaddr,
					app_sock_callback, newp))
			!= SOCK_RESULT_OK)
	{
		sock_trace_result(newp, "sock_gethostbyaddr()", result) ;
		proc_shutdown(newp) ;
		proc_shutdown(pcont) ;
		return;
	}
	proc_new_state(newp, PS_W_DNS) ;
}


static void proc_hostinfo_recvd(PROC_CONTEXT_T *pcont)
{
	T_SOCK_HOSTINFO_CNF *hinfo ;
	TRACE_FUNCTION("proc_hostinfo_recvd()") ;

	hinfo = (T_SOCK_HOSTINFO_CNF *) pcont->last_evt ;
	if (hinfo->result != SOCK_RESULT_OK)
	{
		sock_trace_result(pcont, "SOCK_HOSTINFO_CNF", hinfo->result) ;
	}
	else
	{
		TRACE_EVENT_P3("%s: connected peer is %s (%s)", proc_string(pcont),
				hinfo->hostname, inet_ntoa(hinfo->ipaddr)) ;
	}
	proc_begin_comm(pcont) ;
}
#endif


/*==== Exported functions ====================================================*/

void app_sock_callback_DNS(T_SOCK_EVENTSTRUCT *event, void *context)
{
	emo_printf("__DNS__(event_type=%d, result=%d, socket=%d, context=%08X)", event->event_type, event->result, event->socket, context);
	
	if (event->event_type != SOCK_HOSTINFO_CNF)
	{
		emo_printf("__DNS__ wrong event type");
	}

	((PROC_CONTEXT_T *)context)->last_evt = event ;     /* Save event in process context. */
	comm_dns_result((PROC_CONTEXT_T *)context);
}

void app_gethostbyname(const char *hostname)
{
	T_SOCK_RESULT result ;        /* Result of query call. */

	emo_printf("peek_gethostbyname()");

	if (!s_tcp_socket_bearar_open)
	{
		memset(&proc_context_dns, 0, sizeof(proc_context_dns));
		strcpy(proc_context_dns.server_name, hostname);
		app_open_bearer_tcp(&proc_context_dns);
		proc_new_state(&proc_context_dns, PS_W_DCM_OPEN_ONLY);
		return;
	}

	strcpy(proc_context_dns.server_name, hostname);
	proc_new_state(&proc_context_dns, PS_W_DNS);

	result = sock_gethostbyname(sock_api_inst, proc_context_dns.server_name, app_sock_callback_DNS, &proc_context_dns) ;

	sock_trace_result(&proc_context_dns, "sock_gethostbyname()", result) ;
	if (result != SOCK_RESULT_OK)
	{
		TRACE_ERROR("sock_gethostbyname() failed, sleep...") ;
		vsi_t_sleep(VSI_CALLER 2000) ;
	}
}

void app_ui_send(PROC_CONTEXT_T *pcont, U32 event_type)
{
	void *msg;

	emo_printf("app_ui_send() generating event: %d (%08X)", event_type, event_type);

	if (pcont != &proc_context_tcp && pcont != &proc_context_udp)
	{
		if (pcont == &proc_context_dns && event_type == EMOBIIX_SOCK_CREA)
		{
			app_gethostbyname(pcont->server_name);
			return;
		}

		peek_proc_socket_event(pcont, event_type);
		return;
	}

	switch (event_type)
	{
		case EMOBIIX_SOCK_CREA:
			msg = P_ALLOC(EMOBIIX_SOCK_CREA);
			break;

		case EMOBIIX_SOCK_RECV:
			msg = P_ALLOC(EMOBIIX_SOCK_RECV);
			//trace_dump_data((U8 *) pcont->eventBuf, pcont->data_rcvd);//MIN(APP_DATA_DUMP_LENGTH, pcont->data_rcvd));
			//((T_EMOBIIX_SOCK_RECV *)msg)->data = (U32)pcont->eventBuf;
			((T_EMOBIIX_SOCK_RECV *)msg)->size = pcont->data_rcvd;
			((T_EMOBIIX_SOCK_RECV *)msg)->data = (U32)pcont->eventBuf;
			//memcpy((void *)((T_EMOBIIX_SOCK_RECV *)msg)->data, pcont->eventBuf, pcont->data_rcvd);
			break;

		case EMOBIIX_SOCK_SENT:
			msg = P_ALLOC(EMOBIIX_SOCK_SENT);
			break;

		case EMOBIIX_SOCK_CONN:
			msg = P_ALLOC(EMOBIIX_SOCK_CONN);
			break;

		case EMOBIIX_SOCK_DCON:
			msg = P_ALLOC(EMOBIIX_SOCK_DCON);
			break;

		default:
			emo_printf("app_ui_send() unknown event_type %d", event_type);
			return;
	}

	PSENDX(UI, msg);
}

int app_open_bearer_tcp(PROC_CONTEXT_T *pcont)
{
	T_SOCK_BEARER_INFO bearer_info;
	int bearerRet;

	memset(&bearer_info, 0, sizeof(bearer_info));

	bearer_info.bearer_handle = sock_bearer_handle;
	bearer_info.app_handle    = APP_handle;
	bearer_info.bearer_type   = sock_bearer_type;

	bearer_info.apn_valid = TRUE;
	bearer_info.phone_nr_valid = FALSE;
	bearer_info.cid = 1;

//	strcpy(bearer_info.apn, "track.t-mobile.com");
//	strcpy(bearer_info.user_id, "getpeek");
//	strcpy(bearer_info.password, "txtbl123");

	strcpy(bearer_info.apn, "wap.cingular");
	strcpy(bearer_info.user_id, "WAP@CINGULARGPRS.COM");
	strcpy(bearer_info.password, "CINGULAR1");

	bearer_info.user_id_valid = TRUE;
	bearer_info.password_valid = TRUE;

	bearer_info.ip_address = SOCK_IPADDR_ANY;
//	bearer_info.dns1 = inet_aton("67.199.130.4");
//	bearer_info.dns2 = inet_aton("67.199.130.4");
	bearer_info.dns1 = inet_aton("66.209.10.201");
	bearer_info.dns2 = inet_aton("66.209.10.202");
	bearer_info.gateway = SOCK_IPADDR_ANY;
	bearer_info.authtype = SOCK_AUTH_NO;
	bearer_info.data_compr = FALSE; // compression?
	bearer_info.header_comp = FALSE; // compression?
	bearer_info.precedence = 0;
	bearer_info.delay = 0;
	bearer_info.reliability = 0;  // what does this do??
	bearer_info.peak_throughput = 0;
	bearer_info.mean_througput = 0;
	bearer_info.shareable = FALSE;

	emo_printf("app_open_bearer_tcp() opening socket bearer");
	bearerRet = sock_open_bearer(sock_api_inst, bearer_select, 0, &bearer_info, app_sock_callback, pcont);

	if(bearerRet > 0) 
	{
		emo_printf("app_open_bearer_tcp(): sock_open_bearer returned error code %d", bearerRet);
		return -1;
	}

	return 0;
}

int app_socket_emobiix (PROC_CONTEXT_T *pcont, int __type, int __protocol) {

	emo_printf("app_socket_emobiix() connecting to %s on %d", server_name, port_number);

	pcont->bearer_only = FALSE;
	pcont->ptype = (APP_PROCTYPE_T)__type;
	pcont->ipproto = (T_SOCK_IPPROTO)__protocol;
	pcont->in_shutdown = FALSE;
	pcont->psocket = 0;
	pcont->network_is_open = FALSE;
	pcont->psocket_is_open = FALSE;
	pcont->server_port = (__type EQ AP_UDPFORK) ? 0x700 : HTONS(port_number); // 0x700 some how maps to port 7
	strcpy(pcont->server_name,  (__type EQ AP_UDPFORK) ? "<myself>" : server_name);
	pcont->server_ipaddr = (__type EQ AP_UDPFORK) ?  SOCK_IPADDR_ANY : inet_aton(pcont->server_name) ;

	if(pcont->ipproto == SOCK_IPPROTO_UDP)	
		return 0;

	if (!s_tcp_socket_bearar_open)
	{
		if (app_open_bearer_tcp(pcont) >= 0)
			proc_new_state(pcont, PS_W_DCM_OPEN);
	}
	else
	{
		pcont->network_is_open = TRUE;
		app_open_socket(pcont);
	}

	return 0;
}

int app_socket(PROC_CONTEXT_T *pcont, int type, int proto) 
{
	T_SOCK_RESULT result;

	pcont->bearer_only = FALSE;
	pcont->ptype = (APP_PROCTYPE_T)type;
	pcont->ipproto = (T_SOCK_IPPROTO)proto;
	pcont->in_shutdown = FALSE;
	pcont->psocket = 0;
	pcont->network_is_open = FALSE;
	pcont->psocket_is_open = FALSE;
	//pcont->server_port = (__type EQ AP_UDPFORK) ? 0x700 : HTONS(port_number); // 0x700 some how maps to port 7
	//strcpy(pcont->server_name,  (__type EQ AP_UDPFORK) ? "<myself>" : server_name);
	//pcont->server_ipaddr = (__type EQ AP_UDPFORK) ?  SOCK_IPADDR_ANY : inet_aton(pcont->server_name) ;

	if (!s_tcp_socket_bearar_open)
	{
		if (app_open_bearer_tcp(pcont) < 0)
			return 0;
		
		proc_new_state(pcont, PS_W_DCM_OPEN);
		return 1;
	}

	pcont->network_is_open = TRUE;

	emo_printf("app_open_socket(pcont=%08X)", pcont);

	result = sock_create(sock_api_inst, pcont->ipproto, app_sock_callback, pcont);
	if (result NEQ SOCK_RESULT_OK)
	{
		sock_trace_result(pcont, "sock_create()", result) ;
		proc_shutdown(pcont) ;
		return 0;
	}

	proc_new_state(pcont, PS_W_CREAT);
	return 1;
}

int app_connect(PROC_CONTEXT_T *pcont, const char *host, int port)
{
	T_SOCK_RESULT result;

	emo_printf("app_connect(pcont=%08X, host=%s, port=%d)", pcont, host, port);

	//AP_TCPUL, SOCK_IPPROTO_TCP)

	strcpy(pcont->server_name, host);
	pcont->server_port = HTONS(port);
	pcont->server_ipaddr = inet_aton(pcont->server_name);

	result = sock_connect(pcont->psocket, pcont->server_ipaddr, pcont->server_port);
	if (result NEQ SOCK_RESULT_OK)
	{
		sock_trace_result(pcont, "sock_connect()", result) ;
		proc_shutdown(pcont) ;
		return 0;
	}

	proc_new_state(pcont, PS_W_SCONN);
	return 1;
}

static void app_bind_socket(PROC_CONTEXT_T *pcont)
{
	T_SOCK_RESULT result ;
	TRACE_FUNCTION("proc_bind_socket()") ;

	if ((result = sock_bind(pcont->psocket, pcont->server_port))
			!= SOCK_RESULT_OK)
	{
		sock_trace_result(pcont, "sock_bind()", result) ;
		proc_shutdown(pcont) ;
		return;
	}
	proc_new_state(pcont, PS_W_BIND) ;
}

static void app_listen(PROC_CONTEXT_T *pcont)
{
	T_SOCK_RESULT result ;
	TRACE_FUNCTION("proc_listen()") ;

	if ((result = sock_listen(pcont->psocket)) != SOCK_RESULT_OK)
	{
		sock_trace_result(pcont, "sock_listen()", result) ;
		proc_shutdown(pcont) ;
		return;
	}
	proc_new_state(pcont, PS_W_LISTN) ;
}

// Start of Emobiix socket stuff
static void app_comm_recv(PROC_CONTEXT_T *pcont)
{
	T_SOCK_RECV_IND *recv_ind = (T_SOCK_RECV_IND *) pcont->last_evt ;

	TRACE_FUNCTION("app_comm_recv()") ;

	TRACE_EVENT_P1("app_comm_recv(): recv %d bytes", recv_ind->data_length);

	//trace_dump_data((U8 *) recv_ind->data_buffer,
	//                MIN(APP_DATA_DUMP_LENGTH, recv_ind->data_length)) ;
	//recv_ind->data_buffer = 0;
	if(recv_ind->data_length > 0) {

		pcont->data_rcvd = recv_ind->data_length;
		pcont->eventBuf = p_malloc(recv_ind->data_length);
		memcpy(pcont->eventBuf, recv_ind->data_buffer, recv_ind->data_length);

		app_ui_send(pcont, EMOBIIX_SOCK_RECV);
	}

}

static void app_comm_event(PROC_CONTEXT_T *pcont)
{
	TRACE_FUNCTION("app_comm_event()") ;

	switch (pcont->last_evt->event_type)
	{
		case SOCK_CONN_CLOSED_IND:
			/*lint -fallthrough */
		case SOCK_ERROR_IND:
			TRACE_EVENT_P1("app_comm_event(): %s, shutdown", sock_event_string(pcont->last_evt->event_type)) ;
			proc_shutdown(pcont) ;
			return ;
		case SOCK_RECV_IND:
			app_comm_recv(pcont);
			break ;
		case SOCK_FLOW_READY_IND:
			// XXX: Event notify ready to send again
			app_ui_send(pcont, EMOBIIX_SOCK_SENT);
			break ;
		case SOCK_HOSTINFO_CNF: // result event of sock_gethostbyname
			/*
			   if (pcont->ptype EQ AP_DNSQRY) {
			   comm_dns_result(pcont) ;
			   break ;
			   }
			 */
			/*lint -fallthrough */
		case SOCK_CREATE_CNF:
		case SOCK_CLOSE_CNF:
		case SOCK_BIND_CNF:
		case SOCK_LISTEN_CNF:
		case SOCK_CONNECT_CNF:
		case SOCK_SOCKNAME_CNF:
		case SOCK_PEERNAME_CNF:
		case SOCK_MTU_SIZE_CNF:
		case SOCK_CONNECT_IND:
			TRACE_EVENT_P1("app_comm_event(): %s unexpected at all", sock_event_string(pcont->last_evt->event_type));
			proc_shutdown(pcont) ;
			return ;
		case SOCK_BAERER_CLOSED_IND:
			proc_shutdown(pcont);
			break;
		default:
			TRACE_EVENT_P1("app_comm_event(): unknown event %d", pcont->last_evt->event_type);
			proc_shutdown(pcont) ;
			break ;
	}
}

void app_set_profile(char *apn, char *userid, char *password)
{
	sock_bearer_type = SOCK_BEARER_GPRS;

	custom_apn_valid = TRUE;

	TRACE_EVENT_P3("apn_set_profile(): APN: %s - Userid: %s - Password: %s", apn, userid, password);

	strncpy(custom_apn, apn, SOCK_MAX_APN_LEN);
	strncpy(custom_user_id, userid, SOCK_MAX_USERID_LEN);
	strncpy(custom_password, password, SOCK_MAX_PASSWORD_LEN);
}

void app_connect_info(const char *server, unsigned short port)
{
	strncpy(server_name, server, FQDN_LENGTH);
	port_number = port;
}

BOOL app_send_buf(PROC_CONTEXT_T *pcont, char *buffer, int size)
{
	T_SOCK_RESULT result ;        /* Result of send call. */

	emo_printf("app_send_buf()") ;

	if(pcont->ipproto == SOCK_IPPROTO_UDP)
	{
		// use UDP socket and specify destination IP address and destination port
		result = sock_sendto(pcont->psocket, buffer, (U16)size, pcont->server_ipaddr, 
				pcont->server_port);
	}
	else
	{
		result = sock_send(pcont->psocket, buffer, (U16)size);
	}
	sock_trace_result(pcont, "app_send_buf()", result) ;

	switch (result)
	{
		case SOCK_RESULT_OK:
			emo_printf("app_send_buf() Sent (%d) bytes", pcont->data_sent);
			return TRUE;
		case SOCK_RESULT_NO_BUFSPACE:
			emo_printf("app_send_buf() sock_send result no bufferspace");
			return FALSE;            /* Pause until SOCK_FLOW_READY_IND. */
		default:
			emo_printf("app_send_buf() Bad result from sock_send %d", result);
			proc_shutdown(pcont);
			return FALSE;
	}
}

void app_connect_emobiix(void)
{
#ifndef EMO_SIM
	app_set_profile("track.t-mobile.com", "getpeek", "txtbl123");
	app_connect_info("10.150.9.6", 8444);
	app_socket_emobiix(&proc_context_tcp, AP_TCPUL, SOCK_IPPROTO_TCP);
#else
#if 0
	struct sockaddr_in connect_in;
	emo_server_fd = bal_socket(0 /*AF_INET*/, 0 /*SOCK_STREAM*/, 0 /*IPPROTO_TCP*/);
	if (emo_server_fd < 0)
	{
		emo_printf("app_connect() Failed to create socket");
		return;
	}

	app_ui_send(&proc_context_tcp, EMOBIIX_SOCK_CREA);

	memset(&connect_in, 0, sizeof(struct sockaddr_in));
	connect_in.sin_family = 0; /*AF_INET*/
	connect_in.sin_addr.s_addr = bal_inet_addr("69.114.111.9");
	connect_in.sin_port = bal_htons(12345);

	if (bal_connect(emo_server_fd, &connect_in, sizeof(connect_in)) < 0)
	{
		emo_printf("Failed to connect to server");
		return;
	}

	app_ui_send(&proc_context_tcp, EMOBIIX_SOCK_CONN);
#endif
#endif
}

void app_send(void *data) 
{
	T_EMOBIIX_WRITEMSG *writeMessage = (T_EMOBIIX_WRITEMSG *)data;
	emo_printf("app_send()");

#ifndef EMO_SIM
	//trace_dump_data((U8 *)writeMessage->data, writeMessage->size);
	// XXX: Will have to split up bigger buffers
	if(app_send_buf(&proc_context_tcp, (char *)writeMessage->data, writeMessage->size)) {
		// Handle Data sent ok
		emo_printf("app_send(): Wrote data");
	} else {
		// handle failed to send data
		emo_printf("app_send(): Failed to write data");
	}
#else
	if (bal_send(emo_server_fd, writeMessage->data, writeMessage->size, 0) < 0)
	{
		emo_printf("app_send() Failed to send message");
		app_ui_send(&proc_context_tcp, EMOBIIX_SOCK_DCON);
	}
	else
		app_ui_send(&proc_context_tcp, EMOBIIX_SOCK_SENT);
#endif

	p_free(writeMessage->data);
}

#ifdef EMO_SIM
void app_recv()
{
	static char eventBuf[2048] = { 0 };
	int ret = bal_recv(emo_server_fd, eventBuf, sizeof(eventBuf), 0);
	if (ret < 0)
	{
		emo_printf("Failed to read data");
		app_ui_send(&proc_context_tcp, EMOBIIX_SOCK_DCON);
		return;
	}

	proc_context_tcp.eventBuf = eventBuf;
	proc_context_tcp.data_rcvd = ret;
	app_ui_send(&proc_context_tcp, EMOBIIX_SOCK_RECV);
}
#endif

void app_bind_udp_server()
{
	T_SOCK_RESULT result ;

	if (!s_udp_socket_bearer_open)
	{
		app_socket_emobiix(&proc_context_udp, AP_UDPFORK, SOCK_IPPROTO_UDP);
		result = sock_create(sock_api_inst, 17, app_sock_callback, &proc_context_udp);
		if (result NEQ SOCK_RESULT_OK)
		{
			sock_trace_result(&proc_context_udp, "app_bind_udp_server() - sock_create()", result) ;
			proc_shutdown(&proc_context_udp) ;
			return;
		}

		proc_new_state(&proc_context_udp, PS_W_CREAT) ;
	}
}
// End of emobiix socket stuff

/** Initialize the application core.
 *
 * @param handle    own communication handle
 * @return PEI_OK/PEI_ERROR depending on the success of the initialization.
 */
BOOL app_initialize_tcpip(T_HANDLE app_handle)
{
	TRACE_FUNCTION("app_initialize_tcpip()") ;
	memset(&proc_context_tcp, 0, sizeof(proc_context_tcp)) ;
	memset(&proc_context_udp, 0, sizeof(proc_context_udp)) ;
	memset(&proc_context_dns, 0, sizeof(proc_context_dns)) ;

	peek_initialize_sockets();

#ifdef EMO_SIM
	app_connect_emobiix();
#endif
	return PEI_OK ;
}


/* Macro for checking the Socket API events in app_sock_callback(). */
#define CHECK_SOCK_EVT(evttype)                                       \
{                                                                     \
	if (event->event_type != evttype)                                   \
	{                                                                   \
		emo_printf("CHECK_SOCK_EVT() event->event_type = %d", event->event_type); \
		TRACE_ERROR("unexpected event type waiting for " #evttype) ;      \
		proc_shutdown(pcont) ;                                            \
		break ;                                                           \
	}                                                                   \
	if (event->result != SOCK_RESULT_OK)                                \
	{                                                                   \
		if(pcont->pstate == PS_W_DCM_OPEN OR                              \
				pcont->pstate == PS_W_DCM_OPEN_ONLY)                           \
		{ proc_new_state(pcont, PS_IDLE); }                            \
		proc_shutdown(pcont) ;                                            \
		break ;                                                           \
	}                                                                   \
}


/** Socket callback function as specified in the Socket API.
 *
 * @param event      Pointer to event struct passed by API.
 * @param context    Pointer to application context (here: process context)
 * @return
 */
void app_sock_callback(T_SOCK_EVENTSTRUCT *event, void *context)
{
	PROC_CONTEXT_T *pcont = (PROC_CONTEXT_T *)context;

	emo_printf("app_sock_callback() %s", proc_string(pcont)) ;

	pcont->last_evt = event ;     /* Save event in process context. */

	emo_printf("app_sock_callback(event_type=%d, result=%d, socket=%d, context=%08X)", event->event_type, event->result, event->socket, context);
    sock_trace_result(pcont, sock_event_string(event->event_type), event->result);

	if (event->result != SOCK_RESULT_OK)
	{
		switch (event->result)
		{
			case SOCK_RESULT_NOT_READY:
				{
					pcont->errors++ ;
					if(event->result == SOCK_RESULT_NETWORK_LOST)
						pcont->network_is_open = FALSE;
				}
				break;

			case SOCK_RESULT_TIMEOUT:
				app_ui_send(pcont, EMOBIIX_SOCK_DCON);
				return;

			default:
				{
					emo_printf("app_sock_callback() ignoring error %d (%08X)", event->result, event->result);
					return;
				}
		}
	}

	emo_printf("app_sock_callback() processing state %s", proc_string(pcont));

	switch (pcont->pstate)        /* Do a preliminary check of the event. */
	{
		case PS_W_DCM_OPEN:            /* Waiting for DCM to open connection. */
			CHECK_SOCK_EVT(SOCK_OPEN_BEARER_CNF);

			// FST: can't be evaluated-> see makro CHECK_SOCK_EVT
			if (event->result != SOCK_RESULT_OK AND 
					event->result != SOCK_RESULT_BEARER_ACTIVE)
			{
				proc_shutdown(pcont) ;
				return ;
			}
			if (pcont->network_is_open)
			{
				TRACE_ERROR("SOCK_OPEN_BEARER_CNF received but pcont->network_is_open") ;
				proc_shutdown(pcont);
				return ;
			}

			{
				T_NAS_ip ip;
				U8 *octets;

				cmhSM_get_pdp_addr_for_CGPADDR(1, &ip);
				octets = ip.ip_address.ipv4_addr.a4;
				TRACE_EVENT_P4("app_sock_callback(): Received IP address: %d.%d.%d.%d", octets[0], octets[1], octets[2], octets[3]);
			}

			s_tcp_socket_bearar_open = 1;
			pcont->network_is_open = TRUE;
			app_open_socket(pcont);
			break;

		case PS_W_DCM_OPEN_ONLY:
			CHECK_SOCK_EVT(SOCK_OPEN_BEARER_CNF);
			//pcont->network_is_open = TRUE ;
			//proc_new_state(pcont, PS_DCM_OPEN);
			s_tcp_socket_bearar_open = 1;
			app_ui_send(pcont, EMOBIIX_SOCK_CREA);
			break;

		case PS_W_DCLOS:            /* Waiting for DCM to close connection. */
			CHECK_SOCK_EVT(SOCK_CLOSE_BEARER_CNF);

			if (!pcont->network_is_open AND pcont->pstate != PS_IDLE)
			{
				TRACE_ERROR("DCM_CONN_CLOSED received but !pcont->network_is_open") ;
				proc_shutdown(pcont) ;
				return ;
			}
			proc_finish(pcont) ;
			break;

		case PS_W_CONN_INFO:
		{
			T_SOCK_BEARER_INFO_CNF *info = (T_SOCK_BEARER_INFO_CNF *)event;

			TRACE_EVENT("SOCK_BEARER_INFO_CNF received");

			CHECK_SOCK_EVT(SOCK_BEARER_INFO_CNF);

			app_print_conn_info(info);
		}
		break;

		case PS_W_CREAT:            /* Waiting for socket create confirmation. */
			CHECK_SOCK_EVT(SOCK_CREATE_CNF) ;
			pcont->psocket = event->socket ;
			pcont->psocket_is_open = TRUE ;

			if(pcont->ipproto == SOCK_IPPROTO_TCP)
			{
				app_ui_send(pcont, EMOBIIX_SOCK_CREA);
				proc_connect_socket(pcont) ;
				TRACE_EVENT("app_sock_callback() PS_W_CREAT event tcp");
				//bind UDP once TCP socket is established
				//	app_bind_udp_server();
			}
			else
			{
				// This is not possible in the moment because the RNET_API does not 
				// provide a sendto() function. Therefore it is is only possible to sent
				// via "connected" UDP sockets.
				// TODO: if the next statement will be enabled the "proc_connect_socket()" has to be removed!!
				// proc_begin_comm(pcont);
				//proc_connect_socket(pcont) ;
				TRACE_EVENT("app_sock_callback() PS_W_CREAT event udp");	
				s_udp_socket_bearer_open = 1;
				app_bind_socket(pcont);
			}
			break ;

		case PS_W_BIND:
			CHECK_SOCK_EVT(SOCK_BIND_CNF) ;
			//app_listen(pcont);
			proc_new_state(pcont, PS_COMM);
			break ;

		case PS_W_LISTN:
			CHECK_SOCK_EVT(SOCK_LISTEN_CNF) ;
			app_pstat(pcont) ;
			proc_new_state(pcont, PS_LISTENS) ; /* Nothing more to do here. */
			break ;

		case PS_LISTENS:            /* SOCK_CONNECT_IND or SOCK_CLOSE_CNF */
			emo_printf("app_sock_callback() PS_LISTENS");
			if (event->event_type EQ SOCK_CONNECT_IND AND (pcont->ipproto == SOCK_IPPROTO_UDP))
			{
				emo_printf("app_sock_callback(): PS_LISTENS event for UDP");
				//app_incoming(pcont);
			}
			break;

		case PS_W_DNS:
			CHECK_SOCK_EVT(SOCK_HOSTINFO_CNF) ;
			comm_dns_result(pcont);
			break ;

		case PS_W_SCONN:            /* Waiting for socket connect confirmation. */
			CHECK_SOCK_EVT(SOCK_CONNECT_CNF) ;
			/* Notify UI of socket connection */
			app_ui_send(pcont, EMOBIIX_SOCK_CONN);

			//proc_begin_comm(pcont) ;
			proc_new_state(pcont, PS_COMM); // advance to read/write state
			break ;

		case PS_COMM:               /* Happily exchanging data. */
			app_comm_event(pcont) ;
			break ;

		case PS_W_SCLOS:            /* Waiting for socket close confirmation. */
			CHECK_SOCK_EVT(SOCK_CLOSE_CNF) ;

			pcont->psocket_is_open = FALSE ;
			pcont->psocket = 0;
			app_pstat(pcont) ;
			if (pcont->n_reps >= pcont->spec_reps OR
					pcont->in_shutdown)
			{        
				proc_close_conn(pcont) ;
			}
			else
			{
				pcont->data_sent = 0 ;
				pcont->data_rcvd = 0 ;
				pcont->items_sent = 0 ;
				pcont->items_rcvd = 0 ;
				app_open_socket(pcont) ;
			}
			break ;

		case PS_IDLE:               /* Initial state, process not running. */
			TRACE_EVENT_P2("app_sock_callback(): %s receives %s (ignored)",
					proc_string(pcont), sock_event_string(event->event_type)) ;
			break ;

		case PS_DCM_OPEN:
			if(event->event_type == SOCK_BAERER_CLOSED_IND)
			{
				TRACE_ERROR("SOCK_BAERER_CLOSED_IND -> Shutdown");
				if(event->result == SOCK_RESULT_NETWORK_LOST)
				{
					pcont->network_is_open = FALSE;
				}
				proc_shutdown(pcont) ;
			}
			break;

		case PS_INVALID:            /* Invalid state. */
			TRACE_EVENT_P1("app_sock_callback(): %s invalid state", proc_string(pcont)) ;
			break;

		default:
			TRACE_ERROR("app_sock_callback(): Default Statement");
			break;
			/*
			   if(event->event_type == SOCK_DCM_ERR_IND)
			   {
			   TRACE_ERROR("SOCK_DCM_ERR_IND -> Shutdown");
			   }
			   proc_shutdown(pcont) ;
			   return ;
			 */
	}
	/* Free data buffer if it has not been freed yet. */
	if ( (event->event_type EQ SOCK_RECV_IND) AND
			((T_SOCK_RECV_IND *) event)->data_buffer )
	{
		MFREE(((T_SOCK_RECV_IND *) event)->data_buffer) ;
	}
	pcont->last_evt = NULL ;
	TRACE_EVENT_P1("leave app_sock_callback() for %s", proc_string(pcont)) ;
}


#if 0
/*
 * Application command functions.
 */


/** Start a data communication process of the appropriate type..
 *
 * @param size     Amount of data to download or number of items to transfer.
 */
void app_start_tcpdl(int prov, int size, int reps)
{ proc_init(&proc_context_tcp, prov, size, reps, AP_TCPDL, SOCK_IPPROTO_TCP,
		port_number ? port_number : PORT_CHARGEN) ; }

void app_start_tcpul(int prov, int size, int reps)
{ proc_init(&proc_context_tcp, prov, size, reps, AP_TCPUL, SOCK_IPPROTO_TCP,
		port_number ? port_number : PORT_DISCARD) ; }

void app_start_udpdl(int prov, int size, int reps)
{ proc_init(&proc_context_udp, prov, size, reps, AP_UDPDL, SOCK_IPPROTO_UDP,
		port_number ? port_number : PORT_CHARGEN) ; }

void app_start_udpul(int prov, int size, int reps)
{ proc_init(&proc_context_udp, prov, size, reps, AP_UDPUL, SOCK_IPPROTO_UDP,
		port_number ? port_number : PORT_DISCARD) ; }

void app_start_tcpecho(int prov, int items, int reps)
{ proc_init(&proc_context_tcp, prov, items, reps, AP_TCPECHO, SOCK_IPPROTO_TCP,
		port_number ? port_number : PORT_ECHO) ; }

void app_start_udpecho(int prov, int items, int reps)
{ proc_init(&proc_context_udp, prov, items, reps, AP_UDPECHO, SOCK_IPPROTO_UDP,
		port_number ? port_number : PORT_ECHO) ; }

void app_start_dnsquery(int prov, int times, char *address)
{
	if (address)
	{
		strncpy(query_name, address, FQDN_LENGTH) ;
	}
	else
	{
		query_name[0] =0 ;
	}
	proc_init(&proc_context_tcp, prov, times,1, AP_DNSQRY,(T_SOCK_IPPROTO)0, 0) ;
}

void app_start_tcpsrv(int prov, int port, int repeat)
{ proc_init(&proc_context_tcp, prov, 0, repeat, AP_TCPSRV, SOCK_IPPROTO_TCP, port) ; }


/** Shutdown the specified process.
 *
 * @param pid    Process ID.
 */
void app_shutdown(void)
{
	TRACE_FUNCTION("app_shutdown()") ;
	proc_shutdown(&proc_context_tcp) ;
	proc_shutdown(&proc_context_udp) ;
}


/** Set the current server name or IP address.
 *
 * @param server    Name or IP address (in dotted decimal notation) of server.
 */
void app_server(char *server)
{
	if (server)
	{
		strncpy(server_name, server, FQDN_LENGTH) ;
	}
	TRACE_EVENT_P1("server_name is %s", server_name) ;
}


/** Set or show the current buffer size.
 *
 * @param bufsize    size of buffer as a string or NULL
 */
void app_buffer(char *bufsize)
{
	if (bufsize)
	{
		buffer_size = atoi(bufsize) ;
	}
	TRACE_EVENT_P1("buffer_size is %d", buffer_size) ;
}


/** Set or show the current buffer size.
 *
 * @param port     port number override
 */
void app_port(char *port)
{
	if (port)
	{
		port_number = (U16) atoi(port) ;
	}
	if (port_number)
	{
		TRACE_EVENT_P1("port number override is %d", port_number) ;
	}
	else
	{
		TRACE_EVENT("standard port numbers used") ;
	}
}


/** Set or show the current bearer type.
 *
 * @param bearer     bearer type
 */
void app_bearer(char *bearer)
{
	if (bearer)
	{
		if (!strcmp(string_to_lower(bearer), "any"))
		{
			sock_bearer_type = SOCK_BEARER_ANY;
		}
		else if (!strcmp(string_to_lower(bearer), "gprs"))
		{
			sock_bearer_type = SOCK_BEARER_GPRS ;
		}
		else if (!strcmp(string_to_lower(bearer), "gsm"))
		{
			sock_bearer_type = SOCK_BEARER_GSM;
		}
		else if (!strcmp(string_to_lower(bearer), "prof"))
		{
			sock_bearer_type = SOCK_BEARER_USE_PROFILE;
		}    
		else if (!strcmp(string_to_lower(bearer), "spec"))
		{
			sock_bearer_type = SOCK_BEARER_AS_SPECIFIED;
		}

		else
		{
			TRACE_EVENT_P1("bearer type %s unknown", bearer) ;
		}
	}
	TRACE_EVENT_P1("bearer type is %s", sock_bearer_type_string(sock_bearer_type)) ;
}
#endif

/** Trace information about the process.
 */
void app_pstat(PROC_CONTEXT_T *pcont)
{
	TRACE_FUNCTION("app_pstat()") ;

	TRACE_EVENT_P3("%s in_shutdown %d last_evt %08x",
			proc_string(pcont), pcont->in_shutdown, pcont->last_evt) ;
	TRACE_EVENT_P6("prot %d srv %s %s:%d sp_it %d sp_rep %d",
			pcont->ipproto,
			pcont->server_name ? pcont->server_name : "",
			inet_ntoa(pcont->server_ipaddr),
			NTOHS(pcont->server_port),
			pcont->spec_items,
			pcont->spec_reps) ;
	TRACE_EVENT_P6("dta tx %d dta rx %d it tx %d it rx %d rep %d errs %d",
			pcont->data_sent,
			pcont->data_rcvd,
			pcont->items_sent,
			pcont->items_rcvd,
			pcont->n_reps,
			pcont->errors) ;
	TRACE_EVENT_P5("Socket descr: %x, %sNetwork%s, %spSocket%s",
			pcont->psocket,
			pcont->network_is_open ? "" : "no ",
			pcont->network_is_open ? " open" : "",
			pcont->psocket_is_open ? "" : "no ",
			pcont->psocket_is_open ? " open" : "") ;
	TRACE_EVENT_P4("global: server %s query %s buffer %d port %d",
			server_name, query_name, buffer_size, port_number) ;
}


/** Make the application stop or continue receiving data from the network by
 * calling the xoff or xon function, respectively.
 *
 * @param flow_on    if non-zero, switch flow on; off otherwise.
 */
void app_switch_flow(PROC_CONTEXT_T *pcont, int flow_on)
{
	TRACE_FUNCTION("app_switch_flow()") ;

	if (flow_on)
	{
		TRACE_EVENT("switching socket to xon") ;
		sock_flow_xon(pcont->psocket) ;
	}
	else
	{
		TRACE_EVENT("switching socket to xoff") ;
		sock_flow_xoff(pcont->psocket) ;
	}
}

LOCAL void app_print_conn_info(T_SOCK_BEARER_INFO_CNF *info)
{
	TRACE_EVENT_P1("BearerType: %s",
			sock_bearer_type_string(info->bearer_params.bearer_type));
	TRACE_EVENT_P1("APN: %s", info->bearer_params.apn);
	TRACE_EVENT_P1("PhoneNumber: %s", info->bearer_params.phone_nr);
	TRACE_EVENT_P1("UserId: %s", info->bearer_params.user_id);
	TRACE_EVENT_P1("Password: %s", info->bearer_params.password);
	TRACE_EVENT_P1("IP-Address: %s", inet_ntoa(info->bearer_params.ip_address));
	TRACE_EVENT_P1("DNS1-Address: %s", inet_ntoa(info->bearer_params.dns1));  
	TRACE_EVENT_P1("DNS2-Address: %s", inet_ntoa(info->bearer_params.dns2));
	TRACE_EVENT_P1("Gateway-Address: %s", inet_ntoa(info->bearer_params.gateway));
	TRACE_EVENT_P1("CID: %d",info->bearer_params.cid);
}


#if 0
void app_open_bearer(int prov, int size, int reps)
{
	proc_context_tcp.bearer_only = TRUE;
	proc_init(&proc_context_tcp, prov, size, reps, AP_NONE, SOCK_IPPROTO_TCP, port_number );
	proc_context_tcp.bearer_only = FALSE;

}

#endif

void app_close_tcp_bearer(PROC_CONTEXT_T *context)
{
	emo_printf("app_close_tcp_bearer() closing socket for %08X", context);
	context->in_shutdown = TRUE;
	s_tcp_socket_bearar_open = 0;
	sock_close_bearer(sock_api_inst, sock_bearer_handle, app_sock_callback, context);
}


#endif /* FF_GPF_TCPIP */

/* EOF */
