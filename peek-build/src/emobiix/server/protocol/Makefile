TARGET = protocol.a
PROTOCOL = ../../protocol/fri.asn1

OBJ = $(patsubst %.c,%.o,$(filter-out converter-sample.c,$(wildcard *.c)))

INCLUDE  = .

.PHONY: prepare

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ); $(RANLIB) $@

prepare: 
	$(ASN1C) -gen-PER -fnative-types $(PROTOCOL)

clean:
	$(RM) $(TARGET) *.o *.d Makefile.am.sample *.c *.h 

include $(BASEDIR)/Makefile.rules
