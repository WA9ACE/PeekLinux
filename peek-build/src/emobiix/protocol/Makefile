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
# DEFINE += EMIT_ASN_DEBUG=1

FILTER = converter-sample.c
SRC = $(filter-out $(FILTER),$(wildcard *.c))
OBJ = $(patsubst %.c,%.o,$(SRC))

INCLUDE   = .
INCLUDE  += $(SRCDIR)/include

.PHONY: prepare

TARGET = protocol.a
PROTOCOL = fri.asn1

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

prepare: 
	$(ASN1C) -R -gen-PER -fnative-types $(PROTOCOL)

clean:
	$(RM) $(TARGET) *.o *.d *.pp Makefile.am.sample *P.h *P.c

include $(BASEDIR)/Makefile.rules
