DEFINE  = HAVE_RECV
#DEFINE  += fprintf=emo_fprintf
#DEFINE += RECV_TYPE_ARG1
#DEFINE += RECV_TYPE_ARG2
#DEFINE += RECV_TYPE_ARG3
#DEFINE += RECV_TYPE_ARG4
#DEFINE += RECV_TYPE_RETV
#DEFINE += HAVE_SEND
#DEFINE += SEND_TYPE_ARG1
#DEFINE += SEND_QUAL_ARG2
#DEFINE += SEND_TYPE_ARG2
#DEFINE += SEND_TYPE_ARG3
#DEFINE += SEND_TYPE_ARG4
#DEFINE += SEND_TYPE_RETV
DEFINE += HAVE_CONFIG_H
DEFINE += HAVE_STDBOOL_H
DEFINE += HAVE_BOOL_T
DEFINE += NG_TIMEVAL
DEFINE += Error=\#error
DEFINE += ssize_t=int

VARIANT_DEFINE :=

INCLUDE  = $(SRCDIR)/netsurf/emoport
INCLUDE += $(SRCDIR)/emobiix
INCLUDE += $(SRCDIR)/netdepends/curl-7.21.1/include
INCLUDE += $(SRCDIR)/netdepends/curl-7.21.1/emoport
INCLUDE += $(M23DIR)/g23m/nexgen/coreip
INCLUDE += $(SRCDIR)/zlib
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/cdginc

SRC  = lib/base64.c
SRC += lib/connect.c
SRC += lib/cookie.c
SRC += lib/dict.c
SRC += lib/easy.c
SRC += lib/escape.c
SRC += lib/file.c
SRC += lib/formdata.c
SRC += lib/getenv.c
SRC += lib/getinfo.c
SRC += lib/hostip.c
SRC += lib/hostip4.c
SRC += lib/hash.c
SRC += lib/multi.c
SRC += lib/hostsyn.c
SRC += lib/http.c
SRC += lib/http_chunks.c
SRC += lib/http_digest.c
SRC += lib/inet_ntop.c
SRC += lib/inet_pton.c
SRC += lib/if2ip.c
SRC += lib/curl_fnmatch.c
SRC += lib/fileinfo.c
SRC += lib/wildcard.c
SRC += lib/memdebug.c
SRC += lib/mprintf.c
SRC += lib/netrc.c
SRC += lib/progress.c
SRC += lib/security.c
SRC += lib/select.c
SRC += lib/md5.c
SRC += lib/share.c
SRC += lib/splay.c
SRC += lib/parsedate.c
SRC += lib/llist.c
SRC += lib/content_encoding.c
SRC += lib/sendf.c
SRC += lib/speedcheck.c
SRC += lib/strequal.c
SRC += lib/strerror.c
SRC += lib/strdup.c
SRC += lib/curl_rand.c
SRC += lib/strtok.c
SRC += lib/timeval.c
SRC += lib/transfer.c
SRC += lib/url.c
SRC += lib/version.c
SRC += lib/strtoofft.c
SRC += lib/sslgen.c
SRC += lib/rawstr.c
SRC += lib/curl_addrinfo.c
SRC += lib/slist.c
SRC += lib/nonblock.c
SRC += lib/curl_memrchr.c
SRC += lib/pingpong.c
SRC += lib/curl_threads.c
SRC += lib/warnless.c
SRC += lib/curl_gethostname.c
SRC += emoport/socket.c
SRC += emoport/sim-socket.c


DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libcurl.a

all: $(TARGET)
OBJ  = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

$(TARGET): $(OBJ)
	$(subst /cygdrive/c,C:,$(AR) $(ARFLAGS) $@ $?)

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules

