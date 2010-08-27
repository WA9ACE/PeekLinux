#include "hw/peek.h"
#include "qemu_socket.h"
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "console.h"


#ifdef _WIN32

#define POLLIN      0x0001    /* There is data to read */
#define POLLPRI     0x0002    /* There is urgent data to read */
#define POLLOUT     0x0004    /* Writing now will not block */
#define POLLERR     0x0008    /* Error condition */
#define POLLHUP     0x0010    /* Hung up */
#define POLLNVAL    0x0020    /* Invalid request: fd not open */

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

typedef struct LocostoKeyPadState {
   int keyid;
} LocostoKeyPadState;


struct pollfd {
        SOCKET fd;
        short events;
        short revents;

};
#endif

static void peek_client_read(void *opaque)
{
	fprintf(stderr, "Got peek_client_read()\n");
}

static void peek_client_write(void *opaque)
{
        fprintf(stderr, "Got peek_client_write()\n");
}


//int socket(int domain, int type, int protocol);
int SockCreate(int domain, int type, int protocol, void *opaque) {

	peek_socket_s *s = (peek_socket_s *)opaque;

        s->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s->sock == -1) {
		return -1;
	}

	return s->sock;
}

//        int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
int SockConnect(int sockfd, void *opaque) {

	int on=1;
	char buf[256];
	peek_socket_s *s = (peek_socket_s *)opaque;
	struct sockaddr_in sin = *((struct sockaddr_in *) s->cTransferbuffer);

	//sin.sin_family = AF_INET;
	//sin.sin_port = htons(2501);
	//sin.sin_addr.s_addr = inet_addr("72.20.1.6");
	//memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s:%u", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
	fprintf(stderr, "Connecting to %s\n", buf);
	do {
		s->sock = inet_connect(buf, SOCK_STREAM);

	} while((errno == 4) && (s->sock == -1));
	
	if(s->sock == -1) {
		fprintf(stderr, "SockConnect() failed: %s = %d\n", strerror(errno), errno);
		return -1;
	}
//	qemu_set_fd_handler(s->sock, peek_client_write, NULL, s);
        /*
        if (connect(s->sock, (const struct sockaddr_in *)&sin, sizeof(struct sockaddr_in)) == -1) {
		fprintf(stderr, "SockConnect() failed: %s = %d\n", strerror(errno), errno);
                close(s->sock);
        	return -1;
        }
	*/
        //ioctl(s->sock, FIONBIO, (unsigned long *)&on);

	socket_set_nonblock(s->sock);

	return 0;
}
int SockWrite(char *buffer, int len, void *opaque) {
        peek_socket_s *s = (peek_socket_s *)opaque;
	int i;
	//fprintf(stderr, "SockWrite() sending data with len %d\n", len);
	//for(i=0;i < len; i++) {	
	//	fprintf(stderr, "Sending Buffer %08X\n", buffer[i]);
	//}
	return send(s->sock, buffer, len, 0);
}

int SockRead(char *buffer, int len, void *opaque) {
        peek_socket_s *s = (peek_socket_s *)opaque;
	//fprintf(stderr, "SockRead() reading data\n");
	return recv(s->sock, buffer, len, 0);
}

uint32_t peek_socket_poll(peek_socket_s *s) {

	int ret;
	struct pollfd ufds;

	if(!s)return 0;
	if(!s->sock)return 0;

	ret = -1;

	ufds.fd = s->sock;
	ufds.events = POLLIN;

	do {
		ret = poll(&ufds, 1, 500);
	//	fprintf(stderr, "SockPoll() loop ret %d/ errno %d\n", ret, errno);
	} while ((errno == 4) && (ret == -1));

  	if(ret <= 0) {
          //      fprintf(stderr, "SockPoll() failed: %s = %d\n", strerror(errno), errno);
		return -1; // timeout or error
	}
	//fprintf(stderr, "SockPoll() Got Data\n");
	if(ufds.revents & POLLIN) {
		return 1;
	}
	return -1;
}

static uint32_t peek_socket_read(void *opaque, target_phys_addr_t offset)
{
    peek_socket_s *s = (peek_socket_s *)opaque;
    uint32_t retBuf = 0;

    LOCO_DEBUG(LOCO_DEBUG_SOCKET, LOCO_DLVL_INFO, "%s: offset=0x%02X\n", __FUNCTION__, (int)offset);

    switch (offset) {
    case 0x0: // Connect return
	return s->sock;
    case 0x4: // Write return val;
	return s->wRet;
    case 0x8: // Poll read
	retBuf = peek_socket_poll(s);
	//fprintf(stderr, "Sock Polling read val=%d\n", retBuf);
	return retBuf;
    case 0xc: // Read data from buffer
        if(s->rTransferSize > 0) {
                //Start transfer
            //    fprintf(stderr, "Reading back: 0x%08X\n", offset);
                memcpy(&retBuf, s->rTransferbuffer+s->rTransferCount, 1);
                if(s->rTransferCount < s->rTransferSize)
                        s->rTransferCount++;
        }
	return retBuf;
    case 0x10: // read return val
	return s->rRet;
    case 0x18: //Connect return val
	return s->cRet;
    case 0x38:
	return s->key;
	case 0x3c:
	return s->state;
    default:
        LOCO_DEBUG(LOCO_DEBUG_SOCKET, LOCO_DLVL_ERR,
                   "%s: UNMAPPED_OFFSET = 0x%08X\n", __FUNCTION__, (int)offset);
    }
    return 0;
}

static void peek_socket_write(void *opaque, target_phys_addr_t offset, uint32_t value)
{
    peek_socket_s *s = (peek_socket_s *)opaque;

    LOCO_DEBUG(LOCO_DEBUG_SOCKET, LOCO_DLVL_INFO, "%s: offset=0x%02X value=0x%02x\n", __FUNCTION__, (int)offset, value);

    switch (offset) {
    case 0x0: // socket fd 
	if(!value)
	   return close(s->sock);
	s->sock = value;
	break;
    case 0x4: // type
	s->type = value;
	break;
    case 0x8: // domain
	s->domain = value;
	break;
    case 0xC: // proto
	s->protocol = value;
        break;
    case 0x10: //socket()
	//s->sock = SockCreate(s->domain, s->type, s->protocol, s);
        //printf(stderr, "SimConnect() returned %d\n", s->sock);
	break;
    case 0x14: // sock_connect trig
	//fprintf(stderr, "Socket connect started\n");
	s->cRet = SockConnect(s->sock, s);
	s->cTransferSize = 0;
	s->cTransferCount = 0;
	free(s->cTransferbuffer);
	break;
    case 0x18: // sock_connect transfer 
	//fprintf(stderr, "Socket transfer count=%d\n", s->cTransferCount);
	if(s->cTransferSize > 0) {
		//Start transfer
		//fprintf(stderr, "got: 0x%08X\n", value);
		memcpy(s->cTransferbuffer+s->cTransferCount, &value, 1);
		if(s->cTransferCount < s->cTransferSize)
			s->cTransferCount++;
	}
	break;
    case 0x1C: // sock transfer size
	//fprintf(stderr, "Socket transfer size %d\n", value);
	s->cTransferSize = value;
	s->cTransferbuffer = malloc(s->cTransferSize);
	s->cTransferCount = 0;
	break;
    case 0x20: // Write transfer 
	if(s->wTransferSize > 0) {
		//Start write buffer transfer
		//fprintf(stderr, "write got: 0x%08X\n", value);
                memcpy(s->wTransferbuffer+s->wTransferCount, &value, 1);
                if(s->wTransferCount < s->wTransferSize)
                        s->wTransferCount++;
	}
	break;
    case 0x24: // Write size
	//fprintf(stderr, "Setting write size %d\n", value);
	s->wTransferSize = value;
	s->wTransferbuffer = malloc(s->wTransferSize);
	s->wTransferCount = 0;
	break;
    case 0x28: // Write trig
	s->wRet = SockWrite(s->wTransferbuffer, s->wTransferSize, s);
	s->wTransferCount = 0;
	s->wTransferSize = 0;
	free(s->wTransferbuffer);
	break;
    case 0x2C: // read trig
	//fprintf(stderr, "Socket read started\n");
	s->rRet = SockRead(s->rTransferbuffer, s->rTransferSize, s);
	//fprintf(stderr, "Socket read %d\n", s->rRet);
//	if (s->rRet == -1 && errno != EWOULDBLOCK)
	s->rTransferSize = s->rRet;
	s->rTransferCount = 0;
	break;
    case 0x30: // read transfer reset 
	free(s->rTransferbuffer);
	s->rTransferCount = 0;
	s->rTransferCount = 0;
	//fprintf(stderr, "read transfer reset\n");
	break;
    case 0x34: // read transfer size
	//fprintf(stderr, "Socket read size %d\n", value);
	s->rTransferSize = value;
	s->rTransferbuffer = malloc(s->rTransferSize);
	s->rTransferCount = 0;
	break;
    case 0x38:
	s->key = value;
	break;
	case 0x3c:
	s->state = value;
	break;

    default:
        LOCO_DEBUG(LOCO_DEBUG_SOCKET, LOCO_DLVL_ERR, "%s: UNMAPPED_OFFSET = 0x%08X and value=0x%08X\n", __FUNCTION__, (int)offset, value);
    }
}

#ifdef _WIN32
int poll(struct pollfd *fds, unsigned int nfds, int timo)
{
    struct timeval timeout, *toptr;
    fd_set ifds, ofds, efds, *ip, *op;
    int i, rc;

    /* Set up the file-descriptor sets in ifds, ofds and efds. */
    FD_ZERO(&ifds);
    FD_ZERO(&ofds);
    FD_ZERO(&efds);
    for (i = 0, op = ip = 0; i < nfds; ++i) {
	fds[i].revents = 0;
	if(fds[i].events & (POLLIN|POLLPRI)) {
		ip = &ifds;
		FD_SET(fds[i].fd, ip);
	}
	if(fds[i].events & POLLOUT) {
		op = &ofds;
		FD_SET(fds[i].fd, op);
	}
	FD_SET(fds[i].fd, &efds);
    } 

    /* Set up the timeval structure for the timeout parameter */
    if(timo < 0) {
	toptr = 0;
    } else {
	toptr = &timeout;
	timeout.tv_sec = timo / 1000;
	timeout.tv_usec = (timo - timeout.tv_sec * 1000) * 1000;
    }

#ifdef DEBUG_POLL
    printf("Entering select() sec=%ld usec=%ld ip=%lx op=%lx\n",
           (long)timeout.tv_sec, (long)timeout.tv_usec, (long)ip, (long)op);
#endif
    rc = select(0, ip, op, &efds, toptr);
#ifdef DEBUG_POLL
    printf("Exiting select rc=%d\n", rc);
#endif

    if(rc <= 0)
	return rc;

    if(rc > 0) {
        for (i = 0; i < nfds; ++i) {
            int fd = fds[i].fd;
    	if(fds[i].events & (POLLIN|POLLPRI) && FD_ISSET(fd, &ifds))
    		fds[i].revents |= POLLIN;
    	if(fds[i].events & POLLOUT && FD_ISSET(fd, &ofds))
    		fds[i].revents |= POLLOUT;
    	if(FD_ISSET(fd, &efds))
    		/* Some error was detected ... should be some way to know. */
    		fds[i].revents |= POLLHUP;
#ifdef DEBUG_POLL
        printf("%d %d %d revent = %x\n", 
                FD_ISSET(fd, &ifds), FD_ISSET(fd, &ofds), FD_ISSET(fd, &efds), 
                fds[i].revents
        );
#endif
        }
    }
    return rc;
}
#endif

static CPUReadMemoryFunc *peek_socket_readfn[] = {
    peek_socket_read,
    peek_socket_read,
    peek_socket_read,
};

static CPUWriteMemoryFunc *peek_socket_writefn[] = {
    peek_socket_write,
    peek_socket_write,
    peek_socket_write,
};

#define KEY_UNKNOWN 0
#define KCD_0          (1)
#define KCD_1          (2)
#define KCD_2          (3)
#define KCD_3          (4)
#define KCD_4          (5)
#define KCD_5          (6)
#define KCD_6          (7)
#define KCD_7          (8)
#define KCD_8          (9)
#define KCD_9          (10)
#define KCD_A          (11)
#define KCD_B          (12)
#define KCD_C          (13)
#define KCD_D          (14)
#define KCD_E          (15)
#define KCD_F          (16)
#define KCD_G          (17)
#define KCD_H          (18)
#define KCD_I          (19)
#define KCD_J          (20)
#define KCD_K          (21)
#define KCD_L          (22)
#define KCD_M          (23)
#define KCD_N          (24)
#define KCD_O          (25)
#define KCD_P          (26)

/* KCD_MUX = 1 */
#define KCD_Q          (27)
#define KCD_R          (28)
#define KCD_S          (29)
#define KCD_T          (30)
#define KCD_U          (31)
#define KCD_V          (32)
#define KCD_W          (33)
#define KCD_X          (34)
#define KCD_Y          (35)
#define KCD_Z          (36)
#define KCD_AT         (37)
#define KCD_SPACE      (38)
#define KCD_SHIFT_L    (39)
#define KCD_SHIFT_R    (40)
#define KCD_ENTER      (41)
#define KCD_LOCK       (42)
#define KCD_REDUCE     (43)
#define KCD_DOT        (44)
#define KCD_COMMA      (45)
#define KCD_QUOTE      (46)
#define KCD_NAV_CENTER (47)
#define KCD_CANCLE     (48)
#define KCD_BACKSPACE  (49)
#define KCD_POWR       (50)
#define KCD_UP         (51)
#define KCD_DOWN       (52)

static int linux_key_map[] = {
	KEY_UNKNOWN,
	KCD_CANCLE,
	KCD_1,
	KCD_2,
	KCD_3,
	KCD_4,
	KCD_5,
	KCD_6,
	KCD_7,
	KCD_8,
	KCD_9,
	KEY_UNKNOWN,
	KCD_REDUCE,
	KEY_UNKNOWN,
	KCD_BACKSPACE,
	KCD_LOCK, /* tab */
	KCD_Q,
	KCD_W,
	KCD_E,
	KCD_R,
	KCD_T,
	KCD_Y,
	KCD_U,
	KCD_I,
	KCD_O,
	KCD_P,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KCD_ENTER,
	KEY_UNKNOWN,
	KCD_A,
	KCD_S,
	KCD_D,
	KCD_F,
	KCD_G,
	KCD_H,
	KCD_J,
	KCD_K,
	KCD_L,
	KEY_UNKNOWN,
	KCD_QUOTE,
	KEY_UNKNOWN,
	KCD_SHIFT_L,
	KEY_UNKNOWN,
	KCD_Z,
	KCD_X,
	KCD_C,
	KCD_V,
	KCD_B,
	KCD_N,
	KCD_M,
	KCD_COMMA,
	KCD_DOT,
	KCD_6,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KCD_SPACE,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KCD_UP,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KCD_UP,
	KEY_UNKNOWN,
	KCD_DOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KCD_DOWN // slash
};
#if 0	
static int linux_keys[0x80] = {
    [0x01] = 16,    /* Q */
    [0x02] = 37,    /* K */
    [0x03] = 24,    /* O */
    [0x04] = 25,    /* P */
    [0x05] = 14,    /* Backspace */
    [0x06] = 30,    /* A */
    [0x07] = 31,    /* S */
    [0x08] = 32,    /* D */
    [0x09] = 33,    /* F */
    [0x0a] = 34,    /* G */
    [0x0b] = 35,    /* H */
    [0x0c] = 36,    /* J */

    [0x11] = 17,    /* W */
    [0x12] = 62,    /* Menu (F4) */
    [0x13] = 38,    /* L */
    [0x14] = 40,    /* ' (Apostrophe) */
    [0x16] = 44,    /* Z */
    [0x17] = 45,    /* X */
    [0x18] = 46,    /* C */
    [0x19] = 47,    /* V */
    [0x1a] = 48,    /* B */
    [0x1b] = 49,    /* N */
    [0x1c] = 42,    /* Shift (Left shift) */
    [0x1f] = 65,    /* Zoom+ (F7) */

    [0x21] = 18,    /* E */
    [0x22] = 39,    /* ; (Semicolon) */
    [0x23] = 12,    /* - (Minus) */
    [0x24] = 13,    /* = (Equal) */
    [0x2b] = 56,    /* Fn (Left Alt) */
    [0x2c] = 50,    /* M */
    [0x2f] = 66,    /* Zoom- (F8) */

    [0x31] = 19,    /* R */
    [0x32] = 29,    /* Right Ctrl */
    [0x34] = 57,    /* Space */
    [0x35] = 51,    /* , (Comma) */
    [0x37] = 72,    /* Up */
    [0x3c] = 82,    /* Compose (Insert) */
    [0x3f] = 64,    /* FullScreen (F6) */

    [0x41] = 20,    /* T */
    [0x44] = 52,    /* . (Dot) */
    [0x46] = 77,    /* Right */
    [0x4f] = 63,    /* Home (F5) */
    [0x51] = 21,    /* Y */
    [0x53] = 80,    /* Down */
    [0x55] = 28,    /* Enter */
    [0x5f] =  1,    /* Cycle (ESC) */

    [0x61] = 22,    /* U */
    [0x64] = 75,    /* Left */

    [0x71] = 23,    /* I */
    [0x75] = 15,    /* KP Enter (Tab) */
};
#endif
static void locosto_keyboard_event (peek_socket_s *s, int keycode)
{
	int down = 1;
	int extended = 0;

    if (keycode == 0xe0) {
		extended = 1;
   		return;
    } else if (keycode & 0x80) {
    	keycode &= 0x7f;
    	down = 0;
    }
    if (extended) {
    	keycode |= 0x80;
    	extended = 0;
    }
    fprintf(stderr, "Got keypad event %d - orig %d - state %d\n",
linux_key_map[keycode], keycode, down);
    s->key=linux_key_map[keycode];
	s->state = down;
}

void peek_socket_init(peek_socket_s *s)
{
    int io;
    io = cpu_register_io_memory(peek_socket_readfn, peek_socket_writefn, s);
    cpu_register_physical_memory(0xFFFE9800, 0x7FF, io);

    s->key=0;
    qemu_add_kbd_event_handler((QEMUPutKBDEvent *) locosto_keyboard_event, s);
}

