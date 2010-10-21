TARGET = purple-server

INCLUDE  = ./ 
INCLUDE += ./include
INCLUDE += $(SRCDIR)/soaprpc
INCLUDE += $(SRCDIR)/utils/include
INCLUDE += $(GSOAPINC)
INCLUDE += $(subst -I,,$(shell pkg-config --cflags purple))

DEFINE  = WITH_GZIP
DEFINE += WITH_NOCDATA
DEFINE += WITH_OPENSSL

OBJ = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

LIB  = $(GSOAPLIB)/libgsoapssl++.a -lssl -lcrypto -lz
LIB += /usr/lib/libcurl.a -lidn
LIB += $(SRCDIR)/soaprpc/soaprpc.a
LIB += $(SRCDIR)/soaprpc/soapserve.a
LIB += $(SRCDIR)/utils/utils.a -lxerces-c
LIB += $(shell pkg-config --libs purple)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $(OBJ) $(LIB)

clean:
	$(RM) $(TARGET) $(OBJ)

include $(BASEDIR)/Makefile.rules
