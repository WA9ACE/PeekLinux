include Makefile.globals

TARGET=server
SUBDIR=protocol application utils soaprpc 

INCLUDE  = protocol
INCLUDE += application/include
INCLUDE += $(BOOSTINC)

DEFINE  = WITH_OPENSSL
DEFINE += WITH_GZIP

OBJ = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

LIB  = application/application.a
LIB += protocol/protocol.a
LIB += utils/utils.a
LIB += soaprpc/soaprpc.a
LIB += $(GSOAPLIB)/libgsoapssl++.a -lssl -lcrypto -lz
LIB += $(BOOSTLIB)/libboost_system.a
LIB += $(BOOSTLIB)/libboost_thread.a -lpthread
LIB += -lxerces-c

.PHONY: $(SUBDIR)

all: server

server: $(SUBDIR) $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIB) 

protocol soaprpc:
	$(MAKE) -C $@ prepare
	$(MAKE) -C $@

application utils:
	$(MAKE) -C $@

clean: 
	$(MAKE) -C protocol clean
	$(MAKE) -C application clean
	$(MAKE) -C utils clean
	$(MAKE) -C soaprpc clean
	$(RM) $(TARGET) *.o *.d

include $(BASEDIR)/Makefile.rules
