DEFINE  = EPERM=1
DEFINE += EINVAL=22
DEFINE += ENOMEM=12

# XXX This is a hack! Hopefully we dont use xer/per encoding
DEFINE += alloca=p_malloc
DEFINE += malloc=p_malloc
DEFINE += calloc=p_calloc
DEFINE += realloc=p_realloc
DEFINE += free=p_free

# Debug
DEFINE += EMIT_ASN_DEBUG=1

FILTER = converter-sample.c
SRC = $(filter-out $(FILTER),$(wildcard *.c))
OBJ = $(patsubst %.c,%.o,$(SRC))

INCLUDE   = .
INCLUDE += $(SRCDIR)/include
INCLUDE += $(M23DIR)/chipsetsw/riviera/rvf
INCLUDE += $(M23DIR)/chipsetsw/riviera/rvm
INCLUDE += $(M23DIR)/csw-system/os/nucleus/1_15/plus
INCLUDE += $(M23DIR)/csw-system/os/nucleus/1_15
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/cdginc


.PHONY: prepare

TARGET = protocol.a
PROTOCOL = fri.asn1

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

prepare: 
	$(ASN1C) -R -gen-PER -fnative-types $(PROTOCOL)

clean:
	$(RM) $(TARGET) *.o *.d *.pp *.pp_ Makefile.am.sample *P.h *P.c

include $(BASEDIR)/Makefile.rules
